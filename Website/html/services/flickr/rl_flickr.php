<?php
	require_once("phpFlickr.php");
	
	// api key, info on website
	$api_key              	= "9d2a1eeb72830f58dd77d56672a3e54b";
	$api_secret             = "076ac459ed1b2cc9";
	
	// secret token, for this application only 
	$token					= "72157624696645001-37e82552010bcade";
	
	// are we doing geo search or tag search?
	$geoSearch = "0";
	$gsearch = $_POST['gsearch'];
	if( $gsearch ) { $geoSearch = "1"; }
	
	// minimum upload date
	$minTimeStamp = -1;
	$ts = $_POST['ts'];
	if( $ts ) { $minTimeStamp = $ts; }
	
	// string of tags to search for
	$tagString = "";
	$tags = $_POST['tag'];
	if( $tags ) { $tagString = implode( $tags, ", " ); }
	
	// radius, long and lat for geo search
	$radius = "";
	$long = "";
	$lat = "";
	$getRadius = $_POST['radius'];
	$getLong = $_POST['long'];
	$getLat = $_POST['lat'];
	if( $getRadius && $getLong && $getLat ) {
		$radius = $getRadius;
		$long = $getLong;
		$lat = $getLat;
	}
	
	// create new object 
	$f = new phpFlickr($api_key, $api_secret);
	// auth it
	$f->setToken($token);
	
	// yes geosearch
	if( $geoSearch == "1" ) {
		$photos = $f->photos_search( array(	"lon"=>$long,
											"lat"=>$lat,
											"radius"=>$radius,
											"radius_units"=>"mi",
											"min_upload_date"=>$minTimeStamp,
											"sort"=>"date-posted-desc",
											"extras"=>"date_upload,geo,owner_name,url_m" ) );		
	} else { // or tag search
		$photos = $f->photos_search(array(	"tags"=>$tagString,
											"tag_mode"=>"any",
											"min_upload_date"=>$minTimeStamp,
											"sort"=>"date-posted-desc",
											"extras"=>"date_upload,geo,owner_name,url_m" ));
	}
	
    // sort the array because Flickr sucks and messes up the timestamps
    function array_sort($array, $on, $order='SORT_DESC') {
      $new_array = array();
      $sortable_array = array();
 
      if (count($array) > 0) {
          foreach ($array as $k => $v) {
              if (is_array($v)) {
                  foreach ($v as $k2 => $v2) {
                      if ($k2 == $on) {
                          $sortable_array[$k] = $v2;
                      }
                  }
              } else {
                  $sortable_array[$k] = $v;
              }
          }
 
          switch($order) {
              case 'SORT_ASC':   
                  //echo "ASC";
                  asort($sortable_array);
              break;
              case 'SORT_DESC':
                  //echo "DESC";
                  arsort($sortable_array);
              break;
          }
 
          foreach($sortable_array as $k => $v) {
              $new_array[] = $array[$k];
          }
      }
      return $new_array;
    } 	
	
	$tempPhoto = $photos['photo'];
	$photoArray = array_sort( $tempPhoto, "dateupload" );
	
	/*
	echo "<pre>";
	$tempPhoto = $photos['photo'];
	print_r( $tempPhoto );
	$photo = array_sort( $tempPhoto, "dateupload" );
	print_r( $newPhoto);
	echo "</pre>";
	*/
	
	
	// print XML to page for return
	if ( $photos ) {
		header("Content-type: text/xml");
		$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
		$_xml .="<results>\r\n";
		
		// we just want to return one photo
		if( $minTimeStamp == -1 ) {
			//$photoArray = $photos['photo'];
			$photo = $photoArray[0];
			
			$_xml .="\t<photo>\r\n";
			$_xml .="\t\t<user_name>".$photo['ownername']."</user_name>\r\n";
			$_xml .="\t\t<title><![CDATA[".$photo['title']."]]></title>\r\n";
			//$_xml .="\t\t<title>".$photo['title']."</title>\r\n";	
			$_xml .="\t\t<link><![CDATA[".$photo['url_m']."]]></link>\r\n";			
			$_xml .="\t\t<timestamp>".$photo['dateupload']."</timestamp>\r\n";
			$_xml .="\t</photo>\r\n";			
		} else {
			// loop through all recent photos that were uploaded recently
			foreach( $photoArray as $photo ) {
				if( (float)$photo['dateupload'] <= (float)$minTimeStamp ) {
					break;
				} else {
					$_xml .="\t<photo>\r\n";
					$_xml .="\t\t<user_id>".$photo['ownername']."</user_id>\r\n";
					$_xml .="\t\t<title><![CDATA[".$photo['title']."]]></title>\r\n";
					//$_xml .="\t\t<title>".$photo['title']."</title>\r\n";
					$_xml .="\t\t<link><![CDATA[".$photo['url_m']."]]></link>\r\n";				
					$_xml .="\t\t<timestamp>".$photo['dateupload']."</timestamp>\r\n";
					// link
					$_xml .="\t</photo>\r\n";
				}
			}
		}
		
		// wrap up xml
		
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
	
?>