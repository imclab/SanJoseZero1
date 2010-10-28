<?php
	require( "db.php" );
	require( "Blocks.php" );

	$b = new Blocks();

	if( $_GET['bid'] ) {
		if( $_GET['next'] == 1 ) {
			$rows = $b->buildingsFromNextBlockId( $_GET['bid'] );
		} else {
			$rows = $b->buildingsFromBlockId( $_GET['bid'] );
		} 
		echo "<pre>";
		print_r($rows);
		echo "</pre>";	
	
	} else if( $_POST['bid'] ) {
		if( $_POST['next'] == 1 ) {
			$rows = $b->buildingsFromNextBlockId( $_POST['bid'] );
		} else {
			//$rows = $b->buildingsFromBlockId( $_POST['bid'] );
			$rows = $b->buildingsFromLastBlockAddedWithContent(  );
			//$rows = $b->getRecentBuildings(); 
		}
		
		if ( count( $rows ) > 0 ) {
			//echo "<div class='block'>";		
			foreach( $rows as $buildings ) {
				echo "<div class='emitters'  >";
				foreach( $buildings as $building ) {
					echo "<div class='emitter' title='".$building['block_id']."' >";
					$strippnp = str_replace( "/pluginplay/", "", $building['type']);
					$stripnb = str_replace( "/nearby", "", $strippnp);
					$pos = strpos( $stripnb, "_" );
					if( $pos ) {
						$newstring = substr( $stripnb, 0, $pos);//, strlen( $stripnb ) );
					} else {
						$newstring = $stripnb;
					}
					
					$pos2 = strpos( $newstring, "/" );
					if( $pos2 ) {
						$newstring = substr( $newstring, 0, $pos2 );
					}
					
					if( $stripnb ) {
						echo "<div class='img'><img src='img/".$newstring.".png' /></div>";
					} else {
						echo "<div class='img'><img src='img/empty.png' /></div>";
					}
					
					$data  = $building['data'];
					$twitterData = explode( "&*!", $data );
					
					echo "<div class='parts'>";
					//echo "<p class='data'>dummy data</p>";
					//echo "<p class='type'>".$newstring."</p>";
					if( $newstring == "twitter" || $newstring == "foursquare" ) {
						echo "<p class='type'>".$newstring.", ". $twitterData[2] .".</p>";
					} else {
						echo "<p class='type'>".$newstring.", ". $building['data'] .".</p>";
					}
					echo "</div>";
					echo "</div>";			
				}
				
				echo "</div>";
			}
		} else {
			echo "empty";
		}
	}
	
?>