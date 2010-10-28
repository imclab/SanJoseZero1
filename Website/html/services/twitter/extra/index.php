<?php
/**
 * @file
 * User has successfully authenticated with Twitter. Access tokens saved to session and DB.
 */

/* Load required lib files. */
session_start();
require_once('twitteroauth/twitteroauth.php');
require_once('config.php');

$token = "116859961-IfPlwpEc431s0CFpCXuo15tzwILLdvkdHl9tWijx";  // hardcoded for our client/app
$secret = "RLRPSi1JzMPKntDorVg3SbgQAItqxEiZ6MfgxjdfS4A"; // hardcoded for our client/app

/* If access tokens are not available redirect to connect page. */
//if (empty($_SESSION['access_token']) || empty($_SESSION['access_token']['oauth_token']) || empty($_SESSION['access_token']['oauth_token_secret'])) {
//    header('Location: ./clearsessions.php');
//}
/* Get user access tokens out of the session. */
$access_token = $_SESSION['access_token'];

//echo $access_token['oauth_token'] . "<br/>";
//echo $access_token['oauth_token_secret'] . "<br/>";

/* Create a TwitterOauth object with consumer/user tokens. */
//$connection = new TwitterOAuth(CONSUMER_KEY, CONSUMER_SECRET, $access_token['oauth_token'], $access_token['oauth_token_secret']);
$connection = new TwitterOAuth(CONSUMER_KEY, CONSUMER_SECRET, $token, $secret);

/* If method is set change API call made. Test is called by default. */
$content = $connection->get('statuses/mentions');

/*
echo "<pre />";
print_r( $content );
echo "<pre />";
*/

// do some silly stuff to get at contents
/*
for( $j = 0 ; $j < count($content); $j++ ) {
	$vars = get_object_vars( $content[$j] );
	$user = get_object_vars( $vars['user'] );
	//echo "id:" . $user['id'] . ", name: " . $user['name'] . ", tweetID: " . $vars['id'] . "<br />";
	$time = $vars['created_at'];
	$timestamp = strtotime( $vars['created_at'] );
	date_default_timezone_set('GMT');
	$stampCheck = strftime( "%a %b %e %T %z %Y", $timestamp );	
	//$stampCheck = strftime( "%c", $timestamp );
	echo $time."<br />";
	echo $timestamp."<br />";
	echo $stampCheck."<br /><br />";
}

echo "<br />";
*/

if ( $content ) {
	$file= fopen("tw_results.xml", "w");
	$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
	$_xml .="<settings>\r\n";
	
	for( $j = 0 ; $j < count($content); $j++ ) {
		$vars = get_object_vars( $content[$j] );
		$user = get_object_vars( $vars['user'] );
		//echo "id:" . $user['id'] . ", name: " . $user['name'] . ", tweetID: " . $vars['id'] . "<br />";
		$timestamp = strtotime( $vars['created_at'] );
		
		$_xml .="\t<tweet>\r\n";
		$_xml .="\t\t<user_id>".$user['id']."</user_id>\r\n";
		$_xml .="\t\t<user_name>".$user['name']."</user_name>\r\n";
		$_xml .="\t\t<content>".$vars['text']."</content>\r\n";
		$_xml .="\t\t<tweet_id>".$vars['id']."</tweet_id>\r\n";
		$_xml .="\t\t<timestamp>".$timestamp."</timestamp>\r\n";
		$_xml .="\t</tweet>\r\n";
	}
	
	$_xml .="</settings>";
	fwrite($file, $_xml);
	fclose($file);
	echo "XML has been written.  <a href=\"results.xml\">View the XML.</a>";

} else {
	echo "No Records found";
}


/*
// dump everything
echo "<pre>";
print_r( $content );
echo "</pre>";
*/

/* Some example calls */
//$connection->get('users/show', array('screen_name' => 'abraham')));
//$connection->post('statuses/update', array('status' => date(DATE_RFC822)));
//$connection->post('statuses/destroy', array('id' => 5437877770));
//$connection->post('friendships/create', array('id' => 9436992)));
//$connection->post('friendships/destroy', array('id' => 9436992)));

?>