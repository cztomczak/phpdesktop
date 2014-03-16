<?php

$explorer = $_ENV["SYSTEMROOT"] . '\\explorer.exe';
$folder_to_open = "C:\\Windows";
system("$explorer /n,/e,$folder_to_open");

?>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Open folder in Explorer</title>
<h1>Open folder in Explorer</h1>

<pre>
$explorer = $_ENV["SYSTEMROOT"] . '\\explorer.exe';
$folder_to_open = "C:\\Windows";
system("$explorer /n,/e,$folder_to_open");
</pre>
