/*
Filename: INET.H
Description: Defines some wrapper classes for wininet functions
Date: 25/06/2005

Copyright (c) 2005 by Gilad Novik.  (Web: http://gilad.gsetup.com, Email: gilad@gsetup.com)
All rights reserved.

Copyright / Usage Details
-------------------------
You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise)
when your product is released in binary form. You are allowed to modify the source code in any way you want
except you cannot modify the copyright details at the top of the module. If you want to distribute source
code with your application, then you are only allowed to distribute versions released by the author. This is
to maintain a single distribution point for the source code.
*/

#pragma once

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

class CInternetException;
class CInternetStatucCallback;
class CInternetHandle;
class CInternetSession;
class CInternetConnection;
class CHttpConnection;
class CFtpConnection;
class CInternetFile;
class CHttpFile;
class CHttpsFile;
class CFtpFile;
class CFtpFindFile;

class CInternetException
{
public:
	explicit CInternetException(HINTERNET hHandle,LPCTSTR szMessage,...) : m_hHandle(hHandle)
	{
		m_dwError=GetLastError();
#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
        DWORD dwLength=0;
		InternetGetLastResponseInfo(&m_dwInternetError,NULL,&dwLength);
		if (dwLength)
		{
			InternetGetLastResponseInfo(&m_dwInternetError,m_szInternetMessage.GetBuffer(dwLength-1),&dwLength);
			m_szInternetMessage.ReleaseBuffer();
		}
		else
			m_szInternetMessage=_T("Unknown error");
#else
		DWORD dwLength=sizeof(m_szInternetMessage)/sizeof(TCHAR);
		if (!InternetGetLastResponseInfo(&m_dwInternetError,m_szInternetMessage,&dwLength) && GetLastError()!=ERROR_INSUFFICIENT_BUFFER)
			lstrcpy(m_szInternetMessage,_T("Unknown error"));
#endif
		va_list va;
		va_start(va,szMessage);
#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
		m_szMessage.FormatV(szMessage,va);
#else
		_vsntprintf(m_szMessage,sizeof(m_szMessage)/sizeof(TCHAR)-1,szMessage,va);
#endif
		va_end(va);
	}

	virtual ~CInternetException()
	{
	}

	HINTERNET GetInternetHandle() const
	{
		return m_hHandle;
	}

	DWORD GetErrorCode() const
	{
		return m_dwError;
	}

	DWORD GetInternetErrorCode() const
	{
		return m_dwInternetError;
	}

	LPCTSTR GetErrorMessage() const
	{
		return m_szMessage;
	}

	LPCTSTR GetInternetErrorMessage() const
	{
        return m_szInternetMessage;
	}

protected:
	HINTERNET m_hHandle;
	DWORD m_dwError,m_dwInternetError;
#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	_CSTRING_NS::CString m_szMessage,m_szInternetMessage;
#else
	TCHAR m_szMessage[512],m_szInternetMessage[512];
#endif
};

class CInternetStatucCallback
{
public:
	virtual void OnCallback(CInternetHandle& Handle,HINTERNET hInternet,DWORD dwStatus,LPVOID pStatusInformation,DWORD dwStatusInformationLength)
	{
		switch (dwStatus)
		{
		case INTERNET_STATUS_CLOSING_CONNECTION:
			OnClosingConnection(Handle,hInternet);
			break;
		case INTERNET_STATUS_CONNECTED_TO_SERVER:
			OnConnected(Handle,hInternet,(SOCKADDR*)pStatusInformation);
			break;
		case INTERNET_STATUS_CONNECTING_TO_SERVER:
			OnConnecting(Handle,hInternet,(SOCKADDR*)pStatusInformation);
			break;
		case INTERNET_STATUS_CONNECTION_CLOSED:
			OnConnectionClosed(Handle,hInternet);
            break;
		case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
			break;
		case INTERNET_STATUS_DETECTING_PROXY:
			OnDetectingProxy(Handle,hInternet);
			break;
		case INTERNET_STATUS_HANDLE_CLOSING:
			OnHandleClosing(Handle,hInternet);
			break;
		case INTERNET_STATUS_HANDLE_CREATED:
			OnHandleCreated(Handle,hInternet,(INTERNET_ASYNC_RESULT*)pStatusInformation);
			break;
		case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
			OnIntermediateResponse(Handle,hInternet);
			break;
		case INTERNET_STATUS_NAME_RESOLVED:
			OnNameResolved(Handle,hInternet,(LPCTSTR)pStatusInformation);
			break;
		case INTERNET_STATUS_PREFETCH:
			break;
		case INTERNET_STATUS_RECEIVING_RESPONSE:
			OnReceivingResponse(Handle,hInternet);
			break;
		case INTERNET_STATUS_REDIRECT:
			OnRedirect(Handle,hInternet,(LPCTSTR)pStatusInformation);
			break;
		case INTERNET_STATUS_REQUEST_COMPLETE:
			OnRequestComplete(Handle,hInternet,(INTERNET_ASYNC_RESULT*)pStatusInformation);
			break;
		case INTERNET_STATUS_REQUEST_SENT:
			OnRequestSent(Handle,hInternet,*(DWORD*)pStatusInformation);
			break;
		case INTERNET_STATUS_RESOLVING_NAME:
			OnResolvingName(Handle,hInternet,(LPCTSTR)pStatusInformation);
			break;
		case INTERNET_STATUS_RESPONSE_RECEIVED:
			OnResponseReceived(Handle,hInternet,*(DWORD*)pStatusInformation);
			break;
		case INTERNET_STATUS_SENDING_REQUEST:
			OnSendingRequest(Handle,hInternet);
			break;
		case INTERNET_STATUS_STATE_CHANGE:
			OnStateChange(Handle,hInternet,*(DWORD*)pStatusInformation);
			break;
		}
	}

