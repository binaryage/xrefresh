using System;

namespace XRefresh
{
	public partial class PageNetwork : GUI.FirefoxDialog.PropertyPage
	{
		Model model;

		public PageNetwork(Model model)
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
			Model.SettingsRow settings = model.GetSettings();
			checkAcceptOnlyLocal.Checked = settings.SlaveHost.ToLower() == "localhost";
			editRemoteComputer.Text = settings.SlaveHost;
			editPort.SetValue(settings.Port);

			checkForUpdates.Checked = settings.CheckForUpdates;
			checkUsageStatistics.Checked = settings.SendUsage;
		}

		private void Save()
		{
			Model.SettingsRow settings = model.GetSettings();
			settings.SlaveHost = editRemoteComputer.Text;
			settings.Port = editPort.GetValue(0);

			settings.CheckForUpdates = checkForUpdates.Checked;
			settings.SendUsage = checkUsageStatistics.Checked;
		}

		private void UpdateStates()
		{
			bool localChecked = !checkAcceptOnlyLocal.Checked;
			editRemoteComputer.Enabled = localChecked;
			labelRemoteComputer.Enabled = localChecked;

			bool updates = checkForUpdates.Checked;
			checkUsageStatistics.Enabled = updates;
		}

		private void checkForUpdates_CheckedChanged(object sender, EventArgs e)
		{
			UpdateStates();
		}

		private void checkAcceptOnlyLocal_CheckedChanged(object sender, EventArgs e)
		{
			UpdateStates();
		}
	}
}
