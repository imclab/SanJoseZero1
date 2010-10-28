<?php

//Put in the key and secret for your Foursquare app
//Your values will be different than mine
$consumer_key = "1NE4MG4JCTMPQ0RGF4JNKE0ZQ3H2BK01C2U2IWTZNWF0LY4A";
$consumer_secret = "EVGYLILDZVW3H1CY3UJ0DPJKBWFI235ZLY1T24U5FTCVX1XH";

//Includes the foursquare-asyc library files
require_once('EpiCurl.php');
require_once('EpiOAuth.php');
require_once('EpiFoursquare.php');

session_start();
$foursquareObj = new EpiFoursquare($consumer_key, $consumer_secret);
$foursquareObj->setToken($_REQUEST['oauth_token'],$_SESSION['secret']);
$token = $foursquareObj->getAccessToken();

echo "<br>" . $token->oauth_token;
echo "<br>" . $token->oauth_token_secret . "<br>";

$foursquareObj->setToken($token->oauth_token, $token->oauth_token_secret);

try {
   //Making a call to the API
   $foursquareTest = $foursquareObj->get_user();
   print_r($foursquareTest->response);
 } catch (Exception $e) {
   echo "Error: " . $e;
 }

?>