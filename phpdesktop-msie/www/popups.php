<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Test popups</title>
<h1>Test popups</h1>

<p>
    Only width/height/top/left properties passed to window.open()
    are currently implemented. Changing others, such as 'location',
    'resizable' won't have any effect.
</p>

<p>
    The title of the popup window will be set according to the meta
    &lt;title&gt; tag. If there is none, title of the main window
    will be used.
</p>

<script>
function CreatePopup(url, name) {
    // See window.open() in MSDN documentation:
    // http://msdn.microsoft.com/en-us/library/ie/ms536651(v=vs.85).aspx
    if (!name)
        name = '';
    features = 'width=800,height=600'; // top,left
    features += 'location=0,menubar=0,toolbar=0,status=0,fullscreen=0,';
    features += 'resizable=1,scrollbars=1,';
    replace = false;
    return window.open(url, name, features, replace);
}
</script>

<p>
    <a href="#" onclick="CreatePopup('/phpinfo.php')">
        CreatePopup('/phpinfo.php')
    </a>
</p>

<p>Javascript code:</p>

<pre style="background: #ddd;">
function CreatePopup(url, name) {
    // See window.open() in MSDN documentation:
    // http://msdn.microsoft.com/en-us/library/ie/ms536651(v=vs.85).aspx
    if (!name)
        name = '';
    features = 'width=800,height=600'; // top,left
    features += 'location=0,menubar=0,toolbar=0,status=0,fullscreen=0,';
    features += 'resizable=1,scrollbars=1,';
    replace = false;
    return window.open(url, name, features, replace);
}
</pre>
