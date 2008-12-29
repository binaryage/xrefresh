#region License
// Copyright 2006 James Newton-King
// http://www.newtonsoft.com
//
// This work is licensed under the Creative Commons Attribution 2.5 License
// http://creativecommons.org/licenses/by/2.5/
//
// You are free:
//    * to copy, distribute, display, and perform the work
//    * to make derivative works
//    * to make commercial use of the work
//
// Under the following conditions:
//    * You must attribute the work in the manner specified by the author or licensor:
//          - If you find this component useful a link to http://www.newtonsoft.com would be appreciated.
//    * For any reuse or distribution, you must make clear to others the license terms of this work.
//    * Any of these conditions can be waived if you get permission from the copyright holder.
#endregion

using System;
using System.Collections.Generic;
using System.Text;

namespace Newtonsoft.Json.Utilities
{
	internal static class StringUtils
	{
		public static bool ContainsWhiteSpace(string s)
		{
			for (int i = 0; i < s.Length; i++)
			{
				if (char.IsWhiteSpace(s[i]))
					return true;
			}
			return false;
		}

		public static bool IsWhiteSpace(string s)
		{
			for (int i = 0; i < s.Length; i++)
			{
				if (!char.IsWhiteSpace(s[i]))
					return false;
			}
			return true;
		}

		public static string EnsureEndsWith(string s, string value)
		{
			if (s == null)
				throw new ArgumentNullException("s");

			string trimmedString = s.TrimEnd(null);

			if (trimmedString.Length < value.Length ||
				string.Compare(trimmedString, trimmedString.Length - value.Length, value, 0, value.Length, StringComparison.OrdinalIgnoreCase) != 0)
			{
				return trimmedString + value;
			}

			return s;
		}

		public static void IfNotNullOrEmpty(string value, Action<string> trueAction)
		{
			IfNotNullOrEmpty(value, trueAction, null);
		}

		public static void IfNotNullOrEmpty(string value, Action<string> trueAction, Action<string> falseAction)
		{
			if (!string.IsNullOrEmpty(value))
			{
				if (trueAction != null)
					trueAction(value);
			}
			else
			{
				if (falseAction != null)
					falseAction(value);
			}
		}
	}
}