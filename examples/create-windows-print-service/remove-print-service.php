<?php

/**
Remove Windows Print Service (On Shutdown Application)

Requirements:

- Install nssm http://nssm.cc/download and store into root directory
- Create log file to store printer status and activities in www directory

**/

// nssm.exe in root directory
$nssm = dirname(__DIR__) . DIRECTORY_SEPARATOR . 'nssm.exe';
// php file that provice print service will regitstered to windows as service using nssm.exe
$logfile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'printer.log';
$timestamps = date('Y-m-d H:i:s');

// Execute nssm.exe command to stop php file in windows service
exec($nssm . ' stop PrinterServerServiceRunner');
// Execute nssm.exe command to remove php windows service
exec($nssm . ' remove PrinterServerServiceRunner confirm');

// Put log into that file when service is successfully install and running on machine
file_put_contents($logfile, "[$timestamps][INFO] New session" . PHP_EOL);
