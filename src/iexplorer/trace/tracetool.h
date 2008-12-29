// tracetool.h
//
// Author : Thierry Parent
// Version : 8.0.0
//
// HomePage :  http://www.codeproject.com/csharp/TraceTool.asp
// Download :  http://sourceforge.net/projects/tracetool/
// See License.txt for license information

#pragma once

#include <deque>
#include <string>
using namespace std ;

#pragma warning(push)
#pragma warning(disable: 4267 4244) 

class TTrace ;
class TraceNode ;
class TraceNode ;
class TraceNodeEx ; 
class WinTrace ;
class TraceOptions ;
class TMemberNode ;

//-------------------------------------------------------------------------

const int WMD_TRACETOOL            = 123 ;  // identification code 'traceTool'

// Icones
const int CST_ICO_DEFAULT          = -1 ;
const int CST_ICO_FORM             = 0 ;
const int CST_ICO_COMPONENT        = 1 ;
const int CST_ICO_CONTROL          = 3 ;
const int CST_ICO_PROP             = 5 ;
const int CST_ICO_MENU             = 15 ;
const int CST_ICO_MENU_ITEM        = 16 ;
const int CST_ICO_COLLECT_ITEM     = 21 ;
const int CST_ICO_WARNING          = 22 ;
const int CST_ICO_ERROR            = 23 ;
const int CST_ICO_INFO             = 24 ;     // default

// plugin
const int CST_PLUG_ONACTION        = 1 ;
const int CST_PLUG_ONBEFOREDELETE  = 2 ;
const int CST_PLUG_ONTIMER         = 4 ;

// resource kind
const int CST_RES_BUT_RIGHT        = 1 ;     // Button on right
const int CST_RES_BUT_LEFT         = 2 ;     // Button on left
const int CST_RES_LABEL_RIGHT      = 3 ;     // Label on right
const int CST_RES_LABELH_RIGHT     = 4 ;     // Label on right HyperLink
const int CST_RES_LABEL_LEFT       = 5 ;     // Label on left
const int CST_RES_LABELH_LEFT      = 6 ;     // Label on left hyperlink
const int CST_RES_MENU_ACTION      = 7 ;     // Item menu in the Actions Menu
const int CST_RES_MENU_WINDOW      = 8 ;     // Item menu in the Windows Menu. Call CreateResource on the main win trace to create this menu item

// resource id
const int CST_ACTION_CUT           = 1 ;     // cut same as copy then delete
const int CST_ACTION_COPY          = 2 ;     // copy
const int CST_ACTION_DELETE        = 3 ;     // delete selected
const int CST_ACTION_SELECT_ALL    = 4 ;     // select all
const int CST_ACTION_RESIZE_COLS   = 5 ;     // resize columns
const int CST_ACTION_VIEW_INFO     = 6 ;     // view trace info
const int CST_ACTION_VIEW_PROP     = 7 ;     // view properties
const int CST_ACTION_PAUSE         = 8 ;     // Pause on
const int CST_ACTION_SAVE          = 9 ;     // SaveToFile
const int CST_ACTION_CLEAR_ALL     = 10 ;    // clear all
const int CST_ACTION_CLOSE_WIN     = 11 ;    // Close win
const int CST_ACTION_RESUME        = 12 ;    // resume from Pause 

const int CST_ACTION_LABEL_INFO    = 20 ;    // TracesInfo label
const int CST_ACTION_LABEL_LOGFILE = 21 ;    // LabelLogFile label
const int CST_ACTION_VIEW_MAIN     = 50 ;    // View Main trace
const int CST_ACTION_VIEW_ODS      = 51 ;    // ODS
const int CST_ACTION_OPEN_XML      = 52 ;    // XML trace -> Tracetool XML traces
const int CST_ACTION_EVENTLOG      = 53 ;    // Event log
const int CST_ACTION_TAIL          = 54 ;    // Tail

// Command
   
