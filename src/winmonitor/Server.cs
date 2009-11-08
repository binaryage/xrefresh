using System;
using System.Collections.Generic;
using System.Drawing;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using Newtonsoft.Json;
using System.IO;

namespace XRefresh
{
	// implemented thanks to:
	// http://www.codeguru.com/Csharp/Csharp/cs_network/sockets/article.php/c7695/
	public class Server
	{
        public static string SEPARATOR = "---XREFRESH-MESSAGE---";

		public class ClientMessage
		{
			public string command;
			public string text;
			public string type;
			public string agent;
			public string page;
			public string url;

			public ClientMessage()
			{
			}

			public ClientMessage(string command, string text)
			{
				this.command = command;
				this.text = text;
			}
		}

		public class ServerMessage
		{
			public string command;

			public ServerMessage(string command)
			{
				this.command = command;
			}
		}

		public class ServerMessageAbout : ServerMessage
		{
			public string version;
			public string agent; // implementation specific string e.g. "WinDrum"

			public ServerMessageAbout(string version, string agent)
				: base("AboutMe")
			{
				this.version = version;
				this.agent = agent;
			}
		}

		public class ServerMessageRefresh : ServerMessage
		{
			public class File
			{
				public File(string root, Model.Activity activity)
				{
					action = activity.type.ToString().ToLower();
					path1 = RelativePath(activity.path1, root);
					path2 = RelativePath(activity.path2, root);
				}

				private string RelativePath(string path, string root)
				{
					if (path != null && path.StartsWith(root)) return path.Substring(root.Length).TrimStart('\\');
					return path;
				}

				public string action;
				public string path1;
				public string path2;
			}

			public string date;
			public string time;
			public string root;
			public string name;
			public string type;
			public File[] files;
            public Dictionary<string, object> contents = new Dictionary<string, object>();

			public ServerMessageRefresh(Model.FoldersRow folder, bool positive)
				: base("DoRefresh")
			{
				lock (folder.activities)
				{
					DateTime d = DateTime.Now;
					date = d.ToShortDateString();
					time = d.ToLongTimeString();
					root = folder.Path;
					name = folder.Name;

					// <path, most significant activity>
					Dictionary<string, Model.Activity> ops = new Dictionary<string, Model.Activity>();
					while (folder.activities.Count > 0)
					{
						Model.Activity activity = folder.activities.Dequeue();
						string sourcePath = activity.path1;
						string destPath = activity.type == Model.ActivityType.Renamed ? activity.path2 : activity.path1;

						// here is quite difficult logic to explain
						// I try to track and accumulate file change events and keep most significant information about what happened to files
						//
						// for example Microsoft Visual Studio 200 does this during file save (CTRL+S):
						//   1) saves file into temporary file (I get 'created' event and possibly many 'changed' events)
						//   2) deletes old file
						//   3) renames temporary file to the original name (location)
						// I need here to track renames and keep up-to-date location
						// I also keep most significant event: (deleted|created)>renamed>changed
						// if there was delete and create in row, I then change the event type to 'Changed'

						Model.Activity lastActivity = null;
						if (ops.ContainsKey(sourcePath)) lastActivity = ops[sourcePath];
						if (ops.ContainsKey(destPath)) lastActivity = ops[destPath];
						if (lastActivity!=null)
						{
							// nonsense of modifying deleted file ? stay on deleted message
							if (lastActivity.type == Model.ActivityType.Deleted && activity.type == Model.ActivityType.Changed) activity = lastActivity;
							// some apps may delete file and then recreate it, instead of changing it's content
							if (lastActivity.type == Model.ActivityType.Deleted && (activity.type == Model.ActivityType.Renamed || activity.type == Model.ActivityType.Created))
							{
								activity.type = Model.ActivityType.Changed;
								activity.path1 = destPath;
								activity.path2 = "";
							}
							// creation is more significant than modification and renaming
							if (lastActivity.type == Model.ActivityType.Created && (activity.type == Model.ActivityType.Renamed || activity.type == Model.ActivityType.Changed)) activity = lastActivity;
							// renaming is more significant to modification
							if (lastActivity.type == Model.ActivityType.Renamed && activity.type == Model.ActivityType.Changed) activity = lastActivity;
							// purge last activity
							activity.passed = activity.passed || lastActivity.passed;
							if (ops.ContainsKey(sourcePath)) ops.Remove(sourcePath);
							if (ops.ContainsKey(destPath)) ops.Remove(destPath);
						}
						// store new activity
						ops.Add(destPath, activity);
					}

					int passedCount = 0;
					foreach (Model.Activity activity in ops.Values)
					{
						if (activity.passed) passedCount++;
					}

					files = new File[passedCount];
					int i = 0;
					foreach (Model.Activity activity in ops.Values)
					{
						if (activity.passed)
						{
							files[i] = new File(root, activity);
                            try // file reading may throw, because someone deletes the file or something, so we should be safe here
                            {
                                if (activity.path1.EndsWith(".css"))
                                {
                                    TextReader tr = new StreamReader(root + "\\" + files[i].path1);
                                    string content = tr.ReadToEnd();
                                    tr.Close();
                                    string key = files[i].path1;
                                    contents[key] = content;
                                }
                            }
                            catch (Exception)
                            {
                            }
							i++;
						}
					}
				}
			}

