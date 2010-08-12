<?php
	$images = array();
	$images["/pluginplay/birdfeeder"] = "images/bird00.png";
	$images["/pluginplay/flickr"] = "images/flickr00.png";
	$images["/pluginplay/foursquare"] = "images/foursq00.png";
	$images["/pluginplay/foursquare/nearby"] = "images/foursq01.png";
	$images["/pluginplay/hopscotch"] = "images/hops00.png";
	$images["/pluginplay/megaphone"] = "images/mega00.png";
	$images["/pluginplay/picnictable"] = "images/picnic00.png";
	$images["/pluginplay/playground"] = "images/play00.png";
	$images["/pluginplay/signal"] = "images/signal00.png";
	$images["/pluginplay/twitter"] = "images/twitter0.png";

	$currentRow = $_POST["row"];
	$newCurrentRow = $currentRow;
	
	$blocks = array();
	//connect to db
	$link = mysql_connect('localhost', 'root', 'root');
	if (!$link) {
	    die('Could not connect: ' . mysql_error());
	} else {
	}
	
	mysql_select_db('san_jose',$link);
	
	$sql = "SELECT * FROM `sj_city_block` ORDER BY `id` DESC limit 0, 100";
	$result = mysql_query($sql);
	$numBlocks = mysql_num_rows($result);
	
	//get all the blocks
	for ($i=0; $i<$numBlocks; $i++){
		$array = mysql_fetch_array($result, MYSQL_ASSOC);
		$block = array();
		$block["id"] = $array["id"];
		if ($i == 0) $newCurrentRow = $block["id"];
		
		if ($block["id"] == $currentRow){
			break;
		}
		
		$block["rows"] = array();
		array_push($blocks, $block);
	}
		
	//loop through blocks + query db
	for ($i=0; $i<count($blocks); $i++){
		$sql = "SELECT * FROM `sj_building` WHERE `block_id` = '" . $blocks[$i]["id"] . "' ORDER BY `row`,`index` ASC limit 0, 30";
		$result = mysql_query($sql);
		if (!$result) {
		    die('Invalid query: ' . mysql_error());
		}
		
		$numBuildings = mysql_num_rows($result);		
		$row = 0;
		$newRow = array();
		array_push($buildings[$i]["rows"], $newRow);
		
		for ($j=0; $j<$numBuildings; $j++){
			$building = mysql_fetch_array($result, MYSQL_ASSOC);
			while ($building["row"] != $row){
				$row++;
				$newRow = array();
				array_push($buildings[$i]["rows"], $newRow);
			}			
			array_push($buildings[$i]["rows"][$row], $building);
		}		
	}
		
	mysql_close($link);
	echo($newCurrentRow . ";");
		for ($i=0; $i<count($buildings); $i++){
?>
	<tr id="<?php echo("block:".$i); ?>">
		<td class="row"><div><?php echo("row is ". $blocks[$i]["id"]); ?></div><br />
<?php
		for ($j=0; $j<count($buildings[$i]["rows"]); $j++){
?>
		<div id="<?php echo("stack:".$i . ":" . $j); ?>" class="stack">
<?php	
			for ($k=0; $k<count( $buildings[$i]["rows"][$j]); $k++){
?>
			<div class="building"><img width="20px" src="<?php echo($images[$buildings[$i]["rows"][$j][$k]["type"]]); ?>" /><br /></div>
<?php
			}
			if (count( $buildings[$i]["rows"][$j]) == 0){
?>
			<div class="building"><img src="images/spacer.gif" width="20px" /><br /></div>
<?php			
			}

?>
		</div>
<?php
		}
?>
		</td>
	</tr>
<?php
	}
?>