const int CST_TREE_COLUMNWIDTH     = 93  ;   // Columns widths 
const int CST_USE_MULTICOL_TREE    = 94  ;   // same as CST_USE_TREE but the tree is multicolumn
const int CST_TREE_MULTI_COLUMN    = 95  ;   // change the columns titles
const int CST_TREE_COLUMNTITLE     = 96  ;   // change the tree to display multiple column
const int CST_DISPLAY_TREE         = 97  ;   // display tree windows
const int CST_TREE_NAME            = 98  ;   // param : the new name of the tree (use CST_USE_TREE just before to specify the tree)
const int CST_USE_TREE             = 99  ;   // param : Id (CLSID for example) of the tree to use for other command.
const int CST_INIT                 = 100 ;
const int CST_TRACE_ID             = 101 ;   // param : CLSID
const int CST_SHOW                 = 102 ;   // param : 1 : show.  0 : hide
const int CST_ICO_INDEX            = 103 ;   // param : image index
const int CST_CLEAR_ALL            = 104 ;   // no param

const int CST_WINWATCH_NAME        = 110 ;   // param : window name
const int CST_WINWATCH_ID          = 111 ;   // param : Window id
const int CST_WATCH_NAME           = 112 ;   // param : watch name
const int CST_WATCH_VALUE          = 113 ;   // param : watch value

const int CST_CLEAR_NODE           = 300 ;   // param : the node to clear + flag
const int CST_CLEAR_SUBNODES       = 301 ;   // param : the parent node
const int CST_THREAD_ID            = 302 ;   // param : thread ID
const int CST_PROCESS_NAME         = 303 ;   // param process name
const int CST_MESSAGE_TIME         = 304 ;   // param : the time of the message
const int CST_THREAD_NAME          = 305 ;   // param : thread name (java)

const int CST_CREATE_MEMBER        = 500 ;   // param : Member name
const int CST_MEMBER_FONT_DETAIL   = 501 ;   // param : ColId Bold Italic Color size  Fontname
const int CST_MEMBER_COL2          = 502 ;   // param : info col 2
const int CST_MEMBER_COL3          = 504 ;   // param : info col 3
const int CST_ADD_MEMBER           = 505 ;   // add member to upper level. No param (for now)

const int CST_NEW_NODE             = 550 ;   // param : parent node ID
const int CST_LEFT_MSG             = 551 ;   // param : left msg
const int CST_RIGHT_MSG            = 552 ;   // param : right msg
const int CST_SELECT_NODE          = 553 ;   // set the node as 'Selected' by the user.  param : Node id
const int CST_GET_NODE             = 554 ;   // return the node id
const int CST_USE_NODE             = 555 ;   // use an existing node. param : Node id
const int CST_APPEND_LEFT_MSG      = 556 ;   // param : left msg to append
const int CST_APPEND_RIGHT_MSG     = 557 ;   // param : right msg to append
const int CST_FOCUS_NODE           = 558 ;   // Focus to the node.
const int CST_SAVETOTEXT           = 559 ;   // save to text file, parameter : filename
const int CST_SAVETOXML            = 560 ;   // save to  XML file, parameter : filename
const int CST_LOADXML              = 561 ;   // load an XML file to the current wintrace
const int CST_LOGFILE              = 562 ;   // set the log file for a wintrace 
const int CST_LINKTOPLUGIN         = 563 ;   // link a wintrace to a plugin
const int CST_CREATE_RESOURCE      = 564 ;   // create a resource on a wintrace
const int CST_SET_TEXT_RESOURCE    = 565 ;   // set the text resource
const int CST_DISABLE_RESOURCE     = 566 ;   // disable a resource
const int CST_FONT_DETAIL          = 567 ;   // param : ColId Bold Italic Color size  Fontname
const int CST_FLUSH                = 800 ;  // special case to be interpreted by the sender thread (not to be send)


//====================================================================================

/// <summary>
/// The list of command to send
/// </summary>

template <class _Ty,	class _Ax = allocator<_Ty> >
class CommandDeque : public deque<_Ty, _Ax>
{	
public :
   // formatting Helper functions.
   
   /// back : code only
   void Add(const int msg) 
   {
      char message [10] ;
      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
      sprintf_s(message, 10,"%5d", msg);
      #else
      sprintf(message, "%5d", msg);
      #endif
      push_back (message) ;
   } ;
   
   //-------------------------------------------------------------------------
   
   /// front : code only
   void AddFront(const int msg)
   {
      char message[10];
      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
      sprintf_s(message, 10,"%5d", msg);
      #else
      sprintf(message, "%5d", msg);
      #endif
      push_front (message) ;
   }
   
   //-------------------------------------------------------------------------
   