			internal void Log(Bitmap micon, string msg)
			{
				Pair<Bitmap, string>[] lines = new Pair<Bitmap, string>[files.Length];
				int counter = 0;
				foreach (File file in files)
				{
					Bitmap icon = null;
					string text = file.path1;
					switch (file.action)
					{
						case "created": icon = Properties.Resources.Created; break;
						case "deleted": icon = Properties.Resources.Deleted; break;
						case "changed": icon = Properties.Resources.Changed; break;
						case "renamed":
							icon = Properties.Resources.Renamed;
							text += " -> " + file.path2;
							break;
					}
					lines[counter] = new Pair<Bitmap, string>(icon, text);
					counter++;
				}
				ActivityLog.Current.AddEventLog(micon, msg + root + " [" + lines.Length.ToString() + "]", lines);
			}
		}


		public class ClientInfo
		{
			Socket socket;
			Server parent;
			byte[] dataBuffer = new byte[64*1024];
            String buffer;
            string reminder;

			public int id;
			public string type;
			public string agent;
			public string page;
			public string url;
			public bool muted;

			public ClientInfo(int id, Socket socket, Server parent)
			{
				this.id = id;
				this.socket = socket;
				this.parent = parent;
				this.muted = false;
				this.page = "";
				this.agent = "";
				this.type = "";
                this.reminder = "";
			}

			public string GetClientFriendlyName()
			{
				return string.Format("{0}[{1}]", Context.GetClientTypeString(type), id);
			}

			// this the call back function which will be invoked when the socket
			// detects any client writing of data on the stream
			public void OnDataReceived(IAsyncResult asyn)
			{
                try
                {
                    int iRx = 0;
                    // Complete the BeginReceive() asynchronous call by EndReceive() method
                    // which will return the number of characters written to the stream 
                    // by the client
                    iRx = socket.EndReceive(asyn);
                    if (iRx == 0)
                    {
                        // it seems client died
                        parent.RemoveClient(id);
                        return;
                    }

                    char[] chars = new char[iRx];
                    System.Text.Decoder d = System.Text.Encoding.UTF8.GetDecoder();
                    d.GetChars(dataBuffer, 0, iRx, chars, 0);
                    System.String data = new System.String(chars);
                    string[] separators = new string[] { XRefresh.Server.SEPARATOR };
                    string[] messages = data.Split(separators, StringSplitOptions.None);
                    messages[0] = this.reminder + messages[0]; // TODO: this is wrong, reminder should be extracted before decoding (bug case: packet fragmentation in the middle of UTF8 multichar code sequence)

                    for (int i = 0; i < messages.Length - 1; i++)
                    {
                        string messageJson = messages[i];
                        messageJson = messageJson.Replace("\0", ""); // HACK to get rid of \0 bytes
                        ClientMessage message = JsonConvert.DeserializeObject<ClientMessage>(messageJson);
                        ProcessMessage(message);
                    }
                    this.reminder = messages[messages.Length - 1];
                    // continue the waiting for data on the Socket
                    WaitForData();
                }
                catch (ObjectDisposedException e)
                {
                    Utils.LogException(GetClientFriendlyName() + " closed socket without notice.", e);
                    parent.RemoveClient(id); // TODO: ?
                }
                catch (SocketException e)
                {
                    // fire exception when browser crashed
                    Utils.LogException(GetClientFriendlyName() + " died without saying goodbye (crashed?)", e);
                    parent.RemoveClient(id); // TODO: ?
                }
                catch (Exception e)
                {
                    Utils.LogException(GetClientFriendlyName() + " thrown exception", e);
                    parent.RemoveClient(id); // TODO: ?
                }
			}

