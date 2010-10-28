<?php
	require_once('EpiCurl.php');
	require_once('EpiOAuth.php');
	require_once('EpiFoursquare.php');
	
	$consumer_key = "1NE4MG4JCTMPQ0RGF4JNKE0ZQ3H2BK01C2U2IWTZNWF0LY4A";
	$consumer_secret = "EVGYLILDZVW3H1CY3UJ0DPJKBWFI235ZLY1T24U5FTCVX1XH";
	$token = "QUQAEJRQNZV0LWL5FPSTVT0NFGSU1NBC3BDVPSQDWV4YDGNM";
	$secret = "HRVDYR2V2FUVBTGHVCLIAYHVMP1PWTGYN2KKKUBPZU3LFM30";

	$minCheckIn = "-1";
	$mc = $_GET['mc'];
	if( $mc ) { $minCheckIn = $mc; }
	
	$venueId = "19462";
	$vid = $_GET['vid'];
	if( $vid ) { $venueId = $vid; }

	$radius = "";
	$rad = $_GET['radius'];
	if( $rad ) { $radius = $rad; }
	
	$nb = "0";
	if( $_GET['nb'] ) { $nb = "1"; }

	$foursquareObj = new EpiFoursquare($consumer_key, $consumer_secret);
	$foursquareObj->setToken($token, $secret);

	try {
		//19462
 		$params = array("vid"=>$venueId);
   		
  		$foursquareTest = $foursquareObj->get_venue( $params );
  		$fsResponse = $foursquareTest->response['venue'];	
		
		/*
		echo "<pre>";
		print_r( $fsResponse );
		echo "</pre>";
		*/
		
		if( $nb == "1" ) {
			$newParams = array( "geolat"=>$fsResponse['geolat'], "geolong"=>$fsResponse['geolong'], "l"=>"50" );
			$fsn = $foursquareObj->get_venues( $newParams );
			$groups = $fsn->response['groups'];
			$nearbys = $groups[1];
			
			foreach( $nearbys['venues'] as $venue ) {
				$stats = $venue['stats'];
				if( $venue['id'] != $vid && $stats['herenow'] > 0 ) { //&& $venue['distance'] <= 10
					$ps = array( "vid"=>$venue['id'] );
					$fsvs = $foursquareObj->get_venue( $ps );
					$fsr = $fsvs->response['venue'];
					$nbcheckins = $fsr['checkins'];
				}
			}
			
			/*
			echo "<pre>";
			print_r( $fsr );
			echo "</pre>";			
			*/
		}
   		
		if ( $fsResponse ) {
			header("Content-type: text/xml");
			
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";
			
			if( $minCheckIn == "-1" ) {
				$checkins = $fsResponse['checkins'];
				$checkin = $checkins[0];
				
				$greatestVIDCheckin = strtotime( $checkin['created'] );
				if( $nb == "1" ) {
					$check = $nbcheckins[0];
					$greatestNearbyCheckin = strtotime( $check['created'] );
				
					if( $greatestVIDCheckin > $greatestNearbyCheckin ) {
						$person = $checkin['user'];	
						
						$_xml .="\t<checkin>\r\n";
						$_xml .="\t\t<person_name>".$person['firstname']." ".$person['lastname']."</person_name>\r\n";
						$_xml .="\t\t<venue_name>".$fsResponse['name']."</venue_name>\r\n";
						$_xml .="\t\t<timestamp>".$greatestVIDCheckin."</timestamp>\r\n";				
						$_xml .="\t</checkin>\r\n";					
					} else {
						$person = $check['user'];
						
						$_xml .="\t<checkin>\r\n";
						$_xml .="\t\t<person_name>".$person['firstname']." ".$person['lastname']."</person_name>\r\n";
						$_xml .="\t\t<venue_name>".$fsr['name']."</venue_name>\r\n";
						$_xml .="\t\t<timestamp>".$greatestNearbyCheckin."</timestamp>\r\n";				
						$_xml .="\t</checkin>\r\n";						
					}
				} else {

					$person = $checkin['user'];	
					
					$_xml .="\t<checkin>\r\n";
					$_xml .="\t\t<person_name>".$person['firstname']." ".$person['lastname']."</person_name>\r\n";
					$_xml .="\t\t<venue_name>".$fsResponse['name']."</venue_name>\r\n";
					$_xml .="\t\t<timestamp>".$greatestVIDCheckin."</timestamp>\r\n";				
					$_xml .="\t</checkin>\r\n";	
				
				}
			} else {
				foreach( $fsResponse['checkins'] as $checkin ) {
					$timestamp = strtotime( $checkin['created'] );
					if( $timestamp == $minCheckIn ) {
						break;
					}
	
					$person = $checkin['user'];
					$_xml .="\t<checkin>\r\n";
					$_xml .="\t\t<person_name>".$person['firstname']." ".$person['lastname']."</person_name>\r\n";
					$_xml .="\t\t<venue_name>".$fsResponse['name']."</venue_name>\r\n";
					$_xml .="\t\t<timestamp>".$timestamp."</timestamp>\r\n";				
					$_xml .="\t</checkin>\r\n";				
				}
				if( $nb == "1" ) {
					foreach( $nbcheckins as $check ) {
						$timestamp = strtotime( $check['created'] );
						if( $timestamp == $minCheckIn ) {
							break;
						}
		
						$person = $check['user'];
						$_xml .="\t<checkin>\r\n";
						$_xml .="\t\t<person_name>".$person['firstname']." ".$person['lastname']."</person_name>\r\n";
						$_xml .="\t\t<venue_name>".$fsr['name']."</venue_name>\r\n";
						$_xml .="\t\t<timestamp>".$timestamp."</timestamp>\r\n";				
						$_xml .="\t</checkin>\r\n";		
					}
				}
			}
			
			$_xml .="</results>";
			echo $_xml;
		} else {
			header("Content-type: text/xml");
	
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";
	
			$_xml .="</results>";
			echo $_xml;
		}
		
 	} catch (Exception $e) {
 		header("Content-type: text/xml");

		$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
		$_xml .="<results>\r\n";

		$_xml .="</results>";
		echo $_xml;
 	}
 	
?>