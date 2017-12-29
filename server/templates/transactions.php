<table class="content" border="0" cellpadding="0" cellspacing="0" width="100%">
<tr class="content_header">
	<td width="30" align="center"></td>
	<td align="center">name</td>
	<td align="center">org</td>
	<td align="center">org_unit</td>
	<td align="center">country</td>
	<td align="center">state</td>
	<td align="center">email</td>
	<td align="center">phone</td>
	<td align="center">reg_time</td>
	<td align="center">exp_time</td>
	<td align="center">comment</td>
	<td align="center">number</td>
</tr>

<?php
$db_table = "purchases";
$query = "SELECT * FROM ".$db_table;
$query_result = @mysql_query ($query);

if (isset($query_result) & !empty($query_result)) {
	while ($row = mysql_fetch_assoc($query_result)) { ?>
<tr>
	<td align="center"><input name="check_list[]" value="<?php echo $row['cid']; ?>" type="checkbox" value="checked"></td>
	<td align="center"><?php echo $row['name']; ?></td>
	<td align="center"><?php echo $row['org']; ?></td>
	<td align="center"><?php echo $row['org_unit']; ?></td>
	<td align="center"><?php echo $row['country']; ?></td>
	<td align="center"><?php echo $row['state']; ?></td>
	<td align="center"><?php echo $row['email']; ?></td>
	<td align="center"><?php echo $row['phone']; ?></td>
	<td align="center"><?php echo date('Y-m-d',$row['reg_time']); ?></td>
	<td align="center"><?php echo date('Y-m-d',$row['exp_time']); ?></td>
	<td align="center"><?php
		if (strlen($row['comment']) > 22) {
			echo substr($row['comment'],0,22)."...";
		} else { echo $row['comment']; } ?></td>
	<td><?php echo $row['num']; ?></td>
</tr>
<?php }} ?>
</table>
