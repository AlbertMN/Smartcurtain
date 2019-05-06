<?php
require_once __DIR__."/../includes/requirements.php";

$status = false;
$message = "";
$code = null;

if (isset($_GET["client_id"]) && isset($_GET["response_type"]) && isset($_GET["scope"]) && isset($_GET["redirect_url"])) {
    $db->where("id", $_GET["client_id"]);
    $client = $db->getOne("clients");

    if (!is_null($client)) {
        $redirect_url = $_GET["redirect_url"];

        if (filter_var($redirect_url, FILTER_VALIDATE_URL)) {
            $response_type = $_GET["response_type"];

            if ($response_type == "code") {
                $scopes = explode(",", $_GET["scope"]);
                $scope_ids = "";

                foreach ($scopes as $scope) {
                    $db->where("name", $scope);
                    $db_scope = $db->getOne("scopes");
                    if (!is_null($db_scope))
                        $scope_ids .= "$db_scope[id],";
                }
                if (!empty($scope_ids)) {
                    $scope_ids = rtrim($scope_ids,",");

                    if ($_SESSION["logged_in"]) {
                        $_SESSION["authorization_data"] = array(
                            "code" => uniqid(),
                            "scope_ids" => $scope_ids,
                            "created" => $datetime,
                            "expiry" => date("Y-m-d H:i:s", strtotime("+1 year")),
                            "client_id" => $client["id"],
                            "user_id" => $_SESSION["user_id"]
                        );

                        $_SESSION["redirect_url"] = $redirect_url;
                    } else {
                        $_SESSION["authorization_data"] = null;
                        $_GET["redirect_url"] = null;
                    }

                    $status = true;
                } else {
                    $message = "No existing scopes by given names exist";
                }
            } else {
                //Possibly more to come
                $message = "Invalid response type";
            }
        } else {
            $message = "invalid redirect URL";
        }
    } else {
        $message = "Client with given ID does not exist";
    }
} else {
    $message = "Required fields not set";
}

/*echo json_encode(array(
    "status" => $status,
    "message" => $message,
    "code" => $code
));*/
?>

<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Authorize curtain access</title>

    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
</head>
<body>

<div class="container" style="margin-top:40px;">
    <div class="row justify-content-md-center">
        <div class="col-md-6">
            <div class="card">
                <div class="card-body">
                    <?php
                    if ($status) {
                        if ($_SESSION["logged_in"]) {
                            echo "<h5 class=\"text-center card-title\">Grant '$client[application_name]' access</h5>
                            <p class=\"text-center card-text\">Hello $user[name], the application is requesting the following permission(s);</p>
                            
                            <hr>
                            <ul>";

                            foreach ($scopes as $scope) {
                                $db->where("name", $scope);
                                $db_scope = $db->getOne("scopes");
                                if (!is_null($db_scope)) {
                                    echo "<li><b>".ucfirst($db_scope["name"]).":</b> $db_scope[description]</li>";
                                }
                            }
                            
                            echo "</ul>
                            
                            <p class='text-center'><a href='http://gardin.albe.pw/logout.php?redirect_to=api/".(urlencode(basename($_SERVER['REQUEST_URI'])))."'>Log out</a></p>
                            <a href=\"accept_authorization.php\" class=\"btn btn-primary btn-block\">Accept and grant access</a>";
                        } else {
                            echo "<h5 class=\"text-center card-title\">Grant '$client[application_name]' access</h5>
                            <p class=\"text-center card-text\">After logging in, you will be presented with the permissions the application is requesting.
                            You can choose whether to approve or deny the request.</p>
                            
                            <hr>
                            
                            <form action='http://gardin.albe.pw/login.php?redirect_to=api/".(urlencode(basename($_SERVER['REQUEST_URI'])))."' method='post'>
                                <div class='form-group'>
                                    <label><b>Email</b></label>
                                    <input class='form-control' name='email' type='email' placeholder='Your smart curtain account email'>
                                </div>
                                <div class='form-group'>
                                    <label><b>Password</b></label>
                                    <input class='form-control' name='password' type='password' placeholder='Your password'>
                                </div>
                                
                                <button type='submit' class=\"btn btn-primary btn-block\">Login</button>
                            </form>";
                        }
                    } else {
                        echo "<h5 class=\"text-center card-title\">Error</h5><p class=\"text-center card-text\">$message</p>";
                    }
                    ?>
                </div>
            </div>
        </div>
    </div>
</div>

</body>
</html>
