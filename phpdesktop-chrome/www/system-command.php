<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>system() command</h1>

<?php

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