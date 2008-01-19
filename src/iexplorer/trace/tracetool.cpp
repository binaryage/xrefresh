// tracetool.h
//
// Author : Thierry Parent
// Version : 8.0.0
//
// HomePage :  http://www.codeproject.com/csharp/TraceTool.asp
// Download :  http://sourceforge.net/projects/tracetool/
// See License.txt for license information 
#include "stdafx.h"   // remove precompiled header

#ifdef _DEBUG

#include <windows.h>
#include <stdio.h>
#include <winsock.h>
#include <tchar.h>
#include <assert.h>

#ifndef _WIN32_WCE
#include <process.h>
#else
#include <Kfuncs.h>
#endif

#include "tracetool.h"
#pragma warning(disable: 4267 4244) 
// static TTrace initialization

struct sockaddr_in TTrace::m_serverSockAddr ;              // Socket adress
CRITICAL_SECTION   TTrace::criticalSection ;
TraceOptions *     TTrace::m_options = new TraceOptions();
WinTrace *         TTrace::m_winTrace = new WinTrace() ;
WinWatch *         TTrace::m_winWatch = new WinWatch() ;

WORD               TTrace::m_DeviceId1 = 0;
WORD               TTrace::m_DeviceId2 = 0;
WORD               TTrace::m_DeviceId3 = 0;
bool               TTrace::m_IsSocketInitialized = false ;
int                TTrace::m_socketHandle = -1; 
DWORD              TTrace::m_ClockSequenceBase = TTrace::Init();  // Initialize the Tracetool system

//==========================================================================

// TTRACE  

/// <summary>
// Initialize the Tracetool system
/// </summary>

DWORD TTrace::Init (void)
{
   InitializeCriticalSection(&criticalSection);
   // The spatially unique node identifier. Because we are not sure there is a network card ,
   // we generate random number instead of unique network card id.
   m_DeviceId1 = LOWORD(GetTickCount()*rand());
   m_DeviceId2 = LOWORD(rand());
   m_DeviceId3 = LOWORD(rand());

   // sequence number
   m_ClockSequenceBase = rand();
   return m_ClockSequenceBase ;
}

//-------------------------------------------------------------------------

void TTrace::Stop (void)
{
   CloseSocket() ;

   if (m_options != NULL)
      delete m_options ;
   m_options = NULL ;


   if (m_winTrace != NULL)
      delete m_winTrace ;

   if (m_winWatch != NULL)
      delete m_winWatch ;

   m_winTrace = NULL ;
}

//-------------------------------------------------------------------------

/// <summary>
// Close viewer connection.
// Replace TTrace destructor
/// </summary>

void TTrace::CloseSocket() 
{
   if (m_IsSocketInitialized == true)
   {
      // close connection 
      shutdown(m_socketHandle,2);   // SD_BOTH
      closesocket(m_socketHandle);

      WSACleanup();
      m_IsSocketInitialized = false ;
   }
   //DeleteCriticalSection(&criticalSection) ;
}

//-------------------------------------------------------------------------

/// <summary>
// Helper function : convert a widestring to a C string.
// The caller is responsive to destroy the new string
/// </summary>

char * TTrace::WideToMbs (const wchar_t * WideStr, int indent) 
{
   if (WideStr == NULL) return NULL;

	TCHAR buf[4096];
	wcsncpy_s(buf+indent, 4096-indent, WideStr, _TRUNCATE);
	for (int i=0; i<indent; i++) buf[i] = ' ';
	TCHAR* p = buf+indent;
	while (*p)
	{
		if (*p=='\n' || *p=='\r' || *p=='\t') *p = ' ';
		++p;
	}
	int lenStr = 2*(wcslen(buf) + 1);
	char * result = (char *) malloc (lenStr) ;
	::WideCharToMultiByte(CP_ACP, 0, buf, -1, result, (int)lenStr, 0, 0);
   return result;
}

//-------------------------------------------------------------------------


/// <summary>
// Create an unique ID. 
// For windows CE compatibility reason, we cannot ask microsoft API to return an unique ID
// Since this function uses random generator instead of computer id it cannot be
// garanted that generated GUID is unique in the world.
/// </summary>
char * TTrace::CreateTraceID()   // static
{
   unsigned long  Data1;
   unsigned short Data2;
   unsigned short Data3;
   WORD Sequence ;
   char * bufferId ;

   SYSTEMTIME systemTime;
   FILETIME fileTime;

   GetSystemTime(&systemTime);

   BOOL bResult = SystemTimeToFileTime(&systemTime, &fileTime);
   if (!bResult) 
      return NULL;

   //0-3    The low field of the timestamp.
   Data1 = fileTime.dwLowDateTime; 

   //4-5    The middle field of the timestamp.
   Data2 = LOWORD(fileTime.dwHighDateTime);

   //6-7    The high field of the timestamp multiplexed 
   //       with the version number.
   //       Version number is 0x0002
   Data3 = (HIWORD(fileTime.dwHighDateTime) & 0xFFF0) | 0x0002;

   //8-9    Here we store sequence number
   m_ClockSequenceBase++;

   Sequence = LOWORD(m_ClockSequenceBase);

   //10-15  The spatially unique node identifier.
   //       Because there is no network card we generate random number
   //       instead of unique network card id.

   bufferId = (char*)malloc(32+1);

   #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
   sprintf_s (bufferId, 33,"%08x%04x%04x%04x%04x%04x%04x", Data1, Data2, Data3, Sequence,m_DeviceId1,m_DeviceId2,m_DeviceId3) ;
   #else
   sprintf (bufferId, "%08x%04x%04x%04x%04x%04x%04x", Data1, Data2, Data3, Sequence,m_DeviceId1,m_DeviceId2,m_DeviceId3) ;
   #endif
   return bufferId;
}

//-------------------------------------------------------------------------

HWND StartDebugWin ()
{

#ifndef _WIN32_WCE

   // first check if already running
   HWND hWndDBG;
   hWndDBG = FindWindow(TEXT("TFormReceiver"), TEXT("FormReceiver"));
   if (hWndDBG!=NULL) 
      return hWndDBG ;

   // get the path in the registry
   HKEY hKey; 
   TCHAR Buffer[MAX_PATH]; // char
   DWORD dwBufferSize = MAX_PATH; 
   RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
      _T("SOFTWARE\\TraceTool"), 
      0, KEY_READ, &hKey); 
   RegQueryValueEx(  hKey, _T("FilePath"), NULL, NULL, (LPBYTE) Buffer, &dwBufferSize); 
   RegCloseKey(hKey); 

   // run the tracetool process
   STARTUPINFO si; 
   PROCESS_INFORMATION pi; 

   ZeroMemory( &si, sizeof(si) ); 
   si.cb = sizeof(si); 
   si.dwFlags = STARTF_USESHOWWINDOW ;
   si.wShowWindow = SW_NORMAL ;
   ZeroMemory( &pi, sizeof(pi) ); 

   if (CreateProcess(Buffer, NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pi) == false)
      return NULL ;

   // wait for the proces
   WaitForInputIdle(pi.hProcess, 3 * 1000); // wait for 3 seconds to get idle
   CloseHandle(pi.hThread);
   CloseHandle(pi.hProcess);

   // check if the window is created now
   hWndDBG = FindWindow(TEXT("TFormReceiver"), TEXT("FormReceiver"));
   if (hWndDBG!=NULL) 
      return hWndDBG ;

#endif
   // stil not found ? Run traceTool first to register it
   return NULL ;                                      
}

//-------------------------------------------------------------------------
/// <summary>
/// send the trace to the viewer
/// </summary>
/// <param name="Commands">The list of command to send</param>
/// <param name="winWatch">window watch</param>
void TTrace::SendToClient(CommandList * Commands, const WinWatch * winWatch) 
{
   Commands->AddFront(CST_WINWATCH_ID, winWatch->id.c_str());
   SendToClient(Commands) ;
}

//-------------------------------------------------------------------------

/// <summary>
/// send the trace to the viewer
/// </summary>/// <param name="Commands">The list of command to send</param>
/// <param name="winTrace">window trace</param>
void TTrace::SendToClient(CommandList * Commands, const WinTrace * winTrace)   
{
   // CST_USE_TREE MUST be inserted at the first position
   if (winTrace != NULL && winTrace->id != "") 
      Commands->AddFront (CST_USE_TREE ,winTrace->id.c_str());
   SendToClient(Commands) ;
}

//-------------------------------------------------------------------------

