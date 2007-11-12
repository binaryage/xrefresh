using System;
using System.Windows.Forms;

namespace Zayko.Dialogs.UnhandledExceptionDlg
{
	public partial class CustomExceptionDialog: Form
	{

		public CustomExceptionDialog()
		{
			InitializeComponent();
		}

		private void UnhandledExDlgForm_Load(object sender, EventArgs e)
		{
			buttonNotSend.Focus();
		}
	}
}