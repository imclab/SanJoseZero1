<?php
	$link = mysql_connect('localhost', 'zero1_sanjose', 'kanarick');
	if (!$link) { die( 'Could not connect: ' . mysql_error() ); }	
	mysql_select_db('zero1_pluginplay1',$link);
?>