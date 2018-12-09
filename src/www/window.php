
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Test window</title>
<h1>Test window</h1>

<h2>window.resizeTo(), window.moveTo()</h2>

<a href="#" onclick="window.resizeTo(640, 480)">
    window.resizeTo(640, 480)
</a>

<br><br>

<a href="#" onclick="window.moveTo(0, 0)">
    window.moveTo(0, 0)
</a>

<h2>window.alert()</h2>

<a href="#" onclick="window.alert(1)">
    window.alert(1)
</a>

<p>Set "hide_dialog_boxes" to true in settings.json to disable
    alerts.</p>

<h2>window.close() - do not use</h2>

<p>window.close() does not work as expected, do not use it, you have to call
window.external.CloseWindow() instead, try it: </p>

<p>
    <a href="#"
        onclick="window.external.CloseWindow()">window.external.CloseWindow()</a>
</p>

<p>You might override window.close() implementation using this code:</p>

<pre style="background: #ddd;">
window.close = function() {
    window.external.CloseWindow();
}
</pre>