/// <summary>
/// send the trace to the viewer
/// </summary>/// <param name="Commands">The list of command to send</param>
void TTrace::SendToClient(CommandList * Commands)   
{
   // TODO : place the Command list into a stack and let a thread send it !!!!!!!!!!!

   // Save the last error. TraceTool must be neutral about the error
   DWORD err = GetLastError() ;

   // compute line size and create buffer to send
   size_t j ; // int j;
   char *line;

   j=0;
   CommandList::const_iterator c1_elem ;
   CommandList::const_iterator c1_end ;

   c1_end  = Commands->end () ;
   for (c1_elem = Commands->begin (); c1_elem < c1_end ; c1_elem++) 
      j += (* c1_elem).length() +1 ;

   line=(char*)malloc(j+1);

   // create a single buffer line for all commands
   j=0;
   for (c1_elem = Commands->begin (); c1_elem < c1_end ; c1_elem++) 
   {
      string str = (* c1_elem) ;
      size_t strLen = str.length()+1 ;
      memcpy(line+j,str.c_str(),strLen);
      j += strLen ;
   }
   Commands->clear() ;
   line[j]=0;

   if (m_options->sendMode == Socket)
   {
      // ensure only one thread send strings to the viewer using socket.
      EnterCriticalSection (&criticalSection) ; 

      int err;
      WSADATA wsaData;

      if (m_IsSocketInitialized == false)
      {
         err = WSAStartup( MAKEWORD(1,1), &wsaData );
         if ( err != 0 ) 
         {
            LeaveCriticalSection (&criticalSection) ; 
            return;
         }

         memset(&m_serverSockAddr,0,sizeof(m_serverSockAddr));
         // Try to convert the string as an IP address (e.g., "192.168.55.100") 
         m_serverSockAddr.sin_addr.s_addr = inet_addr(m_options->socketHost);  // host to network port 

         // If not in IP format, get the address via DSN... 
         if (m_serverSockAddr.sin_addr.s_addr == INADDR_NONE) 
         { 
             hostent* lphost;  

             // request the host address
             lphost = gethostbyname(m_options->socketHost); // lphost is allocated by Windows Sockets

             if (lphost != NULL) {
                 m_serverSockAddr.sin_addr.S_un.S_addr = ((LPIN_ADDR)lphost->h_addr_list[0])->s_addr; 
                 //wsprintf (Host_adr, L"%s (%d.%d.%d.%d)", Host,
                 //    serverSockAddr.sin_addr.S_un.S_un_b.s_b1,
                 //    serverSockAddr.sin_addr.S_un.S_un_b.s_b2,
                 //    serverSockAddr.sin_addr.S_un.S_un_b.s_b3,
                 //    serverSockAddr.sin_addr.S_un.S_un_b.s_b4) ;

             } else {         // else name was invalid (or couldn't be resolved)
                 LeaveCriticalSection (&criticalSection) ; 
                 return; 
             }
         }


         m_serverSockAddr.sin_port = htons(m_options->socketPort);             // port to network port  
         m_serverSockAddr.sin_family = AF_INET;                                // AF_*** : INET=internet 

         // Socket creation 
         if ( (m_socketHandle = socket(AF_INET,SOCK_STREAM,0)) < 0) 
         {
            //MessageBox(NULL,_T("Couldn't create socket"),_T("Socket Error"),MB_OK);		
            LeaveCriticalSection (&criticalSection) ; 
            return;
         }

			int result;
			int buffsize = 0; // don't buffer sockets
			result = setsockopt(m_socketHandle, SOL_SOCKET, SO_SNDBUF, (const char*)&buffsize, sizeof(buffsize));

         // Open connection 
         if(connect(m_socketHandle,(struct sockaddr *)&m_serverSockAddr,	sizeof(m_serverSockAddr))<0)	{
            //MessageBox(NULL,_T("Couldn't connect"),_T("Socket Error"),MB_OK);		
            LeaveCriticalSection (&criticalSection) ; 
            return;
         }
         m_IsSocketInitialized = true ;
      }
      // send the line lenght
      char buffer [100] ;

      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
      sprintf_s(buffer,100,"%d",j);
      #else
      sprintf(buffer,"%d",j);
      #endif

		//OutputDebugString(CA2T(buffer)); // debug
      send(m_socketHandle,buffer,strlen(buffer)+1,0);	

      // send the buffer 
      int i = send(m_socketHandle,line,j,0);

      LeaveCriticalSection (&criticalSection) ; 
      //
      //TCHAR message[256];
      //_stprintf(message,TEXT("Done: %d bytes sent (%d normally)"),i,j);
      //MessageBox(NULL,message,_T("Socket Error"),MB_OK);		

   } else {

      HWND hWndDBG;
      hWndDBG = StartDebugWin() ; // FindWindow(TEXT("TFormReceiver"), TEXT("FormReceiver"));

      if (hWndDBG!=NULL) {
         COPYDATASTRUCT CDS;

         CDS.cbData = j+1;
         CDS.dwData = WMD_TRACETOOL; // identification code 'traceTool'
         CDS.lpData = line; // no need to add #0, because String are null terminated
         SendMessage(hWndDBG, WM_COPYDATA, 0, (LPARAM)(&CDS));  //WParam(Application.Handle)
      }
   }
   free(line);
   SetLastError (err) ;  // preserve last error   
}



//-------------------------------------------------------------------------

/// <summary>
/// Clear all traces (main window)
/// </summary>

void TTrace::ClearAll ()
{
   m_winTrace->ClearAll () ;
}

//------------------------------------------------------------------------------

/// <summary>
///  Show or hide the trace program
/// </summary>

void TTrace::Show (bool IsVisible)
{
   CommandList Commands  ;
   if (IsVisible == true)
      Commands.Add (CST_SHOW , 1);
   else
      Commands.Add (CST_SHOW , 0);

   TTrace::SendToClient (&Commands);
}

//------------------------------------------------------------------------------

// Sub System
// TODO : static void Flush (FlushTimeOut : integer = 5000);
// TODO : static void stop ; // Stop tracetool sub system. Must be called before exiting plugin
// TODO : static void start ; // restart tracetool sub system if STOP was called

//==========================================================================

/* TMemberNode   */

/// <summary>
/// Create a Member node (or a sub member)
/// </summary>
/// <param name="strCol1">optional column 1</param>
/// <param name="strCol2">optional column 2</param>
/// <param name="strCol3">optional column 3</param>

TMemberNode::TMemberNode(const char * strCol1 /* = NULL */, const char * strCol2 /* = NULL */, const char * strCol3 /* = NULL */)
{
   if (strCol1 != NULL)
      col1 = strCol1;

   if (strCol2 != NULL)
      col2 = strCol2;

   if (strCol3 != NULL)
      col3 = strCol3;

   // create sub members only if needed
   m_Members = NULL ; // new deque <TMemberNode *> ;
   m_FontDetails = NULL ; 
}

//-------------------------------------------------------------------------

/// <summary>
/// Create a Member node (or a sub member)
/// </summary>
/// <param name="strCol1">optional column 1</param>
/// <param name="strCol2">optional column 2</param>
/// <param name="strCol3">optional column 3</param>

TMemberNode::TMemberNode(const wchar_t * strCol1 , const wchar_t * strCol2 /* = NULL */, const wchar_t * strCol3 /* = NULL */)
{
   if (strCol1 != NULL)
   {
      char * col = TTrace::WideToMbs(strCol1) ;
      col1 = col ; // copy
      free (col) ;
   }

   if (strCol2 != NULL) 
   {
      char * col = TTrace::WideToMbs(strCol2) ;
      col2 = col ; // copy
      free (col) ;
   }

   if (strCol3 != NULL)
   {
      char * col = TTrace::WideToMbs(strCol3) ;
      col3 = col ; // copy
      free (col) ;
   }

   // create sub members only if needed
   m_Members = NULL ; // new deque <TMemberNode *> ;
   m_FontDetails = NULL ; 
}

//-------------------------------------------------------------------------

/// <summary>
// When a TraceNode is send, the "Members" field is converted to commands.
// The convertion automatically delete all sub members and the "Members" field.
// Destructor is then needed only if you don't send the node.
/// </summary>

TMemberNode::~TMemberNode() 
{
   if (m_Members != NULL)
   {
      deque <TMemberNode *>::const_iterator MemberBegin;
      deque <TMemberNode *>::const_iterator MemberEnd;

      MemberEnd  = m_Members->end () ;
      for (MemberBegin = m_Members->begin (); MemberBegin < MemberEnd ; MemberBegin++) 
      {
         TMemberNode * member = * MemberBegin ;
         delete member ;
      }
      delete m_Members ;
      m_Members = NULL ;
   }

   if (m_FontDetails != NULL)
   {
      deque <FontDetail *>::const_iterator FontDetailBegin;
      deque <FontDetail *>::const_iterator FontDetailEnd;

      FontDetailEnd  = m_FontDetails->end () ;
      for (FontDetailBegin = m_FontDetails->begin (); FontDetailBegin < FontDetailEnd ; FontDetailBegin++) 
      {
         FontDetail * fontDetail = * FontDetailBegin ;
         delete fontDetail ;
      }
      delete m_FontDetails ;
      m_FontDetails = NULL ;
   }

}

//-------------------------------------------------------------------------

/// <summary>
// create sub members only if needed
/// </summary>

deque <TMemberNode *> * TMemberNode::Members() 
{
   if (m_Members == NULL)
      m_Members = new deque <TMemberNode *> ;
   return m_Members ;
}

//-------------------------------------------------------------------------

/// <summary>
/// Recursively add members to the node commandList
/// <summary>
/// <param name="commands">Where to store members </param>

void TMemberNode::AddToStringList (CommandList * commands) 
{
   if (m_Members == NULL)
      return ;

   // the root node node itself is not send 
   deque <TMemberNode *>::const_iterator MemberBegin;
   deque <TMemberNode *>::const_iterator MemberEnd;

   MemberEnd  = m_Members->end () ;
   for (MemberBegin = m_Members->begin (); MemberBegin < MemberEnd ; MemberBegin++) 
   {
      TMemberNode * member = * MemberBegin ;
      member->_AddToStringList(commands);
      delete member ;  // delete sub members
   }
   delete m_Members ;  // delete member array
   m_Members = NULL ;
}

//-------------------------------------------------------------------------

