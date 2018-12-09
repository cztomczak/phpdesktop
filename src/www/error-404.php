<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Error 404</title>
<h1>Error 404</h1>

<p>
    Cick the non-existing url below. If ["web_server"]["404_handler"]
    is set in settings.json file then clicking the link below
    will forward you to "/pretty-urls.php/foo.html". You can then
    access the "/foo.html" url using $_SERVER["PATH_INFO"] or
    $_SERVER["REQUEST_URI"].
</p>
<p>Try it:
    <ul>
    <li><a href="/company.html">/company.html</a>
    <li><a href="/company.html?xyz=1">/company.html?xyz=1</a>
    </ul>
</p>
