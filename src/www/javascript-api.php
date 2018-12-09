<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<h1>Javascript API</h1>

<p>PHP Desktop exposes its API through the javascript "phpdesktop"
object that is binded to the window object.</p>

<a href="javascript:alert(phpdesktop.GetVersion())">
    phpdesktop.GetVersion()</a>

<p>Get the PHP Desktop version. This value is taken from the version
info embedded in the phpdesktop executable.</p>

<a href="javascript:phpdesktop.ToggleFullscreen()">
    phpdesktop.ToggleFullscreen()</a>

<p>To go into fullscreen mode or exit fullscreen mode.</p>

<a href="javascript:alert(phpdesktop.IsFullscreen())">
    phpdesktop.IsFullscreen()</a>

<p>Whether in fullscreen mode.</p>
