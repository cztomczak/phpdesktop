<style type="text/css">@import url("style.css");</style>
<a href="/index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Pretty urls</title>
<h1>Pretty urls</h1>

<h2>$_SERVER</h2>

<?php
function print_url_variables()
{
    $url_vars = [];
    foreach ($_SERVER as $k => $v) {
        if (is_array($v)) {
            continue;
        }
        if (strpos($v, "pretty-urls.php") !== false
                || strpos($v, "company") !== false
                || strpos($v, "xyz") !== false
                || strpos($v, "foo") !== false
                || strpos($v, "bar") !== false
                || $k == "REQUEST_METHOD"
                || $k == "REQUEST_URI") {
            $url_vars[$k] = $v;
        }
    }
    print "<pre>";
    print_r($url_vars);
    print "</pre>";
}
print_url_variables();
?>

<p>
    Pretty urls can be handled by setting ["web_server"]["404_handler"]
    option in settings.json file. This is how it works:
    <ul>
    <li>Set 404_handler to "/pretty-urls.php"
    <li>Load url like "/company/5"
    <li>Mongoose web server does not find such file "/company/5",
        and instead makes a request to "/pretty-urls.php/company/5".
    <li>To know the pretty url that was accessed check
        the PATH_INFO or REQUEST_URI environment variables.
    </ul>
</p>
<p>
    Test pretty urls:
    <ul>
    <li><a href="/company/5">/company/5</a>
    <li><a href="/company/5?xyz=1">/company/5?xyz=1</a>
    <li><a href="/company-5.html">/company-5.html</a>
    <li><a href="/company_5.html">/company_5.html</a>
    </ul>
</p>

<p>
    Test POST request to
    <select onchange="document.getElementById('myform').action=this.value;">
        <option value="/company/5">/company/5</option>
        <option value="/company.html">/company.html</option>
        <option value="/pretty-urls.php/company/5">
            /pretty-urls.php/company/5
        </option>
        <option value="/pretty-urls.php/company/5.html">
            /pretty-urls.php/company/5.html
        </option>
    </select>
    <form action="/company/5" method=POST id="myform">
    <input type=text name=foo value="abc">
    <input type=submit>
    </form>
</p>

<?php if ($_SERVER["REQUEST_METHOD"] == "POST"): ?>
<h2>POST data</h2>
<pre>
<?php print_r($_POST); ?>
</pre>
<?php endif; ?>

<p>
    Other tests (these urls do not require 404_handler to be set):
    <ul>
    <li><a href="/pretty-urls.php/company/5">
        /pretty-urls.php/company/5</a>
    <li><a href="/pretty-urls.php/company/5?xyz=1">
        /pretty-urls.php/company/5?xyz=1</a>
    <li><a href="/pretty-urls.php/company\5">
        /pretty-urls.php/company\5</a>
    <li><a href="/pretty-urls.php?xyz=1">
        /pretty-urls.php?xyz=1</a>
    </ul>
</p>


