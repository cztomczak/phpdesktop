<?php

$explorer = $_ENV["SYSTEMROOT"] . '\\explorer.exe';
$folder_to_open = "C:\\Windows";
// Using "system" function would cause a false/positive
// by Bkav antivirus on virustotal.com. Using shell_exec
// instead solves the issue.
shell_exec("$explorer /n,/e,$folder_to_open");

?>

<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Open folder in Explorer</title>
<h1>Open folder in Explorer</h1>

<pre>
$explorer = $_ENV["SYSTEMROOT"] . '\\explorer.exe';
$folder_to_open = "C:\\Windows";
// Using "system" function would cause a false/positive
// by Bkav antivirus on virustotal.com. Using shell_exec
// instead solves the issue.
shell_exec("$explorer /n,/e,$folder_to_open");
</pre>
