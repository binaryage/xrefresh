// Copyright (c) 2006, Antonin Hildebrand
// Copyright (c) 2006, Sven Groot, see license.txt for details
#pragma once

#define FS                                        FormatString

#define TRACE_I                                   TTrace::Debug()->Send
#define TRACE_E                                   WarningBeep(),TTrace::Error()->Send
#define TRACE_W                                   TTrace::Warning()->Send

#define TRACE_LI(x)                               Debug()->Send(x); TTrace::Debug()->Send(FS(_T("%s: %s"), id2.c_str(), x));
#define TRACE_LE(x)                               WarningBeep(),m_Trace->Error()->Send(x); TTrace::Error()->Send(FS(_T("%s: %s"), id2.c_str(), x));
#define TRACE_LW(x)                               Warning()->Send(x); TTrace::Warning()->Send(FS(_T("%s: %s"), id2.c_str(), x));


inline void WarningBeep() 
{
#ifdef _DEBUG
	Beep(200, 80);
#endif
}

inline void LongBeep() 
{
#ifdef _DEBUG
	Beep(1000, 380);
#endif
}

CString LoadStringResource(UINT id);
const TCHAR* FormatString(const TCHAR* format, ...);
CString VariantToString(VARIANT * va);

struct CCS
{
	CRITICAL_SECTION cs;

	CCS() {InitializeCriticalSection(&cs);}
	~CCS() {DeleteCriticalSection(&cs);}

	void Enter() {EnterCriticalSection(&cs);}
	void Leave() {LeaveCriticalSection(&cs);}
	bool CanEnter() { bool res = TryEnterCriticalSection(&cs)?true:false; if (res) Leave(); return res; }
};

class CPtrGuard {
public:
	CPtrGuard(void* ptr) : m_Ptr(ptr) { }
	~CPtrGuard() { delete m_Ptr; }
private:
	void* m_Ptr;
};

class CAPtrGuard {
public:
	CAPtrGuard(void* aptr) : m_APtr(aptr) { }
	~CAPtrGuard() { delete[] m_APtr; }
private:
	void* m_APtr;
};


class CCSGuard {
public:
	CCSGuard(CCS& cs) : m_CS(cs) { m_CS.Enter(); }
	~CCSGuard() { m_CS.Leave(); }
private:
	CCS&														m_CS;
};

#define CSGUARD(cs) CCSGuard zzz__csguard__zzz(cs);

#define SQL_ERROR_BODY(db, err, command) \
{\
	throw CXRefreshSQLError(err, FS(_T("SQL:  %s\nErr %d: %s\nFile: %s:%d"), command, err, sqlite3_errmsg16(db), _T(__FILE__), __LINE__));\
}

#if defined(_DEBUG)
#define CHECK_COM(hr, msg) \
{\
	HRESULT _hr_ = hr;\
	if (FAILED(_hr_)) \
	{ \
	CString s; \
	s.Format(_T("%s\n%s:%d"), msg, _T(__FILE__), __LINE__); \
	throw CXRefreshCOMError(_hr_, s); \
	}\
}
#else
#define CHECK_COM(hr, msg) \
{\
	HRESULT _hr_ = hr;\
	if (FAILED(_hr_)) \
	{ \
	throw CXRefreshCOMError(_hr_); \
	}\
}
#endif // _DEBUG

#if defined(_DEBUG)
#define CHECK(hr) \
{\
	HRESULT _hr_ = hr;\
	if (FAILED(_hr_)) \
{ \
	CString s; \
	s.Format(_T("%s:%d"), _T(__FILE__), __LINE__); \
	HandleError(CXRefreshCOMError(_hr_, s).ErrorMessage()); \
	DebugBreak();\
}\
}
#else
#define CHECK(hr) \
{\
	HRESULT _hr_ = hr;\
	if (FAILED(_hr_)) \
{ \
	throw CXRefreshCOMError(_hr_); \
}\
}
#endif // _DEBUG

