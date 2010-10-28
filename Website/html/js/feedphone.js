var Main = new Class({

	load: function() {
		
		var latestId = "1284661911";
		
		function getManyBlocks( bid, limit, start ){
			var req = new Request({
				url: '../php/getManyBuildings.php',
				data: "bid="+bid+"&limit="+limit+"&start="+start,
				onSuccess: function(txt){
					onReqSuccess(txt);
				},
				onFailure: function(xhr){
					onReqSuccess(xhr);
				}
			});
			req.send();
		}
		
		function getBlock( bid, next ){
			var req = new Request({
				//url: '../php/getTopBuildings.php',
				url: '../php/getBuildings.php',
				data: "bid="+bid+"&next="+next,
				onSuccess: function(txt){ onReqSuccess(txt); },
				onFailure: function(xhr){ onReqSuccess(xhr); }
			});
			req.send();
		}

		
		//&*!
		
		function onReqManySuccess( txt ) {
			$('blocks-wrapper').set( 'html', txt  );			
		}
		
		function onReqSuccess(txt) {
			/*
			if( txt == "empty" ) {
				alert( "empty" );
				//getBlock( latestId, 1 );
			} else {
			*/
				$('screen-wrapper').set( 'html', txt  );
				
				var screenWidth = 976;
				var screenHeight = 421;
				var buildingWidth = 25;
				var vBlockSpace = 9;
				var topBuffer = 11;
				var numBuildings = $$('.emitters').length;
				var numBlocks = $$('.emitter').length;
				
				var buildingsLookUp = new Array( $$('.emitter').length );
				
				function add( el) {
					el.set( 'class', 'item' );
					el.inject( $('log'), "top");
					var mySlide = new Fx.Slide(el).hide();
					mySlide.slideIn();
				}
				
				function finished() {
					var growDiv = new Fx.Tween( $('log'), {duration: 600, transition: Fx.Transitions.Quad.easeOut} );
					growDiv.addEvent( 'onComplete', function() {
						$('log').set( 'html', '' );
						$('log').setStyle( 'opacity', 1);
						getBlock( latestId, 0 );
					});
					growDiv.start( 'opacity', 0 );
				}
				
				function done( ) {
					$$('.emitter').each( function( el, index ) {
						var growDiv = new Fx.Tween( el, {duration: 600, transition: Fx.Transitions.Quad.easeOut} );
						if( numBlocks-1 == index ) {
							growDiv.addEvent( 'onComplete', function() {
								finished();
							});
						}
						growDiv.start( 'bottom', screenHeight );
					});
					
					$('log').tween( 'opacity', 0 );
					
					//$('log').setStyle( 'opacity', 1 );
					//getBlock( "1281564675" );
				}
				
				function tweenItem( el, index, check ) {
					var moveDiv = new Fx.Tween( el, {duration: 600, transition: Fx.Transitions.Quad.easeOut} );
					moveDiv.addEvent( 'onStart', function() {
						var logVersion = el.clone();
						add( logVersion );
					});
					
					if( numBlocks-1 == check ) {
						moveDiv.addEvent( 'onComplete', function() {
							done( check);
						});
					}
		
					moveDiv.start( 'bottom', ( screenHeight - buildingWidth-(25*index)-vBlockSpace*index)-topBuffer );		
				}
				
				var count = 0;
				$$('.emitters').each( function( els, elsIndex ) {
					
					$(els).setStyle( 'left', ( ( screenWidth / numBuildings ) * elsIndex ) + ( ( screenWidth / numBuildings ) / 2 ) - ( buildingWidth/2 ) );
					
					$(els).getElements( '.emitter' ).each( function( el, elIndex ) {
						buildingsLookUp[count] = elIndex;
						count++;
						el.setStyle( 'bottom', (-1)*( elIndex*25 ) );
					});
					
				});
		
				$$('.emitter').each( function( el, index ) {
					(function() {(function(){
						tweenItem( el, buildingsLookUp[ index ], index );
					}).delay(300*index) }).delay(300);
				});
				
			//}
			
		}
		
		//getManyBlocks( "1283347942", "30", "0" );
		//getManyBlocks( "1283347962", "30", "0" );
		getBlock( latestId, "0" ); // lots of items
		//getBlock( "1283347890" ); // one item
		
		/*
		
  	Edit   	Delete  	1284132505 	2010-09-10
	Edit 	Delete 	1284132500 	2010-09-10
	Edit 	Delete 	1284132495 	2010-09-10
	
SELECT * FROM `sj_city_block` WHERE id > 1284132495 ORDER BY `sj_city_block`.`id`  ASC LIMIT 0, 1	
		*/
		
	}
	
});

var main = new Main();

window.addEvent( 'domready', main.load.bind( main ) );