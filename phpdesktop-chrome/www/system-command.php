<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>system() command</h1>

<pre style="background:#ddd">
// system() command may hang on Windows XP for some programs,
// wmic.exe is one of them. See Issue 124:
// https://code.google.com/p/phpdesktop/issues/detail?id=124

ob_start();
$ret = system("C:\Windows\System32\wbem\wmic.exe path win32_computersystemproduct get uuid");
$output = ob_get_contents();
ob_end_clean();

if ($ret !== false) {
    echo "Command executed successfully.&lt;br&gt;";
    echo "Output: $output";
} else {
    echo "Command failed.";
}
</pre>

<?php

// system() command may hang on Windows XP for some programs,
// wmic.exe is one of them. See Issue 124:
// https://code.google.com/p/phpdesktop/issues/detail?id=124

ob_start();
$ret = system("C:\Windows\System32\wbem\wmic.exe path win32_computersystemproduct get uuid");
$output = ob_get_contents();
ob_end_clean();

if ($ret !== false) {
    echo "Command executed successfully.<br>";
    echo "Output: $output";
} else {
    echo "Command failed.";
}


?>

