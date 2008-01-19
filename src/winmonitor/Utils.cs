using System;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.Win32;

namespace XRefresh
{
	// Summary:
	// Defines a values pair that can be set or retrieved.
	[Serializable]
	public struct Pair<TFirst, TSecond>
	{
		TFirst first;
		TSecond second;

		public TFirst First
		{
			get { return first; }
			set { first = value; }
		}

		public TSecond Second
		{
			get { return second; }
			set { second = value; }
		}

		public Pair(TFirst first, TSecond second) 
		{ 
			this.first = first; 
			this.second = second; 
		}
	}

	// from: http://www.codeproject.com/csharp/wildcardtoregex.asp

	/// <summary>
	/// Represents a wildcard running on the
	/// <see cref="System.Text.RegularExpressions"/> engine.
	/// </summary>
	public class FileMask : Regex
	{
		public enum Type {
			None,
			All,
			Substring,
			Wildcard,
			Mask,
			Regexp,
		}

		public Type type;
		static Type lastType;

		/// <summary>
		/// Initializes a wildcard with the given search pattern.
		/// </summary>
		/// <param name="pattern">The wildcard pattern to match.</param>
		public FileMask(string pattern) : 
			base(WildcardToRegex(pattern), RegexOptions.CultureInvariant|RegexOptions.IgnoreCase)
		{
			this.type = lastType; // this is ugly and not thread-safe, I know :-(
		}

		/// <summary>
		/// Converts a wildcard to a regex.
		/// </summary>
		/// <param name="pattern">The wildcard pattern to convert.</param>
		/// <returns>A regex equivalent of the given wildcard.</returns>
		private static string WildcardToRegex(string wildcard)
		{
			if (wildcard.Length == 0)
			{
				lastType = Type.None;
				return "";
			}
			if (wildcard=="*")
			{
				lastType = Type.All;
				return "^.*$";
			}
			if (wildcard[0] == '^' || wildcard[wildcard.Length - 1] == '$')
			{
				lastType = Type.Regexp;
				return wildcard; // already regexp
			}

			bool anyWildcard = false;
			StringBuilder sb = new StringBuilder(wildcard.Length + 8);
			for (int i = 0; i < wildcard.Length; i++)
			{
				char c = wildcard[i];
				switch (c)
				{
					case '*':
						sb.Append(".*");
						anyWildcard = true;
						break;
					case '?':
						sb.Append(".");
						anyWildcard = true;
						break;
					case '\\':
						if (i < wildcard.Length - 1)
							sb.Append(Regex.Escape(wildcard[++i].ToString()));
						break;
					default:
						sb.Append(Regex.Escape(wildcard[i].ToString()));
						break;
				}
			}
			if (anyWildcard)
			{
				if (wildcard.StartsWith("*."))
				{
					lastType = Type.Mask;
				}
				else
				{
					lastType = Type.Wildcard;
				}
				return "^" + sb.ToString() + "$";
			}
			else
			{
				lastType = Type.Substring;
				return "^.*" + sb.ToString() + ".*$";
			}
		}

		public string GetTypeInfo()
		{
			switch (type)
			{
				case Type.All: return "any file";
				case Type.Mask: return "filename mask";
				case Type.Regexp: return "regular expression";
				case Type.None: return "no files";
				case Type.Substring: return "filename substring";
				case Type.Wildcard: return "wildcard expression";
			}
			return "unknown type";
		}
	}

	class Utils
	{
		const string RootKey = "Software\\XRefresh";

		static public string GetSettingsPath()
		{
			try {
				RegistryKey key = Registry.CurrentUser.OpenSubKey(RootKey);
				string path = (string)key.GetValue("SettingsPath");
				if (path.Length == 0) return "settings.xml";
				return path;
			}
			catch (NullReferenceException)
			{
				RegistryKey key = Registry.CurrentUser.CreateSubKey(RootKey);
				SetSettingsPath("settings.xml");
				return "settings.xml";
			}
		}

		static public void SetSettingsPath(string path)
		{
			RegistryKey key = Registry.CurrentUser.CreateSubKey(RootKey);
			key.SetValue("SettingsPath", path);
		}

		static public int GetRefreshCounter()
		{
			try
			{
				RegistryKey key = Registry.CurrentUser.OpenSubKey(RootKey);
				return (int)key.GetValue("RefreshCounter");
			}
			catch (NullReferenceException)
			{
				RegistryKey key = Registry.CurrentUser.CreateSubKey(RootKey);
				SetRefreshCounter(0);
				return 0;
			}
		}

		static public void SetRefreshCounter(int count)
		{
			RegistryKey key = Registry.CurrentUser.CreateSubKey(RootKey);
			key.SetValue("RefreshCounter", count);
		}

