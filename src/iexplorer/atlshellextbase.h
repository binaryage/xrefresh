#ifndef __ATLSHELLEXTBASE_H__
#define __ATLSHELLEXTBASE_H__

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

#ifndef __cplusplus
   #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
   #error atlshellbase.h requires atlbase.h to be included first
#endif

#include <shlobj.h>
#include <shlguid.h> 
#include <shellapi.h>

#include <commctrl.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")


class CShellMalloc
{
public:
   LPMALLOC m_pMalloc;
   void Init()
   {
      m_pMalloc = NULL;
      // It is safe to call ::SHGetMalloc()/::CoGetMalloc() without
      // first calling ::CoInitialize() according to MSDN.
      if( FAILED( ::SHGetMalloc(&m_pMalloc) ) ) {
         // TODO: TERMINATE
      }
   }
   void Term()
   {
      if( m_pMalloc!=NULL ) m_pMalloc->Release();
   }
   operator LPMALLOC() const
   {
      return m_pMalloc;
   }
   LPVOID Alloc(ULONG cb)
   {
      ATLASSERT(m_pMalloc!=NULL);
      ATLASSERT(cb>0);
      return m_pMalloc->Alloc(cb);
   }
   void Free(LPVOID p)
   {
      ATLASSERT(m_pMalloc!=NULL);
      ATLASSERT(p);
      m_pMalloc->Free(p);
   }
};


class CShellImageLists
{
public:
   HIMAGELIST m_hImageListSmall;
   HIMAGELIST m_hImageListLarge;
   CSimpleMap<CComBSTR, int> m_mapFiles;
   CShellImageLists() : m_hImageListSmall(NULL), m_hImageListLarge(NULL)
   {
   }
   virtual ~CShellImageLists()
   {
      if( m_hImageListSmall ) ImageList_Destroy(m_hImageListSmall);
      if( m_hImageListLarge ) ImageList_Destroy(m_hImageListLarge);
   }
   BOOL Create(HINSTANCE hResource, LPCTSTR RootID, LPCTSTR FolderID)
   {
      // Set the small image list
      if( m_hImageListSmall != NULL ) ImageList_Destroy(m_hImageListSmall);
      int nSmallCx = ::GetSystemMetrics(SM_CXSMICON);
      int nSmallCy = ::GetSystemMetrics(SM_CYSMICON);
      m_hImageListSmall = ImageList_Create(nSmallCx, nSmallCy, ILC_COLORDDB | ILC_MASK, 4, 0);

      // Set the large image list
      if( m_hImageListLarge != NULL ) ImageList_Destroy(m_hImageListLarge);
      int nLargeCx = ::GetSystemMetrics(SM_CXICON);
      int nLargeCy = ::GetSystemMetrics(SM_CYICON);
      m_hImageListLarge = ImageList_Create(nLargeCx, nLargeCy, ILC_COLORDDB | ILC_MASK, 4, 0);
      
      if( m_hImageListSmall != NULL ) {
         HICON hIcon;
         hIcon = (HICON)::LoadImage(hResource, 
                                    MAKEINTRESOURCE(RootID),
                                    IMAGE_ICON, 
                                    nSmallCx, nSmallCy, 
                                    LR_DEFAULTCOLOR);
         ImageList_AddIcon(m_hImageListSmall, hIcon);
         hIcon = (HICON)::LoadImage(hResource, 
                                    MAKEINTRESOURCE(FolderID),
                                    IMAGE_ICON, 
                                    nSmallCx, nSmallCy, 
                                    LR_DEFAULTCOLOR);
         ImageList_AddIcon(m_hImageListSmall, hIcon);
      }      
      if( m_hImageListSmall != NULL ) {
         HICON hIcon;
         hIcon = (HICON)::LoadImage(hResource,
                                    MAKEINTRESOURCE(RootID),
                                    IMAGE_ICON,
                                    nLargeCx, nLargeCy,
                                    LR_DEFAULTCOLOR);
         ImageList_AddIcon(m_hImageListLarge, hIcon);
         hIcon = (HICON)::LoadImage(hResource,
                                    MAKEINTRESOURCE(FolderID),
                                    IMAGE_ICON,
                                    nLargeCx, nLargeCy,
                                    LR_DEFAULTCOLOR);
         ImageList_AddIcon(m_hImageListLarge, hIcon);
      }
      
      return TRUE;
   }
};