extern DWORD gUIThreadId;
#define I_AM_UI_THREAD (GetCurrentThreadId()==gUIThreadId)
#define I_AM_BROWSER_THREAD(browserId) GetBrowserManager().IsBrowserThread(GetCurrentThreadId(), browserId)

extern HWND gUIThreadHWND;
#define UI_THREAD_HWND (ATLASSERT(gUIThreadHWND),gUIThreadHWND)

//#define WATCHE_EXITS_ENABLED

#if defined _DEBUG && defined WATCH_EXITS_ENABLED
#define WATCH_EXITS ExitWatcher my_function_watcher(__LINE__)
#else
#define WATCH_EXITS   
#endif

class ExitWatcher {
public:
	ExitWatcher(int line) : m_Line(line) {}
	~ExitWatcher() { TRACE_I(FS(_T("====> at line %d"), m_Line)); }

	int m_Line;
};

#define INDENT_WATCHER IndentWatcher my_indent_watcher(this)
//#define INDENT_WATCHER  

class IndentWatcher {
public:
	IndentWatcher(WinTrace* trace) : m_Trace(trace) { ATLASSERT(m_Trace); m_Trace->Debug()->Indent(); m_Trace->Error()->Indent(); m_Trace->Warning()->Indent(); }
	~IndentWatcher() { m_Trace->Debug()->UnIndent(); m_Trace->Error()->UnIndent(); m_Trace->Warning()->UnIndent(); }
private:
	WinTrace*												m_Trace;
};

#ifdef _DEBUG
#define CHECK_REFCOUNT_BEFORE_DELETE(p) { int refcount = p->AddRef()-1; ATLASSERT(refcount==1); p->Release(); }
#else
#define CHECK_REFCOUNT_BEFORE_DELETE(p) 
#endif

#ifdef _DEBUG
#define EXPECTED_REFCOUNT(p, count) ATLASSERT(p->AddRef()==count+1); p->Release();
#else
#define EXPECTED_REFCOUNT(p, count) 
#endif

#ifdef _DEBUG
#define CHECK_REFCOUNT_BEFORE_DELETE2(p) { IUnknown* ___x; p->QueryInterface(IID_IUnknown, (void**)&___x); int __res = ___x->AddRef(); ATLASSERT(__res==3); ___x->Release(); ___x->Release(); }
#else
#define CHECK_REFCOUNT_BEFORE_DELETE2(p) 
#endif

#define INIT_TRACE(name) WinTrace(FS(_T(#name) _T("[%08X]"), this), FS(_T(#name) _T("[%08X]"), this))

#define CHECK_THREAD_OWNERSHIP ATLASSERT(GetRoot().CheckThreadOwnership(GetResourceId()));

inline void HandleError(const CString &errorMessage)
{
	CString message = LoadStringResource(IDS_ERROR_BASEMESSAGE);
	message += errorMessage;
	MessageBox(NULL, message, LoadStringResource(IDS_TOOLBAR_NAME), MB_OK | MB_ICONERROR);
}

#define COM_INTERFACE_ENTRY_BREAK_NOT_FOUND()\
{NULL,\
	NULL,\
	_Break},

template <UINT nID, class T>
class CEasySink : public IDispEventImpl<nID, T>
{
public:
	HRESULT EasyAdvise(IUnknown* pUnk) 
	{ 
		AtlGetObjectSourceInterface(pUnk,
			&m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum);
		return DispEventAdvise(pUnk, &m_iid);
	}
	HRESULT EasyUnadvise(IUnknown* pUnk) 
	{
		AtlGetObjectSourceInterface(pUnk,
			&m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum);
		return DispEventUnadvise(pUnk, &m_iid);
	}
};

//////////////////////////////////////////////////////////////////////////

//#define DEBUG_COM_OBJECTS 1

#define IMPLEMENT_CLASS_NAME(name) static const TCHAR* GetClassNameForDebug() { return _T(#name); }

extern long g_ComObjectBreak;

