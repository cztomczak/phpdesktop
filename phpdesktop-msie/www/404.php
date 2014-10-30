<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Test 404</title>
<h1>Test 404</h1>

<p>
See the "msie" &gt; "error_page" option in settings.json.<br>
See also the "web_server" &gt; "404_handler" option in settings.json.<br>
When the "404_handler" option is set, it will take precedence over
the "error_page" option.
</p>

Test 404 navigation error:
<a href="non_existent.html">non_existent.html</a>
