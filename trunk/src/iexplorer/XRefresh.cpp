// XRefresh.cpp : Implementation of DLL Exports.
#include "stdafx.h"
#include "resource.h"
#include "Module.h"

class CXRefreshModule : public CAtlDllModuleT<CXRefreshModule> {
public:
	DECLARE_LIBID(LIBID_XRefreshLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_XREFRESH, "{05B6ED58-9CB1-4270-82F7-30CCD8C4F8F0}")
};

CXRefreshModule _AtlModule;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

static void SetupExceptionHandler()
{
#ifndef _DEBUG
	/*
	BT_SetSupportEMail(_T("bugs@xrefresh.com"));
	BT_SetFlags(BTF_DETAILEDMODE | BTF_EDIETMAIL | BTF_ATTACHREPORT);
	BT_SetSupportURL(_T("http://xrefresh.com/support"));
	BT_SetDialogMessage(BTDM_INTRO1, _T("XRefresh Internet Explorer Addon has crashed."));
	BT_SetDialogMessage(BTDM_INTRO2, _T("Please help us improve this software by submitting this bug report."));
	*/
#endif
}

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE_I(_T("==="));
		TRACE_I(_T("DLL: process attach"));
		DisableThreadLibraryCalls(hInstance);
		IsolationAwareInit();
		CoInitialize(NULL);
		SetupExceptionHandler();
		return _AtlModule.DllMain(dwReason, lpReserved); 
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE_I(_T("DLL: process detach"));
		CoUninitialize();
		//IsolationAwareCleanup();
		return TRUE;
	}

	return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
	TRACE_I(_T("DllCanUnloadNow()"));
	return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	TRACE_I(_T("DllGetClassObject(...)"));
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	TRACE_I(_T("DllRegisterServer()"));
	// registers object, typelib and all interfaces in typelib
	return _AtlModule.DllRegisterServer();
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	TRACE_I(_T("DllUnregisterServer()"));
	return _AtlModule.DllUnregisterServer();
}