	virtual void OnClosingConnection(CInternetHandle& Handle,HINTERNET hInternet)
	{
	}

	virtual void OnConnected(CInternetHandle& Handle,HINTERNET hInternet,const SOCKADDR* pAddress)
	{
	}

	virtual void OnConnecting(CInternetHandle& Handle,HINTERNET hInternet,const SOCKADDR* pAddress)
	{
	}

	virtual void OnConnectionClosed(CInternetHandle& Handle,HINTERNET hInternet)
	{
	}

	virtual void OnDetectingProxy(CInternetHandle& Handle,HINTERNET hInternet)
	{
	}

	virtual void OnHandleClosing(CInternetHandle& Handle,HINTERNET hInternet)
	{
	}

	virtual void OnHandleCreated(CInternetHandle& Handle,HINTERNET hInternet,const INTERNET_ASYNC_RESULT* pResult)
	{
		DWORD_PTR dwContext=(DWORD_PTR)&Handle;
		InternetSetOption((HINTERNET)pResult->dwResult,INTERNET_OPTION_CONTEXT_VALUE,&dwContext,sizeof(DWORD_PTR));
	}

	virtual void OnIntermediateResponse(CInternetHandle& Handle,HINTERNET hInternet)
	{
	}

	virtual void OnNameResolved(CInternetHandle& Handle,HINTERNET hInternet,LPCTSTR szName)
	{
	}

	virtual void OnReceivingResponse(CInternetHandle& Handle,HINTERNET hInternet)
	{
	}

	virtual void OnRedirect(CInternetHandle& Handle,HINTERNET hInternet,LPCTSTR szURL)
	{
	}

	virtual void OnRequestComplete(CInternetHandle& Handle,HINTERNET hInternet,const INTERNET_ASYNC_RESULT* pResult)
	{
	}

	virtual void OnRequestSent(CInternetHandle& Handle,HINTERNET hInternet,DWORD dwSize)
	{
	}

	virtual void OnResolvingName(CInternetHandle& Handle,HINTERNET hInternet,LPCTSTR szName)
	{
	}

	virtual void OnResponseReceived(CInternetHandle& Handle,HINTERNET hInternet,DWORD dwSize)
	{
	}

	virtual void OnSendingRequest(CInternetHandle& Handle,HINTERNET hInternet)
	{
	}

	virtual void OnStateChange(CInternetHandle& Handle,HINTERNET hInternet,DWORD dwFlags)
	{
	}
};

class CInternetHandle
{
public:
	CInternetHandle() : m_hHandle(NULL), m_pCallback(NULL)
	{
	}

	virtual ~CInternetHandle()
	{
		Close();
	}

	void Close()
	{
		if (m_hHandle)
		{
			InternetCloseHandle(m_hHandle);
			m_hHandle=NULL;
		}
	}

	operator HINTERNET() const
	{
		return m_hHandle;
	}

