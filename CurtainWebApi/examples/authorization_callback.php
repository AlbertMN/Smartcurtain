<?php
require_once __DIR__."/../includes/requirements.php";

if (isset($_GET["state"]) && isset($_GET["code"])) {
    switch ($_GET["state"]) {
        case "OK":
            $return = file_get_contents("http://gardin.albe.pw/api/token.php?client_id=1&client_secret=test&code=$_GET[code]&grant_type=authorization_code");
            echo $return;
            break;
        case "error":
            echo "An error occurred on acceptance of authorization; ".$_GET["message"];
            break;
    }
}