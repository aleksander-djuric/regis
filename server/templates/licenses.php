<table class="content" border="0" cellpadding="0" cellspacing="0" width="100%">
	<tr class="content_header">
		<td align="center"></td>
		<td align="center">uid</td>
		<td align="center">ip</td>
		<td align="center">last_time</td>
		<td align="center">serial</td>
		<td align="center">cert</td>
	</tr>
<?php

print_table($showdata);

function print_table($showdata) {
	$db_table="licenses";
	$query = "SELECT * FROM ".$db_table;
	if ($showdata == 'Active') $query .= " WHERE (cert != '')";
	else if ($showdata == 'NonActive') $query .= " WHERE (cert = '')";

	$result = @mysql_query ($query);
	if (!$result) return;

	while ($row = mysql_fetch_assoc($result)) { ?>
	<tr>
		<td align="center"><input name="check_list[]" value="<?php echo $row['uid'];
		?>" type="checkbox"<?php if (empty($row['uid'])) echo ' disabled="disabled"'; ?> value="checked"></td>
		<td align="center"><?php echo $row['uid']; ?></td>
		<td align="center"><?php echo $row['ip']; ?></td>
		<td align="center"><?php echo date('Y-m-d', $row["last_time"]); ?></td>
		<td align="center"><?php echo $row["serial"]; ?></td>
		<td align="center"><?php echo substr($row["cert"],0,30)."..."; ?></td>
	</tr><?php
	}
} ?>
</table>
