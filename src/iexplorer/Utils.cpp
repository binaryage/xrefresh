// XRefreshUtils.cpp : Implementation of XRefreshUtils

#include "stdafx.h"
#include <Mlang.h>

// {4717CC40-BCB9-11d0-9336-00A0C90DCAA9}
EXTERN_C const GUID DECLSPEC_SELECTANY SID_GetCaller = { 0x4717cc40, 0xbcb9, 0x11d0, { 0x93, 0x36, 0x0, 0xa0, 0xc9, 0xd, 0xca, 0xa9 } };

long g_ComObjectBreak = -1;
long ATL::g_LastComObjectDebugId = 0;

CString LoadStringResource(UINT id)
{
	CString s;
	s.LoadString(id);
	return s;
}

static CRITICAL_SECTION g_kFormatStringCS;
static bool g_bFormatStringCS = false;

#define FORMAT_STRING_BUFFER_COUNT 100000
const TCHAR *
FormatStringVA(const TCHAR * format, va_list arglist)
{
	if (!g_bFormatStringCS) 
	{
		InitializeCriticalSection(&g_kFormatStringCS);
		g_bFormatStringCS = true;
	}

	EnterCriticalSection(&g_kFormatStringCS);

	static TCHAR buffer[FORMAT_STRING_BUFFER_COUNT];
	static int iterator = 0;

	if (iterator > (FORMAT_STRING_BUFFER_COUNT/4)) iterator = 0;
	TCHAR * actual = buffer + iterator;
	int size = FORMAT_STRING_BUFFER_COUNT - iterator;

	int ret = _vstprintf_s(actual, size, format, arglist);
	iterator += ret >= 0 ? ret + 1 : size;
	buffer[iterator-1] = 0;

	LeaveCriticalSection(&g_kFormatStringCS);
	return actual;
}

const TCHAR *
FormatString(const TCHAR * format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	const TCHAR * ret = FormatStringVA(format, arglist);
	va_end(arglist);
	return ret;
}

CString VariantToString(VARIANT * va)
{
	CString s;
	switch(va->vt) { 
	case VT_BSTR:
		return CString(va->bstrVal);
	case VT_BSTR | VT_BYREF:
	case VT_BSTR | VT_BYREF | VT_R4:
		return CString(*va->pbstrVal);
	case VT_I4:
		s.Format(_T("%d"), va->lVal);
		return s;
	case VT_I4 | VT_BYREF:
		s.Format(_T("%d"), *va->plVal);
	case VT_R8:
		s.Format(_T("%f"), va->dblVal);
		return s;
	}

	//CComVariant	v;
	//try {
	//	v.ChangeType(VT_BSTR, va);
	//	s = v.bstrVal;
	//}
	//catch (...)
	//{
	//	ATLASSERT(FALSE); // unknown VARIANT type (this ASSERT is optional)
	//	return CString("");
	//}

	return s;
}

BSTR 
GetUnicodeHTML(CComPtr<IHTMLDocument2> spDocument)
{
	ATLASSERT(!!spDocument);
	BSTR str = 0;

	// retrieve IPersistStreamInit interface
	CComQIPtr<IPersistStreamInit> persistStream = spDocument;
	ATLASSERT(!!persistStream);

	// save stream
	IStream* stream;
	CreateStreamOnHGlobal(NULL, TRUE, &stream);
	persistStream->Save(stream, FALSE);

	// lock stream memory
	HGLOBAL handle;
	GetHGlobalFromStream(stream, &handle);
	size_t size = GlobalSize(handle);
	LPVOID ptr = GlobalLock(handle);

	if (size>=2 && ((TCHAR*)ptr)[0]==0xFEFF) // we have her UCS-2LE Unicode little-endian
	{
		// copy string as is
		ATLASSERT(size%2==0);
		str = ::SysAllocStringLen(NULL, (ULONG)size>>1);
		memcpy(str, ptr, size);
		str[size>>1] = 0; // add ending marker
	}
	else
	{
		// try to convert string
		str = ::SysAllocStringLen(NULL, (ULONG)size+2); // +1 for leading 0xFEFF, +1 for ending marker 0
		str[0] = 0xFEFF;
		int written = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)ptr, (int)size, str+1, (int)size+1); // str+1 means start writing from second character
		str[written+1] = 0;
	}
	GlobalUnlock(handle);

	// don't forget to release the stream
	stream->Release();
	return str;
}