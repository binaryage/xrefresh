//========================================================================================
//
// Author:          Pascal Hurni
// Creation Date:	12.12.2003
//
// Copyright 2003-2004 Mortimer Systems
//
// This software is free. It is licensed under the BSDL (3-clauses BSD), please refer to it
// for details.
//
//========================================================================================

#ifndef __WTLHELPERS_H_FUNKYGUID_92618941__
#define __WTLHELPERS_H_FUNKYGUID_92618941__

#ifndef _WTLHELPERS_CSTRING

#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)
#define _WTLHELPERS_CSTRING
#endif

#if defined(_WTL_USE_CSTRING)
#define _WTLHELPERS_CSTRING
#endif

#if defined(_WTL_NO_CSTRING) || defined(_ATL_TMP_NO_CSTRING)
#undef _WTLHELPERS_CSTRING
#endif

#endif // _WTLHELPERS_CSTRING

//========================================================================================
// extending ATL
//========================================================================================
namespace ATL {
//========================================================================================


//----------------------------------------------------------------------------------------
// Override of CComObjectStack that doesn't freak out and assert when 
// the IUnknown methods are called.
//
// Unknown author

#ifdef __ATLCOM_H__
	
template <class Base>
class CComObjectStack2 : public CComObjectStack<Base>
{
public:
    CComObjectStack2() : CComObjectStack<Base>()
    { }
    ~CComObjectStack2()
    { }

    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }

    STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject)
    { 
        return _InternalQueryInterface(iid, ppvObject); 
    }
};

#endif // __ATLCOM_H__


//----------------------------------------------------------------------------------------
// CSimpleDialog2 - Prebuilt modal dialog that uses standard buttons
// This version simply makes OnCloseCmd virtual, so that derivates can react
// on close. (Get some interesting info from controls)
//
// Pascal Hurni, 2003

#ifdef __ATLWIN_H__

template <WORD t_wDlgTemplateID, BOOL t_bCenter = TRUE>
class CSimpleDialog2 : public CDialogImplBase
{
public:
	int DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		ATLASSERT(m_hWnd == NULL);
		_Module.AddCreateWndData(&m_thunk.cd, (CDialogImplBase*)this);
		int nRet = ::DialogBox(_Module.GetResourceInstance(),
			MAKEINTRESOURCE(t_wDlgTemplateID), hWndParent, (DLGPROC)StartDialogProc);
		m_hWnd = NULL;
		return nRet;
	}

	typedef CSimpleDialog2<t_wDlgTemplateID, t_bCenter>	thisClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDOK, IDNO, OnCloseCmd)
	END_MSG_MAP()

	virtual LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(t_bCenter)
			CenterWindow(GetParent());
		return TRUE;
	}

	virtual LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		::EndDialog(m_hWnd, wID);
		return 0;
	}
};


//----------------------------------------------------------------------------------------
// Automatic window
// Use like CWindowImpl<>
//
// Windows of this kind must be created with ::CreateWindow() passing the
// window class. New window objects are automatically created and destroyed.
//
// This is usefull when adding custom control to a dialog, simply set the
// correct Window Class and your controls will be automatically created at
// dialog creation time.
//
// Before any window creation, register its class with something like this
//		CMyControl::GetWndClassInfo().Register(NULL);
//
// Pascal Hurni, april 2004

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CAutoWindowImpl : public CWindowImplBaseT< TBase, TWinTraits >
{
public:
	DECLARE_WND_CLASS(NULL)

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		if (T::GetWndClassInfo().m_lpszOrigName == NULL)
			T::GetWndClassInfo().m_lpszOrigName = GetWndClassName();
		ATOM atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);

		dwStyle = T::GetWndStyle(dwStyle);
		dwExStyle = T::GetWndExStyle(dwExStyle);

		return CWindowImplBaseT< TBase, TWinTraits >::Create(hWndParent, rcPos, szWindowName,
			dwStyle, dwExStyle, nID, atom, lpCreateParam);
	}

	static LRESULT CALLBACK StartWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		T* pThis = new T;
		ATLASSERT(pThis != NULL);
		pThis->m_hWnd = hWnd;
		pThis->m_thunk.Init(pThis->GetWindowProc(), pThis);
		WNDPROC pProc = (WNDPROC)&(pThis->m_thunk.thunk);
		WNDPROC pOldProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)pProc);
	#ifdef _DEBUG
		// check if somebody has subclassed us already since we discard it
		if(pOldProc != StartWindowProc)
			ATLTRACE2(atlTraceWindowing, 0, _T("Subclassing through a hook discarded.\n"));
	#else
		pOldProc;	// avoid unused warning
	#endif
		return pProc(hWnd, uMsg, wParam, lParam);
	}

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete static_cast<T*>(this);
	}
};

