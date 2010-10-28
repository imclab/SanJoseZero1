<?php
	require( "db.php" );
	require( "Blocks.php" );

	$b = new Blocks();

	if( $_GET['limit'] ) {
		$rows = $b->getRecentBuildings( $_GET['bid'], $_GET['limit'], $_GET['start'] );
		
		echo "<pre>";
		print_r($rows);
		echo "</pre>";	
	
	} else if( $_POST['limit'] ) {
		$rows = $b->getRecentBuildings( $_POST['bid'], $_POST['limit'], $_POST['start'] );
		if ( count( $rows ) > 0 ) {	
			foreach( $rows as $key => $block ) {
				echo "<div class='block' title='" . $key . "'>";
				if( count( $block ) != 0 ) {
					foreach( $block as $buildings ) {
						echo "<div class='buildings' title='' >";
						foreach( $buildings as $building ) {
							echo "<div class='building' title='".$building['block_id']."' >";
								$strippnp = str_replace( "/pluginplay/", "", $building['type']);
								$stripnb = str_replace( "/nearby", "", $strippnp);
								if( $stripnb ) {
									echo "<div class='img'><img src='img/".$stripnb.".png' /></div>";
								} else {
									echo "<div class='img'><img src='img/empty.png' /></div>";
								}
								echo "<div class='parts'>";
									echo "<p class='data'>dummy data</p>";
									echo "<p class='type'>".$stripnb."</p>";
								echo "</div>";
							echo "</div>";
						}
						echo "</div>";
					}
				} else {
					echo "<div class='buildings empty'></div>";
				}
				echo "</div>";
			}				
		} else {
			echo"empty";
		}
	}
?>