#pragma once

#include "SocketComm.h"
class CConnectionManager;
class CXRefreshBHO;

#define CONNECTION_BUFFER_SIZE					(1024*256) // 256kb

///////////////////////////////////////////////////////////////////////////////

class CConnection : public CSocketComm {
public:
	CConnection(CConnectionManager* parent);
	virtual ~CConnection();

public:

	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent);

protected:
	CConnectionManager*							m_Parent;
	char										m_Buffer[CONNECTION_BUFFER_SIZE];
	int											m_BufferPos;
};

///////////////////////////////////////////////////////////////////////////////

class CReconnectListener : public CSocketComm {
public:
	CReconnectListener(CConnectionManager* parent);
	virtual ~CReconnectListener();

public:
	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent);

protected:
	CConnectionManager*							m_Parent;
};

///////////////////////////////////////////////////////////////////////////////

class CConnectionManager {
public:
	friend CConnection;
	CConnectionManager(CXRefreshBHO* parent);
	~CConnectionManager();

	void										Connect();
	void										Disconnect();
	bool										IsConnected();

	void										ProcessMessage(Json::Value& msg);

	void										SendHello();
	void										SendBye();
	void										SendSetPage(CString title, CString url);

	void										PerformRefresh();
	void										Reconnect();

	void										Log(CString message, int icon);

	void										StartReconnectListener();
	void										StopReconnectListener();

	void										RequestStartingReconnectListener();
	void										RequestSendInfoAboutPage();
	void										UpdateIcon();

protected:
	CString										FileSentence(int n, CString s);
	CString										GetStory(Json::Value& msg);
	void										Send(Json::Value& msg);

	CXRefreshBHO*								m_Parent;
	CConnection									m_Connection;
	CReconnectListener							m_ReconnectListener;
	bool										m_Connected;
	CString										m_Agent;
	CString										m_Version;
};
