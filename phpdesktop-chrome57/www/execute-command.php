<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<title>Execute command</title>
<h1>Execute command</h1>

<pre>
// Execution of commands may hang on Windows XP for some programs,
// wmic.exe is one of them. The solution is to use a command like
// this "echo | command...". See Issue 124:
// https://code.google.com/p/phpdesktop/issues/detail?id=124

$output = shell_exec("echo | {$_ENV['SYSTEMROOT']}\System32\wbem\wmic.exe path win32_computersystemproduct get uuid");

if ($output) {
    echo "Command executed successfully.&lt;br&gt;";
    echo "Output: $output";
} else {
    echo "Command failed.";
}
</pre>

<?php

// Execution of commands may hang on Windows XP for some programs,
// wmic.exe is one of them. The solution is to use a command like
// this "echo | command...". See Issue 124:
// https://code.google.com/p/phpdesktop/issues/detail?id=124

$output = shell_exec("echo | {$_ENV['SYSTEMROOT']}\System32\wbem\wmic.exe path win32_computersystemproduct get uuid");

if ($output) {
    echo "Command executed successfully.<br>";
    echo "Output: $output";
} else {
    echo "Command failed.";
}

?>

