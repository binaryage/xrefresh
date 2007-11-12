using System;

using FlexFieldControlLib;

namespace XRefresh
{
	public partial class TimeoutEdit : FlexFieldControl
	{
		public TimeoutEdit()
		{
			FieldCount = 1;
			SetMaxLength(4);
			SetSeparatorText(":");
			SetSeparatorText(0, String.Empty);
			SetSeparatorText(FieldCount, String.Empty);
			
			InitializeComponent();
		}
	}
}
