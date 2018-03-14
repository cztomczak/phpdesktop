<?php

error_reporting(-1);

ini_set('zlib.output_compression', 0);
ini_set('output_buffering', 0);
ini_set('implicit_flush', 1);

// This buffer length value is copied from "mongoose.c" file.
// If you would like to reduce buffer size then modify "mongoose.c"
// file and rebuild phpdesktop from sources.
define("MG_BUF_LEN", 8192);

function fprint($s)
{
    $args = func_get_args();
    call_user_func_array('printf', $args);
    print(str_repeat(" ", MG_BUF_LEN));
    @ob_flush();
    flush();
}

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Cache-Control: no-cache, no-store, must-revalidate");
header("Pragma: no-cache");

?>

<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Output buffer flush</title>
<h1>Output buffer flush</h1>

<p>
    This example forces flush of output buffer repeatedly.
</p>
<p>
    This technique works differently with Mongoose web server.
    Mongoose forces to always read 8192 characters (MG_BUF_LEN)
    before sending output. The solution is to output 8192 spaces
    before each ob_flush / flush calls. Spaces are ignored in html,
    so this output is not visible. It should really be 8192
    characters minus characters previously outputted for it to
    work always correctly. In this simple examples this is not
    required.
</p>

<?php

fprint("\r\n\r\n");
sleep(1);
fprint("Slept for 1 second<br>");
sleep(2);
fprint("Slept for 2 seconds<br>");
sleep(3);
fprint("Slept for 3 seconds<br>");
fprint("Done.")

?>