<?php
	$consumer_key = "1NE4MG4JCTMPQ0RGF4JNKE0ZQ3H2BK01C2U2IWTZNWF0LY4A";
	$consumer_secret = "EVGYLILDZVW3H1CY3UJ0DPJKBWFI235ZLY1T24U5FTCVX1XH";
	
	//sneaky token and secret stored
	$token = "QUQAEJRQNZV0LWL5FPSTVT0NFGSU1NBC3BDVPSQDWV4YDGNM";
	$secret = "HRVDYR2V2FUVBTGHVCLIAYHVMP1PWTGYN2KKKUBPZU3LFM30";

	require_once('EpiCurl.php');
	require_once('EpiOAuth.php');
	require_once('EpiFoursquare.php');

	//session_start();
	$foursquareObj = new EpiFoursquare($consumer_key, $consumer_secret);
	//$foursquareObj->setToken($_REQUEST['oauth_token'],$_SESSION['secret']);
	//$token = $foursquareObj->getAccessToken();
	$foursquareObj->setToken($token, $secret);

	// print oauth info for storage
	//echo "<br>" . $token->oauth_token;
	//echo "<br>" . $token->oauth_token_secret . "<br>";

	try {
   		//61626 san jose city hall
   		//19462 ny penn station
   		$params = array("vid"=>19462); 
  		$foursquareTest = $foursquareObj->get_venue( $params );
  		$fsResponse = $foursquareTest->response['venue'];
  		/*
  		echo "<pre>";
  		print_r( $fsResponse );
  		echo "</pre>";
  		
  		*/
  		/*
  		foreach( $fsResponse['checkins'] as $checkin ) {
  			//$person = $checkin['user'];
   			//$aDate = date( $checkin['created'] );
   			//echo "id:" . $checkin['id'] . ", created: " . $checkin['created'] . ", person_id: ".$person['id']."<br />";
   			echo strtotime($checkin['created'])."<br/>";
   		}
   		*/
   		
		if ( $fsResponse ) {
			$file= fopen("fo_results.xml", "w");
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<settings>\r\n";
			
			foreach( $fsResponse['checkins'] as $checkin ) {
				$person = $checkin['user'];
				$_xml .="\t<checkin>\r\n";
				$_xml .="\t\t<checkin_id>".$checkin['id']."</checkin_id>\r\n";
				$_xml .="\t\t<person_id>".$person['id']."</person_id>\r\n";
				$_xml .="\t\t<timestamp>".strtotime($checkin['created'])."</timestamp>\r\n";				
				$_xml .="\t</checkin>\r\n";
			}
			
			$_xml .="</settings>";
			fwrite($file, $_xml);
			fclose($file);
			echo "XML has been written.  <a href=\"results.xml\">View the XML.</a>";
		
		} else {
			echo "No Records found";
		}  
		
   		
 	} catch (Exception $e) {
   		echo "Error: " . $e;
 	}
?>