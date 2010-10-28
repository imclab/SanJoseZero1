<?php
	$browser = strpos($_SERVER['HTTP_USER_AGENT'],"iPhone");
	if ($browser == true)  {
?>
<html>
	<head>
		<meta http-equiv="content-type" content="text/html; charset=utf-8" />	
		<title>Plug-in-Play</title>

		<link rel="stylesheet" type="text/css" href="css/reset.css" />		
		<link rel="stylesheet" type="text/css" href="css/main.css" />
		<link rel="stylesheet" type="text/css" href="css/feedphone.css" />
		<link rel="stylesheet" type="text/css" href="css/block.css" />		
		<link rel="stylesheet" type="text/css" href="css/logphone.css" />
		<script type="text/javascript" src="js/moo.js"></script>
		<script type="text/javascript" src="js/more.js"></script>		
		<script type="text/javascript" src="js/feedphone.js"></script>		
	</head>
	
	<body>
		<div id="wrapper">


			<div id="screen-wrapper">	
			</div>
			
			<div id="blocks-wrapper">
			</div>
			
			<div id="log-wrapper">
				<div id="log">
				</div>
			</div>


		</div>
	</body>
</html>
<?
	} else {
?>
<html>
	<head>
		<meta http-equiv="content-type" content="text/html; charset=utf-8" />	
		<title>Plug-in-Play</title>

		<link rel="stylesheet" type="text/css" href="css/reset.css" />		
		<link rel="stylesheet" type="text/css" href="css/main.css" />
		<link rel="stylesheet" type="text/css" href="css/feed.css" />
		<link rel="stylesheet" type="text/css" href="css/block.css" />		
		<link rel="stylesheet" type="text/css" href="css/log.css" />
		<script type="text/javascript" src="js/moo.js"></script>
		<script type="text/javascript" src="js/more.js"></script>		
		<script type="text/javascript" src="js/main.js"></script>
		<script type="text/javascript" src="js/feed.js"></script>		
	</head>
	
	<body>
		<div id="wrapper">

			<div id="home" class="cols">
				<div class="left">
					<div id="pip-logo"><a href="index.php"><img src="img/common/pip_logo.png" /></a></div>
					
					<div id="nav-wrapper">
						<div id="nav">
							<div class="item" >
								<a href="index.php" id="home"></a>
							</div>
							<div class="item" >
								<a href="info.php" id="info"></a>
							</div>
							<div class="item" >
								<a href="part.php" id="part"></a>
							</div>
							<div class="item" >
								<a href="doc.php" id="doc"></a>
							</div>
							<div class="item" >
								<a href="#" id="feed"></a>
							</div>					
						</div>
					</div>
				
					<div id="thing"><img src="img/common/thing.png" /></div>
					<div id="rl-logo"><a href="http://lab.rockwellgroup.com/"><img src="img/common/rl_logo.png" /></a></div>
				</div>
				
				<div class="right">
					<img src="img/common/pip_header.png" />
					
					<div id="feed-nav">
						<img src="img/feed/live_archive.png" />
					</div>

					<div id="screen-wrapper">	
					</div>
					
					<div id="blocks-wrapper">
					</div>
					
					<div id="log-wrapper">
						<div id="log">
						</div>
					</div>

				</div>
			</div>
			<div class="push"></div>
		</div>
		<div id="footer"><img src="img/common/footer.png" /></div>
	</body>
</html>
<?
	}
?>