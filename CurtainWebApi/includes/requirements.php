<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

session_start();

if (!isset($_SESSION["logged_in"]))
    $_SESSION["logged_in"] = false;

if (!isset($_SESSION["user_id"]))
    $_SESSION["user_id"] = 0;

date_default_timezone_set('Europe/Copenhagen');

require_once __DIR__."/libraries/MySQLi/MysqliDb.php";
$db = new MysqliDb ('localhost', 'gardin', 'UgTyBoeCSWkk59Mc', 'gardin');

$user = null;
if ($_SESSION["logged_in"]) {
    $db->where("id", $_SESSION["user_id"]);
    $user = $db->getOne("users");

    if (is_null($user)) {
        $_SESSION["logged_in"] = false;
        $_SESSION["user_id"] = 0;
    }
}

$datetime = date("Y-m-d H:i:s");