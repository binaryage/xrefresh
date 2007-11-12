// Copyright (c) 2007 Michael Chapman
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


using System;

namespace FlexFieldControlLib
{
   internal class CedeFocusEventArgs : EventArgs
   {
      public Action Action
      {
         get
         {
            return _action;
         }
         set
         {
            _action = value;
         }
      }

      public Direction Direction
      {
         get
         {
            return _direction;
         }
         set
         {
            _direction = value;
         }
      }

      public int FieldIndex
      {
         get
         {
            return _fieldIndex;
         }
         set
         {
            _fieldIndex = value;
         }
      }

      public Selection Selection
      {
         get
         {
            return _selection;
         }
         set
         {
            _selection = value;
         }
      }

      private Action _action;
      private Direction _direction;
      private int _fieldIndex;
      private Selection _selection;
   }

   internal enum Selection
   {
      None,
      All
   }

   internal enum Action
   {
      None,
      Trim,
      Home,
      End
   }

   internal enum Direction
   {
      Forward,
      Reverse
   }
}