#endif // __ATLWIN_H__

//----------------------------------------------------------------------------------------
// Extension to CSimpleArray and CSimpleMap to allow operator=
//
// Pascal Hurni, october 2004

#if defined(__ATLBASE_H__)

template <class T>
class CSimpleArray2 : public CSimpleArray<T>
{
public:
	CSimpleArray2<T>& operator=(const CSimpleArray2<T>& src)
	{
		RemoveAll();
		m_aT = (T*)malloc(src.GetSize() * sizeof(T));
		if (m_aT != NULL)
			m_nAllocSize = src.GetSize();
		for (int i=0; i<src.GetSize(); i++)
			Add(src[i]);
		return *this;
	}
};

template <class TKey, class TVal>
class CSimpleMap2 : public CSimpleMap<TKey, TVal>
{
public:
	CSimpleMap2<TKey, TVal>& operator=(const CSimpleMap2<TKey, TVal>& src)
	{
		RemoveAll();
		m_aKey = (TKey*)malloc(src.GetSize() * sizeof(TKey));
		m_aVal = (TVal*)malloc(src.GetSize() * sizeof(TVal));
		if (!m_aKey || !m_aVal)
			return *this;
		m_nSize = src.GetSize();
		for (int i=0; i<src.GetSize(); i++)
			SetAtIndex(i, src.m_aKey[i], src.m_aVal[i]);
		return *this;
	}
};

#endif // __ATLBASE_H__


//----------------------------------------------------------------------------------------
// An implementation for CComEnum that connects to a CSimpleArray<>
// (much like CComEnumOnSTL works for STL collections)
//
// Pascal Hurni, july 2004

#if defined(__ATLBASE_H__) && defined(__ATLCOM_H__)

template <class Base, const IID* piid, class T, class Copy, class CollType>
class ATL_NO_VTABLE IEnumOnCArrayImpl : public Base
{
public:
	HRESULT Init(IUnknown *pUnkForRelease, CollType& collection)
	{
		m_spUnk = pUnkForRelease;
		m_pcollection = &collection;
		m_iter = 0;
		return S_OK;
	}
	STDMETHOD(Next)(ULONG celt, T* rgelt, ULONG* pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)(void)
	{
		if (m_pcollection == NULL)
			return E_FAIL;
		m_iter = 0;
		return S_OK;
	}
	STDMETHOD(Clone)(Base** ppEnum);
//Data
	CComPtr<IUnknown> m_spUnk;
	CollType* m_pcollection;
	int m_iter;
};

template <class Base, const IID* piid, class T, class Copy, class CollType>
STDMETHODIMP IEnumOnCArrayImpl<Base, piid, T, Copy, CollType>::Next(ULONG celt, T* rgelt, ULONG* pceltFetched)
{
	if (rgelt == NULL || (celt != 1 && pceltFetched == NULL))
		return E_POINTER;
	if (m_pcollection == NULL)
		return E_FAIL;

	ULONG nActual = 0;
	HRESULT hr = S_OK;
	T* pelt = rgelt;
	while (SUCCEEDED(hr) && m_iter != m_pcollection->GetSize() && nActual < celt)
	{
		hr = Copy::copy(pelt, &m_pcollection->operator[](m_iter));
		if (FAILED(hr))
		{
			while (rgelt < pelt)
				Copy::destroy(rgelt++);
			nActual = 0;
		}
		else
		{
			pelt++;
			m_iter++;
			nActual++;
		}
	}
	if (pceltFetched)
		*pceltFetched = nActual;
	if (SUCCEEDED(hr) && (nActual < celt))
		hr = S_FALSE;
	return hr;
}

