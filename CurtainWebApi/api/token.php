<?php
require_once __DIR__."/../includes/requirements.php";

$status = false;
$message = "";

$access_token = null;
$refresh_token = null;
$expires = null;

//Credit to Scott; https://stackoverflow.com/a/13733588/4880538
function getToken($length){
    $token = "";
    $codeAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    $codeAlphabet.= "abcdefghijklmnopqrstuvwxyz";
    $codeAlphabet.= "0123456789";
    $max = strlen($codeAlphabet); // edited

    for ($i=0; $i < $length; $i++) {
        $token .= $codeAlphabet[random_int(0, $max-1)];
    }

    return $token;
}

if (isset($_GET["client_id"]) && isset($_GET["client_secret"])) {
    $db->where("id", $_GET["client_id"]);
    $db->where("client_secret", $_GET["client_secret"]);
    $client = $db->getOne("clients");

    if (!is_null($client)) {
        if (isset($_GET["grant_type"])) {
            $grant_type = $_GET["grant_type"];

            if ($grant_type == "authorization_code") {
                //Get authentication code from scratch
                if (isset($_GET["code"])) {
                    $code = $_GET["code"];

                    $db->where("code", $code);
                    $db->where("client_id", $client["id"]);
                    $code_row = $db->getOne("active_codes");

                    if (!is_null($code_row)) {
                        if (empty($code_row["access_token"]) && empty($code_row["refresh_token"])) {
                            $access_token = getToken(32);
                            $refresh_token = getToken(32);
                            $expires = 360;

                            $db->where("id", $code_row["id"]);
                            if ($db->update("active_codes", array("access_token" => $access_token, "refresh_token" => $refresh_token, "access_expiry" => date("Y-m-d H:i:s", strtotime("+$expires minutes"))), 1)) {
                                $status = true;
                            } else {
                                $message = "Failed to insert access and refresh token to code row; ".$db->getLastError();
                            }
                        } else {
                            $message = "This code already has tokens. Use the access token to make requests, or use the refresh token to get a new access token";
                        }
                    } else {
                        $message = "Invalid code";
                    }
                } else {
                    $message = "Code not set";
                }
            } else if ($grant_type == "refresh_token") {
                if (isset($_GET["refresh_token"])) {
                    $db->where("refresh_token", $_GET["refresh_token"]);
                    $db->where("client_id", $client["id"]);
                    $code_row = $db->getOne("active_codes");

                    if (!is_null($code_row)) {
                        $access_token = getToken(32);
                        $refresh_token = getToken(32);
                        $expires = 360;

                        $db->where("id", $code_row["id"]);
                        if ($db->update("active_codes", array("access_token" => $access_token, "refresh_token" => $refresh_token, "access_expiry" => date("Y-m-d H:i:s", strtotime("+$expires minutes"))), 1)) {
                            $status = true;
                        } else {
                            $message = "Failed to update access and refresh token to code row; ".$db->getLastError();
                        }
                    } else {
                        $message = "Invalid refresh token";
                    }
                } else {
                    $message = "Required credentials not set";
                }
            } else {
                $message = "Invalid grant type";
            }
        } else {
            $message = "'grant_type' not set";
        }
    } else {
        $message = "Client ID and secret combination doesn't exist";
    }

} else {
    $message = "Required fields not set";
}

echo json_encode(array(
    "status" => $status,
    "message" => $message,
    "access_token" => $access_token,
    "refresh_token" => $refresh_token,
    "expires" => $expires
));