---
title: XRefresh = browser refresh automation for web developers
product_title: XRefresh
subtitle: browser refresh automation for web developers
layout: product
icon: /shared/img/xrefresh-icon.png
repo: http://github.com/darwin/xrefresh
support: http://github.com/darwin/xrefresh/issues
downloadtitle: Download v1.4
download: http://addons.mozilla.org/en-US/firefox/addon/7711
subdownload: 
subdownloadlink:
mainshot:
mainshotfull:
overlaysx: 880px
overlaysy: 608px
overlaycx: 25px
overlaycy: 10px
mainright: <object width="470" height="282"><param name="movie" value="http://www.youtube.com/v/6pL6YTWlbI4&hl=cs&fs=1&color1=0x3a3a3a&color2=0x999999&border=1"></param><param name="allowFullScreen" value="true"></param><param name="allowscriptaccess" value="always"></param><embed src="http://www.youtube.com/v/6pL6YTWlbI4&hl=cs&fs=1&color1=0x3a3a3a&color2=0x999999&border=0" type="application/x-shockwave-flash" allowscriptaccess="always" allowfullscreen="true" width="470" height="282"></embed></object><div class="video-note">screencast by <b>Sean Schertell</b>, visit <a href="http://zentools.net/"><b>ZenTools homepage</b></a></div>
mainrightshift: 70px
facebook: 1
retweet: 1
---

<div class="more-box more-box-align" style="margin-top: 60px">
    <div class="tf-ad-2">Looking for XRefresh for iPhone development?</div>
    <div class="tf-ad-3">Then, check out <a href="http://github.com/melito/mish">Mish by Melito</a>!</div>
</div>

## Features

<img class="content-image" src="/images/howto.png" width="200" style="float: left; margin-right: 20px">

XRefresh is a browser plugin which will refresh current web page due to file change in selected folders. This makes it possible to do live page editing with your favorite HTML/CSS editor.

Typical usage scenario is for local web development on a machine with two monitors. The first monitor is for source code editor and the second one is fully dedicated to previewing web page in Firefox. Thanks to XRefresh, page in Firefox is automatically updated with saved changes into source files (html, css, js, images). XRefresh also provides advanced feature "Soft Refresh" which enables you to modify CSS files on-the-fly without full refresh. With this feature you can get similar experience like live editing of CSS in Firebug (without the pain of syncing changes back to your sources).

<p style="clear:both"></p>


## Windows