namespace ATL {
	template <class Base>
	class CComObjectDebug : public CComObject<Base>
	{
	public:
		typedef CComObject<Base> _base;
		CComObjectDebug(void* x = NULL) throw() : _base(x), m_ThunkId(0) {}
		STDMETHOD_(ULONG, AddRef)() 
		{
			if (m_ThunkId) TRACE_I(FS(_T("[%04d] %s AddRef to %d"), m_ThunkId, m_Name, m_dwRef+1));
			if (m_ThunkId==g_ComObjectBreak) DebugBreak();
			return _base::AddRef();
		}
		STDMETHOD_(ULONG, Release)()
		{
			if (m_ThunkId) TRACE_I(FS(_T("[%04d] %s Release to %d"), m_ThunkId, m_Name, m_dwRef-1));
			if (m_ThunkId==g_ComObjectBreak) DebugBreak();
			return _base::Release();
		}
		static HRESULT WINAPI CreateInstance(CComObjectDebug<Base>** pp) throw();

		CString												m_Name;
		long													m_ThunkId;
	};

	extern long g_LastComObjectDebugId;

	template <class Base>
	HRESULT WINAPI CComObjectDebug<Base>::CreateInstance(CComObjectDebug<Base>** pp) throw()
	{
		static CCS cs;
		{
			CSGUARD(cs);
			g_LastComObjectDebugId++;
		}

		ATLASSERT(pp != NULL);
		if (pp == NULL)
			return E_POINTER;
		*pp = NULL;

		HRESULT hRes = E_OUTOFMEMORY;
		CComObjectDebug<Base>* p = NULL;
		ATLTRY(p = new CComObjectDebug<Base>())
			if (p != NULL)
			{
				p->m_ThunkId = g_LastComObjectDebugId;
				p->m_Name.Format(_T("%s[%08X]"), Base::GetClassNameForDebug(), p);
				p->SetVoid(NULL);
				p->InternalFinalConstructAddRef();
				hRes = p->_AtlInitialConstruct();
				if (SUCCEEDED(hRes))
					hRes = p->FinalConstruct();
				if (SUCCEEDED(hRes))
					hRes = p->_AtlFinalConstruct();
				p->InternalFinalConstructRelease();
				if (hRes != S_OK)
				{
					delete p;
					p = NULL;
				}
			}
			*pp = p;
			return hRes;
	}
}

#if defined _DEBUG && defined DEBUG_COM_OBJECTS
#define CComObject CComObjectDebug
#endif

typedef CComObject<CXRefreshScriptSite>		TScriptSite;

//////////////////////////////////////////////////////////////////////////
// Class that adds wstring support to runtimem_Error
class CXRefreshRuntimeError {
public:
	CXRefreshRuntimeError(const CString &message) : m_kMessage(message)
	{
		LPTSTR str = m_kMessage.LockBuffer();
		TRACE_E(str);
		m_kMessage.UnlockBuffer();
	}

	CXRefreshRuntimeError(const CXRefreshRuntimeError &right) : m_kMessage(right.ErrorMessage())
	{
		LPTSTR str = m_kMessage.LockBuffer();
		TRACE_E(str);
		m_kMessage.UnlockBuffer();
	}

	CXRefreshRuntimeError& operator=(const CXRefreshRuntimeError &right)
	{
		m_kMessage = right.ErrorMessage();
	}

	virtual ~CXRefreshRuntimeError()
	{
	}

	CString ErrorMessage() const
	{
		return m_kMessage;
	}

private:
	CString m_kMessage;
};

// Exception class for errors that occurred when calling Win32 API functions
class CXRefreshWindowsError : public CXRefreshRuntimeError {
public:
	CXRefreshWindowsError(HRESULT error, const CString &message) : CXRefreshRuntimeError(message), m_Error(error) { }
	CXRefreshWindowsError(HRESULT error) : CXRefreshRuntimeError(GetMessageFromErrorCode(error)), m_Error(error) { }
	CXRefreshWindowsError(const CXRefreshWindowsError &right) : CXRefreshRuntimeError(right), m_Error(right.m_Error) { }
	virtual ~CXRefreshWindowsError() { }

