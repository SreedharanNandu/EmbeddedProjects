using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Timers;
using System.IO;





namespace FlashTool
{

    

    public partial class FTMain : Form
    {
        private string title = "FlashTool - Flash Programming Tool (by Sree) ";
        private static readonly string toolVersion = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(3);

        private Thread myNeoviThread;
        private NeoviThread myNeoVI;


        private System.Timers.Timer MainTimer;



        public FTMain()
        {
            InitializeComponent();

            this.Text = this.title;

            myNeoviThread = new Thread(new ThreadStart(ThreadFunction));


          

            MainTimer = new System.Timers.Timer();
            MainTimer.Interval = 20;
            MainTimer.Elapsed += new ElapsedEventHandler(MainTimerTick);
            MainTimer.SynchronizingObject = this;
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            // stop the worker thread before the main form closes
            myNeoviThread.Abort();
            MainTimer.Stop();
            FlashTrace.Write("APP", "FlashTool closed");
            FlashTrace.CloseLog();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            // start the NeoVI worker thread after the main form loads
            myNeoviThread.Start();

            MainTimer.Start();
        }

        private void ThreadFunction()
        {
            myNeoVI = new NeoviThread(this);

            // initialize support forms

            myNeoVI.Run();
        }

        private string lastFlashStatus = "";
        private int completeTick = 0;                       // when the flash reported complete (0 = not yet)
        private const int BarSettleMs = 250;                // the bar is drawn without animation: only a short pause before "Complete"
        public bool setDefaultValuesNeeded = true;
        private void MainTimerTick(object sender, System.Timers.ElapsedEventArgs e)
        {
            if (this.setDefaultValuesNeeded == true)
            {
                // initialize NeoVI channels
                UpdateNeoviChannels();

                // set default signal values
                setDefaultSignalValues();

                this.setDefaultValuesNeeded = false;
            }

            // update NeoVI status
            updateNeoviStatus();

            // update GUI from received signals
            checkForUpdatedSignals_CAN();

            //update the flash programming progress
            update_panel();


        }

        private void update_panel()
        {
            int level = Math.Min(100, (int)this.myNeoVI.progress_level);
            string st = this.myNeoVI.flashStatus;

            // build every text once, assign it only when it changed (this runs every 20 ms; assigning
            // the same text twice per tick, or on every tick, makes the label flicker)
            // no percentage before a flash was requested: "Progress 0%" appears only when the flash really starts
            bool started = this.myNeoVI.flashRequested || this.myNeoVI.flashRunning || st.Length > 0;
            if (!started) level = 0;
            // The Windows progress bar draws its fill with a short animation, so it reaches the end a moment after
            // the value is set. "Complete", the title text and the message box wait until the bar has caught up.
            bool done = level >= 100 && st.StartsWith("Flash complete");
            if (!done) this.completeTick = 0;
            else if (this.completeTick == 0) this.completeTick = Environment.TickCount | 1;
            bool settled = done && unchecked(Environment.TickCount - this.completeTick) >= BarSettleMs;
            bool pending = done && !settled;
            bool complete = settled;
            string result = complete ? " - Complete" : st.StartsWith("Flash aborted") ? " - Aborted" : st.StartsWith("Flash failed") ? " - Failed" : "";
            string progressText = "Progress " + level.ToString() + "%" + result;
            if (Progress.Visible != started) Progress.Visible = started;          // progress is hidden until programming starts
            if (ProgressBar.Visible != started) ProgressBar.Visible = started;
            if (started)
            {
                if (Progress.Text != progressText) Progress.Text = progressText;
                if (ProgressBar.Value != level) { ProgressBar.Value = level; ProgressBar.Invalidate(); }
            }

            // tool version + what the boot reported (version, serial, programmed status)
            string ecu = this.myNeoVI.ecuInfo;
            string versionText = "Tool v" + toolVersion + "   |   " + (string.IsNullOrEmpty(ecu) ? "ECU: not read yet" : ecu);
            if (labelVersion.Text != versionText) labelVersion.Text = versionText;

            // show the flash step in the title bar, report the end result once
            string titleText = this.title + (st.Length > 0 && !pending ? " - " + st : "");
            if (this.Text != titleText) this.Text = titleText;
            if (!pending && st != this.lastFlashStatus)
            {
                this.lastFlashStatus = st;
                if (st.StartsWith("Flash failed"))
                {
                    MessageBox.Show(st, "Flash");
                }
                else if (st.StartsWith("Flash complete") && level >= 100)
                {
                    // the status is only set after erase, download, check, state read and reset all succeeded
                    MessageBox.Show(st, "Flash");
                }
            }

        }
        private void setDefaultSignalValues()
        {



        }


