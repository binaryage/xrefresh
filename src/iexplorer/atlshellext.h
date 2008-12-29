#ifndef __ATLSHELLEXT_H__
#define __ATLSHELLEXT_H__

#pragma once

///////////////////////////////////////////////////////////////////
// Shell Extension wrappers
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001-2005 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#ifndef __ATLSHELLEXTBASE_H__
   #error atlshellext.h requires atlshellextbase.h to be included first
#endif

#include <atlwin.h>
#include <prsht.h>

#include "resource.h"


//////////////////////////////////////////////////////////////////////////////
// CPidl

class CPidl
{
public:
   LPITEMIDLIST m_pidl;

public:
   CPidl() : m_pidl(NULL)
   {
   }

   virtual ~CPidl()
   {
      Delete();
   }

   BOOL IsEmpty() const
   {
      return PidlIsEmpty(m_pidl);
   }

   operator LPITEMIDLIST() const
   {
      return m_pidl;
   }

   LPITEMIDLIST* operator&()
   {
      ATLASSERT(m_pidl==NULL);
      return &m_pidl;
   }

   void Attach(LPITEMIDLIST pSrc)
   {
      Delete();
      m_pidl = pSrc;
   }

   LPITEMIDLIST Detach()
   {
      LPITEMIDLIST pidl = m_pidl;
      m_pidl = NULL;
      return pidl;
   }

   DWORD GetByteSize() const
   { 
      return PidlGetByteSize(m_pidl);
   }

   UINT GetCount() const
   { 
      return PidlGetCount(m_pidl);
   }

   LPCITEMIDLIST GetNextItem() const
   { 
      return PidlGetNextItem(m_pidl);
   }

   LPCITEMIDLIST GetLastItem() const
   { 
      return PidlGetLastItem(m_pidl);
   }

   LPITEMIDLIST CopyFirstItem() const
   { 
      return PidlCopyFirstItem(m_pidl);
   }
  
   LPITEMIDLIST Copy() const
   { 
      return PidlCopy(m_pidl);
   }

   void Delete()
   { 
      PidlDelete(m_pidl);
      m_pidl = NULL;
   }

   void Copy(LPCITEMIDLIST pidlSource) 
   { 
      Delete();
      m_pidl = PidlCopy(pidlSource);
   }  

   void Concatenate(LPCITEMIDLIST pidl2)
   {
      if( (m_pidl == NULL) && (pidl2 == NULL) ) return;
      if( m_pidl == NULL ) {
         m_pidl = PidlCopy(pidl2);
         return;
      }
      if( pidl2 == NULL ) return;
      DWORD cb1, cb2;
      cb1 = GetByteSize() - sizeof(USHORT);
      cb2 = PidlGetByteSize(pidl2);
      LPITEMIDLIST pidlNew = (LPITEMIDLIST) _Module.m_Allocator.Alloc(cb1 + cb2);
      if( pidlNew != NULL ) {
         ::CopyMemory(pidlNew, m_pidl, cb1);
         ::CopyMemory( ((LPBYTE)pidlNew) + cb1, pidl2, cb2 );
      }
      Attach(pidlNew);
   }

   void RemoveLast()
   {
      LPITEMIDLIST pidlLast = const_cast<LPITEMIDLIST>(PidlGetLastItem(m_pidl));
      if( pidlLast != NULL ) pidlLast->mkid.cb = 0;
   }

   inline static BOOL PidlIsEmpty(LPCITEMIDLIST pidl)
   {
      return pidl == NULL || pidl->mkid.cb == 0;
   }

   static LPCITEMIDLIST PidlGetLastItem(LPCITEMIDLIST pidl) 
   { 
      // Get the PIDL of the last item in the list 
      LPCITEMIDLIST pidlLast = NULL;  
      if( pidl != NULL ) {    
         while( pidl->mkid.cb > 0 ) { 
            pidlLast = pidl;
            pidl = PidlGetNextItem(pidl);       
         }      
      }  
      return pidlLast; 
   } 

   inline static LPITEMIDLIST PidlGetNextItem(LPCITEMIDLIST pidl) 
   { 
      return pidl == NULL ? NULL : (LPITEMIDLIST)(LPBYTE)(((LPBYTE)pidl) + pidl->mkid.cb);
   }  

   static UINT PidlGetCount(LPCITEMIDLIST pidlSource)
   {
      UINT cbTotal = 0; 
      if( pidlSource != NULL ) {    
         while( pidlSource->mkid.cb > 0 ) {
            cbTotal++;
            pidlSource = PidlGetNextItem(pidlSource); 
         }
      }
      return cbTotal;
   }

   static DWORD PidlGetByteSize(LPCITEMIDLIST pidlSource)
   { 
      DWORD cbTotal = 0; 
      if( pidlSource != NULL ) {
         while( pidlSource->mkid.cb > 0 ) {
            cbTotal += pidlSource->mkid.cb;
            pidlSource = PidlGetNextItem(pidlSource);
         }
         // Add the size of the NULL terminating ITEMIDLIST    
         cbTotal += sizeof(USHORT);
      }  
      return cbTotal; 
   }

   static void PidlDelete(LPITEMIDLIST pidlSource)
   {
      if( pidlSource == NULL ) return;
      _Module.m_Allocator.Free( pidlSource );
   }

   static LPITEMIDLIST PidlCopy(LPCITEMIDLIST pidlSource) 
   { 
      LPITEMIDLIST pidlTarget = NULL;
      DWORD cbSource = 0;
      if( NULL == pidlSource ) return NULL;
      // Allocate the new pidl
      cbSource = PidlGetByteSize(pidlSource);
      pidlTarget = (LPITEMIDLIST) _Module.m_Allocator.Alloc(cbSource); 
      if( pidlTarget == NULL ) return NULL;  // Copy the source to the target 
      ::CopyMemory(pidlTarget, pidlSource, cbSource);
      return pidlTarget; 
   }

   static LPITEMIDLIST PidlCopyFirstItem(LPCITEMIDLIST pidlSource) 
   { 
      LPITEMIDLIST pidlTarget = NULL; 
      DWORD cbSource = 0;  
      if( NULL == pidlSource ) return NULL;  
      // Allocate the new pidl 
      cbSource = pidlSource->mkid.cb + sizeof(USHORT); 
      pidlTarget = (LPITEMIDLIST) _Module.m_Allocator.Alloc(cbSource); 
      if( pidlTarget == NULL ) return NULL;  // Copy the source to the target 
      ::CopyMemory(pidlTarget, pidlSource, cbSource);
      // Terminate the IDList 
      *(WORD*) (((LPBYTE)pidlTarget)+pidlTarget->mkid.cb) = 0;  
      return pidlTarget;
   }
};


/////////////////////////////////////////////////////////////////////////////
// CPidlList

class CPidlList
{
public:
   CPidlList()
   {
      m_pidls = NULL;
      m_nCount = 0;
   }

   CPidlList(HWND hwndList, DWORD dwListViewMask = LVNI_SELECTED)
   {
      ATLASSERT(::IsWindow(hwndList));

      m_pidls = NULL;
      m_nCount = 0;

      UINT nCount = ( dwListViewMask == LVNI_SELECTED ? ListView_GetSelectedCount(hwndList) : ListView_GetItemCount(hwndList) );
      if( nCount == 0 ) return;

      LPITEMIDLIST* pidls = (LPITEMIDLIST*) _Module.m_Allocator.Alloc(nCount * sizeof(LPITEMIDLIST));   
      if( pidls == NULL ) return;
   
      int nItem = -1;
      UINT i = 0;
      while( (nItem = ListView_GetNextItem(hwndList, nItem, dwListViewMask)) != -1 )
      {
         LVITEM lvi = { 0 };
         lvi.mask = LVIF_PARAM;
         lvi.iItem = nItem;
         ListView_GetItem(hwndList, &lvi);
         pidls[i++] = CPidl::PidlCopy( (LPITEMIDLIST) lvi.lParam );
      }

      m_pidls = pidls;
      m_nCount = i;
   }
   
   virtual ~CPidlList()
   {
      Delete();
   }

   operator LPCITEMIDLIST *() const 
   { 
      return const_cast<LPCITEMIDLIST*>(m_pidls); 
   }

