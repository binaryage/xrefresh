#pragma once

inline void ReportRegistryError()
{
	LPVOID lpMsgBuf;
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
		TRACE_E(_T("Format message failed"));
		return;
	}
	TRACE_E(FS(_T("Registry: %s"), lpMsgBuf));
	LocalFree(lpMsgBuf);
}

inline HKEY GetRegistryKey(LPCTSTR keyRoot, LPCTSTR keyName)
{
	TCHAR* fullKeyName;
	HKEY hKey = NULL;
	LONG lRes;

	size_t lenRoot = _tcslen(keyRoot);
	size_t lenName = _tcslen(keyName);
	fullKeyName = (TCHAR*)malloc(sizeof(TCHAR)*(lenRoot + lenName + 1));
	if (!fullKeyName) return NULL;

	_tcscpy_s(fullKeyName, lenRoot + lenName + 1, keyRoot);
	_tcscpy_s(fullKeyName + lenRoot, lenName + 1, keyName);

	lRes = RegOpenKeyEx(HKEY_CURRENT_USER, fullKeyName, 0, KEY_ALL_ACCESS, &hKey);
	free(fullKeyName);

	if (lRes != ERROR_SUCCESS)	return NULL;
	return hKey;
}

inline int GetStringValueFromRegistry(LPCTSTR keyRoot, LPCTSTR keyName, LPCTSTR valueName, LPTSTR value, LPDWORD size)
{
	int result = 0;
	TCHAR *fullKeyName;
	HKEY hKey = NULL;
	LONG lRes;

	size_t lenRoot = _tcslen(keyRoot);
	size_t lenName = _tcslen(keyName);
	fullKeyName = (TCHAR*)malloc(sizeof(TCHAR)*(lenRoot + lenName + 1));
	if (!fullKeyName) return 1;

	_tcscpy_s(fullKeyName, lenRoot + lenName + 1, keyRoot);
	_tcscpy_s(fullKeyName + lenRoot, lenName + 1, keyName);

	lRes = RegOpenKeyEx(HKEY_CURRENT_USER, fullKeyName, 0, KEY_ALL_ACCESS, &hKey);
	if (lRes == ERROR_SUCCESS)
	{
		DWORD dwType;
		lRes = RegQueryValueEx(hKey, valueName, NULL, &dwType, (LPBYTE)value, size);
		if (dwType != REG_SZ) result = 4;
		if (lRes != ERROR_SUCCESS) result = 3;
	}
	else result = 2;

	free(fullKeyName);
	RegCloseKey(hKey);
	return result;
}

inline int SetStringValueToRegistry(LPCTSTR keyRoot, LPCTSTR keyName, LPCTSTR valueName, LPCTSTR value)
{
	int result = 0;
	TCHAR* fullKeyName;
	HKEY hKey = NULL;
	LONG lRes;

	size_t lenRoot = _tcslen(keyRoot);
	size_t lenName = _tcslen(keyName);
	fullKeyName = (TCHAR*)malloc(sizeof(TCHAR)*(lenRoot + lenName + 1));
	if (!fullKeyName) return 1;

	_tcscpy_s(fullKeyName, lenRoot + lenName + 1, keyRoot);
	_tcscpy_s(fullKeyName + lenRoot, lenName + 1, keyName);

	lRes = RegOpenKeyEx(HKEY_CURRENT_USER, fullKeyName, 0, KEY_ALL_ACCESS, &hKey);
	if (lRes != ERROR_SUCCESS)
	{
		if (RegCreateKey(HKEY_CURRENT_USER, fullKeyName, &hKey))
		{
			free(fullKeyName);
			ReportRegistryError();
			return 2;
		}
	}

	DWORD dwType = REG_SZ;
	DWORD dwSize = (DWORD)(_tcslen(value)+1)*sizeof(TCHAR);
	lRes = RegSetValueEx(hKey, valueName, NULL, dwType, (LPBYTE)value, dwSize);
	if (lRes != ERROR_SUCCESS) 
	{
		result = 3;
		ReportRegistryError();
	}

	free(fullKeyName);
	RegCloseKey(hKey);
	return result;
}

inline int GetDWORDValueFromRegistry(LPCTSTR keyRoot, LPCTSTR keyName, LPCTSTR valueName, LPDWORD value)
{
	int result = 0;
	TCHAR *fullKeyName;
	HKEY hKey = NULL;
	LONG lRes;

	size_t lenRoot = _tcslen(keyRoot);
	size_t lenName = _tcslen(keyName);
	fullKeyName = (TCHAR*)malloc(sizeof(TCHAR)*(lenRoot + lenName + 1));
	if (!fullKeyName) return 1;

	_tcscpy_s(fullKeyName, lenRoot + lenName + 1, keyRoot);
	_tcscpy_s(fullKeyName + lenRoot, lenName + 1, keyName);

	lRes = RegOpenKeyEx(HKEY_CURRENT_USER, fullKeyName, 0, KEY_ALL_ACCESS, &hKey);
	if (lRes == ERROR_SUCCESS)
	{
		DWORD dwType;
		DWORD dwSize = sizeof(DWORD);
		lRes = RegQueryValueEx(hKey, valueName, NULL, &dwType, (LPBYTE)value, &dwSize);
		if (dwType != REG_DWORD) result = 4;
		if (lRes != ERROR_SUCCESS) result = 3;
	}
	else result = 2;

	free(fullKeyName);
	RegCloseKey(hKey);
	return result;
}

inline int SetDWORDValueToRegistry(LPCTSTR keyRoot, LPCTSTR keyName, LPCTSTR valueName, DWORD value)
{
	int result = 0;
	TCHAR* fullKeyName;
	HKEY hKey = NULL;
	LONG lRes;

	size_t lenRoot = _tcslen(keyRoot);
	size_t lenName = _tcslen(keyName);
	fullKeyName = (TCHAR*)malloc(sizeof(TCHAR)*(lenRoot + lenName + 1));
	if (!fullKeyName) return 1;

	_tcscpy_s(fullKeyName, lenRoot + lenName + 1, keyRoot);
	_tcscpy_s(fullKeyName + lenRoot, lenName + 1, keyName);

	lRes = RegOpenKeyEx(HKEY_CURRENT_USER, fullKeyName, 0, KEY_ALL_ACCESS, &hKey);
	if (lRes != ERROR_SUCCESS)
	{
		if (RegCreateKey(HKEY_CURRENT_USER, fullKeyName, &hKey))
		{
			free(fullKeyName);
			ReportRegistryError();
			return 2;
		}
	}

	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);
	lRes = RegSetValueEx(hKey, valueName, NULL, dwType, (LPBYTE)&value, dwSize);
	if (lRes != ERROR_SUCCESS) 
	{
		ReportRegistryError();
		result = 3;
	}

	free(fullKeyName);
	RegCloseKey(hKey);
	return result;
}
