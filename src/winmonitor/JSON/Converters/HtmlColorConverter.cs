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
using System.Drawing;

namespace Newtonsoft.Json.Converters
{
	public class HtmlColorConverter : JsonConverter
	{
		public override void WriteJson(JsonWriter writer, object value)
		{
			writer.WriteValue(ColorTranslator.ToHtml((Color) value));
		}

		public override bool CanConvert(Type valueType)
		{
			return typeof(Color).IsAssignableFrom(valueType);
		}
	}
}
