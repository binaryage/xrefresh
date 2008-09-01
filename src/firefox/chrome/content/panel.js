// Thanks to:
// Christoph Dorn <christoph@christophdorn.com>
// Shane Caraveo
//////////////////////////////////////////////////////////////////
// For communication protocol between extension and server
// we use UTF-8 encoded JSON to exchange messages
//
// This source contains copy&pasted various bits from Firebug sources.
// open custom scope
FBL.ns(function() {
    with(FBL) {

        const nsIPrefBranch = Ci.nsIPrefBranch;
        const nsIPrefBranch2 = Ci.nsIPrefBranch2;

        const xrefreshPrefService = Cc["@mozilla.org/preferences-service;1"];

        const xrefreshPrefs = xrefreshPrefService.getService(nsIPrefBranch2);
        const xrefreshURLs =
        {
            main: "http://www.xrefresh.com",
            docs: "http://www.xrefresh.com/docs",
            contribute: "http://www.xrefresh.com/contribute"
        };

        const xrefreshPrefDomain = "extensions.xrefresh";

        // see http://www.xulplanet.com/tutorials/mozsdk/sockets.php
        var drumTransport = null;
        var drumOutStream = null;
        var drumInStream = null;
        var drumPump = null;

        var drumReady = false;

        var drumName = "";
        var drumVersion = "";
        var drumInitiatedRefresh = false;

        // see http://www.xulplanet.com/tutorials/mozsdk/serverpush.php
        var listenerServer = null;

        var recorders = {};

        var readyStateTimer = null;
        var xrefreshOptionUpdateMap = {};

        if (Firebug.TraceModule)
        {
            Firebug.TraceModule.DBG_XREFRESH = false;
            var type = xrefreshPrefs.getPrefType('extensions.firebug.DBG_XREFRESH');
            if (type != nsIPrefBranch.PREF_BOOL) try {
                xrefreshPrefs.setBoolPref('extensions.firebug.DBG_XREFRESH', false);
            } catch(e) {}
            Firebug.TraceModule.DBG_CASPER = false;
            var type = xrefreshPrefs.getPrefType('extensions.firebug.DBG_CASPER');
            if (type != nsIPrefBranch.PREF_BOOL) try {
                xrefreshPrefs.setBoolPref('extensions.firebug.DBG_CASPER', false);
            } catch(e) {}
        }

        function debugLog() {
            if (FBTrace && FBTrace.DBG_XREFRESH) FBTrace.sysout.apply(this, arguments);
        }

        FBL.$XREFRESH_STR = function(name)
        {
            return document.getElementById("strings_xrefresh").getString(name);
        };
        FBL.$XREFRESH_STRF = function(name, args)
        {
            return document.getElementById("strings_xrefresh").getFormattedString(name, args);
        };

        ////////////////////////////////////////////////////////////////////////
        // Firebug.XRefreshExtension, here we go!
        //
        Firebug.XRefreshExtension = extend(Firebug.ActivableModule,
        {
            reminder: '',
            /////////////////////////////////////////////////////////////////////////////////////////
            checkFirebugVersion: function()
            {
                var version = Firebug.getVersion();
                if (!version) return false;
                var a = version.split('.');
                if (a.length<2) return false;
                // we want Firebug version 1.2+ (including alphas/betas and other weird stuff)
                return parseInt(a[0], 10)>=1 && parseInt(a[1], 10)>=2;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getPrefDomain: function()
            {
                return xrefreshPrefDomain;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            initialize: function()
            {
                this.panelName = 'XRefreshExtension';
                this.description = "External browser refresh";

                Firebug.ActivableModule.initialize.apply(this, arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            shutdown: function()
            {
                if (Firebug.getPref('defaultPanelName') == 'XRefreshExtension') {
                    Firebug.setPref('defaultPanelName', 'console');
                }
                Firebug.ActivableModule.shutdown.apply(this, arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onPanelActivate: function(context, init)
            {
                if (!init)
                context.window.location.reload();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onFirstPanelActivate: function(context, init)
            {
                // Just before onPanelActivate, no previous activecontext
                // Note: connection at this point was buggy.
                // I didn't find out the reason. It seems delayed connection works well.
                if (this.scheduledDisconnection) clearTimeout(this.scheduledDisconnection);
                this.scheduledDisconnection = undefined;
                if (this.alreadyActivated) return;
                this.alreadyActivated = true;
				
                setTimeout(bind(this.startupCheck, this), 1000);
                setTimeout(bind(this.connectDrum, this), 1000);
                setTimeout(bind(this.startListener, this), 2000);
                this.checkTimeout = setTimeout(bind(this.connectionCheck, this), 5000);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            startupCheck: function()
            {
				if (!this.checkFirebugVersion())
				{
	                this.log("XRefresh Firefox extension works best with Firebug 1.2 or higher. Please upgrade Firebug to latest version.", "warn");
				}
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onLastPanelDeactivate: function(context, init)
            {
                if (this.scheduledDisconnection) clearTimeout(this.scheduledDisconnection);
                this.scheduledDisconnection = setTimeout(bind(this.disconnect, this), 10000);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            disconnect: function()
            {
                this.scheduledDisconnection = undefined;
                if (!this.alreadyActivated) return;
                this.alreadyActivated = undefined;

                // just after onPanelDeactivate, no remaining activecontext
                this.disconnectDrum();
                this.stopListener();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            connectionCheck: function()
            {
				this.checkTimeout = undefined;
                if (drumReady) return;
                this.log("Unable to connect to XRefresh Monitor", "warn");
                this.log("Please check if you have running XRefresh Monitor.", "bulb");
                this.log("    On Windows, it is program running in system tray. Look for Programs -> XRefresh -> XRefresh.exe", "bulb");
                this.log("    On Mac, it is running command-line program xrefresh-server. It should be available on system path after 'sudo gem install xrefresh-server'", "bulb");
                this.log("You may also want to check your firewall settings. XRefresh Firefox extension expects Monitor to talk from " + this.getPref('host') + " on port " + this.getPref('port'), "bulb");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            shortConnectionCheck: function()
            {
				this.shortCheckTimeout = undefined;
                if (drumReady) return;
                this.log("Unable to connect to XRefresh Monitor. Please check if you have running XRefresh Monitor and tweak your firewall settings if needed.", "warn");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            destroyAllRecorders: function()
            {
                for (var r in recorders)
                {
                    if (recorders.hasOwnProperty(r)) recorders[r].stop();
                }
                recorders = {};
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showPanel: function(browser, panel)
            {
                Firebug.ActivableModule.showPanel.apply(this, arguments);
                var isXRefreshExtension = panel && panel.name == "XRefreshExtension";
                if (isXRefreshExtension) this.updatePanel();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onOptionsShowing: function(popup)
            {
                for (var child = popup.firstChild; child; child = child.nextSibling)
                {
                    if (child.localName == "menuitem")
                    {
                        var option = child.getAttribute("option");
                        if (option)
                        {
                            var checked = false;
                            checked = this.getPref(option);
                            child.setAttribute("checked", checked);
                        }
                    }
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            visitWebsite: function(which)
            {
                openNewTab(xrefreshURLs[which]);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getPref: function(name)
            {
                var prefName = xrefreshPrefDomain + "." + name;

                var type = xrefreshPrefs.getPrefType(prefName);
                if (type == nsIPrefBranch.PREF_STRING)
                return xrefreshPrefs.getCharPref(prefName);
                else if (type == nsIPrefBranch.PREF_INT)
                return xrefreshPrefs.getIntPref(prefName);
                else if (type == nsIPrefBranch.PREF_BOOL)
                return xrefreshPrefs.getBoolPref(prefName);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            setPref: function(name, value)
            {
                var prefName = xrefreshPrefDomain + "." + name;

                var type = xrefreshPrefs.getPrefType(prefName);
                if (type == nsIPrefBranch.PREF_STRING)
                xrefreshPrefs.setCharPref(prefName, value);
                else if (type == nsIPrefBranch.PREF_INT)
                xrefreshPrefs.setIntPref(prefName, value);
                else if (type == nsIPrefBranch.PREF_BOOL)
                xrefreshPrefs.setBoolPref(prefName, value);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onToggleOption: function(menuitem)
            {
                var option = menuitem.getAttribute("option");
                var checked = menuitem.getAttribute("checked") == "true";

                this.setPref(option, checked);
				this.updatePanel();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            installGate: function(context)
            {
                debugLog("installed gate for>" + context.window.document.URL);

                // the gate is here to stop propagation of some events
                // beyond HTML document borders.
                var doc = context.chrome.window.document;

                // remove all "keyset" elements to disable keyboard shortcuts
                // note: i've tried to set disabled attribute to keys => didn't work
                //       i've tried to remove keys from keysets => didn't work
                var keys = doc.getElementsByTagName("keyset");
                this.savedKeys = [];
                this.savedParents = [];
                for (i = 0; i < keys.length; i++)
                {
                    debugLog("keyset: " + keys[i].id);
                    this.savedKeys[i] = keys[i];
                    this.savedParents[i] = keys[i].parentNode;
                }

                for (j = 0; j < this.savedKeys.length; j++)
                {
                    debugLog("key: " + this.savedKeys[j].id + " ?");
                    this.savedParents[j].removeChild(this.savedKeys[j]);
                }

                // TODO: would be nice to insert placeholders for keysets
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            uninstallGate: function(context)
            {
                debugLog("uninstalling gate for>" + context.window.document.URL);

                this.restorePageOffset(context);

                // restore keyset elements
                for (i = 0; i < this.savedKeys.length; i++)
                {
                    this.savedParents[i].appendChild(this.savedKeys[i]);
                    debugLog("restored key: " + this.savedKeys[i].id + "! into: " + this.savedParents[i].id);
                }

                // start recording new session
                var recorder = this.getRecorder(context);
                if (recorder.state != "replaying") this.error("Recorder is in bad state.");
                recorder.restoreState(context.window);

                this.updateRecorderPanel();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            printRecordersStats: function()
            {
				if (!(FBTrace && FBTrace.DBG_XREFRESH)) return;
                this.log("Recorders:");
                for (var r in recorders)
                {
                    if (recorders.hasOwnProperty(r))
                    {
                        this.log("  " + r + " >> " + recorders[r].getStats() + " " + recorders[r].state + " C=" + recorders[r].destroyMarker);
                    }
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getRecorder: function(context)
            {
                return recorders[context.window.document.URL];
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            destroyRecorder: function(url, counter)
            {
                var recorder = recorders[url];
                if (recorder && recorder.destroyMarker == counter)
                {
                    debugLog("destroying recorder due to timeout [" + (counter + "") + "] >" + url);
                    delete recorders[url];
                    this.printRecordersStats();
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            documentStateWatcher: function(context) {
                },
            /////////////////////////////////////////////////////////////////////////////////////////
            initContext: function(context)
            {
                Firebug.ActivableModule.initContext.apply(this, arguments);
                debugLog("init context>" + context.window.document.URL);
                this.printRecordersStats();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            reattachContext: function(context)
            {
                Firebug.ActivableModule.reattachContext.apply(this, arguments);
                debugLog("reattach context>" + context.window.document.URL);
                this.printRecordersStats();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            destroyContext: function(context, persistedState)
            {
                Firebug.ActivableModule.destroyContext.apply(this, arguments);
                if (!this.isEnabled(context)) return;
                var panel = FirebugContext.getPanel("XRefreshExtension");
                debugLog("destroy context>" + context.window.document.URL);

                // we need to destroy recorders to prevent leaks
                // we will defer recorder destroy for recorderKeepAlive time
                // if there is activity on him (revisit) I won't destroy it
                //
                // we need to do this, because we get this order during refresh:
                // destroyContext http://somepage (-> defer recorder destroy)
                // loadContext http://somepage (-> cancel recorder destroy)
                // * here we don't destroy http://somepage because it was revisited
                //
                // this is page switch scenario:
                // destroyContext http://somepage (-> defer recorder destroy)
                // loadContext http://someotherpage (-> do nothing)
                // * here we destroy http://somepage because it was NOT revisited
                var recorder = recorders[context.window.document.URL];
                if (recorder)
                {
                    var counter = recorder.destroyMarker;
                    setTimeout(bindFixed(this.destroyRecorder, this, context.window.document.URL + "", counter), this.getPref('recorderKeepAlive'));
                    if (recorder.state == "replaying")
                    {
                        recorder.stopReplay();
                        recorder.restoreState(context.window);
                    }
                }

                this.printRecordersStats();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showContext: function(browser, context)
            {
                Firebug.ActivableModule.showContext.apply(this, arguments);
                debugLog("show context>" + context.window.document.URL);
                this.updatePanel();
                this.printRecordersStats();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            loadedContext: function(context)
            {
                Firebug.ActivableModule.loadedContext.apply(this, arguments);
                if (!this.isEnabled(context)) return;

                var recorder = this.getRecorder(context);
                var nextDestroyMarker = 0;
                if (recorder) nextDestroyMarker = recorder.destroyMarker + 1;
                // visiting old page prevent destroying recorder if found
                debugLog("loaded context>" + context.window.document.URL);
                this.sendSetPage(context.browser.contentTitle, context.window.document.URL);

                if (drumInitiatedRefresh)
                {
                    debugLog("drum initiated refresh>" + context.window.document.URL);
                    drumInitiatedRefresh = false;
                    if (recorder && recorder.state != "stopped")
                    {
                        this.log("Replaying recorded macros ...", "rreplay");
                        this.replayRecorder(context);
                    }
                }
                else
                {
                    debugLog("not drum initiated refresh>" + context.window.document.URL);
                    var wasPaused = recorder ? recorder.state == "paused": false;
                    var wasRecording = recorder ? recorder.state == "recording": this.getPref("enableRecorder");

                    // create new recorder
                    recorder = new Casper.Events.recorder(bind(this.updateRecorderPanel, this));
                    debugLog("created recorder>" + recorder);
                    for (var eventName in Casper.Events.handler)
                    {
                        recorder.listener.addListener(eventName);
                    }

                    // register recorder
                    debugLog("starting recorder>" + recorder);
                    if (wasRecording) recorder.start(context.window, wasPaused);
                    debugLog("registering recorder as>" + context.window.document.URL);
                }

                recorder.destroyMarker = nextDestroyMarker;
                recorders[context.window.document.URL] = recorder;

                this.printRecordersStats();
                this.updateRecorderPanel();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonRefresh: function()
            {
                FirebugContext.getPanel("XRefreshExtension").refresh(FirebugContext);
                this.log("Manual refresh performed by user", "rreq");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonStatus: function()
            {
				if (this.checkTimeout) clearTimeout(this.checkTimeout);
				this.checkTimeout = undefined;
				if (this.shortCheckTimeout) clearTimeout(this.shortCheckTimeout);
				this.shortCheckTimeout = undefined;
                if (drumReady)
                {
                    this.log("Disconnection requested by user", "disconnect_btn");
                    this.disconnectDrum();
                }
                else
                {
                    this.log("Connection requested by user", "connect_btn");
                    this.shortCheckTimeout = setTimeout(bind(this.shortConnectionCheck, this), 3000);
                    this.connectDrum();
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonStopRecorder: function()
            {
                var currentRecorder = this.getRecorder(FirebugContext);
                if (!currentRecorder) return;
                currentRecorder.stop(FirebugContext.window);
                this.updateRecorderPanel();
                this.log("Macro Recorder has been stopped by user", "rstop");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonStartRecorder: function()
            {
                var currentRecorder = this.getRecorder(FirebugContext);
                if (!currentRecorder) return;
                currentRecorder.start(FirebugContext.window);
                this.updateRecorderPanel();
                this.log("Macro Recorder has been started by user", "rstart");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replayRecorder: function(context)
            {
                var recorder = this.getRecorder(context);
                if (!recorder) return;
                if (!recorder.currentTest) return;
                if (recorder.state == "stopped") return;
                if (recorder.state == "replaying")
                {
                    this.error("Cannot replay macro, recorder is in replay state.");
                    return;
                }

                recorder.saveState();
                recorder.pause(context.window);
                this.installGate(context);
                recorder.currentTest.complete = bindFixed(this.uninstallGate, this, context);
                recorder.replay(context.window);
                // the replay is asynchronous !!!
                this.updateRecorderPanel();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonReplayRecorder: function()
            {
                this.replayRecorder(FirebugContext);
                this.log("Macro Recorder has been replayed by user", "rreplay");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonPauseRecorder: function()
            {
                var currentRecorder = this.getRecorder(FirebugContext);
                if (!currentRecorder) return;

                var panel = FirebugContext.getPanel("XRefreshExtension");
                if (!panel) return;
                var browser = panel.context.browser;
                if (!browser) return;
                var buttonPause = browser.chrome.$("fbXRefreshExtensionRecorderPause");
                if (!hasClass(buttonPause, "paused"))
                {
                    currentRecorder.pause(FirebugContext.window);
                }
                else
                {
                    currentRecorder.unpause(FirebugContext.window);
                }

                this.updateRecorderPanel();
                this.log("Macro Recorder has been paused by user", "rpause");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            connectDrum: function()
            {
                var transportService = Cc["@mozilla.org/network/socket-transport-service;1"].getService(Components.interfaces.nsISocketTransportService);
                drumTransport = transportService.createTransport(null, 0, this.getPref("host"), this.getPref("port"), null);
                drumOutStream = drumTransport.openOutputStream(0, 0, 0);

                var stream = drumTransport.openInputStream(0, 0, 0);
                drumInStream = Cc["@mozilla.org/scriptableinputstream;1"].createInstance(Components.interfaces.nsIScriptableInputStream);
                drumInStream.init(stream);

                var dataListener = {
                    parent: this,
                    data: "",
                    onStartRequest: bind(function(request, context) {},
                    this),
                    onStopRequest: function(request, context, status) {
                        this.parent.onServerDied();
                    },
                    onDataAvailable: function(request, context, inputStream, offset, count) {
                        this.data += drumInStream.read(count);
                        this.parent.onDataAvailable(this);
                    }
                };

                drumPump = Components.classes["@mozilla.org/network/input-stream-pump;1"].createInstance(Components.interfaces.nsIInputStreamPump);
                drumPump.init(stream, -1, -1, 0, 0, false);
                drumPump.asyncRead(dataListener, null);

                this.sendHello();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            disconnectDrum: function()
            {
                if (drumReady) this.log("Disconnected from XRefresh Monitor", "disconnect");
                // it is nice to say good bye ...
                this.sendBye();
                if (drumInStream)
                {
                    drumInStream.close();
                    drumInStream = null;
                }
                if (drumOutStream)
                {
                    drumOutStream.close();
                    drumOutStream = null;
                }
                drumReady = false;
                this.updateConnectionPanel();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            reconnectDrum: function()
            {
                this.disconnectDrum();
                this.connectDrum();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            startListener: function()
            {
                var server = Components.classes["@mozilla.org/network/server-socket;1"].
                createInstance(Components.interfaces.nsIServerSocket);

                var listener =
                {
                    onSocketAccepted: bind(function(socket, transport)
                    {
                        this.log("Reconnection request received");
                        Firebug.XRefreshExtension.reconnectDrum();
                    },
                    this),

                    onStopListening: function(serverSocket, status) {}
                }

                var i;
                var range = this.getPref("portRange");
                var port = this.getPref("port");
                var loopbackonly = this.getPref("localConnectionsOnly");
                for (i = 1; i <= range; i++)
                {
                    // find some free port below drumPort
                    try {
                        server.init(port - i, loopbackonly, -1);
                        server.asyncListen(listener);
                        listenerServer = server;
                        return;
                    }
                    catch(e) {}
                }
                // it seems, no port is available
                this.error("No unused port available in given range: " + (port - range) + "-" + (port - 1));
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            stopListener: function()
            {
                if (!listenerServer) return;
                listenerServer.close();
                listenerServer = null;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendMessageToServer: function(message)
            {
                if (!drumOutStream) return false;
                var data = Casper.JSON.stringify(message) + '\n';
                // every message is delimited with new line
                var utf8data = UTF8.encode(data);
                drumOutStream.write(utf8data, utf8data.length);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onServerDied: function()
            {
                if (drumReady) this.error("Monitor has closed connection");

                if (drumInStream)
                {
                    drumInStream.close();
                    drumInStream = null;
                }
                if (drumOutStream)
                {
                    drumOutStream.close();
                    drumOutStream = null;
                }
                drumReady = false;
                this.updateConnectionPanel();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onDataAvailable: function(listener)
            {
                // try to parse incomming message
                // here we expect server to send always valid stream of {json1}\n{json2}\n{json3}\n...
                // TODO: make this more robust to server formating failures
                debugLog(data);
                var data = listener.data;
                var parts = listener.data.split('\n');
                var buffer = this.reminder;
                for (var i = 0; i < parts.length; i++) {
                    buffer += UTF8.decode(parts[i]);
                    if (FBTrace && FBTrace.DBG_XREFRESH) FBTrace.dumpProperties("  buffer:", buffer);

                    var message = Casper.JSON.parse(buffer);
                    if (!message) continue;
                    // we have only partial buffer? go for next chunk
                    buffer = '';
                    // message completed, clear buffer for incomming data
                    if (FBTrace && FBTrace.DBG_XREFRESH) FBTrace.dumpProperties("    message:", message);
                    try {
                        this.processMessage(message);
                    } catch(e) {
                        this.error("Error in processing message from XRefresh Monitor");
                    }
                }
                this.reminder = buffer;
                listener.data = "";
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getMessageCSSFiles: function(message)
            {
                var files = [];
                var re = /\.css$/;
                for (var i = 0; i < message.files.length; i++)
                {
                    var file = message.files[i];
                    if (file.action == 'changed')
                    {
                        if (file.path1.match(re))
                        {
                            files.push(message.root + '\\' + file.path1);
                            // TODO: this should be platform specific
                        }
                    }
                }
                return files;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            processMessage: function(message)
            {
                debugLog("Received message:" + message.command);
                if (message.command == "DoRefresh")
                {
                    var panel = FirebugContext.getPanel("XRefreshExtension");
                    if (this.getPref("fastCSS"))
                    {
                        var cssFiles = this.getMessageCSSFiles(message);
                        if (cssFiles.length == message.files.length)
                        // contains only CSS files
                        {
                            this.showEvent(message, 'fastcss');
                            return panel.updateCSS(FirebugContext, cssFiles);
                        }
                    }
                    this.showEvent(message, 'refresh');
                    panel.refresh(FirebugContext);
                }
                if (message.command == "AboutMe")
                {
                    drumReady = true;
                    drumName = message.agent;
                    drumVersion = message.version;
                    this.log("Connected to " + drumName + " " + drumVersion, "connect");
                    this.updateConnectionPanel();
                    this.sendSetPage(FirebugContext.browser.contentTitle, FirebugContext.window.document.URL);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updatePanel: function()
            {
                this.updateRefreshPanel();
                this.updateRecorderPanel();
                this.updateConnectionPanel();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateConnectionPanel: function()
            {
                if (!FirebugContext) return;
                // safety net
                var panel = FirebugContext.getPanel("XRefreshExtension");
                if (!panel) return;
                var browser = panel.context.browser;
                if (!browser) return;
                var buttonStatus = browser.chrome.$("fbXRefreshExtensionButtonStatus");
                if (!buttonStatus) return;
                buttonStatus.className = "toolbar-text-button toolbar-connection-status";

                if (drumReady)
                {
                    buttonStatus.label = "Connected to " + drumName + " " + drumVersion;
                    setClass(buttonStatus, "toolbar-text-button toolbar-status-connected");
                }
                else
                {
                    buttonStatus.label = "Disconnected";
                    setClass(buttonStatus, "toolbar-text-button toolbar-status-disconnected");
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateRefreshPanel: function()
            {
                if (!FirebugContext) return;
                // safety net
                var panel = FirebugContext.getPanel("XRefreshExtension");
                if (!panel) return;
                var browser = panel.context.browser;
                if (!browser) return;
                var buttonRefresh = browser.chrome.$("fbXRefreshExtensionRefresh");
                if (!buttonRefresh) return;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateRecorderPanel: function()
            {
                if (!FirebugContext) return;
                var enabled = this.isEnabled(FirebugContext);
				var enabledRecorder = this.getPref("enableRecorder");
                // safety net
                var panel = FirebugContext.getPanel("XRefreshExtension");
                if (!panel) return;
                panel.showToolbarButtons("fbXRefreshExtensionRecorder", enabled && enabledRecorder);
                var browser = panel.context.browser;
                if (!browser) return;
                var status = browser.chrome.$("fbXRefreshExtensionRecorderStatus");
                if (!status) return;
                var statusLabel = browser.chrome.$("fbXRefreshExtensionRecorderStatusLabel");
                if (!statusLabel) return;
				var buttonStop = browser.chrome.$("fbXRefreshExtensionRecorderStop");
                var buttonStart = browser.chrome.$("fbXRefreshExtensionRecorderStart");
                var buttonReplay = browser.chrome.$("fbXRefreshExtensionRecorderReplay");
                var buttonPause = browser.chrome.$("fbXRefreshExtensionRecorderPause");
                statusLabel.className = "toolbar-recorder-status";

                var currentRecorder = this.getRecorder(FirebugContext);
                if (!currentRecorder)
                {
                    statusLabel.value = "Waiting for document ...";
                    setClass(statusLabel, "toolbar-recorder-invalid");

                    buttonStart.disabled = true;
                    buttonStop.disabled = true;
                    buttonPause.disabled = true;
                    buttonReplay.disabled = true;
                    return;
                }
                var stats = currentRecorder.getStats();

                removeClass(buttonPause, "paused");
                switch (currentRecorder.state)
                {
                case "recording":
                    statusLabel.value = stats + " Recording ... ";
                    setClass(statusLabel, "toolbar-recorder-recording");

                    buttonStart.disabled = true;
                    buttonStop.disabled = false;
                    buttonPause.disabled = false;
                    buttonReplay.disabled = false;
                    break;

                case "paused":
                    setClass(buttonPause, "paused");
                    statusLabel.value = stats + " Paused ";
                    setClass(statusLabel, "toolbar-recorder-paused");

                    buttonStart.disabled = true;
                    buttonStop.disabled = false;
                    buttonPause.disabled = false;
                    buttonReplay.disabled = false;
                    break;

                case "stopped":
                    statusLabel.value = stats + " Stopped";
                    setClass(statusLabel, "toolbar-recorder-stopped");

                    buttonStart.disabled = false;
                    buttonStop.disabled = true;
                    buttonPause.disabled = true;
                    buttonReplay.disabled = true;
                    break;

                case "replaying":
                    statusLabel.value = stats + " Replaying ... ";
                    setClass(statusLabel, "toolbar-recorder-replaying");

                    buttonStart.disabled = true;
                    buttonStop.disabled = true;
                    buttonPause.disabled = true;
                    buttonReplay.disabled = true;
                    break;
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showEvent: function(message, icon)
            {
                message.time = this.getCurrentTime();
                var event = new BlinkEvent(0, message, icon);
                return this.publishEvent(event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showLog: function(text, icon)
            {
                var event = new BlinkEvent(1, {
                    text: text,
                    time: this.getCurrentTime()
                },
                icon);
                return this.publishEvent(event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getCurrentTime: function()
            {
                var d = new Date();
                var h = d.getHours() + "";
                var m = d.getMinutes() + "";
                var s = d.getSeconds() + "";
                while (h.length < 2) h = "0" + h;
                while (m.length < 2) m = "0" + m;
                while (s.length < 2) s = "0" + s;
                return h + ":" + m + ":" + s;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            log: function(text, icon)
            {
                if (!icon) icon = "info";
                return this.showLog(text, icon);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            error: function(text, icon)
            {
                if (!icon) icon = "error";
                return this.showLog(text, icon);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            publishEvent: function(event)
            {
                if (!FirebugContext) return;
                var panel = FirebugContext.getPanel("XRefreshExtension");
                if (panel) panel.publish(event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendHello: function()
            {
                var message =
                {
                    command: "Hello",
                    type: "Firefox",
                    agent: navigator.userAgent.toLowerCase()
                };
                this.sendMessageToServer(message);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendBye: function()
            {
                var message =
                {
                    command: "Bye"
                };
                this.sendMessageToServer(message);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendSetPage: function(contentTitle, contentURL)
            {
                var message =
                {
                    command: "SetPage",
                    page: contentTitle,
                    url: contentURL
                };
                this.sendMessageToServer(message);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            storePageOffset: function(context)
            {
                debugLog("Storing offsets for " + context.window.document.URL);
                var recorder = recorders[context.window.document.URL];
                if (!recorder) return;
                recorder.offsets = [context.window.pageXOffset, context.window.pageYOffset];
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            restorePageOffset: function(context)
            {
                debugLog("Restoring offsets for " + context.window.document.URL);
                var recorder = recorders[context.window.document.URL];
                if (!recorder) return;
                var data = recorder.offsets;
                if (!data) return;
                context.window.scrollTo(data[0], data[1]);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            openPermissions: function(event, context)
            {
                cancelEvent(event);

                var browserURI = FirebugChrome.getBrowserURI(context);
                var host = this.getHostForURI(browserURI);

                var params = {
                    permissionType: this.getPrefDomain(),
                    windowTitle: $XREFRESH_STR(this.panelName + ".Permissions"),
                    introText: $XREFRESH_STR(this.panelName + ".PermissionsIntro"),
                    blockVisible: true,
                    sessionVisible: false,
                    allowVisible: true,
                    prefilledHost: host,
                };

                openWindow("Browser:Permissions", "chrome://browser/content/preferences/permissions.xul", "", params);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getMenuLabel: function(option, location, shortened)
            {
                var label = "";
                var host = "";

                switch (option)
                {
                case "disable-site":
                    if (isSystemURL(location.spec))
                    label = "SystemPagesDisable";
                    else if (!getURIHost(location))
                    label = "LocalFilesDisable";
                    else
                    label = "HostDisable";

                    if (shortened)
                    return $XREFRESH_STR("panel.Disabled");
                    break;

                case "enable-site":
                    if (isSystemURL(location.spec))
                    label = "SystemPagesEnable";
                    else if (!getURIHost(location))
                    label = "LocalFilesEnable";
                    else
                    label = "HostEnable";

                    if (shortened)
                    return $XREFRESH_STR("panel.Enabled");
                    break;

                case "enable":
                    return $XREFRESH_STR("panel.Enabled");

                case "disable":
                    return $XREFRESH_STR("panel.Disabled");
                }

                if (!label)
                return null;

                label = this.panelName + "." + label;
                return $XREFRESH_STRF(label, [getURIHost(location)]);
            }

        });

        ////////////////////////////////////////////////////////////////////////
        //
        //
        top.BlinkEvent = function(type, message, icon)
        {
            this.type = type;
            this.message = message;
            this.opened = false;
            this.icon = icon;
        };

        Firebug.XRefreshExtension.XHR = domplate(Firebug.Rep,
        {
            tagRefresh:
            DIV({
                class: "blinkHead closed $object|getIcon",
                _repObject: "$object"
            },
            A({
                class: "blinkTitle",
                onclick: "$onToggleBody"
            },
            IMG({
                class: "blinkIcon",
                src: "blank.gif"
            }),
            SPAN({
                class: "blinkDate"
            },
            "$object|getDate"),
            SPAN({
                class: "blinkURI"
            },
            "$object|getCaption"),
            SPAN({
                class: "blinkInfo"
            },
            "$object|getInfo")
            ),
            DIV({
                class: "details"
            })
            ),

            tagLog:
            DIV({
                class: "blinkHead $object|getIcon",
                _repObject: "$object"
            },
            A({
                class: "blinkTitle"
            },
            IMG({
                class: "blinkIcon",
                src: "blank.gif"
            }),
            SPAN({
                class: "blinkDate"
            },
            "$object|getDate"),
            SPAN({
                class: "blinkURI"
            },
            "$object|getCaption")
            )
            ),

            /////////////////////////////////////////////////////////////////////////////////////////
            getCaption: function(event)
            {
                if (event.type == 0) return 'Project \'' + event.message.name + '\': ';
                if (event.type == 1) return event.message.text;
                return "???";
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getIcon: function(event)
            {
                return event.icon;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getDate: function(event)
            {
                return ' [' + event.message.time + ']';
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            filesSentence: function(i, s)
            {
                if (i == 0) return null;
                if (i == 1) return 'one item ' + s;
                return i + ' items ' + s;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            glue: function(a)
            {
                if (!a.length) return "";
                var last = a.length - 1;
                var s = a[0];
                for (var i = 1; i < a.length; i++)
                {
                    if (i == last) s += " and ";
                    else s += ", ";
                    s += a[i];
                }
                return s;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getInfo: function(event)
            {
                var m = event.message;
                var changed = 0;
                var created = 0;
                var deleted = 0;
                var renamed = 0;
                for (var i = 0; i < m.files.length; i++)
                {
                    var file = m.files[i];
                    if (file.action == "changed") changed++;
                    if (file.action == "created") created++;
                    if (file.action == "deleted") deleted++;
                    if (file.action == "renamed") renamed++;
                }

                var s1 = this.filesSentence(created, "created");
                var s2 = this.filesSentence(deleted, "deleted");
                var s3 = this.filesSentence(changed, "changed");
                var s4 = this.filesSentence(renamed, "renamed");
                var a = [];
                if (s1) a.push(s1);
                if (s2) a.push(s2);
                if (s3) a.push(s3);
                if (s4) a.push(s4);
                return this.glue(a);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onToggleBody: function(e)
            {
                if (isLeftClick(e)) this.toggle(e.currentTarget);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            toggle: function(target)
            {
                var logRow = getAncestorByClass(target, "logRow-blink");
                var row = getChildByClass(logRow, "blinkHead")
                var event = row.repObject;
                if (event.type != 0) return;

                var details = getChildByClass(row, "details");

                toggleClass(row, "opened");
                toggleClass(row, "closed");

                event.opened = false;
                if (hasClass(row, "opened"))
                {
                    event.opened = true;
                    this.showEventDetails(event, details);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showEventDetails: function(event, details)
            {
                var s = '';
                var m = event.message;
                s += '<table class="ftable" cellpadding="0" cellspacing="0">';
                s += '<tr><td class="froot" colspan="2">' + m.root + '</td></tr>';

                var i;
                for (i = 0; i < m.files.length; i++)
                {
                    var file = m.files[i];
                    fa = file.path1.split('\\');
                    if (fa.length > 0)
                    {
                        fa2 = fa.pop();
                        fa1 = fa.join('\\');
                        if (fa1) fa1 += '\\';

                        if (!file.path2)
                        {
                            s += '<tr><td class="faction ' + file.action + '"></td><td class="ffile"><span class="ffa1">' + fa1 + '</span><span class="ffa2">' + fa2 + '</span></td></tr>';
                        }
                        else
                        {
                            fb = file.path2.split('\\');
                            fb2 = fb.pop();
                            fb1 = fb.join('\\');
                            if (fb1) fb1 += '\\';
                            s += '<tr><td class="faction ' + file.action + '"></td><td class="ffile"><span class="ffa1">' + fa1 + '</span><span class="ffa2">' + fa2 + '</span> -&gt; <span class="ffb1">' + fb1 + '</span><span class="ffb2">' + fb2 + '</span></td></tr>';
                        }
                    }
                }

                s += '</table>';

                details.innerHTML = s;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            supportsObject: function(object)
            {
                return object instanceof BlinkEvent;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getRealObject: function(event, context)
            {
                return event.message;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getContextMenuItems: function(event)
            {
                return null;
            }
        });

        function XRefreshExtensionPanel() {}
        XRefreshExtensionPanel.prototype = extend(Firebug.Panel,
        {
            name: "XRefreshExtension",
            title: "XRefresh",
            searchable: false,
            editable: false,

            wasScrolledToBottom: true,

            /////////////////////////////////////////////////////////////////////////////////////////
            initialize: function()
            {
                Firebug.Panel.initialize.apply(this, arguments);

                this.applyCSS();
                this.restoreState();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            applyCSS: function()
            {
                this.applyPanelCSS("chrome://xrefresh/skin/panel.css");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            safeGetURI: function(browser)
            {
                try
                {
                    return this.context.browser.currentURI;
                }
                catch(exc)
                {
                    return null;
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            refresh: function(context)
            {
                var localOnly = Firebug.XRefreshExtension.getPref('localPagesOnly');
                var uri = this.safeGetURI();

                if (localOnly)
                {
                    // TODO: refresh localhost only
                    var localhost = true;
                    if (! (uri.scheme == "file" || localhost)) return;
                }

                Firebug.XRefreshExtension.storePageOffset(context);
                drumInitiatedRefresh = true;
                this.context.browser.reloadWithFlags(
                /*this.context.browser.webNavigation.LOAD_FLAGS_BYPASS_CACHE*/
                );
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateStyleSheet: function(document, element, path)
            {
                var file = Cc["@mozilla.org/file/local;1"].createInstance(Components.interfaces.nsILocalFile);
                file.initWithPath(path);
                // note: async loading doesn't work
                //              var ios = Cc["@mozilla.org/network/io-service;1"].getService(Components.interfaces.nsIIOService);
                //              var fileURI = ios.newFileURI(file);
                //              var channel = ios.newChannelFromURI(fileURI, document.characterSet, null);
                //              channel.loadFlags |= Ci.nsIRequest.LOAD_BYPASS_CACHE;
                var observer = {
                    onStreamComplete: function(aLoader, aContext, aStatus, aLength, aResult)
                    {
                        var styleElement = document.createElement("style");
                        styleElement.setAttribute("type", "text/css");
                        styleElement.appendChild(document.createTextNode(aResult));
                        var attrs = ["media", "title", "disabled"];
                        for (var i = 0; i < attrs.length; i++)
                        {
                            var attr = attrs[i];
                            if (element.hasAttribute(attr)) styleElement.setAttribute(attr, element.getAttribute(attr));
                        }
                        element.parentNode.replaceChild(styleElement, element);
                        styleElement.originalHref = element.originalHref ? element.originalHref: element.href;
                    }
                };
                //              var sl = Cc["@mozilla.org/network/stream-loader;1"].createInstance(Components.interfaces.nsIStreamLoader);
                //              sl.init(channel, observer, null); // <- this doesn't work on FF3b3
                var inputStream = Cc["@mozilla.org/network/file-input-stream;1"].createInstance(Components.interfaces.nsIFileInputStream);
                var scriptableStream = Cc["@mozilla.org/scriptableinputstream;1"].createInstance(Components.interfaces.nsIScriptableInputStream);

                inputStream.init(file, -1, 0, 0);
                scriptableStream.init(inputStream);
                var content = scriptableStream.read(scriptableStream.available());
                scriptableStream.close();
                inputStream.close();

                observer.onStreamComplete(null, null, null, null, content);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            collectDocuments: function(frame)
            {
                var documents = [];
                if (!frame) return documents;
                if (frame.document) documents.push(frame.document);
                var frames = frame.frames;
                for (var i = 0; i < frames.length; i++) documents = documents.concat(this.collectDocuments(frames[i]));
                return documents;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            doesCSSNameMatch: function(cssLink, cssFile)
            {
                var firstQ = cssLink.indexOf('?');
                if (firstQ != -1) cssLink = cssLink.substring(0, firstQ);
                var lastLinkSlash = cssLink.lastIndexOf('/');
                if (lastLinkSlash != -1) cssLink = cssLink.substring(lastLinkSlash + 1);
                var lastFileSlash = cssFile.lastIndexOf('/');
                if (lastFileSlash != -1) cssFile = cssFile.substring(lastFileSlash + 1);
                var res = (cssFile.toLowerCase() == cssLink.toLowerCase());
                debugLog('Match ' + cssLink + ' vs. ' + cssFile + ' result:' + (res ? 'true': 'false'));
                return res;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replaceMatchingStyleSheetsInDocument: function(document, cssFile)
            {
                debugLog('Replacing CSS in document', document);
                var styleSheetList = document.styleSheets;
                for (var i = 0; i < styleSheetList.length; i++)
                {
                    var styleSheetNode = styleSheetList[i].ownerNode;
                    // this may be <style> or <link> node
                    if (!styleSheetNode) continue;
                    var href = styleSheetNode.href || styleSheetNode.originalHref;
                    if (!href) continue;
                    if (this.doesCSSNameMatch(href, cssFile)) this.updateStyleSheet(document, styleSheetNode, cssFile);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replaceMatchingStyleSheets: function(context, cssFile)
            {
                var documents = this.collectDocuments(context.window);
                for (var i = 0; i < documents.length; i++) this.replaceMatchingStyleSheetsInDocument(documents[i], cssFile);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateCSS: function(context, cssFiles)
            {
                debugLog('Replacing css files', cssFiles);
                for (var i = 0; i < cssFiles.length; i++)
                {
                    var cssFile = cssFiles[i].replace('\\', '/');
                    // convert windows backslashes to forward slashes
                    this.replaceMatchingStyleSheets(context, cssFile);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            publish: function(event)
            {
                var recorder = recorders[FirebugContext.window.document.URL];
                if (!recorder) return;
                if (!recorder.published) recorder.published = [];
                recorder.published.push(event);
                event.root = this.append(event, "blink", null, null);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            restoreState: function()
            {
                var recorder = recorders[FirebugContext.window.document.URL];
                if (!recorder) return;
                var data = recorder.published;
                if (!data) return;

                var i;
                for (i = 0; i < data.length; i++)
                {
                    event = data[i];
                    event.root = this.append(event, "blink", null, null);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            clear: function()
            {
                if (this.panelNode) clearNode(this.panelNode);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            show: function(state)
            {
                var enabled = Firebug.XRefreshExtension.isEnabled(this.context);
				var enabledRecorder = Firebug.XRefreshExtension.getPref("enableRecorder");
                this.showToolbarButtons("fbXRefreshExtensionButtons", true);
                this.showToolbarButtons("fbXRefreshExtensionControls", enabled);
                this.showToolbarButtons("fbXRefreshExtensionRecorder", enabled && enabledRecorder);

                // the default page with description and enable button is
                // visible only if debugger is disabled.
                if (enabled)
                	Firebug.ModuleManagerPage.hide(this);
                else
                	Firebug.ModuleManagerPage.show(this, Firebug.XRefreshExtension);

                if (this.wasScrolledToBottom)
                scrollToBottom(this.panelNode);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            hide: function()
            {
                this.showToolbarButtons("fbXRefreshExtensionButtons", false);
                this.showToolbarButtons("fbXRefreshExtensionControls", false);
                this.showToolbarButtons("fbXRefreshExtensionRecorder", false);
                this.wasScrolledToBottom = isScrolledToBottom(this.panelNode);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getOptionsMenuItems: function()
            {
                return null;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getTopContainer: function()
            {
                return this.panelNode;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            createRow: function(rowName, className)
            {
                var elt = this.document.createElement("div");
                elt.className = rowName + (className ? " " + rowName + "-" + className: "");
                return elt;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            append: function(objects, className, rep)
            {
                var container = this.getTopContainer();
                var scrolledToBottom = isScrolledToBottom(this.panelNode);
                var row = this.createRow("logRow", className);

                this.appendObject.apply(this, [objects, row, rep]);

                container.appendChild(row);

                if (scrolledToBottom)
                scrollToBottom(this.panelNode);

                return row;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            appendObject: function(object, row, rep)
            {
                var rep = rep ? rep: Firebug.getRep(object);
                var res = "";
                switch (object.type)
                {
                case 0:
                    res = rep.tagRefresh.append({
                        object: object
                    },
                    row);
                    break;
                case 1:
                    res = rep.tagLog.append({
                        object: object
                    },
                    row);
                    break;
                }
                if (object.opened)
                {
                    rep.toggle(row.childNodes[0].childNodes[0]);
                }
                return res;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            applyPanelCSS: function(url)
            {
                var styleElement = this.document.createElement("link");
                styleElement.setAttribute("type", "text/css");
                styleElement.setAttribute("href", url);
                styleElement.setAttribute("rel", "stylesheet");

                var head = this.getHeadElement(this.document);
                if (head) head.appendChild(styleElement);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getHeadElement: function(doc)
            {
                var heads = doc.getElementsByTagName("head");
                if (heads.length == 0) return doc.documentElement;
                return heads[0];
            }
        });

        Firebug.registerActivableModule(Firebug.XRefreshExtension);
        Firebug.registerRep(Firebug.XRefreshExtension.XHR);
        Firebug.registerPanel(XRefreshExtensionPanel);
    }
});
// close custom scope