	void SetOption(DWORD dwOption,LPVOID pBuffer,DWORD dwLength) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!InternetSetOption(m_hHandle,dwOption,pBuffer,dwLength))
			throw CInternetException(m_hHandle,_T("Failed to set an internet option (%u)"),dwOption);
	}

	void SetOption(DWORD dwOption,DWORD dwValue) throw(...)
	{
		SetOption(dwOption,&dwValue,sizeof(dwValue));
	}

	void SetOption(DWORD dwOption,LPCTSTR szBuffer) throw(...)
	{
		SetOption(dwOption,(LPVOID)szBuffer,lstrlen(szBuffer));
	}

	void QueryOption(DWORD dwOption,LPVOID pBuffer,LPDWORD pdwLength) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!InternetQueryOption(m_hHandle,dwOption,pBuffer,pdwLength))
			throw CInternetException(m_hHandle,_T("Failed to query an internet option (%u)"),dwOption);
	}

	void QueryOption(DWORD dwOption,DWORD& dwValue) throw(...)
	{
		DWORD dwLength=sizeof(dwValue);
		QueryOption(dwOption,&dwValue,&dwLength);
	}

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	void QueryOption(DWORD dwOption,_CSTRING_NS::CString& szBuffer) throw(...)
	{
		ATLASSERT(m_hHandle);
		DWORD dwLength=0;
		if (InternetQueryOption(m_hHandle,dwOption,NULL,&dwLength) || GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			BOOL bResult=InternetQueryOption(m_hHandle,dwOption,szBuffer.GetBuffer(dwLength/sizeof(TCHAR)-1),&dwLength);
			szBuffer.ReleaseBuffer();
            if (bResult)
				return;
		}
		throw CInternetException(m_hHandle,_T("Failed to query an internet option (%u)"),dwOption);
	}
#endif

	void SetCallback(CInternetStatucCallback* pCallback) throw(...)
	{
		DWORD_PTR dwContext=(DWORD_PTR)this;
		SetOption(INTERNET_OPTION_CONTEXT_VALUE,&dwContext,sizeof(DWORD_PTR));
		InternetSetStatusCallback(m_hHandle,(m_pCallback=pCallback) ? InternetStatusCallback : NULL);
	}

protected:
	HINTERNET m_hHandle;
	CInternetStatucCallback* m_pCallback;

private:
	static void CALLBACK InternetStatusCallback(HINTERNET hInternet,DWORD_PTR dwContext,DWORD dwInternetStatus,LPVOID lpvStatusInformation,DWORD dwStatusInformationLength)
	{
		CInternetHandle* pHandle=NULL;
		DWORD dwLength=sizeof(DWORD_PTR);
		if (InternetQueryOption(hInternet,INTERNET_OPTION_CONTEXT_VALUE,&pHandle,&dwLength) && pHandle->m_pCallback)
			pHandle->m_pCallback->OnCallback(*pHandle,hInternet,dwInternetStatus,lpvStatusInformation,dwStatusInformationLength);
	}

	friend class CFtpConnection;
};

class CInternetSession : public CInternetHandle
{
public:
	CInternetSession(LPCTSTR szUserAgent=NULL,DWORD dwAccessType=INTERNET_OPEN_TYPE_PRECONFIG,LPCTSTR lpszProxy=NULL,LPCTSTR lpszProxyBypass=NULL,DWORD dwFlags=0) throw(...)
	{
		TCHAR szFilename[MAX_PATH];
		if (!szUserAgent)
		{
            GetModuleFileName(NULL,szFilename,sizeof(szFilename)/sizeof(TCHAR));
			PathRemoveExtension(szFilename);
			szUserAgent=PathFindFileName(szFilename);
		}
		if (!(m_hHandle=InternetOpen(szUserAgent,dwAccessType,lpszProxy,lpszProxyBypass,dwFlags)))
			throw CInternetException(m_hHandle,_T("Failed to open an internet session"));
	}
};

class CInternetConnection : public CInternetHandle
{
public:
	CInternetConnection(HINTERNET hSession,LPCTSTR szServer,INTERNET_PORT nPort,LPCTSTR szUsername,LPCTSTR szPassword,DWORD dwService,DWORD dwFlags) throw(...)
	{
		ATLASSERT(hSession);
		if (!(m_hHandle=InternetConnect(hSession,szServer,nPort,szUsername,szPassword,dwService,dwFlags,(DWORD_PTR)(CInternetHandle*)this)))
			throw CInternetException(m_hHandle,_T("Failed to connect to server (%s:%u)"),szServer,nPort);
	}
};