template <class Base, const IID* piid, class T, class Copy, class CollType>
STDMETHODIMP IEnumOnCArrayImpl<Base, piid, T, Copy, CollType>::Skip(ULONG celt)
{
	HRESULT hr = S_OK;
	while (celt--)
	{
		if (m_iter != m_pcollection->GetSize())
			m_iter++;
		else
		{
			hr = S_FALSE;
			break;
		}
	}
	return hr;
}

template <class Base, const IID* piid, class T, class Copy, class CollType>
STDMETHODIMP IEnumOnCArrayImpl<Base, piid, T, Copy, CollType>::Clone(Base** ppEnum)
{
	typedef CComObject<CComEnumOnCArray<Base, piid, T, Copy, CollType> > _class;
	HRESULT hRes = E_POINTER;
	if (ppEnum != NULL)
	{
		*ppEnum = NULL;
		_class* p;
		hRes = _class::CreateInstance(&p);
		if (SUCCEEDED(hRes))
		{
			hRes = p->Init(m_spUnk, *m_pcollection);
			if (SUCCEEDED(hRes))
			{
				p->m_iter = m_iter;
				hRes = p->_InternalQueryInterface(*piid, (void**)ppEnum);
			}
			if (FAILED(hRes))
				delete p;
		}
	}
	return hRes;
}

template <class Base, const IID* piid, class T, class Copy, class CollType, class ThreadModel = CComObjectThreadModel>
class ATL_NO_VTABLE CComEnumOnCArray :
	public IEnumOnCArrayImpl<Base, piid, T, Copy, CollType>,
	public CComObjectRootEx< ThreadModel >
{
public:
	typedef CComEnumOnCArray<Base, piid, T, Copy, CollType, ThreadModel > _CComEnum;
	typedef IEnumOnCArrayImpl<Base, piid, T, Copy, CollType > _CComEnumBase;
	BEGIN_COM_MAP(_CComEnum)
		COM_INTERFACE_ENTRY_IID(*piid, _CComEnumBase)
	END_COM_MAP()
};

#endif


//----------------------------------------------------------------------------------------
// CStringEx - String class extensions
//
// Pascal Hurni, 2003
//
// Well, the constructor I added which LoadString(), already existed in CString
// with the LPCTSTR constructor. So my new class is worthless. I made the CopyTo()
// member a simple function

#ifdef _WTLHELPERS_CSTRING

//----------------------------------------------------------------------------------------
// Copy a CString to a supplied buffer, Target is either ANSI or UNICODE

inline void CStringCopyToA(CString &Str, LPSTR Target, UINT TargetMaxLen = -1)		// Yes -1 for UINT !
{
	UINT CopyLen = Str.GetLength();
	if (CopyLen+1 > TargetMaxLen)
		CopyLen = TargetMaxLen-1;

#ifdef _UNICODE
	size_t chars;
	wcstombs_s(&chars, Target, TargetMaxLen, Str, CopyLen);
#else
	strncpy(Target, Str, CopyLen);
#endif
	Target[CopyLen] = '\0';
}

inline void CStringCopyToW(CString &Str, LPWSTR Target, UINT TargetMaxLen = -1)		// Yes -1 for UINT !
{
	UINT CopyLen = Str.GetLength();
	if (CopyLen+1 > TargetMaxLen)
		CopyLen = TargetMaxLen-1;

#ifdef _UNICODE
	wcsncpy_s(Target, TargetMaxLen, Str, CopyLen);
#else
	mbstowcs(Target, Str, CopyLen);
#endif
	Target[CopyLen] = '\0';
}

