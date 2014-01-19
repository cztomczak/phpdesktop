<a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Welcome to PHP Desktop Chrome</title>
<h1>Welcome to PHP Desktop Chrome</h1>

<p>Scripts in www/ directory:</p>

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
<br><hr>
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


