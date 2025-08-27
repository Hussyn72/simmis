<?
date_default_timezone_set("Asia/Calcutta");

file_put_contents('/tmp/sending_pinno.log', date("Y-m-d H:i:s") . '*********************' . 'Inside Sending PinNo Script Log'.'********************** '.  PHP_EOL, FILE_APPEND);

//DB CONNECTION
global $DBConn;
$DBConn = pg_connect("dbname='e2fax' user='domains'");

if(!$DBConn)
{
        echo "Error : Unable to open database\n";
        file_put_contents('/tmp/sending_pinno.log', date("Y-m-d H:i:s") . ' Error : Unable to open database'.  PHP_EOL, FILE_APPEND);
}
else
{
        //  echo "Opened database successfully\n";
        file_put_contents('/tmp/sending_pinno.log', date("Y-m-d H:i:s") . ' DB Connection Established'.  PHP_EOL, FILE_APPEND);
}

$Query="";














?>
