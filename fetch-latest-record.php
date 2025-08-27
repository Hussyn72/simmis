<?php
date_default_timezone_set("Asia/Calcutta");
$log_file = '/tmp/fetchlatestrecord.log';

//DB CONNECTION
global $DBConn;
$DBConn = pg_connect("dbname='e2fax' user='domains'");

if(!$DBConn)
{
        echo "Error : Unable to open database\n";
        file_put_contents($log_file, date("Y-m-d H:i:s") . ' Error : Unable to open database'.  PHP_EOL, FILE_APPEND);
}
else
{
        //  echo "Opened database successfully\n";
        file_put_contents($log_file, date("Y-m-d H:i:s") . ' DB Connection Established'.  PHP_EOL, FILE_APPEND);
}

file_put_contents($log_file, date("Y-m-d H:i:s") . '*********************' . 'START'.'********************** '.  PHP_EOL, FILE_APPEND);

if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    // Check if the 'rowId' parameter is set in the POST data
        if (isset($_POST['rowId']))
        {
                // Retrieve the 'rowId' value from the POST data
                $rowId = $_POST['rowId'];
                $order_no = $_POST['orderNumber'];
                $simno = $_POST['simNumber'];
		$tripid = $_POST['tripid'];
                file_put_contents($log_file, date("Y-m-d H:i:s") . $rowId .  PHP_EOL, FILE_APPEND);
                file_put_contents($log_file, date("Y-m-d H:i:s") . $order_no .  PHP_EOL, FILE_APPEND);
                file_put_contents($log_file, date("Y-m-d H:i:s") . $simno .  PHP_EOL, FILE_APPEND);

                if (empty($order_no)) {
                    $condition = "c.order_no IS NULL";
                } else {
                    $condition = "c.order_no = '$order_no'";
                }

                ////$Query = "select c.order_no,c.from_date,c.tripid,s.country,c.booking_date,c.activated,s.pinno,s.puk1,c.status,c.sim_phone_no,coalesce(c.handset_model,c.handset_model,'-') as handset_model, s.simno, s.vendor,COALESCE(c.clientname,u.clientname) as clientname,COALESCE(c.emailadd,u.emailadd) as emailadd,COALESCE(p.type,c.payment_mode) as type,t.no_of_documents_uploaded,COALESCE(c.comment || ' Zip Code:- '  || c.add_pinno,c.comment) as comment,t.documents_uploaded,s.qrtext,coalesce(vd.activation_scriptname,'0') as activation_scriptname from clienttrip as c JOIN sim_stock as s ON c.sim_phone_no = s.sim_phone_no LEFT JOIN sim_user u on c.username = u.username LEFT JOIN payment_discount as p ON c.tripid = p.trip_id AND p.type Not in('discounts') LEFT JOIN tsim_order_documents t ON c.order_no=t.order_no LEFT JOIN countrydata cd ON cd.countryname=s.country LEFT JOIN ta_vendors vd on s.vendor=vd.vendor where $condition AND s.simno='$simno' AND s.activation_reqd=true AND c.status not in('Canceled','cardlost')  order by c.activated,c.from_date asc,booking_date;";
                #$Query = "select c.order_no,c.from_date,c.tripid,s.country,c.booking_date,c.activated,s.pinno,s.puk1,c.status,c.sim_phone_no,coalesce(c.handset_model,c.handset_model,'-') as handset_model, s.simno, s.vendor,COALESCE(c.clientname,u.clientname) as clientname,COALESCE(c.emailadd,u.emailadd) as emailadd,COALESCE(p.type,c.payment_mode) as type,t.no_of_documents_uploaded,COALESCE(c.comment || ' Zip Code:- '  || c.add_pinno,c.comment) as comment,t.documents_uploaded,s.qrtext,coalesce(vd.activation_scriptname,'0') as activation_scriptname from clienttrip as c JOIN sim_stock as s ON c.sim_phone_no = s.sim_phone_no LEFT JOIN sim_user u on c.username = u.username LEFT JOIN payment_discount as p ON c.tripid = p.trip_id AND p.type Not in('discounts') LEFT JOIN tsim_order_documents t ON c.order_no=t.order_no LEFT JOIN countrydata cd ON cd.countryname=s.country LEFT JOIN ta_vendors vd on s.vendor=vd.vendor where $condition AND c.tripid='$tripid' AND s.activation_reqd=true AND c.status not in('Canceled','cardlost')  order by c.activated,c.from_date asc,booking_date;";
                #$Query = " select X.*,coalesce(pl.vendor_sku,X.sku) as vendor_sku,coalesce(coalesce(vd.activation_scriptname,vd1.activation_scriptname),'0') as activation_scriptname from (select c.order_no,c.from_date,c.tripid,s.country,c.booking_date,c.activated,s.pinno,s.puk1,c.status,c.sim_phone_no,coalesce(c.handset_model,c.handset_model,'-') as handset_model, s.simno, s.vendor,COALESCE(c.clientname,u.clientname) as clientname,COALESCE(c.emailadd,u.emailadd) as emailadd,COALESCE(p.type,c.payment_mode) as type,t.no_of_documents_uploaded,COALESCE(c.comment || ' Zip Code:- '  || c.add_pinno,c.comment) as comment,t.documents_uploaded,s.qrtext,sim_country_sku(s.country) as sku from clienttrip as c JOIN sim_stock as s ON c.sim_phone_no = s.sim_phone_no LEFT JOIN sim_user u on c.username = u.username LEFT JOIN payment_discount as p ON c.tripid = p.trip_id AND p.type Not in('discounts') LEFT JOIN tsim_order_documents t ON c.order_no=t.order_no LEFT JOIN countrydata cd ON cd.countryname=s.country where $condition AND c.tripid='$tripid' AND s.activation_reqd=true AND c.status not in('Canceled','cardlost')) X left join tsim_plans_v3 pl on pl.planid=X.sku left join ta_vendors vd on vd.vendor=pl.vendor left join ta_vendors vd1 on vd1.vendor=X.vendor";
		$Query = "select Y.*, coalesce(vd.activation_scriptname,'0') as activation_scriptname from (select X.order_no,X.from_date,X.tripid,X.country,X.booking_date,X.activated,X.pinno,X.puk1,X.status,X.sim_phone_no,X.handset_model,X.simno,X.clientname,X.emailadd,X.type,X.no_of_documents_uploaded,X.comment,X.documents_uploaded,X.qrtext,X.sku,coalesce(coalesce(X.vendor,X.vendor1),X.vendor2) as vendor from (select c.order_no,c.from_date,c.tripid,s.country,c.booking_date,c.activated,s.pinno,s.puk1,c.status,c.sim_phone_no,coalesce(c.handset_model,c.handset_model,'-') as handset_model, s.simno,COALESCE(c.clientname,u.clientname) as clientname,COALESCE(c.emailadd,u.emailadd) as emailadd,COALESCE(p.type,c.payment_mode) as type,t.no_of_documents_uploaded,COALESCE(c.comment || ' Zip Code:- '  || c.add_pinno,c.comment) as comment,t.documents_uploaded,s.qrtext,c.tripid_min as sku,pl.vendor, (select vendor from tsim_plans_v3_all where lower(planid) in (select lower(sku) from sim_sku_country_map where country = s.country) order by cost limit 1) as vendor1,(select vendor from tsim_plans_v3_all where planid ilike (select replace(replace(planid,'+-36','-36'),'-','%') from tsim_plans_v3_all where vendor_sku ilike replace(REGEXP_REPLACE(REGEXP_REPLACE(REGEXP_REPLACE(REGEXP_REPLACE(replace(c.tripid_min,'-eesim','-esim'), '-[1-9]*$' , '', 'g'),'45400|45407|45431',''),'-eg*$','-','g'),'-ar-esim','-','g'),'-','%')) order by cost limit 1) vendor2  from clienttrip as c left join sim_stock as s on c.sim_phone_no = s.sim_phone_no LEFT JOIN sim_user u on c.username = u.username LEFT JOIN payment_discount as p ON c.tripid = p.trip_id AND p.type Not in('discounts') LEFT JOIN tsim_order_documents t ON c.order_no=t.order_no LEFT JOIN countrydata cd ON cd.countryname=s.country left join tsim_plans_v3 pl on pl.planid=c.tripid_min where $condition AND c.tripid='$tripid'  AND s.activation_reqd=true AND c.status not in('Canceled','cardlost')) X ) Y left join ta_vendors vd on vd.vendor=Y.vendor";
                file_put_contents($log_file, date("Y-m-d H:i:s") . $Query .  PHP_EOL, FILE_APPEND);
                $ExecQuery = pg_query($DBConn, $Query);
                if($ExecQuery)
                {
                        file_put_contents($log_file, date("Y-m-d H:i:s") ."Query Executed Sucessfully " .  PHP_EOL, FILE_APPEND);
                        $data = pg_fetch_all($ExecQuery);
                        $json_response = json_encode($data);
                        echo $json_response;
                        return $json_response;
                }
 		else
                {
                        file_put_contents($log_file, date("Y-m-d H:i:s") . "Query Execution Failed ".  PHP_EOL, FILE_APPEND);
                }
        }
}
?>

