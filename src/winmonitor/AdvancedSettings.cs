using System;
using System.Windows.Forms;

namespace XRefresh
{
	public partial class AdvancedSettings : Form
	{
		private Model model; // temporary model copy for configuration form
		
		public AdvancedSettings(Model model)
		{
			this.model = model;
			InitializeComponent();
			firefoxDialog.ResetEvent += new GUI.FirefoxDialog.FirefoxDialog.ResetEventHandler(firefoxDialog_ResetEvent);
		}

		void firefoxDialog_ResetEvent()
		{
			DialogResult res = MessageBox.Show("Do you really want to reset settings to default values?", "Delete item", MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
			if (res != DialogResult.OK) return;
			model.CreateDefaultSettings();
			this.firefoxDialog.UpdatePages();
		}

		private void OnLoad(object sender, EventArgs e)
		{
			this.firefoxDialog.AddPage("Main", new PageMain(model));
			this.firefoxDialog.AddPage("Ignores", new PageFilters(model));
			this.firefoxDialog.AddPage("Network", new PageNetwork(model));

			this.firefoxDialog.Init();
		}
	}
}