   /// back : code + string
   void Add(const int msg, const char *StrValue)
   {
      char * message ;
      if (StrValue == NULL)
      {
         message = (char*)malloc(5+1) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message, 6,"%5d", msg);
         #else
         sprintf(message, "%5d", msg);
         #endif
      } else {
         int MsgLen = 5+strlen(StrValue)+1 ;
         message = (char*)malloc(MsgLen) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message, MsgLen,"%5d%s", msg, StrValue);
         #else
         sprintf(message, "%5d%s", msg, StrValue);
         #endif
      }
      push_back (message) ;
      free (message) ;
   }
   
   //-------------------------------------------------------------------------
   
   /// front : code + string
   void AddFront(const int msg, const char * StrValue)
   {
      char * message ;
      if (StrValue == NULL)
      {
         message = (char*)malloc(5+1) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message,6, "%5d", msg);
         #else
         sprintf(message, "%5d", msg);
         #endif
      } else {
         int MsgLen = 5+strlen(StrValue)+1 ;
         message = (char*)malloc(MsgLen) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message, MsgLen,"%5d%s", msg, StrValue);
         #else
         sprintf(message, "%5d%s", msg, StrValue);
         #endif
      }
      push_front (message) ;
      free (message) ;
   }
   
   //-------------------------------------------------------------------------
   
   /// back : code + int 
   void Add(const int msg, const int intValue)
   {
      char message[20];
      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)   // visual studio 2005 : deprecated function
      sprintf_s(message, 20, "%5d%11d", msg, intValue);      // 5 + 11 + 1
      #else
      sprintf(message, "%5d%11d", msg, intValue);      // 5 + 11 + 1
      #endif
      push_back (message) ;
   }
   
   //-------------------------------------------------------------------------
   
   /// front : code + int
   void AddFront(const int msg, const int intValue)
   {
      char message[20];
      #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
      sprintf_s(message, 20, "%5d%11d", msg, intValue);         // 5 + 11 + 1
      #else
      sprintf(message, "%5d%11d", msg, intValue);         // 5 + 11 + 1
      #endif
      push_front (message) ;
   }
   //-------------------------------------------------------------------------
   
   /// back : code + int + string
   void Add(const int msg, const int intValue, const char * StrValue)
   {
      char * message ;

      if (StrValue == NULL)
      {
         message = (char*)malloc(17) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message,17, "%5d%11d", msg, intValue);
         #else
         sprintf(message, "%5d%11d", msg, intValue);
         #endif
      } else {
         int messLen = 17+strlen(StrValue) ;
         message = (char*)malloc(messLen) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message,messLen, "%5d%11d%s", msg, intValue, StrValue);  // 5 + 11 + xxx + 1
         #else
         sprintf(message, "%5d%11d%s", msg, intValue, StrValue);  // 5 + 11 + xxx + 1
         #endif
      }

      push_back (message) ;
      free (message) ;
   }
   
   //-------------------------------------------------------------------------
   
   /// front : code + int + string
   void AddFront(const int msg, const int intValue, const char * StrValue)
   {
      char * message ;

      if (StrValue == NULL)
      {
         message = (char*)malloc(17) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message, 17,"%5d%11d", msg, intValue);              // 5 + 11 + 1
         #else
         sprintf(message, "%5d%11d", msg, intValue);              // 5 + 11 + 1
         #endif
      } else {
         int mess_len = 17+strlen(StrValue) ;
         message = (char*)malloc(mess_len) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)   // visual studio 2005 : deprecated function
         sprintf_s(message,mess_len, "%5d%11d%s" , msg, intValue, StrValue);  // 5 + 11 + xxx + 1
         #else
         sprintf(message, "%5d%11d%s", msg, intValue, StrValue);  // 5 + 11 + xxx + 1
         #endif
      }
      push_front (message) ;
      free (message) ;
  }
   
   //-------------------------------------------------------------------------
   /// back : code + int + int + int + string  
   void Add(const int msg, const int int1, const int int2 , const int int3 , const char * StrValue)
   {
      char * message ;

      if (StrValue == NULL)
      {
         message = (char*)malloc(39) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message,39, "%5d%11d%11d%11d", msg, int1, int2 , int3);               // 5 + 11 + 11 + 11 + 1
         #else
         sprintf(message, "%5d%11d%11d%11d", msg, int1, int2 , int3);               // 5 + 11 + 11 + 11 + 1
         #endif
      } else {
         int mess_len = 39+strlen(StrValue) ;
         message = (char*)malloc(mess_len) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message, mess_len, "%5d%11d%11d%11d%s", msg, int1, int2 , int3 , StrValue);  // 5 + 11 + 11 + 11 + xxx + 1
         #else
         sprintf(message, "%5d%11d%11d%11d%s", msg, int1, int2 , int3 , StrValue);  // 5 + 11 + 11 + 11 + xxx + 1
         #endif
      }

      push_back (message) ;
      free (message) ;
 }
   //-------------------------------------------------------------------------
   /// back : code + int + int + int + int + string  
   void Add(const int msg, const int int1, const int int2 , const int int3 , const int int4 , const char * StrValue)
   {
      char * message ;

      if (value == NULL)
      {
         message = (char*)malloc(50) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf(message, "%5d%11d%11d%11d%11d", msg, int1, int2 , int3, int4);                // 5 + 11 + 11 + 11 + 11 + 1
         #else
         sprintf(message, "%5d%11d%11d%11d%11d", msg, int1, int2 , int3, int4);                // 5 + 11 + 11 + 11 + 11 + 1
         #endif
      } else {
         int messLen = 50+strlen(StrValue) ;
         message = (char*)malloc(messLen) ;
         #if defined(_MSC_VER) && (_MSC_VER >= 1400) && (! UNDER_CE)  // visual studio 2005 : deprecated function
         sprintf_s(message,messLen, "%5d%11d%11d%11d%11d%s", msg, int1, int2 , int3 , int4, StrValue);   // 5 + 11 + 11 + 11 + 11 + xxx + 1
         #else
         sprintf(message, "%5d%11d%11d%11d%11d%s", msg, int1, int2 , int3 , int4, StrValue);   // 5 + 11 + 11 + 11 + 11 + xxx + 1
         #endif
      }

      push_back (message) ;
      free (message) ;           
   }

} ;

