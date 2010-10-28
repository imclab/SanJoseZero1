<?php
	require_once('twitteroauth/twitteroauth.php');
	require_once('TwitterSearch.php');
	
	// api key info from website
	$consumer_key = "wvAinuO5swtOWqmBBy7w";
	$consumer_secret = "4BoTgCceQzucUBWKDWfQjyhxqqYEkO65ueA2iF2w";
	
	// secret oauth stuff for this application
	$token = "116859961-IfPlwpEc431s0CFpCXuo15tzwILLdvkdHl9tWijx";
	$secret = "RLRPSi1JzMPKntDorVg3SbgQAItqxEiZ6MfgxjdfS4A";
	
	// min timestamp for tweets
	$minSince = "-1";
	$ms = $_POST['ms'];
	if( $ms ) { $minSince = $ms; }
	
	// are we doing a hash search or @ search?
	$doHash = "0";
	$doHash = $_POST['hash'];
	
	// hash tags to search for
	$tagString = "";
	$tags = $_POST['tag'];
	for( $i = 0; $i < count($tags); $i++ ) { $tags[$i] = "#".$tags[$i]; }
	if( $tags ) { $tagString = implode( $tags, " OR " ); }
	
	
	// do hash search, un authenticated and uses different api
	if( $doHash == "1" ) {
		// create new object and pass the hash tags
		$hashSearch = new TwitterSearch($tagString);
		// set the user agent
		$hashSearch->user_agent = 'rl_twittersearch:chrisallick@gmail.com';
		// we want all we can get back
		$hashSearch->rpp = '100';
		//time stamp yes or no?
		if( $minSince != "-1" ) { $hashSearch->since = $minSince; }
		$hashSearchResults = $hashSearch->results();

		// if results
		if( $hashSearchResults ) {
			header("Content-type: text/xml");
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";
			
			// only return one tweet if we don't set the timestamp
			if( $minSince == "-1" ) {
					$hashVars = get_object_vars( $hashSearchResults[0] );
					$timestamp = strtotime( $hashVars['created_at'] );
					
					$_xml .="\t<tweet type='hash'>\r\n";
					$_xml .="\t\t<user_name>".$hashVars['from_user']."</user_name>\r\n";
					$_xml .="\t\t<content>".$hashVars['text']."</content>\r\n";
					$_xml .="\t\t<tweet_id>".$hashVars['id']."</tweet_id>\r\n"; // this is the limit for since_id
					$_xml .="\t\t<timestamp>".$timestamp."</timestamp>\r\n";			
					$_xml .="\t</tweet>\r\n";				
			} else { // return all the tweets
				foreach( $hashSearchResults as $hash ) {
					$hashVars = get_object_vars( $hash );
					$timestamp = strtotime( $hashVars['created_at'] );
					
					$_xml .="\t<tweet type='hash'>\r\n";
					$_xml .="\t\t<user_name>".$hashVars['from_user']."</user_name>\r\n";
					$_xml .="\t\t<content>".$hashVars['text']."</content>\r\n";
					$_xml .="\t\t<tweet_id>".$hashVars['id']."</tweet_id>\r\n"; // this is the limit for since_id
					$_xml .="\t\t<timestamp>".$timestamp."</timestamp>\r\n";		
					$_xml .="\t</tweet>\r\n";
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
		// we want that @replies, and we want to do an authenticated search
	} else if( $doHash == "0" ) {
		// create new object and pass the info
		$connection = new TwitterOAuth($consumer_key, $consumer_secret, $token, $secret);
		
		// if we care about timestamp set it here
		if( $minSince == "-1" ) {
			$content = $connection->get('statuses/mentions');
		} else {
			$content = $connection->get('statuses/mentions', array('since_id' => $minSince) );
		}
		
		// get the response and check for error
		$response = get_object_vars($content);
		
		// if no error col
		if ( !$response['error'] ) {
			header("Content-type: text/xml");
			
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";
			
			//return latest tweet if no timestamp
			if( $minSince == "-1" ) {
					$vars = get_object_vars( $content[0] );
					$user = get_object_vars( $vars['user'] );
					$timestamp = strtotime( $vars['created_at'] );
					
					$_xml .="\t<tweet type='reply'>\r\n";
					$_xml .="\t\t<user_name>".$user['name']."</user_name>\r\n";
					$_xml .="\t\t<content>".$vars['text']."</content>\r\n";
					$_xml .="\t\t<tweet_id>".$vars['id']."</tweet_id>\r\n";
					$_xml .="\t\t<timestamp>".$timestamp."</timestamp>\r\n";
					$_xml .="\t</tweet>\r\n";			
			} else {
				// loop through all tweets since timestamp
				for( $j = 0 ; $j < count($content); $j++ ) {
					$vars = get_object_vars( $content[$j] );
					$user = get_object_vars( $vars['user'] );
					$timestamp = strtotime( $vars['created_at'] );
					
					$_xml .="\t<tweet type='reply'>\r\n";
					$_xml .="\t\t<user_name>".$user['name']."</user_name>\r\n";
					$_xml .="\t\t<content>".$vars['text']."</content>\r\n";
					$_xml .="\t\t<tweet_id>".$vars['id']."</tweet_id>\r\n";
					$_xml .="\t\t<timestamp>".$timestamp."</timestamp>\r\n";
					$_xml .="\t</tweet>\r\n";
				}
			}
			
			$_xml .="</results>";
			echo $_xml;		
		} else {
			// error return empty
			header("Content-type: text/xml");
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";

			$_xml .="</results>";
			echo $_xml;
		}		
	}
?>