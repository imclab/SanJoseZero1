<?php


	// Are we getting the current emitID?
	$getCurEmitID = "0";
	$getCurEmitID = $_POST['getCurEmitID'];
	
	// Are we inserting data?
	$logData = "0";
	$logData = $_POST['logData'];
	
	// Are we getting data?
	$getData = "0";
	$getData = $_POST['getData'];


	// Setup the xml results
//	header("Content-type: text/xml");
	$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
	$_xml .="<results>\r\n";


	// Get the highest Emit ID, stored in <curEmitID> tag
	if ( $getCurEmitID == "1" ) {
		$link = mysql_connect('localhost', 'zero1_sanjose', 'kanarick');
		if (!$link) {
			$_xml .="\t<error>could not connect to database</error>\r\n";
		} else {
			mysql_select_db('zero1_pluginplay1',$link);
			$sql = "SELECT * FROM sj_cdm_log WHERE emitID >= ALL (SELECT emitID FROM sj_cdm_log);";
			$result = mysql_query($sql);
			if (!$result) {
				$_xml .="\t<error>could not get current emitID</error>\r\n";
			} else {
				$array = mysql_fetch_array($result, MYSQL_ASSOC);
				$emitID = $array["emitID"];
				$_xml .="\t<curEmitID>".$emitID."</curEmitID>\r\n";
			}
			mysql_close($link);
		}
	
	// Insert data
	} else if ( $logData == "1" ) {
		$curEmitID = $_POST['curEmitID'];
		$date = $_POST['date'];
		$milliseconds = $_POST['milliseconds'];
		$activeParticles = $_POST['activeParticles'];
		
		$link = mysql_connect('localhost', 'zero1_sanjose', 'kanarick');
		if (!$link) {
			$_xml .="\t<error>could not connect to database</error>\r\n";
		} else {
			mysql_select_db('zero1_pluginplay1',$link);
			$sql = "INSERT INTO sj_cdm_log VALUES ('".$curEmitID."', '".$date."', '".$milliseconds."', UNIX_TIMESTAMP(), '".$activeParticles."');";
			$result = mysql_query($sql);
			if (!$result) {
				$_xml .="\t<error>invalid insert statement</error>\r\n";
			} else {
				$_xml .="\t<success>insert success!</success>\r\n";
			}
			mysql_close($link);
		}
	
	// Retreive data from the database
	} else if ( $getData == "1" ) {
		$date = $_POST['date'];
		$curEmitID = $_POST['curEmitID'];  // pass '-1' for the earliest emit ID
	
		$link = mysql_connect('localhost', 'zero1_sanjose', 'kanarick');
		if (!$link) {
			$_xml .="\t<error>could not connect to database</error>\r\n";
		} else {
			mysql_select_db('zero1_pluginplay1',$link);

			$sql = "SELECT * FROM sj_cdm_log WHERE date = ".$date." AND emitID > ".$curEmitID." ORDER BY emitID LIMIT 20;";
			$result = mysql_query($sql);

			if ( !$result ) {
				$_xml .="\t<error>could not get query results</error>\r\n";
			} else {
				while ($row = mysql_fetch_array ($result, MYSQL_ASSOC)) {
					$_xml .="\t<emission>\r\n";
					$_xml .="\t\t<emitID>".$row["emitID"]."</emitID>\r\n";
//					$_xml .="\t\t<date>".$row["date"]."</date>\r\n";
					$_xml .="\t\t<timestamp>".$row["timestamp"]."</timestamp>\r\n";
					$_xml .="\t\t<activeParticles>".$row["activeParticles"]."</activeParticles>\r\n";
					$_xml .="\t\t<milliseconds>".$row["milliseconds_since_last_emit"]."</milliseconds>\r\n";
					$_xml .="\t</emission>\r\n";
				}
			}
			mysql_close($link);
		}

	// Or, note that no command was supplied
	} else {
		$_xml .="\t<error>no command given</error>\r\n";
	}

	// Close out the xml file
	$_xml .="</results>";		
	echo $_xml;
?>