   UINT GetCount() const 
   { 
      return m_nCount; 
   }

   LPITEMIDLIST* Detach()
   {
      LPITEMIDLIST* pidls = m_pidls;
      m_pidls = NULL;
      m_nCount = 0;
      return pidls;
   }
   
   HRESULT Attach(LPITEMIDLIST* pidlSource, UINT nCount)
   {
      Delete();
      m_pidls = pidlSource;
      m_nCount = nCount;
      return S_OK;
   }
   
   HRESULT Copy(LPCITEMIDLIST* pidlSource, UINT nCount)
   {
      ATLASSERT(pidlSource);
      ATLASSERT(nCount>0);
      if( (pidlSource == NULL) || (nCount == 0) ) return E_INVALIDARG;
      Delete();
      m_pidls = (LPITEMIDLIST*) _Module.m_Allocator.Alloc(nCount * sizeof(LPITEMIDLIST));
      if( m_pidls == NULL ) return E_OUTOFMEMORY;
      // Copy the items
      m_nCount = nCount;
      for( UINT i = 0; i < nCount; i++ ) m_pidls[i] = CPidl::PidlCopy(pidlSource[i]);
      return S_OK;
   }
   
   void Delete()
   {
      if( m_pidls == NULL ) return;
      for( UINT i = 0; i < m_nCount; i++ ) _Module.m_Allocator.Free((LPVOID)m_pidls[i]);
      _Module.m_Allocator.Free(m_pidls);
      m_pidls = NULL;
      m_nCount = 0;
   }

   HRESULT Filter(IShellFolder* pFolder, DWORD dwItemMask)
   {
      ATLASSERT(pFolder);
      ATLASSERT(dwItemMask!=0);

      if( m_nCount == 0 ) return S_OK;

      LPITEMIDLIST* pidls = (LPITEMIDLIST*) _Module.m_Allocator.Alloc(m_nCount * sizeof(LPITEMIDLIST));
      if( pidls == NULL ) return E_OUTOFMEMORY;

      UINT nCount = 0;
      for( UINT i = 0; i < m_nCount; i++ ) {
         DWORD dwAttr = dwItemMask;
         LPCITEMIDLIST pidl = m_pidls[i];
         pFolder->GetAttributesOf(1, &pidl, &dwAttr);
         if( (dwAttr & dwItemMask) == dwItemMask ) {
            pidls[nCount] = CPidl::PidlCopy( m_pidls[i] );
            nCount++;
         }
      }

      // We've recreated a new PIDL list.
      // The allocated memory for the list may actually be too large, but
      // it doesn't matter for the PIDL functions.
      return Attach(pidls, nCount);
   }

   LPITEMIDLIST* m_pidls;
   UINT m_nCount;
};


#ifdef __ATLCOM_H__

/////////////////////////////////////////////////////////////////////////////
// CPidlEnum

class ATL_NO_VTABLE CPidlEnum : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public IEnumIDList
{
public:
   CPidlEnum() : m_iCount(0), m_iPos(0), m_pCur(NULL)
   {
   }

DECLARE_NO_REGISTRY()
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPidlEnum)
   COM_INTERFACE_ENTRY_IID(IID_IEnumIDList,IEnumIDList)
END_COM_MAP()

public:
   HRESULT _Init(LPCITEMIDLIST pPidlArray, UINT nCount)
   {      
      m_pidl.Copy(pPidlArray);
      m_iCount = nCount;
      Reset();
      return S_OK;
   }

   STDMETHOD(Next)(ULONG /*celt*/, LPITEMIDLIST* rgelt, ULONG* pceltFetched)
   { 
      ATLTRACE2(atlTraceCOM, 0, _T("IEnumIDList::Next\n"));
      *rgelt = NULL; 
      if( pceltFetched ) *pceltFetched = 0;

      if( m_pidl.IsEmpty() ) return S_FALSE;

      m_iPos++;
      if( m_iPos > m_iCount ) return S_FALSE;

      *rgelt = CPidl::PidlCopyFirstItem(m_pCur);
      m_pCur = CPidl::PidlGetNextItem(m_pCur);
      if( (*rgelt != NULL)  && ((*rgelt)->mkid.cb != 0) ) { 
          if( pceltFetched != NULL ) *pceltFetched = 1; 
          return S_OK; 
      }

      return E_OUTOFMEMORY; 
   }   

   STDMETHOD(Reset)(void)
   { 
      ATLTRACE2(atlTraceCOM, 0, _T("IEnumIDList::Reset\n"));
      m_iPos = 0;
      m_pCur = m_pidl;
      return S_OK; 
   }

   STDMETHOD(Skip)(ULONG /*celt*/)
   {
      ATLTRACENOTIMPL(_T("IEnumIDList::Skip"));
   }

   STDMETHOD(Clone)(IEnumIDList** /*ppEnum*/)
   {
      ATLTRACENOTIMPL(_T("IEnumIDList::Clone"));
   }

public:
   CPidl m_pidl;
   LPCITEMIDLIST m_pCur;
   UINT m_iCount; 
   UINT m_iPos;
};


//////////////////////////////////////////////////////////////////////////////
// IShellFolderImpl

#define GET_SHGDN_FOR(dwFlags)         ((DWORD)dwFlags & (DWORD)0x0000FF00)
#define GET_SHGDN_RELATION(dwFlags)    ((DWORD)dwFlags & (DWORD)0x000000FF)

template< class T, typename DataStruct >
class ATL_NO_VTABLE IShellFolderImpl : 
   public IShellFolder,
   public IPersistFolder2,
   public CPidl
{
public:

   // IPersistFolder

   STDMETHOD(GetClassID)(CLSID* pClassID)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IPersistFolder::GetClassID\n"));
      ATLASSERT(pClassID);
      if( pClassID == NULL ) return E_POINTER;
      *pClassID = T::GetObjectCLSID();
      return S_OK;
   }
  
   STDMETHOD(Initialize)(LPCITEMIDLIST pList)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IPersistFolder::Initialize\n"));
      ATLASSERT(pList);
      if( pList == NULL ) return E_INVALIDARG;
      Copy(pList);
      return S_OK;
   }

   // IPersistFolder2

   STDMETHOD(GetCurFolder)(LPITEMIDLIST* ppidl)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IPersistFolder2::GetCurFolder\n"));
      ATLASSERT(ppidl);
      if( ppidl == NULL ) return E_INVALIDARG;
      *ppidl = CPidl::PidlCopy(m_pidl);
      return S_OK;
   }

   // IShellFolder

   STDMETHOD(EnumObjects)(HWND, DWORD, LPENUMIDLIST* ppRetVal)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellFolder::EnumObjects\n"));
      ATLASSERT(ppRetVal);
      // Return empty collection
      DataStruct v[1];
      v[0].cb = 0;
      CComObject<CPidlEnum>* pEnum;
      HRESULT Hr = CComObject<CPidlEnum>::CreateInstance(&pEnum);
      if( FAILED(Hr) ) return Hr;
      Hr = pEnum->_Init((LPITEMIDLIST) v, 1);
      if( FAILED(Hr) ) return Hr;
      Hr = pEnum->QueryInterface(IID_IEnumIDList, (LPVOID*) ppRetVal);
      if( FAILED(Hr) ) return Hr;
      return S_OK;
   }

   STDMETHOD(BindToObject)(LPCITEMIDLIST /*pidl*/, LPBC, REFIID /*riid*/, LPVOID* ppRetVal)
   {
      // Subfolders not implemented
      *ppRetVal = NULL;
      ATLTRACENOTIMPL(_T("IShellFolder::BindToObject"));
   }

   STDMETHOD(BindToStorage)(LPCITEMIDLIST, LPBC, REFIID, LPVOID* ppRetVal)
   {
      *ppRetVal = NULL;
      ATLTRACENOTIMPL(_T("IShellFolder::BindToStorage"));
   }

   STDMETHOD(CreateViewObject)(HWND /*hwndOwner*/, REFIID riid, LPVOID* ppRetVal )
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellFolder::CreateViewObject\n"));
      ATLASSERT(!::IsBadReadPtr(&riid,sizeof(IID)));
      if( riid != IID_IShellView ) return E_NOINTERFACE;
      T* pT = static_cast<T*>(this);
      return pT->_CreateShellFolderView(ppRetVal);
   }

   STDMETHOD(GetUIObjectOf)(HWND, UINT, LPCITEMIDLIST*, REFIID, LPUINT, LPVOID*)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellFolder::GetUIObjectOf\n"));
      return E_NOINTERFACE;
   }

   STDMETHOD(GetDisplayNameOf)(LPCITEMIDLIST, DWORD, LPSTRRET)
   {
      ATLTRACENOTIMPL(_T("IShellFolder::GetDisplayNameOf"));
   }

   STDMETHOD(GetAttributesOf)(UINT, LPCITEMIDLIST*, LPDWORD rgfInOut)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellFolder::GetAttributesOf\n"));      
      *rgfInOut = 0;
      return S_OK;
   }

   STDMETHOD(ParseDisplayName)(HWND, LPBC, LPOLESTR, LPDWORD, LPITEMIDLIST* ppList, LPDWORD)
   {
      *ppList = NULL;
      ATLTRACENOTIMPL(_T("IShellFolder::ParseDisplayName"));
   }

   STDMETHOD(SetNameOf)(HWND, LPCITEMIDLIST, LPCOLESTR, DWORD, LPITEMIDLIST*)
   {
      ATLTRACENOTIMPL(_T("IShellFolder::SetNameOf"));
   }

   STDMETHOD(CompareIDs)(LPARAM, LPCITEMIDLIST, LPCITEMIDLIST)
   {
      ATLTRACENOTIMPL(_T("IShellFolder::CompareIDs"));
   }

   HRESULT _CreateShellFolderView(LPVOID* /*ppvObj*/)
   {
      ATLTRACENOTIMPL(_T("IShellFolder::_CreateShellFolderView"));
   }
};


