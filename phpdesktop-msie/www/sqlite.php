<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>SQLite</h1>

<?php

error_reporting(-1);
include "./_pdo.php";
$db_file = "./sqlite-file.db";
PDO_Connect("sqlite:$db_file");
echo("PDO_Connect(): success!<br> $db_file file created");

?>