/*==============================================================================================*/
/* Project      = UDS Bootloader for uPD78F1827                                                 */
/* Module       = App_Can_Tp.c                                                                  */
/* Description  = CAN Transport Protocol (ISO 15765-2), single shared buffer, no heap           */
/*                                                                                              */
/* Context rules (important for the RAM-saving design):                                         */
/*   - CANTP_ProcessRxFrame()  : CAN RX ISR. Only copies data and sets flags. Never transmits.  */
/*   - everything else         : main context (1 ms task / UDS task).                           */
/*   - Variables written by the ISR are never written by main in the same phase (ownership via  */
/*     sState); the few exceptions are protected with CANTP_LOCK()/CANTP_UNLOCK().              */
/*   - CANTP_LOCK() is DI()/EI(): use from main context only, never nested, never from an ISR.  */
/*==============================================================================================*/

#include <CG_macrodriver.h>
#include "system_def.h"
#include "HAL_Init.h"

#include "App_Can_Tp.h"
#include "App_Can.h"
#include <string.h>

/*----------------------------------------------------------------------------------------------*/
/* PCI types                                                                                    */
/*----------------------------------------------------------------------------------------------*/
#define PCI_SF              0x00u
#define PCI_FF              0x10u
#define PCI_CF              0x20u
#define PCI_FC              0x30u

#define FC_CTS              0x00u
#define FC_WAIT             0x01u
#define FC_OVFLW            0x02u

/* sFcReq values (ISR -> main) */
#define FCREQ_NONE          0u
#define FCREQ_CTS           1u
#define FCREQ_OVFLW         2u

/*----------------------------------------------------------------------------------------------*/
/* States                                                                                       */
/*----------------------------------------------------------------------------------------------*/
#define ST_IDLE             0u   /* buffer free                                                 */
#define ST_RX_WAIT_CF       1u   /* ISR owns buffer, collecting consecutive frames              */
#define ST_RX_READY         2u   /* complete request, waiting for UDS                           */
#define ST_RX_PROC          3u   /* UDS owns buffer                                             */
#define ST_TX_SF            4u   /* single frame built, waiting for CAN FIFO space              */
#define ST_TX_FF            5u   /* first frame pending                                         */
#define ST_TX_WAIT_FC       6u   /* waiting for flow control from tester                        */
#define ST_TX_CF            7u   /* sending consecutive frames                                  */

#if (USE_INTERRUPTS == 1) 
  #define CANTP_LOCK()        DI()
  #define CANTP_UNLOCK()      EI()
#else
  #define CANTP_LOCK()        
  #define CANTP_UNLOCK()      
#endif
/*----------------------------------------------------------------------------------------------*/
/* Data  (total: CANTP_BUF_SIZE + CANTP_BUF_PAD + 25 bytes)                                     */
/*----------------------------------------------------------------------------------------------*/
volatile u8           gBuf[CANTP_BUF_SIZE + CANTP_BUF_PAD];   /* the one and only message buffer  */

static volatile u8  sState;      /* see ST_xxx                                                  */
static u16          sLen;        /* total message length (rx or tx)                             */
static u16          sPos;        /* bytes received (ISR) / sent (main)                          */
static u8           sSn;         /* next expected (rx) / next to send (tx) sequence number      */
static u8           sBlk;        /* rx: CFs since last FC / tx: CFs sent in current block       */
static u16          sTimer;      /* ms timeout, N_Cr (rx) or N_Bs/N_As (tx)                     */
static volatile u8  sRxAct;      /* ISR -> main: CF/FF seen, reload sTimer                      */
static volatile u8  sFcReq;      /* ISR -> main: flow control frame to send                     */
static volatile u8  sFcEvt;      /* ISR -> main: flow control from tester received              */
static volatile u8  sFcSts;      /*   ... its status                                            */
static volatile u8  sFcBs;       /*   ... its block size                                        */
static volatile u8  sFcSt;       /*   ... its STmin (raw)                                       */
static u8           sTxBs;       /* block size requested by tester                              */
static u8           sTxStmin;    /* STmin in ms                                                 */
static u8           sStminCnt;   /* ms counter until next CF is allowed                         */
static u8           sWft;        /* number of FC.WAIT received                                  */
static u8           sTxMsgNo;    /* EAL tx message number of the response ID                    */
static u8           sDrop;       /* dropped frame counter (debug)                               */

