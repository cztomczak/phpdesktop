<?php

$temp = __DIR__."/temp";
putenv("TMP=$temp");
putenv("TEMP=$temp");
putenv("TMPDIR=$temp");

?>

<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>TMP directory</h1>

<p>
A few calls to putenv() overwrite the default directory,
call sys_get_temp_dir() to see it if was set successfully.
</p>

<p>You can also set the temp directory using the "cgi_temp_dir"
option in the settings.json file.</p>

<h2>sys_get_temp_dir()</h2>

<pre>
<?php echo sys_get_temp_dir(); ?>
</pre>
