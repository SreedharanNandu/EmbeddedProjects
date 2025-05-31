#ifndef OPCRTL_H
#define OPCRTL_H


#define RCO_GROUP_RCMETRICS            (0x00)    /*!< Metrics group. */
#define RCO_ATTR_VERSION               (0x00)    /*!< RC version retrieval. */

#define RCO_GROUP_MODE                 (0x01)    /*!< Operation mode group. */
#define RCO_ATTR_SOFTRESET             (0x00)    /*!< Resets Hardware to defaults */
#define RCO_ATTR_MFRD_A                (0x03)   
#define RCO_GROUP_COMMUNICATION        (0x02)    /*!< Communications group. */
#define RCO_ATTR_RFBAUD                (0x00)    /*!< Baud rate for the CL RF UART, \n
                                                      TxSpeed: param_a and RxSpeed: param_b.   */
#define RCO_VAL_RF106K                 (0x00)    /* RF UART speed setting(s). */
#define RCO_VAL_RF212K                 (0x01)    /* */
#define RCO_VAL_RF424K                 (0x02)    /* */
#define RCO_ATTR_RS232BAUD             (0x01)    /*!< Serial interface baud setting. */
#define RCO_VAL_SER9600                (0x00)    /* Serial RS232 speed setting(s). */
#define RCO_VAL_SER19200               (0x01)    /* */
#define RCO_VAL_SER38400               (0x02)    /* */
#define RCO_VAL_SER57600               (0x03)    /* */
#define RCO_VAL_SER115200              (0x04)    /* */
#define RCO_VAL_SER230400              (0x05)    /* */
#define RCO_VAL_SER460800              (0x06)    /* */
#define RCO_VAL_SER921600              (0x07)    /* */
#define RCO_VAL_SER1288000             (0x08)    /* */
#define RCO_ATTR_CRC                   (0x03)    /*!< TxCRC: param_a, RxCRC: param_b */
#define RCO_VAL_CRC_OFF                (0x00)    /* */
#define RCO_VAL_CRC_ON                 (0x01)    /* */
#define RCO_ATTR_FLUSH_BUFFER          (0x04)    /*!< no parameter needed */
#define RCO_ATTR_DRIVER_SEL            (0x06)    /*!< tbd */
#define RCO_ATTR_SIGOUT_SEL            (0x07)    /*!< tbd */
#define RCO_ATTR_UART_SEL              (0x08)    
#define RCO_GROUP_SYSTEM               (0x03)    /*!< System group/settings. */
#define RCO_ATTR_SOFT_POWER_DOWN       (0x00)    /*!< Soft Power down functional attribute. */
#define RCO_VAL_SOFT_POWER_UP          (0x00)    /* */
#define RCO_VAL_SOFT_POWER_DOWN        (0x01)    /* */
#define RCO_ATTR_TEMP_SENS             (0x01)    /* tbd */
#define RCO_VAL_TEMP_SENS_ON           (0x00)    /* */
#define RCO_VAL_TEMP_SENS_OFF          (0x01)    /* */
#define RCO_ATTR_IRQ_PUSH_PULL         (0x02)    /*!< IrqMode = PushPull or open drain output pad */
#define RCO_VAL_IRQ_PUSH_PULL_OFF      (0x00)    /* */
#define RCO_VAL_IRQ_PUSH_PULL_ON       (0x01)    /* */
#define RCO_ATTR_IRQ_INV               (0x03)    /*!< Inversion of Irq output pad */
#define RCO_VAL_IRQ_INV_OFF            (0x00)    /* */
#define RCO_VAL_IRQ_INV_ON             (0x01)    /* */
#define RCO_ATTR_WATERLEVEL            (0x04)    /* tbd */
#define RCO_ATTR_SELFTEST              (0x05)  
#define RCO_GROUP_RF                   (0x04)    /*!< RF driver settings. */
#define RCO_ATTR_RCV_OFF               (0x00)    /*!< Switches Receiver either on or off. */
#define RCO_VAL_RCV_ON                 (0x00)    /* */
#define RCO_VAL_RCV_OFF                (0x01)    /* */
#define RCO_ATTR_RF_EN                 (0x01)    /*!< Switches the driver on or off.\n
                                                       param_a: Tx1, param_b: Tx2 */
#define RCO_VAL_RF_OFF                 (0x00)    /* */
#define RCO_VAL_RF_ON                  (0x01)    /* */
#define RCO_ATTR_RF_INV_TXON           (0x03)    /*!< Activates or deactivates the inverted driver output in case the drivers are switched on.\n
                                                      param_a: Tx1, param_b: Tx2 */