//////////////////////////////////////////////////////////////////////////////
// IShellViewImpl

template< class T >
class ATL_NO_VTABLE IShellViewImpl : public IShellView2
{
public:
   enum { IDC_LISTVIEW = 123 };

   BEGIN_MSG_MAP(IShellViewImpl<T>)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
      MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenu)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      NOTIFY_CODE_HANDLER(NM_SETFOCUS, OnNotifySetFocus)
      NOTIFY_CODE_HANDLER(NM_KILLFOCUS, OnNotifyKillFocus)
   END_MSG_MAP()

   IShellViewImpl() :
      m_uState(SVUIA_DEACTIVATE), 
      m_uViewMode(FVM_DETAILS),
      m_hWnd(NULL), 
      m_hwndList(NULL),
      m_hChangeNotify(NULL)
   {
      ::ZeroMemory(&m_ShellFlags,sizeof(m_ShellFlags));
      m_dwListViewStyle = WS_TABSTOP | 
                          WS_VISIBLE |
                          WS_CHILD | 
                          LVS_REPORT | 
                          LVS_NOSORTHEADER |
                          LVS_SHAREIMAGELISTS;
   }

public:
   HWND m_hWnd;
   HWND m_hwndList;
   DWORD m_dwListViewStyle;
#ifdef _DEBUG
   const MSG* m_pCurrentMsg;
#endif
   FOLDERVIEWMODE m_uViewMode;
   SHELLFLAGSTATE m_ShellFlags;
   FOLDERSETTINGS m_FolderSettings;
   CComQIPtr<IShellBrowser> m_spShellBrowser;
   CComQIPtr<ICommDlgBrowser, &IID_ICommDlgBrowser> m_spCommDlg;
   UINT m_uState;
   ULONG m_hChangeNotify;