			private bool ProcessMessage(ClientMessage message)
			{
				string log;
				switch (message.command)
				{
					case "Hello":
						type = message.type;
						if (type == null) type = "?";
						agent = message.agent;
						if (agent == null) agent = "?";
						log = String.Format("{0}: connected", GetClientFriendlyName());
						ActivityLog.Current.AddEventLog(Context.GetClientTypeIcon(type), log, Utils.LogLine(Properties.Resources.Information, agent));
						// reply with AboutMe message
						AssemblyName name = Assembly.GetExecutingAssembly().GetName();
						ServerMessageAbout msg = new ServerMessageAbout(Utils.GetVersionString(), name.Name);
						SendMessage(msg);
						break;
					case "Bye":
						parent.RemoveClient(id); // unregister from parent
						log = String.Format("{0}: disconnected", GetClientFriendlyName());
						ActivityLog.Current.AddEventLog(Context.GetClientTypeIcon(type), log);
						return false;
					case "SetPage":
						page = message.page;
						if (page == null) page = "";
						url = message.url;
						if (url == null) url = "";
						if (page.Length > 0)
						{
							log = String.Format("{0}: changed page to '{1}'", GetClientFriendlyName(), page);
							ActivityLog.Current.AddEventLog(Context.GetClientTypeIcon(type), log, Utils.LogLine(Properties.Resources.Information, url));
						}
						break;
				}
				Context.Current.UpdateTrayIcon();
				return true;
			}

			public void SendMessage(ServerMessage message)
			{
				// serialize message to JSON
                char[] data = JsonConvert.SerializeObject(message).ToCharArray();

				// encode string into UTF-8
				System.Text.Encoder encoder = System.Text.Encoding.UTF8.GetEncoder();
				int bufferSize = encoder.GetByteCount(data, 0, data.Length, true);
                char[] separator = XRefresh.Server.SEPARATOR.ToCharArray();
                byte[] buffer = new byte[bufferSize + separator.Length];
				encoder.GetBytes(data, 0, data.Length, buffer, 0, true);
                for (int i = 0; i < separator.Length; i++)
                {
                    buffer[bufferSize + i] = (byte)separator[i];
                }
            
				// send as UTF-8 string
				socket.Send(buffer);
			}

			// start waiting for data from the client
			public void WaitForData()
			{
				try
				{
					// start receiving any data written by the connected client asynchronously
					socket.BeginReceive(dataBuffer, 0, dataBuffer.Length, SocketFlags.None, new AsyncCallback(OnDataReceived), this);
				}
				catch (SocketException e)
				{
					// fire exception when browser crashed
					Utils.LogException(GetClientFriendlyName() + " failed to receive message.", e);
					parent.RemoveClient(id); // TODO: ?
				}
			}

