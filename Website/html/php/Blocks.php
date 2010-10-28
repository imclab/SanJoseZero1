<?php
	class Blocks {
		
		function lastBlockAdded() {
			$sql = "SELECT * FROM sj_city_block b1 WHERE b1.id >= ALL ( SELECT b2.id FROM sj_city_block b2)";
			
			$result = mysql_query( $sql );
			$row = array();
			$row[0] = mysql_fetch_assoc( $result );
			
			return $row;
		}
		
		function buildingsFromLastBlockAddedWithContent() {
			$sql = "SELECT * FROM sj_building b1 WHERE b1.block_id >= ALL ( SELECT b2.block_id FROM sj_building b2 )";

			$result = mysql_query( $sql );
			$num = mysql_num_rows( $result );
			
			$i = 0;
			while( $row = mysql_fetch_assoc( $result ) ) {
				$buildings[ $i ] = array();
				while( $i < $row['row'] ) { 
					$i++;
					$buildings[ $i ] = array();
				}
				$buildings[ $row['row'] ][] = $row;
				$i++;
			}
			
			//$buildings = $this->trimEnding( $buildings, $num-- );

			return $buildings;
		}
		
		function trimEnding( $array, $num ) {
			for( $i = $num; $i > 0; $i-- ) {
				if( count( $array[$i] ) == 0 ) {
					echo "empty <br/>";
					unset( $array[$i] );
				} else {
					break;
				}
			}
			
			return $array;
		}
		
		function buildingsFromBlockId( $bid = null ) {
			$sql = "SELECT * FROM sj_building WHERE block_id = ".$bid." ORDER BY row ASC LIMIT 0, 100";
			
			$result = mysql_query( $sql );
			$num = mysql_num_rows( $result );
			
			$i = 0;
			while( $row = mysql_fetch_assoc( $result ) ) {
				$buildings[ $i ] = array();
				while( $i < $row['row'] ) { 
					$i++;
					$buildings[ $i ] = array();
				}
				$buildings[ $row['row'] ][] = $row;
				$i++;
			}
			
			//$buildings = $this->trimEnding( $buildings, $num-- );

			return $buildings;
		}
		/*
		function buildingsFromNextBlockId( $bid ) {
			$sql = "SELECT * FROM sj_city_block WHERE id > " . $bid . " ORDER BY sj_city_block.id  ASC LIMIT 0, 1";
			echo $sql;
			$result = mysql_query( $sql );
			print_r( $result );

			$i = 0;
			while( $row = mysql_fetch_assoc( $result ) ) {
				print_r( $row );
			}

		}		
		*/
		//function getRecentBuildings( $bid, $limit, $start ) { 
		function getRecentBuildings( ) {
		
			/*
			if( $bid == "null" ) {
				$sql = "SELECT * FROM sj_city_block ORDER BY sj_city_block.id DESC LIMIT " . $start . ", " . $limit;
			} else {
				$sql = "SELECT * FROM sj_city_block WHERE sj_city_block.id < " . $bid . " ORDER BY sj_city_block.id DESC LIMIT " . $start . " , " . $limit;
			}
			*/
			$sql = "SELECT * FROM sj_city_block b1 WHERE b1.id >= ALL ( SELECT b2.id FROM sj_city_block b2)";
			
			$result = mysql_query( $sql );			
			$blocks = array();
			
			$i = 0;
			while( $row = mysql_fetch_assoc( $result ) ) {
				$block = $this->buildingsFromBlockId( $row['id'] );
				$blocks[$row['id']]=$block;
				$i++;
			}
			
			return $blocks[0];
			
		}		
	}
?>
