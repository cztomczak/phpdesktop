# PHP Desktop


Table of contents:
* [Introduction](#introduction)
* [Downloads](#downloads)
* [Support](#support)
* [Fundings for new features](#fundings-for-new-features)
* [Support development](#support-development)
* [Seeking sponsors](#seeking-sponsors)


## Introduction

PHP Desktop is an open source project founded by Czarek Tomczak
in 2012 to provide a way for developing native desktop GUI applications
using web technologies such as PHP, HTML5, JavaScript and SQLite. Think
of it as Electron for PHP. It is a convienient tool for converting PHP
web apps and PHP CLI tools to desktop applications with little effort.
The development workflow you are used to while creating web applications
remains the same, there is no new framework / API to learn. The process
of turning an existing website into a desktop application is basically
a matter of copying it to the "phpdesktop/www/" directory.

In a certain sense phpdesktop acts as a PHP to EXE compiler. It embeds
a web browser, a multi-threaded web server and a PHP interpreter. All
embedded into a single application, a portable folder that you can easily
distribute to end users by packing it to zip archive or by making an
[installer for your application](../../wiki/Knowledge-Base#application-installer).
The web server embedded is a custom edition of
[Mongoose](https://en.wikipedia.org/wiki/Mongoose_(web_server)),
a web server used by NASA on the International Space Station.
Supported browsers are Internet Explorer and Google Chrome via
open source Chromium. The package with Chrome embedded has no
external dependencies, everything is included in phpdesktop binaries
and works out of the box on a user's computer.

All popular PHP frameworks are supported, see the
[PHP frameworks support](../../wiki/PHP-frameworks-support) wiki page
for example configurations for CakePHP, CodeIgniter, Laravel, Symfony, Yii
and Zend Framework.  You can create a standalone executable for
distribution with the help of the
[Inno Setup installer](../../wiki/Knowledge-Base#application-installer).
PHP sources can be protected with the many of the available
[PHP encoders](../../wiki/Knowledge-Base#how-do-i-protect-php-sources-in-the-www-directory).
PHP Desktop is released under non-restrictive license, thus it is
[free for commercial use](../../wiki/Knowledge-Base#can-i-use-php-desktop-in-a-commercial-closed-sourced-project).

It is one of the top goals for PHP Desktop to be stable, to work reliably.
PHP Desktop does not suffer from memory leaks. PHP by design was never
intended for running long hours/days, as desktop applications usually do.
This is not a concern when using PHP Desktop, as it is running an internal
web server and serving pages through CGI. So when PHP script execution
ends, PHP-CGI process is killed and all memory is always freed.

Lots of other useful information can be found on the
[Knowledge Base](../../wiki/Knowledge-Base) wiki page,
[Discussions page](../../discussions) and on the
[Old Forum](https://groups.google.com/d/forum/phpdesktop).


## Downloads

  * Windows
    - [PHP Desktop Chrome 130.1 for Windows](https://github.com/cztomczak/phpdesktop/releases/tag/chrome-v130.1) release
  * Mac
    - [PHP Desktop Chrome 130.1 for Mac](https://github.com/cztomczak/phpdesktop/releases/tag/mac-v130.1) release
  * Linux
    - [PHP Desktop Chrome 72.1 for Linux](https://github.com/cztomczak/phpdesktop/releases/tag/linux-v72.0) release
  
There are two ways to be notified about new releases:
- Watch the project with the "Releases only" option checked
- Subscribe to the [RSS/Atom feed](https://github.com/cztomczak/phpdesktop/releases.atom)


## Support

* Documentation is on the [Wiki Pages](../../wiki). Start with the
  [Knowledge Base](../../wiki/Knowledge-Base), [Settings](../../wiki/Settings)
  and [Chrome settings](../../wiki/Chrome-settings) wiki pages.
* Ask questions and report problems on the
  [Discussions page](../../discussions)
* See also this externally maintained tutorial:
  [Create your first Desktop Application with PHP and PHP Desktop](https://lancecourse.com/en/howto/create-your-first-desktop-application-with-php-and-php-desktop)


## Fundings for new features

Here is a list of ongoing fundings for new features in PHP Desktop:

Feature | Announced | Status
--- | --- | ---
New Linux release with latest Chromium | [Discussion #359](https://github.com/cztomczak/phpdesktop/discussions/359) | Ongoing funding 


## Support development

If you would like to support PHP Desktop general development efforts by
making a donation please click the PayPal Donate button below:

[![Donate](https://raw.githubusercontent.com/wiki/cztomczak/phpdesktop/donate.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JQSTPDRRM8AQ8&no_shipping=1)


## Seeking sponsors

PHP Desktop is seeking companies to sponsor further development of the project.
There are many proposals for new features submitted in the issue tracker and
on wiki pages. Most notable are:

* Monthly releases with latest Chrome
* An automated build system
* Cross-platform support with full functionality on Mac and Linux
* More Javascript and PHP API exposed for accomplishing desktop specific tasks
* Better integration with desktop, taskbar and systray
* HTTPS/SSL support
* Custom window themes
* Running app in background
* SQLite encryption
* Desktop notifications

If your company would like to sponsor PHP Desktop development efforts
then please contact [Czarek](https://www.linkedin.com/in/czarektomczak/).
Long term sponsorships are welcome and Czarek is open to ideas about
the project. He would love to spend more time on developing this project,
but he can't afford doing so in his free time.
