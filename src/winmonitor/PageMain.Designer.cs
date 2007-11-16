namespace XRefresh
{
    partial class PageMain
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PageMain));
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.panel1 = new System.Windows.Forms.Panel();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.buttonSettingsFile = new System.Windows.Forms.Button();
			this.editSettingsFile = new System.Windows.Forms.TextBox();
			this.labelSettingsFile = new System.Windows.Forms.Label();
			this.checkSaveSettings = new System.Windows.Forms.CheckBox();
			this.checkRunOnStartup = new System.Windows.Forms.CheckBox();
			this.panel2 = new System.Windows.Forms.Panel();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.buttonSoundFile = new System.Windows.Forms.Button();
			this.editSoundFile = new System.Windows.Forms.TextBox();
			this.labelSoundFile = new System.Windows.Forms.Label();
			this.checkFlashIconOnRefresh = new System.Windows.Forms.CheckBox();
			this.checkPlaySoundOnRefresh = new System.Windows.Forms.CheckBox();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.checkEnableLogging = new System.Windows.Forms.CheckBox();
			this.label3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.editWaitTimeout = new XRefresh.TimeoutEdit();
			this.toolTip = new System.Windows.Forms.ToolTip(this.components);
			this.tableLayoutPanel1.SuspendLayout();
			this.panel1.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 1;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.panel2, 0, 1);
			this.tableLayoutPanel1.Controls.Add(this.groupBox3, 0, 2);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 4;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 100F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 100F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 70F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel1.Size = new System.Drawing.Size(390, 390);
			this.tableLayoutPanel1.TabIndex = 0;
			// 
			// panel1
			// 
			this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.panel1.Controls.Add(this.label1);
			this.panel1.Controls.Add(this.groupBox1);
			this.panel1.Location = new System.Drawing.Point(3, 3);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(384, 94);
			this.panel1.TabIndex = 0;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(17, 95);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(60, 13);
			this.label1.TabIndex = 20;
			this.label1.Text = "Sound File:";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.buttonSettingsFile);
			this.groupBox1.Controls.Add(this.editSettingsFile);
			this.groupBox1.Controls.Add(this.labelSettingsFile);
			this.groupBox1.Controls.Add(this.checkSaveSettings);
			this.groupBox1.Controls.Add(this.checkRunOnStartup);
			this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox1.Location = new System.Drawing.Point(0, 0);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(384, 94);
			this.groupBox1.TabIndex = 21;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Startup && Shutdown";
			// 
			// buttonSettingsFile
			// 
			this.buttonSettingsFile.Location = new System.Drawing.Point(349, 64);
			this.buttonSettingsFile.Name = "buttonSettingsFile";
			this.buttonSettingsFile.Size = new System.Drawing.Size(24, 22);
			this.buttonSettingsFile.TabIndex = 25;
			this.buttonSettingsFile.Text = "...";
			this.buttonSettingsFile.UseVisualStyleBackColor = true;
			this.buttonSettingsFile.Click += new System.EventHandler(this.buttonSettingsFile_Click);
			// 
			// editSettingsFile
			// 
			this.editSettingsFile.Location = new System.Drawing.Point(80, 65);
			this.editSettingsFile.Name = "editSettingsFile";
			this.editSettingsFile.Size = new System.Drawing.Size(269, 20);
			this.editSettingsFile.TabIndex = 23;
			this.toolTip.SetToolTip(this.editSettingsFile, "The location of a settings file. \r\nSettings will be saved in XML format.");
			// 
			// labelSettingsFile
			// 
			this.labelSettingsFile.AutoSize = true;
			this.labelSettingsFile.Location = new System.Drawing.Point(13, 68);
			this.labelSettingsFile.Name = "labelSettingsFile";
			this.labelSettingsFile.Size = new System.Drawing.Size(64, 13);
			this.labelSettingsFile.TabIndex = 24;
			this.labelSettingsFile.Text = "Settings file:";
			// 
			// checkSaveSettings
			// 
			this.checkSaveSettings.AutoSize = true;
			this.checkSaveSettings.Location = new System.Drawing.Point(9, 42);
			this.checkSaveSettings.Name = "checkSaveSettings";
			this.checkSaveSettings.Size = new System.Drawing.Size(177, 17);
			this.checkSaveSettings.TabIndex = 18;
			this.checkSaveSettings.Text = "Save settings to default location";
			this.checkSaveSettings.UseVisualStyleBackColor = true;
			this.checkSaveSettings.CheckedChanged += new System.EventHandler(this.checkSaveSettings_CheckedChanged);
			// 
			// checkRunOnStartup
			// 
			this.checkRunOnStartup.AutoSize = true;
			this.checkRunOnStartup.Location = new System.Drawing.Point(9, 19);
			this.checkRunOnStartup.Name = "checkRunOnStartup";
			this.checkRunOnStartup.Size = new System.Drawing.Size(143, 17);
			this.checkRunOnStartup.TabIndex = 17;
			this.checkRunOnStartup.Text = "Run on Windows startup";
			this.toolTip.SetToolTip(this.checkRunOnStartup, "When checked, XRefresh will start automatically during windows startup.");
			this.checkRunOnStartup.UseVisualStyleBackColor = true;
			// 
			// panel2
			// 
			this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.panel2.Controls.Add(this.groupBox2);
			this.panel2.Location = new System.Drawing.Point(3, 103);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(384, 94);
			this.panel2.TabIndex = 1;
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.buttonSoundFile);
			this.groupBox2.Controls.Add(this.editSoundFile);
			this.groupBox2.Controls.Add(this.labelSoundFile);
			this.groupBox2.Controls.Add(this.checkFlashIconOnRefresh);
			this.groupBox2.Controls.Add(this.checkPlaySoundOnRefresh);
			this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox2.Location = new System.Drawing.Point(0, 0);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(384, 94);
			this.groupBox2.TabIndex = 21;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Refresh Feedback";
			// 
			// buttonSoundFile
			// 
			this.buttonSoundFile.Location = new System.Drawing.Point(349, 65);
			this.buttonSoundFile.Name = "buttonSoundFile";
			this.buttonSoundFile.Size = new System.Drawing.Size(24, 22);
			this.buttonSoundFile.TabIndex = 22;
			this.buttonSoundFile.Text = "...";
			this.buttonSoundFile.UseVisualStyleBackColor = true;
			this.buttonSoundFile.Click += new System.EventHandler(this.buttonSoundFile_Click);
			// 
			// editSoundFile
			// 
			this.editSoundFile.Location = new System.Drawing.Point(80, 66);
			this.editSoundFile.Name = "editSoundFile";
			this.editSoundFile.Size = new System.Drawing.Size(269, 20);
			this.editSoundFile.TabIndex = 20;
			this.toolTip.SetToolTip(this.editSoundFile, "Location of sound file.");
			// 
			// labelSoundFile
			// 
			this.labelSoundFile.AutoSize = true;
			this.labelSoundFile.Location = new System.Drawing.Point(20, 69);
			this.labelSoundFile.Name = "labelSoundFile";
			this.labelSoundFile.Size = new System.Drawing.Size(57, 13);
			this.labelSoundFile.TabIndex = 21;
			this.labelSoundFile.Text = "Sound file:";
			// 
			// checkFlashIconOnRefresh
			// 
			this.checkFlashIconOnRefresh.AutoSize = true;
			this.checkFlashIconOnRefresh.Location = new System.Drawing.Point(9, 19);
			this.checkFlashIconOnRefresh.Name = "checkFlashIconOnRefresh";
			this.checkFlashIconOnRefresh.Size = new System.Drawing.Size(144, 17);
			this.checkFlashIconOnRefresh.TabIndex = 20;
			this.checkFlashIconOnRefresh.Text = "Flash tray icon on refresh";
			this.toolTip.SetToolTip(this.checkFlashIconOnRefresh, "Signal refresh by changing icon for a little moment during refresh.");
			this.checkFlashIconOnRefresh.UseVisualStyleBackColor = true;
			// 
			// checkPlaySoundOnRefresh
			// 
			this.checkPlaySoundOnRefresh.AutoSize = true;
			this.checkPlaySoundOnRefresh.Location = new System.Drawing.Point(9, 42);
			this.checkPlaySoundOnRefresh.Name = "checkPlaySoundOnRefresh";
			this.checkPlaySoundOnRefresh.Size = new System.Drawing.Size(130, 17);
			this.checkPlaySoundOnRefresh.TabIndex = 19;
			this.checkPlaySoundOnRefresh.Text = "Play Sound on refresh";
			this.toolTip.SetToolTip(this.checkPlaySoundOnRefresh, "Signal refresh by playing sound.");
			this.checkPlaySoundOnRefresh.UseVisualStyleBackColor = true;
			this.checkPlaySoundOnRefresh.CheckedChanged += new System.EventHandler(this.checkPlaySoundOnRefresh_CheckedChanged);
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.checkEnableLogging);
			this.groupBox3.Controls.Add(this.label3);
			this.groupBox3.Controls.Add(this.label2);
			this.groupBox3.Controls.Add(this.editWaitTimeout);
			this.groupBox3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox3.Location = new System.Drawing.Point(3, 203);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(384, 64);
			this.groupBox3.TabIndex = 2;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Advanced";
			// 
			// checkEnableLogging
			// 
			this.checkEnableLogging.AutoSize = true;
			this.checkEnableLogging.Location = new System.Drawing.Point(9, 45);
			this.checkEnableLogging.Name = "checkEnableLogging";
			this.checkEnableLogging.Size = new System.Drawing.Size(100, 17);
			this.checkEnableLogging.TabIndex = 23;
			this.checkEnableLogging.Text = "Enable Logging";
			this.toolTip.SetToolTip(this.checkEnableLogging, "You may want to enable logging for program diagnostics. When enabled, the Event l" +
					"og  is available from context menu on tray icon.");
			this.checkEnableLogging.UseVisualStyleBackColor = true;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(132, 22);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(61, 13);
			this.label3.TabIndex = 2;
			this.label3.Text = "miliseconds";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(8, 22);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(83, 13);
			this.label2.TabIndex = 1;
			this.label2.Text = "Waiting timeout:";
			this.toolTip.SetToolTip(this.label2, resources.GetString("label2.ToolTip"));
			// 
			// editWaitTimeout
			// 
			this.editWaitTimeout.AllowInternalTab = false;
			this.editWaitTimeout.AutoHeight = true;
			this.editWaitTimeout.BackColor = System.Drawing.SystemColors.Window;
			this.editWaitTimeout.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.editWaitTimeout.FieldCount = 1;
			this.editWaitTimeout.Location = new System.Drawing.Point(92, 19);
			this.editWaitTimeout.Name = "editWaitTimeout";
			this.editWaitTimeout.ReadOnly = false;
			this.editWaitTimeout.Size = new System.Drawing.Size(38, 20);
			this.editWaitTimeout.TabIndex = 0;
			this.toolTip.SetToolTip(this.editWaitTimeout, resources.GetString("editWaitTimeout.ToolTip"));
			// 
			// toolTip
			// 
			this.toolTip.AutoPopDelay = 20000;
			this.toolTip.InitialDelay = 500;
			this.toolTip.IsBalloon = true;
			this.toolTip.ReshowDelay = 100;
			// 
			// PageMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.tableLayoutPanel1);
			this.Name = "PageMain";
			this.Size = new System.Drawing.Size(390, 390);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.panel2.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox checkRunOnStartup;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.TextBox editSoundFile;
        private System.Windows.Forms.CheckBox checkPlaySoundOnRefresh;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label labelSoundFile;
        private System.Windows.Forms.CheckBox checkFlashIconOnRefresh;
        private System.Windows.Forms.CheckBox checkSaveSettings;
        private System.Windows.Forms.Button buttonSoundFile;
        private System.Windows.Forms.Button buttonSettingsFile;
        private System.Windows.Forms.TextBox editSettingsFile;
        private System.Windows.Forms.Label labelSettingsFile;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private TimeoutEdit editWaitTimeout;
        private System.Windows.Forms.ToolTip toolTip;
		private System.Windows.Forms.CheckBox checkEnableLogging;

    }
}