public:
   // IOleWindow
   
   STDMETHOD(GetWindow)(HWND* phWnd)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IOleWindow::GetWindow\n"));
      ATLASSERT(phWnd);
      ATLASSERT(::IsWindow(m_hWnd));
      *phWnd = m_hWnd;
      return S_OK;
   }

   STDMETHOD(ContextSensitiveHelp)(BOOL)
   {
      ATLTRACENOTIMPL(_T("IOleWindow::ContextSesitiveHelp"));
   }

   // IShellView

   STDMETHOD(TranslateAccelerator)(LPMSG /*lpmsg*/)
   {
      return E_NOTIMPL;
   }

   STDMETHOD(Refresh)(void)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::Refresh\n"));
      ATLASSERT(::IsWindow(m_hwndList));
      // Refill the list
      T* pT = static_cast<T*>(this);
      pT->_FillListView();
      return S_OK;
   }

   STDMETHOD(AddPropertySheetPages)(
      DWORD /*dwReserved*/,
      LPFNADDPROPSHEETPAGE /*lpfn*/, 
      LPARAM /*lParam*/)
   {
      ATLTRACENOTIMPL(_T("IShellView::AddPropertySheetPages"));
   }

   STDMETHOD(SelectItem)(LPCITEMIDLIST /*pidlItem*/, UINT /*uFlags*/)
   {
      ATLTRACENOTIMPL(_T("IShellView::SelectItem"));
   }

   STDMETHOD(GetItemObject)(UINT /*uItem*/, REFIID /*riid*/, LPVOID* ppRetVal)
   {
      ATLASSERT(ppRetVal);
      ppRetVal;
      ATLTRACENOTIMPL(_T("IShellView::GetItemObject"));
   }

   STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
   {
      ATLTRACENOTIMPL(_T("IShellView::EnableModeless"));
   }

   STDMETHOD(GetCurrentInfo)(LPFOLDERSETTINGS lpFS)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::GetCurrentInfo\n"));
      ATLASSERT(lpFS);
      *lpFS = m_FolderSettings;
      return S_OK;
   }

   STDMETHOD(UIActivate)(UINT uState)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::UIActivate (%d)\n"), uState);
      ATLASSERT(m_spShellBrowser);
      // Only do this if we are active
      if( m_uState == uState ) return S_OK;      
      // _ViewActivate() handles merging of menus etc
      T* pT = static_cast<T*>(this);
      if( SVUIA_ACTIVATE_FOCUS == uState ) ::SetFocus(m_hwndList);
      pT->_ViewActivate(uState);
      if( uState != SVUIA_DEACTIVATE) {
         // Update the status bar: set 'parts' and change text
         LRESULT lResult;
         int nPartArray[1] = { -1 };
         m_spShellBrowser->SendControlMsg(FCW_STATUS, SB_SETPARTS, 1, (LPARAM)nPartArray, &lResult);
         // Set the statusbar text to the default description.
         // The string resource IDS_DESCRIPTION must be defined!
         TCHAR szName[128] = { 0 };
         ::LoadString(_Module.GetResourceInstance(), IDS_DESCRIPTION, szName, (sizeof(szName) / sizeof(TCHAR)) - 1);
         m_spShellBrowser->SendControlMsg(FCW_STATUS, SB_SETTEXT, 0, (LPARAM)szName, &lResult);
      }
      return S_OK;
   }

   STDMETHOD(CreateViewWindow)(
      IShellView* /*lpPrevView*/,
      LPCFOLDERSETTINGS pFS, 
      IShellBrowser* pSB,
      RECT* prcView, 
      HWND* phWnd)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::CreateViewWindow\n"));
      ATLASSERT(prcView);
      ATLASSERT(pSB);
      ATLASSERT(pFS);
      ATLASSERT(phWnd);

      // Register the ClassName.
      // The ClassName comes from the string resource IDS_CLASSNAME!
      TCHAR szClassName[64] = { 0 };
      ::LoadString(_Module.GetResourceInstance(), IDS_CLASSNAME, szClassName, (sizeof(szClassName) / sizeof(TCHAR)) - 1);
      WNDCLASS wc = { 0 };
      *phWnd = NULL;
      // If our window class has not been registered, then do so
      if( !::GetClassInfo(_Module.GetModuleInstance(), szClassName, &wc) ) {
         wc.style          = 0;
         wc.lpfnWndProc    = (WNDPROC)WndProc;
         wc.cbClsExtra     = 0;
         wc.cbWndExtra     = 0;
         wc.hInstance      = _Module.GetModuleInstance();
         wc.hIcon          = NULL;
         wc.hCursor        = ::LoadCursor(NULL, IDC_ARROW);
         wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
         wc.lpszMenuName   = NULL;
         wc.lpszClassName  = szClassName; 
         if( !::RegisterClass(&wc) ) return HRESULT_FROM_WIN32(::GetLastError());
      }

      // Set up the member variables
      m_spShellBrowser = pSB;
      m_spCommDlg = pSB;
      m_FolderSettings = *pFS;
      m_ShellFlags.fWin95Classic = TRUE;
      m_ShellFlags.fShowAttribCol = TRUE;
      m_ShellFlags.fShowAllObjects = TRUE;
      m_uViewMode = (FOLDERVIEWMODE) m_FolderSettings.ViewMode;

      // Get our parent window
      HWND hwndShell = NULL;
      m_spShellBrowser->GetWindow(&hwndShell);
      // Create host window
      T* pT = static_cast<T*>(this);
      *phWnd = ::CreateWindowEx(WS_EX_CONTROLPARENT,
                                szClassName,
                                NULL,
                                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP,
                                prcView->left,
                                prcView->top,
                                prcView->right - prcView->left,
                                prcView->bottom - prcView->top,
                                hwndShell,
                                NULL,
                                _Module.GetModuleInstance(),
                                (LPVOID) pT);
      if( *phWnd == NULL ) return HRESULT_FROM_WIN32(::GetLastError());

      pT->_MergeToolbar(SVUIA_ACTIVATE_FOCUS);
      return S_OK;
   }

   STDMETHOD(DestroyViewWindow)(void)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::DestroyViewWindow\n"));
      ATLASSERT(m_spShellBrowser);

      // Make absolutely sure all our UI is cleaned up.
      UIActivate(SVUIA_DEACTIVATE);

      ::DestroyWindow(m_hWnd);

      // Release the shell browser objects
      m_spShellBrowser.Release();
      m_spCommDlg.Release();

      return S_OK;
   }

   STDMETHOD(SaveViewState)(void)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::SaveViewState\n"));
      return S_OK;
   }

   // IShellView2

   STDMETHOD(CreateViewWindow2)(LPSV2CVW2_PARAMS lpParams) 
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::CreateViewWindow2\n"));
      // pvid takes precedence over pfs->ViewMode 
      _ViewModeFromSVID(lpParams->pvid, (FOLDERVIEWMODE*) &lpParams->pfs->ViewMode);
      // Create the view...
      return CreateViewWindow(lpParams->psvPrev, lpParams->pfs, lpParams->psbOwner, lpParams->prcView, &lpParams->hwndView);
   }

   STDMETHOD(GetView)(SHELLVIEWID* pvid, ULONG uView) 
   {
      if( pvid == NULL ) return E_INVALIDARG;
      switch( uView ) { 
      case 0: return _SVIDFromViewMode(FVM_ICON, pvid); 
      case 1: return _SVIDFromViewMode(FVM_SMALLICON, pvid); 
      case 2: return _SVIDFromViewMode(FVM_LIST, pvid); 
      case 3: return _SVIDFromViewMode(FVM_DETAILS, pvid); 
      case SV2GV_CURRENTVIEW: return _SVIDFromViewMode(FOLDERVIEWMODE(m_uViewMode), pvid); 
      case SV2GV_DEFAULTVIEW: return _SVIDFromViewMode(FVM_ICON, pvid); 
      } 
      return E_FAIL; 
   }

   STDMETHOD(HandleRename)(LPCITEMIDLIST /*pidlNew*/) 
   {
      ATLTRACENOTIMPL(_T("IShellView2::HandleRename"));
   }

   STDMETHOD(SelectAndPositionItem)(LPCITEMIDLIST /*pidlItem*/, UINT /*uFlags*/, POINT* /*point*/) 
   {
      ATLTRACENOTIMPL(_T("IShellView2::SelectAndPositionItem"));
   }

   // View handlers

   LRESULT _ViewActivate(UINT uState)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::_ViewActivate %d\n"), uState);
      // Don't do anything if the state isn't really changing
      if( m_uState == uState ) return S_OK;
      T* pT = static_cast<T*>(this);
      pT->_ViewDeactivate();
      // Only do this if we are active
      if( uState != SVUIA_DEACTIVATE ) {
         pT->_MergeMenus(uState);
         pT->_UpdateToolbar();
      }
      m_uState = uState;
      return 0;
   }

   LRESULT _ViewDeactivate(void)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellView::_ViewDeactivate\n"));
      T* pT = static_cast<T*>(this);
      m_uState = SVUIA_DEACTIVATE;
      pT->_MergeMenus(m_uState);
      pT->_MergeToolbar(m_uState);
      return 0;
   }

   // Since ::SHGetSettings() is not implemented in all versions of the shell, get the 
   // function address manually at run time. This allows the extension to run on all 
   // platforms.
   void _GetShellSettings(SHELLFLAGSTATE &sfs, DWORD dwMask)
   {
      typedef void (WINAPI *PFNSHGETSETTINGSPROC)(LPSHELLFLAGSTATE lpsfs, DWORD dwMask);
      HMODULE hInstShell32 = ::LoadLibrary(_T("shell32.dll"));
      if( hInstShell32 == NULL ) return;
      static PFNSHGETSETTINGSPROC ShGetSettings = NULL;
      if( ShGetSettings == NULL )
         ShGetSettings = (PFNSHGETSETTINGSPROC) ::GetProcAddress(hInstShell32, "SHGetSettings");
      if( ShGetSettings == NULL ) return;
      ShGetSettings(&sfs, dwMask);
      ::FreeLibrary(hInstShell32);
   }

   // Register for view changes.
   // This API was previously undocumented and thus only exported by ordinal. Since then,
   // Microsoft was forced to document it (reluctantly) due to a lawsuit.
   void _ChangeNotifyRegister(LPCITEMIDLIST pidl, UINT uMsg, LONG fEvents = SHCNE_UPDATEDIR)
   {
#define SHCNF_ACCEPT_INTERRUPTS     0x0001 
#define SHCNF_ACCEPT_NON_INTERRUPTS 0x0002 
      typedef ULONG (WINAPI *PFNSHCHANGENOTIFYREGISTER)(HWND hwnd, int fSources, LONG fEvents, UINT wMsg, int cEntries, SHChangeNotifyEntry *pshcne);
      HMODULE hInstShell32 = ::LoadLibrary(_T("shell32.dll")); 
      if( hInstShell32 == NULL ) return;
      static PFNSHCHANGENOTIFYREGISTER ShChangeNotifyRegister = NULL; 
      if( ShChangeNotifyRegister == NULL ) 
         ShChangeNotifyRegister = (PFNSHCHANGENOTIFYREGISTER) ::GetProcAddress(hInstShell32, "SHChangeNotifyRegister");
      if( ShChangeNotifyRegister == NULL ) 
         ShChangeNotifyRegister = (PFNSHCHANGENOTIFYREGISTER) ::GetProcAddress(hInstShell32, MAKEINTRESOURCE(2)); 
      if( ShChangeNotifyRegister == NULL ) return;
      SHChangeNotifyEntry Nr;
      Nr.pidl = pidl;
      Nr.fRecursive = TRUE;
      m_hChangeNotify = ShChangeNotifyRegister(m_hWnd, 
         SHCNF_ACCEPT_INTERRUPTS | SHCNF_ACCEPT_NON_INTERRUPTS,
         fEvents,
         uMsg,
         1, &Nr);
   }
   void _ChangeNotifyDeregister()
   {
      if( m_hChangeNotify == NULL ) return;
      typedef BOOL (WINAPI *PFNSHCHANGENOTIFYDEREGISTER)(ULONG hID);
      HMODULE hInstShell32 = ::GetModuleHandle(_T("shell32.dll")); 
      if( hInstShell32 == NULL ) return;
      static PFNSHCHANGENOTIFYDEREGISTER ShChangeNotifyDeregister = NULL; 
      if( ShChangeNotifyDeregister == NULL ) 
         ShChangeNotifyDeregister = (PFNSHCHANGENOTIFYDEREGISTER) ::GetProcAddress(hInstShell32, "SHChangeNotifyDeregister");
      if( ShChangeNotifyDeregister == NULL ) 
         ShChangeNotifyDeregister = (PFNSHCHANGENOTIFYDEREGISTER) ::GetProcAddress(hInstShell32, MAKEINTRESOURCE(4)); 
      if( ShChangeNotifyDeregister == NULL ) return;
      ShChangeNotifyDeregister(m_hChangeNotify);
      m_hChangeNotify = NULL;
      ::FreeLibrary(hInstShell32);  // Matches the LoadLibrary() in _ChangeNotifyRegister()
   }

   // A helper function which will take care of some of
   // the fancy new Win98 settings...
   void _UpdateShellSettings(void)
   {
      // Get the m_ShellFlags state
      _GetShellSettings(m_ShellFlags, 
         SSF_DESKTOPHTML | 
         SSF_NOCONFIRMRECYCLE | 
         SSF_SHOWALLOBJECTS | 
         SSF_SHOWATTRIBCOL | 
         SSF_DOUBLECLICKINWEBVIEW | 
         SSF_SHOWCOMPCOLOR |
         SSF_WIN95CLASSIC);

      // Update the ListView control accordingly
#ifndef LVS_EX_DOUBLEBUFFER
      const DWORD LVS_EX_DOUBLEBUFFER = 0x00010000;
#endif
      DWORD dwExStyles = LVS_EX_HEADERDRAGDROP |      // Allow but no auto-persist
                         LVS_EX_DOUBLEBUFFER;         // Causes blue marquee on WinXP
      if( !m_ShellFlags.fWin95Classic 
          && !m_ShellFlags.fDoubleClickInWebView ) 
      {
         dwExStyles |= LVS_EX_ONECLICKACTIVATE | 
                       LVS_EX_TRACKSELECT | 
                       LVS_EX_UNDERLINEHOT;
      }
      ListView_SetExtendedListViewStyle(m_hwndList, dwExStyles);
   }

   LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
   {
#ifdef STRICT
      return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
#else
      return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
#endif
   }

   static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
   {
      T* pT = (T*) ::GetWindowLong(hWnd, GWL_USERDATA);
      if( uMessage == WM_NCCREATE ) {
         LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam;
         pT = (T*) lpcs->lpCreateParams;
         ::SetWindowLong(hWnd, GWL_USERDATA, (LONG) pT);
         // Set the window handle
         pT->m_hWnd = hWnd;
         return 1;
      }
      ATLASSERT(pT);
#ifdef _DEBUG
      MSG msg = { pT->m_hWnd, uMessage, wParam, lParam, 0, { 0, 0 } };
      const MSG* pOldMsg = pT->m_pCurrentMsg;
      pT->m_pCurrentMsg = &msg;
#endif
      // pass to the message map to process
      LRESULT lRes = 0;
      BOOL bRet = pT->ProcessWindowMessage(pT->m_hWnd, uMessage, wParam, lParam, lRes, 0);
      // Restore saved value for the current message
#ifdef _DEBUG
      ATLASSERT(pT->m_pCurrentMsg==&msg);
      pT->m_pCurrentMsg = pOldMsg;
#endif
      if( !bRet ) lRes = pT->DefWindowProc(uMessage, wParam, lParam);
      return lRes;
   }

   // Message handlers

   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      ATLTRACE2(atlTraceWindowing, 0, _T("IShellView::OnSetFocus\n"));
      ::SetFocus(m_hwndList);
      return 0;
   }

   LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      ATLTRACE2(atlTraceWindowing, 0, _T("IShellView::OnKillFocus\n"));
      return 0;
   }

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->_UpdateShellSettings();
      return 0;
   }

   LRESULT OnNotifySetFocus(UINT /*CtlID*/, LPNMHDR /*lpnmh*/, BOOL &/*bHandled*/)
   {
      ATLTRACE2(atlTraceWindowing, 0, _T("IShellView::OnNotifySetFocus\n"));
      ATLASSERT(m_spShellBrowser);
      // Tell the browser one of our windows has received the focus. This should always 
      // be done before merging menus (_ViewActivate() merges the menus) if one of our 
      // windows has the focus.
      T* pT = static_cast<T*>(this);
      m_spShellBrowser->OnViewWindowActive(pT);
      pT->_ViewActivate(SVUIA_ACTIVATE_FOCUS);
      if( m_spCommDlg ) m_spCommDlg->OnStateChange(this, CDBOSC_SETFOCUS);
      return 0;
   }

   LRESULT OnNotifyKillFocus(UINT /*CtlID*/, LPNMHDR /*lpnmh*/, BOOL &/*bHandled*/)
   {
      ATLTRACE2(atlTraceWindowing, 0, _T("IShellView::OnNotifyKillFocus\n"));
      T* pT = static_cast<T*>(this);
      pT->_ViewActivate(SVUIA_ACTIVATE_NOFOCUS);
      if( m_spCommDlg ) m_spCommDlg->OnStateChange(this, CDBOSC_KILLFOCUS);
      return 0;
   }

   LRESULT OnInitMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->_UpdateMenu((HMENU)wParam, NULL);
      return 0;
   }

   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      // Resize the ListView to fit our window
      if( m_hwndList ) ::MoveWindow(m_hwndList, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
      return 0;
   }

   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // avoid flicker
   }

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      // Create the ListView
      T* pT = static_cast<T*>(this);
      if( pT->_CreateListView() )
         if( pT->_InitListView() )
            pT->_FillListView();
      return 0;
   }

   // Operations

   typedef enum {
      TBI_STD = 0,
      TBI_VIEW,
      TBI_LOCAL
   } TOOLBARITEM;

   typedef struct
   {
      TOOLBARITEM nType;
      TBBUTTON tb;
   } NS_TOOLBUTTONINFO, *PNS_TOOLBUTTONINFO;

   BOOL _AppendToolbarItems(
      PNS_TOOLBUTTONINFO pButtons, 
      int nCount, 
      LPARAM lOffsetFile, 
      LPARAM lOffsetView, 
      LPARAM lOffsetCustom)
   {
      ATLASSERT(nCount>0);
      ATLASSERT(!::IsBadReadPtr(pButtons, nCount*sizeof(NS_TOOLBUTTONINFO)));
      ATLASSERT(m_spShellBrowser);
      LPTBBUTTON ptbb = (LPTBBUTTON) ::GlobalAlloc(GPTR, sizeof(TBBUTTON) * nCount);
      if( ptbb == NULL ) return FALSE;
      for( int j = 0; j < nCount; j++ ) {
         switch( pButtons[j].nType ) {
         case TBI_STD:
            (ptbb + j)->iBitmap = lOffsetFile + pButtons[j].tb.iBitmap;
            break;
         case TBI_VIEW:
            (ptbb + j)->iBitmap = lOffsetView + pButtons[j].tb.iBitmap;
            break;
         case TBI_LOCAL:
            (ptbb + j)->iBitmap = lOffsetCustom + pButtons[j].tb.iBitmap;
            break;
         }
         (ptbb + j)->idCommand = pButtons[j].tb.idCommand;
         (ptbb + j)->fsState = pButtons[j].tb.fsState;
         (ptbb + j)->fsStyle = pButtons[j].tb.fsStyle;
         (ptbb + j)->dwData = pButtons[j].tb.dwData;
         (ptbb + j)->iString = pButtons[j].tb.iString;
      }
      m_spShellBrowser->SetToolbarItems(ptbb, nCount, FCT_MERGE);
      ::GlobalFree((HGLOBAL)ptbb);
      return TRUE;
   }

   UINT _GetMenuPosFromID(HMENU hMenu, UINT ID) const
   {
      UINT nCount = ::GetMenuItemCount(hMenu);
      for( UINT i = 0; i < nCount; i++ ) {
         if( ::GetMenuItemID(hMenu, i) == ID ) return i;
      }
      return (UINT) -1;
   }

   BOOL _AppendMenu(HMENU hMenu, HMENU hMenuSource, UINT nPosition)
   {
      ATLASSERT(::IsMenu(hMenu));
      ATLASSERT(::IsMenu(hMenuSource));
      // Get the HMENU of the popup
      if( hMenu == NULL ) return FALSE;
      if( hMenuSource == NULL ) return FALSE;
      // Make sure that we start with only one separator menu-item
      int iStartPos = 0;
      if( ::GetMenuState(hMenuSource, 0, MF_BYPOSITION) & MF_SEPARATOR ) {
         if( (nPosition == 0) || 
             (::GetMenuState(hMenu, nPosition - 1, MF_BYPOSITION) & MF_SEPARATOR) ) {
            iStartPos++;
         }
      }
      // Go...
      int nMenuItems = ::GetMenuItemCount(hMenuSource);
      for( int i = iStartPos; i < nMenuItems; i++ ) {
         // Get state information
         UINT state = ::GetMenuState(hMenuSource, i, MF_BYPOSITION);
         TCHAR szItemText[256] = { 0 };
         int nLen = ::GetMenuString(hMenuSource, i, szItemText, (sizeof(szItemText) / sizeof(TCHAR)) - 1, MF_BYPOSITION);
         // Is this a separator?
         if( state & MF_SEPARATOR ) {
            ::InsertMenu(hMenu, nPosition++, state | MF_STRING | MF_BYPOSITION, 0, _T(""));
         }
         else if( state & MF_POPUP ) {
            // Strip the HIBYTE because it contains a count of items
            state = LOBYTE(state) | MF_POPUP;
            // Then create the new submenu by using recursive call
            HMENU hSubMenu = ::CreateMenu();
            _AppendMenu(hSubMenu, ::GetSubMenu(hMenuSource, i), 0);
            ATLASSERT(::GetMenuItemCount(hSubMenu)>0);
            // Non-empty popup -- add it to the shared menu bar
            ::InsertMenu(hMenu, nPosition++, state | MF_BYPOSITION, (UINT) hSubMenu, szItemText);
         }
         else if( nLen > 0 ) {
            // Only non-empty items should be added
            ATLASSERT(szItemText[0] != _T('\0'));
            ATLASSERT(::GetMenuItemID(hMenuSource, i)>FCIDM_SHVIEWFIRST && ::GetMenuItemID(hMenuSource, i)<FCIDM_SHVIEWLAST);
            // Here the state does not contain a count in the HIBYTE
            ::InsertMenu(hMenu, nPosition++, state | MF_BYPOSITION, ::GetMenuItemID(hMenuSource, i), szItemText);
         }
      }
      return TRUE;
   }

   HRESULT _ViewModeFromSVID(const SHELLVIEWID* pvid, FOLDERVIEWMODE* pViewMode) const
   {
      ATLASSERT(pvid);
      ATLASSERT(pViewMode);
      HRESULT Hr = S_OK; 
      if( *pvid == VID_LargeIcons )      *pViewMode = FVM_ICON; 
      else if( *pvid == VID_SmallIcons ) *pViewMode = FVM_SMALLICON; 
      else if( *pvid == VID_Thumbnails ) *pViewMode = FVM_THUMBNAIL; 
      else if( *pvid == VID_ThumbStrip ) *pViewMode = FVM_THUMBSTRIP; 
      else if( *pvid == VID_List )       *pViewMode = FVM_LIST; 
      else if( *pvid == VID_Tile )       *pViewMode = FVM_TILE; 
      else if( *pvid == VID_Details )    *pViewMode = FVM_DETAILS; 
      else { 
         *pViewMode = FVM_ICON; 
         Hr = E_FAIL; 
      } 
      return Hr; 
   }

   HRESULT _SVIDFromViewMode(FOLDERVIEWMODE mode, SHELLVIEWID* svid) const
   { 
      ATLASSERT(svid);
      switch( mode ) { 
      case FVM_SMALLICON:  *svid = VID_SmallIcons; break;
      case FVM_LIST:       *svid = VID_List;       break;
      case FVM_DETAILS:    *svid = VID_Details;    break;
      case FVM_THUMBNAIL:  *svid = VID_Thumbnails; break;
      case FVM_TILE:       *svid = VID_Tile;       break;
      case FVM_THUMBSTRIP: *svid = VID_ThumbStrip; break;
      case FVM_ICON:       *svid = VID_LargeIcons; break;
      default:             *svid = VID_LargeIcons; break;
      }
      return S_OK;
   } 

   BOOL _IsExplorerMode() const
   {
      ATLASSERT(m_spShellBrowser);
      // MSDN actually documents that we can determine if we're in explorer mode
      // by asking for the tree control.
      HWND hwndTree = NULL;
      return( SUCCEEDED(m_spShellBrowser->GetControlWindow(FCW_TREE, &hwndTree)) && hwndTree );
   }

   // Overridables

   BOOL _CreateListView(void)
   {
      // Initialize and create the actual List View control
      ATLASSERT((m_dwListViewStyle & (WS_VISIBLE|WS_CHILD))==(WS_VISIBLE|WS_CHILD));
      m_dwListViewStyle &= ~LVS_TYPEMASK;
      switch( m_uViewMode ) {
      case FVM_ICON:
         m_dwListViewStyle |= LVS_ICON;
         break;
      case FVM_SMALLICON:
         m_dwListViewStyle |= LVS_SMALLICON;
         break;
      case FVM_LIST:
         m_dwListViewStyle |= LVS_LIST;
         break;
      case FVM_DETAILS:
         m_dwListViewStyle |= LVS_REPORT;
         break;
      default:
         m_dwListViewStyle |= LVS_ICON;
         break;
      }
      if( (FWF_ALIGNLEFT & m_FolderSettings.fFlags) != 0 ) m_dwListViewStyle |= LVS_ALIGNLEFT;
      if( (FWF_AUTOARRANGE & m_FolderSettings.fFlags) != 0 ) m_dwListViewStyle |= LVS_AUTOARRANGE;
#if (_WIN32_IE >= 0x0500)
      if( (FWF_SHOWSELALWAYS & m_FolderSettings.fFlags) != 0 ) m_dwListViewStyle |= LVS_SHOWSELALWAYS;
#endif

      // Go on,.. create the ListView control
      m_hwndList = ::CreateWindowEx( (FWF_NOCLIENTEDGE & m_FolderSettings.fFlags) != 0 ? 0 : WS_EX_CLIENTEDGE,
                                     WC_LISTVIEW,
                                     NULL,
                                     m_dwListViewStyle,
                                     0,0,0,0,
                                     m_hWnd,
                                     (HMENU) IDC_LISTVIEW,
                                     _Module.GetModuleInstance(),
                                     NULL);
      if( m_hwndList == NULL ) return FALSE;
      T* pT = static_cast<T*>(this);
      pT->_UpdateShellSettings();
      return TRUE;
   }
   BOOL _InitListView(void) { return TRUE; };
   BOOL _FillListView(void) { return TRUE; };
   BOOL _MergeToolbar(UINT /*uState*/) { return TRUE; };
   BOOL _MergeMenus(UINT /*uState*/) { return TRUE; };
   BOOL _UpdateToolbar() { return TRUE; };
   BOOL _UpdateMenu(HMENU /*hMenu*/, LPCITEMIDLIST /*pidl*/) { return TRUE; };
};