	CXRefreshWindowsError& operator=(const CXRefreshWindowsError &right)
	{
		CXRefreshRuntimeError::operator=(right);
		m_Error = right.m_Error;
	}

	HRESULT ErrorCode() const
	{
		return m_Error;
	}
private:
	HRESULT m_Error;

	static CString GetMessageFromErrorCode(DWORD error)
	{
		LPTSTR message;
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, error, LANG_USER_DEFAULT, reinterpret_cast<LPTSTR>(&message), 0, NULL) > 0 )
		{
			CString result = message;
			LocalFree(message);
			return result;
		}
		else
		{
			// If we couldn't get an error message, we'll build one using the error code
			CString s;
			s.Format(_T("Error code: %08X"), error);
			return s;
		}
	}
};

// Exception class for errors that occurred when calling Win32 API functions
class CXRefreshCOMError : public CXRefreshRuntimeError {
public:
	CXRefreshCOMError(HRESULT error, const CString &message) : CXRefreshRuntimeError(message+_T("\n")+GetMessageFromErrorCode(error)), m_Error(error) { }
	CXRefreshCOMError(HRESULT error) : CXRefreshRuntimeError(GetMessageFromErrorCode(error)), m_Error(error) { }
	CXRefreshCOMError(const CXRefreshCOMError &right) : CXRefreshRuntimeError(right), m_Error(right.m_Error) { }
	virtual ~CXRefreshCOMError() { }

	CXRefreshCOMError& operator=(const CXRefreshCOMError &right)
	{
		CXRefreshRuntimeError::operator=(right);
		m_Error = right.m_Error;
	}

	DWORD ErrorCode() const
	{
		return m_Error;
	}
private:
	static CString GetMessageFromErrorCode(HRESULT error)
	{
		LPTSTR message;
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, error, LANG_USER_DEFAULT, reinterpret_cast<LPTSTR>(&message), 0, NULL) > 0 )
		{
			CString s;
			s.Format(_T("HRESULT: %08X> %s"), error, message);
			LocalFree(message);
			return s;
		}
		else
		{
			// If we couldn't get an error message, we'll build one using the error code
			CString s;
			s.Format(_T("HRESULT: %08X"), error);
			return s;
		}
	}

	HRESULT m_Error;
};

// Exception class for errors that occurred when calling Win32 API functions
class CXRefreshSQLError : public CXRefreshRuntimeError {
public:
	CXRefreshSQLError(int error, const CString &message) : CXRefreshRuntimeError(message), m_Error(error) { }
	CXRefreshSQLError(int error) : CXRefreshRuntimeError(_T("?")), m_Error(error) { }
	CXRefreshSQLError(const CXRefreshSQLError &right) : CXRefreshRuntimeError(right), m_Error(right.m_Error) { }
	virtual ~CXRefreshSQLError() { }

	CXRefreshSQLError& operator=(const CXRefreshSQLError& right)
	{
		CXRefreshRuntimeError::operator=(right);
		m_Error = right.m_Error;
	}

	int ErrorCode() const
	{
		return m_Error;
	}
private:
	int m_Error;
};

////////////////////////////////////////////////////////////////
// MSDN Magazine -- August 2003
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET on Windows XP. Tab size=3.
//
// ---
// This class encapsulates the process of finding a window with a given class name
// as a descendant of a given window. To use it, instantiate like so:
//
//        CFindWnd fw(hwndParent,classname);
//
// fw.m_hWnd will be the HWND of the desired window, if found.
//
class CFindWnd {
private:
	//////////////////
	// This private function is used with EnumChildWindows to find the child
	// with a given class name. Returns FALSE if found (to stop enumerating).
	//
	static BOOL CALLBACK FindChildClassHwnd(HWND hwndParent, LPARAM lParam) {
		CFindWnd *pfw = (CFindWnd*)lParam;
		HWND hwnd = FindWindowEx(hwndParent, NULL, pfw->m_classname, NULL);
		if (hwnd) {
			pfw->m_hWnd = hwnd;    // found: save it
			return FALSE;            // stop enumerating
		}
		EnumChildWindows(hwndParent, FindChildClassHwnd, lParam); // recurse
		return TRUE;                // keep looking
	}

public:
	LPCTSTR m_classname;            // class name to look for
	HWND m_hWnd;                    // HWND if found

