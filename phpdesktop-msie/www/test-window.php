
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Test window</title>
<h1>Test window</h1>

<a href="javascript:void(0)" onclick="window.resizeTo(640, 480)">
    window.resizeTo(640, 480)
</a>

<br><br>

<a href="javascript:void(0)" onclick="window.moveTo(0, 0)">
    window.moveTo(0, 0)
</a>

<h2>Test alert</h2>

<a href="javascript:void(0)" onclick="window.alert(1)">
    window.alert(1)
</a>

<p>Set "hide_dialog_boxes" to true in settings.json to disable
    alerts.</p>
