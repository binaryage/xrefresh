using System;
using System.ComponentModel;
using System.Windows.Forms;

namespace XRefresh
{
	public partial class ScannerProgress : Form
	{
		BackgroundWorker worker;

		public ScannerProgress(string folder, BackgroundWorker worker)
		{
			this.worker = worker;
			InitializeComponent();
			UpdateFolder(folder);
			UpdateAction("");
		}

		public void UpdateFolder(string folder)
		{
			this.labelFolder.Text = folder;
		}

		public void UpdateAction(string action)
		{
			this.labelAction.Text = action;
		}

		private void buttonCancel_Click(object sender, EventArgs e)
		{
			worker.CancelAsync();
		}
	}
}