/*----------------------------------------------------------------------------------------------*/
/* Local prototypes                                                                             */
/*----------------------------------------------------------------------------------------------*/
static u8   CANTP_TxFrame(u8 *pFrame);
static void CANTP_FcService(void);
static void CANTP_TxService(void);
static u8   CANTP_StminToMs(u8 raw);

/*==============================================================================================*/
/* Function: CANTP_Init                                                                         */
/*==============================================================================================*/
void CANTP_Init(void)
{
    sState    = ST_IDLE;
    sLen      = 0u;
    sPos      = 0u;
    sSn       = 0u;
    sBlk      = 0u;
    sTimer    = 0u;
    sRxAct    = 0u;
    sFcReq    = FCREQ_NONE;
    sFcEvt    = 0u;
    sFcSts    = 0u;
    sFcBs     = 0u;
    sFcSt     = 0u;
    sTxBs     = 0u;
    sTxStmin  = 0u;
    sStminCnt = 0u;
    sWft      = 0u;
    sDrop     = 0u;
    sTxMsgNo  = App_Get_Tx_Msg_No(CAN_TX_ID);
}

/*==============================================================================================*/
/* Function: CANTP_ProcessRxFrame                                                               */
/* Context : CAN RX ISR. Copy + flags only, takes a few microseconds, never calls the EAL TX.   */
/*==============================================================================================*/
void CANTP_ProcessRxFrame(const u8 *pData, u8 length)
{
    u8  pci;
    u8  st;
    u8  n;
    u16 total;

    if (length == 0u)
    {
        return;
    }

    pci = (u8)(pData[0] & 0xF0u);
    st  = sState;

    /*------------------------------------------------------------------------------------------*/
    /* Flow control: only latch it, the TX state machine (main) evaluates it                    */
    /*------------------------------------------------------------------------------------------*/
    if (pci == PCI_FC)
    {
        if (length >= 3u)
        {
            sFcSts = (u8)(pData[0] & 0x0Fu);
            sFcBs  = pData[1];
            sFcSt  = pData[2];
            sFcEvt = 1u;                 /* must be written last */
        }
        return;
    }

    /*------------------------------------------------------------------------------------------*/
    /* Consecutive frame                                                                        */
    /*------------------------------------------------------------------------------------------*/
    if (pci == PCI_CF)
    {
        if (st != ST_RX_WAIT_CF)
        {
            return;                      /* not expected, ISO: ignore */
        }

        if ((u8)(pData[0] & 0x0Fu) != sSn)
        {
            sState = ST_IDLE;            /* wrong sequence number: abort reception */
            return;
        }

        total = (u16)(sLen - sPos);      /* bytes still missing */
        n = (total > 7u) ? 7u : (u8)total;

        if (length < (u8)(n + 1u))
        {
            sState = ST_IDLE;            /* short CF: malformed, abort */
            return;
        }

        memcpy(&gBuf[sPos], &pData[1], n);
        sPos = (u16)(sPos + n);
        sSn  = (u8)((sSn + 1u) & 0x0Fu);
        sRxAct = 1u;

        if (sPos >= sLen)
        {
            sState = ST_RX_READY;        /* hand over to UDS */
        }
#if (CANTP_RX_BS != 0u)
        else
        {
            sBlk++;
            if (sBlk >= CANTP_RX_BS)
            {
                sBlk = 0u;
                sFcReq = FCREQ_CTS;      /* next block */
            }
        }
#endif
        return;
    }

    /*------------------------------------------------------------------------------------------*/
    /* SF / FF start a new message. Accepted only when the buffer is free or still being filled */
    /* (a new SF/FF aborts an unfinished reception, ISO 15765-2 N_UNEXP_PDU handling).          */
    /*------------------------------------------------------------------------------------------*/
    if ((pci == PCI_SF) || (pci == PCI_FF))
    {
        if ((st != ST_IDLE) && (st != ST_RX_WAIT_CF))
        {
            sDrop++;                     /* buffer owned by UDS / TX engine */
            return;
        }

        if (pci == PCI_SF)
        {
            n = (u8)(pData[0] & 0x0Fu);
            if ((n == 0u) || (n > 7u) || (n >= length))
            {
                return;                  /* invalid SF_DL */
            }
            memcpy(gBuf, &pData[1], n);
            sLen   = n;
            sPos   = n;
            sState = ST_RX_READY;
        }
        else
        {
            if (length < 8u)
            {
                return;                  /* FF always has DLC 8 */
            }
            total = (u16)(((u16)(pData[0] & 0x0Fu) << 8) | pData[1]);
            if (total < 8u)
            {
                return;                  /* would have fitted a SF: invalid */
            }
            if (total > CANTP_BUF_SIZE)
            {
                sState = ST_IDLE;
                sFcReq = FCREQ_OVFLW;    /* tell the tester it does not fit */
                return;
            }
            memcpy(gBuf, &pData[2], 6);
            sLen   = total;
            sPos   = 6u;
            sSn    = 1u;
            sBlk   = 0u;
            sRxAct = 1u;
            sFcReq = FCREQ_CTS;
            sState = ST_RX_WAIT_CF;      /* must be written last */
        }
    }
}