#ifdef _UNICODE
	#define CStringCopyTo CStringCopyToW
#else
	#define CStringCopyTo CStringCopyToA
#endif

//----------------------------------------------------------------------------------------
// Tiny class to supply on-the-fly string conversion for ANSI/UNICODE
//
// Used for function that wants not TCHAR strings but explicitely WCHAR or CHAR.
// Example:
//		void DoSomeThingsWithUnicodeString(LPCWSTR lpszText);
//
//		TCHAR SomeText[128];
//		_tcscpy(SomeText, _T("Foo Bar"));
//		DoSomeThingsWithUnicodeString( CWideStringRef(SomeText).GetRef() );
//
//		CString AnotherText = _T("Bar Foo");
//		DoSomeThingsWithUnicodeString( CWideStringRef(AnotherText).GetRef() );
//
// When _UNICODE is defined, a simple reference to the passed string is returned by GetRef(), thus
// only stack based allocation are made (quick).
// When it is not defined (ANSI), a conversion is made and GetRef() returns a local copy.
// (Stack based if not more than STACK_STRING_LEN chars. If more, heap based)

class CWideStringRef
{
public:
	CWideStringRef(LPCSTR Original)
	{
		int OriginalLen = (int)strlen(Original);
		m_String = m_StackString;
		m_Allocated = false;
		if (OriginalLen > STACK_STRING_LEN)
			m_Allocated = (m_String = new WCHAR[OriginalLen+1]) != NULL;

		if (m_String)
			::MultiByteToWideChar(CP_ACP, 0, Original, -1, m_String, OriginalLen+1);
	}

	CWideStringRef(LPCWSTR Original)
	{
		m_Allocated = false;
		m_String = const_cast<LPWSTR>(Original);
	}

	~CWideStringRef()
	{
		if (m_Allocated)
			delete [] m_String;
	}

	// Not const (LPCWSTR) because some nasty functions prototypes are not.
	LPWSTR GetRef()
	{
		return m_String;
	}

	operator LPWSTR() const
	{
		return m_String;
	}

protected:
	LPWSTR m_String;
	enum { STACK_STRING_LEN = 255 };
	bool m_Allocated;
	WCHAR m_StackString[STACK_STRING_LEN+1];
};

class CAnsiStringRef
{
public:
	CAnsiStringRef(LPCWSTR Original)
	{
		int OriginalLen = (int)wcslen(Original);
		m_String = m_StackString;
		m_Allocated = false;
		if (OriginalLen > STACK_STRING_LEN)
			m_Allocated = (m_String = new CHAR[OriginalLen+1]) != NULL;

		if (m_String)
			::WideCharToMultiByte(CP_ACP, 0, Original, -1, m_String, OriginalLen+1, NULL, NULL);
	}

	CAnsiStringRef(LPCSTR Original)
	{
		m_String = const_cast<LPSTR>(Original);
	}

	~CAnsiStringRef()
	{
		if (m_Allocated)
			delete [] m_String;
	}

	// Not const (LPCSTR) because some nasty functions prototypes are not.
	LPSTR GetRef()
	{
		return m_String;
	}

	operator LPSTR() const
	{
		return m_String;
	}

protected:
	LPSTR m_String;
	enum { STACK_STRING_LEN = 255 };
	bool m_Allocated;
	CHAR m_StackString[STACK_STRING_LEN+1];
};

#endif

//----------------------------------------------------------------------------------------
// CRegKeyEx - CRegKey extension to handle CString
//
// Pascal Hurni, 2003

#ifdef _WTLHELPERS_CSTRING

class CRegKeyEx : public CRegKey
{
public:

