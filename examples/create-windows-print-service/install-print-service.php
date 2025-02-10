<?php

/**

Create Windows Print Service (On Startup Application)

Requirements:
- Install nssm http://nssm.cc/download and store into root directory
- Create ESCPOS PHP (In my case, I use https://github.com/mike42/escpos-php) to communicate with ESCPOS Printer Devices in www directory
- Install Hoa Project / Workerman to create websocket server to communicate with browser
  - Hoa Project <https://github.com/hoaproject/Websocket> (Read Only Repository)
  - Workerman <https://github.com/walkor/workerman>
- Create log file to store printer status and activities in www directory

**/

// nssm.exe in root directory
$nssm = dirname(__DIR__) . DIRECTORY_SEPARATOR . 'nssm.exe';
// php file that provice print service will regitstered to windows as service using nssm.exe
$service = dirname(__DIR__) . DIRECTORY_SEPARATOR . 'www' . DIRECTORY_SEPARATOR . 'printer-server.php';
// php.exe for running php file inside service
$php = dirname(__DIR__) . DIRECTORY_SEPARATOR . 'php' . DIRECTORY_SEPARATOR . 'php.exe';

// Execute nssm.exe command to register php file to windows service
exec($nssm . ' install PrinterServerServiceRunner ' . $php . ' ' . $service);
// Execute nssm.exe command to start windows service has been registered
exec($nssm . ' start PrinterServerServiceRunner');

// Create printer log file
$logfile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'printer.log';
$timestamps = date('Y-m-d H:i:s');
// Put log into that file when service is successfully install and running on machine
file_put_contents($logfile, "[$timestamps][INFO] Service has been install and registered on your machine" . PHP_EOL);