class CShellModule : public CComModule
{
public:
   HRESULT Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, const GUID* plibid = NULL)
   {
      ::OleInitialize(NULL);
#ifdef INITCOMMONCONTROLSEX
      INITCOMMONCONTROLSEX iccex;
      iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
      iccex.dwICC = ICC_LISTVIEW_CLASSES;
      ::InitCommonControlsEx(&iccex);
#else
      ::InitCommonControls();
#endif // INITCOMMONCONTROLSEX

#ifndef _NO_CLIPFORMATS
#define CFSTR_OLECLIPBOARDPERSISTONFLUSH TEXT("OleClipboardPersistOnFlush")
#define CFSTR_DRAGIMAGEBITS              TEXT("DragImageBits")
#ifndef CFSTR_LOGICALPERFORMEDDROPEFFECT 
   #define CFSTR_LOGICALPERFORMEDDROPEFFECT TEXT("Logical Performed DropEffect")
#endif // CFSTR_LOGICALPERFORMEDDROPEFFECT
      m_CFSTR_FILEDESCRIPTOR             = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
      m_CFSTR_FILECONTENTS               = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_FILECONTENTS);
      m_CFSTR_PASTESUCCEEDED             = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_PASTESUCCEEDED);
      m_CFSTR_LOGICALPERFORMEDDROPEFFECT = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_LOGICALPERFORMEDDROPEFFECT);
      m_CFSTR_PERFORMEDDROPEFFECT        = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT);
      m_CFSTR_PREFERREDDROPEFFECT        = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
      m_CFSTR_SHELLIDLIST                = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_SHELLIDLIST);
      m_CFSTR_OLECLIPBOARDPERSISTONFLUSH = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_OLECLIPBOARDPERSISTONFLUSH);
#if (_WIN32_WINNT >= 0x0500)
      m_CFSTR_TARGETCLSID                = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_TARGETCLSID);
      m_CFSTR_DRAGCONTEXT                = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_DRAGCONTEXT);
      m_CFSTR_DRAGIMAGEBITS              = (CLIPFORMAT) ::RegisterClipboardFormat(CFSTR_DRAGIMAGEBITS);      
#endif // WIN32_WINNT
#endif // _NO_CLIPFORMATS

      // Get Shell allocator
      m_Allocator.Init();
      // Get Win version
      OSVERSIONINFO ovi = { 0 };
      ovi.dwOSVersionInfoSize = sizeof(ovi);
      ::GetVersionEx(&ovi);
      m_wWinMajor = (WORD) ovi.dwMajorVersion;
      m_wWinMinor = (WORD) ovi.dwMinorVersion;
      return CComModule::Init(p, h, plibid);
   }
   void Term()
   {
      m_Allocator.Term();
      CComModule::Term();
   }
   // Shell Allocator
   CShellMalloc m_Allocator;
   WORD m_wWinMajor;
   WORD m_wWinMinor;
#ifndef _NO_CLIPFORMATS
   // Clipboard formats
   CLIPFORMAT m_CFSTR_SHELLIDLIST;
   CLIPFORMAT m_CFSTR_FILECONTENTS;
   CLIPFORMAT m_CFSTR_PASTESUCCEEDED;
   CLIPFORMAT m_CFSTR_FILEDESCRIPTOR;
   CLIPFORMAT m_CFSTR_PERFORMEDDROPEFFECT;
   CLIPFORMAT m_CFSTR_PREFERREDDROPEFFECT;
   CLIPFORMAT m_CFSTR_LOGICALPERFORMEDDROPEFFECT;
   CLIPFORMAT m_CFSTR_OLECLIPBOARDPERSISTONFLUSH;
#if (_WIN32_WINNT >= 0x0500)
   CLIPFORMAT m_CFSTR_TARGETCLSID;
   CLIPFORMAT m_CFSTR_DRAGCONTEXT;
   CLIPFORMAT m_CFSTR_DRAGIMAGEBITS;
#endif // _WIN32_WINNT
#endif // _NO_CLIPFORMATS
};


#endif // __ATLSHELLEXTBASE_H__

