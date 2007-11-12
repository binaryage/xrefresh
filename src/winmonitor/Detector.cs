using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;

namespace XRefresh
{
	abstract class Scanner
	{
		static Dictionary<string, Dictionary<string, int>> extCache = new Dictionary<string, Dictionary<string, int>>();
		Model.FoldersRow folder;

		// corresponds to imageListTypes
		public virtual int GetImageIndex() 
		{
			return -1;
		}

		public virtual string GetName()
		{
			return "?";
		}

		public virtual string GetDescription()
		{
			return "";
		}

		public virtual bool Scan(string path)
		{
			return false;
		}

		// this routine will guess project name from directory path
		public virtual string SuggestName(string path)
		{
			string dir = Utils.NormalizePath(path);
			if (dir.Length == 0) return "";
			int last = dir.LastIndexOf('/');
			if (last == -1) last = 0; else last++;
			string candidate = dir.Substring(last);
			char[] separatorChars = new char[] { ' ', '\t', '\n', '_', '.', ',', ';', ':', '-' };
			candidate.TrimStart(separatorChars);
			int first = candidate.IndexOfAny(separatorChars);
			if (first == -1) first = candidate.Length;
			string result = candidate.Substring(0, first);
			if (result.Length == 0) return "";
			result = result.Substring(0, 1).ToUpper() + result.Substring(1);
			return result;
		}

		public virtual void SetFolder(Model.FoldersRow folder)
		{
			this.folder = folder;
			folder.ClearIncludes();
			folder.ClearExcludes();
		}

		public virtual void SetupFilters()
		{
		}

		protected virtual void AddInclude(string mask)
		{
			AddInclude(mask, "");
		}

		protected virtual void AddInclude(string mask, string info)
		{
			Model model = folder.Table.DataSet as Model;
			model.IncludeFilters.AddIncludeFiltersRow(mask, info, folder);
		}

		protected virtual void AddExclude(string mask)
		{
			AddExclude(mask, "");
		}

		protected virtual void AddExclude(string mask, string info)
		{
			Model model = folder.Table.DataSet as Model;
			model.ExcludeFilters.AddExcludeFiltersRow(mask, info, folder);
		}

		protected int ContainsDir(string root, string path)
		{
			string fullpath = Path.Combine(root, path);
			return Directory.Exists(fullpath)?1:0;
		}

		protected int ContainsFile(string root, string path)
		{
			string fullpath = Path.Combine(root, path);
			return File.Exists(fullpath)?1:0;
		}

		public bool HasExtCache(string path)
		{
			string f = Utils.NormalizePath(path).ToLower();
			if (extCache.ContainsKey(f)) return true;
			return false;
		}

		protected bool ContainsAtLeastNumFiles(string root, string pattern, int num)
		{
			string f = Utils.NormalizePath(root).ToLower();
			BuildExtCache(f);
			try {
				return extCache[f][pattern]>=num;
			}
			catch (Exception)
			{
				return false;
			}
		}

		private void BuildExtCache(string f)
		{
			if (extCache.ContainsKey(f)) return; // already done
			Dictionary<string, int> d = new Dictionary<string, int>();
			BuildExtCacheWorker(f, d, f.Length);
			extCache.Add(f, d);
		}

		private void BuildExtCacheWorker(String folder, Dictionary<string, int> dict, int rootLen)
		{
			if (folder.Length > rootLen + 1)
				Detector.Current.ReportProgress(folder.Substring(rootLen + 1));
			if (Detector.Current.WasCancelled()) throw new CancelException();
			if (!Directory.Exists(folder)) return;
			String[] files = Directory.GetFiles(folder);
			// first look for files
			foreach (String file in files)
			{
				if (Detector.Current.WasCancelled()) throw new CancelException();
				string ext = Path.GetExtension(file);
				if (ext.Length == 0) continue;
				ext = ext.Substring(1);
				try {
					dict[ext]++;
				}
				catch (Exception)
				{
					dict.Add(ext, 1);
				}
			}

			String[] dirs = Directory.GetDirectories(folder);
			foreach (String dir in dirs)
			{
				if (Detector.Current.WasCancelled()) throw new CancelException();
				if (dir.Length > rootLen + 1)
					Detector.Current.ReportProgress(dir.Substring(rootLen + 1));

				// optimization, directory must pass global exclude filters
				// this is here mainly to not traverse .svn subdirectories
				if (Context.Model.PassesGlobalFilters(dir, new MatchReason()))
				{
					BuildExtCacheWorker(dir, dict, rootLen);
				}
			}
		}
	}

	class GenericScanner : Scanner
	{
		// corresponds to imageListTypes
		public override int GetImageIndex() 
		{
			return 1;
		}
		public override string GetName()
		{
			return "Generic";
		}
		public override string GetDescription()
		{
			return "Generic folder";
		}
		public override bool Scan(string path)
		{
			return false;
		}
		public override string SuggestName(string path)
		{
			string dir = Path.GetDirectoryName(path);
			if (dir.Length == 0) return "";
			int last = dir.LastIndexOf(Path.DirectorySeparatorChar);
			if (last == -1) last = 0; else last++;
			string candidate = dir.Substring(last);
			char[] whiteSpaceChars = new char[] { ' ', '\t', '\n', '_' };
			candidate.TrimStart(whiteSpaceChars);
			int first = candidate.IndexOfAny(whiteSpaceChars);
			if (first == -1) first = candidate.Length;
			return candidate.Substring(0, first);
		}
	}

