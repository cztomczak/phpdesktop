<?php

header($_SERVER['SERVER_PROTOCOL'] . ' 500 Internal Server Error', false, 500);

?>

<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Error 500</title>
<h1>Error 500</h1>

<p>
    Sent header: 500 Internal Server Error.<br>
    If you're seeing this then all is OK.
</p>
