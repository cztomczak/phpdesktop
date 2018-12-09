<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<h1>SQLite</h1>

<?php

error_reporting(-1);
include "./_pdo.php";
$db_file = "./sqlite-database.db";
PDO_Connect("sqlite:$db_file");
print("PDO_Connect(): successsfully connected<br>");
print("The database file: <b>$db_file</b><br>");

$queries = <<< HTML
CREATE TABLE test (id int PRIMARY KEY, name varchar(50));
INSERT INTO test VALUES (1, "Amelia");
INSERT INTO test VALUES (2, "Beatrice");
HTML;

print("<h2>Create table and insert exampe data</h2>");
print("<pre>");
$queries = explode(";", $queries);
foreach ($queries as $query) {
    $query = trim($query);
    if (!$query) continue;
    $stmt = @PDO_Execute($query);
    if (!$stmt || ($stmt && $stmt->errorCode() != 0)) {
        $error = PDO_ErrorInfo();
        print_r($error[2]);
        break;
    }
    print($query."\n");
}
print("</pre>");

print("<h2>Fetch data</h2>");
print("PDO_FetchAll('SELECT * FROM test')");
print("<pre>");
$data = PDO_FetchAll("SELECT * FROM test");
print_r($data);
print("</pre>");

?>