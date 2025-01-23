<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>
<br><br>

<p>
    For SSL to work you have to provide a Certificate Authority file and you should keep that file up to date.
    For example you can download the Mozilla CA certificate store in PEM fornat from here:
    <a href="https://curl.se/docs/caextract.html">https://curl.se/docs/caextract.html</a>
</p>
<p>
    Then you have to set it in "php.ini" file using "openssl.cafile" option.
    If using curl extension also set the "curl.cainfo" option.
</p>

<h3>Test 1</h3>

<?php

$html = file_get_contents("https://curl.se/docs/caextract.html");
if ($html) {
    echo "<b>OK</b><br>";
    echo "Downloaded web page via HTTPS link<br>";
    echo "Page size: <code>" . strlen($html) . "</code> bytes";
} else {
    echo "<b>FAILED</b> to download web page via HTTPS link";
}

?>

<h3>Test 2</h3>

<?php

$hostname = "news-web.php.net";
$ssloptions = array(
    "capture_peer_cert" => true, 
    "capture_peer_cert_chain" => true, 
    "allow_self_signed"=> false, 
    "CN_match" => $hostname,
    "verify_peer" => true,
    "SNI_enabled" => true,
    "SNI_server_name" => $hostname,
);
 
$ctx = stream_context_create( array("ssl" => $ssloptions) );
$result = stream_socket_client("ssl://$hostname:443", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
$cont = stream_context_get_params($result);
$x509 = $cont["options"]["ssl"]["peer_certificate"];
$certparsed = openssl_x509_parse($x509);

foreach($cont["options"]["ssl"]["peer_certificate_chain"] as $chaincert)
{
    $chainparsed = openssl_x509_parse($chaincert);
    $chain_public_key = openssl_get_publickey($chaincert);
    $r = openssl_x509_verify($x509, $chain_public_key);   
    if ($r==1)
    {
        echo "<b>OK</b><br>";
        echo "<code>" . $certparsed['subject']['CN'] . "</code>";
        echo " was digitally signed by ";
        echo "<code>" . $chainparsed['subject']['CN'] . "</code>";
    }
}

?>