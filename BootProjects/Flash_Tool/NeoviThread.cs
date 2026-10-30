using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Byte_Help;


namespace FlashTool
{
    
    
    public enum NeoviState
    {
        Init,
        Connected,
        Disconnected,
        Fault,
        Count
    };

    public class NeoviThread
    {
        // internal objects
        private FTMain myMainFormReference;
        private NeoVIDriver myNeovi;
        private FTCommMessage[] commMessages_CAN;
        public int lengthcounter = 0;
        public int counter = 0;
        public string line;
        public int length;
        public string extension;
        public string path;
        public byte progress_level;
        public bool session_started=false;
        public bool file_read_req;
        public bool file_open_resp;
        public byte file_ack_retry_cnt;
        public byte file_req_retry_cnt;
        public bool file_ack_resp;
        public byte jump_to_app_cnt;
        // control interfaces write
        public NeoviState requestedState;
        public bool SheepCounting;
        public bool CAN_Enabled = true;
        public bool[] commMessagesEnabled_CAN;
        // control interfaces read
        public NeoviState currentState;
        public string errorStatus;
        public bool CAN_Error;
        private int lastCanErrorTick = Environment.TickCount;   // when the last CAN error was seen; the error clears itself after a quiet time
        private int[] commErrorDelayCounter;
        
        public IntelHexParser hex = null;

        // signal interfaces read
        public NeoviThread(FTMain myForm)
        {
            myMainFormReference = myForm;
            currentState = NeoviState.Init;
            requestedState = NeoviState.Init;

            myNeovi = new NeoVIDriver();

            InitializeCommMessages();

            SheepCounting = false;

            commErrorDelayCounter = new int[(int)FTCommBus.Count];
            resetCommErrorDelayCounters();
        }
    
        public void resetCommErrorDelayCounters()
        {
            for (int i = 0; i < (int)FTCommBus.Count; i++)
            {
                commErrorDelayCounter[i] = 0;
            }
        }

        public void setNeoviChannel(FTCommBus bus, short channelID)
        {
            this.myNeovi.NeoVI_Channel[(int)bus] = (int)channelID;
        }

        private void InitializeCommMessages()
        {
            // create public message enable buffers

            this.commMessagesEnabled_CAN = new bool[(int)FTCommMessageId_CAN.Count];



            // enable all messages by default

            for (int i = 0; i < this.commMessagesEnabled_CAN.Length; i++)
            {
                this.commMessagesEnabled_CAN[i] = true;
            }
            // create private message buffers
            this.commMessages_CAN = new FTCommMessage[(int)FTCommMessageId_CAN.Count];
            // initialize messages
            
            this.commMessages_CAN[(int)FTCommMessageId_CAN.DiagReq] = new FTCommMessage(FTMessageType.WriteCAN, 0x7E0, new byte[] { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF });
            this.commMessages_CAN[(int)FTCommMessageId_CAN.DiagResp] = new FTCommMessage(FTMessageType.ReadCAN, 0x7E8, new byte[] { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF });
            
        }



        // ---------------------------------------------------------------- UDS flashing
        [System.Runtime.InteropServices.DllImport("winmm.dll")]
        private static extern uint timeBeginPeriod(uint ms);
        [System.Runtime.InteropServices.DllImport("winmm.dll")]
        private static extern uint timeEndPeriod(uint ms);

        public volatile bool flashRequested = false;
        public volatile bool flashRunning = false;
        public string flashStatus = "";
        public volatile string ecuInfo = "";          // boot version / serial / programmed status, shown on the panel

        // Disconnect while programming: the flash thread notices this flag inside every CAN wait / transmit and stops.
        private volatile bool abortRequested = false;
        public void AbortFlash()
        {
            this.abortRequested = true;
            FlashTrace.Write("FLASH", "abort requested by the user");
        }

