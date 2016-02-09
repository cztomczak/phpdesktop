<?php

error_reporting(-1);

if (isset($_GET["getit"])) {
    header('Content-type: text/plain');
    header('Content-Disposition: attachment; filename="phpdesktop.txt"');
    print("This file was downloaded using the PHP Desktop browser!");
    exit();
}

?>

<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>Download</h1>

<p>You can disable ability to download files by setting
the "enable_downloads" option to false in settings.json.</p>

<p>
    Download file:
    <a href="download.php?getit=1">phpdesktop.txt</a>
</p>
