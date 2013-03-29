<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>

<title>Test context menu</title>
<h1>Test context menu</h1>

Click right mouse button to see a custom context menu.

<div id=contextMenu style="display: none; background: #eee;
        border=#999 1px solid; width: 150px; padding: 0.5em 1em;
        line-height: 1.5em; font-family: Verdana; font-size: 12px;
        position: absolute; z-index: 100000;">
    <div>Copy</div>
    <div>Cut</div>
    <div>Paste</div>
    <hr>
    <div>Select all</div>
</div>

<script>
var contextMenuEnabled = false;
function IsParentOf(node, id) {
    do {
        if (node.id == id) {
            return true;
        }
        node = node.parentNode;
    } while (node);
    return false;
}
document.body.onclick = function(){
    if (IsParentOf(window.event.srcElement, "contextMenu")) {
        return true;
    }
    document.getElementById("contextMenu").style.display = "none";
}
document.body.oncontextmenu = function(){
    if (IsParentOf(window.event.srcElement, "contextMenu")) {
        return false;
    }
    var mouseX = event.clientX + (document.documentElement.scrollLeft
        || document.body.scrollLeft);
    var mouseY = event.clientY + (document.documentElement.scrollTop
        || document.body.scrollTop);
    var contextMenu = document.getElementById("contextMenu");
    contextMenu.style.top = mouseY;
    contextMenu.style.left = mouseX;
    contextMenu.style.display = "block";
    return false;
}
</script>
