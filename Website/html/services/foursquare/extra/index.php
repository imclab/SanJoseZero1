<?php

//Put in the key and secret for your Foursquare app
//Your values will be different than mine
$consumer_key = "1NE4MG4JCTMPQ0RGF4JNKE0ZQ3H2BK01C2U2IWTZNWF0LY4A";
$consumer_secret = "EVGYLILDZVW3H1CY3UJ0DPJKBWFI235ZLY1T24U5FTCVX1XH";
$loginurl = "";

//Includes the foursquare-asyc library files
require_once('EpiCurl.php');
require_once('EpiOAuth.php');
require_once('EpiFoursquare.php');

session_start();
try{
  $foursquareObj = new EpiFoursquare($consumer_key, $consumer_secret);
  $results = $foursquareObj->getAuthorizeUrl();
  $loginurl = $results['url'] . "?oauth_token=" . $results['oauth_token'];
  $_SESSION['secret'] = $results['oauth_token_secret'];
} catch (Execption $e) {
  //If there is a problem throw an exception
}

echo "<a href='" . $loginurl . "'>Login Via Foursquare</a>";  //Display the Foursquare login link
echo "<br>";
//This is your OAuth token and secret generated above
//The OAuth token is part of the Foursquare link above
//They are dynamic and will change each time you refresh the page
//If everything is working correctly both of these will show up when you open index.php
var_dump($results['oauth_token']);
echo "<br>";
var_dump($_SESSION['secret']);

?>