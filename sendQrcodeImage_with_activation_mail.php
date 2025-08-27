<?php
date_default_timezone_set("Asia/Calcutta");
global $DBConn,$logfile;
$logfile = "/tmp/sendQrcodeImage_with_activation_mail.log";

file_put_contents($logfile,date('Y-m-d H:i:s')."****** START ********".PHP_EOL,FILE_APPEND);

$DBConn = pg_connect("dbname='e2fax' user='domains'");
if($DBConn)
{
	echo "Database Connection has established";
	file_put_contents($logfile,date('Y-m-d H:i:s')."Database Connection has established".PHP_EOL,FILE_APPEND);
}
else
{
	echo "Unable to Open Database";
	file_put_contents($logfile,date('Y-m-d H:i:s')."Unable to Open Database".PHP_EOL,FILE_APPEND);
}































?>

