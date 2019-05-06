<?php
header("Access-Control-Allow-Origin: *");
require_once __DIR__."/../../includes/requirements.php";

$status = false;
$message = "";

$percentage = null;
$speed = null;
$encoder_pos = -999;
$max_pos = -999;

if (isset($_GET["curtain_id"]) && isset($_GET["api_key"])) {
    $db->where("id", $_GET["curtain_id"]);
    $db->where("api_key", $_GET["api_key"]);
    $curtain = $db->getOne("curtain");

    if (!is_null($curtain)) {
        $status = true;
        $percentage = $curtain["current_percentage"];
        $speed = $curtain["speed"];
        $encoder_pos = $curtain["encoder_pos"];
        $max_pos = $curtain["max_pos"];

        $db->where("id", $_GET["curtain_id"]);
        $db->where("api_key", $_GET["api_key"]);
        $db->update("curtain", array("encoder_pos" => -999), 1);
    } else {
        $message = "Invalid curtain ID or API key";
    }
} else {
    $message = "Required data not set";
}

echo json_encode(array(
    "status" => $status,
    "message" => $message,
    "percentage" => $percentage,
    "speed" => $speed,
    "encoder_pos" => $encoder_pos,
    "max_pos" => $max_pos
));