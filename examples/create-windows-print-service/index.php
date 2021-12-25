<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <title>Printer Server</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inconsolata:wght@500&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="http://127.0.0.1:55072/css/bootstrap.min.css">
    <link rel="stylesheet" type="text/css" href="http://127.0.0.1:55072/css/style.css">
</head>
<body>
    <div class="fixed-top bg-white d-flex justify-content-between align-items-center">
        <strong class="font-weight-bold pl-3">Printer Server</strong>
        <button class="btn btn-sm btn-danger rounded-0 px-3" id="close-window"><strong>✖</strong></button>
    </div>
    <div id="logContainer" class="container-fluid bg-dark text-white py-5" style="white-space: pre-line;"></div>
    <div class="fixed-bottom bg-white p-2 text-center">
        <?php

        ob_start(); // Turn on output buffering
        system('ipconfig /all'); //Execute external program to display output
        $mycom=ob_get_contents(); // Capture the output into a variable
        ob_clean(); // Clean (erase) the output buffer

        $findme = "Physical";
        $pmac = strpos($mycom, $findme); // Find the position of Physical text
        $mac=substr($mycom,($pmac+36),17); // Get Physical Address

        ?>
        <span>MAC Address: <?= $mac ?></span> | <span>HOST: <?= gethostname() ?></span> | <span id="time">Waiting...</span>
    </div>
    <script src="http://127.0.0.1:55072/js/jquery.min.js"></script>
    <script src="http://127.0.0.1:55072/js/moment-with-locales.min.js"></script>
    <script>

        const logContainer = document.getElementById("logContainer")
        const closeWindowButton = document.getElementById('close-window')

        var poll = () => {
            fetch('http://127.0.0.1:55072/printer.log', {
                method: 'GET'
            })
            .then(res => res.text())
            .then((txt) => {
                logContainer.innerHTML = txt
            })
            .catch((err) => poll())
        }
        setInterval(poll, 2000)

        setInterval(function () {
            logContainer.scrollTop = logContainer.scrollHeight;
        }, 5000)

        moment.locale('id')
        setInterval(function () {
            document.getElementById('time').innerHTML = moment().format('Do MMMM YYYY, hh:mm:ss a')
        }, 1000)

        closeWindowButton.addEventListener('click', (e) => {
            e.preventDefault()
            if ( confirm('Apakah anda yakin?') ) {
                $.post( 'http://127.0.0.1:55072/remove-print-service.php', null, function(response) {
                    console.log('Printer service removed!')
                }, 'json' )
                window.close()
            }
        })

        window.addEventListener('DOMContentLoaded', () => {
            $.post( 'http://127.0.0.1:55072/install-print-service.php', null, function(response) {
                console.log('Printer service installed!')
            }, 'json' )
        })

    </script>
</body>
</html>