* install [Firebug 1.4 or higher][firebug] 
* install [XRefresh Addon][addon] (you don't need this step if you are going to use XRefresh with IE only).
* download [Windows Monitor][winmonitor] and go through installation process. It will install XRefresh traybar application and IE plugin.

### Usage

Launch **XRefresh** and check for it in the traybar.  
After start the icon is gray and it means that there are no browsers connected to XRefresh.

<img class="raw-image" src="/images/tray.png" width="121" height="36">

Tell **XRefresh** where are located your files. XRefresh will watch for your modifications.   
Tip: You can simply drag&drop interesting folders onto configuration dialog.

<img class="raw-image" src="/images/config.png" width="479" height="120">

Let **Firefox** or **Internet Explorer** connect to XRefresh.

#### Firefox 
XRefresh has it's own tab panel in Firebug window. You need to enable Firebug for particular site to enable XRefresh functionality.

<a href="/images/ffintro.png"><img class="raw-image" src="/images/ffintro.png" width="600"></a>

#### Interenet Explorer
In Interenet Explorer 7 you can find XRefresh icon in the tools in the top right corner (the icon may be hidden in the chevron section).

<img class="raw-image" src="/images/ie7toolbar.png" width="306" height="46">

The icon reflects the connection status. You may click it to open XRefresh Panel

<img class="raw-image" src="/images/ie7console.png" width="392" height="136">

## OSX

* install [Firebug 1.4 or higher][firebug] 
* install [XRefresh Addon][addon]
* install [Ruby Cocoa][rubycocoa] (filesystem monitoring depends on native Cocoa FSEvent API)
* execute `sudo gem install xrefresh-server --source http://gemcutter.org`.

<img class="shadow" src="/images/osx.png" width="920">

### Usage

After installation xrefresh-server executable should get onto your system path. 
To start server simply run `xrefresh-server` from command-line.

Having problems running it? Maybe RubyCocoa problems some people had on Leopard. Check <a href="http://gist.github.com/158851">http://gist.github.com/158851</a>.

After first run, default config file will be created in `~/.xrefresh-server.yml`:
You are encouraged to modify paths section to map to your working project directories.
 
By default config file is searched first in current directory and then in your home directory.
You may also specify path to config file via `--config` parameter.

## FAQ

#### Why is dual monitor setup great for web development?
> One display is fully dedicated to your browser window showing page you are currently editing. With XRefresh you don't need to switch between windows. Stay in your favorite text editor.

#### What is "Soft Refresh CSS" feature?
> <img src="/images/soft-refresh-checkbox.png" style="float:left; height: 100px; padding-right: 10px"> XRefresh is able to replace externally linked CSS file with updated version without reloading whole page. I call it "soft refresh"  and it is handy in dynamic AJAX-style applications. The page stays in same state and does not blink during refresh. With this feature you can get similar experience like Firebug on-the-fly CSS editing (without the [pain of syncing changes back to original sources](http://code.google.com/p/fbug/issues/detail?id=179)). This is disabled by default, you need to enable it in XRefresh menu (under context menu on XRefresh tab button).
Here is a minimal [example page using this technique][soft-refresh-example], you should be able to see soft refresh icons when modifying style.css.

#### What is "Soft Refresh JS" feature?
> XRefresh is also able to replace externally linked JS file with updated version without reloading whole page. It works similar to Soft Refresh of CSS, but there is a catch. Remember, that updated script is just evaluated as-is in the context of main window (because it is added as a new script tag). So it is not able to remove deleted functions, it will not update anonymous functions bound to elements or for example it will not call jQuery onReady function again. If you don't understand these consequences rather do not enable this feature and go with full refresh. This is disabled by default, you need to enable it in XRefresh menu (under context menu on XRefresh tab button). Here is a minimal [example page using this technique][soft-refresh-example], you should be able to see soft refresh icons when modifying code.js.

#### Is there a file system monitor available for Unix?
> Michael did some work on [porting it](http://github.com/ycros/xrefresh) over. It is probably not finished, but it should be a piece of cake for hacker like you to make it happen.

#### Is there a support for Safari? Opera?
> No plans, I'm happy with Firefox. Support for IE will be dropped in the future. I hate brain-dead IE extension model.

#### I'm editing files directly on my server via ssh, is it possible to use XRefresh over network?
> XRefresh monitor communicates with browser extension using TCP/IP. So, it is possible, but it may be tricky because you need to disable firewall and make sure they see each other. By default browser extension tries to connect to 127.0.0.1 on port 41258. In Firefox type `about:config` into the URL bar and filter keys by "xrefresh". Keys `extensions.xrefresh.host` and `extensions.xrefresh.localConnectionsOnly` is what you are looking for.

#### Do you support @import linked css files in Soft Refresh of CSS?
> No, you have to link all css files directly from root HTML using &lt;link rel="..."&gt; tag. Other kinds of css stylesheets linkage are ignored during soft refresh.

#### I don't see XRefresh panel in IE. What should I try?
> Delete these keys using RegEdit:
`HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Discardable\PostSetup\Component Categories\{00021493-0000-0000-C000-000000000046}`
`HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Discardable\PostSetup\Component Categories\{00021494-0000-0000-C000-000000000046}`

## History

* **v1.4** (09.11.2009)
    * [[darwin][darwin]] tested Firebug 1.5 compatibility
    * [[darwin][darwin]] fixed broken option checkers in panel context menus of Firebug (<a href="http://getsatisfaction.com/binaryage/topics/conflicts_with_firebug_1_5_0b9">more info</a>)

* **v1.3** (09.11.2009)
    * [[darwin][darwin]] fixed crashing of WinMonitor when dealing with unicode filenames (fixes [Issue 7](http://github.com/darwin/xrefresh/issues#issue/7) and [Issue 8](http://github.com/darwin/xrefresh/issues#issue/7))
    * [[darwin][darwin]] tested to work under Firebug 1.5 + Firefox 3.6
    * [[darwin][darwin]] IE8 compatibility

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

## Links

### Articles and Reviews
  * [Automated browser refresh addon for Firefox and IE](http://www.ilovecolors.com.ar/automated-browser-refresh-addon-for-firefox-and-ie) by **Elio Rivero**
  * [Automated Browser Refresher For Developing Easier: XRefresh](http://www.webresourcesdepot.com/automated-browser-refresher-for-developing-easier-xrefresh/) by **Web Resources Depot**
  * [Browser Refresh Automation (XRefresh)](http://brenelz.com/2009/06/19/browser-refresh-automation-xrefresh/) by **Brenley Dueck**

### Software used

  * Great [Silk icons][silk] by **Mark James**
  * FlexControl by **Michael Chapman**
  * [Json.NET](http://james.newtonking.com/projects/json-net.aspx) by **James Newton-King**
  * [XPTable](http://www.codeproject.com/KB/list/XPTable.aspx) by **Mathew Hall**
  * [MozBar](http://www.codeproject.com/KB/menus/MozBar.aspx) by **Patrik Bohman**
  * [UnhandledExceptionDlg](http://www.codeproject.com/KB/exception/UnhandledExceptionClass.aspx) by **Vitaly Zayko**
  * [FirefoxDialog](http://www.codeproject.com/KB/miscctrl/ControlFirefoxDialog.aspx?msg=1856449) by **Rafey**
  * [RubyCocoa](http://rubycocoa.sourceforge.net) by **RubyCocoa Team**

Thanks guys!

[winmonitor]: http://xrefresh.googlecode.com/files/xrefresh-1.3.msi
[blackout]: http://github.com/blackout
[darwin]: http://github.com/darwin
[addon]: http://addons.mozilla.org/en-US/firefox/addon/7711/
[firebug]: https://addons.mozilla.org/en-US/firefox/addons/versions/1843
[firefox]: http://firefox.com
[ie]: http://www.microsoft.com/windows/internet-explorer/default.aspx
[soft-refresh-example]: http://github.com/darwin/xrefresh/tree/master/test
[silk]: http://www.famfamfam.com/lab/icons/silk/
[rubycocoa]: http://rubycocoa.sourceforge.net