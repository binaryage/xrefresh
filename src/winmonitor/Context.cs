using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Threading;
using System.Windows.Forms;
using Abhinaba.SysTray;

namespace XRefresh
{
	class Context : ApplicationContext
	{
		public static Context Current;
		NotifyIcon notifyIcon;
		Configuration configuration = new Configuration();
		ActivityLog activityLog = new ActivityLog();
		static Model model = new Model();
		Worker worker;
		Server server;
		Detector detector = new Detector();
		Menu menu;
		Icon icon = Properties.Resources.XRefreshDisconnected;
		System.Timers.Timer memTimer = new System.Timers.Timer();
		int mainThreadID = -1;
		SysTray sysTray;

		public static Model Model 
		{ 
			get { return model;  }
			set { model = value;  }
		}

		public int MainThreadID
		{
			get { return mainThreadID; }
		}

		public Context()
		{
			Current = this;
			Utils.CheckOrGenerateUniqueId();

			mainThreadID = Thread.CurrentThread.GetHashCode();

			memTimer.Interval = 1000; // execute after one second
			memTimer.Elapsed += new System.Timers.ElapsedEventHandler(memTimer_Elapsed);

			LoadSettings();
			model.Init();

			menu = new Menu();
			sysTray = new SysTray("", icon, menu);
			notifyIcon = sysTray.m_notifyIcon;
			notifyIcon.DoubleClick += new EventHandler(notifyIcon_DoubleClick);

			server = new Server();
			server.Start();

			menu.RebuildMenu();
			UpdateTrayIcon();

			worker = new Worker();
			model.Start();

			ThreadExit += new EventHandler(OnThreadExit);
			memTimer.Start();

			if (Utils.IsRunningFirstTime())
			{
				// open help page
				try
				{
					Process.Start(System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath) + "\\Help.html");
				}
				catch (Exception)
				{
					// opening help file was not so important ...
				}
				if (MessageBox.Show("XRefresh is running for the first time.\nDo you want to configure it now? [recommended]\n\nNote:\nYou can do it anytime later. Just right click on XRefresh icon in the traybar and select 'Configuration' from context menu.", "XRefresh Configuration during first start", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
				{
					ShowConfiguration(this, new EventArgs());
				}
			}
		}

		void memTimer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
		{
			memTimer.Interval = 10000; // 10 seconds
			Utils.ReduceMemoryUsage();
		}

		void notifyIcon_DoubleClick(object sender, EventArgs e)
		{
			ShowConfiguration(sender, e);
		}

		void OnThreadExit(object sender, EventArgs e)
		{
			SaveSettings();

			// we must manually tidy up and remove the icon before we exit
			// otherwise it will be left behind until the user mouses over
			notifyIcon.Visible = false;
		}

		public void LoadSettings()
		{
			model.Load(Utils.GetSettingsPath());
		}

		public void SaveSettings()
		{
			model.Save(Utils.GetSettingsPath());
			// renew startup settings in windows registry
			UpdateStartupSettings();
		}

		private void UpdateStartupSettings()
		{
			if (model.GetSettings().RunOnWindowsStartup)
			{
				if (!Utils.IsStartupPathPresent(Application.ExecutablePath))
				{
					Utils.SetStartupPath(Application.ExecutablePath);
				}
			}
			else
			{
				Utils.RemoveStartupPath();
			}
		}

		public void ShowConfiguration(object sender, EventArgs e)
		{
			Debug.Assert(mainThreadID == Thread.CurrentThread.GetHashCode());
			if (configuration.Visible)
			{
				if (configuration.WindowState == FormWindowState.Minimized) configuration.WindowState = FormWindowState.Normal;
				return;
			}
			if (configuration.IsDisposed) configuration = new Configuration();
			configuration.LoadConfiguration();
			Activate(configuration);
		}

		public void ShowActivityLog(object sender, EventArgs e)
		{
			Debug.Assert(mainThreadID == Thread.CurrentThread.GetHashCode());
			if (activityLog.Visible)
			{
				if (activityLog.WindowState == FormWindowState.Minimized) activityLog.WindowState = FormWindowState.Normal;
				return;
			}
			if (activityLog.IsDisposed) activityLog = new ActivityLog();
			Activate(activityLog);
		}

		private void Activate(Form window)
		{
			// if we are already showing the window => focus it
			if (window.Visible)
			{
				window.Activate();
				if (window.WindowState == FormWindowState.Minimized) window.WindowState = FormWindowState.Normal;
			}
			else
			{
				window.Show();
			}
		}

		public void Exit(object sender, EventArgs e)
		{
			configuration.Close();
			activityLog.Close();
			try
			{
				Application.Exit();
			}
			catch (Exception)
			{ }
		}

		public void Help(object sender, EventArgs e)
		{
		}

		public void About(object sender, EventArgs e)
		{
			AboutDialog aboutDialog = new AboutDialog();
			aboutDialog.ShowDialog();
		}

		public void UpdateTrayIcon()
		{
			lock (server)
			{
				string tooltip = "XRefresh - ";
				if (server.IsAnyClientConnected())
				{
					SetIcon(Properties.Resources.XRefreshConnected);
					if (server.clients.Count == 1)
					{
						Dictionary<int, Server.ClientInfo>.Enumerator enumerator = server.clients.GetEnumerator();
						enumerator.MoveNext();
						tooltip += String.Format("Connected to {0}", GetClientTypeString(enumerator.Current.Value.type));
					}
					else
						tooltip += String.Format("Connected to {0} browsers", server.clients.Count);
				}
				else
				{
					SetIcon(Properties.Resources.XRefreshDisconnected);
					tooltip += "Waiting for browser connection ...";
				}
				notifyIcon.Text = tooltip;
			}
		}

		public void SetIcon(Icon icon)
		{
			sysTray.m_DefaultIcon = icon;
			sysTray.StopAnimation();
			notifyIcon.Icon = icon;
		}

		public static string GetClientTypeString(string type)
		{
			return type;
		}

		public static Bitmap GetClientTypeIcon(string type)
		{
			switch (type)
			{
				case "Firefox": return Properties.Resources.Firefox;
				case "Internet Explorer": return Properties.Resources.InternetExplorer;
				case "Safari": return Properties.Resources.Safari;
				case "Opera": return Properties.Resources.Opera;
			}
			return null;
		}

		public static int GetClientTypeIndex(string type)
		{
			switch (type)
			{
				case "Firefox": return 6;
				case "Internet Explorer": return 5;
				case "Safari": return 7;
				case "Opera": return 8;
			}
			return 0;
		}

		public void FlashIcon(bool active)
		{
			if (active)
				sysTray.SetAnimationClip(Properties.Resources.AnimA16);
			else
			{
				if (server.IsAnyClientConnected())
				{
					sysTray.SetAnimationClip(Properties.Resources.AnimI16);
				}
				else
				{
					sysTray.SetAnimationClip(Properties.Resources.AnimD16);
				}
			}
			sysTray.StartAnimation(50, 0);
		}
	}
}
