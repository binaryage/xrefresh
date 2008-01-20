using System;
using System.Diagnostics;
using System.IO;
using System.Web;
using System.Windows.Forms;
using Zayko.Dialogs.UnhandledExceptionDlg;
using System.Threading;

namespace XRefresh
{
	static class Program
	{
		static CustomExceptionHandler exceptionHandler;

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
#if !DEBUG
			bool ok;
			Mutex mutex = new Mutex(true, "XRefreshMutex", out ok);
			if (!ok)
			{
				MessageBox.Show("Another instance of XRefresh is already running. See icons in tray-bar.", "Multiple instances", MessageBoxButtons.OK, MessageBoxIcon.Stop);
				return;
			}
#endif 
			ResetWorkingDirectory();
			
			Utils.DisableErrorReporting(Path.GetFileName(Application.ExecutablePath));
			SetupExceptionsHandler();
			
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			// instead of running a form, we run an ApplicationContext
			Application.Run(new Context());

			GC.KeepAlive(mutex); // important!
		}

		private static void ResetWorkingDirectory()
		{
			Directory.SetCurrentDirectory(Application.StartupPath);
		}

		private static void SetupExceptionsHandler()
		{
			// create new instance of UnhandledExceptionDlg:
			exceptionHandler = new CustomExceptionHandler();

			// uncheck "Restart App" check box by default:
			exceptionHandler.RestartApp = false;

			// implement your sending protocol here. You can use any information from System.Exception
			exceptionHandler.OnSendExceptionClick += delegate(object sender, SendExceptionClickEventArgs ar)
			{
				// user clicked on "Send Error Report" button:
				if (ar.SendExceptionDetails)
				{
					string email = "antonin@hildebrand.cz";
					string subject = "Bug report: " + ar.UnhandledException.Message;
					string body = "Enter your description here and send this mail to antonin@hildebrand.cz.\nThank you very much.\n\n---\nHere is stack trace:\n" + ar.UnhandledException.StackTrace;
					string message = string.Format("mailto:{0}?subject={1}&body={2}", email, HttpUtility.UrlPathEncode(subject), HttpUtility.UrlPathEncode(body));
					Process.Start(message);
				}

				// user wants to restart the app
				if (ar.RestartApp)
				{
					Process.Start(System.Windows.Forms.Application.ExecutablePath);
				}
			};
		}
	}
}