/*==============================================================================================*/
/* Function: CANTP_MainFunction (1 ms)                                                          */
/*==============================================================================================*/
void CANTP_MainFunction(void)
{

    /* 1. flow control frame requested by the ISR */
    CANTP_FcService();

    /* 2. receive timeout (N_Cr) */
    if (sState == ST_RX_WAIT_CF)
    {
        if (sRxAct != 0u)
        {
            sRxAct = 0u;
            sTimer = CANTP_TIMEOUT_MS;
        }
        else if (sTimer != 0u)
        {
            sTimer--;
            if (sTimer == 0u)
            {
                CANTP_LOCK();
                if ((sState == ST_RX_WAIT_CF) && (sRxAct == 0u))
                {
                    sState = ST_IDLE;    /* tester stopped sending CFs: free the buffer */
                }
                CANTP_UNLOCK();
            }
        }
        else
        {
            sTimer = CANTP_TIMEOUT_MS;
        }
    }

    /* 3. transmit engine */
    CANTP_TxService();
}

/*==============================================================================================*/
/* Function: CANTP_FcService - send the flow control frame the ISR asked for                    */
/*==============================================================================================*/
static void CANTP_FcService(void)
{
    u8 req;
    u8 f[8];

    req = sFcReq;
    if (req == FCREQ_NONE)
    {
        return;
    }

    if ((req == FCREQ_CTS) && (sState != ST_RX_WAIT_CF))
    {
        /* reception was aborted in the meantime, FC is obsolete */
        CANTP_LOCK();
        if (sFcReq == req)
        {
            sFcReq = FCREQ_NONE;
        }
        CANTP_UNLOCK();
        return;
    }

    f[0] = (u8)(PCI_FC | ((req == FCREQ_OVFLW) ? FC_OVFLW : FC_CTS));
    f[1] = (u8)CANTP_RX_BS;
    f[2] = (u8)CANTP_RX_STMIN_MS;
    memset(&f[3], CANTP_PAD_BYTE, 5);

    if (CANTP_TxFrame(f) != 0u)
    {
        CANTP_LOCK();
        if (sFcReq == req)               /* ISR may have requested a newer one meanwhile */
        {
            sFcReq = FCREQ_NONE;
        }
        CANTP_UNLOCK();
    }
    /* else: CAN FIFO full, retried at the next tick (N_Cr timer protects against deadlock) */
}

/*==============================================================================================*/
/* Function: CANTP_StminToMs - ISO 15765-2 STmin decoding                                       */
/*==============================================================================================*/
static u8 CANTP_StminToMs(u8 raw)
{
    if (raw <= 0x7Fu)
    {
        return raw;
    }
    if ((raw >= 0xF1u) && (raw <= 0xF9u))
    {
        return 1u;                       /* 100..900 us: tick is 1 ms, round up */
    }
    return 0x7Fu;                        /* reserved: use the maximum */
}