#define RCO_ATTR_RF_INV_TXOFF          (0x04)    /*!< Activates or deactivates the inverted driver output in case the drivers are switched off (loadmodulation).\n
                                                       param_a: Tx1, param_b: Tx2 */
#define RCO_VAL_RF_INV_TX_OFF          (0x00)    /* */
#define RCO_VAL_RF_INV_TX_ON           (0x01)   
#define RCO_GROUP_SIGNAL               (0x05)    /*!< Signal interface group. */
#define RCO_ATTR_GAIN                  (0x00)    /*!< Sets the Gain attribute (max. 0x07). */
#define RCO_ATTR_MINLEVEL              (0x01)    /*!< Sets the RX min. level (max. 0x0F). */
#define RCO_ATTR_COLLEVEL              (0x02)    /*!< Sets the Collision Min. level for the receiver (max. 0x07). */
#define RCO_ATTR_RXWAIT                (0x03)    /*!< Sets the number of RxWaitBits (max. 0x3F) [bits]      */
#define RCO_ATTR_CWGSN                 (0x04)    /*!< Sets the CW of the N-Mos driver (max. 0x0F) */
#define RCO_ATTR_MODGSN                (0x05)    /*!< Sets the MOD of the N-Mos driver (max. 0x0F) */
#define RCO_ATTR_CWGSP                 (0x06)    /*!< Sets the CW of the P-Mos driver (max. 0x3F) */
#define RCO_ATTR_MODGSP                (0x07)    /*!< Sets the MOD of the P-Mos driver (max. 0x3F) */
#define RCO_ATTR_MODWIDTH              (0x09)    /*!< Sets the Modwidth register */
#define RCO_ATTR_ADD_IQ                (0x0A)    /*!< Sets the ADDIQ mode (max. 0x03) */
#define RCO_ATTR_TAU_RCV               (0x0C)    /* */
#define RCO_ATTR_TAU_SYNC              (0x0D)  
#define RCO_GROUP_TIMER                (0x06)    /*!< Timer relevant settings. */
#define RCO_ATTR_TIMER_START_STOP      (0x00)    /*!< tbd */
#define RCO_VAL_TIMER_START_NOW        (0x00)    /* */
#define RCO_VAL_TIMER_STOP_NOW         (0x01)    /* */
#define RCO_ATTR_TIMER_AUTO            (0x01)    /*!< tbd */
#define RCO_ATTR_TIMER_AUTO_RESTART    (0x02)    /*!< tbd */
#define RCO_VAL_TIMER_AUTO_OFF         (0x00)    /* */
#define RCO_VAL_TIMER_AUTO_ON          (0x01)    /* */
#define RCO_ATTR_TIMER_GATED           (0x03)    /*!< param_a: value of TGated (max. 3) */
#define RCO_ATTR_TIMER_PRESCALER       (0x04)    /*!< param_a: TPrescaler_LO, \n
                                                    param_b: TPrescaler_HI (max. 0x0F) */
#define RCO_ATTR_TIMER_RELOAD          (0x05)    /*!< param_a: TReloadVal_LO \n
                                                    param_b: TReloadVal_HI      */
#define RCO_ATTR_TIMER_COUNTER         (0x06)   
#define RCO_GROUP_CRC                  (0x07)    /*!< CRC relevant settings. */
#define RCO_ATTR_CRC_PRESET            (0x00) 
#define RCO_ATTR_CRC_RESULT            (0x01)    /*!< param_a: Result_LSB, param_b: Result_MSB */
#define RCO_ATTR_CRC_CALCULATE         (0x03)  
#define RCO_GROUP_PROTOCOL             (0x08)    /*!< Protocol (data exchange) settings. */
#define RCO_ATTR_CRYPTO1_ON            (0x08)  
#define RC522_UARTSEL_SHL_VALUE        0x06 /* */
#define RC522_SPEED_SHL_VALUE          0x04 /* */
#define RC522_MINLEVEL_SHL_VALUE       0x04 /* */
#define RC522_GAIN_SHL_VALUE           0x04 /* */
#define RC522_TGATED_SHL_VALUE         0x05 /* */
#define RC522_DRIVERSEL_SHL_VALUE      0x04 /* */
#define RC522_CWGSN_SHL_VALUE          0x04 /* */
#define RC522_ADDIQ_SHL_VALUE          0x06 /* */
#define RC522_TAURCV_SHL_VALUE         0x02 /* */
#define RC522_CRYPTO1ON_SHL_VALUE      0x03 /* */


#endif 



