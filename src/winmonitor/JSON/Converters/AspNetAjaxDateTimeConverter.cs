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
using System.Globalization;

namespace Newtonsoft.Json.Converters
{
	public class AspNetAjaxDateTimeConverter : JsonConverter
	{
		public override void WriteJson(JsonWriter writer, object value)
		{
			DateTime dateTime = (DateTime)value;
			long javaScriptTicks = JavaScriptConvert.ConvertDateTimeToJavaScriptTicks(dateTime);

			writer.WriteValue("@" + javaScriptTicks.ToString(null, CultureInfo.InvariantCulture) + "@");
		}

		public override object ReadJson(JsonReader reader, Type objectType)
		{
			string dateTimeText = (string) reader.Value;
			dateTimeText = dateTimeText.Substring(1, dateTimeText.Length - 2);

			long javaScriptTicks = Convert.ToInt64(dateTimeText);

			return JavaScriptConvert.ConvertJavaScriptTicksToDateTime(javaScriptTicks);
		}

		public override bool CanConvert(Type valueType)
		{
			return typeof(DateTime).IsAssignableFrom(valueType);
		}
	}
}