	inline LONG QueryValue(CString &Value, LPCTSTR lpszValueName)
	{
		// First determine size
		DWORD dwType = NULL;
		DWORD dwCount;
		LONG lRes = RegQueryValueEx(m_hKey, (LPTSTR)lpszValueName, NULL, &dwType, NULL, &dwCount);
//		ATLASSERT(lRes == ERROR_SUCCESS);
		if (lRes!=ERROR_SUCCESS)
			return lRes;

		dwType = NULL;
		lRes = RegQueryValueEx(m_hKey, (LPTSTR)lpszValueName, NULL, &dwType,
			(LPBYTE)Value.GetBuffer(dwCount), &dwCount);
		Value.ReleaseBuffer();
		ATLASSERT((lRes!=ERROR_SUCCESS) || (dwType == REG_SZ) ||
				 (dwType == REG_MULTI_SZ) || (dwType == REG_EXPAND_SZ));
		return lRes;
	}


};

#endif // _WTLHELPERS_CSTRING

//----------------------------------------------------------------------------------------
// CString helper for GetWindowText()

#ifdef _WTLHELPERS_CSTRING

inline bool GetWndText(HWND hWnd, CString& strText)
{
	ATLASSERT(::IsWindow(hWnd));
	int TextLen = ::GetWindowTextLength(hWnd);
	LPTSTR lpstr = strText.GetBufferSetLength(TextLen);
	if (lpstr == NULL)
		return false;

	TextLen = ::GetWindowText(hWnd, lpstr, TextLen+1);
	strText.ReleaseBuffer();
	return TextLen != 0;
}

#endif // _WTLHELPERS_CSTRING

//----------------------------------------------------------------------------------------
// function to determine if the running app is really themed
//
// Pascal Hurni, spring 2004

#ifdef __ATLTHEME_H__

inline bool IsAppReallyThemed()
{
	if (CTheme::IsThemingSupported() && ::IsThemeActive() && ::IsAppThemed())
	{
		DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(GetModuleHandle(_T("comctl32.dll")), ("DllGetVersion"));
		if(pDllGetVersion)
		{
			DLLVERSIONINFO dvi;
			ZeroMemory(&dvi, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);
			if (SUCCEEDED( (*pDllGetVersion)(&dvi) ) && (dvi.dwMajorVersion >= 6))
				return true;
		}
	}
	return false;
}

#endif // __ATLTHEME_H__

#ifdef __ATLTHEME_H__
	#define IS_APP_REALLY_THEMED	(IsAppReallyThemed())
#else
	#define IS_APP_REALLY_THEMED	(false)
#endif

//========================================================================================
};	// namespace ATL


//========================================================================================
// extending WTL
//========================================================================================
namespace WTL {
//========================================================================================

//----------------------------------------------------------------------------------------
// CWindowImpl that permit self destruction at OnFinalMessage().

#ifdef __ATLWIN_H__

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CWindowSelfDestructImpl : public CWindowImplBaseT< TBase, TWinTraits >
{
public:
	DECLARE_WND_CLASS(NULL)

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		if (T::GetWndClassInfo().m_lpszOrigName == NULL)
			T::GetWndClassInfo().m_lpszOrigName = GetWndClassName();
		ATOM atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);

		dwStyle = T::GetWndStyle(dwStyle);
		dwExStyle = T::GetWndExStyle(dwExStyle);

		m_MessageDepth = 0;
		m_DestroyMe = false;