//////////////////////////////////////////////////////////////////////////////
// IShellFolderViewCBImpl

template< class T >
class ATL_NO_VTABLE IShellFolderViewCBImpl : public IShellFolderViewCB
{
public:
   STDMETHOD(MessageSFVCB)(UINT uMsg, WPARAM wParam, LPARAM lParam)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellFolderViewCB::MessageSFVCB (%ld,%ld,%ld)\n"), uMsg, wParam, lParam);
      LONG lResult = 0;
      T* pT = static_cast<T*>(this);
      BOOL bResult = pT->ProcessWindowMessage(NULL, uMsg, wParam, lParam, lResult, 0);
      return bResult ? lResult : E_NOTIMPL;
   }
};


//////////////////////////////////////////////////////////////////////////////
// CShellPropertyPage

template< class T >
class ATL_NO_VTABLE CShellPropertyPage :
   public IShellPropSheetExt,
   public IShellExtInit,
   public CWindow
{
public:
   TCHAR m_szFileName[MAX_PATH];
   const MSG* m_pCurrentMsg;
   bool m_bSeenAddRef;

   CShellPropertyPage()
   {
      m_szFileName[0] = _T('\0');
      m_bSeenAddRef = false;
   }

   // IShellPropSheetExt

   STDMETHOD(AddPages)(LPFNADDPROPSHEETPAGE pfnAddPage, LPARAM lParam)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellPropSheetExt::AddPages\n"));

      T* pT = static_cast<T*>(this);

      TCHAR szCaption[40] = { 0 };
      ::LoadString(_Module.GetResourceInstance(), T::IDS_TABCAPTION, szCaption, (sizeof(szCaption) / sizeof(TCHAR)) - 1);

      PROPSHEETPAGE psp = { 0 };
      psp.dwSize        = sizeof(psp);
      psp.dwFlags       = PSP_USETITLE | PSP_USECALLBACK;
      psp.hInstance     = _Module.GetResourceInstance();
      psp.pszTemplate   = MAKEINTRESOURCE(T::IDD);
      psp.hIcon         = 0;
      psp.pszTitle      = szCaption;
      psp.pfnDlgProc    = (DLGPROC) T::PageDlgProc;
      psp.pcRefParent   = NULL;
      psp.pfnCallback   = T::PropSheetPageProc;
      psp.lParam        = (LPARAM) pT;
      HPROPSHEETPAGE hPage = ::CreatePropertySheetPage(&psp);            
      if( hPage == NULL ) return E_OUTOFMEMORY;

      if( pfnAddPage(hPage, lParam) == FALSE ) {
         ::DestroyPropertySheetPage(hPage);
         return E_FAIL;
      }

      return MAKE_HRESULT(SEVERITY_SUCCESS, 0, T::ID_TAB_INDEX); // COMCTRL Ver 4.71 allows us to set the initial page index
   }

   STDMETHOD(ReplacePage)(UINT /*uPageID*/, LPFNADDPROPSHEETPAGE /*lpfnReplaceWith*/, LPARAM /*lParam*/) 
   {
      // The Shell doesn't call this for file class Property Sheets
      ATLTRACENOTIMPL(_T("IShellPropSheetExt::ReplacePage"));
   }

   // IShellExtInit

   STDMETHOD(Initialize)(LPCITEMIDLIST /*pidlFolder*/, IDataObject* pDataObject, HKEY /*hkeyProgID*/) 
   {
      ATLTRACE2(atlTraceCOM, 0, _T("IShellExtInit::Initialize\n"));
      ATLASSERT(pDataObject);
      STGMEDIUM medium = { 0 };
      FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
      if( SUCCEEDED( pDataObject->GetData(&fe, &medium) ) ) {
         // Get the file name from the CF_HDROP.
         HDROP hDrop = (HDROP) GlobalLock(medium.hGlobal);
         UINT uCount = ::DragQueryFile(hDrop, (UINT) -1, NULL, 0);
         if( uCount > 0 ) ::DragQueryFile(hDrop, 0, m_szFileName, MAX_PATH);
         GlobalUnlock(medium.hGlobal);
         ::ReleaseStgMedium(&medium);
      }
      return S_OK;
   }

   // Callbacks

   static UINT CALLBACK PropSheetPageProc(HWND /*hwnd*/, UINT uMsg, LPPROPSHEETPAGE ppsp)
   {
      ATLTRACE2(atlTraceCOM, 0, _T("CShellPropertyPage::PropSheetPageProc %ld\n"), uMsg);
      T* pT = (T*) ppsp->lParam;
      switch( uMsg ) {
      case PSPCB_CREATE:
         if( !pT->m_bSeenAddRef ) pT->AddRef();
         return 1; // Allow dialog creation
#if (_WIN32_IE >= 0x0500)
      case PSPCB_ADDREF:
         ATLASSERT(pT);
         pT->AddRef();
         pT->m_bSeenAddRef = true;
         break;
#endif
      case PSPCB_RELEASE:
         ATLASSERT(pT);
         pT->Release();
         break;
      }
      return 0;
   }

   void SetModified(BOOL bChanged = TRUE)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(GetParent()!=NULL);
      ::SendMessage(GetParent(), bChanged ? PSM_CHANGED : PSM_UNCHANGED, (WPARAM) m_hWnd, 0L);
   }

   static int CALLBACK PageDlgProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
   {    
      T* pT = (T*) ::GetWindowLong(hWnd, GWL_USERDATA);
      LRESULT lRes = 0;
      if( uMessage == WM_INITDIALOG ) {
         pT = (T*) ((LPPROPSHEETPAGE)lParam)->lParam;
         ::SetWindowLong(hWnd, GWL_USERDATA, (LONG) pT);
         // Set the window handle
         pT->m_hWnd = hWnd;
         lRes = 1;
      }
      if( pT == NULL ) {
         // The first message might be WM_SETFONT not WM_INITDIALOG as expected...
         return FALSE;
      }
      MSG msg = { pT->m_hWnd, uMessage, wParam, lParam, 0, { 0, 0 } };
      const MSG* pOldMsg = pT->m_pCurrentMsg;
      pT->m_pCurrentMsg = &msg;
      // Pass to the message map to process
      BOOL bRet = pT->ProcessWindowMessage(pT->m_hWnd, uMessage, wParam, lParam, lRes, 0);
      // Restore saved value for the current message
      ATLASSERT(pT->m_pCurrentMsg == &msg);
      pT->m_pCurrentMsg = pOldMsg;
      // Set result if message was handled
      if( bRet ) {
         switch( uMessage ) {
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
         }
         ::SetWindowLong(pT->m_hWnd, DWL_MSGRESULT, lRes);
         return TRUE;
      }
      if( uMessage == WM_NCDESTROY ) {
         // Clear out window handle
         ::SetWindowLong(hWnd, GWL_USERDATA, 0L);
         pT->m_hWnd = NULL;
      }
      return FALSE;
   }
};


