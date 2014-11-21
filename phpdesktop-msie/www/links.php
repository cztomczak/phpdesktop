<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Test links</title>
<h1>Test links</h1>

<h2>External links</h2>

External links should open in OS default browser:<br>
<a href="http://www.google.com/">http://www.google.com/</a>

<h2>target=_blank</h2>

<p>
    Target _blank should open in a popup window, test it:
    <a target=_blank href="/index.php">index.php</a>
</p>

<pre style="background: #ddd;">
&lt;a target=_blank href="/index.php"&gt;index.php&lt;/a&gt;
</pre>
