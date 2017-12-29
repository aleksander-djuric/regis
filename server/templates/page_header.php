<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
	<meta charset="utf-8">
	<title>License Tracker</title>
	<link rel="stylesheet" type="text/css" href="style/admin.css">
</head>
<body>
<form>
	<div id="search" align="right">
		<input type="text" class="searchTerm" placeholder="Поиск" onchange="document.getElementById('search')"/>
	</div>

	<div id="navigation">
	<?php switch ($showdata) {
	case 'Active':
		echo "<button class='button' name='action' value='reset' formmethod='post' type='submit'>Деактивировать</button>";
		break;
	case 'NonActive':
		echo "<button class='button' name='action' value='activate' formmethod='post' type='submit'>Активировать</button>";
		break;
	case 'All':
		echo "<button class='button' name='action' value='activate' formmethod='post' type='submit'>Активировать</button>";
		echo "<button class='button' name='action' value='reset' formmethod='post' type='submit'>Деактивировать</button>";
		break;
	case 'Transactions':
		echo "<button class='button' name='show' value='Editor' formmethod='get' type='submit'>Добавить</button>";
		echo "<button class='button' name='action' value='delete' formmethod='post' type='submit'>Удалить</button>";
		break;
	} ?>
	</div>

	<div id="container">

	<div id="menu">
	<a href="#" class="menuitem">Лицензии</a>
	<div class="submenu">
		<a href="/index.php?show=All">Все лицензии</a><br />
		<a href="/index.php?show=Active">Активные</a></br>
		<a href="/index.php?show=NonActive">Не активные</a>
	</div>
	<a href="/index.php?show=Transactions">Транзакции</a>
	</div>

	<div id="content">
