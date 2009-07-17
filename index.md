---
title: XRefresh
layout: wikistyle
repo: http://github.com/darwin/xrefresh
support: http://github.com/darwin/xrefresh/issues
download: http://xrefresh.googlecode.com/files/xrefresh-1.2.msi # one more link is down in the list!
version: Version 1.2
---

# XRefresh can refresh browser as you modify source files

<img src="/images/welcome.png" width="540" height="184">

XRefresh is a browser plugin which will refresh current web page due to file change in selected folders.

Typical usage scenario is for local web development on a machine with two monitors:

  * Monitor1: browser with current web page being developed
  * Monitor2: editor/IDE, graphical editor and other tools for source editing

When you hit CTRL+S (or whatever key for save), XRefresh will detect it and refresh a web page for you.

## XRefresh components

First you have a monitor running which is a classic OS application listening to filesystem events and you also need a browser extensions which performs the refresh when file changes are signaled.

### XRefresh Monitor for Windows
* standalone windows traybar application watching for changes in selected folder(s)
* requires .NET Framework 2.0 or higher
* runs on Windows 2000/2003/XP/Vista (32-bit)

### XRefresh Monitor for OSX
* command-line server application watching for changes in selected folder(s)
* requires OSX 10.5 (Leopard) or higher
* requires Ruby and rubygems (this is installed on Leopard by default)

### XRefresh extension for Firefox
* browser plugin which listens for monitor requests and performs refresh commands
* requires [Firefox 3.0 or higher][firefox] + [Firebug 1.4 or higher][firebug]
* runs on any platform supported by Firefox (tested on Windows and OSX 10.5)

### XRefresh addon for Internet Explorer
* browser plugin which listens for monitor requests and performs refresh commands
* requires [Internet Explorer 6, 7 or 8][ie]
* runs on Windows 2000/2003/XP/Vista (32-bit)

## Installation on Windows