typedef CommandDeque<string> CommandList ;

//====================================================================================

/// <summary>
/// NodeContext is used internaly for the Indent and UnIndent functions.
/// </summary>

class NodeContext
{
public :
   DWORD threadId ;                        
   string nodeId ;                         
} ;

//====================================================================================

/// <summary>
/// Specify a font detail for traces columns items and members. 
/// </summary>

class FontDetail
{         
 public :
      int ColId  ;
      bool Bold  ;
      bool Italic ;
      int Color  ;    // -$7FFFFFFF-1..$7FFFFFFF;
      int Size   ;
      string FontName ;
} ;

//====================================================================================

/// <summary>
/// TMemberNode represent the informations displayed on the "Trace info panel" for a specific trace
/// </summary>

class TMemberNode
{
private :
   void _AddToStringList (CommandList * commands) ;
   deque <TMemberNode *> * m_Members ;          // sub members
   deque <FontDetail *> * m_FontDetails ;       // fonts
public :
   string col1 ;                                // Column 1
   string col2 ;                                // Column 2
   string col3 ;                                // Column 3
   int    tag ;                                 // User defined tag, NOT SEND to the viewer
   
   //-------------------------------------------------------------------------
   TMemberNode(const char    * strCol1 = NULL, const char    * strCol2 = NULL, const char    * strCol3 = NULL) ; // constructor
   TMemberNode(const wchar_t * strCol1       , const wchar_t * strCol2 = NULL, const wchar_t * strCol3 = NULL) ; // constructor

   ~TMemberNode() ;                                                                            // destructor
   deque <TMemberNode *> * Members() ;                                                         // sub members
   TMemberNode * Add (TMemberNode * member) ;                                                  // add a sub member
   TMemberNode * Add (const char    * strCol1, const char    * strCol2 = NULL, const char    * strCol3 = NULL);  // create and add a sub member
   TMemberNode * Add (const wchar_t * strCol1, const wchar_t * strCol2 = NULL, const wchar_t * strCol3 = NULL);  // create and add a sub member

   TMemberNode * SetFontDetail (const int ColId, const bool Bold , const bool Italic = false , const int Color = -1 , const int Size = 0 , const char * FontName = NULL) ;

   void AddToStringList (CommandList * commands) ;                                             // convert to command
} ;


//====================================================================================

/// <summary>
/// WinTrace represent a windows tree where you put traces
/// </summary>

