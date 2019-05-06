<?php
require_once __DIR__."/includes/requirements.php";

$status = false;
$message = "";

if (!$_SESSION["logged_in"]) {
    if (isset($_POST["email"]) && isset($_POST["password"])) {
        $db->where("email", $_POST["email"]);
        $the_user = $db->getOne("users");

        if (!is_null($the_user)) {
            if (password_verify($_POST["password"], $the_user["password"])) {
                $_SESSION["logged_in"] = true;
                $_SESSION["user_id"] = $the_user["id"];

                $status = true;
            }
        } else {
            $message = "User with given email does not exist";
        }
    } else {
        $message = "Email and or password not set";
        var_dump($_POST);
    }
} else {
    $message = "Already logged in";
}

if (isset($_GET["redirect_to"])) {
    header("Location: $_GET[redirect_to]");
}

echo json_encode(array(
    "status" => $status,
    "message" => $message
));