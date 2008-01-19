// XRefreshToolbar.h : Declaration of the CXRefreshToolbar

#pragma once
#include "resource.h"

#define ALT_MAP_TB_TOOLBAR                        1
#define IDC_TB_TOOLBAR                            1

//////////////////////////////////////////////////////////////////////////
// CXRefreshToolbar
class ATL_NO_VTABLE CXRefreshToolbar:
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXRefreshToolbar, &CLSID_XRefreshToolbar>,
	public IObjectWithSiteImpl<CXRefreshToolbar>,
	public IDispatchImpl<IXRefreshToolbar, &IID_IXRefreshToolbar, &LIBID_XRefreshLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDeskBand,
	public CWindowImpl<CXRefreshToolbar>
{
public:
	CXRefreshToolbar();
	virtual ~CXRefreshToolbar();

	DECLARE_REGISTRY_RESOURCEID(IDR_XREFRESHTOOLBAR)

	DECLARE_NOT_AGGREGATABLE(CXRefreshToolbar)

	BEGIN_COM_MAP(CXRefreshToolbar)
		COM_INTERFACE_ENTRY(IXRefreshToolbar)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IObjectWithSite)
		COM_INTERFACE_ENTRY(IDeskBand)
	END_COM_MAP()

	BEGIN_MSG_MAP(CXRefreshToolbar)
		NOTIFY_HANDLER(IDC_TB_TOOLBAR, TBN_DROPDOWN, OnToolbarDropdown)
	ALT_MSG_MAP(ALT_MAP_TB_TOOLBAR)
		COMMAND_CODE_HANDLER(0, OnToolbarMenu) // 0 == from menu
		NOTIFY_CODE_HANDLER(TTN_NEEDTEXT, OnToolbarNeedText)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT                                       FinalConstruct() { return S_OK; }
	void                                          FinalRelease() {}

public:
	// IOleWindow (inherited from IDeskBand)
	STDMETHODIMP                                  GetWindow(HWND *phwnd);
	STDMETHODIMP                                  ContextSensitiveHelp(BOOL fEnterMode);

	// IDockingWindow (inherited from IDeskBand)
	STDMETHODIMP                                  ShowDW(BOOL fShow);
	STDMETHODIMP                                  CloseDW(DWORD dwReserved);
	STDMETHODIMP                                  ResizeBorderDW(LPCRECT prcBorder, IUnknown *punkToolbarSite, BOOL fReserved);

	// IDeskBand
	STDMETHODIMP                                  GetBandInfo(DWORD dwBandId, DWORD dwViewMode, DESKBANDINFO *pdbi);

	// IInputObject
	STDMETHODIMP                                  UIActivateIO(BOOL fActivate, LPMSG lpMsg);
	STDMETHODIMP                                  HasFocusIO();
	STDMETHODIMP                                  TranslateAcceleratorIO(LPMSG lpMsg);

	// IObjectWithSite
	STDMETHODIMP                                  SetSite(IUnknown *punkSite);
	STDMETHODIMP                                  GetSite(REFIID riid, void **ppvSite);

	static void                                   HandleError(const CString &errorMessage);

protected:
	CComPtr<IWebBrowser2>                         GetBrowser();
	virtual POINTL                                GetMinSize() const;
	virtual POINTL                                GetMaxSize() const;
	virtual POINTL                                GetActualSize() const;

	virtual void                                  CreateMainWindow();
	virtual CString                               GetTitle();

	virtual LRESULT                               OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT                               OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT                               OnToolbarDropdown(WPARAM wParam, LPNMHDR lParam, BOOL& bHandled);
	virtual bool                                  OnGeneralDropdown(LPNMTOOLBAR data);
	virtual LRESULT                               OnToolbarNeedText(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	virtual LRESULT                               OnToolbarMenu(WORD wCode, WORD wId, HWND hWnd, BOOL& bHandled);

	CComPtr<IWebBrowser2>                         m_Browser;
	TBrowserId                                    m_BrowserId;
	CComPtr<IInputObjectSite>                     m_Site;
	DWORD                                         m_dwBandId;
	DWORD                                         m_dwViewMode;

	CContainedWindow                              m_Toolbar;

	int                                           m_iToolbarHeight;
	CImageList                                    m_kImageList;
	CBitmap                                       m_ToolbarBitmap;
};

OBJECT_ENTRY_AUTO(__uuidof(XRefreshToolbar), CXRefreshToolbar)
