<?php

	$buildings = $_POST["buildings"];
	
	//connect to db
	$link = mysql_connect('localhost', 'root', 'root');
	if (!$link) {
	    die('Could not connect: ' . mysql_error());
	} else {
		echo("connected!\n");
	}
	
	mysql_select_db('san_jose',$link);
	
	//create city block
	$sql = "INSERT INTO `san_jose`.`sj_city_block` (`id`, `time`) VALUES (UNIX_TIMESTAMP(), NOW());";
	$result = mysql_query($sql);
	if (!$result) {
	    die('Invalid query: ' . mysql_error());
	}
	//get city block
	$sql = "SELECT * FROM `sj_city_block` ORDER BY  `sj_city_block`.`id` DESC LIMIT 1";
	$result = mysql_query($sql);
	if (!$result) {
	    die('Invalid query: ' . mysql_error());
	}
	$array = mysql_fetch_array($result, MYSQL_ASSOC);
	$block_id = $array["id"];	
	
	echo("Created new block of ID ". $block_id . "\n");
	echo("this many buildings: ".count($buildings)."\n");
	
	//insert buildings into block
	for ($i = 0; $i<count($buildings); $i++){		
		$ranId = time() . "_" . $i;
		$sql = "INSERT INTO `sj_building` (`id`, `type`, `data`, `row`, `index`, `block_id`) VALUES ('" . $ranId . "',";
		$sql .= "'" . $buildings[$i]["type"] ."', ";
		$sql .= "'" . $buildings[$i]["data"] . "', ";
		$sql .= "'" . $buildings[$i]["row"] . "', ";
		$sql .= "'" . $buildings[$i]["index"] . "', ";
		$sql .= "'" . $block_id ."');";
		echo("Query is ".$sql . "\n");
		$result = mysql_query($sql);
		if (!$result) {
		    die('Invalid query: ' . mysql_error());
		}
	}
	
	mysql_close($link);

?>