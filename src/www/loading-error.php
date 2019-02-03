<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>Loading error</h1>

<p>
    Try loading a non-existent url. This is an http protocol
    so it will be handled by the web server. In ours case
    it is pretty-urls.php script that will handle the 404
    error, as it is configured via "404_handler" in settings.json
    file.
    <br><br>
    <a href="/non-existent.html">non-existent.html</a>
</p>
