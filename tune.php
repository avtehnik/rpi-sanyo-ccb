<?php
/*
 simple example how to send data to fm_control server 
*/
	
if(isset($_POST['f'])){
	$fp = stream_socket_client("udp://192.168.1.5:1093", $errno, $errstr, STREAM_SERVER_BIND);
	if (!$fp) {
		die("$errstr ($errno)");
	}
	list($hi , $lo) =  explode(".",$_POST['f']);
	$packet = chr($hi).chr($lo);
	fwrite($fp,$packet );
	exit;
}


$chanels = array(
array('91.4', 'Рось FM', 'News/Talk'),
array('100.2', 'Super Radio', 'Hot AC'),
array('100.6', 'Авторадио Украина', 'AC'),
array('101.0', '101 Dalmatin', 'AC'),
array('101.6', 'Europa Plus Украина', 'CHR'),
array('102.4', 'Radio ROKS', 'Rock'),
array('102.9', 'Наше Радио', 'AC'),
array('103.3', 'Эра FM', 'News/Talk'),
array('103.7', 'Русское Радио Украина', 'Russian Top 40'),
array('104.1', 'Хит FM	Hot', 'AC/AC'),
array('104.5', 'Радио Дача', 'AC'),
array('105.0', 'Kiss FM', 'Dance'),
array('106.1', 'Люкс FM', 'Hot AC'),
array('107.1', 'Радио Шарманка', 'CHR/Top 40'),
array('107.5', 'Радио Шансон', 'Шансон')
);

?><!DOCTYPE html>
<html style="height: 100%; padding-left: 3px;"  >
    <head>
        <meta http-equiv="content-type" content="text/html; charset=utf-8"/>
        <title>raspberry pi + lc72131 fm tuner </title>
		<!-- change  location of mootools  to your -->
        <script src="mootools-core.js" type="text/javascript"></script>
        <script type="text/javascript">
            
        </script>
    </head>
    <body style="height: 100%;margin: 0;padding: 0;"  >
		<script type="text/javascript">

			window.addEvent('domready', function() {
				var buttons = document.getElements('button');
				buttons.addEvent('click',function(e){
					new Request.JSON({
						url: 'tune.php'
					}).post('f='+e.target.get('data-f'));
				})
			});
		</script>
		<?php
		foreach($chanels as $channel){
			echo '<div><button data-f="'.$channel['0'].'" >'.$channel['0'].'</button>'.$channel['1'].' '.$chanel['2'].'  </div>';
		}
		?>
    </body>
</html>