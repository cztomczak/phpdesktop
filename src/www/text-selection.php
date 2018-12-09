<html>
<head>
    <title>Test text selection</title>
</head>
<body onselectstart="return false;" style="cursor: default;">

<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<h1>Test text selection</h1>

<p>Selection of this text should be disabled.</p>

<p>The code for this:</p>

<pre>
&lt;body onselectstart="return false;" style="cursor: default;"&gt;
</pre>

</body>
</html>
