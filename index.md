---
title: XRefresh
layout: wikistyle
repo: http://github.com/darwin/xrefresh
support: http://github.com/darwin/xrefresh/issues
download: http://xrefresh.googlecode.com/files/xrefresh-0.7.msi
version: Version 0.7
---

# XRefresh can refresh browser as you modify source files

<img src="/images/welcome.png" width="540" height="184">

XRefresh is a browser plugin which will refresh current web page due to file change in selected folders.

Typical usage scenario is for local web development on a machine with two monitors:

  * Monitor1: browser with current web page being developed
  * Monitor2: editor/IDE, graphical editor and other tools for source editing

When you hit CTRL+S (or whatever key for save), XRefresh will detect it and refresh a web page for you.

## XRefresh components

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
* requires Firefox 1.5 or higher + Firebug 1.2 or higher (http://getfirebug.com)
* runs on any platform supported by Firefox (tested on Windows and OSX 10.5)

### XRefresh addon for Internet Explorer
* browser plugin which listens for monitor requests and performs refresh commands
* requires Internet Explorer 6 or 7
* runs on Windows 2000/2003/XP/Vista (32-bit)

## Installation on Windows

Install [Firebug 1.3][firebug] and then install [XRefresh Addon][addon] (you don't need this step if you are going to use XRefresh with IE only).

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
<img src="/images/ffintro.png" width="417" height="179">

In Interenet Explorer 7 you can find XRefresh icon in the tools in the top right corner (the icon may be hidden in the chevron section).
<img src="/images/ie7toolbar.png" width="306" height="46">

The icon reflects the connection status. You may click it to open XRefresh> Panel
<img src="/images/ie7console.png" width="392" height="136">

## Installation on OSX

Install [Firebug 1.3][firebug] and then install [XRefresh Addon][addon].

Execute ``sudo gem install xrefresh-server``.

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
> One display is fully dedicated to your browser window showing page you are currently editing. With XRefresh you don't need to switch between windows. Stay in your text editor.

#### What does it mean "Fast CSS replace" feature?
> XRefresh is able to replace externally linked CSS file with updated version without reloading whole page. This is handy in dynamic AJAX-style applications. The page stays in same state and does not blink during refresh. This is quite harder to setup and some people reported troubles. This is disabled by default, you need to enable it in XRefresh menu (under icon on Firebug's toolbar). Check [this post](http://getsatisfaction.com/xrefresh/topics/xrefresh_0_7_with_fast_css_update_for_firefox) for more info.

#### Is there a file system monitor available for Unix?
> Michael did some work on [porting it](http://github.com/ycros/xrefresh) over. It is probably not finished, but it should be a piece of cake for hacker like you to make it happen.

#### Is there a support for Safari? Opera?
> No plans, I'm happy with Firefox

#### I'm editing files directly on my server via ssh, is it possible to use XRefresh over network?
> XRefresh monitor communicates with browser extension using TCP/IP. So, it is possible, but it may be tricky because you need to disable firewall and make sure they see each other. By default browser extension tries to connect to 127.0.0.1 on port 41258. In Firefox type ``about:config`` into the URL bar and filter keys by "xrefresh". Keys ``extensions.xrefresh.host`` and ``extensions.xrefresh.localConnectionsOnly`` is what you are looking for.

## Articles about XRefresh

  * [Automated browser refresh addon for Firefox and IE](http://www.ilovecolors.com.ar/automated-browser-refresh-addon-for-firefox-and-ie) by **Elio Rivero**

## History

* **v0.9** (to be released)
  * [[darwin][darwin]] OSX monitor ignores events from .git directories
  * [[darwin][darwin]] XRefresh respects cached resources
  * [[darwin][darwin]] Changed extension guid to xrefresh@hildebrand.cz, compatibility with Firebug 1.3

* **v0.8** (19.07.2008)
  * [[darwin][darwin]] Added OSX support
  * [[darwin][darwin]] Fixed bug in extension networking
  * [[darwin][darwin]] Extension can be enabled/disabled per site (uses new firebug 1.2 feature for this)

* **v0.7** (25.02.2008)
  * [[darwin][darwin]] Fast CSS refresh feature

* **v0.6** (02.01.2008)
  * [[darwin][darwin]] Public release

* **v0.5** (12.11.2007)
  * [[darwin][darwin]] Internal alpha

[darwin]: http://github.com/darwin
[addon]: http://addons.mozilla.org/en-US/firefox/addon/7711/
[download]: http://xrefresh.googlecode.com/files/xrefresh-0.7.msi
[firebug]: http://getfirebug.com