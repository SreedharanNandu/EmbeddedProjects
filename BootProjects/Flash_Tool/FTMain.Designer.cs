namespace FlashTool
{
    partial class FTMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FTMain));
            this.Connect = new System.Windows.Forms.Button();
            this.Program = new System.Windows.Forms.Button();
            this.ProgressBar = new System.Windows.Forms.ProgressBar();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.Open = new System.Windows.Forms.Button();
            this.Progress = new System.Windows.Forms.Label();
            this.labelCAN = new System.Windows.Forms.Label();
            this.labelVersion = new System.Windows.Forms.Label();
            this.neoviStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.SuspendLayout();
            // 
            // Connect
            // 
            this.Connect.Location = new System.Drawing.Point(28, 31);
            this.Connect.Name = "Connect";
            this.Connect.Size = new System.Drawing.Size(88, 80);
            this.Connect.TabIndex = 0;
            this.Connect.Text = "Connect";
            this.Connect.UseVisualStyleBackColor = true;
            this.Connect.Click += new System.EventHandler(this.Connect_Click);
            // 
            // Program
            // 
            this.Program.Location = new System.Drawing.Point(783, 29);
            this.Program.Name = "Program";
            this.Program.Size = new System.Drawing.Size(86, 82);
            this.Program.TabIndex = 1;
            this.Program.Text = "Program";
            this.Program.UseVisualStyleBackColor = true;
            this.Program.Click += new System.EventHandler(this.Program_Click);
            // 
            // ProgressBar
            // 
            this.ProgressBar.BackColor = System.Drawing.SystemColors.Control;
            this.ProgressBar.ForeColor = System.Drawing.Color.DodgerBlue;
            this.ProgressBar.Location = new System.Drawing.Point(134, 88);
            this.ProgressBar.Name = "ProgressBar";
            this.ProgressBar.Size = new System.Drawing.Size(631, 23);
            this.ProgressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.ProgressBar.TabIndex = 2;
            this.ProgressBar.Visible = false;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(134, 31);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(576, 20);
            this.textBox2.TabIndex = 3;
            // 
            // Open
            // 
            this.Open.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Open.Location = new System.Drawing.Point(716, 31);
            this.Open.Name = "Open";
            this.Open.Size = new System.Drawing.Size(36, 24);
            this.Open.TabIndex = 4;
            this.Open.Text = ".......";
            this.Open.UseVisualStyleBackColor = true;
            this.Open.Click += new System.EventHandler(this.Open_Click);
            // 
            // Progress
            // 
            this.Progress.AutoEllipsis = true;
            this.Progress.AutoSize = true;
            this.Progress.Location = new System.Drawing.Point(134, 60);
            this.Progress.Name = "Progress";
            this.Progress.Size = new System.Drawing.Size(48, 13);
            this.Progress.TabIndex = 5;
            this.Progress.Text = "Progress";
            this.Progress.Visible = false;
            // 
            // labelCAN
            // 
            this.labelCAN.AutoSize = true;
            this.labelCAN.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.labelCAN.Location = new System.Drawing.Point(660, 63);
            this.labelCAN.Name = "labelCAN";
            this.labelCAN.Size = new System.Drawing.Size(85, 13);
            this.labelCAN.TabIndex = 6;
            this.labelCAN.Text = "CAN Status : Off";
            // 
            // labelVersion
            // 
            this.labelVersion.AutoEllipsis = true;
            this.labelVersion.Location = new System.Drawing.Point(28, 124);
            this.labelVersion.Name = "labelVersion";
            this.labelVersion.Size = new System.Drawing.Size(841, 18);
            this.labelVersion.TabIndex = 7;
            this.labelVersion.Text = "Tool";
            // 
            // neoviStatusLabel
            // 
            this.neoviStatusLabel.BackColor = System.Drawing.SystemColors.Control;
            this.neoviStatusLabel.Name = "neoviStatusLabel";
            this.neoviStatusLabel.Size = new System.Drawing.Size(49, 18);
            this.neoviStatusLabel.Text = "Status";
            // 
            // FTMain
            // 
            this.ClientSize = new System.Drawing.Size(900, 151);
            this.Controls.Add(this.labelVersion);
            this.Controls.Add(this.labelCAN);
            this.Controls.Add(this.Progress);
            this.Controls.Add(this.Open);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.ProgressBar);
            this.Controls.Add(this.Program);
            this.Controls.Add(this.Connect);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "FTMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "FlashTool - 1.0";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormMain_FormClosing);
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.HelpProvider helpProvider1;
        private System.Windows.Forms.Button buttonConnect;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBoxHeader;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.ToolStripStatusLabel neoviStatusLabel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button Connect;
        private System.Windows.Forms.Button Program;
        private System.Windows.Forms.ProgressBar ProgressBar;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button Open;
        private System.Windows.Forms.Label Progress;
        private System.Windows.Forms.Label labelCAN;
        private System.Windows.Forms.Label labelVersion;

    }
}