/// internal recursive 
void TMemberNode::_AddToStringList (CommandList * commands) 
{

   commands->Add (CST_CREATE_MEMBER, col1.c_str()) ;     // first column can be NULL
   if (! col2.empty())
      commands->Add (CST_MEMBER_COL2, col2.c_str()) ;
   if (! col3.empty())
      commands->Add (CST_MEMBER_COL3, col3.c_str()) ;


   if (m_FontDetails != NULL)
   {
      deque <FontDetail *>::const_iterator FontBegin;
      deque <FontDetail *>::const_iterator FontEnd;
      FontEnd  = m_FontDetails->end () ;
      for (FontBegin = m_FontDetails->begin (); FontBegin < FontEnd ; FontBegin++) 
      {
         FontDetail * font = * FontBegin ;

         char * message ;
         int bold   = (font->Bold  ) ? 1 : 0 ;
         int italic = (font->Italic) ? 1 : 0;
         int MsgLen ;

         if (font->FontName.empty())
         {
            message = (char*)malloc(32+1) ;
            #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
            sprintf_s(message, 33,"%5d%3d%1d%1d%11d%11d", CST_MEMBER_FONT_DETAIL,font->ColId,bold,italic,font->Color,font->Size);
            #else
            sprintf(message, "%5d%3d%1d%1d%11d%11d", CST_MEMBER_FONT_DETAIL,font->ColId,bold,italic,font->Color,font->Size);
            #endif
         } else {
            MsgLen = font->FontName.length()+32+1 ;
            message = (char*)malloc(MsgLen) ;
            const char * fontName = font->FontName.c_str() ;
            #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
            sprintf_s(message, MsgLen,"%5d%3d%1d%1d%11d%11d%s", CST_MEMBER_FONT_DETAIL,font->ColId,bold,italic,font->Color,font->Size,fontName);
            #else
            sprintf(message, "%5d%3d%1d%1d%11d%11d%s", CST_MEMBER_FONT_DETAIL,font->ColId,bold,italic,font->Color,font->Size,fontName);
            #endif
         }
         commands->push_back (message) ;
         free (message) ;         
         delete font ;
      }
      delete m_FontDetails ;
      m_FontDetails = NULL ;
   }

   if (m_Members != NULL)
   {
      deque <TMemberNode *>::const_iterator MemberBegin;
      deque <TMemberNode *>::const_iterator MemberEnd;

      MemberEnd  = m_Members->end () ;
      for (MemberBegin = m_Members->begin (); MemberBegin < MemberEnd ; MemberBegin++) 
      {
         TMemberNode * member = * MemberBegin ;
         member->_AddToStringList(commands);
         delete member ;
      }
      delete m_Members ;
      m_Members = NULL ;
   }
   commands->Add (CST_ADD_MEMBER) ;     // close the member group
}

//-------------------------------------------------------------------------

/// <summary>
/// Add a sub member. NOTE : the member in argument will be destroyed when the container node is destroyed
/// </summary>
/// <param name="member">an already constructed sub member</param>
TMemberNode * TMemberNode::Add (TMemberNode * member) 
{
   Members()->push_back(member);
   return member;
}

//-------------------------------------------------------------------------

/// <summary>
/// create and add a sub member
/// </summary>
/// <param name="strCol1">Column 1 of the new sub member</param>
/// <param name="strCol1">Optional column 2 of the new sub member</param>
/// <param name="strCol1">Optional column 3 of the new sub member</param>
TMemberNode * TMemberNode::Add (const char * strCol1 , const char * strCol2 /* = NULL */, const char * strCol3 /* = NULL */)
{
   TMemberNode * member;
   member = new TMemberNode (strCol1, strCol2, strCol3);
   Add(member);
   return member; 
}

//-------------------------------------------------------------------------

/// <summary>
/// Set member font
/// </summary>
/// <param name="ColId">Column number (0..2)</param>
/// <param name="Bold">Change font to bold</param>
/// <param name="Italic">Change font to Italic</param>
/// <param name="Color">Change Color. Use -1 to keep default color</param>
/// <param name="Size">Change font size, use zero to keep normal size</param>
/// <param name="FontName">Change font name</param>
/// <returns>The TMemberNode </returns>
TMemberNode * TMemberNode::SetFontDetail(const int ColId,  const bool Bold ,  const bool Italic /*= false*/ ,  const int Color /*= -1*/ , const int Size /*= 0*/ ,  const char * FontName /*= NULL*/) 
{
   if (m_FontDetails == NULL)
      m_FontDetails = new deque <FontDetail *> ;
   FontDetail * font = new FontDetail() ;

   font->ColId    = ColId ;
   font->Bold     = Bold ;
   font->Italic   = Italic ;
   font->Color    = Color ;
   font->Size     = Size ;
   if (FontName != NULL)
      font->FontName = FontName ;
   m_FontDetails->push_back(font);
   return this ;
}

//-------------------------------------------------------------------------

/// <summary>
/// create and add a sub member
/// </summary>
/// <param name="strCol1">Column 1 of the new sub member</param>
/// <param name="strCol1">Optional column 2 of the new sub member</param>
/// <param name="strCol1">Optional column 3 of the new sub member</param>
TMemberNode * TMemberNode::Add (const wchar_t * strCol1 , const wchar_t * strCol2 /* = NULL */, const wchar_t * strCol3 /* = NULL */)
{
   TMemberNode * member;
   member = new TMemberNode (strCol1, strCol2, strCol3);
   Add(member);
   return member; 
}

//==========================================================================

/* TraceOptions */

/// <summary>
/// TraceTool Options constructor
/// </summary>

TraceOptions::TraceOptions (void)
{
   SendProcessName = false ;
   m_processFileName = NULL ;
   socketHost = NULL ;
   socketPort = 8090 ;
#ifdef _WIN32_WCE
   // for pocket pc : default is socket and host on 192.168.55.100
   sendMode = Socket ;                   
   SetSocketHost("192.168.55.100"); 
#else
   // For windows desktop : default is windows messages. In case of you switch to socket, the default host is localhost
   sendMode = WinMsg ;                   
	//sendMode = Socket;                   
   SetSocketHost("127.0.0.1"); 
#endif 
}

//-------------------------------------------------------------------------

/// <summary>
/// TraceTool Options destructor
/// </summary>

TraceOptions::~TraceOptions(void) 
{
   if (socketHost != NULL)
      free (socketHost) ;
   if (m_processFileName != NULL)
      free (m_processFileName) ;
}

//-------------------------------------------------------------------------

/// <summary>
/// Set the socket host
/// </summary>
/// <param name="Host">socket host</param>

void TraceOptions::SetSocketHost (const char * Host) 
{
   if (socketHost != NULL)
      free (socketHost) ;
   socketHost = NULL ;
   if (Host == NULL)
      return ;
   int lenhost = strlen(Host) + 1 ;
   socketHost = (char *) malloc (lenhost) ;
   #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
   strcpy_s (socketHost, lenhost, Host) ;
   #else
   strcpy (socketHost, Host) ;
   #endif
}

//-------------------------------------------------------------------------

void TraceOptions::SetSocketHost (const wchar_t * Host) 
{
   if (socketHost != NULL)
      free (socketHost) ;
   socketHost = TTrace::WideToMbs(Host) ;
}

//-------------------------------------------------------------------------

/// <summary>
/// Helper function : return the process name without path
/// </summary>

const char * TraceOptions::CheckProcessName (void) 
{
   if (m_processFileName == NULL)
   {
      WCHAR wFileName [MAX_PATH+1] ;      
      size_t nbChar ;

      // use wide GetModuleFileNameW in place of GetModuleFileNameA for winCE compatibility
      wFileName[GetModuleFileNameW (0 /* hInstance */ ,wFileName,MAX_PATH)] = 0; 

      // Convert widestring (wcs) to multibyte string (mbs)
      char *pmbFilename = (char *)malloc( MAX_PATH+1 );
      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
      //size_t i ;
      //nbChar = 
      wcstombs_s(&nbChar, pmbFilename, MAX_PATH+1, wFileName, MAX_PATH );       
      #else
      nbChar = wcstombs( pmbFilename, wFileName, MAX_PATH );       
      #endif

      // bypass any path before the module name
      const char * ptr ;
      ptr = pmbFilename + nbChar -1;
      while (ptr > pmbFilename)
      {
         if ((*ptr == '/') || 
            (*ptr == '\\') || 
            (*ptr == ':'))
         {
            ptr ++ ;
            break ;
         } else {
            ptr-- ;
         }
      }

      if ((*ptr == '/') || 
         (*ptr == '\\') || 
         (*ptr == ':'))
         ptr ++ ;

      int fileLength = strlen(ptr) + 1 ;
      m_processFileName = (char *) malloc (fileLength) ;
      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)   // visual studio 2005 : deprecated function
      strcpy_s (m_processFileName, fileLength, ptr) ;
      #else
      strcpy (m_processFileName, ptr) ;
      #endif
      free (pmbFilename) ;
   }
   return m_processFileName ;
}

//==========================================================================

/// protected constructor
WinWatch::WinWatch(void) 
{
   id = "" ; 	                                                      // Wintrace id (empty for the main window)
   enabled = true ;                                                  // enable or disable watches
}

//-------------------------------------------------------------------------

