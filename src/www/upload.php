<?php
error_reporting(-1);
?>

<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Upload</title>
<h1>Upload</h1>

<p>Temp directory: <?php echo sys_get_temp_dir(); ?></p>

<form enctype="multipart/form-data" action="upload.php" method="POST">
    Max file size is set in php.ini<br>
    Send this file: <input name="myfile" type="file" />
    <input type="submit" value="Send File" />
</form>

<h2>$_FILES</h2>
<pre>
<?php print_r($_FILES); ?>
</pre>

<?php if (count($_FILES)): ?>

<?php $myfile = $_FILES["myfile"]["tmp_name"]; ?>

<h2>Check the uploaded file</h2>
<pre>
is_file() = <?php echo is_file($myfile); ?>

is_readable() = <?php echo is_readable($myfile); ?>

is_writable() = <?php echo is_writable($myfile); ?>
</pre>

<h2>Move the uploaded file</h2>

<?php
$success = move_uploaded_file($myfile,
                              __DIR__."/".basename($_FILES["myfile"]["name"]));
?>

<?php if ($success): ?>
<p>
    The uploaded file (<?php echo $_FILES["myfile"]["name"]; ?>) was moved to
    the www/ directory using the <b>move_uploaded_file()</b> function.
    See the listing of files by going to <a href="index.php">index.php</a>
</p>
<?php else: ?>
<p style="color: red;">
    move_uploaded_file() failed.
</p>
<?php endif; // $success ?>

<?php endif; // count($_FILES) ?>

