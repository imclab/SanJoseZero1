<?php
	require_once('twitteroauth/twitteroauth.php');
	require_once('TwitterSearch.php');
	
	$consumer_key = "wvAinuO5swtOWqmBBy7w";
	$consumer_secret = "4BoTgCceQzucUBWKDWfQjyhxqqYEkO65ueA2iF2w";
	$token = "116859961-IfPlwpEc431s0CFpCXuo15tzwILLdvkdHl9tWijx";
	$secret = "RLRPSi1JzMPKntDorVg3SbgQAItqxEiZ6MfgxjdfS4A";
	
	$minSince = "-1";
	$ms = $_GET['ms'];
	if( $ms ) { $minSince = $ms; }
	
	$doHash = "0";
	$doHash = $_GET['hash'];
	
	$tagString = "";
	$tags = $_GET['tag'];
	for( $i = 0; $i < count($tags); $i++ ) { $tags[$i] = "#".$tags[$i]; }
	if( $tags ) { $tagString = implode( $tags, " OR " ); }
	
	
	
	if( $doHash == "1" ) {
		$hashSearch = new TwitterSearch($tagString);
		$hashSearch->user_agent = 'rl_twittersearch:chrisallick@gmail.com';
		$hashSearch->rpp = '100';
		if( $minSince != "-1" ) { $hashSearch->since = $minSince; }
		$hashSearchResults = $hashSearch->results();

		if( $hashSearchResults ) {
			header("Content-type: text/xml");
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";
			
			if( $minSince == "-1" ) {
					$hashVars = get_object_vars( $hashSearchResults[0] );
					$timestamp = strtotime( $hashVars['created_at'] );
					
					$_xml .="\t<tweet type='hash'>\r\n";
					$_xml .="\t\t<user_name>".$hashVars['from_user']."</user_name>\r\n";
					$_xml .="\t\t<content>".$hashVars['text']."</content>\r\n";
					$_xml .="\t\t<tweet_id>".$hashVars['id']."</tweet_id>\r\n"; // this is the limit for since_id
					$_xml .="\t\t<timestamp>".$timestamp."</timestamp>\r\n";			
					$_xml .="\t</tweet>\r\n";				
			} else {
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
	} else if( $doHash == "0" ) {
		$connection = new TwitterOAuth($consumer_key, $consumer_secret, $token, $secret);
		
		if( $minSince == "-1" ) {
			$content = $connection->get('statuses/mentions');
		} else {
			$content = $connection->get('statuses/mentions', array('since_id' => $minSince) );
		}
		
		$response = get_object_vars($content);
		
		if ( !$response['error'] ) {
			header("Content-type: text/xml");
			
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";
			
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
			header("Content-type: text/xml");
			$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
			$_xml .="<results>\r\n";

			$_xml .="</results>";
			echo $_xml;
		}		
	}
?>
