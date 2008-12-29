using System;
using System.Collections.Generic;
using System.Text;
using System.Web;
using System.Web.Caching;
using System.Security.Principal;

namespace Newtonsoft.Json.Utilities
{
	public abstract class HandlerBase
	{
		private HttpContext _context;

		protected HttpContext Context
		{
			get
			{
				if (_context == null)
					_context = HttpContext.Current;

				return _context;
			}
			set { _context = value; }
		}

		protected HttpApplicationState Application
		{
			get
			{
				if (Context != null)
					return Context.Application;

				return null;
			}
		}

		protected HttpApplication ApplicationInstance
		{
			get
			{
				if (Context != null)
					return Context.ApplicationInstance;

				return null;
			}
		}

		protected Cache Cache
		{
			get
			{
				if (Context != null)
					return Context.Cache;

				return null;
			}
		}

		protected HttpRequest Request
		{
			get
			{
				if (Context != null)
					return Context.Request;

				return null;
			}
		}

		protected HttpResponse Response
		{
			get
			{
				if (Context != null)
					return Context.Response;

				return null;
			}
		}

		protected HttpServerUtility Server
		{
			get
			{
				if (Context != null)
					return Context.Server;

				return null;
			}
		}

		protected TraceContext Trace
		{
			get
			{
				if (Context != null)
					return Context.Trace;

				return null;
			}
		}

		public IPrincipal User
		{
			get
			{
				if (Context != null)
					return Context.User;

				return null;
			}
		}
	}
}
