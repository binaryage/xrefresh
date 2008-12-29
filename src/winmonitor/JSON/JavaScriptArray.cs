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

namespace Newtonsoft.Json
{
	/// <summary>
	/// Represents a JavaScript array.
	/// </summary>
	public class JavaScriptArray : List<object>
	{
		/// <summary>
		/// Initializes a new instance of the <see cref="JavaScriptObject"/> class.
		/// </summary>
		public JavaScriptArray()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="JavaScriptArray"/> class that
		/// contains elements copied from the specified collection.
		/// </summary>
		/// <param name="collection">The collection whose elements are copied to the new array.</param>
		public JavaScriptArray(IEnumerable<object> collection)
			: base(collection)
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="JavaScriptArray"/> class that
		/// is empty and has the specified initial capacity.
		/// </summary>
		/// <param name="capacity">The number of elements that the new array can initially store.</param>
		public JavaScriptArray(int capacity) : base(capacity)
		{
		}
	}
}