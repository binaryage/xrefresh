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
using System.Collections;
using System.Globalization;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Web;
using System.Collections.Generic;
using System.Drawing;
using System.Web.UI.WebControls;
using System.ComponentModel;

namespace Newtonsoft.Json
{
	public class Identifier
	{
		private string _name;

		public string Name
		{
			get { return _name; }
		}

		public Identifier(string name)
		{
			_name = name;
		}

		private static bool IsAsciiLetter(char c)
		{
			return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
		} 

		public override bool Equals(object obj)
		{
			Identifier function = obj as Identifier;

			return Equals(function);
		}

		public bool Equals(Identifier function)
		{
			return (_name == function.Name);
		}

		public static bool Equals(Identifier a, Identifier b)
		{
			if (a == b)
				return true;

			if (a != null && b != null)
				return a.Equals(b);

			return false;
		}

		public override int GetHashCode()
		{
			return _name.GetHashCode();
		}

		public override string ToString()
		{
			return _name;
		}

		public static bool operator ==(Identifier a, Identifier b)
		{
			return Identifier.Equals(a, b);
		}

		public static bool operator !=(Identifier a, Identifier b)
		{
			return !Identifier.Equals(a, b);
		}
	}
}