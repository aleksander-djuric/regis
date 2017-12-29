<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
	<meta charset="utf-8">
	<title>RegiS Installation Script</title>
	<link rel="stylesheet" type="text/css" href="install.css">
</head>
<body>
<?php
$step = (isset($_GET['step'])) ? $_GET['step'] : '';
$check = (isset($_POST['check'])) ? $_POST['check'] : NULL;
$config_file = '../config.php';
$sql_file = 'install.sql';

switch($step) {
	case '1': show_license($check); break;
	case '2': system_checks($check); break;
	case '3': create_database($check); break;
	default: show_license($check);
}

function show_license($check) {
	if ($check == 'next') {
		header('Location: ?step=2');
	} else if ($check) {
		echo "<div class='error'>You must agree to the license</div><br />";
	} ?>
<div id="content">
<h2>License agreement</h2>
<p>Before installing an application, you’re required to agree and accept the following license.</p>
<?php include("license.txt"); ?>
<form action="?step=1" method="post">
	<input type="hidden" name="check" value="first" />
	<p>I agree to the license <input type="checkbox" name="check" value="next" /></p>
	<input class="button" type="submit" value="Continue" />
</form>
</div>
<?php 
}

function system_checks($check) {
	global $config_file;

	$check_php = (phpversion() > '5.0') ? 1 : 0;
	$check_mysql = extension_loaded('mysql') ? 1 : 0;
	$check_config = is_writable($config_file) ? 1 : 0;

	$error = "";
	if (!$check_php) $error .= "You need to use PHP5 or above<br / >";
	if (!$check_mysql) $error .= "MySQL extension needs to be loaded<br />";
	if (!$check_config) $error .= "config.php needs to be writable";

	if ($check == 'next' && empty($error)) {
		header('Location: ?step=3');
	} else if ($check) {
		echo "<div class='error'>$error</div><br />";
	} ?>
<div id="content">
<h2>Checking your system configuration</h2>
<div id="view">
<table class="install">
<tr><th>Feature</th><th>Status</th></tr>
<tr><td>Check PHP Version:</td><td><?php echo ($check_php) ? 'Success' : 'Fail'; ?></td></tr>
<tr><td>Check MySQL:</td><td><?php echo ($check_mysql) ? 'Success' : 'Fail'; ?></td></tr>
<tr><td>Check config.php file:</td><td><?php echo ($check_config) ? 'Writable' : 'Not writable'; ?></td></tr>
</table>
</div><br />
<form action="?step=2" method="post">
	<input type="hidden" name="check" value="next" />
	<input class="button" type="submit" name="continue" value="Continue" />
</form>
</div>
<?php
}

function create_database ($check) {
	global $sql_file;
	global $config_file;

	$db_host = isset($_POST['db_host']) ? $_POST['db_host'] : "";
	$db_name = isset($_POST['db_name']) ? $_POST['db_name'] : "";
	$db_user = isset($_POST['db_user']) ? $_POST['db_user'] : "";
	$db_pswd = isset($_POST['db_pswd']) ? $_POST['db_pswd'] : "";

	$error = "";
	if (!empty($db_host) && !empty($db_name) && !empty($db_user) && !empty($db_pswd)) {
		$conn = mysql_connect($db_host, $db_user, $db_pswd);
		if (!$conn) {
			$error = "Connection failed: " . mysql_error();
		} else {
			if (!mysql_select_db($db_name, $conn)) {
				$error = "Can\'t use $db_name : " . mysql_error();
			}
		}
  
		if (empty($error)) {
			$data = file($sql_file);

			$query = "";
			foreach($data as $line) {
				$line = trim($line);
				if (($line != "") && ($line[0] != '#')) {
					$query .= $line;
  
					if (preg_match('/;\s*$/', $line)) {
						if (!mysql_query($query, $conn)) {
							$error = "Invalid query: " . mysql_error();
							break;
						}
						$query = "";
					}
				}
			}
			mysql_close($conn);
		}
	} else  {
		$error = "All fields are required! Please re-enter";
	}

	if ($check == 'next' && empty($error)) {
		$cf = fopen($config_file, "w+");

$config = '<?php
$db_host = "' . $db_host . '";
$db_name = "' . $db_name . '";
$db_user = "' . $db_user . '";
$db_pswd = "' . $db_pswd . '";
?>';

		if (fwrite($cf, $config) > 0) {
			fclose($cf);
		}

		header("Location: ../index.php");
	} else if ($check) {
		echo "<div class='error'>$error</div><br />";
	} ?>
<div id="content">
<h2>Database configuration</h2>
<form method="post" action="?step=3">
<input type="hidden" name="check" value="next" />
<div id="view">
<table class="install">
<tr><td>Database Host</td><td><input type="text" name="db_host" value="<?php echo $db_host; ?>" size="30"></td></tr>
<tr><td>Database Name</td><td><input type="text" name="db_name" size="30" value="<?php echo $db_name; ?>"></td></tr>
<tr><td>Database Username</td><td><input type="text" name="db_user" size="30" value="<?php echo $db_user; ?>"></td></tr>
<tr><td>Database Password</td><td><input type="text" name="db_pswd" size="30" value="<?php echo $db_pswd; ?>"></td></tr>
</table>
</div><br />
<input class="button" type="submit" name="submit" value="Install">
</form>
</div>
<?php
} ?>
