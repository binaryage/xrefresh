using System;
using System.Diagnostics;
using System.Web;
using System.Windows.Forms;

namespace XRefresh
{
	public partial class AboutDialog: Form
	{
		public AboutDialog()
		{
			InitializeComponent();
			labelVersion.Text += " " + Utils.GetVersionString();
		}

		private void buttonOK_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void linkLabelContact_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
		{
			string email = "antonin@hildebrand.cz";
			string subject = "Feedback";
			string body = "Enter your text here and send this mail to antonin@hildebrand.cz";
			string message = string.Format("mailto:{0}?subject={1}&body={2}", email, subject, body);
			Process.Start(message);
		}

		private void linkLabelSite_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
		{
			string message = "http://xrefresh.binaryage.com";
			Process.Start(message);
        }
	}
}