        private void RunFlash()
        {
            this.flashRequested = false;
            this.flashStatus = "";
            this.ecuInfo = "";
            this.flashRunning = true;
            this.progress_level = 0;
            timeBeginPeriod(1);                       // makes Thread.Sleep(1) really about 1 ms
            try
            {
                // FlashTool.ini is read again for every flash: changes apply without restarting the tool
                IniFile ini = FlashSettings.Load();

                // request / response IDs: DiagReq / DiagResp above, or [CAN] RequestId / ResponseId of the ini file
                int requestId = FlashSettings.RequestId(ini, this.commMessages_CAN[(int)FTCommMessageId_CAN.DiagReq].messageId);
                int responseId = FlashSettings.ResponseId(ini, this.commMessages_CAN[(int)FTCommMessageId_CAN.DiagResp].messageId);

                NeoViCanBus bus = new NeoViCanBus(this.myNeovi);
                bus.AbortCheck = () => this.abortRequested;
                IsoTp tp = new IsoTp(bus, requestId, responseId);
                UdsClient uds = new UdsClient(tp);
                UdsFlasher flasher = new UdsFlasher(uds);
                flasher.Log = s => this.flashStatus = s;
                flasher.Progress = p => this.progress_level = (byte)p;
                flasher.Info = s => this.ecuInfo = s;
                flasher.CancelCheck = () => this.abortRequested;

                // ---- sequence settings: the defaults are in UdsFlasher, FlashTool.ini changes them (no code change needed) ----
                FlashSettings.Apply(ini, tp, uds, flasher);
                FlashSettings.Report(ini);                            // typing mistakes in the ini file go to the log

                FlashTrace.Write("FLASH", "request ID 0x" + requestId.ToString("X3") + ", response ID 0x" + responseId.ToString("X3"));
                flasher.Flash(this.hex);
                this.flashStatus = "Flash complete";
                FlashTrace.Write("FLASH", "finished OK");
            }
            catch (OperationCanceledException)
            {
                this.flashStatus = "Flash aborted by user";
                FlashTrace.Write("FLASH", "aborted by the user, the ECU stays in the state it was in");
            }
            catch (Exception ex)
            {
                this.flashStatus = "Flash failed: " + ex.Message;
                FlashTrace.Write("ERROR", ex.ToString());
            }
            finally
            {
                this.abortRequested = false;
                timeEndPeriod(1);
                DrainReceiveBuffer();               // frames buffered during the flash (error frames of the ECU reset) are not errors of the idle bus
                this.flashRunning = false;
            }
        }

        // progress bar, progress text and result are shown only for a flash that was started in this connection
        private void ClearFlashDisplay()
        {
            if (this.flashRunning) return;
            this.flashStatus = "";
            this.progress_level = 0;
            this.ecuInfo = "";                          // the info belongs to the ECU of the connection that ended
        }

        // Right after Connect: find out what the ECU is running and show its software version, so that the user
        // knows what is in the ECU before reflashing. Disconnect or Program ends it early.
        private void ReadEcuInfoOnConnect()
        {
            try
            {
                IniFile ini = FlashSettings.Load();
                int requestId = FlashSettings.RequestId(ini, this.commMessages_CAN[(int)FTCommMessageId_CAN.DiagReq].messageId);
                int responseId = FlashSettings.ResponseId(ini, this.commMessages_CAN[(int)FTCommMessageId_CAN.DiagResp].messageId);

                NeoViCanBus bus = new NeoViCanBus(this.myNeovi);
                bus.AbortCheck = () => this.requestedState != NeoviState.Connected || this.flashRequested;
                IsoTp tp = new IsoTp(bus, requestId, responseId);
                UdsClient uds = new UdsClient(tp);
                UdsFlasher flasher = new UdsFlasher(uds);
                flasher.Log = s => { };                         // the steps go to the log file only: no progress bar for this
                flasher.Info = s => this.ecuInfo = s;
                flasher.CancelCheck = bus.AbortCheck;

                FlashSettings.Apply(ini, tp, uds, flasher);
                FlashSettings.Report(ini);
                if (!flasher.ReadInfoOnConnect) return;

                this.ecuInfo = "reading ECU info...";
                flasher.ReadEcuInfo();
            }
            catch (OperationCanceledException)
            {
                this.ecuInfo = "";
            }
            catch (Exception ex)
            {
                FlashTrace.Write("ERROR", "ECU info after connect: " + ex.Message);
                this.ecuInfo = "ECU info not read: " + ex.Message;
            }
            finally
            {
                DrainReceiveBuffer();
            }
        }

        // throw away what the driver buffered while the flash was running and clear the CAN error flag
        private void DrainReceiveBuffer()
        {
            try
            {
                for (int i = 0; i < 10; i++)
                {
                    if (!this.myNeovi.GetReceivedMessages() || this.myNeovi.Number_of_ReceivedMessages == 0) break;
                }
            }
            catch (Exception) { }
            CAN_Error = false;
            lastCanErrorTick = Environment.TickCount;
        }