		static public string GetUniqueId()
		{
			try
			{
				RegistryKey key = Registry.CurrentUser.OpenSubKey(RootKey);
				string id = (string)key.GetValue("UniqueId");
				if (id.Length == 0) throw new NullReferenceException();
				return id;
			}
			catch (NullReferenceException)
			{
				RegistryKey key = Registry.CurrentUser.CreateSubKey(RootKey);
				System.Random r = new System.Random();
				int a = r.Next();
				int b = r.Next();
				String s = String.Format("{0}{1}", a, b);
				SetUniqueId(s);
				return s;
			}
		}

		static public void SetUniqueId(string id)
		{
			RegistryKey key = Registry.CurrentUser.CreateSubKey(RootKey);
			key.SetValue("UniqueId", id);
		}

		static public void CheckOrGenerateUniqueId()
		{
			string s = GetUniqueId();
		}

		static public bool IsRunningFirstTime()
		{
			try
			{
				RegistryKey key = Registry.CurrentUser.OpenSubKey(RootKey);
				return ((int)key.GetValue("RunningFirstTime")==0)?false:true;
			}
			catch (NullReferenceException)
			{
				RegistryKey key = Registry.CurrentUser.CreateSubKey(RootKey);
				key.SetValue("RunningFirstTime", 0);
				return true;
			}
		}

		static public bool IsSettingsPathDefault(string path)
		{
			return path.ToLower() == "settings.xml";
		}

		static public bool IsStartupPathPresent(string path)
		{
			try
			{
				RegistryKey key = Registry.CurrentUser.OpenSubKey(@"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
				string value = key.GetValue("XRefresh") as string;
				return value.ToLower().TrimEnd(Path.DirectorySeparatorChar) == path.ToLower().TrimEnd(Path.DirectorySeparatorChar);
			}
			catch (NullReferenceException)
			{
				return false;
			}
		}

		static public bool SetStartupPath(string path)
		{
			RegistryKey key;
			try
			{
				key = Registry.CurrentUser.OpenSubKey(@"Software\\Microsoft\\Windows\\CurrentVersion\\Run", true);
			}
			catch (NullReferenceException)
			{
				key = Registry.CurrentUser.CreateSubKey(@"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
			}
			key.SetValue("XRefresh", path);
			return true;
		}

		static public bool RemoveStartupPath()
		{
			try
			{
				RegistryKey key = Registry.CurrentUser.OpenSubKey(@"Software\\Microsoft\\Windows\\CurrentVersion\\Run", true);
				key.DeleteValue("XRefresh", true);
				return true;
			}
			catch (NullReferenceException)
			{
				return false;
			}
			catch (ArgumentException)
			{
				return false;
			}
		}

		static public bool DisableErrorReporting(string executable)
		{
			RegistryKey key;
			try 
			{
				key = Registry.LocalMachine.OpenSubKey(@"Software\Microsoft\PCHealth\ErrorReporting\ExclusionList", true);
			}
			catch (NullReferenceException)
			{
				key = Registry.LocalMachine.CreateSubKey(@"Software\Microsoft\PCHealth\ErrorReporting\ExclusionList");
			}
			if (key == null) return false;
			key.SetValue(executable, 1, RegistryValueKind.DWord);
			return true;
		}

		static public string NormalizePath(string path)
		{
			return path.TrimEnd(Path.DirectorySeparatorChar).Replace('\\', '/');
		}

		static public void LogException(string msg, Exception e)
		{
			ActivityLog.Current.AddEventLog(Properties.Resources.Error, msg,
				new Pair<Bitmap, string>[] { new Pair<Bitmap, string>(Properties.Resources.Information, e.Message) });
		}

		static public void LogInfo(string msg)
		{
			ActivityLog.Current.AddEventLog(Properties.Resources.Information, msg);
		}

		static public Pair<Bitmap, string>[] LogLine(Bitmap icon, string text) 
		{
			return new Pair<Bitmap, string>[] { new Pair<Bitmap, string>(icon, text) };
		}

		internal static string GetVersionString()
		{
			AssemblyName name = Assembly.GetExecutingAssembly().GetName();
			return string.Format("{0}.{1}", name.Version.Major, name.Version.Minor);
		}

		public static void ReduceMemoryUsage()
		{
			Utils.SetWorkingSet(500000, 100000);
		}

		public static void SetWorkingSet(int max, int min)
		{
			try
			{
				System.Diagnostics.Process process = System.Diagnostics.Process.GetCurrentProcess();
				process.MinWorkingSet = (IntPtr)min;
				process.MaxWorkingSet = (IntPtr)max;
			}
			catch (Exception)
			{
			}
		}
	}
}
