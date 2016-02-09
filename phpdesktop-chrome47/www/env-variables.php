<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Environment variables</title>
<h1>Environment variables</h1>

<pre>
<?php print_r($_ENV); ?>
</pre>