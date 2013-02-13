<a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Welcome to PHP Desktop</title>
<h1>Welcome to PHP Desktop</h1>

<p>Scripts in www/ directory:</h1>

<?php

$files = glob("./*");
foreach ($files as $file) {
	$file = basename($file);
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


