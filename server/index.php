<?php
require_once "config.php";

session_start();

$showdata = (isset($_GET['show']) && (!empty($_GET['show']))) ? $_GET['show'] : "All";
$action = (isset($_POST['action']) && (!empty($_POST['action']))) ? $_POST['action'] : "";

@mysql_pconnect($db_host, $db_user, $db_pswd) or exit("Could not connect to database");
@mysql_select_db($db_name) or exit("Database unavailable");

$timezone = "Asia/Irkutsk";
date_default_timezone_set($timezone);

if ($action == "activate") {
	foreach ($_POST['check_list'] as $checkbox) { 
		$db_table = "licenses";
		$query = "SELECT * FROM ".$db_table." WHERE `uid`='".$checkbox."';";
		$query_result = @mysql_query($query);
		$row = mysql_fetch_assoc($query_result);
		if (!isset($row['cid']) || empty($row['cid'])) continue;
		if (!isset($row['uid']) || empty($row['uid'])) continue;
		$cid = $row['cid'];
		$uid = $row['uid'];

		$db_table = "purchases";
		$query = "SELECT * FROM ".$db_table." WHERE `cid`='".$cid."';";
		$query_result = @mysql_query($query);
		$row = mysql_fetch_assoc($query_result);
		$enccert = make_cert($uid, $row);

		$db_table = "licenses";
		$query = "UPDATE ".$db_table." SET `cert`='".$enccert."' WHERE `uid`='".$uid."';";
		$query_result = @mysql_query ($query);
	}
}

if ($action == "add") {
	$db_table = "purchases";
	$query = "INSERT INTO ".$db_table." 
		(`name`,`org`,`org_unit`,`country`,`state`,`email`,`phone`,`reg_time`,`exp_time`,`comment`,`num`)
		VALUES (
			'".$_POST['name']."',
			'".$_POST['org']."',
			'".$_POST['org_unit']."',
			'".$_POST['country']."',
			'".$_POST['state']."',
			'".$_POST['email']."',
			'".$_POST['phone']."',
			'".strtotime($_POST['reg_time'])."',
			'".strtotime($_POST['exp_time'])."',
			'".$_POST['comment']."',
			'".$_POST['num']."'
		);";

	@mysql_query ($query);
	$cid = mysql_insert_id();
	$db_table = "licenses";
	for ($i=0;$i<$_POST['num'];$i++) {
		$query = "INSERT INTO ".$db_table." (`serial`,`cid`)
			VALUES  (
				'".get_serial($_POST['name'].$_POST['org'].$_POST['org_unit'].$_POST['country'].$_POST['state'].$_POST['email'])."',
				'".$cid."'
			);";
		$query_result = @mysql_query($query);
	}
	header('Location: index.php?show=Transactions');
}

if ($action == "delete") {
	$db_table = "purchases";
	foreach($_POST['check_list'] as $checkbox) {
		$query="DELETE FROM ".$db_table." WHERE `cid`='".$checkbox."';";
		$query_result = @mysql_query ($query);
		$db_table = "licenses";
		$query="DELETE FROM ".$db_table." WHERE `cid`='".$checkbox."';";
		$query_result = @mysql_query ($query);
	}
}

if ($action == "reset") {
	$db_table = "licenses";
	if (!empty($_POST['check_list'])) {
		foreach($_POST['check_list'] as $checkbox) {
			$query = "UPDATE ".$db_table." SET `uid`='',`ip`='',`last_time`='',`cert`= NULL WHERE `uid`='".$checkbox."';";
			$query_result = @mysql_query ($query);
		}
	}
}

include("templates/page_header.php");

switch ($showdata) {
case 'Active':
	include("templates/licenses.php");
	break;
case 'NonActive':
	include("templates/licenses.php");
	break;
case 'All':
	include("templates/licenses.php");
	break;
case 'Transactions':
	include("templates/transactions.php");
	break;
case 'Editor':
	include("templates/editor.php");
	break;
}

include("templates/page_footer.php");

function get_serial($salt) {
	$h = 0;

	for ($i = 0; $i < strlen($salt); $i++) {
		$h += (ord($salt[$i]) << (($i * 5) % 24));
	}

	$serial = sprintf("%04x-%04x-%04x-%04x-%04x",
	$h >> 16, $h & 0xffff,
	mt_rand(0, 0x0fff) | 0x1000,    // four most significant bits holds version number 1
	mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0xffff));

	return $serial;
}

function strtohex($x) {
	$s = '';
	foreach (str_split($x) as $c) $s .= sprintf("%02X", ord($c));
	return ($s);
}

function make_cert($uid, $info) {
	$dn = array();

	if (isset($info['country']) && !empty($info['country'])) $dn['countryName'] = $info['country'];
	if (isset($info['state']) && !empty($info['state'])) $dn['stateOrProvinceName'] = $info['state'];
	if (isset($info['country']) && !empty($info['country'])) $dn['localityName'] = $info['country'];
	if (isset($info['org']) && !empty($info['org'])) $dn['organizationName'] = $info['org'];
	if (isset($info['org_unit']) && !empty($info['org_unit'])) $dn['organizationalUnitName'] = $info['org_unit'];
	if (isset($info['name']) && !empty($info['name'])) $dn['commonName'] = $info['name'];
	if (isset($info['email']) && !empty($info['email'])) $dn['emailAddress'] = $info['email'];

	$configargs = array('private_key_bits' => 1024);
	$keys = openssl_pkey_new($configargs);
	openssl_pkey_export($keys, $privKey);
	$pubKey = openssl_pkey_get_details($keys);
	$pubKey = $pubKey['key'];
	$csr = openssl_csr_new($dn, $privkey);
	$cacert = file_get_contents('/etc/ssl/rootCA.crt');
	$cakey = file_get_contents('/etc/ssl/rootCA.key');
	$sscert = openssl_csr_sign($csr, $cacert, $cakey, 365);
	openssl_x509_export($sscert, $certout);
	openssl_csr_export($csr, $csrout);
	openssl_x509_export($sscert, $certout);

	return openssl_encrypt($certout, "AES-256-CBC", strtohex($uid), 0, $uid);
}
?>
