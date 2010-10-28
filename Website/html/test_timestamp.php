<?php

	$link = mysql_connect('localhost', 'zero1_sanjose', 'kanarick');
	if (!$link) {
		echo("can't connect");
	    die('Could not connect: ' . mysql_error());
	} else {
		echo("connected!\n");
	}


	mysql_select_db('zero1_pluginplay1',$link);
	$sql = "INSERT INTO test_timestamp VALUES (UNIX_TIMESTAMP());";
	$result = mysql_query($sql);
	if (!$result) {
		echo("query didn't work");
	    die('Invalid query: ' . mysql_error());
	}
	
	mysql_close($link);
	
	echo "timestamp entered!";

?>