class CHttpConnection : public CInternetConnection
{
public:
	CHttpConnection(HINTERNET hSession,LPCTSTR szServer,INTERNET_PORT nPort=INTERNET_DEFAULT_HTTP_PORT,LPCTSTR szUsername=NULL,LPCTSTR szPassword=NULL,DWORD dwFlags=0) throw(...) : CInternetConnection(hSession,szServer,nPort,szUsername,szPassword,INTERNET_SERVICE_HTTP,dwFlags)
	{
	}
};

class CFtpConnection : public CInternetConnection
{
public:
	CFtpConnection(HINTERNET hSession,LPCTSTR szServer,INTERNET_PORT nPort=INTERNET_DEFAULT_FTP_PORT,LPCTSTR szUsername=NULL,LPCTSTR szPassword=NULL,DWORD dwFlags=INTERNET_FLAG_PASSIVE) throw(...) : CInternetConnection(hSession,szServer,nPort,szUsername,szPassword,INTERNET_SERVICE_FTP,dwFlags)
	{
	}

	void SetCurrentDirectory(LPCTSTR szDirectory) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpSetCurrentDirectory(m_hHandle,szDirectory))
			throw CInternetException(m_hHandle,_T("Failed to set current directory (%s)"),szDirectory);
	}

	void GetCurrentDirectory(LPTSTR szDirectory,LPDWORD pdwLength) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpGetCurrentDirectory(m_hHandle,szDirectory,pdwLength))
			throw CInternetException(m_hHandle,_T("Failed to get current directory"));
	}

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	void GetCurrentDirectory(_CSTRING_NS::CString& szDirectory) throw(...)
	{
		ATLASSERT(m_hHandle);
		DWORD dwLength=0;
		if (FtpGetCurrentDirectory(m_hHandle,NULL,&dwLength) || GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			BOOL bResult=FtpGetCurrentDirectory(m_hHandle,szDirectory.GetBuffer(dwLength/sizeof(TCHAR)-1),&dwLength);
			szDirectory.ReleaseBuffer();
			if (bResult)
				return;
		}
		throw CInternetException(m_hHandle,_T("Failed to get current directory"));
	}
#endif

	void CreateDirectory(LPCTSTR szDirectory) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpCreateDirectory(m_hHandle,szDirectory))
			throw CInternetException(m_hHandle,_T("Failed to create directory (%s"),szDirectory);
	}

	void RemoveDirectory(LPCTSTR szDirectory) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpRemoveDirectory(m_hHandle,szDirectory))
			throw CInternetException(m_hHandle,_T("Failed to remove directory (%s"),szDirectory);
	}

	void Rename(LPCTSTR szExisting,LPCTSTR szNew) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpRenameFile(m_hHandle,szExisting,szNew))
			throw CInternetException(m_hHandle,_T("Failed to rename file (%s -> %s"),szExisting,szNew);
	}

	void Remove(LPCTSTR szFilename) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpDeleteFile(m_hHandle,szFilename))
			throw CInternetException(m_hHandle,_T("Failed to remove file (%s)"),szFilename);
	}

	void Upload(LPCTSTR szLocal,LPCTSTR szRemote,DWORD dwFlags=FTP_TRANSFER_TYPE_UNKNOWN) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpPutFile(m_hHandle,szLocal,szRemote,dwFlags,(DWORD_PTR)(CInternetHandle*)this))
			throw CInternetException(m_hHandle,_T("Failed to upload file (%s -> %s)"),szLocal,szRemote);
	}

	void Download(LPCTSTR szRemote,LPCTSTR szLocal,BOOL bFailIfExists=TRUE,DWORD dwAttributes=FILE_ATTRIBUTE_NORMAL,DWORD dwFlags=FTP_TRANSFER_TYPE_UNKNOWN) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpGetFile(m_hHandle,szRemote,szLocal,bFailIfExists,dwAttributes,dwFlags,(DWORD_PTR)(CInternetHandle*)this))
			throw CInternetException(m_hHandle,_T("Failed to download file (%s -> %s)"),szRemote,szLocal);
	}

	void Command(LPCTSTR szCommand,DWORD dwFlags=FTP_TRANSFER_TYPE_BINARY) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!FtpCommand(m_hHandle,FALSE,dwFlags,szCommand,(DWORD_PTR)(CInternetHandle*)this,NULL))
			throw CInternetException(m_hHandle,_T("Failed to send ftp command (%s)"),szCommand);
	}

	void Command(LPCTSTR szCommand,CInternetHandle& Handle,DWORD dwFlags=FTP_TRANSFER_TYPE_BINARY) throw(...)
	{
		ATLASSERT(m_hHandle);
		ATLASSERT(Handle.m_hHandle==NULL);
		if (!FtpCommand(m_hHandle,TRUE,dwFlags,szCommand,(DWORD_PTR)(CInternetHandle*)this,&Handle.m_hHandle))
			throw CInternetException(m_hHandle,_T("Failed to send ftp command (%s)"),szCommand);
	}

	void FindFirst(CFtpFindFile& FF,LPCTSTR szSearch=_T("*.*"),DWORD dwFlags=0) throw(...);
};