		return CWindowImplBaseT< TBase, TWinTraits >::Create(hWndParent, rcPos, szWindowName,
			dwStyle, dwExStyle, nID, atom, lpCreateParam);
	}

	virtual WNDPROC GetWindowProc()
	{
		return SelfDestructWindowProc;
	}

	static LRESULT CALLBACK SelfDestructWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		T* pThis = (T*)hWnd;
		// set a ptr to this message and save the old value
		MSG msg = { pThis->m_hWnd, uMsg, wParam, lParam, 0, { 0, 0 } };
		const MSG* pOldMsg = pThis->m_pCurrentMsg;
		pThis->m_pCurrentMsg = &msg;
		// pass to the message map to process
		LRESULT lRes;

		pThis->m_MessageDepth++;
		BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);
		pThis->m_MessageDepth--;
		
		// restore saved value for the current message
		ATLASSERT(pThis->m_pCurrentMsg == &msg);
		pThis->m_pCurrentMsg = pOldMsg;
		// do the default processing if message was not handled
		if(!bRet)
		{
			if(uMsg != WM_NCDESTROY)
				lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
			else
				pThis->m_DestroyMe = true;
		}

		if (pThis->m_DestroyMe && pThis->m_MessageDepth == 0)
		{
			// unsubclass, if needed
			LONG pfnWndProc = ::GetWindowLong(pThis->m_hWnd, GWL_WNDPROC);
			lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
			if(pThis->m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLong(pThis->m_hWnd, GWL_WNDPROC) == pfnWndProc)
				::SetWindowLong(pThis->m_hWnd, GWL_WNDPROC, (LONG)pThis->m_pfnSuperWindowProc);
			// clear out window handle
			HWND hWnd = pThis->m_hWnd;
			pThis->m_hWnd = NULL;
			// clean up after window is destroyed
			pThis->OnFinalMessage(hWnd);
		}

		return lRes;
	}

protected:
	LONG m_MessageDepth;
	bool m_DestroyMe;
};

#endif // __ATLWIN_H__

//----------------------------------------------------------------------------------------
// CDialogImpl that add CMessageFilter behaviour (PreTranslateMessage)

#ifdef __ATLAPP_H__

template <class T, class TBase = CWindow>
class CDialogFilterImpl : public CDialogImpl<T, TBase>, public CMessageLoop
{
public:
	virtual WNDPROC GetDialogProc()
	{
		return FilterDialogProc;
	}
	static LRESULT CALLBACK FilterDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		T* pThis = (T*)hWnd;
		// set a ptr to this message and save the old value
		MSG msg = { pThis->m_hWnd, uMsg, wParam, lParam, 0, { 0, 0 } };
		const MSG* pOldMsg = pThis->m_pCurrentMsg;
		pThis->m_pCurrentMsg = &msg;

		// TODO: Add handling of idle message

		if (!pThis->PreTranslateMessage(&msg))
		{
			// pass to the message map to process
			LRESULT lRes;
			BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);
			// restore saved value for the current message
			ATLASSERT(pThis->m_pCurrentMsg == &msg);
			pThis->m_pCurrentMsg = pOldMsg;
			// set result if message was handled
			if(bRet)
			{
				switch (uMsg)
				{
				case WM_COMPAREITEM:
				case WM_VKEYTOITEM:
				case WM_CHARTOITEM:
				case WM_INITDIALOG:
				case WM_QUERYDRAGICON:
				case WM_CTLCOLORMSGBOX:
				case WM_CTLCOLOREDIT:
				case WM_CTLCOLORLISTBOX:
				case WM_CTLCOLORBTN:
				case WM_CTLCOLORDLG:
				case WM_CTLCOLORSCROLLBAR:
				case WM_CTLCOLORSTATIC:
					return lRes;
					break;
				}
				::SetWindowLong(pThis->m_hWnd, DWL_MSGRESULT, lRes);
				return TRUE;
			}
		}
		if(uMsg == WM_NCDESTROY)
		{
			// clear out window handle
			HWND hWnd = pThis->m_hWnd;
			pThis->m_hWnd = NULL;
			// clean up after dialog is destroyed
			pThis->OnFinalMessage(hWnd);
		}
		return FALSE;
	}
};

#endif // __ATLAPP_H__

//----------------------------------------------------------------------------------------
// Usefull addons, see <atlddx.h> for the base one
//
// Pascal Hurni, 2003

#ifdef __ATLDDX_H__

// Use bool vars with DDX
#define DDX_CHECKB(nID, var) \
	if(nCtlID == (UINT)-1 || nCtlID == nID) { \
		int nValue = (int)var; \
		DDX_Check(nID, nValue, bSaveAndValidate); \
		var = nValue?true:false; }

