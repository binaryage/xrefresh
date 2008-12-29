using System;

namespace XRefresh
{
	class CancelException : Exception
	{
	}

	class ModelException : Exception
	{
		public ModelException(string message) : base(message)
		{

		}
	}
}
