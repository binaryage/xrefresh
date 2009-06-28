// we use UTF-8 encoded JSON to exchange messages between extension and server
//
// this source contains copy&pasted various bits from Firebug sources.

// open custom scope
FBL.ns(function() {
    with(FBL) {
        const Cc = Components.classes;
        const Ci = Components.interfaces;

        const xrefreshPrefService =    Cc["@mozilla.org/preferences-service;1"];
        const socketServer =           Cc["@mozilla.org/network/server-socket;1"];
        const socketTransportService = Cc["@mozilla.org/network/socket-transport-service;1"];
        const inputStream =            Cc["@mozilla.org/scriptableinputstream;1"];
        const inputStreamPump =        Cc["@mozilla.org/network/input-stream-pump;1"];
        const localFile =              Cc["@mozilla.org/file/local;1"];
        const fileInputStream =        Cc["@mozilla.org/network/file-input-stream;1"];

        const nsIPrefBranch  = Ci.nsIPrefBranch;
        const nsIPrefBranch2 = Ci.nsIPrefBranch2;

        const xrefreshPrefs = xrefreshPrefService.getService(nsIPrefBranch2);

        const xrefreshHomepage = "http://xrefresh.binaryage.com";
        
        const messageSeparator = "---XREFRESH-MESSAGE---";

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
        
        // shortcuts (available in this closure):
        var module;   // <-- here will be stored Firebug.XRefresh singleton (extension module)
        var server;   // <-- here will be stored Firebug.XRefreshServer singleton
        var listener; // <-- here will be stored Firebug.XRefreshListener singleton
        
        ////////////////////////////////////////////////////////////////////////
        // Firebug.XRefreshServer is singleton, it keeps system-wide connection to xrefresh server
        //
        server = Firebug.XRefreshServer = {
            // see http://www.xulplanet.com/tutorials/mozsdk/sockets.php
            transport: null,
            outStream: null,
            inStream: null,
            pump: null,
            ready: false,
            name: '',
            version: '',
            data: '',
            /////////////////////////////////////////////////////////////////////////////////////////
            connect: function() {
                dbg(">> XRefreshServer.connect", arguments);

                this.releaseStreams();
                this.data = '';
                
                var transportService = socketTransportService.getService(Ci.nsISocketTransportService);
                this.transport = transportService.createTransport(null, 0, module.getPref("host"), module.getPref("port"), null);
                this.outStream = this.transport.openOutputStream(0, 0, 0);

                var stream = this.transport.openInputStream(0, 0, 0);
                this.inStream = inputStream.createInstance(Ci.nsIScriptableInputStream);
                this.inStream.init(stream);

                var that = this;
                var listener = {
                    onStartRequest: function(request, context) {
                    },
                    onStopRequest: function(request, context, status) {
                        that.onServerDied();
                    },
                    onDataAvailable: function(request, context, inputStream, offset, count) {
                        that.data += that.inStream.read(count);
                        that.onDataAvailable();
                    }
                };

                this.pump = inputStreamPump.createInstance(Ci.nsIInputStreamPump);
                this.pump.init(stream, -1, -1, 0, 0, false);
                this.pump.asyncRead(listener, null);

                this.sendHello();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            disconnect: function() {
                dbg(">> XRefreshServer.disconnect", arguments);
                if (this.ready) {
                    module.log("Disconnected from XRefresh Server", "disconnect");
                    // it is nice to say good bye ...
                    this.sendBye();
                    this.ready = false;
                }
                this.releaseStreams();
                module.updatePanels();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            releaseStreams: function() {
                if (this.inStream) {
                    this.inStream.close();
                    this.inStream = null;
                }
                if (this.outStream) {
                    this.outStream.close();
                    this.outStream = null;
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onServerDied: function() {
                dbg(">> XRefreshServer.onServerDied", arguments);
                if (this.ready) {
                    module.error("XRefresh Server has closed connection");
                    this.ready = false;
                }
                this.releaseStreams();
                module.updatePanels();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onDataAvailable: function() {
                dbg(">> XRefresh.onDataAvailable", arguments);
                // try to parse incomming message
                // here we expect server to send always valid stream of {json1}---XREFRESH-MESSAGE---{json2}---XREFRESH-MESSAGE---{json3}---XREFRESH-MESSAGE---...
                var data = this.data;
                dbg(data||"empty message");
                var parts = data.split(messageSeparator);
                for (var i = 0; i < parts.length-1; i++) {
                    var buffer = UTF8.decode(parts[i]);
                    try {
                        var message = JSON.parse(buffer);
                    } catch (e) {}
                    if (!message) {
                        module.error("Unable to parse server JSON message: "+buffer);
                        continue;
                    }
                    dbg("    message:", message);
                    module.processMessage(message);
                }
                this.data = parts[parts.length-1];
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            send: function(message) {
                dbg(">> XRefresh.send: "+message.command, arguments);
                if (!this.outStream) {
                    dbg("  !! outStream is null", arguments);
                    return false;
                }
                var data = JSON.stringify(message);
                var utf8data = UTF8.encode(data) + messageSeparator;
                this.outStream.write(utf8data, utf8data.length);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            // message helpers
            sendHello: function() {
                var message = {
                    command: "Hello",
                    type: "Firefox",
                    agent: navigator.userAgent.toLowerCase()
                };
                this.send(message);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendBye: function() {
                var message = {
                    command: "Bye"
                };
                this.send(message);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            sendSetPage: function(contentTitle, contentURL) {
                var message = {
                    command: "SetPage",
                    page: contentTitle,
                    url: contentURL
                };
                this.send(message);
            }
        };
        
        ////////////////////////////////////////////////////////////////////////
        // listener is singleton, it keeps system-wide listener to xrefresh server boot
        // when server boots, it pings this listener and extension than can attempt to connect using server
        //
        listener = Firebug.XRefreshListener = {
            server: null, // see http://www.xulplanet.com/tutorials/mozsdk/serverpush.php
            /////////////////////////////////////////////////////////////////////////////////////////
            start: function() {
                dbg(">> XRefreshListener.start", arguments);
                var server = socketServer.createInstance(Ci.nsIServerSocket);
                var range = module.getPref("portRange");
                var port = module.getPref("port");
                var loopbackonly = module.getPref("localOnly");
                var listener = {
                    onSocketAccepted: function(socket, transport) {
                        module.log("Reconnection request received");
                        module.stop();
                        module.start();
                    },
                    onStopListening: function(serverSocket, status) {
                    }
                };
                for (var i = 1; i <= range; i++) {
                    // find some free port below the starting port
                    try {
                        server.init(port - i, loopbackonly, -1);
                        server.asyncListen(listener);
                        this.server = server;
                        return;
                    }
                    catch(e) {}
                }
                // it seems, no port is available
                module.error("No unused port available in given range: " + (port - range) + "-" + (port - 1));
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            stop: function() {
                dbg(">> XRefreshListener.stop", arguments);
                if (!this.server) return;
                this.server.close();
                this.server = null;
            }
        };
        
        ////////////////////////////////////////////////////////////////////////
        // Firebug.XRefresh extension module, here we go!
        //
        module = Firebug.XRefresh = extend(Firebug.ActivableModule, {
            events: [],
            offsets: {},
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
            startupCheck: function() {
                if (!this.checkFirebugVersion()) {
                    this.log("XRefresh Firefox extension works only with Firebug 1.4. Please upgrade Firebug to latest version.", "warn");
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            initialize: function() {
                dbg(">> XRefresh.initialize", arguments);
                this.panelName = 'xrefresh';
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
            onEnabled: function(context) {
                dbg(">> XRefresh.onEnabled", arguments);
                module.start();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onDisabled: function(context) {
                dbg(">> XRefresh.onDisabled", arguments);
                module.stop();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onSuspendFirebug: function(context) {
                dbg(">> XRefresh.onSuspendFirebug", arguments);
                // TODO: onResumeFirebug && onSuspendFirebug is not usable at all, it is called during browser refresh operations WTF?!!
                // if (this.scheduledDisconnection) clearTimeout(this.scheduledDisconnection);
                // this.scheduledDisconnection = setTimeout(function() {
                //     module.stop();
                // }, 5000);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            onResumeFirebug: function(context) {
                dbg(">> XRefresh.onResumeFirebug", arguments);
                // module.start();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            start: function() {
                dbg(">> XRefresh.start", arguments);
                if (this.scheduledDisconnection) clearTimeout(this.scheduledDisconnection);
                delete this.scheduledDisconnection;
                if (this.alreadyActivated) return;
                this.alreadyActivated = true;
                setTimeout(function() {
                    module.startupCheck();
                }, 1000);
                setTimeout(function() {
                    module.connect();
                }, 1500);
                setTimeout(function() {
                    listener.start();
                }, 2000);
                this.checkTimeout = setTimeout(function() {
                    module.connectionCheck();
                }, 5000);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            stop: function() {
                dbg(">> XRefresh.stop", arguments);
                module.disconnect();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            connect: function() {
                dbg(">> XRefresh.connect", arguments);
                server.connect();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            disconnect: function() {
                dbg(">> XRefresh.disconnect", arguments);
                delete this.scheduledDisconnection;
                this.alreadyActivated = false;
                // just after onPanelDeactivate, no remaining activecontext
                server.disconnect();
                listener.stop();
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            connectionCheck: function(context) {
                dbg(">> XRefresh.connectionCheck", arguments);
                delete this.checkTimeout;
                if (module.suppressNextConnectionCheck) {
                    delete module.suppressNextCheck;
                    return;
                }
                if (server.ready) return;
                this.log("Unable to connect to XRefresh Server", "warn");
                this.log("Please check if you have running XRefresh Server.", "bulb");
                this.log("    On Windows, it is program running in system tray. Look for Programs -> XRefresh -> XRefresh.exe", "bulb");
                this.log("    On Mac, it is running command-line program xrefresh-server. It should be available on system path after 'sudo gem install xrefresh-server'", "bulb");
                this.log("You may also want to check your firewall settings. XRefresh Firefox extension expects Server to talk from " + this.getPref('host') + " on port " + this.getPref('port'), "bulb");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            shortConnectionCheck: function(context) {
                dbg(">> XRefresh.shortConnectionCheck", arguments);
                delete this.shortCheckTimeout;
                if (module.suppressNextConnectionCheck) {
                    delete module.suppressNextCheck;
                    return;
                }
                if (server.ready) return;
                this.log("Unable to connect to XRefresh Server. Please check if you have running XRefresh Server and tweak your firewall settings if needed.", "warn");
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
                if (!context) return; // BUG in FB1.4?
                dbg(">> XRefresh.showContext: " + context.window.document.URL);
                Firebug.ActivableModule.showContext.apply(this, arguments);
                this.updatePanel(context);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            loadedContext: function(context) {
                dbg(">> XRefresh.loadedContext: " + context.window.document.URL);
                Firebug.ActivableModule.loadedContext.apply(this, arguments);
                if (!this.isEnabled(context)) return;
                this.restorePageOffset(context);
                this.sendSetPage(context.browser.contentTitle, context.window.document.URL);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonRefresh: function(context) {
                dbg(">> XRefresh.buttonRefresh: " + context.window.document.URL);
                context.getPanel(this.panelName).refresh(context);
                this.log("Manual refresh performed by user", "rreq");
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonClear: function(context) {
                dbg(">> XRefresh.buttonClear: " + context.window.document.URL);
                this.events = [];
                context.getPanel(this.panelName).clear(context);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            buttonStatus: function(context) {
                dbg(">> XRefresh.buttonStatus: " + context.window.document.URL);
                if (this.checkTimeout) clearTimeout(this.checkTimeout);
                delete this.checkTimeout;
                if (this.shortCheckTimeout) clearTimeout(this.shortCheckTimeout);
                delete this.shortCheckTimeout;
                if (server.ready) {
                    this.log("Disconnection requested by user", "disconnect_btn");
                    module.disconnect();
                } else {
                    this.log("Connection requested by user", "connect_btn");
                    module.connect();
                    this.shortCheckTimeout = setTimeout(function() {
                        module.shortConnectionCheck();
                    }, 3000);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getMessageCSSFiles: function(message) {
                var files = [];
                var re = /\.css$/;
                for (var i = 0; i < message.files.length; i++) {
                    var file = message.files[i];
                    if (file.action == 'changed') {
                        if (file.path1.match(re)) {
                            files.push(file.path1);
                        }
                    }
                }
                return files;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            getMessageJSFiles: function(message) {
                var files = [];
                var re = /\.js$/;
                for (var i = 0; i < message.files.length; i++) {
                    var file = message.files[i];
                    if (file.action == 'changed') {
                        if (file.path1.match(re)) {
                            files.push(file.path1);
                        }
                    }
                }
                return files;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            checkServerCompatibility: function() {
                var version = server.version.split('.');
                for (var i=0; i < version.length; i++) {
                    version[i] = parseInt(version[i], 10);
                }
                if (server.name=='OSX xrefresh-server' && version[0]>=0 && version[1]>=2) return true;
                if (server.name=='XRefresh' && version[0]>=1 && version[1]>=0) return true;
                return false;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            processMessage: function(message) {
                dbg(">> XRefresh.processMessage:"+ message.command);
                if (!this.isEnabled(this.currentContext)) {
                    dbg("Skipped message because the panel is not enabled");
                    return;
                }
                switch (message.command) {
                    case 'DoRefresh':
                        if (this.getPref("softRefresh")) {
                            var cssFiles = this.getMessageCSSFiles(message);
                            if (cssFiles.length == message.files.length) {
                                // message contains only CSS files
                                TabWatcher.iterateContexts(function(context) {
                                    module.showEvent(context, message, 'softRefresh');
                                    var panel = context.getPanel(module.panelName);
                                    panel.updateCSS(context, cssFiles, message.contents); // perform soft refresh
                                });

                                return;
                            }
                        }
                        if (this.getPref("softRefreshJS")) {
                            var jsFiles = this.getMessageJSFiles(message);
                            if (jsFiles.length == message.files.length) {
                                // message contains only JS files
                                TabWatcher.iterateContexts(function(context) {
                                    module.showEvent(context, message, 'softRefreshJS');
                                    var panel = context.getPanel(module.panelName);
                                    panel.updateJS(context, jsFiles, message.contents); // perform soft refresh
                                });

                                return;
                            }
                        }
                        TabWatcher.iterateContexts(function(context) {
                            module.showEvent(context, message, 'refresh');
                            var panel = context.getPanel(module.panelName);
                            panel.refresh(context);
                        });
                        break;
                    case 'AboutMe':
                        server.ready = true;
                        server.name = message.agent;
                        server.version = message.version;
                        module.log("Connected to " + server.name + " " + server.version, "connect");
                        if (module.checkServerCompatibility()) {
                            TabWatcher.iterateContexts(function(context) {
                                server.sendSetPage(context.browser.contentTitle, context.window.document.URL);
                            });
                        } else {
                            module.suppressNextConnectionCheck = true;
                            module.error("This server is not compatible with XRefresh Firefox extension, please update it to the latest version.");
                            module.disconnect();
                        }
                        module.updatePanels();
                        break;
                    default: 
                        module.error("Received unexpected command from server: "+message.command);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updatePanels: function() {
                var that = this;
                TabWatcher.iterateContexts(function(context) {
                    that.updatePanel(context);
                });
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
                if (server.ready) {
                    buttonStatus.label = "Connected";
                    buttonStatus.setAttribute('tooltiptext', "Connected to " + server.name + " " + server.version);
                } else {
                    buttonStatus.label = "Disconnected";
                    buttonStatus.setAttribute('tooltiptext', "Disconnected - click to attempt connection");
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
                var event = new XRefreshLogRecord(0, message, icon);
                return this.publishEvent(context, event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            showLog: function(context, text, icon) {
                var event = new XRefreshLogRecord(1, { text: text, time: this.getCurrentTime() }, icon);
                return this.publishEvent(context, event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            log: function(text, icon) {
                if (!icon) icon = "info";
                TabWatcher.iterateContexts(function(context) {
                    module.showLog(context, text, icon);
                });
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            error: function(text, icon) {
                if (!icon) icon = "error";
                TabWatcher.iterateContexts(function(context) {
                    module.showLog(context, text, icon);
                });
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            publishEvent: function(context, event) {
                dbg(">> XRefresh.publishEvent", arguments);
                var panel = context.getPanel(this.panelName);
                if (!panel) {
                    dbg("  !! unable to lookup panel:"+this.panelName);
                    return;
                }
                this.events.push(event);
                panel.publish(event);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            republishAllEvents: function(panel) {
                dbg(">> XRefresh.republishAllEvents", arguments);
                for (var i=0; i < this.events.length; i++) {
                    panel.publish(this.events[i]);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            storePageOffset: function(context) {
                dbg(">> XRefresh.storePageOffset", arguments);
                if (context.browser.webProgress.isLoadingDocument) {
                    dbg("   ! document is still being loaded, skipping ...");
                    return;
                }
                var key = context.window.document.URL;
                var win = context.window;
                this.offsets[key] = [win.pageXOffset, win.pageYOffset];
                dbg("  -> stored: "+this.offsets[key][0]+'x'+this.offsets[key][1]);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            restorePageOffset: function(context) {
                dbg(">> XRefresh.restorePageOffset", arguments);
                var key = context.window.document.URL;
                var data = this.offsets[key];
                if (!data) return;
                var win = context.window;
                win.scrollTo(data[0], data[1]);
                dbg("  -> scroll to:" + data[0] + 'x' + data[1]);
            }
        });

        ////////////////////////////////////////////////////////////////////////
        //
        //
        top.XRefreshLogRecord = function(type, message, icon) {
            this.type = type;
            this.message = message;
            this.opened = false;
            this.icon = icon;
        };

        Firebug.XRefresh.Record = domplate(Firebug.Rep, {
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

                for (var i = 0; i < m.files.length; i++) {
                    var file = m.files[i];
                    fa = file.path1.split('\\');
                    if (fa.length > 0) {
                        fa2 = fa.pop();
                        fa1 = fa.join('\\');
                        if (fa1) fa1 += '\\';

                        if (!file.path2) {
                            s += '<tr><td class="faction ' + file.action + '"></td><td class="ffile"><span class="ffa1">' + fa1 + '</span><span class="ffa2">' + fa2 + '</span></td></tr>';
                        } else {
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
                return object instanceof XRefreshLogRecord;
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
            name: "xrefresh",
            title: "XRefresh",
            searchable: false,
            editable: false,
            wasScrolledToBottom: true,
            /////////////////////////////////////////////////////////////////////////////////////////
            enablePanel: function(module) {
                dbg(">> XRefreshPanel.enablePanel; " + this.context.getName());
                Firebug.ActivablePanel.enablePanel.apply(this, arguments);
                this.clear();
                Firebug.XRefresh.republishAllEvents(this);
                if (this.wasScrolledToBottom)
                    scrollToBottom(this.panelNode);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            disablePanel: function(module) {
                dbg(">> XRefreshPanel.disablePanel; " + this.context.getName());
                Firebug.ActivablePanel.disablePanel.apply(this, arguments);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            initialize: function() {
                dbg(">> XRefreshPanel.initialize", arguments);
                Firebug.ActivablePanel.initialize.apply(this, arguments);
                this.applyCSS();
                Firebug.XRefresh.republishAllEvents(this);
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
                var browser = context.browser;
                var url = context.window.document.location;
                browser.loadURIWithFlags(url, browser.webNavigation.LOAD_IS_REFRESH|browser.webNavigation.LOAD_FLAGS_BYPASS_CACHE|browser.webNavigation.LOAD_FLAGS_BYPASS_PROXY);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateStyleSheet: function(document, element, content) {
                dbg('>> XRefreshPanel.updateStyleSheet', [element, content]);
                var styleElement = document.createElement("style");
                styleElement.setAttribute("type", "text/css");
                styleElement.appendChild(document.createTextNode(content));
                var attrs = ["media", "title", "disabled"];
                for (var i = 0; i < attrs.length; i++) {
                    var attr = attrs[i];
                    if (element.hasAttribute(attr)) styleElement.setAttribute(attr, element.getAttribute(attr));
                }
                element.parentNode.replaceChild(styleElement, element);
                styleElement.originalHref = element.originalHref ? element.originalHref: element.href;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateJavaScript: function(document, element, content) {
                dbg('>> XRefreshPanel.updateJavaScript', [element, content]);
                var styleElement = document.createElement("script");
                styleElement.setAttribute("type", "text/javascript");
                styleElement.appendChild(document.createTextNode(content));
                var attrs = ["media", "title", "disabled", "src"];
                for (var i = 0; i < attrs.length; i++) {
                    var attr = attrs[i];
                    if (element.hasAttribute(attr)) styleElement.setAttribute(attr, element.getAttribute(attr));
                }
                element.parentNode.replaceChild(styleElement, element);
                dbg('>> XRefreshPanel.updateJavaScript: JavaScript Reloaded! ' + element.src);
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
                cssFile = cssFile.replace(/\\/g, '/'); // convert windows backslashes to forward slashes
                var firstQ = cssLink.indexOf('?');
                if (firstQ != -1) cssLink = cssLink.substring(0, firstQ);
                var lastLinkSlash = cssLink.lastIndexOf('/');
                if (lastLinkSlash != -1) cssLink = cssLink.substring(lastLinkSlash + 1);
                var lastFileSlash = cssFile.lastIndexOf('/');
                if (lastFileSlash != -1) cssFile = cssFile.substring(lastFileSlash + 1);
                var res = (cssFile.toLowerCase() == cssLink.toLowerCase());
                dbg('>> XRefreshPanel.Match ' + cssLink + ' vs. ' + cssFile + ' result:' + (res ? 'true': 'false'));
                return res;
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replaceMatchingStyleSheetsInDocument: function(document, cssFile, contents) {
                dbg('>> XRefreshPanel.replaceMatchingStyleSheetsInDocument', arguments);
                var styleSheetList = document.styleSheets;
                for (var i = 0; i < styleSheetList.length; i++) {
                    var styleSheetNode = styleSheetList[i].ownerNode;
                    // this may be <style> or <link> node
                    if (!styleSheetNode) continue;
                    var href = styleSheetNode.href || styleSheetNode.originalHref;
                    if (!href) continue;
                    if (this.doesCSSNameMatch(href, cssFile)) {
                        var content = contents[cssFile];
                        if (!contents) {
                            module.error("Unable to lookup CSS file content for file: "+cssFile);
                            continue;
                        }
                        this.updateStyleSheet(document, styleSheetNode, content);
                    }
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replaceMatchingJavaScriptInDocument: function(document, jsFile, contents) {
                dbg('>> XRefreshPanel.replaceMatchingJavaScriptInDocument', arguments);
                var javascriptList = document.getElementsByTagName('script');
                dbg('>> XRefreshPanel.replaceMatchingJavaScriptInDocument: javascriptList.length = ' + javascriptList.length);
        
                for (var i = 0; i < javascriptList.length; i++) {
                    var javascriptNode = javascriptList[i];
                    var src = javascriptNode.src;
                    if (!src) continue;
                    
                    // prep test vars
                    // convert windows slashes to what the rest of the world uses, basename, convert to lowercase
                    var jsFileTest = jsFile.replace(/\\/g, '/').replace(/.*\//, '').toLowerCase();
                    // basename, convert to lowercase
                    var srcTest = src.replace(/.*\//, '').toLowerCase();
                    
                    dbg('>> XRefreshPanel.replaceMatchingJavaScriptInDocument: compare: ' + jsFileTest + ' vs ' + srcTest);
                    // does the script element src attrib match our filename coming in from the change
                    if (jsFileTest.match(srcTest)) {
                        var content = contents[jsFile];
                        if (!contents) {
                            module.error("Unable to lookup JS file content for file: "+jsFile);
                            continue;
                        }
                        dbg('>> XRefreshPanel.replaceMatchingJavaScriptInDocument: Found Match!');
                        this.updateJavaScript(document, javascriptNode, content);
                    }
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replaceMatchingStyleSheets: function(context, cssFile, contents) {
                var documents = this.collectDocuments(context.window);
                for (var i = 0; i < documents.length; i++){ 
                    this.replaceMatchingStyleSheetsInDocument(documents[i], cssFile, contents);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            replaceMatchingJavaScript: function(context, jsFile, contents) {
                var documents = this.collectDocuments(context.window);
                for (var i = 0; i < documents.length; i++){ 
                    this.replaceMatchingJavaScriptInDocument(documents[i], jsFile, contents);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateCSS: function(context, cssFiles, contents) {
                dbg('>> XRefreshPanel.updateCSS', cssFiles, contents);
                for (var i = 0; i < cssFiles.length; i++) {
                    var cssFile = cssFiles[i];
                    this.replaceMatchingStyleSheets(context, cssFile, contents);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            updateJS: function(context, jsFiles, contents) {
                dbg('>> XRefreshPanel.updateJS', jsFiles, contents);
                for (var i = 0; i < jsFiles.length; i++) {
                    var jsFile = jsFiles[i];
                    this.replaceMatchingJavaScript(context, jsFile, contents);
                }
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            publish: function(event) {
                dbg(">> XRefreshPanel.publish", arguments);
                this.append(event, "blink", null, null);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            clear: function() {
                dbg(">> XRefreshPanel.clear", arguments);
                if (!this.panelNode) return;
                clearNode(this.panelNode);
            },
            /////////////////////////////////////////////////////////////////////////////////////////
            show: function(state) {
                dbg(">> XRefreshPanel.show", arguments);
                var enabled = Firebug.XRefresh.isAlwaysEnabled();
                if (enabled) {
                     Firebug.XRefresh.disabledPanelPage.hide(this);
                     this.showToolbarButtons("fbXRefreshControls", true);
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
                    optionMenu("Use Soft Refresh for CSS", "softRefresh"),
                    optionMenu("Use Soft Refresh for JS", "softRefreshJS"),
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
                    case 0: res = rep.tagRefresh.append({ object: object }, row); break;
                    case 1: res = rep.tagLog.append({ object: object }, row); break;
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
        Firebug.registerRep(Firebug.XRefresh.Record);
        Firebug.registerPanel(XRefreshPanel);
    }
}); // close custom scope