using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;

namespace XRefresh
{
	partial class Model
	{
		const int MAX_PENDING_ACTIVITIES = 1000;

		public enum ActivityType
		{
			// note: the order is important for filtering in ServerMessageRefresh
			Deleted = 1, // the deletion of a file
			Created = 2, // the creation of a file
			Changed = 3, // the change of a file
			Renamed = 4, // the renaming of a file
		}

		public class Activity
		{
			public ActivityType type;
			public DateTime time;
			public string path1;
			public string path2;
			public bool passed;
			public string reason;

			public Activity(ActivityType type, DateTime time, string path1, string path2, bool passed, string reason)
			{
				this.type = type;
				this.time = time;
				this.path1 = path1;
				this.path2 = path2;
				this.reason = reason;
				this.passed = passed;
			}

			public static bool operator <(Activity a, Activity b)
			{
				return a.type < b.type;
			}

			public static bool operator >(Activity a, Activity b)
			{
				return a.type > b.type;
			}
		}

		partial class FoldersRow
		{
			// we hold one watcher for each folder
			public FileSystemWatcher watcher;

			// queue for pending activities
			public Queue<Activity> activities = new Queue<Activity>();

			public bool positive;

			public override string ToString()
			{
				return Name;
			}

			public void Init()
			{
				lock (this)
				{
					positive = false;
					try {
						watcher = CreateWatcher();
					}
					catch (ArgumentException e) {
						// add error into log
						ActivityLog.Current.AddEventLog(Properties.Resources.Error, e.Message);
						watcher = null;
					}
				}
			}

			public void Start()
			{
				lock (this)
				{
					if (!Enabled) return;
					if (watcher == null) return;
					watcher.EnableRaisingEvents = true;
				}
			}

			public void Stop()
			{
				lock (this)
				{
					if (watcher == null) return;
					watcher.EnableRaisingEvents = false;
				}
			}

			public bool HasWatcher(FileSystemWatcher w)
			{
				lock (this)
				{
					if (watcher == null) return false;
					if (object.ReferenceEquals(w, watcher)) return true;
				}
				return false;
			}

			public Server.ServerMessageRefresh PrepareRefreshMessage()
			{
				Server.ServerMessageRefresh message;
				lock (this)
				{
					if (activities.Count == 0) return null;
					message = new Server.ServerMessageRefresh(this, positive);
					ClearActivities();
					return message;
				}
			}

			private void ClearActivities()
			{
				activities.Clear();
				positive = false;
			}

			public void AddActivity(Activity activity)
			{
				lock (this)
				{
					// test for too many activities (worker is broken or busy ?)
					if (activities.Count > MAX_PENDING_ACTIVITIES)
					{
						throw new ModelException("Activity queue is full. Worker thread is busy. Some file activity may be not detected.");
					}
					activities.Enqueue(activity);
					if (activity.passed) positive = true;
				}
			}

			public int GetPositiveActivitiesCount()
			{
				lock (this)
				{
					int count = 0;
					foreach (Activity activity in activities)
					{
						if (activity.passed) count++;
					}
					return count;
				}
			}

			protected FileSystemWatcher CreateWatcher()
			{
				// create a new FileSystemWatcher and set its properties
				FileSystemWatcher watcher = new FileSystemWatcher();
				watcher.Path = Path;
				watcher.NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.FileName | NotifyFilters.DirectoryName;
				watcher.Filter = "*"; // we perform filtering on our own
				watcher.IncludeSubdirectories = true;

				// add event handlers
				watcher.Changed += new FileSystemEventHandler(OnChanged);
				watcher.Created += new FileSystemEventHandler(OnCreated);
				watcher.Deleted += new FileSystemEventHandler(OnDeleted);
				watcher.Renamed += new RenamedEventHandler(OnRenamed);

				return watcher;
			}

			private static void OnChanged(object source, FileSystemEventArgs e)
			{
				// skip events about "changed" directories
				if (Directory.Exists(e.FullPath)) return;

				CreateActivity(source, ActivityType.Changed, e.FullPath);
			}