class WinTrace
{
   friend TTrace ;   // TTrace use direct access to debug, warning and error
private :
   TraceNode * debug;
   TraceNode * warning;
   TraceNode * error;
   void init() ;
public :
   WinTrace (void) ;                                                 // constructor. Used to map to existing window
//   WinTrace (const char    * winTraceID , const char    * winTraceTitle)  ;      // constructor. Used to create a new window
   WinTrace (const wchar_t * winTraceID , const wchar_t * winTraceTitle)  ;      // constructor. Used to create a new window
   ~WinTrace(void) ;                                                 // destructor
   
   TraceNode    * Debug()       { return debug   ;} ;                // Debug Node
   TraceNode    * Warning()     { return warning ;} ;                // Warning node
   TraceNode    * Error()       { return error   ;} ;                // Error node
   string id ; 	                                                   // Wintrace id (empty for the main window)
   int    tag ;                                                      // User defined tag, NOT SEND to the viewer
	wstring id2;																		// Wintrace id as wchar string

   void SaveToTextfile  (const char    * FileName) ;                 // Save the window tree traces to a text file
   void SaveToTextfile  (const wchar_t * FileName) ;                 // Save the window tree traces to a text file
   void SaveToXml       (const char    * FileName) ;                 // Save the window tree traces to an XML file
   void SaveToXml       (const wchar_t * FileName) ;                 // Save the window tree traces to an XML file
   void LoadXml         (const char    * FileName) ;                 // Load an XML file to the window tree traces
   void LoadXml         (const wchar_t * FileName) ;                 // Load an XML file to the window tree traces
   void SetLogFile      (const char    * FileName, int Mode) ;       // Set the log file.(Path is relative to the viewer)
   void SetLogFile      (const wchar_t * FileName, int Mode) ;       // Set the log file.(Path is relative to the viewer)
   void DisplayWin      () ;                                         // Show the window tree
   void SetMultiColumn  (const int MainColIndex = 0) ;               // change the tree to display user defined multiple columns
   void SetColumnsTitle (const char    * Titles) ;                   // set columns title
   void SetColumnsTitle (const wchar_t * Titles) ;                   // set columns title
   void SetColumnsWidth (const char    * Widths) ;                   // set columns width
   void SetColumnsWidth (const wchar_t * Widths) ;                   // set columns width
   void ClearAll () ;                                                // Clear all trace for the window tree
   
   // Plugin API. not available on pocket PC
   #ifndef _WIN32_WCE
   void CreateResource  (const int ResId    , const int ResType ,                // Create a resource on the window
                         const int ResWidth , const char * ResText = NULL) ;   
   void CreateResource  (const int ResId    , const int ResType ,                // Create a resource on the window
                         const int ResWidth , const wchar_t * ResText = NULL) ;   
   void DisableResource (const int ResId) ;                                      // Disable tracetool or user created resources
   void SetTextResource (const int ResId, const char    * ResText) ;             // Set the resource text
   void SetTextResource (const int ResId, const wchar_t * ResText) ;             // Set the resource text
   void LinkToPlugin    (const char   * PluginName,  const int flags) ;          // Attach a winTrace to a plugin
   void LinkToPlugin    (const wchar_t * PluginName, const int flags) ;          // Attach a winTrace to a plugin
   #endif
} ;

//====================================================================================

/// <summary>
/// WinTrace represent a windows tree where you put watches
/// </summary>

class WinWatch
{
public :
   string id ; 	                                                   // Wintrace id (empty for the main window)
   bool   enabled ;                                                  // enable or disable watches
   int    tag ;                                                      // User defined tag, NOT SEND to the viewer

   WinWatch (void) ;
   WinWatch (const char    * WinWatchID , const char    * WinWatchText) ;
   WinWatch (const wchar_t * WinWatchID , const wchar_t * WinWatchText) ;
   void DisplayWin (void) ;
   void ClearAll (void) ;
   void Send (const char    * WatchName , const char    * WatchValue) ;
   void Send (const wchar_t * WatchName , const wchar_t * WatchValue) ;
} ;

//====================================================================================

/// <summary>
/// TTrace is the entry point for all traces.
/// TTrace give 3 'TraceNode' doors : Warning , Error and Debug.
/// Theses 3 doors are displayed with a special icon (all of them have the 'enabled' property set to true.
/// That class is fully static.
/// </summary>