//////////////////////////////////////////////////////////////////////////////
// CExtractFileIcon

template< class T >
class ATL_NO_VTABLE CExtractFileIcon : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public IExtractIcon
{
public:

BEGIN_COM_MAP(CExtractFileIcon)
   COM_INTERFACE_ENTRY_IID(IID_IExtractIcon,IExtractIcon)
END_COM_MAP()

public:
   CPidl m_pidl;

public:
   void Init(LPCITEMIDLIST pidl)
   {
      m_pidl.Copy(pidl);
   }

   // IExtractIcon

   STDMETHOD(GetIconLocation)(UINT uFlags, 
                              LPTSTR /*szIconFile*/, 
                              UINT /*cchMax*/, 
                              LPINT piIndex, 
                              LPUINT puFlags)
   {
      ATLASSERT(piIndex);
      ATLASSERT(puFlags);
      if( m_pidl.IsEmpty() ) return E_FAIL;
      LPCITEMIDLIST pidlLast = CPidl::PidlGetLastItem(m_pidl);
      T* pData = (T*) pidlLast;
      if( pData != NULL ) {
         switch( pData->type ) {
         case 0:
            *piIndex = 0;
            *puFlags = GIL_NOTFILENAME;
            break;
         case 1:
            *piIndex = 1;
            *puFlags = GIL_NOTFILENAME;
            break;
         default:
            *piIndex = 0;
            *puFlags = GIL_SIMULATEDOC;
            break;
         }
      }
      return S_OK;
   }

