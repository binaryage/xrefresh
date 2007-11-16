// XRefreshHelperbar.h : Declaration of the CXRefreshHelperbar

#pragma once
#include "resource.h"       // main symbols

#include "HelperbarWindow.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CXRefreshHelperbar

class ATL_NO_VTABLE CXRefreshHelperbar :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXRefreshHelperbar, &CLSID_XRefreshHelperbar>,
	public IObjectWithSiteImpl<CXRefreshHelperbar>,
	public IDispatchImpl<IXRefreshHelperbar, &IID_IXRefreshHelperbar, &LIBID_XRefreshLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDeskBand,
	public CWindowImpl<CXRefreshHelperbar>
{
public:
	CXRefreshHelperbar();
	virtual ~CXRefreshHelperbar();

DECLARE_REGISTRY_RESOURCEID(IDR_XREFRESHHELPERBAR)

DECLARE_NOT_AGGREGATABLE(CXRefreshHelperbar)

BEGIN_COM_MAP(CXRefreshHelperbar)
	COM_INTERFACE_ENTRY(IXRefreshHelperbar)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IDeskBand)
	COM_INTERFACE_ENTRY(IDockingWindow)
END_COM_MAP()

BEGIN_MSG_MAP(CXRefreshHelperbar)
END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() { return S_OK; }
	void FinalRelease() {}

public:
	// IOleWindow (inherited from IDeskBand)
	STDMETHODIMP										GetWindow(HWND *phwnd);
	STDMETHODIMP										ContextSensitiveHelp(BOOL fEnterMode);

	// IDockingWindow (inherited from IDeskBand)
	STDMETHODIMP										ShowDW(BOOL fShow);
	STDMETHODIMP										CloseDW(DWORD dwReserved);
	STDMETHODIMP										ResizeBorderDW(LPCRECT prcBorder, IUnknown *punkHelperbarSite, BOOL fReserved);

	// IDeskBand
	STDMETHODIMP										GetBandInfo(DWORD dwBandId, DWORD dwViewMode, DESKBANDINFO *pdbi);

	// IInputObject
	STDMETHODIMP										UIActivateIO(BOOL fActivate, LPMSG lpMsg);
	// A deriving class should override HasFocusIO to properly indicate when its UI has focus
	STDMETHODIMP										HasFocusIO();
	STDMETHODIMP										TranslateAcceleratorIO(LPMSG lpMsg);

	// IObjectWithSite
	STDMETHODIMP										SetSite(IUnknown *punkSite);
	STDMETHODIMP										GetSite(REFIID riid, void **ppvSite);

	void												Log(CString message, int icon);

protected:
	CComPtr<IWebBrowser2>								GetBrowser();

	virtual POINTL 										GetMinSize() const;
	virtual POINTL 										GetMaxSize() const;
	virtual POINTL 										GetActualSize() const;

	virtual void										CreateMainWindow();
	virtual CString										GetTitle();

	CComPtr<IWebBrowser2>								m_Browser;
	TBrowserId											m_BrowserId;
	CComPtr<IInputObjectSite>							m_Site;
	DWORD												m_BandId;
	DWORD												m_ViewMode;

	CHelperbarWindow									m_MainWindow;
};

OBJECT_ENTRY_AUTO(__uuidof(XRefreshHelperbar), CXRefreshHelperbar)