        private void checkForUpdatedSignals_CAN()
        {
          

        }

        private void updateNeoviStatus()
        {
            if (myNeoVI.currentState == NeoviState.Connected)
            {
                if (myNeoVI.requestedState == NeoviState.Connected)
                {
                    this.Connect.Text = "Disconnect";
                    this.Connect.Enabled = true;

                    neoviStatusLabel.Text = "Connected: " + myNeoVI.errorStatus;
                    updateCommBusErrorStatus();
                }

            }
            else if ((myNeoVI.currentState == NeoviState.Disconnected) ||
                (myNeoVI.currentState == NeoviState.Init))
            {
                if ((myNeoVI.requestedState == NeoviState.Disconnected) ||
                    (myNeoVI.requestedState == NeoviState.Init))
                {
                    this.Connect.Text = "Connect";
                    this.Connect.Enabled = true;

                    neoviStatusLabel.Text = "Not Connected";
                    updateCommBusErrorStatus();
                }


            }
            else
            {
                if (this.Connect.Text != "Error")
                {
                    this.Connect.Text = "Error";
                    neoviStatusLabel.Text = "Error: " + myNeoVI.errorStatus;

                    MessageBox.Show("An error has occurred trying to connect to the NeoVI. Please restart the application to try again.", "Error");
                    this.Close();
                }
            }

        }

        private void formatError(Label myLabel)
        {
            myLabel.ForeColor = Color.Red;
        }

        private void formatNormal(Label myLabel)
        {
            myLabel.ForeColor = Color.Black;
        }

        private void formatDisabled(Label myLabel)
        {
            myLabel.ForeColor = Color.Black;
        }

        private void updateCommBusErrorStatus()
        {
            // update error status
            if (this.myNeoVI.CAN_Enabled == true)
            {
                if (this.myNeoVI.CAN_Error == true)
                {
                    labelCAN.Text = "CAN Status: Err";

                    formatError(labelCAN);
                }
                else if(this.myNeoVI.currentState == NeoviState.Connected)
                {
                    labelCAN.Text = "CAN Status: On";
                    formatNormal(labelCAN);
                }
                else if(this.myNeoVI.currentState == NeoviState.Disconnected)
                {
                    labelCAN.Text = "CAN Status: Off";
                    formatNormal(labelCAN);
                }
            }
            else
            {
                labelCAN.Text = "CAN Status: Off";
                formatDisabled(labelCAN);
            }

        }

        private void Connect_Click(object sender, EventArgs e)
        {
            // Disconnect while programming = abort: the flash thread stops at once, then the neoVI is disconnected
            if (this.myNeoVI.currentState == NeoviState.Connected && (this.myNeoVI.flashRunning || this.myNeoVI.flashRequested))
            {
                if (FlashSettings.ConfirmAbort)
                {
                    DialogResult answer = MessageBox.Show(
                        "Programming is in progress.\r\n\r\nAbort it and disconnect?\r\n\r\n"
                        + "Warning:- ECU may not be operate as intended.",
                        "Abort programming", MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2);
                    if (answer != DialogResult.Yes) return;
                }
                FlashTrace.Write("APP", "Disconnect clicked while programming: aborting the flash");
                this.myNeoVI.AbortFlash();
            }
            else
            {
                FlashTrace.Write("APP", this.myNeoVI.currentState == NeoviState.Connected ? "Disconnect clicked" : "Connect clicked");
            }

            if(this.myNeoVI.currentState == NeoviState.Connected)
            {
                this.myNeoVI.requestedState = NeoviState.Disconnected;
                this.Connect.Text = "Disconnecting...";
            }
            else
            {
                this.myNeoVI.requestedState = NeoviState.Connected;
                this.Connect.Text = "Connecting...";
            }
            this.Connect.Enabled = false;
        }