   STDMETHOD(Extract)(LPCTSTR pszFile, 
                      UINT nIconIndex, 
                      HICON* phiconLarge, 
                      HICON* phiconSmall, 
                      UINT nIconSize)
   {
      ATLASSERT(phiconLarge);
      ATLASSERT(phiconSmall);
      if( m_pidl.IsEmpty() ) return E_FAIL;
      LPCITEMIDLIST pidlLast = CPidl::PidlGetLastItem(m_pidl);
      T* pData = (T*) pidlLast;
      if( pData != NULL ) {
         switch( pData->type ) {
         case 0:
            {
               *phiconLarge = ImageList_GetIcon(_Module.m_ImageLists.m_hImageListLarge, 0, ILD_TRANSPARENT);
               *phiconSmall = ImageList_GetIcon(_Module.m_ImageLists.m_hImageListSmall, 0, ILD_TRANSPARENT);
            }
            break;
         case 1:
            {
               *phiconLarge = ImageList_GetIcon(_Module.m_ImageLists.m_hImageListLarge, 1, ILD_TRANSPARENT);
               *phiconSmall = ImageList_GetIcon(_Module.m_ImageLists.m_hImageListSmall, 1, ILD_TRANSPARENT);
            }
            break;
         case 2:
            {
               TCHAR szPath[MAX_PATH];
               TCHAR szExt[MAX_PATH];
               ::lstrcpy( szPath, pData->ffd.cFileName );
               LPTSTR psz = _tcsrchr(szPath, _T('.'));
               szExt[0] = _T('\0');
               if( psz != NULL ) ::lstrcpy(szExt, psz);
               SHFILEINFO sfi = { 0 };
               HIMAGELIST hImageListLarge = (HIMAGELIST) ::SHGetFileInfo(szExt, 
                                             FILE_ATTRIBUTE_NORMAL,
                                             &sfi, sizeof(sfi), 
                                             SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_SYSICONINDEX);
               if (hImageListLarge) *phiconLarge = ImageList_GetIcon(hImageListLarge, sfi.iIcon, ILD_TRANSPARENT);
               HIMAGELIST hImageListSmall = (HIMAGELIST) ::SHGetFileInfo(szExt, 
                                             FILE_ATTRIBUTE_NORMAL,
                                             &sfi, sizeof(sfi), 
                                             SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX);
               if (hImageListSmall) *phiconSmall = ImageList_GetIcon(hImageListSmall, sfi.iIcon, ILD_TRANSPARENT);
            }
            break;
         }
      }
      return S_OK;
   }
};


