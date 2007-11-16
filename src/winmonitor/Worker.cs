using System;
using System.IO;
using System.Net.Sockets;
using System.Threading;

namespace XRefresh
{
	class Worker
	{
		public static Worker Current;
		Thread thread;
		int waitingTimeout;

		public Worker()
		{
			Current = this;

			waitingTimeout = Context.Model.GetSettings().WaitingTimeout;

			thread = new Thread(new ThreadStart(Proc));
			thread.Name = "Worker";
			thread.IsBackground = true;
			thread.Start();
		}

		public void Signal()
		{
			thread.Interrupt();
		}

		public void RefreshSettings()
		{
			waitingTimeout = Context.Model.GetSettings().WaitingTimeout;
		}

		private void Proc()
		{
			SendEcho();
			while (true)
			{
				bool signal = false;
				while (true)
				{
					try
					{
						Thread.Sleep(waitingTimeout); // wait for file system signal
					}
					catch (ThreadInterruptedException)
					{
						signal = true;
						if (!Context.Model.RunningOutOfSpace())
							continue; // let him wait little bit more, for possible next incoming events
					}

					if (signal) break; // was signaled and survived last {waitingTimeout}ms without signal
				}

				// we have got at least one signal and after that with at least {waitingTimeout}ms peace
				SendEvent();
			}
		}

		private void SendEvent()
		{
			Model.SettingsRow settings = Context.Model.GetSettings();
			Server server = Server.Current;

			bool somethingSent = server.SendRefresh();

			if (settings.FlashIconOnRefresh) Context.Current.FlashIcon(somethingSent);
			if (somethingSent)
			{
				// play sound if connected
				if (settings.PlaySoundOnRefresh)
				{
					PlaySound(settings.SoundFile);
				}
			}

			Utils.ReduceMemoryUsage();
		}

		private void PlaySound(string soundFile)
		{
			// sanitize path
			string path = soundFile;
			if (!Path.IsPathRooted(path))
			{
				path = System.Environment.CurrentDirectory + Path.DirectorySeparatorChar + path;
			}

			// play sound
			Sound sound = new Sound();
			sound.Play(path);
		}

		private void SendEcho()
		{
			// just ping already initialized slaves
			Model.SettingsRow settings = Context.Model.GetSettings();
			for (int i = 1; i <= settings.PortRange; i++)
			{
				Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
				try
				{
					socket.Connect(settings.SlaveHost, settings.Port - i);
					socket.Disconnect(false);
				}
				catch (Exception)
				{
					// just swallow it
				}
			}
		}
	}
}