Install [Firebug 1.4][firebug] and then install [XRefresh Addon][addon] (you don't need this step if you are going to use XRefresh with IE only).

Download latest [windows installer][download] and go through installation process. It will install XRefresh traybar application and IE plugin.

<img src="/images/howto.png">

#### Usage

Launch **XRefresh** and check for it in the traybar.  
After start the icon is gray and it means that there are no browsers connected to XRefresh.

<img src="/images/tray.png" width="121" height="36">

Tell **XRefresh** where are located your files. XRefresh will watch for your modifications.   
Tip: You can simply drag&drop interesting folders onto configuration dialog.

<img src="/images/config.png" width="479" height="120">

Let **Firefox** or **Internet Explorer** connect to XRefresh.

XRefresh has it's own tab panel in Firebug window. You need to enable Firebug for particular site to enable XRefresh functionality.
<a href="/images/ffintro.png"><img src="/images/ffintro.png" width="600"></a>

In Interenet Explorer 7 you can find XRefresh icon in the tools in the top right corner (the icon may be hidden in the chevron section).
<img src="/images/ie7toolbar.png" width="306" height="46">

The icon reflects the connection status. You may click it to open XRefresh Panel
<img src="/images/ie7console.png" width="392" height="136">

## Installation on OSX

  * Install [Firebug 1.4][firebug] 
  * Install [XRefresh Addon][addon]
  * Install [Ruby Cocoa][rubycocoa] (filesystem monitoring depends on native Cocoa FSEvent API)
  * Execute ``sudo gem install xrefresh-server``.

<img class="shadow" src="/images/osx.png">

#### Usage

After installation xrefresh-server executable should get onto your system path. 
To start server simply run ``xrefresh-server`` from command-line.

After first run, default config file will be created in ``~/.xrefresh-server.yml``:

    # here specify list of paths to monitor
    paths:
      - /Users/<your-user-name> # by default watch user's home directory
    # - /you/may/add/here/some/other/path
    # - /you/may/add/here/some/another/path

    # you can various filters (ruby regexp pattern)
    # every file is split to dir and file part (for example /Users/mick/proj/coolapp and some_file.rb)
    #   both include filters must be satisfied
    #   both exclude filters must not be satisfied
    # empty value means "apply no filtering"
    dir_include:
    dir_exclude: ^/Users/<your-user-name>/Library|/\.(svn|framework|app|pbproj|pbxproj|xcode(proj)?|bundle)/
    file_include:
    file_exclude: ^(CVS|SCCS|vssver.?.scc|\.(cvsignore|svn|DS_Store)|_svn|Thumbs\.db)$|~$|^(\.(?!htaccess)[^/]*|\.(tmproj|o|pyc)|svn-commit(\.[2-9])?\.tmp)$ # merged TextMate and Netbeans patterns

    # xpert settings
    host: 127.0.0.1
    port: 41258 # known port for clients to connect 
    max_connections: 4 # max client connections
    debug: false # run in debug mode?
    audit: false # audit server activity
    defer_time: 0.5 # aggregation time for events
    sleep_time: 0.1 # don't hung cpu in main loop
 
As you can see, by default monitor watches your home directory excluding all under ~/Library.
You are encouraged to modify paths section to map to your working project directories.
 
By default config file is searched first in current directory and then in your home directory.
You may also specify path to config file via --config parameter.

## FAQ

#### Why is dual monitor setup great for web development?
> One display is fully dedicated to your browser window showing page you are currently editing. With XRefresh you don't need to switch between windows. Stay in your favorite text editor.

#### What is "Soft Refresh CSS" feature?
> XRefresh is able to replace externally linked CSS file with updated version without reloading whole page. I call it "soft refresh"  and it is handy in dynamic AJAX-style applications. The page stays in same state and does not blink during refresh. With this feature you can get similar experience like Firebug on-the-fly CSS editing (without the [pain of syncing changes back to original sources](http://code.google.com/p/fbug/issues/detail?id=179)). This is disabled by default, you need to enable it in XRefresh menu (under context menu on XRefresh tab button). Here is a minimal [example page using this technique][soft-refresh-example], you should be able to see soft refresh icons when modifying style.css.

#### What is "Soft Refresh JS" feature?
> XRefresh is also able to replace externally linked JS file with updated version without reloading whole page. It works similar to Soft Refresh of CSS, but there is a catch. Remember, that updated script is just evaluated as-is in the context of main window (because it is added as a new script tag). So it is not able to remove deleted functions, it will not update anonymous functions bound to elements or for example it will not call jQuery onReady function again. If you don't understand these consequences rather do not enable this feature and go with full refresh. This is disabled by default, you need to enable it in XRefresh menu (under context menu on XRefresh tab button). Here is a minimal [example page using this technique][soft-refresh-example], you should be able to see soft refresh icons when modifying code.js.

#### Is there a file system monitor available for Unix?
> Michael did some work on [porting it](http://github.com/ycros/xrefresh) over. It is probably not finished, but it should be a piece of cake for hacker like you to make it happen.

#### Is there a support for Safari? Opera?
> No plans, I'm happy with Firefox. Support for IE will be dropped in the future. I hate brain-dead IE extension model.

#### I'm editing files directly on my server via ssh, is it possible to use XRefresh over network?
> XRefresh monitor communicates with browser extension using TCP/IP. So, it is possible, but it may be tricky because you need to disable firewall and make sure they see each other. By default browser extension tries to connect to 127.0.0.1 on port 41258. In Firefox type ``about:config`` into the URL bar and filter keys by "xrefresh". Keys ``extensions.xrefresh.host`` and ``extensions.xrefresh.localConnectionsOnly`` is what you are looking for.

#### Do you support @import linked css files in Soft Refresh of CSS?
> No, you have to link all css files directly from root HTML using &lt;link rel="..."&gt; tag. Other kinds of css stylesheets linkage are ignored during soft refresh.

## Articles about XRefresh

  * [Automated browser refresh addon for Firefox and IE](http://www.ilovecolors.com.ar/automated-browser-refresh-addon-for-firefox-and-ie) by **Elio Rivero**

## Contributors
  * I've used great [Silk icons by Mark James][silk]

## History

* **v1.2** (09.07.2009)
  * [[darwin][darwin]] Firefox: Soft Refresh works with CSS files using relative paths ([Issue 4](http://github.com/darwin/xrefresh/issues#issue/4))
  * [[darwin][darwin]] Firefox: Soft Refresh reloads CSS from server, so it works with CSS preprocessors like sass or less

* **v1.1** (29.06.2009)
  * [[blackout][blackout]] Firefox: added Soft Refresh functionality for script files
  * [[blackout][blackout]] Firefox: fixed Soft Refresh bug on Windows ([Issue 3](http://github.com/darwin/xrefresh/issues#issue/3))

* **v1.0.2** (26.06.2009)
  * [[darwin][darwin]] Firefox: refresh correctly bypasses cache
  * [[darwin][darwin]] Firefox: document scroll position is restored after refresh

* **v1.0.1** (21.06.2009)
  * [[darwin][darwin]] fixed fatal bug in 1.0 release (Windows only: communication failed when updating CSS files) - thanks Alejandro Torres for tracking this down

* **v1.0** (21.06.2009)
  * [[darwin][darwin]] compatibility with Firebug 1.4 (unfortunately changes are not backward compatible for older Firebug releases)
  * [[darwin][darwin]] more robust communication protocol (should solve occasionally broken connections)
  * [[darwin][darwin]] Soft Refresh can be used over network (as a side product fixed strange Firefox bugs when reading files from local filesystem)
  * [[darwin][darwin]] fixed problem with Helvetica font on localized Spain Windows
  * [[darwin][darwin]] changed extension guid back to xrefresh@xrefresh.com to continue in original project at [addons.mozilla.org][addon]

* **v0.9** (never packaged)
  * [[darwin][darwin]] OSX monitor ignores events from .git directories
  * [[darwin][darwin]] XRefresh respects cached resources
  * [[darwin][darwin]] changed extension guid to xrefresh@hildebrand.cz, compatibility with Firebug 1.3

* **v0.8** (19.07.2008)
  * [[darwin][darwin]] added OSX support
  * [[darwin][darwin]] fixed bug in extension networking
  * [[darwin][darwin]] extension can be enabled/disabled per site (uses new firebug 1.2 feature for this)

* **v0.7** (25.02.2008)
  * [[darwin][darwin]] "Soft Refresh" feature

* **v0.6** (02.01.2008)
  * [[darwin][darwin]] public release

* **v0.5** (12.11.2007)
  * [[darwin][darwin]] internal alpha

[blackout]: http://github.com/blackout
[darwin]: http://github.com/darwin
[addon]: http://addons.mozilla.org/en-US/firefox/addon/7711/
[download]: http://xrefresh.googlecode.com/files/xrefresh-1.2.msi
[firebug]: https://addons.mozilla.org/en-US/firefox/addons/versions/1843
[firefox]: http://firefox.com
[ie]: http://www.microsoft.com/windows/internet-explorer/default.aspx
[soft-refresh-example]: http://github.com/darwin/xrefresh/tree/master/test
[silk]: http://www.famfamfam.com/lab/icons/silk/
[rubycocoa]: http://rubycocoa.sourceforge.net