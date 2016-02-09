<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<!-- If "fixed_title" option under "popup_window" section (settings.json)
     is empty, then title for the popup window will be set using meta title -->
<title>Popup</title>
<h1>Popup</h1>

<p>
  When creating a window, the ["application"]["dpi_aware"] setting is taken
  into account. If it's set to true, then application window will be resized
  automatically using the current OS DPI settings.
</p>

<p>
  <a href="javascript:window.open('index.php', '', 'width=800,height=600')">
    window.open('index.php', '', 'width=800,height=600')
  </a>
</p>

<p>
<a href="index.php" target="_blank">target="_blank"</a>
</p>
