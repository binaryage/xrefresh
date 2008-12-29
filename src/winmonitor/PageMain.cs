using System;
using System.IO;
using System.Windows.Forms;

namespace XRefresh
{
	public partial class PageMain : GUI.FirefoxDialog.PropertyPage
	{
		Model model;

		public PageMain(Model model)
		{
			this.model = model;
			InitializeComponent();
		}

		public override void OnInit()
		{
			Init();
			UpdateStates();
		}

		public override void OnSetActive()
		{
		}

		public override void OnApply()
		{
			Save();
		}

		private void Init()
		{
			editSettingsFile.Text = Utils.GetSettingsPath();
			checkSaveSettings.Checked = Utils.IsSettingsPathDefault(editSettingsFile.Text);

			Model.SettingsRow settings = model.GetSettings();
			checkRunOnStartup.Checked = settings.RunOnWindowsStartup;
			checkPlaySoundOnRefresh.Checked = settings.PlaySoundOnRefresh;
			checkFlashIconOnRefresh.Checked = settings.FlashIconOnRefresh;
			checkEnableLogging.Checked = settings.EnableLogging;
			editSoundFile.Text = settings.SoundFile;
			editWaitTimeout.SetValue(settings.WaitingTimeout);
		}

		private void Save()
		{
			Utils.SetSettingsPath(editSettingsFile.Text);

			Model.SettingsRow settings = model.GetSettings();
			settings.RunOnWindowsStartup = checkRunOnStartup.Checked;
			settings.PlaySoundOnRefresh = checkPlaySoundOnRefresh.Checked;
			settings.FlashIconOnRefresh = checkFlashIconOnRefresh.Checked;
			settings.SoundFile = editSoundFile.Text;
			settings.WaitingTimeout = editWaitTimeout.GetValue(0);
			settings.EnableLogging = checkEnableLogging.Checked;
		}

		private void UpdateStates()
		{
			bool soundEnabled = checkPlaySoundOnRefresh.Checked;
			editSoundFile.Enabled = soundEnabled;
			labelSoundFile.Enabled = soundEnabled;
			buttonSoundFile.Enabled = soundEnabled;

			bool settingsEnabled = !checkSaveSettings.Checked;
			editSettingsFile.Enabled = settingsEnabled;
			labelSettingsFile.Enabled = settingsEnabled;
			buttonSettingsFile.Enabled = settingsEnabled;
		}

		private void buttonSettingsFile_Click(object sender, EventArgs e)
		{
			OpenFileDialog fd = new OpenFileDialog();
			fd.Filter = "XML files (*.xml)|*.xml|All files (*.*)|*.*";
			fd.FilterIndex = 1;
			//fd.CheckFileExists = true;
			fd.DereferenceLinks = true;
			fd.RestoreDirectory = true;
			fd.Multiselect = false;
			fd.ValidateNames = true;
			fd.FileName = editSettingsFile.Text;
			if (fd.ShowDialog() != DialogResult.OK) return;
			if (Directory.GetCurrentDirectory().ToLower().TrimEnd(Path.DirectorySeparatorChar) ==
				Path.GetDirectoryName(fd.FileName).ToLower().TrimEnd(Path.DirectorySeparatorChar))
			{
				editSettingsFile.Text = Path.GetFileName(fd.FileName);
			}
			else
			{
				editSettingsFile.Text = fd.FileName;
			}

		}

		private void buttonSoundFile_Click(object sender, EventArgs e)
		{
			OpenFileDialog fd = new OpenFileDialog();
			fd.Filter = "WAV files (*.wav)|*.wav|All files (*.*)|*.*";
			fd.FilterIndex = 1;
			fd.CheckFileExists = true;
			fd.DereferenceLinks = true;
			fd.RestoreDirectory = true;
			fd.Multiselect = false;
			fd.ValidateNames = true;
			fd.FileName = editSoundFile.Text;
			if (fd.ShowDialog() != DialogResult.OK) return;
			if (Directory.GetCurrentDirectory().ToLower().TrimEnd(Path.DirectorySeparatorChar) ==
				Path.GetDirectoryName(fd.FileName).ToLower().TrimEnd(Path.DirectorySeparatorChar))
			{
				editSoundFile.Text = Path.GetFileName(fd.FileName);
			}
			else
			{
				editSoundFile.Text = fd.FileName;
			}
		}

		private void checkSaveSettings_CheckedChanged(object sender, EventArgs e)
		{
			UpdateStates();
		}

		private void checkPlaySoundOnRefresh_CheckedChanged(object sender, EventArgs e)
		{
			UpdateStates();
		}
	}
}