			private static void OnCreated(object source, FileSystemEventArgs e)
			{
				CreateActivity(source, ActivityType.Created, e.FullPath);
			}

			private static void OnDeleted(object source, FileSystemEventArgs e)
			{
				CreateActivity(source, ActivityType.Deleted, e.FullPath);
			}

			private static void OnRenamed(object source, RenamedEventArgs e)
			{
				CreateActivity(source, ActivityType.Renamed, e.OldFullPath, e.FullPath);
			}

			private static void CreateActivity(object source, ActivityType type, string path1)
			{
				CreateActivity(source, type, path1, null);
			}

			private static void CreateActivity(object source, ActivityType type, string path1, string path2)
			{
				try 
				{
					// find relevant folder
					FoldersRow folder = Context.Model.FindFolder(source as FileSystemWatcher);
					if (folder == null) throw new Exception("Expected folder not found for activity: " + path1);

					// test folder ignore filter
					MatchReason reason = new MatchReason();
					bool passed = folder.PassesFilters(path1, reason);
					if (type == Model.ActivityType.Renamed)
					{
						// when renaming be more tolerant and see also final names
						passed = passed || folder.PassesFilters(path2, reason);
					}

					// create and register activity
					Activity activity = new Activity(type, DateTime.Now, path1, path2, passed, reason.text);
					folder.AddActivity(activity);

					// signal worker to process new activity
					Worker.Current.Signal();
				}
				catch (Exception e)
				{
					// add error into log
					ActivityLog.Current.AddEventLog(Properties.Resources.Error, e.Message);
				}
			}

			public bool PassesLocalFilters(string path, MatchReason reason)
			{
				if (PassesLocalIncludes(path, reason)) return true;
				return PassesLocalExcludes(path, reason);
			}

			public bool PassesFilters(string path, MatchReason reason)
			{
				Model model = Table.DataSet as Model;
				if (model.PassesGlobalIncludes(path, reason)) return true;
				if (!model.PassesGlobalExcludes(path, reason)) return false;
				if (PassesLocalIncludes(path, reason)) return true;
				return PassesLocalExcludes(path, reason);
			}

			public bool PassesLocalIncludes(string path, MatchReason reason)
			{
				IncludeFiltersRow[] includes = GetIncludeFiltersRows();

				// it must match at least one global include
				foreach (IncludeFiltersRow include in includes)
				{
					FileMask wildcard = new FileMask(include.Mask);
					if (wildcard.IsMatch(path))
					{
						reason.Set(include.Mask, MatchReason.Status.Included);
						return true;
					}
				}

				// didn't match any include
				return false;
			}

			public bool PassesLocalExcludes(string path, MatchReason reason)
			{
				ExcludeFiltersRow[] excludes = GetExcludeFiltersRows();

				// it must not match any global exclude
				foreach (ExcludeFiltersRow exclude in excludes)
				{
					FileMask wildcard = new FileMask(exclude.Mask);
					if (wildcard.IsMatch(path))
					{
						reason.Set(exclude.Mask, MatchReason.Status.Excluded);
						return false;
					}
				}

				// passed excludes
				return true;
			}

			public void ClearIncludes()
			{
				IncludeFiltersRow[] includes = GetIncludeFiltersRows();

				// it must match at least one global include
				foreach (IncludeFiltersRow include in includes)
				{
					include.Delete();
				}
			}

			public void ClearExcludes()
			{
				ExcludeFiltersRow[] excludes = GetExcludeFiltersRows();

				// it must not match any global exclude
				foreach (ExcludeFiltersRow exclude in excludes)
				{
					exclude.Delete();
				}
			}