        public void Run()
        {
            
            // main loop
            while (true)
            {
                if ((this.currentState == NeoviState.Init) || (this.currentState == NeoviState.Disconnected))
                {
                    // disconnected state
                    if (this.requestedState == NeoviState.Connected)
                    {
                        // connect
                        if (Connect() == true)
                        {
                            this.currentState = NeoviState.Connected;
                            ClearFlashDisplay();                    // a new connection starts without the old progress / result
                            ReadEcuInfoOnConnect();                 // 10 01, 10 03, F180, then F191 / F181 in the application
                            #if TEST_A_CAN_MESSAGE
                            // one test frame right after connect: ID 0x7E0, 8 bytes
                            transmitMessage(FTCommBus.CAN, new FTCommMessage(FTMessageType.WriteCAN, 0x7E0, new byte[] { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }));                        
                            #endif
                        }
                        else
                        {
                            this.currentState = NeoviState.Fault;
                        }
                    }
                }
                else if (this.currentState == NeoviState.Connected)
                {
                    // connected state
                    if (this.requestedState == NeoviState.Disconnected)
                    {
                        // disconnect (a flash that was requested but did not start yet is dropped)
                        this.flashRequested = false;
                        this.abortRequested = false;
                        if (Disconnect() == true)
                        {
                            this.currentState = NeoviState.Disconnected;
                            ClearFlashDisplay();                    // progress bar and result are hidden when disconnected

                        }
                        else
                        {
                            this.currentState = NeoviState.Fault;
                        }
                    }
                    else if (this.flashRequested && this.hex != null)
                    {
                        RunFlash();
                    }
                    else
                    {
                        // process main communication messages
                        ProcessCommMessages();
                        CheckNeoVIStatus();
                    }
                }
                else
                {
                    // error state
                }
            }
        }

        private bool Connect()
        {
            FlashTrace.Write("APP", "Connecting to the neoVI...");
            bool ok = this.myNeovi.Connect();
            FlashTrace.Write("APP", ok ? "Connected" : "Connect FAILED: " + this.myNeovi.GetNeoviErrorStatusString());
            return ok;
        }

        private bool Disconnect()
        {
            FlashTrace.Write("APP", "Disconnecting from the neoVI");
            this.myNeovi.Disconnect();
            FlashTrace.Write("APP", "Disconnected");
            return true;
        }



        private void ProcessCommMessages()
        {
            // get any pending receive message(s)
            if (this.myNeovi.GetReceivedMessages() == true)
            {
                // message receipt successful
                for (int receivedMsg = 0; receivedMsg < this.myNeovi.Number_of_ReceivedMessages; receivedMsg++)
                {
                    // process received messages one at a time
                    receiveMessageAndCheckForErrors(NeoVIUtil.ConvertNeoviMessageToCommMessage(ref this.myNeovi.ReceiveMessageBuffer[receivedMsg]));
                }
            }
            else
            {
                // error receiving messages
            }

            // transmit messages


            if (commErrorDelayCounter[(int)FTCommBus.CAN] > 0)
            {
                commErrorDelayCounter[(int)FTCommBus.CAN]--;
            }
            else
            {
                transmitMessage(FTCommBus.CAN, GetNextMessageToTransmit_CAN());
            }

          

            // a CAN error is shown only while errors keep coming (a single error frame, e.g. from an ECU reset, must not stay red)
            if (CAN_Error && (Environment.TickCount - lastCanErrorTick) > 2000) CAN_Error = false;

            // take a nap
            Thread.Sleep(35);
            Console.Write(".");
        }

        private void receiveMessageAndCheckForErrors(FTCommReceiveMessage myReceiveMessage)
        {
            if ((myReceiveMessage.statusBitfield > 2))//((myReceiveMessage.statusBitfield & (int)icsneo40.icsSpyDataStatusBitfield.icsSpyStatusGlobalError) == (int)icsneo40.icsSpyDataStatusBitfield.icsSpyStatusGlobalError)
            {
                // ut oh, there was an error
 
                if (myReceiveMessage.bus == FTCommBus.CAN)
                {
                    commErrorDelayCounter[(int)FTCommBus.CAN] = 0;//10;
                    { CAN_Error = true; lastCanErrorTick = Environment.TickCount; }
                }
 
            }
            else if (myReceiveMessage.statusBitfield == (myReceiveMessage.statusBitfield | (int)icsneo40.icsSpyDataStatusBitfield.icsSpyStatusTx))
            {
                // we transmitted this without an error, clear errors

                if (myReceiveMessage.bus == FTCommBus.CAN)
                {
                    //this.CAN_Error = false;
                }

  
            }
            else
            {
                // good receive message, clear errors

                if (myReceiveMessage.bus == FTCommBus.CAN)
                {
                    //this.CAN_Error = false;
                }

                processReceivedMessage(myReceiveMessage);
            }
        }

