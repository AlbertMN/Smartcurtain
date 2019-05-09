<?php
require_once __DIR__."/../../includes/requirements.php";

$status = false;
$message = "";
$reset_to_ap = false;

if (isset($_GET["curtain_id"]) && isset($_GET["api_key"])) {
    $db->where("id", $_GET["curtain_id"]);
    $db->where("api_key", $_GET["api_key"]);
    $curtain = $db->getOne("curtain");

    if (!is_null($curtain)) {
        $status = true;

        $reset_to_ap = $curtain["reset_to_ap"];
        if ($reset_to_ap) {
            $db->where("id", $_GET["curtain_id"]);
            $db->where("api_key", $_GET["api_key"]);
            $db->update("curtain", array("reset_to_ap" => false), 1);
        }
    } else {
        $message = "Invalid curtain ID or API key";
    }
} else {
    $message = "Curtain ID or API key not set";
}

echo json_encode(array("status" => $status, "message" => $message, "reset_to_ap" => $reset_to_ap));