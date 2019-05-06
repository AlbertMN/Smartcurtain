<?php
require_once __DIR__."/includes/requirements.php";

$_SESSION["logged_in"] = false;
$_SESSION["user_id"] = 0;

if (isset($_GET["redirect_to"])) {
    header("Location: $_GET[redirect_to]");
}

echo json_encode(array(
    "status" => $status,
    "message" => $message
));