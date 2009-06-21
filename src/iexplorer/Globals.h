#pragma once

class CLoggerModel;
class CConsoleWindow;
class CXRefreshBHO;
class CXRefreshHelperbar;
class CBrowserManager;

typedef unsigned int                              TFrameId;
const TFrameId                                    NULL_FRAME = 0;
typedef unsigned int                              TBrowserId;
const TBrowserId                                  NULL_BROWSER = 0;
typedef LONG                                      THandle;
const THandle                                     NULL_HANDLE = 0;
typedef unsigned long                             TWindowId;
const TWindowId                                   NULL_WINDOW = 0;

// messages
const int IDC_TOOLBUTTON                          = 1;
const int IDC_DISABLEBUTTON                       = 2;

#define PRODUCT_ID_TEXT                           _T("44386A7B-7093-4FDC-8B52-5F7E8B968960")
#define IS_BETA                                   1

// system script IDs
#define REGISTRY_ROOT_KEY                         _T("Software\\XRefresh\\")
#define REGISTRY_SETTINGS_KEY                     _T("IEAddon")
#define REGISTRY_ROOT_KEY_SITES                   REGISTRY_ROOT_KEY REGISTRY_SETTINGS_KEY _T("\\Sites")
#define REGISTRY_ROOT_KEY_SITES_PARENT            REGISTRY_ROOT_KEY REGISTRY_SETTINGS_KEY
#define REGISTRY_SETTINGS_HOST                    _T("Host")
#define REGISTRY_SETTINGS_PORT                    _T("Port")
#define REGISTRY_SETTINGS_RANGE                   _T("Range")
#define REGISTRY_OPTIONS_KEY                      _T("Options")

#define DOMEXPLORERTREE_CLASS_NAME                _T("CXRefreshDOMExplorerTree")
#define DOMEXPLORERTREE_WINDOW_NAME               _T("XRefreshDOMExplorerTreeWindow")

#define DOMEXPLORER_CLASS_NAME                    _T("CXRefreshDOMExplorer")
#define DOMEXPLORER_WINDOW_NAME                   _T("XRefreshDOMExplorerWindow")

#define CONSOLE_WINDOW_NAME                       _T("ConsoleWindow")

#define MAIN_TOOLBAR_CLASS_NAME                   _T("CXRefreshToolbar")
#define MAIN_TOOLBAR_WINDOW_NAME                  _T("XRefreshToolbarWindow")

#define HELPERBAR_WORKSPACE_CLASS_NAME            _T("CXRefreshHelperbarWorkspace")
#define HELPERBAR_WORKSPACE_WINDOW_NAME           _T("XRefreshHelperbarWorkspaceWindow")

#define HELPERBAR_MAIN_CLASS_NAME                 _T("CXRefreshHelperbarMain")
#define HELPERBAR_MAIN_WINDOW_NAME                _T("XRefreshHelperbarMainWindow")

#define HELPERBAR_CLASS_NAME                      _T("CXRefreshHelperbar")
#define HELPERBAR_WINDOW_NAME                     _T("XRefreshHelperbarWindow")

#define DOMTREE_WINDOW_NAME                       _T("XRefreshDOMTreeWindow")
#define DOMTREE_CLASS_NAME                        _T("CXRefreshDOMTree")

#define CONSOLE_LIST_WINDOW_NAME                  _T("XRefreshConsoleListWindow")
#define CONSOLE_LIST_CLASS_NAME                   _T("CXRefreshConsoleList")

#define SITES_LIST_WINDOW_NAME                    _T("XRefreshSitesListWindow")
#define SITES_LIST_CLASS_NAME                     _T("CXRefreshSitesList")

#define CONSOLE_WINDOW_CLASS_NAME                 _T("CXRefreshConsoleWindow")

#define MESSAGE_WINDOW_WINDOW_NAME                _T("XRefreshMessageWindow")
#define MESSAGE_WINDOW_CLASS_NAME                 _T("CXRefreshMessageWindow")

#define BROWSER_MESSAGE_WINDOW_NAME               _T("XRefreshBrowserMessageWindow")
#define BROWSER_MESSAGE_WINDOW_CLASS_NAME         _T("CXRefreshBrowserMessageWindow")

#define WAIT_GRANULARITY                          20 // in ms
#define ALLOWED_SITES_HELP_LINK                   _T("http://xrefresh.binaryage.com")

#define MAX_LOGGER_MESSAGES                       200