/*==============================================================================================*/
/* Function: CANTP_TxFrame - hand one 8 byte frame to the EAL FIFO                              */
/* Returns 1 when accepted, 0 when the FIFO is full.                                            */
/* DI/EI because the EAL FIFO counters are also modified by the CAN TX-complete interrupt.      */
/*==============================================================================================*/
static u8 CANTP_TxFrame(u8 *pFrame)
{
    u8 ok;

    CANTP_LOCK();
    ok = EAL_CAN_Tx_Msg(sTxMsgNo, pFrame);
    CANTP_UNLOCK();

    return (ok != 0u) ? 1u : 0u;
}

/*==============================================================================================*/
/* Function: CANTP_TxService - SF/FF/CF state machine (main context only)                       */
/*==============================================================================================*/
static void CANTP_TxService(void)
{
    u8 f[8];
    u8 n;
    u8 st;

    st = sState;

    /*------------------------------------------------------------------------------------------*/
    if (st == ST_TX_SF)
    {
        f[0] = (u8)sLen;                                    /* PCI SF + length */
        memcpy(&f[1], gBuf, (u8)sLen);
        memset(&f[1 + (u8)sLen], CANTP_PAD_BYTE, (u8)(7u - (u8)sLen));

        if (CANTP_TxFrame(f) != 0u)
        {
            sState = ST_IDLE;
        }
        else if (sTimer != 0u)
        {
            sTimer--;
            if (sTimer == 0u)
            {
                sState = ST_IDLE;                           /* N_As timeout: give up */
            }
        }
        else
        {
            sState = ST_IDLE;
        }
        return;
    }

    /*------------------------------------------------------------------------------------------*/
    if (st == ST_TX_FF)
    {
        f[0] = (u8)(PCI_FF | (u8)((sLen >> 8) & 0x0Fu));
        f[1] = (u8)(sLen & 0xFFu);
        memcpy(&f[2], gBuf, 6);

        sFcEvt = 0u;                    /* drop stale FC before the frame leaves */
        if (CANTP_TxFrame(f) != 0u)
        {
            sPos   = 6u;
            sSn    = 1u;
            sWft   = 0u;
            sTimer = CANTP_TIMEOUT_MS;
            sState = ST_TX_WAIT_FC;
        }
        else if (sTimer != 0u)
        {
            sTimer--;
            if (sTimer == 0u)
            {
                sState = ST_IDLE;
            }
        }
        else
        {
            sState = ST_IDLE;
        }
        return;
    }

    /*------------------------------------------------------------------------------------------*/
    if (st == ST_TX_WAIT_FC)
    {
        if (sFcEvt != 0u)
        {
            sFcEvt = 0u;
            n = sFcSts;

            if (n == FC_CTS)
            {
                sTxBs     = sFcBs;
                sTxStmin  = CANTP_StminToMs(sFcSt);
                sBlk      = 0u;
                sStminCnt = 0u;
                sTimer    = CANTP_TIMEOUT_MS;
                sState    = ST_TX_CF;
                st        = ST_TX_CF;                       /* continue below in the same tick */
            }
            else if (n == FC_WAIT)
            {
                sWft++;
                if (sWft > CANTP_WFT_MAX)
                {
                    sState = ST_IDLE;                       /* tester keeps us waiting: abort */
                }
                else
                {
                    sTimer = CANTP_TIMEOUT_MS;
                }
                return;
            }
            else
            {
                sState = ST_IDLE;                           /* OVFLW or invalid: abort */
                return;
            }
        }
        else
        {
            if (sTimer != 0u)
            {
                sTimer--;
            }
            if (sTimer == 0u)
            {
                sState = ST_IDLE;                           /* N_Bs timeout */
            }
            return;
        }
    }

    /*------------------------------------------------------------------------------------------*/
    if (st == ST_TX_CF)
    {
        if (sStminCnt != 0u)
        {
            sStminCnt--;
        }

        while (sStminCnt == 0u)
        {
            n = (u8)(sLen - sPos);
            if (n > 7u)
            {
                n = 7u;
            }

            f[0] = (u8)(PCI_CF | sSn);
            memcpy(&f[1], &gBuf[sPos], n);
            if (n < 7u)
            {
                memset(&f[1 + n], CANTP_PAD_BYTE, (u8)(7u - n));
            }

            sFcEvt = 0u;                /* any FC seen up to now belongs to the previous block */
            if (CANTP_TxFrame(f) == 0u)
            {
                /* CAN FIFO full: retry next tick, N_As supervision */
                if (sTimer != 0u)
                {
                    sTimer--;
                }
                if (sTimer == 0u)
                {
                    sState = ST_IDLE;
                }
                return;
            }

            sPos = (u16)(sPos + n);
            sSn  = (u8)((sSn + 1u) & 0x0Fu);
            sBlk++;
            sTimer = CANTP_TIMEOUT_MS;

            if (sPos >= sLen)
            {
                sState = ST_IDLE;       /* last frame queued: buffer is free again */
                return;
            }

            if ((sTxBs != 0u) && (sBlk >= sTxBs))
            {
                sBlk   = 0u;
                sState = ST_TX_WAIT_FC; /* block complete: wait for next flow control */
                return;
            }

            sStminCnt = sTxStmin;       /* 0: keep sending while the CAN FIFO accepts */
        }
    }
}