//////////////////////////////////////////////////////////////////////////////
// CContextMenuImpl

template< class T >
struct _ATL_CTXMENU_ENTRY
{
  UINT id;
  UINT desc;
  void (__stdcall T::*pfn)(); // method to invoke
};

#define BEGIN_CONTEXTMENU_MAP(theClass) \
   static const _ATL_CTXMENU_ENTRY<theClass>* _GetCtxMap()\
   {\
      typedef theClass _atl_event_classtype;\
      static const _ATL_CTXMENU_ENTRY<_atl_event_classtype> _ctxmap[] = {

#define CONTEXTMENU_HANDLER(id, desc, func) \
    {id, desc, (void (__stdcall _atl_event_classtype::*)())func},\

#define END_CONTEXTMENU_MAP() {0,0,NULL}}; return _ctxmap;}


template< class T >
class ATL_NO_VTABLE CShellContextMenu : 
   public IContextMenu,
   public IShellExtInit
{
public:
   CRegKey m_regClass;
   CSimpleArray<CComBSTR> m_arrFiles;

// IShellExtInit
public:
   STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder,
                         IDataObject* pDataObj, 
                         HKEY hkeyProgID)
   {
      ATLTRACE(_T("CShellContextMenu::Initialize\n"));
      // Get file class
      m_regClass.Open(hkeyProgID, NULL);
      // Get files
      if( pDataObj ) {
         CComPtr<IDataObject> spDataObj(pDataObj);
         STGMEDIUM medium;
         FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
         if( SUCCEEDED(spDataObj->GetData(&fe, &medium)) ) {
            // Get the filenames from the CF_HDROP.
            HDROP hDrop = (HDROP) GlobalLock(medium.hGlobal);
            UINT uCount = ::DragQueryFile(hDrop, (UINT) -1, NULL, 0);
            for( UINT i = 0; i < uCount; i++ ) {
               TCHAR szFileName[MAX_PATH] = { 0 };
               ::DragQueryFile(hDrop, i, szFileName, (sizeof(szFileName) / sizeof(TCHAR)) - 1);
               CComBSTR bstrFilename = szFileName;
               m_arrFiles.Add(bstrFilename);
            }
            GlobalUnlock(medium.hGlobal);
            ::ReleaseStgMedium(&medium);
         }
      }    
      return S_OK;
    }

// IContextMenu
public:
   STDMETHOD(QueryContextMenu)(HMENU hMenu,
                               UINT iIndexMenu,
                               UINT idCmdFirst,
                               UINT idCmdLast,
                               UINT uFlags)
   {
      ATLTRACE(_T("CShellContextMenu::QueryContextMenu\n"));
      const _ATL_CTXMENU_ENTRY<T>* pMap = T::_GetCtxMap();
      UINT i = 0;
      while( pMap->pfn != NULL ) {  
        if( pMap->id == 0 ) {
          ::InsertMenu(hMenu, iIndexMenu++, MF_SEPARATOR | MF_STRING | MF_BYPOSITION, 0, _T(""));
        }
        else {
           TCHAR szText[128] = { 0 };
          ::LoadString(_Module.GetResourceInstance(), pMap->id, szText, (sizeof(szText) / sizeof(TCHAR)) - 1);
          ::InsertMenu(hMenu, iIndexMenu++, MF_STRING | MF_BYPOSITION, idCmdFirst + i, szText);
        }
        i++;
        pMap++;
      }
      T* pT = static_cast<T*>(this);
      pT->UpdateMenu(hMenu, idCmdFirst);
      return MAKE_HRESULT(SEVERITY_SUCCESS, 0, i + 1);
   }

   STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpcmi)
   {
      ATLTRACE(_T("CShellContextMenu::InvokeCommand\n"));
      if( HIWORD(lpcmi->lpVerb) ) return NOERROR; // The command is being sent via a verb
      UINT idxCmd = LOWORD(lpcmi->lpVerb);
      const _ATL_CTXMENU_ENTRY<T>* pMap = T::_GetCtxMap();
      UINT i=0;
      while( pMap->pfn != NULL ) {
         if( i == idxCmd ) {
            T* pT = static_cast<T*>(this);
            CComStdCallThunk<T> thunk;
            thunk.Init(pMap->pfn, pT);
            VARIANT vRes;
            ::VariantInit(&vRes);
            return DispCallFunc(
               &thunk,
               0,
               CC_STDCALL,
               VT_EMPTY, // this is how DispCallFunc() represents void
               0,
               NULL,
               NULL,
               &vRes);
         }
         pMap++;
         i++;
      }
      return E_INVALIDARG;
   }

   STDMETHOD(GetCommandString)(UINT idCmd, UINT uFlags, LPUINT, LPSTR pszName, UINT cchMax)
   {
      ATLTRACE(_T("CContextMenu::GetCommandString\n"));
      switch( uFlags ) {
      case GCS_HELPTEXTA:
      case GCS_HELPTEXTW:
         {
            const _ATL_CTXMENU_ENTRY<T>* pMap = T::_GetCtxMap();
            UINT i=0;
            while( pMap->pfn != NULL ) {
              if( i == idCmd ) {
                if( uFlags == GCS_HELPTEXTA ) {
                   ::LoadStringA(_Module.GetResourceInstance(), pMap->desc, pszName, cchMax);
                }
                else {
                   // BUG: LoadStringW() is not supported on Win95
                   ::LoadStringW(_Module.GetResourceInstance(), pMap->desc, (LPWSTR)pszName, cchMax);
                } 
                return S_OK;
              }
              pMap++;
              i++;
            }
         }
         return E_FAIL;
      case GCS_VERBA:
      case GCS_VERBW:
         return E_FAIL;
      case GCS_VALIDATE:
         return NOERROR;
      default:
         return E_NOTIMPL;
      }
   }

   void UpdateMenu(HMENU hMenu, UINT iCmdFirst) { };
};

#endif // __ATLCOM_H__


//////////////////////////////////////////////////////////////////////////////
// ::SHGetPathFromIDList() wrapper

class CShellPidlPath
{
public:
   CShellPidlPath(LPCITEMIDLIST pidl)
   {
      ATLASSERT(pidl);     
      if( ::SHGetPathFromIDList(pidl, m_szPath) == FALSE ) m_szPath[0] = _T('\0');
   }
   operator LPCTSTR() const { return m_szPath; };
   TCHAR m_szPath[MAX_PATH];
};


//////////////////////////////////////////////////////////////////////////////
// Shell Helper Functions

inline LPITEMIDLIST ShellGetFileNamePidl(LPOLESTR pstrFileName)
{
   LPSHELLFOLDER pDesktopFolder = NULL;
   if( FAILED(::SHGetDesktopFolder(&pDesktopFolder)) ) return NULL;
   LPITEMIDLIST pidl;
   ULONG dwEaten = 0;
   ULONG dwAttribs = 0;
   if( FAILED( pDesktopFolder->ParseDisplayName(NULL, NULL, pstrFileName, &dwEaten, &pidl, &dwAttribs) ) ) {
      pidl = NULL;
   }
   pDesktopFolder->Release();
   return pidl; 
}

#endif // __ATLSHELLEXT_H__

