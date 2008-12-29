namespace XRefresh
{
    partial class FilterTester
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
            this.tableModel = new XPTable.Models.TableModel();
            this.columnModel = new XPTable.Models.ColumnModel();
            this.fileColumn = new XPTable.Models.ImageColumn();
            this.reasonColumn = new XPTable.Models.TextColumn();
            ((System.ComponentModel.ISupportInitialize)(this)).BeginInit();
            this.SuspendLayout();

            ((System.ComponentModel.ISupportInitialize)(this)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private XPTable.Models.TableModel tableModel;
        private XPTable.Models.ColumnModel columnModel;
        private XPTable.Models.ImageColumn fileColumn;
        private XPTable.Models.TextColumn reasonColumn;
    }
}
