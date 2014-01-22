<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Test JavaScript errors</title>
<h1>Test JavaScript errors</h1>

<script>
function EnableErrorHandler() {
    document.getElementById("error_box").style.display = "none";
    // When you set "window.onerror" then a dialog box asking
    // whether to show script debugger won't show anymore.
    window.onerror = function(message, url, line){
        document.getElementById("error_box").style.display = "block";
        document.getElementById("message").innerHTML = message;
        document.getElementById("url").innerHTML = url;
        document.getElementById("line").innerHTML = line;
        return true;
    }
}
function DisableErrorHandler() {
    document.getElementById("error_box").style.display = "none";
    window.onerror = null;
}
</script>

<h2>Enable or disable error handler</h2>

<a href="#"
    onclick="EnableErrorHandler()">EnableErrorHandler()</a>

<br><br>

<a href="#"
    onclick="DisableErrorHandler()">DisableErrorHandler()</a>

<h2>Test error</h2>

<a href="#"
    onclick="NonExistent()">NonExistent()</a>

<div style="background-color: rgb(250, 200, 200); margin-top: 1.5em;"
    id="error_box" style="display: none;">
    <b>Error handled</b><br>
    <b>Message:</b> <span id="message"></span><br>
    <b>Url:</b> <span id="url"></span><br>
    <b>Line:</b> <span id="line"></span><br>
</div>