			internal bool RunningOutOfSpace()
			{
				if (activities.Count > MAX_PENDING_ACTIVITIES / 2) return true;
				return false;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////

		public delegate void FolderIteratorDelegate(FoldersRow folder);

		public void CreateDefaultSettings()
		{
			Settings.Clear();
			SettingsRow settings = Settings.NewSettingsRow();
			Settings.AddSettingsRow(settings);

			GlobalExcludeFilters.Clear();
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow(".svn", "subversion directories");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("_svn", "subversion directories");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("/.", "hidden files from UNIX");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("/System Volume Information", "system files");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("Local Settings/Temporary Internet Files", "temporary files");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.zip", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.rar", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.7z", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.pak", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.arj", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.cab", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.jar", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.gz", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.gz2", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.ace", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.lha", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.uc2", "archive");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.*~", "temp files");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.bak", "backup files");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.tmp", "temp files");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("/CVS", "CVS folders");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.#*", "");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.%*", "");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.DS_Store", "Desktop Services Store");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.pyo", "Python object files");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.o", "object files");
			GlobalExcludeFilters.AddGlobalExcludeFiltersRow("*.obj", "object files");

			GlobalIncludeFilters.Clear();
		}

		/// <summary>
		/// Loads model state from XML file
		/// </summary>
		/// <param name="path">Path of XML file to read</param>
		public void Load(string path)
		{
			if (!new FileInfo(path).Exists)
			{
				CreateDefaultSettings();
				ActivityLog.Current.AddEventLog(Properties.Resources.Error, "Settings file is missing (" + path + "). Reverting to default settings.");
				return;
			}

			lock (this)
			{
				try {
					ReadXml(path, System.Data.XmlReadMode.IgnoreSchema);
				}
				catch (Exception e)
				{
					ActivityLog.Current.AddEventLog(Properties.Resources.Error, "Unable to load settings (" + path + "): " + e.Message);
					CreateDefaultSettings();
					return;
				}

				if (Settings.Count == 0)
				{
					ActivityLog.Current.AddEventLog(Properties.Resources.Error, "Loaded settings table is empty (" + path + "). Revertig to default settings.");
				}
			}
		}

		public void Init()
		{
			lock (this)
			{
				// add info into log
				ActivityLog.Current.AddEventLog(Properties.Resources.Information, "Starting filesystem watchers ...");
				foreach (Model.FoldersRow folder in Folders)
				{
					folder.Init();
				}
			}
		}

		/// <summary>
		/// Saves model state into XML file
		/// </summary>
		/// <param name="path">Path of XML file to be overwritten</param>
		public void Save(string path)
		{
			try
			{
				lock (this)
				{
					WriteXml(path, System.Data.XmlWriteMode.IgnoreSchema);
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show(String.Format("Unable to save config file: {0}\nError: {1}", path, ex.Message), "Save failed", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
			}
		}

		/// <summary>
		/// Stops all folders from reporting file system activity
		/// </summary>
		public void Stop()
		{
			lock (this)
			{
				// add info into log
				ActivityLog.Current.AddEventLog(Properties.Resources.Information, "Halting filesystem watchers ...");
				foreach (Model.FoldersRow folder in Folders)
				{
					folder.Stop();
				}
			}
		}

		/// <summary>
		/// Starts all folders to be reporting file system activity
		/// </summary>
		public void Start()
		{
			lock (this)
			{
				foreach (Model.FoldersRow folder in Folders)
				{
					folder.Start();
				}
			}
		}

		public void IterateFolders(FolderIteratorDelegate it)
		{
			lock (this)
			{
				foreach (Model.FoldersRow folder in Folders)
				{
					it.Invoke(folder);
				}
			}
		}

		public FoldersRow AddFolder(string name, string path, string type)
		{
			FoldersRow folder = Folders.NewFoldersRow();
			folder.Name = name;
			folder.Path = path;
			folder.Type = type;
			lock (this)
			{
				Folders.AddFoldersRow(folder);
				Scanner scanner = Detector.Current.GetScanner(type);
				if (scanner != null)
				{
					scanner.SetFolder(folder);
					scanner.SetupFilters();
				}
			}
			return folder;
		}

		public FoldersRow FindFolder(FileSystemWatcher watcher)
		{
			lock (this)
			{
				foreach (Model.FoldersRow folder in Folders)
				{
					if (folder.HasWatcher(watcher)) return folder;
				}
			}
			return null;
		}

		public SettingsRow GetSettings()
		{
			lock (this)
			{
				return Settings[0];
			}
		}

		public List<Server.ServerMessageRefresh> PrepareRefreshMessages()
		{
			List<Server.ServerMessageRefresh> positiveMessages = new List<Server.ServerMessageRefresh>();
			List<Server.ServerMessageRefresh> negativeMessages = new List<Server.ServerMessageRefresh>();
			lock (this)
			{
				foreach (FoldersRow folder in Folders)
				{
					lock (folder)
					{
						if (folder.positive)
						{
							Server.ServerMessageRefresh message = folder.PrepareRefreshMessage();
							if (message != null) positiveMessages.Add(message);
						}
						else
						{
							Server.ServerMessageRefresh message = folder.PrepareRefreshMessage();
							if (message != null) negativeMessages.Add(message);
						}
					}
				}
			}

			foreach (Server.ServerMessageRefresh message in negativeMessages)
			{
				message.Log(Properties.Resources.Information, "Ignored activity in ");
			}
			foreach (Server.ServerMessageRefresh message in positiveMessages)
			{
				message.Log(Properties.Resources.Information, "Detected activity in ");
			}
			return positiveMessages; // will be sent to browser
		}

		public bool PassesGlobalExcludes(string path, MatchReason reason)
		{
			// it must not match any global exclude
			foreach (GlobalExcludeFiltersRow row in GlobalExcludeFilters)
			{
				FileMask wildcard = new FileMask(row.Mask);
				if (wildcard.IsMatch(path))
				{
					reason.Set(row.Mask, MatchReason.Status.Excluded);
					return false;
				}
			}
			return true;
		}

		public bool PassesGlobalExcludes(string path)
		{
			// it must not match any global exclude
			foreach (GlobalExcludeFiltersRow row in GlobalExcludeFilters)
			{
				FileMask wildcard = new FileMask(row.Mask);
				if (wildcard.IsMatch(path))
				{
					return false;
				}
			}
			return true;
		}

		public bool PassesGlobalIncludes(string path, MatchReason reason)
		{
			// it must match at least one global include
			foreach (GlobalIncludeFiltersRow row in GlobalIncludeFilters)
			{
				FileMask wildcard = new FileMask(row.Mask);
				if (wildcard.IsMatch(path))
				{
					reason.Set(row.Mask, MatchReason.Status.Included);
					return true;
				}
			}

			// didn't match any include
			return false;
		}

		public bool PassesGlobalFilters(string path, MatchReason reason)
		{
			if (PassesGlobalIncludes(path, reason)) return true;
			return PassesGlobalExcludes(path, reason);
		}

		internal bool RunningOutOfSpace()
		{
			lock (this)
			{
				foreach (Model.FoldersRow folder in Folders)
				{
					if (folder.RunningOutOfSpace()) return true;
				}
			}
			return false;
		}
	}

	public class MatchReason
	{
		public enum Status
		{
			Normal,
			Excluded,
			Included,
		}

		string _text;
		Status _status;

		public string text { get { return _text; } }
		public Status status { get { return _status; } }

		public MatchReason()
		{
			Reset();
		}

		public void Reset()
		{
			_status = Status.Normal;
			_text = "";        
		}

		public MatchReason(string text, Status status)
		{
			this._text = text;
			this._status = status;
		}

		internal MatchReason Clone()
		{
			return new MatchReason(_text, _status);
		}

		public void Set(string text, Status status)
		{
			if (this._status > status) 
				throw new ModelException("Inconsistent match rules");
			if (this._status<status)
			{
				this._status = status;
				this._text = text;
			}
		}
	}
}