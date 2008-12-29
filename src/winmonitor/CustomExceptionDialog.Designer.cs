namespace Zayko.Dialogs.UnhandledExceptionDlg
{
    partial class CustomExceptionDialog
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
            if(disposing && (components != null))
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CustomExceptionDialog));
            this.panelTop = new System.Windows.Forms.Panel();
            this.labelTitle = new System.Windows.Forms.Label();
            this.panelDevider = new System.Windows.Forms.Panel();
            this.labelCaption = new System.Windows.Forms.Label();
            this.labelDescription = new System.Windows.Forms.Label();
            this.buttonNotSend = new System.Windows.Forms.Button();
            this.linkLabelData = new System.Windows.Forms.LinkLabel();
            this.checkBoxRestart = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.panelTop.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelTop
            // 
            this.panelTop.BackColor = System.Drawing.SystemColors.Window;
            this.panelTop.Controls.Add(this.label2);
            this.panelTop.Controls.Add(this.label1);
            this.panelTop.Controls.Add(this.labelTitle);
            this.panelTop.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelTop.Location = new System.Drawing.Point(0, 0);
            this.panelTop.Name = "panelTop";
            this.panelTop.Size = new System.Drawing.Size(402, 63);
            this.panelTop.TabIndex = 0;
            // 
            // labelTitle
            // 
            this.labelTitle.AutoSize = true;
            this.labelTitle.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTitle.Location = new System.Drawing.Point(159, 19);
            this.labelTitle.Name = "labelTitle";
            this.labelTitle.Size = new System.Drawing.Size(231, 13);
            this.labelTitle.TabIndex = 0;
            this.labelTitle.Text = "XRefresh application has crashed.";
            // 
            // panelDevider
            // 
            this.panelDevider.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelDevider.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelDevider.Location = new System.Drawing.Point(0, 63);
            this.panelDevider.Name = "panelDevider";
            this.panelDevider.Size = new System.Drawing.Size(402, 2);
            this.panelDevider.TabIndex = 1;
            // 
            // labelCaption
            // 
            this.labelCaption.AutoSize = true;
            this.labelCaption.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelCaption.Location = new System.Drawing.Point(13, 80);
            this.labelCaption.Name = "labelCaption";
            this.labelCaption.Size = new System.Drawing.Size(191, 13);
            this.labelCaption.TabIndex = 3;
            this.labelCaption.Text = "Please tell us about this problem";
            // 
            // labelDescription
            // 
            this.labelDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F);
            this.labelDescription.Location = new System.Drawing.Point(12, 105);
            this.labelDescription.Name = "labelDescription";
            this.labelDescription.Size = new System.Drawing.Size(387, 51);
            this.labelDescription.TabIndex = 4;
            this.labelDescription.Text = resources.GetString("labelDescription.Text");
            // 
            // buttonNotSend
            // 
            this.buttonNotSend.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonNotSend.Location = new System.Drawing.Point(315, 198);
            this.buttonNotSend.Name = "buttonNotSend";
            this.buttonNotSend.Size = new System.Drawing.Size(75, 23);
            this.buttonNotSend.TabIndex = 6;
            this.buttonNotSend.Text = "OK";
            this.buttonNotSend.UseVisualStyleBackColor = true;
            // 
            // linkLabelData
            // 
            this.linkLabelData.AutoSize = true;
            this.linkLabelData.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F);
            this.linkLabelData.Location = new System.Drawing.Point(13, 162);
            this.linkLabelData.Name = "linkLabelData";
            this.linkLabelData.Size = new System.Drawing.Size(265, 13);
            this.linkLabelData.TabIndex = 8;
            this.linkLabelData.TabStop = true;
            this.linkLabelData.Text = "Send me your error report and let me nail that bug down.";
            // 
            // checkBoxRestart
            // 
            this.checkBoxRestart.AutoSize = true;
            this.checkBoxRestart.Checked = true;
            this.checkBoxRestart.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxRestart.Location = new System.Drawing.Point(16, 202);
            this.checkBoxRestart.Name = "checkBoxRestart";
            this.checkBoxRestart.Size = new System.Drawing.Size(107, 17);
            this.checkBoxRestart.TabIndex = 5;
            this.checkBoxRestart.Text = "&Restart XRefresh";
            this.checkBoxRestart.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 7F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(196, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(194, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "We are still in beta, didn\'t we tell you? ;-)";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(12, 15);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(108, 31);
            this.label2.TabIndex = 2;
            this.label2.Text = "Ooops!";
            // 
            // UnhandledExDlgForm
            // 
            this.AcceptButton = this.buttonNotSend;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonNotSend;
            this.ClientSize = new System.Drawing.Size(402, 233);
            this.ControlBox = false;
            this.Controls.Add(this.checkBoxRestart);
            this.Controls.Add(this.linkLabelData);
            this.Controls.Add(this.buttonNotSend);
            this.Controls.Add(this.labelDescription);
            this.Controls.Add(this.labelCaption);
            this.Controls.Add(this.panelDevider);
            this.Controls.Add(this.panelTop);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "UnhandledExDlgForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "XRefresh Crash";
            this.TopMost = true;
            this.Load += new System.EventHandler(this.UnhandledExDlgForm_Load);
            this.panelTop.ResumeLayout(false);
            this.panelTop.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panelTop;
        private System.Windows.Forms.Panel panelDevider;
        private System.Windows.Forms.Label labelCaption;
        private System.Windows.Forms.Label labelDescription;
        private System.Windows.Forms.Button buttonNotSend;
        internal System.Windows.Forms.Label labelTitle;
        internal System.Windows.Forms.CheckBox checkBoxRestart;
        internal System.Windows.Forms.LinkLabel linkLabelData;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
    }
}