class CInternetFile : public CInternetHandle
{
public:
	class CInfo
	{
	public:
		CInfo(DWORD dwLength=0) throw(...)
		{
			Reset(dwLength);
		}

		CInfo(CHttpFile& File) throw(...);
		CInfo(CFtpFile& File) throw(...);

		void Reset(DWORD dwLength=0)
		{
			m_fLimit=0.0;
			m_dwDataLength=dwLength;
			m_dwSecondsLeft=0;
			m_fDownloadRate=m_fAverageDownloadRate=0.0;
			m_dwRead=0;
			m_dwTimingStart=m_dwTimingLast=GetTickCount();
		}

		void SetRateLimit(double fLimit)
		{
			m_fLimit=fLimit;
		}

		DWORD GetTotalDataLength() const
		{
			return m_dwDataLength;
		}

		DWORD GetTotalRead() const
		{
			return m_dwRead;
		}

		DWORD GetTotalTime() const
		{
			return (m_dwTimingLast-m_dwTimingStart)/1000;
		}

		DWORD GetTimeLeft() const
		{
			return m_dwSecondsLeft;
		}

		double GetDownloadRate() const
		{
			return m_fDownloadRate;
		}

		double GetAverageDownloadRate() const
		{
			return m_fAverageDownloadRate;
		}

	protected:
		DWORD m_dwTimingStart,m_dwTimingLast;
		DWORD m_dwRead;

		DWORD m_dwSecondsLeft;
		double m_fDownloadRate,m_fAverageDownloadRate;
		DWORD m_dwDataLength;

		double m_fLimit;

		friend class CInternetFile;
	};

	DWORD Read(LPVOID pBuffer,DWORD dwLength) throw(...)
	{
		ATLASSERT(m_hHandle);
		DWORD dwRead;
		if (!InternetReadFile(m_hHandle,pBuffer,dwLength,&dwRead))
			throw CInternetException(m_hHandle,_T("Failed to read from network (%u bytes)"),dwLength);
		return dwRead;
	}

	DWORD Read(LPVOID pBuffer,DWORD dwLength,CInfo& Info) throw(...)
	{
		ATLASSERT(m_hHandle);
		DWORD dwRead;
		if (!InternetReadFile(m_hHandle,pBuffer,dwLength,&dwRead))
			throw CInternetException(m_hHandle,_T("Failed to read from network (%u bytes)"),dwLength);
		DWORD dwTimingCurrent=GetTickCount();
		if (Info.m_fLimit>0.0f)
		{
			double fTotalTime=(double)(dwTimingCurrent-Info.m_dwTimingStart);
			double fRate=(double)((double)Info.m_dwRead/fTotalTime);
			if (fRate>Info.m_fLimit)
				Sleep((DWORD)(((fRate*fTotalTime)/Info.m_fLimit)-fTotalTime));
		}
		DWORD dwPreviousRead=Info.m_dwRead;
		Info.m_dwRead+=dwRead;
		DWORD dwTime=dwTimingCurrent-Info.m_dwTimingLast;
		if (dwTime)
		{
			Info.m_fDownloadRate=((double)(Info.m_dwRead)-(double)(dwPreviousRead))/((double)(dwTime));
			Info.m_fAverageDownloadRate=(double)(Info.m_dwRead)/(double)(dwTimingCurrent-Info.m_dwTimingStart);
			Info.m_dwTimingLast=dwTimingCurrent;
			if (Info.m_dwDataLength)
				Info.m_dwSecondsLeft=(DWORD)(((double)dwTimingCurrent-Info.m_dwTimingStart)/Info.m_dwRead*(Info.m_dwDataLength-Info.m_dwRead)/1000);
		}
        return dwRead;
	}

	DWORD Write(LPCVOID pBuffer,DWORD dwLength) throw(...)
	{
		ATLASSERT(m_hHandle);
		DWORD dwWritten;
		if (!InternetWriteFile(m_hHandle,pBuffer,dwLength,&dwWritten))
			throw CInternetException(m_hHandle,_T("Failed to write to network (%u bytes)"),dwLength);
		return dwWritten;
	}

