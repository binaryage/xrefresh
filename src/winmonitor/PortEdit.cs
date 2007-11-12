using System;

using FlexFieldControlLib;

namespace XRefresh
{
	public partial class PortEdit : FlexFieldControl
	{
		public PortEdit()
		{
			FieldCount = 1;
			SetMaxLength(5);
			SetSeparatorText(":");
			SetSeparatorText(0, String.Empty);
			SetSeparatorText(FieldCount, String.Empty);
			
			InitializeComponent();
		}
	}
}
