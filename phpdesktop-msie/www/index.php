<h1>Welcome to PHP Desktop</h1>

<p>Scripts in www/ directory:</h1>

<?php

$files = glob("./*.php");
foreach ($files as $file) {
	$file = basename($file);
	printf("<a href='%s'>%s</a><br>", $file, $file);
}

?>
