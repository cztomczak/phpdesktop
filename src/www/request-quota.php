<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Request quota</title>
<h1>Request quota</h1>

<p>Request quota for HTML 5 persistent storage.</p>

<script>
function print(msg) {
    console.log(msg+" [JS]");
    msg = msg.replace("ok", "<b style='color:green'>ok</b>");
    msg = msg.replace("error", "<b style='color:red'>error</b>");
    document.getElementById("console").innerHTML += msg+"<br>";
}
function DoRequestQuota() {
    // Request Quota (only for File System API)
    try {
        navigator.webkitPersistentStorage.requestQuota(PERSISTENT, 1024*1024,
                function(bytes){ print("[ok] Granted bytes: "+bytes);},
                function(error){ print("[error] "+error); });
    } catch(e) {
        navigator.webkitPersistentStorage.requestQuota(1024*1024,
                function(bytes){ print("[ok] Granted bytes: "+bytes);},
                function(error){ print("[error] "+error); });
    }
}
</script>

<a href="javascript:DoRequestQuota()">
    DoRequestQuota()</a>
<br><br>

<div id=console></div>