	class JSScanner : Scanner
	{
		public override int GetImageIndex() { return 3; }
		public override string GetName() { return "JS"; }
		public override string GetDescription() { return "Javascript project"; }
		public override bool Scan(string path)
		{
			return ContainsAtLeastNumFiles(path, "js", 5);
		}
	}

	class HTMLScanner : Scanner
	{
		public override int GetImageIndex() { return 2; }
		public override string GetName() { return "HTML"; }
		public override string GetDescription() { return "Static web project"; }
		public override bool Scan(string path)
		{
			return ContainsAtLeastNumFiles(path, "html", 5) || 
				ContainsAtLeastNumFiles(path, "htm", 5);
		}
	}

	class PHPScanner : Scanner
	{
		public override int GetImageIndex() { return 7; }
		public override string GetName() { return "PHP"; }
		public override string GetDescription() { return "PHP project"; }
		public override bool Scan(string path)
		{
			return ContainsAtLeastNumFiles(path, "php", 5);
		}
	}

	class RORScanner : Scanner
	{
		public override int GetImageIndex() { return 5;  }
		public override string GetName() { return "RoR"; }
		public override string GetDescription() { return "Ruby on Rails project"; }
		public override bool Scan(string path)
		{
			int score = 0;
			score += ContainsDir(path, "app/controllers");
			score += ContainsDir(path, "app/helpers");
			score += ContainsDir(path, "app/models");
			score += ContainsDir(path, "app/views");
			score += ContainsDir(path, "db");
			score += ContainsDir(path, "lib");
			score += ContainsDir(path, "public");
			score += ContainsDir(path, "vendor");
			score += ContainsDir(path, "script");
			score += ContainsDir(path, "test");

			return ContainsAtLeastNumFiles(path, "rb", 5) && score>=7;
		}
		public override void SetupFilters()
		{
			base.SetupFilters();
			AddExclude("*.log");
			AddExclude("/tmp/*");
			AddExclude("/doc/*");
			AddExclude("/db/*");
			AddExclude("Rakefile");
			AddExclude("README");
		}
	}

	class RubyScanner : Scanner
	{
		public override int GetImageIndex() { return 4; }
		public override string GetName() { return "Ruby"; }
		public override string GetDescription() { return "General Ruby project"; }
		public override bool Scan(string path)
		{
			return ContainsAtLeastNumFiles(path, "rb", 3);
		}
	}

	class ASPScanner : Scanner
	{
		public override int GetImageIndex() { return 8; }
		public override string GetName() { return "ASP"; }
		public override string GetDescription() { return "ASP project"; }
		public override bool Scan(string path)
		{
			return ContainsAtLeastNumFiles(path, "asp", 5) || ContainsAtLeastNumFiles(path, "aspx", 5);
		}
	}

	class JavaScanner : Scanner
	{
		public override int GetImageIndex() { return 9; }
		public override string GetName() { return "Java"; }
		public override string GetDescription() { return "Java project"; }
		public override bool Scan(string path)
		{
			return ContainsAtLeastNumFiles(path, "java", 5);
		}
	}

	class PythonScanner : Scanner
	{
		public override int GetImageIndex() { return 6; }
		public override string GetName() { return "Python"; }
		public override string GetDescription() { return "Pyhon project"; }
		public override bool Scan(string path)
		{
			return ContainsAtLeastNumFiles(path, "py", 5);
		}
	}

	class PerlScanner : Scanner
	{
		public override int GetImageIndex() { return 10; }
		public override string GetName() { return "Perl"; }
		public override string GetDescription() { return "Perl project"; }
		public override bool Scan(string path)
		{
			return ContainsAtLeastNumFiles(path, "pl", 5);
		}
	}

	class Detector
	{
		static public Detector Current;
		public List<Scanner> scanners = new List<Scanner>();
		public BackgroundWorker currentWorker;

		public Detector()
		{
			Current = this;

			// add known scanners into detector
			scanners.Add(new RORScanner());
			scanners.Add(new PythonScanner());
			scanners.Add(new PerlScanner());
			scanners.Add(new PHPScanner());
			scanners.Add(new ASPScanner());
			scanners.Add(new JavaScanner());
			scanners.Add(new RubyScanner());
			scanners.Add(new JSScanner());
			scanners.Add(new HTMLScanner());

			// generic scanner must be last
			scanners.Add(new GenericScanner());
		}

		public Scanner GetBestScanner(string path)
		{
			foreach (Scanner s in scanners)
			{
				if (s.Scan(path)) return s;
			}
			return scanners[scanners.Count-1]; // return generic scanner
		}

		public int GetIcon(string name)
		{
			foreach (Scanner s in scanners)
			{
				if (s.GetName() == name) return s.GetImageIndex();
			}
			return 0; // custom type
		}

		public Scanner GetScanner(string name)
		{
			foreach (Scanner s in scanners)
			{
				if (s.GetName() == name) return s;
			}
			return null; // custom type
		}

		internal void ReportProgress(string msg)
		{
			if (currentWorker != null)
			{
				currentWorker.ReportProgress(0, msg);
			}
		}

		internal bool WasCancelled()
		{
			if (currentWorker == null) return false;
			return currentWorker.CancellationPending;
		}

		public bool HasExtCache(string path)
		{
			return scanners[0].HasExtCache(path);
		}
	}
}