	DWORD SetPosition(LONG nDistance,DWORD dwMethod=FILE_BEGIN)
	{
		ATLASSERT(m_hHandle);
		return InternetSetFilePointer(m_hHandle,nDistance,NULL,dwMethod,NULL);
	}

	DWORD GetPosition() const
	{
		ATLASSERT(m_hHandle);
		return InternetSetFilePointer(m_hHandle,0,NULL,FILE_CURRENT,NULL);
	}

	DWORD GetLength() const throw(...)
	{
		ATLASSERT(m_hHandle);
		DWORD dwLength;
		if (!InternetQueryDataAvailable(m_hHandle,&dwLength,0,0))
			throw CInternetException(m_hHandle,_T("Failed to query available data length"));
		return dwLength;
	}
};

class CHttpFile : public CInternetFile
{
public:
	CHttpFile(HINTERNET hConnection,LPCTSTR szVerb,LPCTSTR szObject,LPCTSTR szVersion=NULL,LPCTSTR szReferer=NULL,LPCTSTR* szAcceptTypes=NULL,DWORD dwFlags=INTERNET_FLAG_KEEP_CONNECTION) throw(...)
	{
		ATLASSERT(hConnection);
		static LPCTSTR szAcceptAll[]={_T("*/*"),NULL};
		if (!(m_hHandle=HttpOpenRequest(hConnection,szVerb,szObject,szVersion,szReferer,szAcceptTypes ? szAcceptTypes : szAcceptAll,dwFlags,(DWORD_PTR)(CInternetHandle*)this)))
			throw CInternetException(m_hHandle,_T("Failed to open http request (%s %s)"),szVerb,szObject);
	}

	CHttpFile(HINTERNET hSession,LPCTSTR szURL,LPCTSTR szHeaders=NULL,DWORD dwHeadersLength=(DWORD)-1,DWORD dwFlags=INTERNET_FLAG_KEEP_CONNECTION) throw(...)
	{
		ATLASSERT(hSession);
		if (!(m_hHandle=InternetOpenUrl(hSession,szURL,szHeaders,dwHeadersLength,dwFlags,(DWORD_PTR)(CInternetHandle*)this)))
			throw CInternetException(m_hHandle,_T("Failed to open http url (%s)"),szURL);
	}

	void SendRequest(LPCTSTR szHeaders=NULL,DWORD dwHeadersLength=(DWORD)-1,LPVOID pOptional=NULL,DWORD dwOptionalLength=0) throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!HttpSendRequest(m_hHandle,szHeaders,dwHeadersLength,pOptional,dwOptionalLength))
			throw CInternetException(m_hHandle,_T("Failed to send http request"));
	}

	void QueryInfo(DWORD dwInfoLevel,LPVOID pBuffer,LPDWORD pdwBufferLength,LPDWORD pdwIndex=NULL) const throw(...)
	{
		ATLASSERT(m_hHandle);
		if (!HttpQueryInfo(m_hHandle,dwInfoLevel,pBuffer,pdwBufferLength,pdwIndex))
			throw CInternetException(m_hHandle,_T("Failed to query http info (%u)"),dwInfoLevel);
	}

	void QueryInfo(DWORD dwInfoLevel,DWORD& dwValue,LPDWORD pdwIndex=NULL) const throw(...)
	{
		DWORD dwLength=sizeof(dwValue);
		QueryInfo(dwInfoLevel|HTTP_QUERY_FLAG_NUMBER,&dwValue,&dwLength,pdwIndex);
	}

	void QueryInfo(DWORD dwInfoLevel,SYSTEMTIME& dtTime,LPDWORD pdwIndex=NULL) const throw(...)
	{
		DWORD dwLength=sizeof(dtTime);
		QueryInfo(dwInfoLevel|HTTP_QUERY_FLAG_SYSTEMTIME,&dtTime,&dwLength,pdwIndex);
	}

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	void QueryInfo(DWORD dwInfoLevel,_CSTRING_NS::CString& szBuffer,LPDWORD pdwIndex=NULL) const throw(...)
	{
		ATLASSERT(m_hHandle);
		DWORD dwIndex=pdwIndex ? *pdwIndex : 0,dwLength=0;
		if (HttpQueryInfo(m_hHandle,dwInfoLevel,NULL,&dwLength,&dwIndex) || GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			if (pdwIndex)
				*pdwIndex=dwIndex;
			else
				dwIndex=0;
			BOOL bResult=HttpQueryInfo(m_hHandle,dwInfoLevel,szBuffer.GetBuffer(dwLength/sizeof(TCHAR)-1),&dwLength,pdwIndex ? pdwIndex : &dwIndex);
			szBuffer.ReleaseBuffer();
			if (bResult)
				return;
		}
		throw CInternetException(m_hHandle,_T("Failed to query http info (%u)"),dwInfoLevel);
	}
