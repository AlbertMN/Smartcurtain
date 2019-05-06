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
        if (isset($_GET["percentage"])) {
            if (is_numeric($_GET["percentage"])) {
                $db->where("id", $curtain["id"]);
                if ($db->update("curtain", array("current_percentage" => (int)$_GET["percentage"]))) {
                    $status = true;
                    $message = "Curtain percentage updated to $_GET[percentage]%";
                } else {
                    $message = "Failed to update percentage; ".$db->getLastError();
                }
            } else {
                $message = "Percentage not numeric";
            }
        } else {
            $message = "Percentage not set";
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