			public void Toggle()
			{
				lock (this)
				{
					muted = !muted;
					if (muted)
						Utils.LogInfo(string.Format("{0}: muted by user", GetClientFriendlyName()));
					else
						Utils.LogInfo(string.Format("{0}: enabled by user", GetClientFriendlyName()));
				}
			}

			public void OnToggle(object sender, EventArgs e)
			{
				Toggle();
			}
		}

		/////////////////////////////////////////////////////////////////////////////////

		public static Server Current;

		Socket socket;
		public Dictionary<int, ClientInfo> clients = new Dictionary<int, ClientInfo>();
		int lastClientID = 0;

		public Server()
		{
			Current = this;
		}

		public void Start()
		{
			try {
				Model.SettingsRow settings = Context.Model.GetSettings();

				// create the listening socket
				socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
				IPAddress address = settings.LocalhostOnly ? IPAddress.Loopback : IPAddress.Any;
				IPEndPoint ipLocal = new IPEndPoint(address, settings.Port);

				// bind to local IP Address
				socket.Bind(ipLocal);

				// start listening
				socket.Listen(4);

				// create the call back for any client connections
				socket.BeginAccept(new AsyncCallback(OnClientConnect), null);

				// add notification into log
				ActivityLog.Current.AddEventLog(Properties.Resources.Information, "Started listening for browser connections");
			}
			catch (Exception e)
			{
				Utils.LogException("Server start failed.", e);
			}
		}

		public void Stop()
		{
			// add notification into log
			ActivityLog.Current.AddEventLog(Properties.Resources.Information, "Ended listening for browser connections");
		}

		public void AddClient(int id, ClientInfo client)
		{
			lock (this)
			{
				clients[lastClientID] = client;
			}
			Context.Current.UpdateTrayIcon();
		}

		public void RemoveClient(int id)
		{
			lock (this)
			{
				clients.Remove(id);
			}
			Context.Current.UpdateTrayIcon();
		}

		private void OnClientConnect(IAsyncResult asyn)
		{
			try
			{
				// here we complete/end the BeginAccept() asynchronous call
				// by calling EndAccept() - which returns the reference to a new Socket object
				ClientInfo client = new ClientInfo(++lastClientID, socket.EndAccept(asyn), this);
				AddClient(lastClientID, client);

				// let the worker Socket do the further processing for the just connected client
				client.WaitForData();

				// since the main Socket is now free, it can go back and wait for
				// other clients who are attempting to connect
				socket.BeginAccept(new AsyncCallback(OnClientConnect), null);
			}
			catch (ObjectDisposedException e)
			{
				Utils.LogException("Socket has been unexpectedly closed during client connection.", e);
				RemoveClient(lastClientID); // TODO: ?
			}
			catch (SocketException e)
			{
				Utils.LogException("Client died without saying goodbye (crashed?)", e);
				RemoveClient(lastClientID); // TODO: ?
			}
		}

		public bool IsAnyClientEnabled()
		{
			lock (this)
			{
				foreach (KeyValuePair<int, ClientInfo> pair in clients)
				{
					if (!pair.Value.muted) return true;
				}
			}
			return false;
		}

		public bool IsAnyClientConnected()
		{
			lock (this)
			{
				return clients.Count > 0;
			}
		}

		internal bool ToggleClient(int id)
		{
			lock (this)
			{
				if (!clients.ContainsKey(id)) return false;
				clients[id].Toggle();
				return true;
			}
		}

		internal bool SendRefresh()
		{
			// prepare pending messages
			// note: this will dump all recorded activities
			List<ServerMessageRefresh> messages = Context.Model.PrepareRefreshMessages();

			bool somethingSent = false;
			lock (this)
			{
				// send every message to every client
				foreach (ServerMessageRefresh message in messages)
				{
					foreach (KeyValuePair<int, ClientInfo> pair in clients)
					{
						ClientInfo client = pair.Value;
						if (!client.muted)
						{
							client.SendMessage(message);
							Context.Current.IncreaseRefreshCounter();
							somethingSent = true;
						}
					}
				}
			}
			return somethingSent;
		}
	}
}