	// ctor does the work--just instantiate and go
	CFindWnd(HWND hwndParent, LPCTSTR classname)
		: m_hWnd(NULL), m_classname(classname)
	{
		FindChildClassHwnd(hwndParent, (LPARAM)this);
	}
};

//////////////////////////////////////////////////////////////////////////
// custom global functions
template<class T>
class CComPtrHasher : public stdext::hash_compare<CComPtr<T> > {
public:
	inline const size_t operator()(const CComPtr<T> &s) const
	{
		return (size_t)(void*)s;
	}
	inline bool operator()(const CComPtr<T> &a, const CComPtr<T> &b) const
	{
		return a<b;
	}
};

inline CComPtr<IHTMLDocument2>                        
GetDocument(CComPtr<IWebBrowser2> spBrowser, bool bWait = false, bool bReport=true)
{
	// we should not use document waiting
	ATLASSERT(!bWait);
	// query document dispatch
	HRESULT hr = S_OK;
	DWORD dwCnt = GetTickCount();
	bool bFirstTime = true;
	while (1) 
	{
		CComPtr<IDispatch> spDocument;
		HRESULT hr = spBrowser->get_Document(&spDocument);
		if (SUCCEEDED(hr) || !bWait) 
		{
			if (FAILED(hr) && bReport)
			{
				TRACE_W(_T("Document not available"));
			}
			// ...and query for an HTML document.
			CComQIPtr<IHTMLDocument2> spHTMLDoc = spDocument;
			return spHTMLDoc;
		}

		// failed - document is not ready
		if (bFirstTime && bReport)
		{
			TRACE_W(_T("Document not ready !"));
			bFirstTime = false;
		}

		// there may be some cases where we never get READYSTATE_COMPLETE
		// set a 10 second timeout for insurance
		if (GetTickCount() - dwCnt > 30000)
			break;

		// don't hog the CPU.
		Sleep(WAIT_GRANULARITY);

		// process some messages, otherwise we never get the body
		// BEWARE! other parts of the code can be called when dispatching messages
		//         risk deadlocks !
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	::DispatchMessage(&msg);
	}
	TRACE_E(_T("Failed to get document !!!"));
	return NULL;	
}

inline CComPtr<IHTMLElement>
GetDocumentBody(CComPtr<IHTMLDocument2> spDocument, bool bWait=true)
{
	ATLASSERT(!!spDocument);
	// http://support.microsoft.com/kb/188763
	// it says: although there is ready state "complete" in spDocument, body does not to be ready in case of IFRAME
	//          and this "feature" is BY DESIGN :-)
	CComBSTR state;
	HRESULT hr = S_OK;
	DWORD dwCnt = GetTickCount();
	bool bFirstTime = true;
	while (SUCCEEDED(hr)) 
	{
		CComPtr<IHTMLElement> spBody;
		hr = spDocument->get_body(&spBody);
		ATLASSERT(SUCCEEDED(hr));
		if (!bWait || !!spBody) return spBody;

		if (bFirstTime)
		{
			TRACE_W(_T("Document body not available !"));
			bFirstTime = false;
		}

		// there may be some cases where we never get READYSTATE_COMPLETE
		// set a 10 second timeout for insurance
		if (GetTickCount() - dwCnt > 10000)
			break;

		// don't hog the CPU.
		Sleep(WAIT_GRANULARITY);

		// process some messages, otherwise we never get the body
		// BEWARE! other parts of the code can be called when dispatching messages
		//         risk deadlocks !
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	::DispatchMessage(&msg);
	}
	TRACE_E(_T("Failed to get document body !!!"));
	return NULL;
}