        private void processReceivedMessage(FTCommReceiveMessage myReceiveMessage)
        {
            if (myReceiveMessage.bus == FTCommBus.CAN)
            {

                processReceivedMessage_CAN(myReceiveMessage);
            }
  
        }




        private void processReceivedMessage_CAN(FTCommReceiveMessage myReceiveMessage)
        {
  
            if (myReceiveMessage.message.messageId == this.commMessages_CAN[(int)FTCommMessageId_CAN.DiagResp].messageId)
            {
                if (myReceiveMessage.message.payload.Length == this.commMessages_CAN[(int)FTCommMessageId_CAN.DiagResp].payload.Length)
                {
                    processReceivedMessage_CAN_DiagResp(myReceiveMessage);
                    CAN_Error = false;
                }
                else
                {
                    { CAN_Error = true; lastCanErrorTick = Environment.TickCount; }
                }
            }
        }

        private void processReceivedMessage_CAN_DiagResp(FTCommReceiveMessage myReceiveMessage)
        {
            byte[] buffer = myReceiveMessage.message.payload;
            if (buffer[0] == 0x41)
            {
                commMessages_CAN[(int)FTCommMessageId_CAN.DiagReq].sent = true;
                file_ack_resp = true;
            }
            else if (buffer[0] == 0x45)
            {
                commMessages_CAN[(int)FTCommMessageId_CAN.DiagReq].sent = false;
                file_ack_resp = true;
            }
            else
            {

            }

        }



        private void transmitMessage(FTCommBus txBus, FTCommMessage myTransmitMessage)
        {
            if ((myTransmitMessage != null) && (SheepCounting == true))
            {
                // check message

                if ((txBus == FTCommBus.CAN) && (myTransmitMessage.messageId == 0x9C))
                {
                    // do nothing
                }
                else
                {
                    // Shhhhh, we're trying to sleep!
                    myTransmitMessage = null;
                }
            }

            // transmit message
            if (myTransmitMessage != null)
            {
                if (this.myNeovi.TransmitMessage(txBus, myTransmitMessage) == true)
                {
                    Thread.Sleep(5);
                }
                else
                {
                    // transmit error
                    Console.Write("e");

                    // set an error delay
                    commErrorDelayCounter[(int)txBus] = 1;

                  
   
                    if (txBus == FTCommBus.CAN)
                    {
                        { CAN_Error = true; lastCanErrorTick = Environment.TickCount; }
                    }
  
                }
            }
        }

  

