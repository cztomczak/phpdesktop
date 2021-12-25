# Create & Remove Windows Print Service
This example explain how to create and remove windows print service with nssm.exe and communicate with browser with websocket server.

## Requirements:
- Install nssm http://nssm.cc/download and store into root directory
- Create ESCPOS PHP (In my case, I use https://github.com/mike42/escpos-php) to communicate with ESCPOS Printer Devices in www directory
- Install Hoa Project / Workerman to create websocket server to communicate with browser
  - Hoa Project <https://github.com/hoaproject/Websocket> (Read Only Repository)
  - Workerman <https://github.com/walkor/workerman>
- Create log file to store printer status and activities in www directory
- set this value `"start_fullscreen": true` in `settings.json`

## Directery structure
```
php/
    - php.exe
www/
    vendor/
    composer.json
    composer.lock
    - index.php
    - install-print-service.php
    - remove-print-service.php
    - printer-server.php
    - printer.log
nssm.exe
```

## If this example may useful for you, please give me feedback
- metions @darkterminal
- or send me email to ddarkterminal@pm.me
