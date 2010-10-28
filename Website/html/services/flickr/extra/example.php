<?php
/* Last updated with phpFlickr 1.3.2
 *
 * This example file shows you how to call the 100 most recent public
 * photos.  It parses through them and prints out a link to each of them
 * along with the owner's name.
 *
 * Most of the processing time in this file comes from the 100 calls to
 * flickr.people.getInfo.  Enabling caching will help a whole lot with
 * this as there are many people who post multiple photos at once.
 *
 * Obviously, you'll want to replace the "<api key>" with one provided 
 * by Flickr: http://www.flickr.com/services/api/key.gne
 */

require_once("phpFlickr.php");
$api_key                 = "9d2a1eeb72830f58dd77d56672a3e54b";
$api_secret              = "076ac459ed1b2cc9";

$f = new phpFlickr("9d2a1eeb72830f58dd77d56672a3e54b", "076ac459ed1b2cc9");
$f->setToken("72157624696645001-37e82552010bcade");

$minTimeStamp = "";

$photos = $f->photos_search(array("tags"=>"brown, cow, dead", "tag_mode"=>"all", "min_upload_date"=>"1267377041", "extras"=>"date_upload,date_taken" ));
//"max_upload_date"=>"1267377041"
/*
echo "<pre>";
foreach( $photos['photo'] as $photo ) {
	print_r( $photo );
	//echo "photo id: ".$photo['id'].", owner: ".$photo['owner'].", title: ".$photo['title'].", date: ".$photo['datetaken']."<br />";
}
echo "</pre>";
*/

if ( $photos ) {
	$file= fopen("fl_results.xml", "w");
	$_xml ="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n";
	$_xml .="<settings>\r\n";
	
	foreach( $photos['photo'] as $photo ) {
		$_xml .="\t<photo>\r\n";
		$_xml .="\t\t<user_id>".$photo['owner']."</user_id>\r\n";
		$_xml .="\t\t<title>".$photo['title']."</title>\r\n";
		$_xml .="\t\t<photo_id>".$photo['id']."</photo_id>\r\n";
		$_xml .="\t\t<timestamp>".$photo['dateupload']."</timestamp>\r\n";		
		$_xml .="\t\t<timestamp2>".strftime("%c", $photo['dateupload'] )."</timestamp2>\r\n";				
		$_xml .="\t</photo>\r\n";
	}
	
	$_xml .="</settings>";
	fwrite($file, $_xml);
	fclose($file);
	echo "XML has been written.  <a href=\"results.xml\">View the XML.</a>";

} else {
	echo "No Records found";
}

/*
$recent = $f->photos_getRecent();

foreach ($recent['photo'] as $photo) {
    $owner = $f->people_getInfo($photo['owner']);
    echo "<a href='http://www.flickr.com/photos/" . $photo['owner'] . "/" . $photo['id'] . "/'>";
    echo $photo['title'];
    echo "</a> Owner: ";
    echo "<a href='http://www.flickr.com/people/" . $photo['owner'] . "/'>";
    echo $owner['username'];
    echo "</a><br>";
}
*/

?>