inline bool 
WaitForDocumentReadyState(CComPtr<IHTMLDocument2> spDocument)
{
	// the idea from PopupBlocker2 by Osborn Technologies 
	// (http://www.codeproject.com/atl/popupblocker2.asp)
	CComBSTR state;
	HRESULT hr = S_OK;
	DWORD dwCnt = GetTickCount();
	bool bFirstTime = true;
	while (SUCCEEDED(hr)) 
	{
		hr = spDocument->get_readyState(&state);
		ATLASSERT(SUCCEEDED(hr));
		CString s(state);
		if (s==_T("complete")) 
			return SUCCEEDED(hr);

		if (bFirstTime)
		{
			TRACE_W(_T("Document attached when not completed!"));
			bFirstTime = false;
		}

		// there may be some cases where we never get READYSTATE_COMPLETE
		// set a 30 second timeout for insurance
		if (GetTickCount() - dwCnt > 30000)
		{
			break;
		}

		// don't hog the CPU.
		Sleep(WAIT_GRANULARITY);

		// process some messages, otherwise we never get READYSTATE_COMPLETE.
		// BEWARE! other parts of the code can be called when dispatching messages
		//         the risk of deadlocks !
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	::DispatchMessage(&msg);
	}
	TRACE_E(_T("Failed to wait for document ready state !!!"));
	return S_FALSE;
}

inline CString
GetURL(CComPtr<IHTMLDocument2> spDocument)
{
	ATLASSERT(!!spDocument);
	CComBSTR url;
	CHECK_COM(spDocument->get_URL(&url), FS(_T("Cannot retrieve location URL from document %08X"), spDocument));
	return CString(url);
}

inline CString
GetURL(CComPtr<IWebBrowser2> spBrowser)
{
	ATLASSERT(!!spBrowser);
	CComPtr<IHTMLDocument2> doc = GetDocument(spBrowser);
	if (!doc) return _T("");
	return GetURL(doc);
}

inline CString
GetTitle(CComPtr<IHTMLDocument2> spDocument)
{
	ATLASSERT(!!spDocument);
	CComBSTR title;
	CHECK_COM(spDocument->get_title(&title), FS(_T("Cannot retrieve title from document %08X"), spDocument));
	return CString(title);
}

inline CString
GetTitle(CComPtr<IWebBrowser2> spBrowser)
{
	ATLASSERT(!!spBrowser);
	CComPtr<IHTMLDocument2> doc = GetDocument(spBrowser);
	if (!doc) return _T("");
	return GetTitle(doc);
}

inline CString
GetLocationName(CComPtr<IWebBrowser2> spBrowser)
{
	ATLASSERT(!!spBrowser);
	CComBSTR title;
	CHECK_COM(spBrowser->get_LocationName(&title), FS(_T("Cannot retrieve location name from browser %08X"), spBrowser));
	return CString(title);
}

inline CString
GetLocationURL(CComPtr<IWebBrowser2> spBrowser)
{
	ATLASSERT(!!spBrowser);
	CComBSTR title;
	CHECK_COM(spBrowser->get_LocationURL(&title), FS(_T("Cannot retrieve location URL from browser %08X"), spBrowser));
	return CString(title);
}

inline CString
GetSiteRootUrl(CComPtr<IWebBrowser2> spBrowser)
{
	ATLASSERT(!!spBrowser);
	CString url = GetURL(spBrowser);
	int index = url.Find(_T("//"));
	if (index<0) index = 0; else index+=2;
	int trim = url.Find(_T("/"), index);
	//int trim = url.Find(_T("?"), index);
	if (trim!=-1)
	{
		url.Delete(trim, url.GetLength()-trim);
	}
	return url;
}
