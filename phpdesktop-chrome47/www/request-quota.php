<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Request quota</title>
<h1>Request quota</h1>

<p>Request quota for HTML 5 persistent storage.</p>

<script>
function DoRequestQuota() {
    // Request Quota (only for File System API)
    try {
        navigator.webkitPersistentStorage.requestQuota(PERSISTENT, 1024*1024,
                function(bytes){ window.alert("Granted bytes: "+bytes);},
                function(error){ window.alert(error); });
    } catch(e) {
        navigator.webkitPersistentStorage.requestQuota(1024*1024,
                function(bytes){ window.alert("Granted bytes: "+bytes);},
                function(error){ window.alert(error); });
    }
}
</script>

<a href="javascript:DoRequestQuota()">
    DoRequestQuota()</a>