class TTrace    
{
   friend TraceOptions ;
   friend WinTrace ;
   friend WinWatch ;
   friend TraceNodeEx ;
private:
   static DWORD              m_ClockSequenceBase ;
   static WORD               m_DeviceId1 ;
   static WORD               m_DeviceId2 ;
   static WORD               m_DeviceId3 ;       
   static WinTrace *         m_winTrace  ;
   static WinWatch *         m_winWatch ;
   static TraceOptions *     m_options ;
   static bool               m_IsSocketInitialized  ;
   static struct sockaddr_in m_serverSockAddr;    // Socket adress
   static int                m_socketHandle ;
   static CRITICAL_SECTION   criticalSection ;
   
   TTrace(void) {} ;   // private constructor. No instance allowed
   static void SendToClient(CommandList * Commands, const WinWatch * winWatch);   // send the trace to the viewer
   static void SendToClient(CommandList * Commands, const WinTrace * winTrace);   // send the trace to the viewer
   static void SendToClient(CommandList * Commands) ;                             // send the trace to the viewer
 
public:
   static void CloseSocket() ;                    // close viewer connection
   static char * CreateTraceID ();                // Creates unique GUID. 
   static char * WideToMbs (const wchar_t * WideStr, int indent = 0) ;  // helper function
   
   // helper functions
   static TraceNode    * Debug()       { return m_winTrace->debug ;};
   static TraceNode    * Warning()     { return m_winTrace->warning ;} ;
   static TraceNode    * Error()       { return m_winTrace->error ;} ;
   static TraceOptions * Options()     { return m_options ; } ;  
   static WinTrace     * WindowTrace() { return m_winTrace ; } ;    
   static WinWatch     * Watches()     { return m_winWatch ; } ;
   
   
   static void ClearAll ()  ;              // clear the main trace win
   static void Show (bool IsVisible) ;     // display the viewer
   
   // TODO : Thread Sub System
   // TODO : static void Flush (FlushTimeOut : integer = 5000);
   static void Stop () ; // Stop tracetool sub system. Must be called before exiting plugin
   // TODO : static void Start() ; // restart tracetool sub system if STOP was called
   
   static DWORD Init() ;
   
}  ;

//====================================================================================

/// <summary>
/// Classic Trace Node : allow you to send simple traces.
/// Use TraceNodeEx for more complex traces.
/// The 'Send' functions don't return any object.
/// </summary>

class TraceNode 
{
   friend TraceNodeEx ;
   friend WinTrace ;
public:
   char *     id ;                           // Node id
   bool       enabled ;                      // enable or disable traces
   WinTrace * winTrace ;                     // Owner
   int        tag ;                          // User defined tag, NOT SEND to the viewer   
	int			indent;

   TraceNode(const TraceNode * parentNode = NULL , const bool generateUniqueId = true);   // construct a new trace node, derived from a parent node 
   TraceNode(const TraceNode * parentNode , const char    * newNodeId );            // construct a new trace node, derived from a parent node 
   TraceNode(const TraceNode * parentNode , const wchar_t * newNodeId );            // construct a new trace node, derived from a parent node 

   virtual ~TraceNode(void) ;                                   // destructor

   TraceNodeEx * CreateChildEx (const char    *leftMsg = NULL, const char    *rightMsg = NULL) ;   // prepare minimal sub node without sending it 
   TraceNodeEx * CreateChildEx (const wchar_t *leftMsg = NULL, const wchar_t *rightMsg = NULL) ;   // prepare minimal sub node without sending it 
   //void Indent   (const TCHAR    *leftMsg, const TCHAR    *rightMsg  = NULL) ;                       // send a node based on this node then change the indentation
   //void UnIndent (const TCHAR    *leftMsg = NULL, const TCHAR *rightMsg  = NULL) ;                   // decrement indentation
   int  GetIconIndex (void) ;
	void Indent() { indent++; }
	void UnIndent() { ATLASSERT(indent); indent--; }

//   void Send (const char     *leftMsg, const char     *rightMsg = NULL) ;                  // The most usefull function in that library
   void Send (const wchar_t *wLeftMsg, const wchar_t *wRightMsg = NULL) ;   
   