// Use flags (as bit mask) with DDX
#define DDX_CHECK_FLAGS(nID, var, mask) \
	if(nCtlID == (UINT)-1 || nCtlID == nID) { \
		int nCheck = var & mask ? 1 : 0; \
		DDX_Check(nID, nCheck, bSaveAndValidate); \
		if (bSaveAndValidate) { if (nCheck) var |= mask; else var &= ~mask; } }


#ifdef _WTLHELPERS_CSTRING

#define DDX_LISTBOX_STL(nID, var) \
		if(nCtlID == (UINT)-1 || nCtlID == nID) \
		{ \
			if(!DDX_ListboxSTL(nID, var, bSaveAndValidate)) \
				return FALSE; \
		}
#endif // _WTLHELPERS_CSTRING

#define DDX_COMBOBOX_STL(nID, var, selectedval) \
		if(nCtlID == (UINT)-1 || nCtlID == nID) \
		{ \
			if(!DDX_ComboboxSTL(nID, var, selectedval, bSaveAndValidate)) \
				return FALSE; \
		}
	
template <class T>
class CWinDataExchangeEx : public CWinDataExchange<T>
{
public:

#ifdef _WTLHELPERS_CSTRING
	template <class TSTLColl> 
	BOOL DDX_ListboxSTL(UINT nID, TSTLColl &Coll, BOOL bSave, BOOL UseDelta = FALSE)
	{
		T* pT = static_cast<T*>(this);
		BOOL bSuccess = TRUE;

		WTL::CListBox ListBox = pT->GetDlgItem(nID);

		if(bSave)
		{
			if (UseDelta)
			{
			}
			else
			{
				CString strItem;
				Coll.clear();
				for (int i=0; i<ListBox.GetCount(); i++)
				{
					if (LB_ERR != ListBox.GetText(i, strItem))
					{
						TSTLColl::value_type Item(strItem);
						Coll.push_back(Item);
					}
				}
			}
		}
		else
		{
			ListBox.ResetContent();

			for(TSTLColl::iterator it = Coll.begin(); it != Coll.end(); it++)
			{
				ListBox.AddString(*it);
			}
		}

		if(!bSuccess)
		{
			pT->OnDataExchangeError(nID, bSave);
		}
		return bSuccess;
	}

	//template <class TSTLColl> 
	//BOOL DDX_ComboboxSTL(UINT nID, TSTLColl &Coll, TSTLColl::value_type &SelectedVal, BOOL bSave)
	//{
	//	T* pT = static_cast<T*>(this);
	//	BOOL bSuccess = TRUE;

	//	WTL::CComboBox ComboBox = pT->GetDlgItem(nID);

	//	if(bSave)
	//	{
	//		CString strItem;
	//		Coll.clear();
	//		for (int i=0; i<ComboBox.GetCount(); i++)
	//		{
	//			if (LB_ERR != ComboBox.GetLBText(i, strItem))
	//			{
	//				TSTLColl::value_type Item(strItem);
	//				Coll.push_back(Item);
	//			}
	//		}
	//		DDX_Text(nID, SelectedVal, 0, bSave);
	//	}
	//	else
	//	{
	//		ComboBox.ResetContent();

	//		for(TSTLColl::iterator it = Coll.begin(); it != Coll.end(); it++)
	//		{
	//			ComboBox.AddString(*it);
	//		}
	//		ComboBox.SelectString(-1, SelectedVal);
	//	}

	//	if(!bSuccess)
	//	{
	//		pT->OnDataExchangeError(nID, bSave);
	//	}
	//	return bSuccess;
	//}
#endif // _WTLHELPERS_CSTRING


};

#endif // __ATLDDX_H__


//========================================================================================
};	// namespace WTL


//========================================================================================

#endif // __WTLHELPERS_H_FUNKYGUID_92618941__
