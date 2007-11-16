// Guid.h: interface for the CGuid class.

#pragma once

#define GUID_STRING_LEN	40

class CGuid {
public:

	typedef struct {
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned short Data4;
		unsigned long  Data5;
	} TGUID;

	static const TGUID                             m_Null;

	CGuid();
	virtual ~CGuid();

	CGuid(BSTR guid);
	CGuid(const CString& csGuid);
	CGuid(LPCTSTR lpszGuid);
	CGuid(const CGuid& g);
	CGuid(const TGUID& g);
 
	CGuid& operator=(const TGUID& g);
	CGuid& operator=(const CGuid& g);
	CGuid& operator=(BSTR g);
	CGuid& operator=(const CComBSTR& g);
	CGuid& operator=(const CString& g);
	CGuid& operator=(LPCTSTR g);

	bool operator==(const TGUID& g);
	bool operator==(const CGuid& g);

	bool operator!=(const TGUID& g);
	bool operator!=(const CGuid& g);

	operator CComBSTR();
	operator CString();
	operator LPCTSTR();
	operator TGUID*();
	operator TGUID&();

	bool operator<(const CGuid& g1) const;
	bool operator>(const CGuid& g1) const;

	size_t GetKey() const;

	bool Create();
	bool Create(CComBSTR& guid);

	TCHAR* ConvertTo2();
	bool ConvertTo();
	bool ConvertTo(CComBSTR& bstrGuid);
	bool ConvertTo(CString& csGuid);

	bool ConvertFrom(BSTR bstrGuid);
	bool ConvertFrom(const CComBSTR& bstrGuid);
	bool ConvertFrom(const CString& bstrGuid);
	bool ConvertFrom(LPCTSTR lpszGuid);

protected:
	void copy(const CGuid& g);	

	TGUID                                          m_GUID;
	TCHAR                                          m_Buffer[GUID_STRING_LEN];
};
