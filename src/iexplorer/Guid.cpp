// m_GUID.cpp: implementation of the CGuid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Guid.h"
#include "time.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#pragma comment(lib, "rpcrt4.lib")

const CGuid::TGUID                                CGuid::m_Null = { 0,0,0,0,0 };

CGuid::CGuid()
{
	m_GUID = CGuid::m_Null;
}

CGuid::CGuid(BSTR guid)
{
	operator=(guid);
}

CGuid::CGuid(const CString& csGuid)
{
	operator=(csGuid);
}

CGuid::CGuid(LPCTSTR lpszGuid)
{
	operator=(lpszGuid);
}

CGuid::CGuid(const TGUID& g)
{
	m_GUID = g;
}

CGuid::~CGuid()
{

}

CGuid::CGuid(const CGuid& g)
{
	m_GUID = g.m_GUID;	
}

bool CGuid::operator==(const TGUID& g)
{
	return ::memcmp(&m_GUID, &g, sizeof(TGUID)) == 0;
}

bool CGuid::operator==(const CGuid& g)
{	
	return operator==(g.m_GUID);
}

CGuid::operator TGUID&()
{ 
	return m_GUID; 
}

CGuid::operator TGUID*()
{ 
	return &m_GUID; 
}

CGuid::operator CComBSTR()
{ 
	CComBSTR bstrGuid;
	ConvertTo(bstrGuid);
	return bstrGuid; 
}

CGuid::operator CString()
{ 
	CString csGuid;
	ConvertTo(csGuid);
	return csGuid; 
}

CGuid::operator LPCTSTR()
{ 
	ConvertTo();
	return m_Buffer; 
}

bool CGuid::operator!=(const TGUID& g)
{
	return ::memcmp(&m_GUID, &g, sizeof(TGUID)) != 0;
}

bool CGuid::operator!=(const CGuid& g)
{
	return operator!=(g.m_GUID);
}

CGuid& CGuid::operator=(const TGUID& g)
{
	if (::memcmp(&m_GUID, &g, sizeof(TGUID))!= 0) copy(g);
	return *this;
}

CGuid& CGuid::operator=(const CComBSTR& g)
{
	ATLASSERT(g.m_str);
	ConvertFrom(g);
	return *this;
}

CGuid& CGuid::operator=(BSTR g)
{
	ATLASSERT(g);
	ConvertFrom(g);
	return *this;
}

CGuid& CGuid::operator=(LPCTSTR g)
{
	ATLASSERT(g);
	ConvertFrom(g);
	return *this;
}

CGuid& CGuid::operator=(const CString& g)
{
	ConvertFrom(g);
	return *this;
}

CGuid& CGuid::operator=(const CGuid& g)
{
	if(this!=&g) copy(g.m_GUID);
	return *this;
}

inline void CGuid::copy(const CGuid& g)
{
	m_GUID = g.m_GUID;
}

bool CGuid::operator<(const CGuid& g1) const
{
	// replacement for UuidCompare
	return memcmp(const_cast<TGUID*>(&m_GUID), const_cast<TGUID*>(&g1.m_GUID), sizeof(TGUID))<0;
}

bool CGuid::operator>(const CGuid& g1) const
{
	// replacement for UuidCompare
	return memcmp(const_cast<TGUID*>(&m_GUID), const_cast<TGUID*>(&g1.m_GUID), sizeof(TGUID))>0;
}

bool CGuid::ConvertTo()
{
	// replacement for StringFromGUID2
	_stprintf_s(m_Buffer, GUID_STRING_LEN, _T("{%08X-%04X-%04X-%04X-%08X}"), 
		m_GUID.Data1, 
		m_GUID.Data2,
		m_GUID.Data3,
		m_GUID.Data4, 
		m_GUID.Data5);
	return true;
}

TCHAR* CGuid::ConvertTo2()
{
	// replacement for StringFromGUID2
	_stprintf_s(m_Buffer, GUID_STRING_LEN, _T("%08X_%04X_%04X_%04X_%08X"), 
		m_GUID.Data1, 
		m_GUID.Data2,
		m_GUID.Data3,
		m_GUID.Data4, 
		m_GUID.Data5);
	return m_Buffer;
}

bool CGuid::ConvertTo(CComBSTR& bstrGuid)
{
	// replacement for StringFromGUID2
	ConvertTo();
	bstrGuid = m_Buffer;
	return true;
}

bool CGuid::ConvertTo(CString& csGuid)
{
	// replacement for StringFromGUID2
	ConvertTo();
	csGuid = m_Buffer;
	return true;
}

bool CGuid::ConvertFrom(const CComBSTR& bstrGuid)
{	
	return ConvertFrom(bstrGuid.m_str);
}

bool CGuid::ConvertFrom(BSTR bstrGuid)
{
	if (bstrGuid == NULL)
	{
		return false;
	}
	
	UINT nLen = ::SysStringLen(bstrGuid);
	if (nLen < GUID_STRING_LEN - 4)
	{
		return false;
	}

	CString csguid = bstrGuid;
	if (csguid.GetAt(0) == TCHAR('{'))
	{
		ATLASSERT(csguid.Find(TCHAR('}'))!=-1);
		return ConvertFrom(csguid.Mid(1, csguid.GetLength()-2));
	}
	else
	{
		return ConvertFrom(csguid);
	}
}

bool 
CGuid::ConvertFrom(const CString& csguid)
{
	if (csguid.GetLength() < GUID_STRING_LEN - 4)
	{
		return false;
	}
	return ConvertFrom((LPCTSTR)csguid);
}

bool 
CGuid::ConvertFrom(LPCTSTR lpszGuid)
{
	// replacement for UuidFromString
	ATLASSERT(lpszGuid);
	return (5==_stscanf_s(lpszGuid, _T("{%08X-%04X-%04X-%04X-%08X}"), 
		&m_GUID.Data1, 
		&m_GUID.Data2,
		&m_GUID.Data3,
		&m_GUID.Data4, 
		&m_GUID.Data5));
}

bool 
CGuid::Create(CComBSTR& bstrGuid)
{
	return ConvertFrom(bstrGuid);
}

bool 
CGuid::Create()
{
	// replacement for UuidCreate
	srand((unsigned)time(NULL));
	m_GUID.Data1 = (unsigned long)rand();
	m_GUID.Data2 = (unsigned short)rand();
	m_GUID.Data3 = (unsigned short)rand();
	m_GUID.Data4 = (unsigned short)rand();
	m_GUID.Data5 = (unsigned long)rand();
	return true;
}

size_t CGuid::GetKey() const
{
	// replacement for UuidHash
	return (size_t)(m_GUID.Data1+m_GUID.Data2+m_GUID.Data3+m_GUID.Data4+m_GUID.Data5);
}
