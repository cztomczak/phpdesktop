<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>Loading error</h1>

<p>
    Try loading a non-existent url. This is an http protocol
    so it will be handled by the web server:
    <a href="/non-existent.html">non-existent.html</a>
</p>

<p>
    A custom scheme loading error will be handled by Chrome:
    <a href="asd://non-existent.html">asd://non-existent.html</a><br>
    In a popup window:
    <a target="_blank" href="asd://non-existent.html">asd://non-existent.html</a>
</p>