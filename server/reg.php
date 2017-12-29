<?php
require_once 'config.php';

$query = $_SERVER['QUERY_STRING'];
$ip = $_SERVER['REMOTE_ADDR'];

$data = base64_decode($query, true);
$cakey = file_get_contents('/etc/ssl/rootCA.key');
if (!openssl_private_decrypt($data, $decrypted, $cakey)) {
	exit("BAD REQUEST");
}

parse_str($decrypted, $params);
$serial = $params['serial'];
$uid = $params['uid'];

@mysql_pconnect($db_host, $db_user, $db_pswd) or exit('DATABASE ERROR');
@mysql_select_db($db_name) or exit("DATABASE ERROR");

$db_table="licenses";
$query = "SELECT * FROM $db_table WHERE serial='$serial';";
if (!($result = mysql_query($query))) exit("DATABASE ERROR");
if (!($row = mysql_fetch_assoc($result))) {
	exit("NOT REGISTERED");
}

$last_time = time();
$query = "UPDATE $db_table SET uid='$uid', ip='$ip', last_time='$last_time' WHERE serial='$serial';";

if (!($result = mysql_query($query))) exit("DATABASE ERROR");

if (!empty($row['cert'])) {
	echo $row['cert'];
} else {
	echo "NOT ACTIVATED";
}
?>