        private FTCommMessage GetNextMessageToTransmit_CAN()
        {
            
            FTCommMessage returnMessage = null;

            if (CAN_Enabled == true)
            {
                if (commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagReq] == true)//if req already enabled
                {
                  if (true == updateTransmitMessageData_CAN_DiagReq())//fill in the payload
                  {
                      returnMessage = commMessages_CAN[(int)FTCommMessageId_CAN.DiagReq];//transmit the msg
                      file_req_retry_cnt++;
                      if (file_read_req && !file_open_resp)//end of line transmission
                      {
                            //wait for ack 
                            commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagReq] = false;
                            //start response 
                            commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagResp] = true;
                            file_ack_retry_cnt = 5;
                            
                            file_ack_resp = false;
                      }
                  }
                }
                else if(commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagResp] == true) //if resp enabled and message already sent
                {
                    if (!file_ack_resp)
                    {
                        file_ack_retry_cnt--;
                        if (0 == file_ack_retry_cnt)//retry timeout
                        {
                            
                            if (file_req_retry_cnt > 2)
                            {
                                System.Windows.Forms.MessageBox.Show("No Response From ECU - Click Ok to Close");
                                Environment.Exit(1);
                            }
                            

                            commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagResp] = false;
                            commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagReq] = true;
                            file_read_req = false;
                            file_open_resp = true;
                        }
                        returnMessage = commMessages_CAN[(int)FTCommMessageId_CAN.DiagResp];

                    }
                    else//we got acknowledgement so carry on with next message
                    {
                        file_req_retry_cnt = 0;
                        commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagResp] = false;
                        commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagReq] = true;
                    }
                }
            }
   
            return returnMessage;
        }
        public byte[] linbuff;
        byte[] srecline;
        System.IO.StreamReader file;

        byte index;
        private bool updateTransmitMessageData_CAN_DiagReq()
        {
            bool status = false;
            linbuff = commMessages_CAN[(int)FTCommMessageId_CAN.DiagReq].payload;

            if (this.path == null)
            {
                file_read_req = false;
                file_open_resp = false;
                return status;
            }
            if (file_read_req)
            {
                if (this.counter == 0)
                {
                    file = new System.IO.StreamReader(this.path);
                    commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagReq] = true;
                    commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagResp] = false;
                }

                if ((this.line = file.ReadLine()) != null)
                {
                   session_started = true;
                   progress_level = (byte)((this.counter * 100)/this.lengthcounter);
                    
                    this.counter++;
                    srecline = Encoding.ASCII.GetBytes(this.line);
                    if ((srecline[0] == 'S') &&
                       ((srecline[1] == '0') || (srecline[1] == '9')))
                    {
                        file_read_req = true;
                        return status;
                    }
                    if (this.counter > 0)
                    {
                        //send the diag request
                        status = true;
                    }

                    srecline = StringToByteArrayFastest(this.line);
                    index = 1;
                    this.length = srecline[1]+ 1;//the second  byte gives the length of the record + 1byte length itself


                    file_open_resp = true;
                    file_read_req = false;
                }
                else //end of file,null response,stop transmitting 1C and 1D
                {
                    session_started = false;
                    progress_level = (byte)100;

                    file_open_resp = false;
                    file_read_req = false;
                    commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagReq] = false;
                    commMessagesEnabled_CAN[(int)FTCommMessageId_CAN.DiagResp] = false;

                    return status;
                }
            }
            if (file_open_resp)//file opened successfully
            {
                status = true;

                if (this.length < 8)  //ignore the first byte since its "Sx"
                {
                    Array.ConstrainedCopy(srecline, 1, linbuff, 0, this.length);
                    //end of line transfer
                    file_read_req = true;
                    file_open_resp = false;
                }
                else
                {
                    if ((this.length - index) >= 8u)
                    {
                        linbuff[0] = srecline[index];
                        linbuff[1] = srecline[index + 1];
                        linbuff[2] = srecline[index + 2];
                        linbuff[3] = srecline[index + 3];
                        linbuff[4] = srecline[index + 4];
                        linbuff[5] = srecline[index + 5];
                        linbuff[6] = srecline[index + 6];
                        linbuff[7] = srecline[index + 7];
                        index += 8;
                    }
                    else//lesser than 8 bytes
                    {
                        Array.Clear(linbuff,0,8);
                        Array.ConstrainedCopy(srecline, index, linbuff, 0, (this.length - index + 1));
                        //end of line transfer
                        file_read_req = true;
                        file_open_resp = false;
                    }
                }
            }
            else
            {
                this.length = 0;
                this.counter = 0;
            }

            return status;


        }
        public static byte[] StringToByteArrayFastest(string hex)
        {
            if (hex.Length % 2 == 1)
                throw new Exception("The binary key cannot have an odd number of digits");

            byte[] arr = new byte[hex.Length >> 1];

            for (int i = 0; i < hex.Length >> 1; ++i)
            {
                arr[i] = (byte)((GetHexVal(hex[i << 1]) << 4) + (GetHexVal(hex[(i << 1) + 1])));
            }

            return arr;
        }

        public static int GetHexVal(char hex)
        {
            int val = (int)hex;
            //For uppercase A-F letters:
            return val - (val < 58 ? 48 : 55);
            //For lowercase a-f letters:
            //return val - (val < 58 ? 48 : 87);
            //Or the two combined, but a bit slower:
            //return val - (val < 58 ? 48 : (val < 97 ? 55 : 87));
        }
        private void CheckNeoVIStatus()
        {
            this.errorStatus = this.myNeovi.GetNeoviErrorStatusString();
        }
 
    }
}

