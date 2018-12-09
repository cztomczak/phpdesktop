<style type="text/css">@import url("style.css");</style>
<a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>PHP Desktop Chrome</title>
<h1>PHP Desktop Chrome</h1>

<p>
<b>PHP Desktop version:</b> <?php echo $_ENV["PHPDESKTOP_VERSION"]; ?>
</p>

<p>
<b>Chrome version:</b> <script>document.write(navigator.userAgent);</script>
</p>

<p>
<b>PHP version:</b> <?php echo phpversion(); ?>
</p>

<p>
<b>NOTES:</b>
<ul>
<li>To navigate back and forward use mouse context menu</li>
<li>To disable console window, edit the settings.json file
and change the "show_console" setting to false.</li>
</ul>
</p>

<hr>

<p>Test scripts in the www/ directory:</p>

<?php

$files = glob("./*");
foreach ($files as $file) {
	$file = basename($file);
    if ($file and $file[0] == "_") {
        continue;
    }
	printf("<a href='%s'>%s</a><br>", $file, $file);
}

?>

<hr>
Project website:
<a href="http://github.com/cztomczak/phpdesktop">
    http://github.com/cztomczak/phpdesktop
</a>