WinWatch::WinWatch (const char * WinWatchID , const char * WinWatchText) 
{
   id = "" ; 	                                                      // Wintrace id (empty for the main window)
   enabled = true ;                                                   // enable or disable watches

   if (WinWatchID == NULL || WinWatchID == "")  
      id = TTrace::CreateTraceID() ;
   else
      id = WinWatchID ;

   CommandList Commands  ;

   if (WinWatchText == NULL || WinWatchText == "") 
      Commands.Add(CST_WINWATCH_NAME, id.c_str()) ; 
   else
      Commands.Add(CST_WINWATCH_NAME, WinWatchText) ;
 
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

WinWatch::WinWatch (const wchar_t * WinWatchID , const wchar_t * WinWatchText) 
{
   id = "" ; 	                                                      // Wintrace id (empty for the main window)
   enabled = true ;                                                   // enable or disable watches

   char * strWinWatchID   = TTrace::WideToMbs(WinWatchID) ;
   char * strWinWatchText = TTrace::WideToMbs(WinWatchText) ;

   if (strWinWatchID == NULL || strWinWatchID == "")  
      id = TTrace::CreateTraceID() ;
   else {
      id = strWinWatchID ;  // copy
   }
   CommandList Commands  ;

   if (strWinWatchText == NULL || strWinWatchText == "") 
      Commands.Add(CST_WINWATCH_NAME, id.c_str()) ; 
   else
      Commands.Add(CST_WINWATCH_NAME, strWinWatchText) ;

   if (strWinWatchID != NULL)
      free (strWinWatchID) ;

   if (strWinWatchText != NULL)
      free (strWinWatchText) ;

   TTrace::SendToClient (& Commands,this);
}
//-------------------------------------------------------------------------

void WinWatch::DisplayWin (void) 
{
   CommandList Commands  ;
   Commands.Add(CST_DISPLAY_TREE) ;
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

void WinWatch::ClearAll (void) 
{
   CommandList Commands  ;
   Commands.Add(CST_CLEAR_ALL);
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

void WinWatch::Send (const char * WatchName , const char * WatchValue) 
{
   if (! enabled) 
      return ;

   CommandList Commands  ;
   Commands.Add(CST_WATCH_NAME, WatchName) ;

   // create the member and set col1
   Commands.Add(CST_CREATE_MEMBER);
   // col2 is the value
   Commands.Add (CST_MEMBER_COL2, WatchValue);
   // close the member group
   Commands.Add(CST_ADD_MEMBER);

   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

void WinWatch::Send (const wchar_t * WatchName , const wchar_t * WatchValue) 
{
   if (! enabled) 
      return ;

   char * strWatchName  = TTrace::WideToMbs(WatchName) ;
   char * strWatchValue = TTrace::WideToMbs(WatchValue) ;

   CommandList Commands  ;
   Commands.Add(CST_WATCH_NAME, strWatchName) ;

   // create the member and set col1
   Commands.Add(CST_CREATE_MEMBER);
   // col2 is the value
   Commands.Add (CST_MEMBER_COL2, strWatchValue);
   // close the member group
   Commands.Add(CST_ADD_MEMBER);

   if (strWatchName != NULL)
      free (strWatchName) ;

   if (strWatchValue != NULL)
      free (strWatchValue) ;

   TTrace::SendToClient (& Commands,this);
}

//==========================================================================

/// <summary>
/// WinTrace constructor. <br>
/// you can map a WinTrace to an existing window. <br> 
/// Nothing Is send to the viewer
/// </summary>

WinTrace::WinTrace(void) 
{
   init() ;
}

//-------------------------------------------------------------------------

/// <summary>
/// WinTrace constructor. The Window Trace is create on the viewer (if not already done)
/// </summary>
/// <param name="WinTraceID">Required window trace Id. If empty, a guid will be generated</param>
/// <param name="WinTraceText">The Window Title on the viewer.If empty, a default name will be used</param>

//WinTrace::WinTrace(const char * WinTraceID , const char * WinTraceTitle)  
//{
//   init() ;
//   if (WinTraceID == NULL || WinTraceID == "") 
//      id = TTrace::CreateTraceID() ;
//   else
//      id = WinTraceID ; 
//
//   // create the trace window
//   CommandList Commands  ;
//
//   if (WinTraceTitle == NULL || WinTraceTitle == "")
//      Commands.Add (CST_TREE_NAME, id.c_str()); 
//   else
//      Commands.Add (CST_TREE_NAME, WinTraceTitle); 
//
//   TTrace::SendToClient (& Commands,this);
//}
//
//-------------------------------------------------------------------------

/// <summary>
/// WinTrace constructor. The Window Trace is create on the viewer (if not already done)
/// </summary>
/// <param name="WinTraceID">Required window trace Id. If empty, a guid will be generated</param>
/// <param name="WinTraceText">The Window Title on the viewer.If empty, a default name will be used</param>

WinTrace::WinTrace(const wchar_t * WinTraceID , const wchar_t * WinTraceTitle)  
{
   init() ;

	id2 = WinTraceID;

   char * strWinTraceID    = TTrace::WideToMbs(WinTraceID) ;
   char * strWinTraceTitle = TTrace::WideToMbs(WinTraceTitle) ;

   if (strWinTraceID == NULL || strWinTraceID == "") 
      id = TTrace::CreateTraceID() ;
   else
      id = strWinTraceID ; // copy

   // create the trace window
   CommandList Commands  ;

   if (strWinTraceTitle == NULL || strWinTraceTitle == "")
      Commands.Add (CST_TREE_NAME, id.c_str()); 
   else
      Commands.Add (CST_TREE_NAME, strWinTraceTitle); 

   if (strWinTraceID != NULL)
      free (strWinTraceID) ;

   if (strWinTraceTitle != NULL)
      free (strWinTraceTitle) ;

   TTrace::SendToClient (& Commands,this);
	Debug()->Send(_T("---"));
}

//-------------------------------------------------------------------------
/// private initialize (called by the 2 constructors)
void WinTrace::init() 
{
   debug   = new TraceNode(NULL, false);     // no parent node, don't create a GUID
   error   = new TraceNode(NULL, false);
   warning = new TraceNode(NULL, false);

   debug->iconIndex   = CST_ICO_INFO;        // store the iconIndex. don't add CST_ICO_INDEX command
   error->iconIndex   = CST_ICO_ERROR;
   warning->iconIndex = CST_ICO_WARNING;	

   debug->winTrace   = this ;                // link the 3 node to the window
   error->winTrace   = this ;
   warning->winTrace = this ;	
}

//-------------------------------------------------------------------------

/// <summary>
/// Destructor
/// </summary>
WinTrace::~WinTrace(void) 
{
   delete debug;
   delete error;
   delete warning;
}

//-------------------------------------------------------------------------

/// <summary>
/// Save the window tree traces to a text file
/// </summary>
/// <param name="FileName">file to save</param>
void WinTrace::SaveToTextfile (const char * FileName)
{
   CommandList Commands  ;
   Commands.Add(CST_SAVETOTEXT,FileName); 
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Save the window tree traces to a text file
/// </summary>
/// <param name="FileName">file to save</param>
void WinTrace::SaveToTextfile (const wchar_t * FileName)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(FileName) ;
   Commands.Add(CST_SAVETOTEXT,temp);
   if (temp != NULL)
      free (temp) ;

   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Save the window tree traces to an XML file
/// </summary>
/// <param name="FileName">file to save</param>
void WinTrace::SaveToXml (const char * FileName)
{
   CommandList Commands  ;
   Commands.Add (CST_SAVETOXML , FileName);
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Save the window tree traces to an XML file
/// </summary>
/// <param name="FileName">file to save</param>
void WinTrace::SaveToXml (const wchar_t * FileName)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(FileName) ;
   Commands.Add (CST_SAVETOXML , temp);
   if (temp != NULL)
      free (temp) ;
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Load an XML file to the window tree traces
/// </summary>
/// <param name="FileName">file to open</param>
void WinTrace::LoadXml (const char * FileName)
{
   CommandList Commands  ;
   Commands.Add (CST_LOADXML , FileName);
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Load an XML file to the window tree traces
/// </summary>
/// <param name="FileName">file to open</param>
void WinTrace::LoadXml (const wchar_t * FileName)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(FileName) ;
   Commands.Add (CST_LOADXML , temp);
   if (temp != NULL)
      free (temp) ;
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Set the log file.(Path is relative to the viewer)
/// </summary>
/// <param name="FileName">file to open</param>
/// <param name="Mode"> 0 : Log is disabled. 1 : no limit.	2, daily file (with CCYYMMDD)</param>
void WinTrace::SetLogFile (const char * FileName, const int Mode)
{
   CommandList Commands  ;
   Commands.Add (CST_LOGFILE, Mode , FileName);
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Set the log file.(Path is relative to the viewer)
/// </summary>
/// <param name="FileName">file to open</param>
/// <param name="Mode"> 0 : Log is disabled. 1 : no limit.	2, daily file (with CCYYMMDD)</param>
void WinTrace::SetLogFile (const wchar_t * FileName, const int Mode)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(FileName) ;
   Commands.Add (CST_LOGFILE, Mode , temp);
   if (temp != NULL)
      free (temp) ;
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Show the window tree 
/// </summary>
void WinTrace::DisplayWin ()
{
   CommandList Commands  ;
   Commands.Add (CST_DISPLAY_TREE); 
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// change the tree to display user defined multiple columns
/// must be called before setting column titles
/// </summary>
/// <param name="MainColIndex">The Main column index (default is 0)</param>
void WinTrace::SetMultiColumn(const int MainColIndex)
{
   CommandList Commands  ;
   Commands.Add (CST_TREE_MULTI_COLUMN, MainColIndex); 
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// set columns title
/// </summary>
/// <param name="Titles">Tab separated columns titles</param>
void WinTrace::SetColumnsTitle (const char * Titles)
{
   CommandList Commands  ;
   Commands.Add (CST_TREE_COLUMNTITLE , Titles);
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// set columns title
/// </summary>
/// <param name="Titles">Tab separated columns titles</param>
void WinTrace::SetColumnsTitle (const wchar_t * Titles)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(Titles) ;
   Commands.Add (CST_TREE_COLUMNTITLE , temp);
   if (temp != NULL)
      free (temp) ;
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// set columns widths
/// </summary>
/// <param name="Widths">Tab separated columns width. <br> 
///    The format for each column is width[:Min[:Max]] <br>
///    where Min and Max are optional minimum and maximum column width for resizing purpose.<br>
///    Example : 100:20:80 tab 200:50 tab 100
/// </param>
void WinTrace::SetColumnsWidth (const char * Widths)
{
   CommandList Commands  ;
   Commands.Add (CST_TREE_COLUMNWIDTH , Widths);
   TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// set columns widths
/// </summary>
/// <param name="Widths">Tab separated columns width. <br> 
///    The format for each column is width[:Min[:Max]] <br>
///    where Min and Max are optional minimum and maximum column width for resizing purpose.<br>
///    Example : 100:20:80 tab 200:50 tab 100
/// </param>
void WinTrace::SetColumnsWidth (const wchar_t * Widths)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(Widths) ;
   Commands.Add (CST_TREE_COLUMNWIDTH , temp);
   if (temp != NULL)
      free (temp) ;
  TTrace::SendToClient (& Commands,this);
}

//-------------------------------------------------------------------------

/// <summary>
/// Clear all trace for the window tree 
/// </summary>
void WinTrace::ClearAll ()
{
   CommandList Commands  ;
   Commands.Add (CST_CLEAR_ALL); 
   TTrace::SendToClient (& Commands,this);
}                        

//------------------------------------------------------------------------------
// PLUGIN API
//------------------------------------------------------------------------------
#ifndef _WIN32_WCE

/// <summary>
/// Plugin API : Create a resource.
/// </summary>
/// <param name="ResId">The resource Id (must be >= 100)</param>
/// <param name="ResType">Resource type. See TraceConst  
/// <code>
/// CST_RES_BUT_RIGHT    : Button on right
/// CST_RES_BUT_LEFT     : Button on left
/// CST_RES_LABEL_RIGHT  : Label on right
/// CST_RES_LABELH_RIGHT : Label on right HyperLink
/// CST_RES_LABEL_LEFT   : Label on left
/// CST_RES_LABELH_LEFT  : Label on left hyperlink
/// CST_RES_MENU_ACTION  : Item menu in the Actions Menu
/// CST_RES_MENU_WINDOW  : Item menu in the Windows Menu. 
///                        Call CreateResource on the main win trace to create this menu item
/// </code>
///</param>
/// <param name="ResWidth">Width of the resource. Applicable only to button and labels</param>
/// <param name="ResText">Resource text</param>

void WinTrace::CreateResource (const int ResId , const int ResType , const int ResWidth , const char * ResText /* = NULL */)
{
   CommandList Commands  ;
   Commands.Add (CST_CREATE_RESOURCE, ResId , ResType, ResWidth , ResText);
   TTrace::SendToClient (& Commands,this);
}

//------------------------------------------------------------------------------

void WinTrace::CreateResource (const int ResId , const int ResType , const int ResWidth , const wchar_t * ResText /* = NULL */)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(ResText) ;
   Commands.Add (CST_CREATE_RESOURCE, ResId , ResType, ResWidth , temp);
   if (temp != NULL)
      free (temp) ;
   TTrace::SendToClient (& Commands,this);
}

//------------------------------------------------------------------------------

/// <summary>
/// Plugin API : Disable tracetool or user created resources
/// </summary>
/// <param name="ResId">The resource Id 
/// ResId: resource id to disable. Tracetool resources :
/// <code>
/// CST_ACTION_CUT            : Cut. Same as copy then delete
/// CST_ACTION_COPY           : Copy
/// CST_ACTION_DELETE         : Delete selected
/// CST_ACTION_SELECT_ALL     : Select all
/// CST_ACTION_RESIZE_COLS    : Resize columns
/// CST_ACTION_VIEW_INFO      : View trace info
/// CST_ACTION_VIEW_PROP      : View properties
/// CST_ACTION_PAUSE          : Pause
/// CST_ACTION_SAVE           : SaveToFile
/// CST_ACTION_CLEAR_ALL      : Clear all
/// CST_ACTION_CLOSE_WIN      : Close win
/// CST_ACTION_LABEL_INFO     : TracesInfo label
/// CST_ACTION_LABEL_LOGFILE  : LabelLogFile label
/// CST_ACTION_VIEW_MAIN      : View Main trace
/// CST_ACTION_VIEW_ODS       : ODS
/// CST_ACTION_OPEN_XML       : XML trace -> Tracetool XML traces
/// CST_ACTION_EVENTLOG       : Event log
/// CST_ACTION_TAIL           : Tail
/// </code>
/// </param>

void WinTrace::DisableResource (const int ResId)
{
   CommandList Commands  ;
   Commands.Add (CST_DISABLE_RESOURCE ,ResId);
   TTrace::SendToClient (& Commands,this);
}

//------------------------------------------------------------------------------

/// <summary>
/// Plugin API : Set the resource text (tracetool or user created resources), specified by his Id
/// </summary>
/// <param name="ResId">The resource Id </param>
/// <param name="ResText">Resource text</param>

void WinTrace::SetTextResource (const int ResId, const char * ResText)
{
   CommandList Commands  ;
   Commands.Add (CST_SET_TEXT_RESOURCE ,ResId, ResText);
   TTrace::SendToClient (& Commands,this);
}

//------------------------------------------------------------------------------

void WinTrace::SetTextResource (const int ResId, const wchar_t * ResText)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(ResText) ;
   Commands.Add (CST_SET_TEXT_RESOURCE ,ResId, temp);
   if (temp != NULL)
      free (temp) ;
   TTrace::SendToClient (& Commands,this);
}

//------------------------------------------------------------------------------

/// <summary>
/// Plugin API : Attach a winTrace to a plugin. Many winTrace can be attached to a plugin.
/// Note that a plugin don't need to be attached to a WinTrace.
/// The plugin is identified by his internal name (not dll name).
/// When linked, the plugin can receive event (see ITracePLugin).
/// </summary>
/// <param name="PluginName">name of the plugin</param>
/// <param name="flags">combinaison of CST_PLUG_ONACTION , CST_PLUG_ONBEFOREDELETE , CST_PLUG_ONTIMER</param>

void WinTrace::LinkToPlugin (const char * PluginName, const int flags)
{
   CommandList Commands  ;
   Commands.Add (CST_LINKTOPLUGIN ,flags, PluginName);
   TTrace::SendToClient (& Commands,this);
}

//------------------------------------------------------------------------------

void WinTrace::LinkToPlugin (const wchar_t * PluginName, int flags)
{
   CommandList Commands  ;
   char * temp   = TTrace::WideToMbs(PluginName) ;
   Commands.Add (CST_LINKTOPLUGIN ,flags, temp);
   if (temp != NULL)
      free (temp) ;
   TTrace::SendToClient (& Commands,this);
}

#endif

//==========================================================================

/* TraceNode */

/// <summary>
/// construct a new trace node, derived from a parent node 
/// </summary>
/// <param name="parentNode">Parent node (Optional) </param>
/// <param name="generateUniqueId">generate an unique Id if true (Optional, default is true)</param>

TraceNode::TraceNode(const TraceNode * parentNode  , const bool generateUniqueId /*  = true */ ) 
{  
   id       = NULL ;                     // Node id
   enabled  = true ;                     // enable or disable traces
   winTrace = NULL ;                     // Owner
   tag      = 0 ;                        // User defined tag, NOT SEND to the viewer   
	indent   = 0;
   iconIndex = CST_ICO_DEFAULT ;         // icon index
   contextList = NULL ;
   InitializeCriticalSection(&criticalSection) ;

   if (generateUniqueId)
      id = TTrace::CreateTraceID () ;
   if (parentNode != NULL)
   {
      iconIndex    = parentNode->iconIndex ;
      enabled      = parentNode->enabled ;
      winTrace     = parentNode->winTrace ;
   }
}

//-------------------------------------------------------------------------

// construct a new trace node, derived from a parent node 
TraceNode::TraceNode(const TraceNode * parentNode , const char * newNodeId )
{    
   id       = NULL ;                     // Node id
   enabled  = true ;                     // enable or disable traces
   winTrace = NULL ;                     // Owner
   tag      = 0 ;                        // User defined tag, NOT SEND to the viewer   
	indent   = 0;
   iconIndex = CST_ICO_DEFAULT ;         // icon index
   contextList = NULL ;
   InitializeCriticalSection(&criticalSection) ;

   // set the node id
   if (newNodeId != NULL) 
   {
      int IdLength = strlen(newNodeId)+1 ;
      id = (char*)malloc (IdLength) ;
      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
      strcpy_s(id,IdLength,newNodeId) ;  
      #else
      strcpy(id,newNodeId) ;  
      #endif
   }

   if (parentNode != NULL)
   {
      iconIndex    = parentNode->iconIndex ;
      enabled      = parentNode->enabled ;
      winTrace     = parentNode->winTrace ;
   }
}

//-------------------------------------------------------------------------

// construct a new trace node, derived from a parent node 
TraceNode::TraceNode(const TraceNode * parentNode , const wchar_t * newNodeId )
{   
   id       = NULL ;                     // Node id
   enabled  = true ;                     // enable or disable traces
   winTrace = NULL ;                     // Owner
   tag      = 0 ;                        // User defined tag, NOT SEND to the viewer   
	indent   = 0;
   iconIndex = CST_ICO_DEFAULT ;         // icon index
   contextList = NULL ;
   InitializeCriticalSection(&criticalSection) ;

   // set the node id
   if (newNodeId != NULL) 
      id = TTrace::WideToMbs(newNodeId);

   if (parentNode != NULL)
   {
      iconIndex    = parentNode->iconIndex ;
      enabled      = parentNode->enabled ;
      winTrace     = parentNode->winTrace ;
   }
}

//-------------------------------------------------------------------------

/// <summary>
/// destructor
/// </summary>

TraceNode::~TraceNode(void) 
{
   DeleteCriticalSection(&criticalSection) ;
   if (contextList != NULL)
      delete contextList ;
   contextList = NULL ;

   if (id != NULL)
      free (id) ;
}

//-------------------------------------------------------------------------

/// <summary>
/// prepare minimal sub node without sending it 
/// </summary>
/// <param name="leftMsg" >Optional left message</param>
/// <param name="rightMsg">Optional right message</param>

TraceNodeEx *TraceNode::CreateChildEx (const char *leftMsg /* = NULL */, const char *rightMsg /* = NULL */)
{
   TraceNodeEx *Node;
   Node = new TraceNodeEx(this);   // Node->id is generated (GUID)

   if (leftMsg != NULL && leftMsg[0]!=0) 
      Node->leftMsg = leftMsg ; 

   if (rightMsg != NULL && rightMsg[0]!=0) 
      Node->rightMsg = rightMsg ;

   Node->Commands->Add(CST_ICO_INDEX, iconIndex);      // This->iconIndex is the same as Node->iconIndex
   return Node ;                   
}

//-------------------------------------------------------------------------

/// <summary>
/// prepare minimal sub node without sending it 
/// </summary>
/// <param name="leftMsg" >Optional left message</param>
/// <param name="rightMsg">Optional right message</param>

TraceNodeEx *TraceNode::CreateChildEx (const wchar_t *leftMsg /* = NULL */, const wchar_t *rightMsg /* = NULL */)
{
   TraceNodeEx *Node;
   Node = new TraceNodeEx(this);   // Node->id is generated (GUID)

   if (leftMsg != NULL && leftMsg[0]!=0) 
   {
      char * temp = TTrace::WideToMbs(leftMsg) ;
      Node->leftMsg = temp ; // copy
      free (temp);
   }

   if (rightMsg != NULL && rightMsg[0]!=0) 
   {
      char * temp = TTrace::WideToMbs(rightMsg) ;
      Node->rightMsg = temp ; // copy
      free (temp);
   }

   Node->Commands->Add(CST_ICO_INDEX, iconIndex);      // This->iconIndex is the same as Node->iconIndex
   return Node ;                   
}

//-------------------------------------------------------------------------

/// <summary>
/// Return the IconIndex
/// <//summary>
int TraceNode::GetIconIndex (void)
{
   return iconIndex ;
}

//-------------------------------------------------------------------------

/// <summary>
/// send a node then change the indentation
/// </summary>
/// <param name="leftMsg" >Left message</param>
/// <param name="rightMsg">Optional right message</param>

//void TraceNode::Indent (const TCHAR *leftMsg, const TCHAR *rightMsg) 
//{
//   if (enabled == false) 
//      return ;
//
//   TraceNodeEx * Node;  
//   Node = CreateChildEx (leftMsg, rightMsg) ;   // create a new node using the last context as parent
//   Node->Send() ;
//
//   PushContextId (Node->id) ;  // create a context based on the node id and the current thread
//   delete Node ;
//}

//-------------------------------------------------------------------------
/// <summary>
/// Decrement indentation
/// </summary>
/// <param name="leftMsg" >Optional left message</param>
/// <param name="rightMsg">Optional right message</param>

//void TraceNode::UnIndent (const TCHAR *leftMsg, const TCHAR *rightMsg) 
//{
//   if (enabled == false) 
//      return ;
//   deleteLastContext() ;
//
//	if (!leftMsg) return;
//
//   TraceNodeEx * Node;  
//   Node = CreateChildEx (leftMsg, rightMsg) ;   // create a new node using the last context as parent
//   Node->Send() ;
//   delete Node ;
//}
//

//-------------------------------------------------------------------------

/// <summary>
/// create a context based on the node id and the current thread
/// </summary>
/// <param name="contextId" >Node id</param>

void TraceNode::PushContextId (const char * contextId) 
{
   NodeContext * context = new NodeContext() ;
   context->nodeId = contextId ; // string copy (nodeId is a string not a char *)
   context->threadId = GetCurrentThreadId() ;
   if (contextList == NULL)
      contextList = new deque <NodeContext *> ;

   // enter the node context critical section
   EnterCriticalSection (&criticalSection) ; 
   contextList->push_front (context) ;
   LeaveCriticalSection (&criticalSection) ; 
}

//-------------------------------------------------------------------------

/// <summary>
/// delete the last context for the current thread
/// </summary>

void TraceNode::deleteLastContext (void)
{
   if (contextList == NULL)  // should not happens
      return ; 

   // if empty, no need to enter critical section
   if (contextList->empty())  // should not happens
      return ;

   deque <NodeContext *>::iterator stack_end ; 
   deque <NodeContext *>::iterator stack_ptr ; 
   NodeContext * context ;
   context = NULL ;
   DWORD thid = GetCurrentThreadId();

   // enter the node context critical section
   EnterCriticalSection (&criticalSection) ; 

   // loop context for the current thread
   stack_end = contextList->end() ;                   
   for (stack_ptr = contextList->begin() ; stack_ptr < stack_end ; stack_ptr++) 
   {
      NodeContext * context = * stack_ptr ;
      if (context->threadId == thid)
      {
         delete context ;
         contextList->erase (stack_ptr) ;
         LeaveCriticalSection (&criticalSection) ;
         return ;
      }
   }
   LeaveCriticalSection (&criticalSection) ;
   return ;
}

//-------------------------------------------------------------------------

/// <summary>
/// Retun the last context id for the current thread
/// </summary>

// called by CreateChildEx()
const char * TraceNode::GetLastContextId() 
{
   if (contextList == NULL)
      return id ; 

   // if empty, no need to enter critical section
   if (contextList->empty())  
      return id ;

   char * result ;
   result = id ;

   deque<NodeContext *>::const_iterator stack_end ; 
   deque<NodeContext *>::const_iterator stack_ptr ; 

   DWORD thid = GetCurrentThreadId();

   // enter the node context critical section
   EnterCriticalSection (&criticalSection) ; 

   // loop context for the current thread
   stack_end = contextList->end() ; 

   for (stack_ptr = contextList->begin() ; stack_ptr < stack_end ; stack_ptr++) 
   {
      NodeContext * context = * stack_ptr ;
      if (context->threadId == thid)
      {
         LeaveCriticalSection (&criticalSection) ;
         return context->nodeId.c_str() ;
      }
   }
   LeaveCriticalSection (&criticalSection) ;
   return id ;
} ;            

//-------------------------------------------------------------------------

/// <summary>
/// Create and send a trace node to the viewer
/// </summary>                                
/// <param name="leftMsg">the left message</param>
/// <param name="rightMsg">the optional right message</param>

//void TraceNode::Send(const char *leftMsg, const char *rightMsg /* = NULL */)
//{
//	ATLASSERT(0); // not supported
//   if (enabled == false) 
//      return ;
//
//   TraceNodeEx * Node;  
//   Node = CreateChildEx (leftMsg, rightMsg) ; 
//   Node->Send() ;
//   delete Node ;
//}
//
//-------------------------------------------------------------------------
/// <summary>
/// Create and send a trace node to the viewer
/// </summary>                                
/// <param name="leftMsg">the left message</param>
/// <param name="rightMsg">the optional right message</param>

void TraceNode::Send(const wchar_t *wLeftMsg, const wchar_t *wRightMsg /* = NULL */)
{
   if (enabled == false) 
      return ;

   TraceNodeEx * Node; 
   char * LeftMsg  = TTrace::WideToMbs (wLeftMsg, indent*2) ;
   char * RightMsg = TTrace::WideToMbs (wRightMsg, indent*2) ;
   Node = CreateChildEx (LeftMsg, RightMsg) ; 
   Node->Send() ;
   delete Node ;
   free (LeftMsg) ;
   if (RightMsg != NULL)
       free (RightMsg) ;
}

//-------------------------------------------------------------------------
/// <summary>
/// send a dump to the viewer
/// </summary>                                
/// <param name="leftMsg">Left message</param>
/// <param name="rightMsg">Optional right message</param>
/// <param name="title">The title that appears on top of the dump</param>
/// <param name="memory">The memory to dump</param>
/// <param name="byteCount"></param>

//void TraceNode::SendDump (const char *leftMsg, const char *rightMsg, const char * title, const char * memory, const unsigned byteCount) 
//{
//   if (enabled == false) 
//      return ;
//
//   TraceNodeEx * Node;  
//   Node = CreateChildEx (leftMsg, rightMsg) ; 
//   Node->AddDump (title, memory, 0 , byteCount) ;
//   Node->Send() ;
//   delete Node ;
//}
//
//-------------------------------------------------------------------------
/// <summary>
/// send a dump to the viewer
/// </summary>                                
/// <param name="leftMsg">Left message</param>
/// <param name="rightMsg">Optional right message</param>
/// <param name="title">The title that appears on top of the dump</param>
/// <param name="memory">The memory to dump</param>
/// <param name="byteCount"></param>

void TraceNode::SendDump (const wchar_t *leftMsg, const wchar_t *rightMsg, const wchar_t * title, const char * memory, const unsigned byteCount) 
{
   if (enabled == false) 
      return ;

   TraceNodeEx * Node;  
   Node = CreateChildEx (leftMsg, rightMsg) ; 
   Node->AddDump (title, memory, 0 , byteCount) ;
   Node->Send() ;
   delete Node ;
}

//==========================================================================

// TraceNodeEx

/// <summary>
/// Construct a new trace node, derived from a parent node 
/// TraceNodeEx can also be created from another node (TraceNode or TraceNodeEx)
/// sample : TTrace::Debug()->CreateChildEx (left,right) ;
/// </summary>
/// <param name="parentNode">Parent node</param>
/// <param name="newNodeId">Node Id. If NULL, an unique Id is generated </param>

TraceNodeEx::TraceNodeEx(TraceNode * parentNode , const char * newNodeId ) 
: TraceNode( parentNode,newNodeId )
{                            
   m_Members = NULL ;              // create members only when needed 
   m_FontDetails = NULL ; 
   Commands = new CommandList() ;

   // the only place where the node is created
   if (parentNode == NULL)
   {
      Commands->Add(CST_NEW_NODE, NULL);           // Parameter : Parent Node ID
   } else {
      Commands->Add(CST_NEW_NODE, parentNode->GetLastContextId());  // Parameter : Parent Node ID
   }

   Commands->Add(CST_TRACE_ID, id);           // Id of the new node  
}

//-------------------------------------------------------------------------

/// <summary>
/// Construct a new trace node, derived from a parent node 
/// TraceNodeEx can also be created from another node (TraceNode or TraceNodeEx)
/// sample : TTrace::Debug()->CreateChildEx (left,right) ;
/// </summary>
/// <param name="parentNode">Parent node</param>
/// <param name="newNodeId">Node Id. If NULL, an unique Id is generated </param>

TraceNodeEx::TraceNodeEx(TraceNode * parentNode , const wchar_t * newNodeId ) 
: TraceNode( parentNode,newNodeId )
{                            
   m_Members = NULL ;              // create members only when needed 
   m_FontDetails = NULL ; 
   Commands = new CommandList() ;

   // the only place where the node is created
   if (parentNode == NULL)
   {
      Commands->Add(CST_NEW_NODE, NULL);           // Parameter : Parent Node ID
   } else {
      Commands->Add(CST_NEW_NODE, parentNode->GetLastContextId());  // Parameter : Parent Node ID
   }
   Commands->Add(CST_TRACE_ID, id);           // Id of the new node  
}

//-------------------------------------------------------------------------

TraceNodeEx::TraceNodeEx(TraceNode * parentNode , bool generateUniqueId ) 
: TraceNode( parentNode,generateUniqueId ) 
{                              
   m_Members = NULL ;              // create members only when needed 
   m_FontDetails = NULL ; 
   Commands = new CommandList() ;

   // the only place where the node is created
   if (parentNode == NULL)
   {
      Commands->Add(CST_NEW_NODE, NULL);           // Parameter : Parent Node ID
   } else {
      Commands->Add(CST_NEW_NODE, parentNode->GetLastContextId());  // Parameter : Parent Node ID
   }
   Commands->Add(CST_TRACE_ID, id);           // Id of the new node  
}

//-------------------------------------------------------------------------

/// <summary>
/// Normally destructors are not needed : TTrace::Send clear the node content. 
/// It's usefull only if you create a node without sending it.
/// </summary>

TraceNodeEx::~TraceNodeEx(void) 
{
   delete Commands ;
   if (m_Members != NULL)      // happens only if the nodeEx is not send
      delete m_Members ;

   if (m_FontDetails != NULL)  // happens only if the nodeEx is not send
   {
      deque <FontDetail *>::const_iterator FontDetailBegin;
      deque <FontDetail *>::const_iterator FontDetailEnd;

      FontDetailEnd  = m_FontDetails->end () ;
      for (FontDetailBegin = m_FontDetails->begin (); FontDetailBegin < FontDetailEnd ; FontDetailBegin++) 
      {
         FontDetail * fontDetail = * FontDetailBegin ;
         delete fontDetail ;
      }
      delete m_FontDetails ;
      m_FontDetails = NULL ;
   }
}

//-------------------------------------------------------------------------

/// <summary>
/// Set the IconIndex. This Method can be called only once.
/// Usefull only if you create the TraceNodeEx with NULL parentNode
/// <//summary>
void TraceNodeEx::SetIconIndex (const int newInconIndex)
{
   iconIndex = newInconIndex ;
   Commands->Add(CST_ICO_INDEX, iconIndex);      
}

//-------------------------------------------------------------------------
/// <summary>
/// return the protected "members" list (and create it if needed)
/// <//summary>

TMemberNode * TraceNodeEx::Members() 
{
   if (m_Members == NULL)
      m_Members = new TMemberNode() ;
   return m_Members ;
}

//-------------------------------------------------------------------------

// Send must be reintroduce , because TraceNodeEx add a Send function without parameter.
//void TraceNodeEx::Send (const char *leftMsg, const char *rightMsg) 
//{
//  TraceNode::Send (leftMsg, rightMsg) ;
//}

//-------------------------------------------------------------------------

// Send must be reintroduce , because TraceNodeEx add a Send function without parameter.
void TraceNodeEx::Send (const wchar_t *leftMsg, const wchar_t *rightMsg) 
{
  TraceNode::Send (leftMsg, rightMsg) ;
}

//-------------------------------------------------------------------------

/// <summary>
/// Send the extended node to the viewer
/// <summary>

void TraceNodeEx::Send(void)                                        
{
   if (enabled == false) 
      return ;

   SYSTEMTIME Time;
   char buffer [MAX_PATH] ;

   // convert Members to commands
   if (m_Members != NULL)
   {
      m_Members->AddToStringList (Commands) ;
      delete m_Members ;
      m_Members = NULL ;
   }

   // Add Left message
   if (leftMsg.length() != 0) 
   {
      Commands->Add(CST_LEFT_MSG, leftMsg.c_str());
      leftMsg = "" ;    // reset after send
   }

   // Add Right message
   if (rightMsg.length() != 0) 
   {
      Commands->Add(CST_RIGHT_MSG, rightMsg.c_str());
      rightMsg = "" ;   // reset after send
   }

   if (m_FontDetails != NULL)
   {
      deque <FontDetail *>::const_iterator FontBegin;
      deque <FontDetail *>::const_iterator FontEnd;
      FontEnd  = m_FontDetails->end () ;
      for (FontBegin = m_FontDetails->begin (); FontBegin < FontEnd ; FontBegin++) 
      {
         FontDetail * font = * FontBegin ;

         char * message ;
         int bold   = (font->Bold  ) ? 1 : 0 ;
         int italic = (font->Italic) ? 1 : 0;
         int MsgLen ;

         if (font->FontName.empty())
         {
            message = (char*)malloc(32+1) ;
            #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
            sprintf_s(message, 33,"%5d%3d%1d%1d%11d%11d", CST_FONT_DETAIL,font->ColId,bold,italic,font->Color,font->Size);
            #else
            sprintf(message, "%5d%3d%1d%1d%11d%11d", CST_FONT_DETAIL,font->ColId,bold,italic,font->Color,font->Size);
            #endif
         } else {
            MsgLen = font->FontName.length()+32+1 ;
            message = (char*)malloc(MsgLen) ;
            const char * fontName = font->FontName.c_str() ;
            #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
            sprintf_s(message, MsgLen,"%5d%3d%1d%1d%11d%11d%s", CST_FONT_DETAIL,font->ColId,bold,italic,font->Color,font->Size,fontName);
            #else
            sprintf(message, "%5d%3d%1d%1d%11d%11d%s", CST_FONT_DETAIL,font->ColId,bold,italic,font->Color,font->Size,fontName);
            #endif
         }
         Commands->push_back (message) ;
         free (message) ;         
         delete font ;
      }
      delete m_FontDetails ;
      m_FontDetails = NULL ;
   }

   // Add process name
   if (TTrace::Options()->SendProcessName)
      Commands->AddFront(CST_PROCESS_NAME, TTrace::Options()->CheckProcessName()); 

   // add threading info
   Commands->AddFront(CST_THREAD_ID, GetCurrentThreadId() );    

   // message time
   GetLocalTime(&Time);
   #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
   sprintf_s(buffer,MAX_PATH, "%02d:%02d:%02d:%03d", Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds); 
   #else
   sprintf(buffer, "%02d:%02d:%02d:%03d", Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds); 
   #endif
   Commands->AddFront(CST_MESSAGE_TIME, buffer);

   // send the commands to the viewer.
   TTrace::SendToClient(Commands,winTrace); 
}

//-------------------------------------------------------------------------
/// <summary>
/// Add a dump to the "member"
/// </summary>                                
/// <param name="leftMsg">Left message</param>
/// <param name="rightMsg">Optional right message</param>
/// <param name="title">The title that appears on top of the dump</param>
/// <param name="memory">The memory to dump</param>
/// <param name="byteCount"></param>

TraceNodeEx * TraceNodeEx::AddDump(const wchar_t * Title , const char * memory , const unsigned index , const unsigned byteCount)
{
   char * temp = TTrace::WideToMbs(Title) ;
   AddDump(temp , memory , index , byteCount) ;
   free (temp) ;
   return this ;
}

//-------------------------------------------------------------------------
/// <summary>
/// Add a dump to the "member"
/// </summary>                                
/// <param name="leftMsg">Left message</param>
/// <param name="rightMsg">Optional right message</param>
/// <param name="title">The title that appears on top of the dump</param>
/// <param name="memory">The memory to dump</param>
/// <param name="byteCount"></param>

TraceNodeEx * TraceNodeEx::AddDump(const char * Title , const char * memory , const unsigned index , const unsigned byteCount)
{
   unsigned c,d, byteDumped, beginLine ;
   char hexa_representation [50] ; 
   char Str_representation [20] ;
   char adress [10] ;
   unsigned char * PtrBuf ; 
   char * ptrHex , * ptrString  ;
   unsigned int OneChar ;
   TMemberNode * DumpGroup ;

   if (enabled == false) 
      return this;

   Members () ;  // ensure root member is create

   DumpGroup = new TMemberNode (Title) ;
   DumpGroup->SetFontDetail(0, true);
   m_Members->Add (DumpGroup) ;

   PtrBuf = (unsigned char *) memory ;
   c = 0 ;
   byteDumped = 0 ;
   while (byteDumped < byteCount) 
   {
      d = 0 ;
      beginLine = c ;
      ptrHex = hexa_representation ;
      ptrString = Str_representation ;
      while ((byteDumped < byteCount) && (d < 16)) 
      {
         OneChar = *PtrBuf ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s (ptrHex, 4, "%02X ", OneChar) ;
         #else
         sprintf (ptrHex, "%02X ", OneChar) ;
         #endif
         ptrHex += 3 ;

         //if (OneChar == 0) 
         //   OneChar = '.' ;
         if (isprint(OneChar) == 0)
            OneChar = '.' ;

         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s (ptrString, 2 ,"%c",OneChar) ;
         #else
         sprintf (ptrString,"%c",OneChar) ;
         #endif
         ptrString++ ;

         d++ ;
         c++ ;
         PtrBuf++ ;
         byteDumped++;
      }
      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
      sprintf_s (adress,8,"%06X",beginLine) ; 
      #else
      sprintf (adress,"%06X",beginLine) ; 
      #endif
      DumpGroup->Add (adress , hexa_representation , Str_representation) ;
   }
   // reuse the hexa_representation to printf the number of byte dumped
   #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
   sprintf_s (hexa_representation,50,"%d byte(s) dumped", byteDumped) ;
   #else
   sprintf (hexa_representation,"%d byte(s) dumped", byteDumped) ;
   #endif
   DumpGroup->col2 = hexa_representation ;
   return this ;
}

//------------------------------------------------------------------------------


/// <summary>
/// Change font detail for an item in the trace
/// </summary>
/// <param name="ColId">Column index : Icon=0, Time=1, thread=2, left msg=3, right msg =4 or user defined column</param>
/// <param name="Bold">Change font to bold</param>
/// <param name="Italic">Change font to Italic</param>
/// <param name="Color">Change Color. Use -1 to keep default color</param>
/// <param name="Size">Change font size, use zero to keep normal size</param>
/// <param name="FontName">Change font name</param>
/// <returns>The TMemberNode </returns>

TraceNodeEx * TraceNodeEx::AddFontDetail(const int ColId,  const bool Bold ,  const bool Italic /*= false*/ ,  const int Color /*= -1*/ , const int Size /*= 0*/ ,  const char * FontName /*= NULL*/) 
{
   if (m_FontDetails == NULL)
      m_FontDetails = new deque <FontDetail *> ;
   FontDetail * font = new FontDetail() ;

   font->ColId    = ColId ;
   font->Bold     = Bold ;
   font->Italic   = Italic ;
   font->Color    = Color ;
   font->Size     = Size ;
   if (FontName != NULL)
      font->FontName = FontName ;
   m_FontDetails->push_back(font);
   return this ;
}

//------------------------------------------------------------------------------
/// <summary>
/// Delete the node
/// </summary>

void TraceNodeEx::Delete() 
{
   if (enabled == false)
      return ;

   if (id == NULL)
      return ;

   Commands->AddFront(CST_CLEAR_NODE, id);
   TTrace::SendToClient(Commands,winTrace); 
}

//------------------------------------------------------------------------------
/// <summary>
/// Delete children node
/// </summary>

void TraceNodeEx::DeleteChildren ()
{

   if (enabled == false)
      return ;

   if (id == NULL)
      return ;

   Commands->AddFront(CST_CLEAR_SUBNODES, id);
   TTrace::SendToClient(Commands,winTrace); 
}

//----------------------------------------------------------------------
/// <summary>
/// Show the node in the tree (not means selected, just visible in the tree)
/// </summary>

void TraceNodeEx::Show ()
{
   if (enabled == false)
      return ;

   if (id == NULL)
      return ;

   Commands->AddFront(CST_FOCUS_NODE, id);
   TTrace::SendToClient(Commands,winTrace); 
}

//----------------------------------------------------------------------
/// <summary>
/// Select the node in the viewer
/// </summary>

void TraceNodeEx::SetSelected ()
{
   if (enabled == false)
      return ;

   if (id == NULL)
      return ;

   Commands->AddFront(CST_SELECT_NODE, id);
   TTrace::SendToClient(Commands,winTrace); 
}

//----------------------------------------------------------------------
/// <summary>
/// append right and left texts to an existing node
/// </summary>
/// <param name="leftMsg">Left message to append</param>
/// <param name="rightMsg">Optional right message to append</param>

//void TraceNodeEx::Append (const char * LeftMsg, const char * RightMsg /* = NULL */)
//{
//   if (enabled == false)
//      return ;
//
//   if (id == NULL)
//      return ;
//
//   Commands->AddFront(CST_USE_NODE, id);                           // param : guid
//   if (LeftMsg != NULL)
//      Commands->Add(CST_APPEND_LEFT_MSG, LeftMsg);                 // param : left string
//
//   if (RightMsg != NULL)
//      Commands->Add(CST_APPEND_RIGHT_MSG, RightMsg);               // param : right string
//   TTrace::SendToClient(Commands,winTrace); 
//}

//----------------------------------------------------------------------

/// <summary>
/// append right and left texts to an existing node
/// </summary>
/// <param name="leftMsg">Left message to append</param>
/// <param name="rightMsg">Optional right message to append</param>

//void TraceNodeEx::Append (const wchar_t * LeftMsg, const wchar_t * RightMsg /* = NULL */)
//{
//   char * left  = TTrace::WideToMbs (LeftMsg) ;
//   char * right = TTrace::WideToMbs (RightMsg) ;
//
//   Append (left,right) ;
//
//   if (left != NULL)
//      free (left) ;
//   if (right != NULL)
//      free (right) ;
//}

//----------------------------------------------------------------------

/// <summary>
/// Resend the trace to the server (only left and right message)
/// You can also set the left and right message then call Resend without parameters :
///    Node->leftMsg = "New Left" ;
///    Node->Resend() ;
/// </summary>
/// <param name="leftMsg">Left message to replace</param>
/// <param name="rightMsg">Optional right message to replace</param>

//void TraceNodeEx::Resend (const wchar_t *LeftMsg, const wchar_t *RightMsg /* = NULL */) 
//{
//   char * left  = TTrace::WideToMbs (LeftMsg) ;
//   char * right = TTrace::WideToMbs (RightMsg) ;
//   
//   Resend (left,right) ; 
//   
//   if (left != NULL)
//      free (left) ;
//   if (right != NULL)
//      free (right) ;
//}

//----------------------------------------------------------------------

/// <summary>
/// Resend the trace to the server (only left and right message)
/// You can also set the left and right message then call Resend without parameters :
///    Node->leftMsg = "New Left" ;
///    Node->Resend() ;
/// </summary>
/// <param name="leftMsg">Left message to replace</param>
/// <param name="rightMsg">Optional right message to replace</param>

//void TraceNodeEx::Resend (const char *LeftMsg, const char *RightMsg /* = NULL */) 
//{
//   if (enabled == false)
//      return ;
//
//   if (id == NULL)
//      return ;
//
//   Commands->AddFront(CST_USE_NODE, id);   // param : guid
//
//   if (LeftMsg != NULL)
//      this->leftMsg =  LeftMsg ;
//
//   if (RightMsg != NULL)
//      this->rightMsg = RightMsg ;
//
//   if (leftMsg.length() != 0) 
//   {
//      Commands->Add(CST_LEFT_MSG, leftMsg.c_str());
//      this->leftMsg = "" ;    // reset after send
//   }
//
//   if (rightMsg.length() != 0) 
//   {
//      Commands->Add(CST_RIGHT_MSG, rightMsg.c_str());
//      this->rightMsg = "" ;   // reset after send
//   }
//
//
//   TTrace::SendToClient(Commands,winTrace); 
//}

//==========================================================================

// Close TTrace properly on exit. No memory leak are reported 
class _TraceTool_Cleaner    
{
public:
   ~_TraceTool_Cleaner(void) {TTrace::Stop() ;} ;    
}  _traceTool_Cleaner ;

//==========================================================================
#endif
