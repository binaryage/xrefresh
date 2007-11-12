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
using System.IO;
using System.Collections;
using System.Reflection;
using System.ComponentModel;
using Newtonsoft.Json.Utilities;

namespace Newtonsoft.Json
{
	/// <summary>
	/// Specifies reference loop handling options for the <see cref="JsonWriter"/>.
	/// </summary>
	public enum ReferenceLoopHandling
	{
		/// <summary>
		/// Throw a <see cref="JsonSerializationException"/> when a loop is encountered.
		/// </summary>
		Error = 0,
		/// <summary>
		/// Ignore loop references and do not serialize.
		/// </summary>
		Ignore = 1,
		/// <summary>
		/// Serialize loop references.
		/// </summary>
		Serialize = 2
	}

	/// <summary>
	/// Serializes and deserializes objects into and from the Json format.
	/// The <see cref="JsonSerializer"/> enables you to control how objects are encoded into Json.
	/// </summary>
	public class JsonSerializer
	{
		private ReferenceLoopHandling _referenceLoopHandling;
		private int _level;
		private JsonConverterCollection _converters;

		/// <summary>
		/// Get or set how reference loops (e.g. a class referencing itself) is handled.
		/// </summary>
		public ReferenceLoopHandling ReferenceLoopHandling
		{
			get { return _referenceLoopHandling; }
			set
			{
				if (value < ReferenceLoopHandling.Error || value > ReferenceLoopHandling.Serialize)
				{
					throw new ArgumentOutOfRangeException("value");
				}
				_referenceLoopHandling = value;
			}
		}

