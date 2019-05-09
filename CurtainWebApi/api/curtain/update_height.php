<?php
header("Access-Control-Allow-Origin: *");
require_once __DIR__."/../../includes/requirements.php";

$status = false;
$message = "";

if (isset($_GET["curtain_id"]) && isset($_GET["api_key"])) {
    $db->where("id", $_GET["curtain_id"]);
    $db->where("api_key", $_GET["api_key"]);
    $curtain = $db->getOne("curtain");

    if (!is_null($curtain)) {
        if (isset($_GET["height"])) {
            if (is_numeric($_GET["height"])) {
                $db->where("id", $curtain["id"]);
                if ($db->update("curtain", array("max_pos" => (int)$_GET["height"]))) {
                    $status = true;
                    $message = "Curtain max pos updated to $_GET[height]%";
                } else {
                    $message = "Failed to update max pos; ".$db->getLastError();
                }
            } else {
                $message = "Height not numeric";
            }
        } else {
            $message = "Height not set";
        }
    } else {
        $message = "Invalid curtain ID or API key";
    }
} else {
    $message = "Required data not set";
}

echo json_encode(array(
    "status" => $status,
    "message" => $message
));