<?php
require_once __DIR__."/includes/requirements.php";

$data = array(
    "name" => "Albert Møller Nielsen",
    "email" => "albert@mollernielsen.dk",
    "password" => password_hash("testpass123", PASSWORD_BCRYPT),
    "joined" => $datetime,
);

$db->insert("users", $data);