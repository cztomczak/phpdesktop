<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>


<h1>Speech recognition</h1>

You need to set the "enable-speech-input" flag in settings.json
for the speech recognition to work:

<pre>
"command_line_switches": {
    "enable-speech-input": ""
},
</pre>

<p>After you've done that, restart application and visit this url:</p>

<a href="http://slides.html5rocks.com/#speech-input">
    http://slides.html5rocks.com/#speech-input</a>
