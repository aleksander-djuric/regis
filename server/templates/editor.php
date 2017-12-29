<?php
$lang = "RU";
if (isset($_SESSION['lang']) && !empty($_SESSION['lang'])) $lang = strtoupper($_SESSION['lang']);
else if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) $lang = strtoupper(substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2));
?>

<table id="editor" width="280" align="center">
	<tr><td colspan="2" align="center"><h3>Добавить транзакцию</h3></td></tr>

	<tr><td>name:</td><td align="right"><input name="name" type="text"/></td></tr>
	<tr><td>org:</td><td align="right"><input name="org" type="text"/></td></tr>
	<tr><td>org_unit:</td><td align="right"><input name="org_unit" type='text'/></td></tr>
	<tr><td>country:</td><td align="right"><input name="country" type="text" value='<?php echo $lang; ?>'/></td></tr>
	<tr><td>state:</td><td align="right"><input name="state" type="text"/></td></tr>
	<tr><td>email:</td><td align="right"><input name="email" type="text"/></td></tr>
	<tr><td>phone:</td><td align="right"><input name="phone" type="text"/></td></tr>
	<tr><td>reg_time:</td><td align="right"><input name="reg_time" type="date"/></td></tr>
	<tr><td>exp_time:</td><td align="right"><input name="exp_time" type="date"/></td></tr>
	<tr><td>number:</td><td align="right"><input name="num" type="text"/></td></tr>
	<tr><td>comment:</td><td align="right"><textarea name="comment" rows="6" cols="22"></textarea></td></tr>

	<tr><td class="actions" colspan="2" align="center">
		<button class="button" formmethod="post" name="action" value="add" type="submit">Добавить</button>
		<button class="button" formmethod="get" name="show" value="Transactions">Назад</button>
	</td>
</table>
