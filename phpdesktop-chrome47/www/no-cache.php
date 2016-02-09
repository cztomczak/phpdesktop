<?php

error_reporting(-1);

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Cache-Control: no-cache, no-store, must-revalidate");
header("Pragma: no-cache");

?>

<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>No cache</title>
<h1>No cache</h1>

<p>This example uses PHP header() calls to set cache control.</p>

Some random number:
<?php echo rand(1, 1000); ?>

<p>
    Go back and forward using mouse context menu to see if this number changed,
    if so the cache was disabled successfully.
</p>

<p>
    To avoid caching of static resources put some random query string
    when including js/css files, see for example:
</p>

<pre>
&lt;link href="css/style.css?r=&lt;?php echo time(); ?&gt;" rel="stylesheet" type="text/css" /&gt;
</pre>