<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Popup</title>
<h1>Popup</h1>

<p>
  Window size:
  <script>document.write(window.innerWidth + " x " + window.innerHeight);</script>
</p>

<p>
  <a href="javascript:window.open('popup.php', '', 'width=600,height=400')">
    window.open('popup.php', '', 'width=600,height=400')
  </a>
</p>

<p>
  <a href="index.php" target="_blank">target="_blank"</a>
</p>
