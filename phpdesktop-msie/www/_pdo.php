<?php
// PDO helper functions.
// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

function PDO_Connect($dsn, $user="", $password="")
{
    global $PDO;
    $PDO = new PDO($dsn, $user, $password);
    $PDO->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
}
function PDO_FetchOne($query, $params=null)
{
    global $PDO;
    if (isset($params)) {
        $stmt = $PDO->prepare($query);
        $stmt->execute($params);
    } else {
        $stmt = $PDO->query($query);
    }
    $row = $stmt->fetch(PDO::FETCH_NUM);
    if ($row) {
        return $row[0];
    } else {
        return false;
    }
}
function PDO_FetchRow($query, $params=null)
{
    global $PDO;
    if (isset($params)) {
        $stmt = $PDO->prepare($query);
        $stmt->execute($params);
    } else {
        $stmt = $PDO->query($query);
    }
    return $stmt->fetch(PDO::FETCH_ASSOC);
}
function PDO_FetchAll($query, $params=null)
{
    global $PDO;
    if (isset($params)) {
        $stmt = $PDO->prepare($query);
        $stmt->execute($params);
    } else {
        $stmt = $PDO->query($query);
    }
    return $stmt->fetchAll(PDO::FETCH_ASSOC);
}
function PDO_FetchAssoc($query, $params=null)
{
    global $PDO;
    if (isset($params)) {
        $stmt = $PDO->prepare($query);
        $stmt->execute($params);
    } else {
        $stmt = $PDO->query($query);
    }
    $rows = $stmt->fetchAll(PDO::FETCH_NUM);
    $assoc = array();
    foreach ($rows as $row) {
        $assoc[$row[0]] = $row[1];
    }
    return $assoc;
}
function PDO_Execute($query, $params=null)
{
    global $PDO;
    if (isset($params)) {
        $stmt = $PDO->prepare($query);
        $stmt->execute($params);
        return $stmt;
    } else {
        return $PDO->query($query);
    }
}
function PDO_LastInsertId()
{
    global $PDO;
    return $PDO->lastInsertId();
}
function PDO_ErrorInfo()
{
    global $PDO;
    return $PDO->errorInfo();
}

?>
