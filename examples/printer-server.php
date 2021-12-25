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

date_default_timezone_set("Asia/Jakarta");

use Mike42\Escpos\Printer
use Mike42\Escpos\PrintConnectors\WindowsPrintConnector;

require_once dirname(__FILE__) . '/vendor/autoload.php';

$websocket = new Hoa\Websocket\Server(
    new Hoa\Socket\Server('ws://127.0.0.1:8889')
);

$websocket->on('open', function (Hoa\Event\Bucket $bucket) {
    $message = createMessage('info', 'New connection open! ');
    sendToAll($bucket, $message);
    logger( $message );
    return;
});

$websocket->on('message', function (Hoa\Event\Bucket $bucket) {
    $bucketData = $bucket->getData();
    $message = $bucketData['message'];

    $data = json_decode($message, true);
  
    sendToAll( $bucket, createMessage('info', 'metadata: ' . $data['from']) );
    logger( createMessage('info', 'metadata: ' . $data['from']) );
    sendToAll( $bucket, createMessage('info', 'active_printer: ' . $data['printer_name']) );
    logger( createMessage('info', 'active_printer: ' . $data['printer_name']) );
  
    try {
        
        // write your own php ESCPOS scripts
        // .....
      
        sendToAll( $bucket, createMessage('info', "notifiy: Receipt has been printered!" );
        logger( createMessage('info', "notifiy: Receipt has been printered!" );
      
    } catch (Exception $e) {
        sendToAll( $bucket, createMessage('error', "Couldn't print to this printer: " . $e->getMessage()) );
        logger( createMessage('error', "Couldn't print to this printer: " . $e->getMessage()) );
    }

    return;
});

$websocket->on('close', function (Hoa\Event\Bucket $bucket) {
    echo 'Connection closed', "\n";
    sendToAll($bucket, createMessage('info', 'connection closed!'));
    logger( createMessage('info', 'connection closed!') );
    return;
});

$websocket->run();

function sendToAll( $bucket, $message )
{
    $bucket->getSource()->broadcast($message);
}

function logger( $message )
{
    $logfile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'printer.log';
    if ( !file_exists($logfile) ) {
        touch($logfile);
    }

    $longline = count(file($logfile));
    if ( $longline >= 1500 ) {
        file_put_contents($logfile, createMessage('info', 'New connection open!'));
    }

    file_put_contents($logfile, $message, FILE_APPEND);
}

function createMessage( $type = 'info', $message )
{
    $timestamps = date('Y/m/d H:i:s');
    $log = strtoupper($type);
    return "[$timestamps][$log] $message" . PHP_EOL;
}
