<?php
	require( "db.php" );
	require( "Blocks.php" );

	$b = new Blocks();

	if( $_GET['bid'] ) {
		$rows = $b->buildingsFromBlockId( $_GET['bid'] );
		
		echo "<pre>";
		print_r($rows);
		echo "</pre>";	
	
	} else if( $_POST['bid'] ) {
		$rows = $b->buildingsFromBlockId( $_POST['bid'] );

		if ( count( $rows ) > 0 ) {
			foreach( $rows as $buildings ) {
				echo "<div class='buildings'  >";
				$building = $buildings[0];
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
				echo "</div>";
			}
		} else {
			echo"empty";
		}
	}
	
?>