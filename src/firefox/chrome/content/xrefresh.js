// For communication protocol between extension and server
// we use UTF-8 encoded JSON to exchange messages
//
// This source contains copy&pasted various bits from Firebug sources.

// open custom scope
FBL.ns(function() {
    with(FBL) {
        const Cc = Components.classes;
        const Ci = Components.interfaces;

        const nsIPrefBranch = Ci.nsIPrefBranch;
        const nsIPrefBranch2 = Ci.nsIPrefBranch2;

        const xrefreshPrefService = Cc["@mozilla.org/preferences-service;1"];
        const socketServer = Cc["@mozilla.org/network/server-socket;1"];

        const xrefreshPrefs = xrefreshPrefService.getService(nsIPrefBranch2);
        const xrefreshHomepage = "http://xrefresh.binaryage.com";

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

        if (Firebug.TraceModule) {
            Firebug.TraceModule.DBG_XREFRESH = false;
            var type = xrefreshPrefs.getPrefType('extensions.firebug.DBG_XREFRESH');
            if (type != nsIPrefBranch.PREF_BOOL) try {
                xrefreshPrefs.setBoolPref('extensions.firebug.DBG_XREFRESH', false);
            } catch(e) {}
        }

        function dbg() {
            if (FBTrace && FBTrace.DBG_XREFRESH) FBTrace.sysout.apply(this, arguments);
        }

        var optionMenu = function(label, option) {
            return {
                label: label, 
                nol10n: true,
                type: "checkbox", 
                checked: Firebug.XRefresh.getPref(option), 
                option: option,
                command: function() {
                    Firebug.XRefresh.setPref(option, !Firebug.XRefresh.getPref(option)); // toggle
                }
            };
        };
        
        ////////////////////////////////////////////////////////////////////////
        // Firebug.XRefresh, here we go!
        //
        Firebug.XRefresh = extend(Firebug.ActivableModule, {
            reminder: '',
            /////////////////////////////////////////////////////////////////////////////////////////
            checkFirebugVersion: function() {
                var version = Firebug.getVersion();
                if (!version) return false;
                var a = version.split('.');
                if (a.length<2) return false;
                // we want Firebug version 1.4+ (including alphas/betas and other weird stuff)
                return parseInt(a[0], 10)>=1 && parseInt(a[1], 10)>=4;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            startupCheck: function(context) {
                if (!this.checkFirebugVersion()) {
                    this.log(context, "XRefresh Firefox extension works only with Firebug 1.4 or higher. Please upgrade Firebug to latest version.", "warn");
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            initialize: function() {
                dbg(">> XRefresh.initialize", arguments);
                this.panelName = 'XRefresh';
                this.description = "Browser refresh automation for web developers";
                Firebug.ActivableModule.initialize.apply(this, arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            shutdown: function() {
                dbg(">> XRefresh.shutdown", arguments);
                Firebug.ActivableModule.shutdown.apply(this, arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onPanelEnable: function(context, panelName) {
                if (panelName != this.panelName) return;
                dbg(">> XRefresh.onPanelEnable", arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onPanelDisable: function(context, panelName) {
                if (panelName != this.panelName) return;
                dbg(">> XRefresh.onPanelDisable", arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onSuspendFirebug: function(context) {
                dbg(">> XRefresh.onSuspendFirebug", arguments);
                if (this.scheduledDisconnection) clearTimeout(this.scheduledDisconnection);
                var that = this;
                this.scheduledDisconnection = setTimeout(function() {
                    that.disconnect(context);
                }, 10000);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onResumeFirebug: function(context) {
                dbg(">> XRefresh.onResumeFirebug", arguments);
                if (this.scheduledDisconnection) clearTimeout(this.scheduledDisconnection);
                this.scheduledDisconnection = undefined;
                if (this.alreadyActivated) return;
                this.alreadyActivated = true;
                var that = this;
                setTimeout(function() {
                    that.startupCheck(context);
                }, 1000);
                setTimeout(function() {
                    that.connectDrum(context);
                }, 1000);
                setTimeout(function() {
                    that.startListener(context);
                }, 2000);
                this.checkTimeout = setTimeout(function() {
                    that.connectionCheck(context);
                }, 5000);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            disconnect: function(context) {
                dbg(">> XRefresh.disconnect", arguments);
                this.scheduledDisconnection = undefined;
                if (!this.alreadyActivated) return;
                this.alreadyActivated = undefined;
                // just after onPanelDeactivate, no remaining activecontext
                this.disconnectDrum(context);
                this.stopListener(context);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            connectionCheck: function(context) {
                dbg(">> XRefresh.connectionCheck", arguments);
                delete this.checkTimeout;
                if (drumReady) return;
                this.log(context, "Unable to connect to XRefresh Server", "warn");
                this.log(context, "Please check if you have running XRefresh Server.", "bulb");
                this.log(context, "    On Windows, it is program running in system tray. Look for Programs -> XRefresh -> XRefresh.exe", "bulb");
                this.log(context, "    On Mac, it is running command-line program xrefresh-server. It should be available on system path after 'sudo gem install xrefresh-server'", "bulb");
                this.log(context, "You may also want to check your firewall settings. XRefresh Firefox extension expects Server to talk from " + this.getPref('host') + " on port " + this.getPref('port'), "bulb");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            shortConnectionCheck: function(context) {
                dbg(">> XRefresh.shortConnectionCheck", arguments);
                delete this.shortCheckTimeout;
                if (drumReady) return;
                this.log(context, "Unable to connect to XRefresh Server. Please check if you have running XRefresh Server and tweak your firewall settings if needed.", "warn");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showPanel: function(browser, panel) {
                dbg(">> XRefresh.showPanel", arguments);
                Firebug.ActivableModule.showPanel.apply(this, arguments);
                var isXRefresh = panel && panel.name == this.panelName;
                if (isXRefresh) {
                    this.updatePanel(panel.context);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            visitWebsite: function() {
                dbg(">> XRefresh.visitWebsite", arguments);
                openNewTab(xrefreshHomepage);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getPref: function(name) {
                var prefName = this.getPrefDomain() + "." + name;
                var type = xrefreshPrefs.getPrefType(prefName);
                if (type == nsIPrefBranch.PREF_STRING)
                return xrefreshPrefs.getCharPref(prefName);
                else if (type == nsIPrefBranch.PREF_INT)
                return xrefreshPrefs.getIntPref(prefName);
                else if (type == nsIPrefBranch.PREF_BOOL)
                return xrefreshPrefs.getBoolPref(prefName);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            setPref: function(name, value) {
                var prefName = this.getPrefDomain() + "." + name;
                var type = xrefreshPrefs.getPrefType(prefName);
                if (type == nsIPrefBranch.PREF_STRING)
                xrefreshPrefs.setCharPref(prefName, value);
                else if (type == nsIPrefBranch.PREF_INT)
                xrefreshPrefs.setIntPref(prefName, value);
                else if (type == nsIPrefBranch.PREF_BOOL)
                xrefreshPrefs.setBoolPref(prefName, value);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            initContext: function(context) {
                dbg(">> XRefresh.initContext: " + context.window.document.URL);
                Firebug.ActivableModule.initContext.apply(this, arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            reattachContext: function(context) {
                dbg(">> XRefresh.reattachContext: " + context.window.document.URL);
                Firebug.ActivableModule.reattachContext.apply(this, arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            destroyContext: function(context, persistedState) {
                dbg(">> XRefresh.destroyContext: " + context.window.document.URL);
                Firebug.ActivableModule.destroyContext.apply(this, arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showContext: function(browser, context) {
                dbg(">> XRefresh.showContext: " + context.window.document.URL);
                Firebug.ActivableModule.showContext.apply(this, arguments);
                this.updatePanel(context);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            loadedContext: function(context) {
                dbg(">> XRefresh.loadedContext: " + context.window.document.URL);
                Firebug.ActivableModule.loadedContext.apply(this, arguments);
                if (!this.isEnabled(context)) return;
                this.sendSetPage(context.browser.contentTitle, context.window.document.URL);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonRefresh: function(context) {
                dbg(">> XRefresh.buttonRefresh: " + context.window.document.URL);
                context.getPanel(this.panelName).refresh(context);
                this.log(context, "Manual refresh performed by user", "rreq");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonStatus: function(context) {
                dbg(">> XRefresh.buttonStatus: " + context.window.document.URL);
                if (this.checkTimeout) clearTimeout(this.checkTimeout);
                delete this.checkTimeout;
                if (this.shortCheckTimeout) clearTimeout(this.shortCheckTimeout);
                delete this.shortCheckTimeout;
                if (drumReady) {
                    this.log(context, "Disconnection requested by user", "disconnect_btn");
                    this.disconnectDrum(context);
                } else {
                    this.log(context, "Connection requested by user", "connect_btn");
                    this.shortCheckTimeout = setTimeout(function() {
                        this.shortConnectionCheck(context);
                    }, 3000);
                    this.connectDrum(context);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            connectDrum: function(context) {
                dbg(">> XRefresh.connectDrum", arguments);
                
                var transportService = Cc["@mozilla.org/network/socket-transport-service;1"].getService(Components.interfaces.nsISocketTransportService);
                drumTransport = transportService.createTransport(null, 0, this.getPref("host"), this.getPref("port"), null);
                drumOutStream = drumTransport.openOutputStream(0, 0, 0);

                var stream = drumTransport.openInputStream(0, 0, 0);
                drumInStream = Cc["@mozilla.org/scriptableinputstream;1"].createInstance(Components.interfaces.nsIScriptableInputStream);
                drumInStream.init(stream);

                var dataListener = {
                    context: context,
                    parent: this,
                    data: "",
                    onStartRequest: function(request, context2) {
                    },
                    onStopRequest: function(request, context2, status) {
                        this.parent.onServerDied(context);
                    },
                    onDataAvailable: function(request, context2, inputStream, offset, count) {
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
            disconnectDrum: function(context) {
                dbg(">> XRefresh.disconnectDrum", arguments);
                if (drumReady) this.log(context, "Disconnected from XRefresh Server", "disconnect");
                // it is nice to say good bye ...
                this.sendBye();
                if (drumInStream) {
                    drumInStream.close();
                    drumInStream = null;
                }
                if (drumOutStream) {
                    drumOutStream.close();
                    drumOutStream = null;
                }
                drumReady = false;
                this.updatePanel(context);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            reconnectDrum: function(context) {
                dbg(">> XRefresh.reconnectDrum", arguments);
                this.disconnectDrum(context);
                this.connectDrum(context);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            startListener: function(context) {
                dbg(">> XRefresh.startListener", arguments);
                var server = socketServer.createInstance(Components.interfaces.nsIServerSocket);

                var listener = {
                    onSocketAccepted: function(socket, transport) {
                        Firebug.XRefresh.log(context, "Reconnection request received");
                        Firebug.XRefresh.reconnectDrum(context);
                    },
                    onStopListening: function(serverSocket, status) {
                    }
                };

                var i;
                var range = this.getPref("portRange");
                var port = this.getPref("port");
                var loopbackonly = this.getPref("localOnly");
                for (i = 1; i <= range; i++) {
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
                this.error(context, "No unused port available in given range: " + (port - range) + "-" + (port - 1));
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            stopListener: function() {
                dbg(">> XRefresh.stopListener", arguments);
                if (!listenerServer) return;
                listenerServer.close();
                listenerServer = null;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendMessageToServer: function(message) {
                dbg(">> XRefresh.sendMessageToServer", arguments);
                if (!drumOutStream) {
                    dbg("  !! drumOutStream is null", arguments);
                    return false;
                }
                var data = JSON.stringify(message) + '\n'; // every message is delimited with new line
                var utf8data = UTF8.encode(data);
                drumOutStream.write(utf8data, utf8data.length);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onServerDied: function(context) {
                dbg(">> XRefresh.onServerDied", arguments);
                if (drumReady) this.error(context, "XRefresh Server has closed connection");

                if (drumInStream) {
                    drumInStream.close();
                    drumInStream = null;
                }
                if (drumOutStream) {
                    drumOutStream.close();
                    drumOutStream = null;
                }
                drumReady = false;
                this.updatePanel(context);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onDataAvailable: function(listener) {
                dbg(">> XRefresh.onDataAvailable", arguments);
                // try to parse incomming message
                // here we expect server to send always valid stream of {json1}\n{json2}\n{json3}\n...
                // TODO: make this more robust to server formating failures
                dbg(data||"empty message");
                var data = listener.data;
                var parts = listener.data.split('\n');
                var buffer = this.reminder;
                for (var i = 0; i < parts.length; i++) {
                    buffer += UTF8.decode(parts[i]);
                    dbg("  buffer:", buffer);

                    var message = JSON.parse(buffer);
                    if (!message) continue;
                    // we have only partial buffer? go for next chunk
                    buffer = '';
                    // message completed, clear buffer for incomming data
                    dbg("    message:", message);
                    //try {
                        this.processMessage(listener.context, message);
                    // } catch(e) {
                    //     this.error(listener.context, "Error in processing message from XRefresh Server");
                    // }
                }
                this.reminder = buffer;
                listener.data = "";
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getMessageCSSFiles: function(message) {
                var files = [];
                var re = /\.css$/;
                for (var i = 0; i < message.files.length; i++) {
                    var file = message.files[i];
                    if (file.action == 'changed') {
                        if (file.path1.match(re)) {
                            files.push(message.root + '\\' + file.path1);
                            // TODO: this should be platform specific
                        }
                    }
                }
                return files;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            processMessage: function(context, message) {
                dbg("Received message: " + message.command);
                if (!this.isEnabled(this.currentContext)) {
                    dbg("Skipped message because the panel is not enabled");
                    return;
                }
                if (message.command == "DoRefresh") {
                    var panel = context.getPanel(this.panelName);
                    if (this.getPref("softRefresh")) {
                        var cssFiles = this.getMessageCSSFiles(message);
                        if (cssFiles.length == message.files.length) { // message contains only CSS files?
                            this.showEvent(context, message, 'fastcss');
                            return panel.updateCSS(context, cssFiles); // perform soft refresh
                        }
                    }
                    this.showEvent(context, message, 'refresh');
                    panel.refresh(context);
                    return;
                }
                if (message.command == "AboutMe") {
                    drumReady = true;
                    drumName = message.agent;
                    drumVersion = message.version;
                    this.log(context, "Connected to " + drumName + " " + drumVersion, "connect");
                    this.updatePanel(context);
                    this.sendSetPage(context.browser.contentTitle, context.window.document.URL);
                    return;
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updatePanel: function(context) {
                // safety net
                var panel = context.getPanel(this.panelName);
                if (!panel) return;
                var browser = panel.context.browser;
                if (!browser) return;
                var buttonStatus = browser.chrome.$("fbXRefreshButtonStatus");
                if (!buttonStatus) return;
                buttonStatus.className = "toolbar-text-button toolbar-connection-status";
                if (drumReady) {
                    buttonStatus.label = "Connected to " + drumName + " " + drumVersion;
                    setClass(buttonStatus, "toolbar-text-button toolbar-status-connected");
                } else {
                    buttonStatus.label = "Disconnected";
                    setClass(buttonStatus, "toolbar-text-button toolbar-status-disconnected");
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getCurrentTime: function() {
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
            showEvent: function(context, message, icon) {
                message.time = this.getCurrentTime();
                var event = new BlinkEvent(0, message, icon);
                return this.publishEvent(context, event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showLog: function(context, text, icon) {
                var event = new BlinkEvent(1, { text: text, time: this.getCurrentTime() }, icon);
                return this.publishEvent(context, event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            log: function(context, text, icon) {
                if (!icon) icon = "info";
                return this.showLog(context, text, icon);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            error: function(context, text, icon) {
                if (!icon) icon = "error";
                return this.showLog(context, text, icon);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            publishEvent: function(context, event) {
                dbg(">> XRefresh.publishEvent", arguments);
                var panel = context.getPanel(this.panelName);
                if (!panel) {
                    dbg("  !! unable to lookup panel:"+this.panelName);
                    return;
                }
                panel.publish(event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendHello: function() {
                var message = {
                    command: "Hello",
                    type: "Firefox",
                    agent: navigator.userAgent.toLowerCase()
                };
                this.sendMessageToServer(message);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendBye: function() {
                var message = {
                    command: "Bye"
                };
                this.sendMessageToServer(message);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendSetPage: function(contentTitle, contentURL) {
                var message = {
                    command: "SetPage",
                    page: contentTitle,
                    url: contentURL
                };
                this.sendMessageToServer(message);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            storePageOffset: function(context) {
                dbg("Storing offsets for " + context.window.document.URL);
                // recorder.offsets = [context.window.pageXOffset, context.window.pageYOffset];
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            restorePageOffset: function(context) {
                dbg("Restoring offsets for " + context.window.document.URL);
                // context.window.scrollTo(data[0], data[1]);
            }
        });

        ////////////////////////////////////////////////////////////////////////
        //
        //
        top.BlinkEvent = function(type, message, icon) {
            this.type = type;
            this.message = message;
            this.opened = false;
            this.icon = icon;
        };

        Firebug.XRefresh.XHR = domplate(Firebug.Rep, {
            tagRefresh:
                DIV({class: "blinkHead closed $object|getIcon", _repObject: "$object"},
                    A({class: "blinkTitle", onclick: "$onToggleBody"},
                        IMG({class: "blinkIcon", src: "blank.gif"}),
                        SPAN({class: "blinkDate"}, "$object|getDate"),
                        SPAN({class: "blinkURI" }, "$object|getCaption"),
                        SPAN({class: "blinkInfo"}, "$object|getInfo")
                    ),
                    DIV({class: "details"})
                ),

            tagLog:
                DIV({class: "blinkHead $object|getIcon", _repObject: "$object"},
                    A({class: "blinkTitle"},
                        IMG({class: "blinkIcon", src:"blank.gif"}),
                        SPAN({class: "blinkDate"}, "$object|getDate"),
                        SPAN({class: "blinkURI"}, "$object|getCaption")
                    )
                ),

            /////////////////////////////////////////////////////////////////////////////////////////
            getCaption: function(event) {
                if (event.type == 0) return 'Project \'' + event.message.name + '\': ';
                if (event.type == 1) return event.message.text;
                return "???";
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getIcon: function(event) {
                return event.icon;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getDate: function(event) {
                return ' [' + event.message.time + ']';
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            filesSentence: function(i, s) {
                if (i == 0) return null;
                if (i == 1) return 'one item ' + s;
                return i + ' items ' + s;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            glue: function(a) {
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
            getInfo: function(event) {
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
            onToggleBody: function(e) {
                if (isLeftClick(e)) this.toggle(e.currentTarget);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            toggle: function(target) {
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
            showEventDetails: function(event, details) {
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
            supportsObject: function(object) {
                return object instanceof BlinkEvent;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getRealObject: function(event, context) {
                return event.message;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getContextMenuItems: function(event) {
                return null;
            }
        });

        function XRefreshPanel() {}
        XRefreshPanel.prototype = extend(Firebug.ActivablePanel, {
            name: "XRefresh",
            title: "XRefresh",
            searchable: false,
            editable: false,
            wasScrolledToBottom: true,
            /////////////////////////////////////////////////////////////////////////////////////////
            enablePanel: function(module) {
                dbg(">> XRefreshPanel.enablePanel; " + this.context.getName());
                Firebug.ActivablePanel.enablePanel.apply(this, arguments);
                // this.showCommandLine(true);
                // if (this.wasScrolledToBottom)
                //     scrollToBottom(this.panelNode);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            disablePanel: function(module) {
                dbg(">> XRefreshPanel.disablePanel; " + this.context.getName());
                Firebug.ActivablePanel.disablePanel.apply(this, arguments);
                // this.showCommandLine(false);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            initialize: function() {
                dbg(">> XRefreshPanel.initialize", arguments);
                Firebug.ActivablePanel.initialize.apply(this, arguments);
                this.applyCSS();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            applyCSS: function() {
                this.applyPanelCSS("chrome://xrefresh/skin/panel.css");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            safeGetURI: function(browser) {
                try {
                    return this.context.browser.currentURI;
                } catch(e) {}
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            refresh: function(context) {
                dbg(">> XRefreshPanel.refresh", arguments);
                var uri = this.safeGetURI();
                Firebug.XRefresh.storePageOffset(context);
                drumInitiatedRefresh = true;
                var browser = context.browser;
                var url = context.window.document.location;
                browser.loadURIWithFlags(url, browser.webNavigation.LOAD_FLAGS_FROM_EXTERNAL);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateStyleSheet: function(document, element, path) {
                // TODO: try this: http://markmail.org/message/5mfzam3vgxtmvq3z#query:mozilla.org%2Fnetwork%2Fcache-service+page:1+mid:d6ooz3mhlsexv2rw+state:results
                var file = Cc["@mozilla.org/file/local;1"].createInstance(Components.interfaces.nsILocalFile);
                file.initWithPath(path);
                // note: async loading doesn't work
                //              var ios = Cc["@mozilla.org/network/io-service;1"].getService(Components.interfaces.nsIIOService);
                //              var fileURI = ios.newFileURI(file);
                //              var channel = ios.newChannelFromURI(fileURI, document.characterSet, null);
                //              channel.loadFlags |= Ci.nsIRequest.LOAD_BYPASS_CACHE;
                var observer = {
                    onStreamComplete: function(aLoader, aContext, aStatus, aLength, aResult) {
                        var styleElement = document.createElement("style");
                        styleElement.setAttribute("type", "text/css");
                        styleElement.appendChild(document.createTextNode(aResult));
                        var attrs = ["media", "title", "disabled"];
                        for (var i = 0; i < attrs.length; i++) {
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
            collectDocuments: function(frame) {
                var documents = [];
                if (!frame) return documents;
                if (frame.document) documents.push(frame.document);
                var frames = frame.frames;
                for (var i = 0; i < frames.length; i++) documents = documents.concat(this.collectDocuments(frames[i]));
                return documents;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            doesCSSNameMatch: function(cssLink, cssFile) {
                var firstQ = cssLink.indexOf('?');
                if (firstQ != -1) cssLink = cssLink.substring(0, firstQ);
                var lastLinkSlash = cssLink.lastIndexOf('/');
                if (lastLinkSlash != -1) cssLink = cssLink.substring(lastLinkSlash + 1);
                var lastFileSlash = cssFile.lastIndexOf('/');
                if (lastFileSlash != -1) cssFile = cssFile.substring(lastFileSlash + 1);
                var res = (cssFile.toLowerCase() == cssLink.toLowerCase());
                dbg('Match ' + cssLink + ' vs. ' + cssFile + ' result:' + (res ? 'true': 'false'));
                return res;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replaceMatchingStyleSheetsInDocument: function(document, cssFile) {
                dbg('Replacing CSS in document', document);
                var styleSheetList = document.styleSheets;
                for (var i = 0; i < styleSheetList.length; i++) {
                    var styleSheetNode = styleSheetList[i].ownerNode;
                    // this may be <style> or <link> node
                    if (!styleSheetNode) continue;
                    var href = styleSheetNode.href || styleSheetNode.originalHref;
                    if (!href) continue;
                    if (this.doesCSSNameMatch(href, cssFile)) this.updateStyleSheet(document, styleSheetNode, cssFile);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replaceMatchingStyleSheets: function(context, cssFile) {
                var documents = this.collectDocuments(context.window);
                for (var i = 0; i < documents.length; i++) this.replaceMatchingStyleSheetsInDocument(documents[i], cssFile);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateCSS: function(context, cssFiles) {
                dbg('Replacing css files', cssFiles);
                for (var i = 0; i < cssFiles.length; i++)
                {
                    var cssFile = cssFiles[i].replace('\\', '/');
                    // convert windows backslashes to forward slashes
                    this.replaceMatchingStyleSheets(context, cssFile);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            publish: function(event) {
                dbg(">> XRefreshPanel.publish", arguments);
                this.append(event, "blink", null, null);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            clear: function() {
                if (this.panelNode) clearNode(this.panelNode);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            show: function(state) {
                dbg(">> XRefreshPanel.show", arguments);
                var enabled = Firebug.XRefresh.isAlwaysEnabled();
                if (enabled) {
                     Firebug.XRefresh.disabledPanelPage.hide(this);
                     var enabled = true; // Firebug.XRefresh.isEnabled(this.context);
                     this.showToolbarButtons("fbXRefreshMenu", true);
                     this.showToolbarButtons("fbXRefreshControls", enabled);
                     if (this.wasScrolledToBottom)
                     scrollToBottom(this.panelNode);
                } else {
                    this.hide();
                    Firebug.XRefresh.disabledPanelPage.show(this);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            hide: function() {
                dbg(">> XRefreshPanel.hide", arguments);
                this.showToolbarButtons("fbXRefreshMenu", false);
                this.showToolbarButtons("fbXRefreshControls", false);
                this.wasScrolledToBottom = isScrolledToBottom(this.panelNode);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateOption: function(name, value) {
                dbg(">> XRefreshPanel.updateOption", arguments);
                
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getOptionsMenuItems: function() {
                dbg(">> XRefreshPanel.getOptionsMenuItems", arguments);
                return [
                    optionMenu("Use Soft Refresh (if possible)", "softRefresh"),
                    '-',
                    {
                        label: "Visit XRefresh Website...",
                        nol10n: true,
                        command: function() {
                            Firebug.XRefresh.visitWebsite();
                        }
                    }
                ];
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getTopContainer: function() {
                return this.panelNode;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            createRow: function(rowName, className) {
                var elt = this.document.createElement("div");
                elt.className = rowName + (className ? " " + rowName + "-" + className: "");
                return elt;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            append: function(objects, className, rep) {
                var container = this.getTopContainer();
                var scrolledToBottom = isScrolledToBottom(this.panelNode);
                var row = this.createRow("logRow", className);
                this.appendObject.apply(this, [objects, row, rep]);
                container.appendChild(row);
                if (scrolledToBottom) scrollToBottom(this.panelNode);
                return row;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            appendObject: function(object, row, rep) {
                var rep = rep ? rep: Firebug.getRep(object);
                var res = "";
                switch (object.type) {
                    case 0:
                        res = rep.tagRefresh.append({ object: object }, row);
                        break;
                    case 1:
                        res = rep.tagLog.append({ object: object }, row);
                        break;
                }
                if (object.opened) {
                    rep.toggle(row.childNodes[0].childNodes[0]);
                }
                return res;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            applyPanelCSS: function(url) {
                var links = FBL.getElementsBySelector(this.document, "link");
                for (var i=0; i < links.length; i++) {
                    var link = links[i];
                    if (link.getAttribute('href')==url) return; // already applied
                }
                var styleElement = this.document.createElement("link");
                styleElement.setAttribute("type", "text/css");
                styleElement.setAttribute("href", url);
                styleElement.setAttribute("rel", "stylesheet");

                var head = this.getHeadElement(this.document);
                if (head) head.appendChild(styleElement);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getHeadElement: function(doc) {
                var heads = doc.getElementsByTagName("head");
                if (heads.length == 0) return doc.documentElement;
                return heads[0];
            }
        });

        Firebug.registerActivableModule(Firebug.XRefresh);
        Firebug.registerRep(Firebug.XRefresh.XHR);
        Firebug.registerPanel(XRefreshPanel);
    }
}); // close custom scope