#endif
};

class CHttpsFile : public CHttpFile
{
public:
	CHttpsFile(HINTERNET hConnection,LPCTSTR szVerb,LPCTSTR szObject,LPCTSTR szVersion=NULL,LPCTSTR szReferer=NULL,LPCTSTR* szAcceptTypes=NULL,DWORD dwFlags=INTERNET_FLAG_KEEP_CONNECTION) throw(...) : CHttpFile(hConnection,szVerb,szObject,szVersion,szReferer,szAcceptTypes,dwFlags|INTERNET_FLAG_SECURE)
	{
	}

	CHttpsFile(HINTERNET hSession,LPCTSTR szURL,LPCTSTR szHeaders=NULL,DWORD dwHeadersLength=(DWORD)-1,DWORD dwFlags=INTERNET_FLAG_KEEP_CONNECTION) throw(...) : CHttpFile(hSession,szURL,szHeaders,dwHeadersLength,dwFlags|INTERNET_FLAG_SECURE)
	{
	}
};

class CFtpFile : public CInternetFile
{
public:
	CFtpFile(HINSTANCE hConnection,LPCTSTR szFilename,DWORD dwAccess,DWORD dwFlags=FTP_TRANSFER_TYPE_UNKNOWN) throw(...)
	{
		ATLASSERT(hConnection);
		if (!(m_hHandle=FtpOpenFile(hConnection,szFilename,dwAccess,dwFlags,(DWORD_PTR)(CInternetHandle*)this)))
			throw CInternetException(m_hHandle,_T("Failed to open ftp file (%s)"),szFilename);
	}

	CFtpFile(HINTERNET hSession,LPCTSTR szURL,LPCTSTR szHeaders=NULL,DWORD dwHeadersLength=(DWORD)-1,DWORD dwFlags=INTERNET_FLAG_PASSIVE) throw(...)
	{
		ATLASSERT(hSession);
		if (!(m_hHandle=InternetOpenUrl(hSession,szURL,szHeaders,dwHeadersLength,dwFlags,(DWORD_PTR)(CInternetHandle*)this)))
			throw CInternetException(m_hHandle,_T("Failed to open ftp url (%s)"),szURL);
	}

	ULONGLONG GetSize() const
	{
		ATLASSERT(m_hHandle);
		ULARGE_INTEGER Size;
		Size.LowPart=FtpGetFileSize(m_hHandle,&Size.HighPart);
		return Size.QuadPart;
	}
};

template<class T>
class CInternetFindFile : public CInternetHandle
{
public:
	CInternetFindFile()
	{
		ZeroMemory(&m_fd,sizeof(T));
	}

	BOOL FindNext() throw(...)
	{
		ATLASSERT(m_hHandle);
		if (InternetFindNextFile(m_hHandle,&m_fd))
			return TRUE;
		if (GetLastError()==ERROR_NO_MORE_FILES)
			return FALSE;
		throw CInternetException(m_hHandle,_T("Failed to find next file"));
	}

protected:
	T m_fd;
};

class CFtpFindFile : public CInternetFindFile<WIN32_FIND_DATA>
{
public:
	ULONGLONG GetFileSize() const
	{
		ATLASSERT(m_hHandle);
		ULARGE_INTEGER nSize;
		nSize.LowPart=m_fd.nFileSizeLow;
		nSize.HighPart=m_fd.nFileSizeHigh;
		return nSize.QuadPart;
	}

	BOOL GetFileName(LPTSTR lpstrFileName, int cchLength) const
	{
		ATLASSERT(m_hHandle);
		if(lstrlen(m_fd.cFileName)>=cchLength)
			return FALSE;
		return (lstrcpy(lpstrFileName, m_fd.cFileName)!=NULL);
	}

