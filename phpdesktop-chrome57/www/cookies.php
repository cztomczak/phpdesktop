<?php

error_reporting(-1);
if (isset($_GET["create"]) && $_GET["create"]) {
    $rand = rand(1, 1000);
    setcookie("test-".$rand, "1", time()+3600*24);
    header("Location: cookies.php");
    exit();
}

?>

<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>Cookies</h1>

<p><a href="cookies.php?create=1">Create a random cookie</a></p>

<p>Total cookies: <b><?php echo count($_COOKIE); ?></b></p>

<pre><?php print_r($_COOKIE); ?></pre>
