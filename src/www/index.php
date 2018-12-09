<a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Welcome to PHP Desktop</title>
<h1>Welcome to PHP Desktop</h1>

<p>Scripts in the www/ directory:</h1>

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

<h2>Notes</h2>

<ul>
<li>To disable debugging console window, edit the settings.json
    file and change the "show_console" setting to false.</li>
</ul>

<hr>
Project website:
<a href="http://code.google.com/p/phpdesktop/">
    http://code.google.com/p/phpdesktop/
</a>

<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>
<br><br><br><br>