	BOOL GetFileTitle(LPTSTR lpstrFileTitle, int cchLength) const
	{
		ATLASSERT(m_hHandle);
		TCHAR szBuff[MAX_PATH]={0};
		if(!GetFileName(szBuff,MAX_PATH))
			return FALSE;
		TCHAR szNameBuff[_MAX_FNAME]={0};
#ifdef _SECURE_ATL
		_tsplitpath_s(szBuff, NULL, 0, NULL, 0, szNameBuff, _MAX_FNAME, NULL, 0);
#else
		_tsplitpath(szBuff, NULL, NULL, szNameBuff, NULL);
#endif
		if(lstrlen(szNameBuff)>=cchLength)
			return FALSE;
		return (lstrcpy(lpstrFileTitle, szNameBuff)!=NULL);
	}

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	_CSTRING_NS::CString GetFileName() const
	{
		ATLASSERT(m_hHandle);
		_CSTRING_NS::CString ret=m_fd.cFileName;
		return ret;
	}

	_CSTRING_NS::CString GetFileTitle() const
	{
		ATLASSERT(m_hHandle);
		_CSTRING_NS::CString strFullName = GetFileName();
		_CSTRING_NS::CString strResult;
#ifdef _SECURE_ATL
		_tsplitpath_s(strFullName, NULL, 0, NULL, 0, strResult.GetBuffer(MAX_PATH), MAX_PATH, NULL, 0);
#else
		_tsplitpath(strFullName, NULL, NULL, strResult.GetBuffer(MAX_PATH), NULL);
#endif
		strResult.ReleaseBuffer();
		return strResult;
	}
#endif

	void GetLastWriteTime(FILETIME* pTimeStamp) const
	{
		ATLASSERT(m_hHandle);
		ATLASSERT(pTimeStamp);
		*pTimeStamp = m_fd.ftLastWriteTime;
	}

	void GetLastAccessTime(FILETIME* pTimeStamp) const
	{
		ATLASSERT(m_hHandle);
		ATLASSERT(pTimeStamp);
		*pTimeStamp = m_fd.ftLastAccessTime;
	}
    
	BOOL GetCreationTime(FILETIME* pTimeStamp) const
	{
		ATLASSERT(m_hHandle);
		ATLASSERT(pTimeStamp);
		*pTimeStamp = m_fd.ftCreationTime;
	}

	BOOL MatchesMask(DWORD dwMask) const
	{
		ATLASSERT(m_hHandle);
		return (m_fd.dwFileAttributes & dwMask);
	}

	BOOL IsDots() const
	{
		ATLASSERT(m_hHandle);
		return (IsDirectory() &&	(m_fd.cFileName[0]=='.' && (m_fd.cFileName[1]=='\0' || (m_fd.cFileName[1]=='.' && m_fd.cFileName[2]=='\0'))));
	}

	BOOL IsReadOnly() const
	{
		return MatchesMask(FILE_ATTRIBUTE_READONLY);
	}

	BOOL IsDirectory() const
	{
		return MatchesMask(FILE_ATTRIBUTE_DIRECTORY);
	}

	BOOL IsCompressed() const
	{
		return MatchesMask(FILE_ATTRIBUTE_COMPRESSED);
	}

	BOOL IsSystem() const
	{
		return MatchesMask(FILE_ATTRIBUTE_SYSTEM);
	}

	BOOL IsHidden() const
	{
		return MatchesMask(FILE_ATTRIBUTE_HIDDEN);
	}

	BOOL IsTemporary() const
	{
		return MatchesMask(FILE_ATTRIBUTE_TEMPORARY);
	}

	BOOL IsNormal() const
	{
		return MatchesMask(FILE_ATTRIBUTE_NORMAL);
	}

	BOOL IsArchived() const
	{
		return MatchesMask(FILE_ATTRIBUTE_ARCHIVE);
	}

protected:
	friend class CFtpConnection;
};

inline void CFtpConnection::FindFirst(CFtpFindFile& FF,LPCTSTR szSearch,DWORD dwFlags) throw(...)
{
	ATLASSERT(m_hHandle);
	ATLASSERT(FF.m_hHandle==NULL);
	if (!(FF.m_hHandle=FtpFindFirstFile(m_hHandle,szSearch,&FF.m_fd,dwFlags,(DWORD_PTR)(CInternetHandle*)this)))
		throw CInternetException(m_hHandle,_T("Failed to find files on ftp (%s)"),szSearch);
}

inline CInternetFile::CInfo::CInfo(CHttpFile& File) throw(...)
{
	DWORD dwLength;
	File.QueryInfo(HTTP_QUERY_CONTENT_LENGTH,dwLength);
	Reset(dwLength);
}

inline CInternetFile::CInfo::CInfo(CFtpFile& File) throw(...)
{
	Reset((DWORD)File.GetSize());
}
