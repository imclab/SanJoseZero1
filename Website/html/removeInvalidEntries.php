<?php

	// connect to db
	$link = mysql_connect('localhost', 'zero1_sanjose', 'kanarick');
	if (!$link) {
	    die('Could not connect: ' . mysql_error());
	} else {
		echo("connected!\n");
	}
	
	mysql_select_db('zero1_pluginplay1',$link);
	
	// 14th - 19th
	// UNIX TIMESTAMP on SJ servers is GMT time.
	//				SJ time is GMT - 7hrs.
	$day = 14;
	
	while ($day <= 19) {
		// 2:00 + 7hrs
		$start_clear_timestamp = mktime(9,0,0,9,$day,2010);
		$start_clear_timestamp_str = (string)$start_clear_timestamp;
		// 20:00 + 7hrs
		$next_day = $day + 1;
		$end_clear_timestamp = mktime(3,0,0,9,$next_day,2010);
		$end_clear_timestamp_str = (string)$end_clear_timestamp;
		
		echo("day: ".$day.", start time: ".$start_clear_timestamp_str.", end time: ".$end_clear_timestamp_str."\n");
		
		$sql = "DELETE FROM `sj_city_block` WHERE id > ".$start_clear_timestamp_str." AND id < ".$end_clear_timestamp_str.";";
		$result = mysql_query($sql);
		echo("Query for day: ".$day." sj_city_block");
		if (!$result) {
			die('Invalid query1: '.mysql_error());
		}
		$sql = "DELETE FROM `sj_building` WHERE block_id > ".$start_clear_timestamp_str." AND block_id < ".$end_clear_timestamp_str.";";
		$result = mysql_query($sql);
		echo("Query for day: ".$day." sj_building");
		if (!$result) {
			die('Invalid query2: '.mysql_error());
		}
		
		$day = $day + 1;
		
	}
	
	echo("DONE REMOVING EXTRANEOUS INFO");
	
	

	mysql_close($link);
?>