   //void SendDump (const char    *leftMsg, const char    *rightMsg, const char    * title, const char * memory, const unsigned byteCount) ;  // send a dump to the viewer
   void SendDump (const wchar_t *leftMsg, const wchar_t *rightMsg, const wchar_t * title, const char * memory, const unsigned byteCount) ;  // send a dump to the viewer
  //TraceNode * SetFontDetail (const int ColId, const bool Bold , const bool Italic = false , const int Color = -1 , const int Size = 0 , const char * FontName = NULL) ;

protected :
   CRITICAL_SECTION criticalSection ;                                          // protect Indent and UnIndent functions
   int iconIndex ;                                                             // icon index
   deque <NodeContext *> * contextList ;                                       // context list
   const char * GetLastContextId() ;                                           // get the last context for the current thread
   void PushContextId (const char * contextId) ;                               // save the current context 
   void deleteLastContext (void) ;                                             // delete the last context for the current thread
};

//====================================================================================

/// <summary>
/// TraceNodeEx allow you to construct trace node, adding 'members' before seding it.
/// don't forget to free node.
/// </summary>

class TraceNodeEx : public TraceNode //Base 
{
   friend TTrace ;                         // TTrace add command and process members before sending it.
   friend TraceNode ;                      // TraceNode add command 
public:
   string rightMsg;                        // right message
   string leftMsg;                         // left message
   
   TraceNodeEx (TraceNode * parentNode = NULL , const bool generateUniqueId = true) ;     // constructor
   TraceNodeEx (TraceNode * parentNode , const char    * newNodeId ) ;                    // constructor
   TraceNodeEx (TraceNode * parentNode , const wchar_t * newNodeId ) ;                    // constructor
   virtual ~TraceNodeEx(void) ;                                                                 // and destructor                   
   
   TMemberNode * Members() ;                                                                    // members info
   
   TraceNodeEx * AddDump(const char    * Title , const char * memory  , const unsigned index , const unsigned byteCount) ;   // add dump to the members info
   TraceNodeEx * AddDump(const wchar_t * Title , const char * memory  , const unsigned index , const unsigned byteCount) ;   // add dump to the members info
   TraceNodeEx * AddFontDetail(const int ColId,  const bool Bold ,  const bool Italic = false,  const int Color = -1 , const int Size = 0 ,  const char * FontName = NULL) ; 

   void SetIconIndex (const int newInconIndex) ;
   void Send (void) ;                                                                // send the node
//   void Send (const char    *leftMsg, const char    *rightMsg  = NULL) ;             // The most usefull function in that library
   void Send (const wchar_t *leftMsg, const wchar_t *rightMsg  = NULL) ;             // Wide version. Note : don't mix char * and wchar_t *

   //void Resend (const char    *LeftMsg, const char    *RightMsg = NULL) ;            // resend left and right traces
   void Resend (const wchar_t *LeftMsg, const wchar_t *RightMsg = NULL) ;            // resend left and right traces
   //void Append (const char    *LeftMsg, const char    *RightMsg = NULL) ;            // append left and right traces
   void Append (const wchar_t *LeftMsg, const wchar_t *RightMsg = NULL) ;            // append left and right traces
   void Show () ;                                                        // ensure the trace is visible in the viewer
   void SetSelected() ;                                                  // set the node as selected
   void Delete() ;                                                       // delete the node and all children
   void DeleteChildren() ;                                               // delete the trace children, not the node itself

   // SetIconIndex (int)
   
protected:
   
   TMemberNode * m_Members ;               // node members
   CommandList * Commands ;                // Commands to send
   deque <FontDetail *> * m_FontDetails ;       // fonts
} ;

//====================================================================================

/// <summary>
/// Determine how to send the trace : windows message or socket (for windows Ce for example)
/// </summary>

enum SendMode
{                                  
   WinMsg=1,                               // Windows message
      Socket                                  // Socket message
} ;

//====================================================================================

/// <summary>
/// TTrace options
/// </summary>

class TraceOptions
{
   friend TTrace ;                         // TTrace use the process file name
   friend TraceNodeEx ;                    // TraceNodeEx get TTrace options
protected :
   char * m_processFileName ;              // Process name
   char * socketHost;                      // socket host (name or ip)
   TraceOptions(void) ;                    // constructor
   ~TraceOptions(void) ;                   // and destructor
public : 
   
   SendMode sendMode ;                     // WinMsg or Socket
   int      socketPort ;                   // socket port 
   
   bool     SendProcessName ;                    // if true the process name is send
   const char * CheckProcessName() ;             // helper function : return the process name
   void     SetSocketHost (const char * Host) ;  // set the socket Host
   void     SetSocketHost (const wchar_t * Host); 
} ;


//====================================================================================
#pragma warning(pop)
