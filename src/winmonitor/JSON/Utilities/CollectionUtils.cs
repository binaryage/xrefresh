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
using System.Collections;

namespace Newtonsoft.Json.Utilities
{
    internal delegate T Func<A0, T>(A0 arg0);

    internal static class CollectionUtils
    {
        /// <summary>
        /// Determines whether the collection is null or empty.
        /// </summary>
        /// <param name="collection">The collection.</param>
        /// <returns>
        /// 	<c>true</c> if the collection is null or empty; otherwise, <c>false</c>.
        /// </returns>
        public static bool IsNullOrEmpty(ICollection collection)
        {
            if (collection != null)
            {
                return (collection.Count == 0);
            }
            return true;
        }

        /// <summary>
        /// Determines whether the collection is null or empty.
        /// </summary>
        /// <param name="collection">The collection.</param>
        /// <returns>
        /// 	<c>true</c> if the collection is null or empty; otherwise, <c>false</c>.
        /// </returns>
        public static bool IsNullOrEmpty<T>(ICollection<T> collection)
        {
            if (collection != null)
            {
                return (collection.Count == 0);
            }
            return true;
        }

        /// <summary>
        /// Determines whether the collection is null, empty or its contents are uninitialized values.
        /// </summary>
        /// <param name="collection">The collection.</param>
        /// <returns>
        /// 	<c>true</c> if the collection is null or empty or its contents are uninitialized values; otherwise, <c>false</c>.
        /// </returns>
        public static bool IsNullOrEmptyOrDefault<T>(IList<T> list)
        {
            if (IsNullOrEmpty<T>(list))
                return true;

            return ReflectionUtils.ItemsUnitializedValue<T>(list);
        }

        /// <summary>
        /// Makes a slice of the specified list in between the start and end indexes.
        /// </summary>
        /// <param name="list">The list.</param>
        /// <param name="start">The start index.</param>
        /// <param name="end">The end index.</param>
        /// <returns>A slice of the list.</returns>
        public static IList<T> Slice<T>(IList<T> list, int? start, int? end)
        {
            return Slice<T>(list, start, end, null);
        }

        /// <summary>
        /// Makes a slice of the specified list in between the start and end indexes,
        /// getting every so many items based upon the step.
        /// </summary>
        /// <param name="list">The list.</param>
        /// <param name="start">The start index.</param>
        /// <param name="end">The end index.</param>
        /// <param name="step">The step.</param>
        /// <returns>A slice of the list.</returns>
        public static IList<T> Slice<T>(IList<T> list, int? start, int? end, int? step)
        {
            if (list == null)
                throw new ArgumentNullException("list");

            if (step == 0)
                throw new ArgumentException("Step cannot be zero.", "step");

            List<T> slicedList = new List<T>();

            // nothing to slice
            if (list.Count == 0)
                return slicedList;

            // set defaults for null arguments
            int s = step ?? 1;
            int startIndex = start ?? 0;
            int endIndex = end ?? list.Count;

            // start from the end of the list if start is negitive
            startIndex = (startIndex < 0) ? list.Count + startIndex : startIndex;

            // end from the start of the list if end is negitive
            endIndex = (endIndex < 0) ? list.Count + endIndex : endIndex;

            // ensure indexes keep within collection bounds
            startIndex = Math.Max(startIndex, 0);
            endIndex = Math.Min(endIndex, list.Count - 1);

            // loop between start and end indexes, incrementing by the step
            for (int i = startIndex; i < endIndex; i += s)
            {
                slicedList.Add(list[i]);
            }

            return slicedList;
        }


        /// <summary>
        /// Group the collection using a function which returns the key.
        /// </summary>
        /// <param name="source">The source collection to group.</param>
        /// <param name="keySelector">The key selector.</param>
        /// <returns>A Dictionary with each key relating to a list of objects in a list grouped under it.</returns>
        public static Dictionary<K, List<V>> GroupBy<K, V>(ICollection<V> source, Func<V, K> keySelector)
        {
            if (keySelector == null)
                throw new ArgumentNullException("grouper");

            Dictionary<K, List<V>> groupedValues = new Dictionary<K, List<V>>();

            foreach (V value in source)
            {
                // using delegate to get the value's key
                K key = keySelector(value);
                List<V> groupedValueList;

                // add a list for grouped values if the key is not already in Dictionary
                if (!groupedValues.TryGetValue(key, out groupedValueList))
                {
                    groupedValueList = new List<V>();
                    groupedValues.Add(key, groupedValueList);
                }

                groupedValueList.Add(value);
            }

            return groupedValues;
        }

        /// <summary>
        /// Adds the elements of the specified collection to the specified generic IList.
        /// </summary>
        /// <param name="initial">The list to add to.</param>
        /// <param name="addition">The collection of elements to add.</param>
        public static void AddRange<T>(IList<T> initial, IEnumerable<T> collection)
        {
            if (initial == null)
                throw new ArgumentNullException("initial");

            if (collection == null)
                return;

            foreach (T value in collection)
            {
                initial.Add(value);
            }
        }

        public static List<T> Distinct<T>(List<T> collection)
        {
            List<T> distinctList = new List<T>();

            foreach (T value in collection)
            {
                if (!distinctList.Contains(value))
                    distinctList.Add(value);
            }

            return distinctList;
        }

        public static List<List<T>> Flatten<T>(params IList<T>[] lists)
        {
            List<List<T>> flattened = new List<List<T>>();
            Dictionary<int, T> currentList = new Dictionary<int, T>();

            Recurse<T>(new List<IList<T>>(lists), 0, currentList, flattened);

            return flattened;
        }

        private static void Recurse<T>(IList<IList<T>> global, int current, Dictionary<int, T> currentSet, List<List<T>> flattenedResult)
        {
            IList<T> currentArray = global[current];

            for (int i = 0; i < currentArray.Count; i++)
            {
                currentSet[current] = currentArray[i];

                if (current == global.Count - 1)
                {
                    List<T> items = new List<T>();

                    for (int k = 0; k < currentSet.Count; k++)
                    {
                        items.Add(currentSet[k]);
                    }

                    flattenedResult.Add(items);
                }
                else
                {
                    Recurse(global, current + 1, currentSet, flattenedResult);
                }
            }
        }
    }
}