		public JsonConverterCollection Converters
		{
			get
			{
				if (_converters == null)
					_converters = new JsonConverterCollection();

				return _converters;
			}
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="JsonSerializer"/> class.
		/// </summary>
		public JsonSerializer()
		{
			_referenceLoopHandling = ReferenceLoopHandling.Error;
		}

		#region Deserialize
		/// <summary>
		/// Deserializes the Json structure contained by the specified <see cref="JsonReader"/>.
		/// </summary>
		/// <param name="reader">The <see cref="JsonReader"/> that contains the Json structure to deserialize.</param>
		/// <returns>The <see cref="Object"/> being deserialized.</returns>
		public object Deserialize(JsonReader reader)
		{
			return Deserialize(reader, null);
		}

		/// <summary>
		/// Deserializes the Json structure contained by the specified <see cref="JsonReader"/>
		/// into an instance of the specified type.
		/// </summary>
		/// <param name="reader">The type of object to create.</param>
		/// <param name="objectType">The <see cref="Type"/> of object being deserialized.</param>
		/// <returns>The instance of <paramref name="objectType"/> being deserialized.</returns>
		public object Deserialize(JsonReader reader, Type objectType)
		{
			if (reader == null)
				throw new ArgumentNullException("reader");

			if (!reader.Read())
				return null;

			return GetObject(reader, objectType);
		}

		private JavaScriptArray PopulateJavaScriptArray(JsonReader reader)
		{
			JavaScriptArray jsArray = new JavaScriptArray();

			while (reader.Read())
			{
				switch (reader.TokenType)
				{
					case JsonToken.EndArray:
						return jsArray;
					case JsonToken.Comment:
						break;
					default:
						object value = GetObject(reader, null);

						jsArray.Add(value);
						break;
				}
			}

			throw new JsonSerializationException("Unexpected end while deserializing array.");
		}

		private JavaScriptObject PopulateJavaScriptObject(JsonReader reader)
		{
			JavaScriptObject jsObject = new JavaScriptObject();

			while (reader.Read())
			{
				switch (reader.TokenType)
				{
					case JsonToken.PropertyName:
						string memberName = reader.Value.ToString();

						// move to the value token. skip comments
						do
						{
							if (!reader.Read())
								throw new JsonSerializationException("Unexpected end while deserializing object.");
						} while (reader.TokenType == JsonToken.Comment);

						object value = GetObject(reader, null);

						jsObject[memberName] = value;
						break;
					case JsonToken.EndObject:
						return jsObject;
					case JsonToken.Comment:
						break;
					default:
						throw new JsonSerializationException("Unexpected token while deserializing object: " + reader.TokenType);
				}
			}

			throw new JsonSerializationException("Unexpected end while deserializing object.");
		}

		private object GetObject(JsonReader reader, Type objectType)
		{
			_level++;

			object value;
			JsonConverter converter;

			if (HasMatchingConverter(objectType, out converter))
			{
				return converter.ReadJson(reader, objectType);
			}
			else
			{
				switch (reader.TokenType)
				{
					// populate a typed object or generic dictionary/array
					// depending upon whether an objectType was supplied
					case JsonToken.StartObject:
						value = (objectType != null) ? PopulateObject(reader, objectType) : PopulateJavaScriptObject(reader);
						break;
					case JsonToken.StartArray:
						value = (objectType != null) ? PopulateList(reader, objectType) : PopulateJavaScriptArray(reader);
						break;
					case JsonToken.Integer:
					case JsonToken.Float:
					case JsonToken.String:
					case JsonToken.Boolean:
					case JsonToken.Date:
						value = EnsureType(reader.Value, objectType);
						break;
					case JsonToken.Constructor:
						value = reader.Value.ToString();
						break;
					case JsonToken.Null:
					case JsonToken.Undefined:
						value = null;
						break;
					default:
						throw new JsonSerializationException("Unexpected token whil deserializing object: " + reader.TokenType);
				}
			}

			_level--;

			return value;
		}

		private object EnsureType(object value, Type targetType)
		{
			// do something about null value when the targetType is a valuetype?
			if (value == null)
				return null;

			if (targetType == null)
				return value;

			Type valueType = value.GetType();

			// type of value and type of target don't match
			// attempt to convert value's type to target's type
			if (valueType != targetType)
			{
				TypeConverter targetConverter = TypeDescriptor.GetConverter(targetType);

				if (!targetConverter.CanConvertFrom(valueType))
				{
					if (targetConverter.CanConvertFrom(typeof(string)))
					{
						string valueString = TypeDescriptor.GetConverter(value).ConvertToInvariantString(value);

						return targetConverter.ConvertFromInvariantString(valueString);
					}

					if (!targetType.IsAssignableFrom(valueType))
						throw new InvalidOperationException(string.Format("Cannot convert object of type '{0}' to type '{1}'", value.GetType(), targetType));

					return value;
				}

				return targetConverter.ConvertFrom(value);
			}
			else
			{
				return value;
			}
		}

		private void SetObjectMember(JsonReader reader, object target, Type targetType, string memberName)
		{
			if (!reader.Read())
				throw new JsonSerializationException(string.Format("Unexpected end when setting {0}'s value.", memberName));

			MemberInfo[] memberCollection = targetType.GetMember(memberName);
			Type memberType;
			object value;

			// test if a member with memberName exists on the type
            // otherwise test if target is a dictionary and assign value with the key if it is
			if (!CollectionUtils.IsNullOrEmpty<MemberInfo>(memberCollection))
			{
				MemberInfo member = targetType.GetMember(memberName)[0];

                // ignore member if it is readonly
                if (!ReflectionUtils.CanSetMemberValue(member))
                    return;

				if (member.IsDefined(typeof(JsonIgnoreAttribute), true))
					return;

				// get the member's underlying type
				memberType = ReflectionUtils.GetMemberUnderlyingType(member);

				value = GetObject(reader, memberType);

				ReflectionUtils.SetMemberValue(member, target, value);
			}
			else if (typeof(IDictionary).IsAssignableFrom(targetType))
			{
				// attempt to get the IDictionary's type
				memberType = ReflectionUtils.GetTypedDictionaryValueType(target.GetType());

				value = GetObject(reader, memberType);

				((IDictionary)target).Add(memberName, value);
			}
			else
			{
				throw new JsonSerializationException(string.Format("Could not find member '{0}' on object of type '{1}'", memberName, targetType.GetType().Name));
			}
		}

		private object PopulateList(JsonReader reader, Type objectType)
		{
			IList list;
			Type elementType = ReflectionUtils.GetTypedListItemType(objectType);

			if (objectType.IsArray || objectType == typeof(ArrayList) || objectType == typeof(object))
				// array or arraylist.
				// have to use an arraylist when creating array because there is no way to know the size until it is finised
				list = new ArrayList();
			else if (ReflectionUtils.IsInstantiatableType(objectType) && typeof(IList).IsAssignableFrom(objectType))
				// non-generic typed list
				list = (IList)Activator.CreateInstance(objectType);
			else if (ReflectionUtils.IsSubClass(objectType, typeof(List<>)))
				// generic list
				list = (IList)Activator.CreateInstance(typeof(List<>).MakeGenericType(elementType));
			else
				throw new JsonSerializationException(string.Format("Deserializing list type '{0}' not supported.", objectType.GetType().Name));

			while (reader.Read())
			{
				switch (reader.TokenType)
				{
					case JsonToken.EndArray:
						ArrayList arrayList = list as ArrayList;
						if (arrayList != null)
						{
							// convert back into array now that it is finised
							if (objectType.IsArray)
								list = arrayList.ToArray(elementType);
							else if (objectType == typeof(object))
								list = arrayList.ToArray();
						}
						
						return list;
					case JsonToken.Comment:
						break;
					default:
						object value = GetObject(reader, elementType);

						list.Add(value);
						break;
				}
			}

			throw new JsonSerializationException("Unexpected end when deserializing array.");
		}

		private object PopulateObject(JsonReader reader, Type objectType)
		{
			object newObject = Activator.CreateInstance(objectType);

			while (reader.Read())
			{
				switch (reader.TokenType)
				{
					case JsonToken.PropertyName:
						string memberName = reader.Value.ToString();

						SetObjectMember(reader, newObject, objectType, memberName);
						break;
					case JsonToken.EndObject:
						return newObject;
					default:
						throw new JsonSerializationException("Unexpected token when deserializing object: " + reader.TokenType);
				}
			}

			throw new JsonSerializationException("Unexpected end when deserializing object.");
		}
		#endregion

		#region Serialize
		/// <summary>
		/// Serializes the specified <see cref="Object"/> and writes the Json structure
		/// to a <c>Stream</c> using the specified <see cref="TextWriter"/>. 
		/// </summary>
		/// <param name="textWriter">The <see cref="TextWriter"/> used to write the Json structure.</param>
		/// <param name="value">The <see cref="Object"/> to serialize.</param>
		public void Serialize(TextWriter textWriter, object value)
		{
			Serialize(new JsonWriter(textWriter), value);
		}

		/// <summary>
		/// Serializes the specified <see cref="Object"/> and writes the Json structure
		/// to a <c>Stream</c> using the specified <see cref="JsonWriter"/>. 
		/// </summary>
		/// <param name="jsonWriter">The <see cref="JsonWriter"/> used to write the Json structure.</param>
		/// <param name="value">The <see cref="Object"/> to serialize.</param>
		public void Serialize(JsonWriter jsonWriter, object value)
		{
			if (jsonWriter == null)
				throw new ArgumentNullException("jsonWriter");

			if (value == null)
				throw new ArgumentNullException("value");

			SerializeValue(jsonWriter, value);
		}


		private void SerializeValue(JsonWriter writer, object value)
		{
			JsonConverter converter;

			if (value == null)
			{
				writer.WriteNull();
			}
			else if (HasMatchingConverter(value.GetType(), out converter))
			{
				converter.WriteJson(writer, value);
			}
			else if (value is IConvertible)
			{
				IConvertible convertible = value as IConvertible;

				switch (convertible.GetTypeCode())
				{
					case TypeCode.String:
						writer.WriteValue((string)convertible);
						break;
					case TypeCode.Char:
						writer.WriteValue((char)convertible);
						break;
					case TypeCode.Boolean:
						writer.WriteValue((bool)convertible);
						break;
					case TypeCode.SByte:
						writer.WriteValue((sbyte)convertible);
						break;
					case TypeCode.Int16:
						writer.WriteValue((short)convertible);
						break;
					case TypeCode.UInt16:
						writer.WriteValue((ushort)convertible);
						break;
					case TypeCode.Int32:
						writer.WriteValue((int)convertible);
						break;
					case TypeCode.Byte:
						writer.WriteValue((byte)convertible);
						break;
					case TypeCode.UInt32:
						writer.WriteValue((uint)convertible);
						break;
					case TypeCode.Int64:
						writer.WriteValue((long)convertible);
						break;
					case TypeCode.UInt64:
						writer.WriteValue((ulong)convertible);
						break;
					case TypeCode.Single:
						writer.WriteValue((float)convertible);
						break;
					case TypeCode.Double:
						writer.WriteValue((double)convertible);
						break;
					case TypeCode.DateTime:
						writer.WriteValue((DateTime)convertible);
						break;
					case TypeCode.Decimal:
						writer.WriteValue((decimal)convertible);
						break;
					default:
						SerializeObject(writer, value);
						break;
				}
			}
			else if (value is IList)
			{
				SerializeList(writer, (IList)value);
			}
			else if (value is IDictionary)
			{
				SerializeDictionary(writer, (IDictionary)value);
			}
			else if (value is ICollection)
			{
				SerializeCollection(writer, (ICollection)value);
			}
			else if (value is Identifier)
			{
				writer.WriteRaw(value.ToString());
			}
			else
			{
				Type valueType = value.GetType();

				SerializeObject(writer, value);
			}
		}

		private bool HasMatchingConverter(Type type, out JsonConverter matchingConverter)
		{
			if (_converters != null)
			{
				for (int i = 0; i < _converters.Count; i++)
				{
					JsonConverter converter = _converters[i];

					if (converter.CanConvert(type))
					{
						matchingConverter = converter;
						return true;
					}
				}
			}

			matchingConverter = null;
			return false;
		}

		private void WriteMemberInfoProperty(JsonWriter writer, object value, MemberInfo member)
		{
			if (!ReflectionUtils.IsIndexedProperty(member))
			{
				object memberValue = ReflectionUtils.GetMemberValue(member, value);

				if (writer.SerializeStack.IndexOf(memberValue) != -1)
				{
					switch (_referenceLoopHandling)
					{
						case ReferenceLoopHandling.Error:
							throw new JsonSerializationException("Self referencing loop");
						case ReferenceLoopHandling.Ignore:
							// return from method
							return;
						case ReferenceLoopHandling.Serialize:
							// continue
							break;
						default:
							throw new InvalidOperationException(string.Format("Unexpected ReferenceLoopHandling value: '{0}'", _referenceLoopHandling));
					}
				}

				writer.WritePropertyName(member.Name);
				SerializeValue(writer, memberValue);
			}
		}

		private void SerializeObject(JsonWriter writer, object value)
		{
			Type objectType = value.GetType();

			TypeConverter converter = TypeDescriptor.GetConverter(objectType);

			// use the objectType's TypeConverter if it has one and can convert to a string
			if (converter != null && !(converter is ComponentConverter) && converter.GetType() != typeof(TypeConverter))
			{
				if (converter.CanConvertTo(typeof(string)))
				{
					writer.WriteValue(converter.ConvertToInvariantString(value));
					return;
				}
			}

			writer.SerializeStack.Add(value);

			writer.WriteStartObject();

			List<MemberInfo> members = ReflectionUtils.GetFieldsAndProperties(objectType, BindingFlags.Public | BindingFlags.Instance);

			foreach (MemberInfo member in members)
			{
				if (ReflectionUtils.CanReadMemberValue(member) && !member.IsDefined(typeof(JsonIgnoreAttribute), true))
					WriteMemberInfoProperty(writer, value, member);
			}

			writer.WriteEndObject();

			writer.SerializeStack.Remove(value);
		}

		private void SerializeCollection(JsonWriter writer, ICollection values)
		{
			object[] collectionValues = new object[values.Count];
			values.CopyTo(collectionValues, 0);

			SerializeList(writer, collectionValues);
		}

		private void SerializeList(JsonWriter writer, IList values)
		{
			writer.WriteStartArray();

			for (int i = 0; i < values.Count; i++)
			{
				SerializeValue(writer, values[i]);
			}

			writer.WriteEndArray();
		}

		private void SerializeDictionary(JsonWriter writer, IDictionary values)
		{
			writer.WriteStartObject();

			foreach (DictionaryEntry entry in values)
			{
				writer.WritePropertyName(entry.Key.ToString());
				SerializeValue(writer, entry.Value);
			}

			writer.WriteEndObject();
		}
		#endregion
	}
}