/*==============================================================================================*/
/* Function: CANTP_GetRequest                                                                   */
/*==============================================================================================*/
CANTP_Result_t CANTP_GetRequest(u8 **ppMsg, u16 *pLength)
{
    if (sState == ST_RX_READY)
    {
        sState = ST_RX_PROC;             /* ISR does not touch the buffer in RX_READY/RX_PROC */
        *ppMsg   = gBuf;
        *pLength = sLen;
        return CANTP_OK;
    }
    return CANTP_NO_DATA;
}

/*==============================================================================================*/
/* Function: CANTP_Transmit - response is already in the shared buffer                          */
/*==============================================================================================*/
CANTP_Result_t CANTP_Transmit(u16 length)
{
    if (sState != ST_RX_PROC)
    {
        return CANTP_BUSY;
    }
    if ((length == 0u) || (length > CANTP_BUF_SIZE))
    {
        return CANTP_ERROR;              /* caller still owns the buffer */
    }

    sLen   = length;
    sPos   = 0u;
    sTimer = CANTP_TIMEOUT_MS;
    sState = (length <= 7u) ? ST_TX_SF : ST_TX_FF;

    CANTP_TxService();                   /* try immediately, retries happen in MainFunction */
    return CANTP_OK;
}

/*==============================================================================================*/
/* Function: CANTP_ReleaseRequest                                                               */
/*==============================================================================================*/
void CANTP_ReleaseRequest(void)
{
    if (sState == ST_RX_PROC)            /* no-op if a response has been handed over */
    {
        sState = ST_IDLE;
    }
}

/*==============================================================================================*/
/* Function: CANTP_SendImmediate - SF that does not use the shared buffer (7F xx 78 etc.)       */
/*==============================================================================================*/
CANTP_Result_t CANTP_SendImmediate(const u8 *pData, u8 length)
{
    u8 f[8];

    if (sState != ST_RX_PROC)
    {
        return CANTP_BUSY;
    }
    if ((length == 0u) || (length > 7u))
    {
        return CANTP_ERROR;
    }

    f[0] = length;
    memcpy(&f[1], pData, length);
    memset(&f[1 + length], CANTP_PAD_BYTE, (u8)(7u - length));

    return (CANTP_TxFrame(f) != 0u) ? CANTP_OK : CANTP_BUSY;
}

/*==============================================================================================*/
u8 CANTP_IsIdle(void)
{
    return (sState == ST_IDLE) ? 1u : 0u;
}

u8 CANTP_GetDropCount(void)
{
    return sDrop;
}
