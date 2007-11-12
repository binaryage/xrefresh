using System;
using System.Diagnostics;
using System.IO;
using System.Web;
using System.Windows.Forms;
using Zayko.Dialogs.UnhandledExceptionDlg;

namespace XRefresh
{
	public class SingleInstanceTracker
	{
		bool isFirstInstance = true;
		
		public SingleInstanceTracker(string name)
		{
			// get the name of our process
			string proc = Process.GetCurrentProcess().ProcessName;
			// get the list of all processes by that name
			Process[] processes = Process.GetProcessesByName(proc);
			// if there is more than one process...
			isFirstInstance = !(processes.Length > 1);
		}
		
		public bool IsFirstInstance
		{
			get
			{
				return isFirstInstance;
			}
		}
	}

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
			SingleInstanceTracker tracker = new SingleInstanceTracker("XRefresh");
			if (!tracker.IsFirstInstance)
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