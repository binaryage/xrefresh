namespace XRefresh
{
    partial class PageNetwork
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PageNetwork));
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.panel1 = new System.Windows.Forms.Panel();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label2 = new System.Windows.Forms.Label();
			this.editPort = new XRefresh.PortEdit();
			this.editRemoteComputer = new System.Windows.Forms.TextBox();
			this.labelRemoteComputer = new System.Windows.Forms.Label();
			this.checkAcceptOnlyLocal = new System.Windows.Forms.CheckBox();
			this.panel2 = new System.Windows.Forms.Panel();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.checkForUpdates = new System.Windows.Forms.CheckBox();
			this.checkUsageStatistics = new System.Windows.Forms.CheckBox();
			this.toolTip = new System.Windows.Forms.ToolTip(this.components);
			this.tableLayoutPanel1.SuspendLayout();
			this.panel1.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 1;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.panel2, 0, 1);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 3;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 75F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 75F));
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
			this.panel1.Size = new System.Drawing.Size(384, 69);
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
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.editPort);
			this.groupBox1.Controls.Add(this.editRemoteComputer);
			this.groupBox1.Controls.Add(this.labelRemoteComputer);
			this.groupBox1.Controls.Add(this.checkAcceptOnlyLocal);
			this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox1.Location = new System.Drawing.Point(0, 0);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(384, 69);
			this.groupBox1.TabIndex = 21;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Browser Connections";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(278, 20);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(43, 13);
			this.label2.TabIndex = 26;
			this.label2.Text = "on port:";
			// 
			// editPort
			// 
			this.editPort.AllowInternalTab = false;
			this.editPort.AutoHeight = true;
			this.editPort.BackColor = System.Drawing.SystemColors.Window;
			this.editPort.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.editPort.FieldCount = 1;
			this.editPort.Location = new System.Drawing.Point(322, 17);
			this.editPort.Name = "editPort";
			this.editPort.ReadOnly = false;
			this.editPort.Size = new System.Drawing.Size(51, 20);
			this.editPort.TabIndex = 25;
			this.toolTip.SetToolTip(this.editPort, resources.GetString("editPort.ToolTip"));
			// 
			// editRemoteComputer
			// 
			this.editRemoteComputer.Location = new System.Drawing.Point(106, 42);
			this.editRemoteComputer.Name = "editRemoteComputer";
			this.editRemoteComputer.Size = new System.Drawing.Size(267, 20);
			this.editRemoteComputer.TabIndex = 23;
			this.toolTip.SetToolTip(this.editRemoteComputer, "Fill in host name for remote computer where are you going to run web browser.");
			// 
			// labelRemoteComputer
			// 
			this.labelRemoteComputer.AutoSize = true;
			this.labelRemoteComputer.Location = new System.Drawing.Point(13, 45);
			this.labelRemoteComputer.Name = "labelRemoteComputer";
			this.labelRemoteComputer.Size = new System.Drawing.Size(94, 13);
			this.labelRemoteComputer.TabIndex = 24;
			this.labelRemoteComputer.Text = "Remote computer:";
			this.toolTip.SetToolTip(this.labelRemoteComputer, "Fill in host name for remote computer where are you going to run web browser.");
			// 
			// checkAcceptOnlyLocal
			// 
			this.checkAcceptOnlyLocal.AutoSize = true;
			this.checkAcceptOnlyLocal.Location = new System.Drawing.Point(9, 19);
			this.checkAcceptOnlyLocal.Name = "checkAcceptOnlyLocal";
			this.checkAcceptOnlyLocal.Size = new System.Drawing.Size(197, 17);
			this.checkAcceptOnlyLocal.TabIndex = 17;
			this.checkAcceptOnlyLocal.Text = "Accept only local connections (safe)";
			this.toolTip.SetToolTip(this.checkAcceptOnlyLocal, "If your browser runs on the same machine check this for security.\r\nWe will then a" +
					"ccept only connections from localhost.");
			this.checkAcceptOnlyLocal.UseVisualStyleBackColor = true;
			this.checkAcceptOnlyLocal.CheckedChanged += new System.EventHandler(this.checkAcceptOnlyLocal_CheckedChanged);
			// 
			// panel2
			// 
			this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.panel2.Controls.Add(this.groupBox2);
			this.panel2.Location = new System.Drawing.Point(3, 78);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(384, 69);
			this.panel2.TabIndex = 1;
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.checkForUpdates);
			this.groupBox2.Controls.Add(this.checkUsageStatistics);
			this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox2.Enabled = false;
			this.groupBox2.Location = new System.Drawing.Point(0, 0);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(384, 69);
			this.groupBox2.TabIndex = 21;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Software Updates";
			// 
			// checkForUpdates
			// 
			this.checkForUpdates.AutoSize = true;
			this.checkForUpdates.Enabled = false;
			this.checkForUpdates.Location = new System.Drawing.Point(9, 19);
			this.checkForUpdates.Name = "checkForUpdates";
			this.checkForUpdates.Size = new System.Drawing.Size(113, 17);
			this.checkForUpdates.TabIndex = 20;
			this.checkForUpdates.Text = "Check for updates";
			this.toolTip.SetToolTip(this.checkForUpdates, "XRefresh will perform check for update every week. \r\nThe check is performed during computer inactivity at least 30 minutes after reboot.");
			this.checkForUpdates.UseVisualStyleBackColor = true;
			this.checkForUpdates.CheckedChanged += new System.EventHandler(this.checkForUpdates_CheckedChanged);
			// 
			// checkUsageStatistics
			// 
			this.checkUsageStatistics.AutoSize = true;
			this.checkUsageStatistics.Enabled = false;
			this.checkUsageStatistics.Location = new System.Drawing.Point(9, 42);
			this.checkUsageStatistics.Name = "checkUsageStatistics";
			this.checkUsageStatistics.Size = new System.Drawing.Size(259, 17);
			this.checkUsageStatistics.TabIndex = 19;
			this.checkUsageStatistics.Text = "Send usage statistics (c\'mon, it\'s just one number)";
			this.toolTip.SetToolTip(this.checkUsageStatistics, "XRefresh sends the number of successful refresh events.\r\nThis number is used for our web statistics saying \r\nhow much developers\' time around the globe\r\nhas been saved so far.");
			this.checkUsageStatistics.UseVisualStyleBackColor = true;
			// 
			// toolTip
			// 
			this.toolTip.AutoPopDelay = 20000;
			this.toolTip.InitialDelay = 500;
			this.toolTip.IsBalloon = true;
			this.toolTip.ReshowDelay = 100;
			// 
			// PageNetwork
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.tableLayoutPanel1);
			this.Name = "PageNetwork";
			this.Size = new System.Drawing.Size(390, 390);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.panel2.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox checkAcceptOnlyLocal;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.CheckBox checkUsageStatistics;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox checkForUpdates;
        private System.Windows.Forms.TextBox editRemoteComputer;
        private System.Windows.Forms.Label labelRemoteComputer;
        private System.Windows.Forms.ToolTip toolTip;
        private System.Windows.Forms.Label label2;
        private PortEdit editPort;

    }
}
