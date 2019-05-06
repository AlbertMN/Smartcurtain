<?php
require_once __DIR__."/../includes/requirements.php";

$status = false;
$message = "";

if ($_SESSION["logged_in"]) {
    if (isset($_SESSION["authorization_data"])) {
        if (!is_null($_SESSION["authorization_data"])) {
            if (isset($_SESSION["redirect_url"])) {
                if (!is_null($_SESSION["redirect_url"])) {
                    if ($db->insert("active_codes", $_SESSION["authorization_data"])) {
                        $status = true;

                        header("Location: $_SESSION[redirect_url]?code=".$_SESSION["authorization_data"]["code"]."&state=OK");
                        die();
                    } else {
                        $message = "Failed to insert code to database; ".$db->getLastError();
                    }
                } else {
                    $message = "Redirect URL is invalid";
                }
            } else {
                $message = "Redirect URL not set";
            }
        } else {
            $message = "Data is null";
        }
    } else {
        $message = "Data not set";
    }
} else {
    $message = "You're not logged in";
}

header("Location: $_SESSION[redirect_url]?code=null&state=error&message=$message");