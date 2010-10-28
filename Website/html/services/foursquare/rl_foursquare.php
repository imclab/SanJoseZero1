<?php
	require_once('EpiCurl.php');
	require_once('EpiOAuth.php');
	require_once('EpiFoursquare.php');
	
	// API Key Info, from website
	$consumer_key = "1NE4MG4JCTMPQ0RGF4JNKE0ZQ3H2BK01C2U2IWTZNWF0LY4A";
	$consumer_secret = "EVGYLILDZVW3H1CY3UJ0DPJKBWFI235ZLY1T24U5FTCVX1XH";
	
	// Secrets, generated from API, these are just for this application
	$token = "QUQAEJRQNZV0LWL5FPSTVT0NFGSU1NBC3BDVPSQDWV4YDGNM";
	$secret = "HRVDYR2V2FUVBTGHVCLIAYHVMP1PWTGYN2KKKUBPZU3LFM30";

	// minimum checkin time
	$minCheckIn = "-1";
	$mc = $_POST['mc'];
	if( $mc ) { $minCheckIn = $mc; }
	
	// venue id
	$venueId = "19462";
	$vid = $_POST['vid'];
	if( $vid ) { $venueId = $vid; }

	// radius in meters from venue ( currently not using )
	$radius = "";
	$rad = $_POST['radius'];
	if( $rad ) { $radius = $rad; }
	
	// nearbys
	$nb = "0";
	if( $_POST['nb'] ) { $nb = "1"; }

	// create new object with api key infor
	$foursquareObj = new EpiFoursquare($consumer_key, $consumer_secret);
	// oauth
	$foursquareObj->setToken($token, $secret);

	try {
		//set the vid as params for search 19462 penn station
 		$params = array("vid"=>$venueId);
   		
   		// go get stuff
  		$foursquareTest = $foursquareObj->get_venue( $params );
  		// get response info (we only care about venues)
  		$fsResponse = $foursquareTest->response['venue'];	
		
		/*
		// print the response nicely
		echo "<pre>";
		print_r( $fsResponse );
		echo "</pre>";
		*/
		
		// want nearbys? 1 = yes
		if( $nb == "1" ) {
			// set the params for new query
			$newParams = array( "geolat"=>$fsResponse['geolat'], "geolong"=>$fsResponse['geolong'], "l"=>"50" );
			// go get nearby locations based on the long and lat of the original vid
			$fsn = $foursquareObj->get_venues( $newParams );
			// we only care about groups
			$groups = $fsn->response['groups'];
			// and specifiically  the venues
			$nearbys = $groups[1];
			
			//loop through venues and grab the checkins
			foreach( $nearbys['venues'] as $venue ) {
				$stats = $venue['stats'];
				// is it NOT the original location? and NOT empty? AND close? (not used)
				if( $venue['id'] != $vid && $stats['herenow'] > 0 ) { //&& $venue['distance'] <= 10
					$ps = array( "vid"=>$venue['id'] );
					$fsvs = $foursquareObj->get_venue( $ps );
					$fsr = $fsvs->response['venue'];
					// big boy, we now have an array of all checkins at nearbys
					$nbcheckins = $fsr['checkins'];
				}
			}
			
			/*
			echo "<pre>";
			print_r( $fsr );
			echo "</pre>";			
			*/
		}
   		
   		// did we get a response?
		if ( $fsResponse ) {
			// set headers to print xml YOU CANNOT ECHO OR PRINT ANYTHING BEFORE THIS
			header("Content-type: text/xml");
			
			// some xml stuff
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";
			
			// is there a minimum timestamp? no, just return most recent place
			if( $minCheckIn == "-1" ) {
				$checkins = $fsResponse['checkins'];
				$checkin = $checkins[0];
				
				$greatestVIDCheckin = strtotime( $checkin['created'] );
				// if nb = 1 then we need to compare nearby checkins most recent and current locations checkins most recent
				if( $nb == "1" ) {
					$check = $nbcheckins[0];
					$greatestNearbyCheckin = strtotime( $check['created'] );
				
					// return the right one and you're done
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
					// return current locations most recent checkin
				} else {

					$person = $checkin['user'];	
					
					$_xml .="\t<checkin>\r\n";
					$_xml .="\t\t<person_name>".$person['firstname']." ".$person['lastname']."</person_name>\r\n";
					$_xml .="\t\t<venue_name>".$fsResponse['name']."</venue_name>\r\n";
					$_xml .="\t\t<timestamp>".$greatestVIDCheckin."</timestamp>\r\n";				
					$_xml .="\t</checkin>\r\n";	
				
				}
				// we only want results after this timestamp
			} else {
				// we don't want nearbys
				if( $nb == "0" ) {
					// loop through them and return xml and you're done
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
					//we want nearbys
				} else if( $nb == "1" ) {
					// loop through and your done
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
			
			//finish xml
			$_xml .="</results>";
			echo $_xml;
		} else {
			// no results return empty
			header("Content-type: text/xml");
	
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";
	
			$_xml .="</results>";
			echo $_xml;
		}
		
 	} catch (Exception $e) {
 		// error return empty
 		header("Content-type: text/xml");

		$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
		$_xml .="<results>\r\n";

		$_xml .="</results>";
		echo $_xml;
 	}
 	
?>