        private void Program_Click(object sender, EventArgs e)
        {
            if (this.myNeoVI.currentState != NeoviState.Connected) { MessageBox.Show("Connect first"); return; }
            if (this.myNeoVI.hex == null) { MessageBox.Show("Open a HEX file first"); return; }
            if (this.myNeoVI.flashRunning || this.myNeoVI.flashRequested) return;
            // clear the old result right away so the bar starts from 0 % and "Complete" is not shown for the new run
            this.myNeoVI.progress_level = 0;
            this.myNeoVI.flashStatus = "";
            this.myNeoVI.ecuInfo = "";
            this.lastFlashStatus = "";
            FlashTrace.Write("APP", "Program clicked, HEX file " + this.myNeoVI.path);
            this.myNeoVI.flashRequested = true;
        }

        private void Open_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Intel HEX (*.hex)|*.hex|All files (*.*)|*.*";
            dlg.FilterIndex = 1;
            dlg.RestoreDirectory = true;

            if (dlg.ShowDialog() != DialogResult.OK) return;

            try
            {
                IntelHexParser hex = IntelHexParser.Load(dlg.FileName);

                // split the HEX data into the partitions (see PartitionTable): throws if it does not fit
                FlashPlan plan = FlashPlan.Build(hex, FlashSettings.LoadPartitions());

                this.myNeoVI.hex = hex;
                this.myNeoVI.path = dlg.FileName;
                textBox2.Text = dlg.FileName;

                StringBuilder sb = new StringBuilder();
                foreach (FlashImage image in plan.Images)
                {
                    sb.AppendLine(image.Partition.Name + ":");
                    sb.AppendLine("  0x" + image.StartAddress.ToString("X8") + " - 0x" + image.LastAddress.ToString("X8")
                                  + "  (" + image.Length + " bytes)");
                    sb.AppendLine("  from the HEX file: " + image.HexBytes + " bytes, filled: " + image.FillBytes + " bytes");
                    sb.AppendLine("  checksum: 0x" + image.Checksum.ToString("X4"));
                }
                sb.AppendLine();
                sb.AppendLine(hex.Blocks.Count + " HEX block(s):");
                for (int i = 0; i < hex.Blocks.Count && i < 15; i++)
                {
                    HexBlock b = hex.Blocks[i];
                    sb.AppendLine("0x" + b.Address.ToString("X8") + " - 0x" + b.EndAddress.ToString("X8")
                                  + "  (" + b.Data.Length + " bytes)");
                }
                FlashTrace.Write("APP", "HEX file loaded: " + dlg.FileName + ", " + hex.Blocks.Count + " block(s)");
                foreach (FlashImage image in plan.Images)
                    FlashTrace.Write("APP", "  " + image.Partition.Name + " 0x" + image.StartAddress.ToString("X8") + " - 0x"
                                     + image.LastAddress.ToString("X8") + " (" + image.Length + " bytes), checksum 0x" + image.Checksum.ToString("X4"));
                MessageBox.Show(sb.ToString(), "HEX file loaded");
            }
            catch (Exception ex)
            {
                this.myNeoVI.hex = null;
                FlashTrace.Write("APP", "HEX file rejected: " + dlg.FileName + ": " + ex.Message);
                MessageBox.Show("Could not use this HEX file: " + ex.Message);
            }
        }

        private void UpdateNeoviChannels()
        {
            myNeoVI.setNeoviChannel(FTCommBus.CAN, icsneo40.NETID_HSCAN);
        }



    }
}
