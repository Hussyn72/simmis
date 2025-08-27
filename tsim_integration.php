<?php
date_default_timezone_set("Asia/Kolkata");
echo "Hello Tsim";
$log_file = '/var/log/ownmail/tsim_integration.log';
require_once('check_fire.php');
$db=pg_connect("dbname=e2fax user=domains");
#$db=pg_connect("dbname=e2fax user=domains");
if (!$db) {
	die("Error in connection: " . pg_last_error());
}
file_put_contents($log_file, date("Y-m-d H:i:s") .'DB Connected' .PHP_EOL, FILE_APPEND);
/*GLOBAL $orderid;GLOBAL $items;GLOBAL $email;GLOBAL $phone;GLOBAL $action;GLOBAL $first_name;GLOBAL $last_name;GLOBAL $clientname;GLOBAL $domainname;GLOBAL $company;
GLOBAL $clientadd1;GLOBAL $clientadd2;GLOBAL $address_city;GLOBAL $address_state;GLOBAL $address_country;GLOBAL $postcode;GLOBAL $start_date;GLOBAL $payment_id;GLOBAL $status;
GLOBAL $curr;GLOBAL $total;GLOBAL $sub_total;GLOBAL $total_shipping;GLOBAL $country_specific;GLOBAL $payment_mode;GLOBAL $link_amt;GLOBAL $total_tax;GLOBAL $username;GLOBAL $subtotal;
GLOBAL $mnt;GLOBAL $gift_item;GLOBAL $note;GLOBAL $quantity;GLOBAL $sku;GLOBAL $item_cost;GLOBAL $sk;GLOBAL $country_dur_stdcode;GLOBAL $country;GLOBAL $duration;GLOBAL $std_code;
GLOBAL $max_from_date;GLOBAL $max_to_date;GLOBAL $sim_phone_no;GLOBAL $order_type;GLOBAL $country_specific;GLOBAL $action;
GLOBAL $shipping_add1;GLOBAL $shipping_add2;GLOBAL $shipping_city;GLOBAL $shipping_state;GLOBAL $shipping_country;GLOBAL $shipping_postcode;GLOBAL $flag_user;
GLOBAL $client_mail;
 */
$orderid; $items; $email; $phone; $action; $first_name; $last_name; $clientname; $domainname; $company;$role;
$clientadd1; $clientadd2; $address_city; $address_state; $address_country; $postcode; $start_date; $payment_id; $status;
$curr; $total; $sub_total; $total_shipping; $country_specific; $payment_mode; $link_amt; $total_tax; $username; $subtotal;
$mnt; $gift_item; $note; $quantity; $sku; $item_cost; $sk; $country_dur_stdcode; $country; $duration; $std_code;
$max_from_date; $max_to_date; $sim_phone_no; $order_type; $country_specific; $action;
$shipping_add1; $shipping_add2; $shipping_city; $shipping_state; $shipping_country; $shipping_postcode; $flag_user;
$client_mail;$is_kyc_required;
$headers = "From: TSIM Team <services@tsim.in>\r\n";
if(isset($_POST['submit'])=='' )
{
	file_put_contents($log_file, date("Y-m-d H:i:s") . '***********************Begin************************'. PHP_EOL, FILE_APPEND);
	$json = file_get_contents('php://input');
	$obj = json_decode($json, true);
	$arr = print_r($obj, true);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'json->'.$json . PHP_EOL, FILE_APPEND);
	#file_put_contents($log_file, date("Y-m-d H:i:s") . 'json->'.$arr . PHP_EOL, FILE_APPEND);
	$orderid        = $obj['order']['order_number'];
	$status         = $obj['order']['status'];
	$payment_id     = $obj['order']['payment_details']['method_id'];
	$delivery_method = $obj['order']['shipping_methods'];
	$is_kyc_required =  isset($obj['order']['is_kyc_required']) ? $obj['order']['is_kyc_required'] : null;
	
	$gstin1 = isset($obj['order']['customer']['gstin']) ? $obj['order']['customer']['gstin'] : null;
	$gstin1 = preg_replace("/[^A-Za-z0-9]/", "", $gstin1);


	if (strlen($gstin1) === 15) 
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin1.PHP_EOL, FILE_APPEND);
	  	$gstin=$gstin1;
	} 
	else if(strlen($gstin1) === 0)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . ' GST IN  Field is Empty-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

	}
	else
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN is INVALID-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

	}
	
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Order Number-'.$orderid.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Payment id-'.$payment_id.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Delivery Method-'.$delivery_method.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin.PHP_EOL, FILE_APPEND);
	/*************************************Test-Sending json data to CPP**************************************
	$ch = curl_init();                    // initiate curl
	$url = "https://mail.tsim.in/simmisn.e"; // where you want to post data
	curl_setopt($ch, CURLOPT_URL,$url);
	curl_setopt($ch, CURLOPT_POST, true);  // tell curl you want to post something
	curl_setopt($ch, CURLOPT_POSTFIELDS,"l=admin&p=chair&a=tsim_order&json=$json");
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); // return the output in string format
	$output = curl_exec ($ch); // execute
	curl_close ($ch); // close curl handle
	/**********************************************************************************************************/
	file_put_contents($log_file, date("Y-m-d H:i:s") . '---------Check the role of the client---------------'.PHP_EOL, FILE_APPEND);
	$role = $obj['order']['customer']['role'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Role of the client---'.$role.PHP_EOL, FILE_APPEND);

	$count_sim_sr=0;	
	$cnt_items = count($obj['order']['line_items']);
	$productname = $obj['order']['line_items'][0]['name'];
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Total items -'.$cnt_items.PHP_EOL, FILE_APPEND);
	for($l=0;$l<$cnt_items;$l++){
		$meta_arr_sr = $obj['order']['line_items'][$l]['meta'];
		$srno =  null;
		foreach($meta_arr_sr as $v)
		{
			#if($v['key']==='Serial Number - 1' || $v['key']==='Serial Number')
			if (substr($v['key'],0,13) == 'Serial Number')
			{
				$srno =  $v['value'];
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Serial no -'.$srno.PHP_EOL, FILE_APPEND);
				$count_sim_sr++;
			}

		}
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .'count of Serial no entered -'.$count_sim_sr.PHP_EOL, FILE_APPEND);
	//	if( $role == 'corporate customer' || $role == 'shop manager' || $role == 'corporate_customer' || $role == 'shop_manager')
	if($count_sim_sr > 0)		
	{
		//mail("hoshang@staff.ownmail.com","New Order $orderid from $role","New Order $orderid from $role",$headers);
		mail("services@tsim.in","New Order $orderid from $role","New Order $orderid from $role",$headers);
		//mail("aditya@staff.ownmail.com","New Order $orderid from $role","New Order $orderid from $role",$headers);
		$role = 'reseller_customer';	
		file_put_contents($log_file, date("Y-m-d H:i:s") . '---------Inside corporate customer----------'.PHP_EOL, FILE_APPEND);
		$email          = $obj['order']['customer']['billing_address']['email'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'email-'.$email.PHP_EOL, FILE_APPEND);
		$uname = explode("@",$email);
		$username = $uname[0];
		$username = strtolower($username);
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'TSIM Username-'.$username.PHP_EOL, FILE_APPEND);
		/***********************************Below earlier code which can not handle 7276933911/07183234244 case*********************/
		$phone          = $obj['order']['customer']['billing_address']['phone'];
		$phone1          = str_replace(' ','',$phone);
		if (strpos($phone1, '+') !== false)
		{
			$ph             = explode("+",$phone1);
			$phone          = $ph[1];
		}
		else if (strpos($phone1, '/') !== false)
		{
			$ph_ar =  explode("/",$phone1);
			$phone = $ph_ar[0];
		}
		else $phone = $phone1;
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'phone-'.$phone.PHP_EOL, FILE_APPEND);	
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Order Status-'.$status.PHP_EOL, FILE_APPEND);
		/********************************************************************************************************************************
		if($status == 'cancelled' || $status == 'failed')
			exit;
		if(($payment_id == 'payuindia'  || $payment_id == 'pwa' ) && $status == 'pending' )
		{
			exit;
		}
		if($status == 'cancelled' || $status == 'failed')
			exit;
		if(($payment_id == 'payuindia'  || $payment_id == 'pwa' ) && $status == 'pending' )
		{
			exit;
		}
	/**************************************Inserting Order json in DB****************************************/
	$chk_order = "select order_no from tsim_orders where order_no='$orderid'";
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'chk_order-'.$chk_order.PHP_EOL, FILE_APPEND);
	$res_chk_order = pg_query($db, $chk_order);
	$num_chk_order = pg_num_rows($res_chk_order);
	if($num_chk_order == 0)
	{
		$ins_json = "insert into tsim_orders (order_no, json, status, added_on) values ('$orderid','$json','$status', now())";
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'ins_json-'.$ins_json.PHP_EOL, FILE_APPEND);
		$res_ins_json = pg_query($db, $ins_json);
		if(!$res_ins_json)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Failed to Insert orginal Json---Try For Modified Json'.PHP_EOL, FILE_APPEND);
			$changed_json = str_replace("'","''",$json);
			file_put_contents($log_file, date("Y-m-d H:i:s") . $changed_json.PHP_EOL, FILE_APPEND);
			$ins_changed_json = "insert into tsim_orders (order_no, json, status, added_on) values ('$orderid','$changed_json','$status', now())";
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'ins_json-'.$ins_changed_json.PHP_EOL, FILE_APPEND);
			$res_ins_c_json = pg_query($db, $ins_changed_json);
			if(!$res_ins_c_json)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Failed to Insert altered json---Manual Booking'.PHP_EOL, FILE_APPEND);
				$delete1 = deleteDuplicateEntry($db, $orderid, $log_file); 
				mail("hoshang@staff.ownmail.com","Tsim Order Booking Failed- $orderid","Failed to insert into tsim_orders table due to quoted json data \n$ins_changed_json",$headers);
				mail("aditya@staff.ownmail.com","Tsim Order Booking Failed- $orderid","Failed to insert into tsim_orders table due to quoted json data \n$ins_changed_json",$headers);
				mail("binu@staff.ownmail.com","Tsim Order Booking Failed- $orderid","Failed to insert into tsim_orders table due to quoted json data \n$ins_changed_json",$headers);
				mail("services@tsim.in","Tsim Order Booking Failed- $orderid","Order booking Failed. Contact RnD",$headers);
				exit;
			}
		}
	}
	else
	{
		$update_json = "update tsim_orders set json='$json',status='$status' where order_no='$orderid'";
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Update_json-'.$update_json .PHP_EOL, FILE_APPEND);
		$res_update_json = pg_query($db, $update_json);
	}
	/*****************************************Updating the simmismap order status*********************************/
	$sql = "select * from tsim_simmis_map where order_no = '$orderid'";
	file_put_contents($log_file, date("Y-m-d H:i:s") .$sql.PHP_EOL, FILE_APPEND);
	$res_sql = pg_query($db, $sql);
	$res_num_rows = pg_num_rows($res_sql);
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Records->'.$res_num_rows.'->Exit.'.PHP_EOL, FILE_APPEND);
	if($res_num_rows > 0)
	{
		//$ttttt = pg_fetch_assoc($res_sql);
		$sql_status_update = "UPDATE tsim_simmis_map set status='".$status."' where order_no='".$orderid."'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Update record status->'.$sql_status_update.PHP_EOL, FILE_APPEND);
		$res_sql_status_update = pg_query($sql_status_update);
		exit;
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Inserting into tsim_simmis_map'.PHP_EOL, FILE_APPEND);
	$insert = "insert into tsim_simmis_map (tsim_username,email,contact,order_no,status)values('$username','$email','$phone','$orderid','$status')";
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Insert Record->'.$insert.PHP_EOL, FILE_APPEND);
	$res_insert = pg_query($db, $insert);
	if($res_insert)
		echo "ok";
	else
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Failed to insert into tsim_simmis_map'.PHP_EOL, FILE_APPEND);
		exit;
	}
	$action = 'register';
	/************************************************************************************************************/
	file_put_contents($log_file, date("Y-m-d H:i:s") . '*******************Checking for Order New or Not*************'.PHP_EOL, FILE_APPEND);
//Checking for duplicate entry in tsim_order_deduplication
        $check1 = checkDuplicateEntry($db, $orderid ,$log_file);
        if($check1)
		{
                //Delete entry from deduplication also if you want to proceed with booking 
                file_put_contents($log_file, date("Y-m-d H:i:s") . '*******************Code Exit due to duplicate entry*************'.PHP_EOL, FILE_APPEND);
                exit();
                }
        else
        	{
                $check2 = createDuplicateEntry($db, $orderid ,$log_file);
        	}

	$items = count($obj['order']['line_items']);
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Total items -'.$items.PHP_EOL, FILE_APPEND);
	$line_items = count($obj['order']['line_items']);
	file_put_contents($log_file, date("Y-m-d H:i:s") .'line_items-'.$line_items.PHP_EOL, FILE_APPEND);
	for($i=0; $i<$line_items;$i++)
	{
		if($obj['order']['line_items']["$i"]['meta']['0']['value'] == 'Free Item')
		{
			$gift_item = $obj['order']['line_items']["$i"]['name'];
			$items = $items - 1;
			break;
		}
		else
			$gift_item="TSIM Booking";
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .$gift_item.PHP_EOL, FILE_APPEND);
	for( $j=0; $j<$items;$j++)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'j -'.$j.PHP_EOL, FILE_APPEND);
		$quant       = $obj['order']['line_items']["$j"]['quantity'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Quantity-'.$quant.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s").'j -'.$j.PHP_EOL, FILE_APPEND);
		for($g=0; $g<$quant;$g++)
		{
			for($c=0 ; $c<5 ;$c++)
			{
				if($obj['order']['line_items']["$j"]['meta']["$c"]['key'] == 'new-card-or-refill')
				{
					$order_type = $obj['order']['line_items']["$j"]['meta']["$c"]['value'];
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Order-'.$order_type .PHP_EOL, FILE_APPEND);
					if($order_type == 'Refill')
					{
						$flag = 1;
						$sku            = $obj['order']['line_items']["$j"]['sku'];
						file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU-'.$sku.PHP_EOL, FILE_APPEND);
						//goto refill_order;
						$ret = refill_order($orderid);
					}
					else
					{
						$flag = 0;
						break 3;
					}
				}
			}
		}
	}
	unset($items);
	unset($line_items);
	unset($i);
	unset($quant);
	unset($g);
	unset($c);
	unset($j);
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Order type is-'.$order_type .PHP_EOL, FILE_APPEND);
	if($order_type == 'Refill')
		exit;
	/*********************************Checking for the multiple Userid for the this mail or Mobile No************/
	if(($phone == '' && $phone== NULL) || ($email =='' && $email==NULL))
	{
		exit;
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Checking mis sim username existance with either email or mobile no' .PHP_EOL, FILE_APPEND);
	$select  = "select username, emailadd, mobno from sim_user where emailadd = '$email' or mobno like '%$phone' and domainname='tsim.in'";
	file_put_contents($log_file, date("Y-m-d H:i:s") .$select .PHP_EOL, FILE_APPEND);
	$result    = pg_query($db, $select);
	$num_rows       = pg_num_rows($result);
	file_put_contents($log_file, date("Y-m-d H:i:s") .'NUM ROWS-'.$num_rows .PHP_EOL, FILE_APPEND);
	if($num_rows == 1)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Single user with this mail or contact found.'.PHP_EOL, FILE_APPEND);
		$username = $GLOBALS['username'];
		$rows = pg_fetch_assoc($result);
		$username = $rows['username'];
		$pg_update = "update tsim_simmis_map set tsim_username = '$username' where order_no = '".$orderid."'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'db_update->'.$pg_update.PHP_EOL, FILE_APPEND);
		$res_pg_update = pg_query($db, $pg_update);
		$action = 'user_exists';
	}
	else if($num_rows>1)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Multiple username with this email or mobile no found.' .PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Sending link For User Validation' .PHP_EOL, FILE_APPEND);
		$message = "
			<html>
			<head>
			<title>TSIM</title>
			</head>
			<body>
			<p><a href='https://mail.tsim.in/validate_user.php?o=$orderid&e=$email&m=$phone&r=$role'>Click Here To Validate</a></p>
			</body>
			</html>
			";
		$headers  = 'MIME-Version: 1.0' . "\r\n";
		$headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
		$headers .= 'From: TSIM Team <services@tsim.in>' . "\r\n";
		//$link = "https://mail.tsim.in/validate_user.php?o=$orderid&e=$email&m=$phone";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'link-'.$message.PHP_EOL, FILE_APPEND);
		mail("hoshang@staff.ownmail.com","Validate User for Order no $orderid",$message,$headers);
		mail("husain@staff.ownmail.com","Validate User for Order no $orderid",$message,$headers);
		mail("binu@staff.ownmail.com","Validate User for Order no $orderid",$message,$headers);
		mail("services@tsim.in","Validate User for Order no $orderid","$message",$headers);
		exit;
	}
	else
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") .'No mis user found for this mail or contact no.'.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'created TSIM username is-'.$username.PHP_EOL, FILE_APPEND);
		$user_len = strlen($username);
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'User length is-'.$user_len.PHP_EOL, FILE_APPEND);
		if($user_len <= 8) 
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") . '*******calling create_user function*****************'.PHP_EOL, FILE_APPEND);
			$username = create_user($username, $phone);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'New Username-'.$username.PHP_EOL, FILE_APPEND);
		}
		else
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Check whether the username-'.$username.'-with other domains exists?'.PHP_EOL, FILE_APPEND);
			$chk_user   = "SELECT count(username) from sim_user where primary_emailadd like '$username%' and username not like '1555%'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$chk_user.PHP_EOL, FILE_APPEND);
			$res_chk_user = pg_query($db, $chk_user);
			$rec_user  = pg_fetch_row($res_chk_user);
			$num_user = $rec_user['0'];
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Total user present-'.$num_user .PHP_EOL, FILE_APPEND);
			if($num_user > 0)
			{
				$cnt = $num_user+5;
				file_put_contents($log_file, date("Y-m-d H:i:s") .'count is -'.$cnt .PHP_EOL, FILE_APPEND);
				//for($d=0;$d<$cnt;$d++) Not Working Properly.. Need to think again
				for($d=0;$d<$cnt;$d++)
				{
					$numbers = preg_replace('/[^0-9]/', '', $username);
					$uname = preg_replace('/[^a-zA-Z]/', '', $username);
					file_put_contents($log_file, date("Y-m-d H:i:s") .$numbers.PHP_EOL, FILE_APPEND);
					file_put_contents($log_file, date("Y-m-d H:i:s") .$uname.PHP_EOL, FILE_APPEND);
					$numbers++;
					file_put_contents($log_file, date("Y-m-d H:i:s") .$numbers.PHP_EOL, FILE_APPEND);
					$username = $uname.$numbers;
					file_put_contents($log_file, date("Y-m-d H:i:s") .'New username-'.$username.PHP_EOL, FILE_APPEND);
					$chk_new_user ="select username from sim_user where username = '$username'";
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Chk_new_user-'.$chk_new_user .PHP_EOL, FILE_APPEND);
					$res_chk_new_user = pg_query($db,$chk_new_user);
					if(!$res_chk_new_user)
						file_put_contents($log_file, date("Y-m-d H:i:s") .'Not executes'.PHP_EOL, FILE_APPEND);
					$num_chk_new_user = pg_num_rows($res_chk_new_user);
					file_put_contents($log_file, date("Y-m-d H:i:s") .$num_chk_new_user .PHP_EOL, FILE_APPEND);
					if($num_chk_new_user > 0)
					{
						$flag_user = 0;
						continue;
					}
					else
					{
						$flag_user = 1;
						break;
					}
				}
				if($flag_user == 1)
				{
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Username for this order is-'.$username.PHP_EOL, FILE_APPEND);
				}
				else
				{
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Can not create new user-Exit'.PHP_EOL, FILE_APPEND);
					exit;
				}
			}
			else
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Username for this order is-'.$username.PHP_EOL, FILE_APPEND);
		}
		$chk_user_domains = "select * from users where username='$username'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$chk_user_domains.PHP_EOL, FILE_APPEND);
		$res_chk_user_domains = pg_query($db, $chk_user_domains);
		if(!$res_chk_user_domains)
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Not Executed'.PHP_EOL, FILE_APPEND);
		$nn = pg_num_rows($res_chk_user_domains);
		if($nn > 0)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'*******Username exist with other domainame.Create New User********************'.PHP_EOL, FILE_APPEND);
			$username = create_user($username, $phone);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'New Username-'.$username.PHP_EOL, FILE_APPEND);
		}
		$action = 'register';
	}
	}
	//else if( $role == 'customer' || $role =='' || $role == 'subscriber')
	else
	{
		$role = 'customer';
		file_put_contents($log_file, date("Y-m-d H:i:s") . '---------Inside customer----------'.PHP_EOL, FILE_APPEND);
		$email          = $obj['order']['customer']['email'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'email-'.$email.PHP_EOL, FILE_APPEND);
		$daykey = $obj['order']['line_items']['0']['meta']['0']['key'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Day key-'.$daykey.PHP_EOL, FILE_APPEND);
		if(strpos($daykey, 'Additional Days in trip') !== false){
			$dayvalue = $obj['order']['line_items']['0']['meta']['0']['value'];
		}
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Day Value-'.$dayvalue.PHP_EOL, FILE_APPEND);
		$username = $obj['order']['customer']['username'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'TSIM Username-'.$username.PHP_EOL, FILE_APPEND);
		$username_present_in_json=0;
		if(empty($username))
		{
			$uname = explode("@",$email);
			$username = $uname[0];
		}
		else
		{
			$username_present_in_json=1;
			if(strpos($username,'@') !== false)
			{
				$un = explode("@",$username);
				$username = $un[0];
			}
			else
				$username = $obj['order']['customer']['username'];
		}
		$username = strtolower($username);
		/**************************Check username is single digit or not********************************/
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'TSIM Username-'.$username.PHP_EOL, FILE_APPEND);
		/***********************************Below earlier code which can not handle 7276933911/07183234244 case*********************/
		$phone          = $obj['order']['customer']['billing_address']['phone'];
		$phone1          = str_replace(' ','',$phone);
		if (strpos($phone1, '+') !== false)
		{
			$ph             = explode("+",$phone1);
			$phone          = $ph[1];
		}
		else if (strpos($phone1, '/') !== false)
		{
			$ph_ar =  explode("/",$phone1);
			$phone = $ph_ar[0];
		}
		else $phone = $phone1;
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'phone-'.$phone.PHP_EOL, FILE_APPEND);
		/********************************************************************************************************************************/	
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Order Status-'.$status.PHP_EOL, FILE_APPEND);
		if($status == 'cancelled' || $status == 'failed')
			exit;
		if(($payment_id == 'payuindia'  || $payment_id == 'pwa' ) && $status == 'pending' )
		{
			exit;
		}
		/**************************************Inserting Order json in DB****************************************/
		$chk_order = "select order_no from tsim_orders where order_no='$orderid'";
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'chk_order-'.$chk_order.PHP_EOL, FILE_APPEND);
		$res_chk_order = pg_query($db, $chk_order);
		$num_chk_order = pg_num_rows($res_chk_order);
		if($num_chk_order == 0)
		{
			$ins_json = "insert into tsim_orders (order_no, json, status, added_on) values ('$orderid','$json','$status', now())";
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'ins_json-'.$ins_json.PHP_EOL, FILE_APPEND);
			$res_ins_json = pg_query($db, $ins_json);
			if(!$res_ins_json)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Failed to Insert orginal Json---Try For Modified Json'.PHP_EOL, FILE_APPEND);
				$changed_json = str_replace("'","''",$json);
				file_put_contents($log_file, date("Y-m-d H:i:s") . $changed_json.PHP_EOL, FILE_APPEND);
				$ins_changed_json = "insert into tsim_orders (order_no, json, status, added_on) values ('$orderid','$changed_json','$status', now())";
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'ins_json-'.$ins_changed_json.PHP_EOL, FILE_APPEND);
				$res_ins_c_json = pg_query($db, $ins_changed_json);
				if(!$res_ins_c_json)
				{
					file_put_contents($log_file, date("Y-m-d H:i:s") . 'Failed to Insert altered json---Manual Booking'.PHP_EOL, FILE_APPEND);
					$delete2 = deleteDuplicateEntry($db, $orderid, $log_file); 
					mail("hoshang@staff.ownmail.com","Tsim Order Booking Failed- $orderid","Failed to insert into tsim_orders table due to quoted json data \n$ins_changed_json",$headers);
					mail("aditya@staff.ownmail.com","Tsim Order Booking Failed- $orderid","Failed to insert into tsim_orders table due to quoted json data \n$ins_changed_json",$headers);
					mail("binu@staff.ownmail.com","Tsim Order Booking Failed- $orderid","Failed to insert into tsim_orders table due to quoted json data \n$ins_changed_json",$headers);
					mail("services@tsim.in","Tsim Order Booking Failed- $orderid","Order booking Failed. Contact RnD",$headers);
					exit;
				}
			}
		}
		else
		{
			$update_json = "update tsim_orders set json='$json',status='$status' where order_no='$orderid'";
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Update_json-'.$update_json .PHP_EOL, FILE_APPEND);
			$res_update_json = pg_query($db, $update_json);
		}	
		/*******************************************************************************************************/
		$sql = "select * from tsim_simmis_map where order_no = '$orderid'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$sql.PHP_EOL, FILE_APPEND);
		$res_sql = pg_query($db, $sql);
		$res_num_rows = pg_num_rows($res_sql);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Records->'.$res_num_rows.'->Exit.'.PHP_EOL, FILE_APPEND);
		if($res_num_rows > 0)
		{
			//$ttttt = pg_fetch_assoc($res_sql);
			$sql_status_update = "UPDATE tsim_simmis_map set status='".$status."' where order_no='".$orderid."'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Update record status->'.$sql_status_update.PHP_EOL, FILE_APPEND);
			$res_sql_status_update = pg_query($sql_status_update); 	 
			exit;
		}
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Inserting into tsim_simmis_map'.PHP_EOL, FILE_APPEND);
		$insert = "insert into tsim_simmis_map (tsim_username,email,contact,order_no,status)values('$username','$email','$phone','$orderid','$status')";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Insert Record->'.$insert.PHP_EOL, FILE_APPEND);
		$res_insert = pg_query($db, $insert);
		if($res_insert)
			echo "ok";
		else
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Failed to insert into tsim_simmis_map'.PHP_EOL, FILE_APPEND);
			exit;
		}
		$action = 'register';
		/***********************************************************************************************************/
		file_put_contents($log_file, date("Y-m-d H:i:s") . '*******************Checking for Order New or Not*************'.PHP_EOL, FILE_APPEND);
		$check3 = checkDuplicateEntry($db, $orderid ,$log_file);
        	if($check3)
                	{
		//Delete entry from tsim_order_deduplication to create new order
                file_put_contents($log_file, date("Y-m-d H:i:s") . '*******************Code Exit due to duplicate entry*************'.PHP_EOL, FILE_APPEND);
                exit();
                        }
        	else
                	{
                $check4 = createDuplicateEntry($db, $orderid ,$log_file);
                        }

		$items = count($obj['order']['line_items']);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Total items -'.$items.PHP_EOL, FILE_APPEND);
		$line_items = count($obj['order']['line_items']);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'line_items-'.$line_items.PHP_EOL, FILE_APPEND);
		for($i=0; $i<$line_items;$i++)
		{
			if($obj['order']['line_items']["$i"]['meta']['0']['value'] == 'Free Item')
			{
				$gift_item = $obj['order']['line_items']["$i"]['name'];
				$items = $items - 1;
				break;
			}
			else
				$gift_item="TSIM Booking";
		}
		file_put_contents($log_file, date("Y-m-d H:i:s") .$gift_item.PHP_EOL, FILE_APPEND);
		for( $j=0; $j<$items;$j++)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'j -'.$j.PHP_EOL, FILE_APPEND);
			$quant       = $obj['order']['line_items']["$j"]['quantity'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Quantity-'.$quant.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s").'j -'.$j.PHP_EOL, FILE_APPEND);
			for($g=0; $g<$quant;$g++)
			{
				for($c=0 ; $c<5 ;$c++)
				{
					if($obj['order']['line_items']["$j"]['meta']["$c"]['key'] == 'new-card-or-refill')
					{
						$order_type = $obj['order']['line_items']["$j"]['meta']["$c"]['value'];
						file_put_contents($log_file, date("Y-m-d H:i:s") .'Order-'.$order_type .PHP_EOL, FILE_APPEND);
						if($order_type == 'Refill')
						{
							$flag = 1;
							$sku            = $obj['order']['line_items']["$j"]['sku'];                          
							file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU-'.$sku.PHP_EOL, FILE_APPEND); 
							$ret = refill_order($orderid);
						}                                   
						else
						{
							$flag = 0; 
							break 3;
						}
					}
				}
			}
		}
		unset($items);
		unset($line_items);
		unset($i);
		unset($quant);
		unset($g);
		unset($c);
		unset($j);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Order type is-'.$order_type .PHP_EOL, FILE_APPEND);
		if($order_type == 'Refill')
			exit;
		/*********************************Checking for the multiple Userid for the this mail or Mobile No************/
		if(($phone == '' && $phone== NULL) || ($email =='' && $email==NULL))
		{
			exit;
		}
		$billing_address_data = $obj['order']['billing_address'];
		$shipping_address_data = $obj['order']['shipping_address'];

		$billing_address = $billing_address_data['address_1'] . ', ' . $billing_address_data['address_2'] . ', ' . $billing_address_data['city'] . ', ' . $billing_address_data['state'] . ', ' . $billing_address_data['postcode'] . ', ' . $billing_address_data['country'];

		$shipping_address = $shipping_address_data['address_1'] . ', ' . $shipping_address_data['address_2'] . ', ' . $shipping_address_data['city'] . ', ' . $shipping_address_data['state'] . ', ' . $shipping_address_data['postcode'] . ', ' . $shipping_address_data['country'];

		$orderEmail = $billing_address_data['email'];
		$orderMobile= $billing_address_data['phone'];
	$found_simuser = 0;
	if ($username_present_in_json ==1){
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Checking mis sim username existance with the woocommerce username' .PHP_EOL, FILE_APPEND);
		$select  = "select username from sim_user where username ='$username'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$select .PHP_EOL, FILE_APPEND);
		$result    = pg_query($db, $select);
		$num_rows       = pg_num_rows($result);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'NUM ROWS-'.$num_rows .PHP_EOL, FILE_APPEND);
		if($num_rows == 1){
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Single user with this mail or contact found.'.PHP_EOL, FILE_APPEND);
			#$rows = pg_fetch_assoc($result);
			#$username = $rows['username'];
			$pg_update = "update tsim_simmis_map set tsim_username = '$username' where order_no = '".$orderid."'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'db_update->'.$pg_update.PHP_EOL, FILE_APPEND);
			$res_pg_update = pg_query($db, $pg_update);
			$action = 'user_exists';
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Single user with this username found.'.PHP_EOL, FILE_APPEND);
			$found_simuser = 1;
		}
	}
	if ($found_simuser ==0){
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Checking mis sim username existance with either email or mobile no' .PHP_EOL, FILE_APPEND);
		$select  = "select username, emailadd, mobno from sim_user where emailadd = '$email' or mobno like '%$phone' and domainname='tsim.in'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$select .PHP_EOL, FILE_APPEND);
		$result    = pg_query($db, $select);
		$num_rows       = pg_num_rows($result);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'NUM ROWS-'.$num_rows .PHP_EOL, FILE_APPEND);
		if($num_rows == 1)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Single user with this mail or contact found.'.PHP_EOL, FILE_APPEND);
			$username = $GLOBALS['username'];
			$rows = pg_fetch_assoc($result);
			$username = $rows['username'];
			$pg_update = "update tsim_simmis_map set tsim_username = '$username' where order_no = '".$orderid."'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'db_update->'.$pg_update.PHP_EOL, FILE_APPEND);
			$res_pg_update = pg_query($db, $pg_update);
			$action = 'user_exists';
		}
		else if($num_rows>1)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Multiple username with this email or mobile no found.' .PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Sending link For User Validation' .PHP_EOL, FILE_APPEND);
			$message = "
				<html>
				<head>
				<title>Test Mail</title>
				</head>
				<body>
				<p><a href='https://mail.tsim.in/validate_user.php?o=$orderid&e=$email&m=$phone&r=$role&b=$billing_address&s=$shipping_address&orderemail=$orderEmail&ordermobile=$orderMobile'>Click Here To Validate</a></p>
				</body>
				</html>
				";
			$headers  = 'MIME-Version: 1.0' . "\r\n";
			$headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
			$headers .= 'From: TSIM Team <services@tsim.in>' . "\r\n";
			//$link = "https://mail.tsim.in/validate_user.php?o=$orderid&e=$email&m=$phone";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'link-'.$message.PHP_EOL, FILE_APPEND);
			mail("hoshang@staff.ownmail.com","Validate User for Order no $orderid",$message,$headers);
			mail("aditya@staff.ownmail.com","Validate User for Order no $orderid",$message,$headers);
			mail("binu@staff.ownmail.com","Validate User for Order no $orderid",$message,$headers);
			mail("services@tsim.in","Validate User for Order no $orderid","$message",$headers);
			exit;
		}
		else
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'No mis user found for this mail or contact no.'.PHP_EOL, FILE_APPEND);
			/***************************************Check whether length is less than 2*************************************************/
			file_put_contents($log_file, date("Y-m-d H:i:s") .'created TSIM username is-'.$username.PHP_EOL, FILE_APPEND);   
			$user_len = strlen($username);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'User length is-'.$user_len.PHP_EOL, FILE_APPEND);
			if($user_len <= 8)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . '*******calling create_user function*****************'.PHP_EOL, FILE_APPEND);
				$username = create_user($username, $phone);
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'New Username-'.$username.PHP_EOL, FILE_APPEND);
			}
			else
			{	
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Check whether the username-'.$username.'-with other domains exists?'.PHP_EOL, FILE_APPEND);
				#$chk_user  = "select username from sim_user where username like '$username%' order by username desc limit 1";
				#$chk_user  = "select username from sim_user where username = '$username'";
				//$chk_user   = "SELECT count(username) from sim_user where primary_emailadd like '$username@%' and username not like '1555%'";///Failed in the case of info13
				$chk_user   = "SELECT count(username) from sim_user where primary_emailadd like '$username%' and username not like '1555%'";
				file_put_contents($log_file, date("Y-m-d H:i:s") .$chk_user.PHP_EOL, FILE_APPEND);
				$res_chk_user = pg_query($db, $chk_user);
				$rec_user  = pg_fetch_row($res_chk_user);
				$num_user = $rec_user['0'];
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Total user present-'.$num_user .PHP_EOL, FILE_APPEND);
				if($num_user > 0)
				{
					$cnt = $num_user+5;
					file_put_contents($log_file, date("Y-m-d H:i:s") .'count is -'.$cnt .PHP_EOL, FILE_APPEND);
					//for($d=0;$d<$cnt;$d++) Not Working Properly.. Need to think again
					for($d=0;$d<$cnt;$d++)
					{
						$numbers = preg_replace('/[^0-9]/', '', $username);
						$uname = preg_replace('/[^a-zA-Z]/', '', $username);
						file_put_contents($log_file, date("Y-m-d H:i:s") .$numbers.PHP_EOL, FILE_APPEND);
						file_put_contents($log_file, date("Y-m-d H:i:s") .$uname.PHP_EOL, FILE_APPEND);
						$numbers++;
						file_put_contents($log_file, date("Y-m-d H:i:s") .$numbers.PHP_EOL, FILE_APPEND);
						$username = $uname.$numbers;
						file_put_contents($log_file, date("Y-m-d H:i:s") .'New username-'.$username.PHP_EOL, FILE_APPEND);
						$chk_new_user ="select username from sim_user where username = '$username'";
						file_put_contents($log_file, date("Y-m-d H:i:s") .'Chk_new_user-'.$chk_new_user .PHP_EOL, FILE_APPEND);
						$res_chk_new_user = pg_query($db,$chk_new_user);
						if(!$res_chk_new_user)
							file_put_contents($log_file, date("Y-m-d H:i:s") .'Not executes'.PHP_EOL, FILE_APPEND);
						$num_chk_new_user = pg_num_rows($res_chk_new_user);
						file_put_contents($log_file, date("Y-m-d H:i:s") .$num_chk_new_user .PHP_EOL, FILE_APPEND);
						if($num_chk_new_user > 0)    
						{                         
							$flag_user = 0;  
							continue;       
						}                      
						else                  
						{                    
							$flag_user = 1;     
							break;             
						}  
					}
					if($flag_user == 1)           
					{                          
						file_put_contents($log_file, date("Y-m-d H:i:s") .'Username for this order is-'.$username.PHP_EOL, FILE_APPEND);
					}                                                                                                   
					else                                                                                               
					{                                                                                                 
						file_put_contents($log_file, date("Y-m-d H:i:s") .'Can not create new user-Exit'.PHP_EOL, FILE_APPEND);
						exit;                                                                                       
					}
				}
				else
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Username for this order is-'.$username.PHP_EOL, FILE_APPEND);
			}
			/*******************************Check Username with Domainname******************************************/
			$chk_user_domains = "select * from users where username='$username'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$chk_user_domains.PHP_EOL, FILE_APPEND);
			$res_chk_user_domains = pg_query($db, $chk_user_domains);
			if(!$res_chk_user_domains)
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Not Executed'.PHP_EOL, FILE_APPEND);
			$nn = pg_num_rows($res_chk_user_domains);
			if($nn > 0)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'*******Username exist with other domainame.Create New User********************'.PHP_EOL, FILE_APPEND);
				$username = create_user($username, $phone);
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'New Username-'.$username.PHP_EOL, FILE_APPEND);
			}
			$action = 'register';
		}
	}
	}
	/*else
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . '---------Inside Role-'.$role.'---------'.PHP_EOL, FILE_APPEND);
	}*/		
}
//if(isset($_POST['submit']))
if(isset($_POST['submit']) == 'SUBMIT' && !isset($_POST['new_user']))
{
	file_put_contents($log_file, date("Y-m-d H:i:s") . '--------Inside validation submit for Existing User---------------------------'.PHP_EOL, FILE_APPEND);
	global $orderid,$items,$email,$phone,$action,$first_name,$last_name,$clientname,$domainname,$company,$clientadd1,$clientadd2,$address_city,$address_state,$address_country,$postcode,$start_date,$payment_id,$status,$curr,$total,$sub_total,$total_shipping,$country_specific,$payment_mode,$link_amt,$total_tax,$username,$subtotal,$mnt,$gift_item,$note,$quantity,$sku,$item_cost,$sk,$country_dur_stdcode,$country,$duration,$std_code,$max_from_date,$max_to_date,$sim_phone_no,$order_type,$country_specific,$action,$shipping_add1,$shipping_add2,$shipping_city,$shipping_state,$shipping_country,$shipping_postcode,$flag_user,$client_mail,$db,$log_file,$first_name2,$last_name2,$clientname2,$l_clientname,$l_phone,$role,$is_kyc_required;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Role is -'.$role. PHP_EOL, FILE_APPEND);
	if($role == '' || $role == null)
		$role   =  	$_POST['role']; 
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Role is -'.$role. PHP_EOL, FILE_APPEND);
	$username 	= 	$_POST['set_username'];
	$email 		= 	$_POST['set_emailadd'];
	$orderid 	= 	$_POST['order_no'];
	$phone  	=	$_POST['set_mobno'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Username->'.$username. PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Email->'.$email. PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'orderid->'.$orderid. PHP_EOL, FILE_APPEND); 
	//$role = $obj['order']['customer']['role'];
	//file_put_contents($log_file, date("Y-m-d H:i:s") . 'Role of the client---'.$role.PHP_EOL, FILE_APPEND);
	//exit;
	//$role = 'reseller_customer';
	###Call woocommerce api for the particular order details###############
	/*******************************************************************/
	$chk_order = "select order_no from clienttrip where order_no = '$orderid'";
	file_put_contents("$log_file",date("Y-m-d H:i:s").'chk_order-'.$chk_order. PHP_EOL, FILE_APPEND);
	$res_chk_order = pg_query($db, $chk_order);
	$num_chk_order = pg_num_rows($res_chk_order);
	if($num_chk_order >= 1 )
	{
		echo "\nThis link is expired as $orderid is already booked.";
		file_put_contents("$log_file",date("Y-m-d H:i:s").'This order is already booked. Exit'. PHP_EOL, FILE_APPEND);
		exit;
	}		
	$update_user = "UPDATE tsim_simmis_map SET tsim_username ='$username' where order_no ='$orderid';";
	file_put_contents("$log_file",date("Y-m-d H:i:s").'update_user-'.$update_user . PHP_EOL, FILE_APPEND);
	$res = pg_query($db, $update_user);
	$get_order = "select * from tsim_orders where order_no = '$orderid'";
	file_put_contents("$log_file",date("Y-m-d H:i:s").'get_order-'.$get_order. PHP_EOL, FILE_APPEND);
	$res_get_order = pg_query($db, $get_order);
	if(!$res_get_order)
	{
		file_put_contents("$log_file",date("Y-m-d H:i:s").'No Json for this Order. Exit'. PHP_EOL, FILE_APPEND);	
		exit;
	}
	/***************************Earlier code to fetch the json using api*************************
	$temp_log_file = "/tmp/temp_tsim_order.json";
	$curl_string = "/usr/bin/curl --insecure -H 'Content-Type: application/json' 'https://www.tsim.in/wp-json/wc/v1/orders/$orderid/?consumer_key=ck_8efb60984738bd6b53581a483439a2e4f52c102e&consumer_secret=cs_91b8c2ad305565c5feaaf5c1296164ed4eb4e534'";
	file_put_contents("$log_file",$curl_string . PHP_EOL, FILE_APPEND);
	$ch = curl_init();                    // initiate curl
	curl_setopt_array($ch, array(
		 CURLOPT_RETURNTRANSFER => 1,
		 CURLOPT_URL =>"https://www.tsim.in/wp-json/wc/v1/orders/$orderid/?consumer_key=ck_8efb60984738bd6b53581a483439a2e4f52c102e&consumer_secret=cs_91b8c2ad305565c5feaaf5c1296164ed4eb4e534"
	   ));                                                                                                                   
	$output = curl_exec ($ch); // execute
	file_put_contents("$temp_log_file",$output . PHP_EOL, FILE_APPEND);
	file_put_contents("$log_file",$output . PHP_EOL, FILE_APPEND);
	$json = file_get_contents("$temp_log_file");
	**********************************************************************************************/
	$rec_get_order = pg_fetch_assoc($res_get_order);
	$json = $rec_get_order['json']; 
	$obj = json_decode($json, true);
	$arr = print_r($obj, true);
	//file_put_contents($log_file, date("Y-m-d H:i:s") . 'json->'.$obj. PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'email from validate link->'.$email. PHP_EOL, FILE_APPEND);
	$email          = $obj['order']['customer']['email'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'email->'.$email. PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'arr->'.$arr. PHP_EOL, FILE_APPEND);
	$status = $obj['order']['status'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'status->'.$status. PHP_EOL, FILE_APPEND);
	if($status == 'cancelled' || $status == 'failed')
		exit;
	$items = count($obj['order']['line_items']);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'items->'.$items. PHP_EOL, FILE_APPEND);
	$ret_per_info = parse_personal_info($json);
	/*file_put_contents($log_file, date("Y-m-d H:i:s") . 'First Name-'.$first_name.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Last Name-'.$last_name.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Billing Client Name-'.$clientname.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'First Name-'.$first_name2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Last Name-'.$last_name2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Client Name-'.$clientname2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client Name-'.$l_clientname.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") .'l_phone-'.$l_phone.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Company-'.$company.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add1-'.$clientadd1.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add2-'.$clientadd2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_city -'.$address_city.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_state-'.$address_state.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_country-'.$address_country.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'postcode-'.$postcode.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add1-'.$shipping_add1.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add2-'.$shipping_add2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping City-'.$shipping_city.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_state-'.$shipping_state.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_country-'.$shipping_country.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping postcode-'.$shipping_postcode.PHP_EOL, FILE_APPEND);	
	 */
	$start_date     = $obj['order']['created_at'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$start_date.PHP_EOL, FILE_APPEND);
	$start_date = date('Y-m-d H:i:s',strtotime($start_date));
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$start_date.PHP_EOL, FILE_APPEND);
	$status         = $obj['order']['status'];
	$curr           = $obj['order']['currency'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Currency-'.$curr.PHP_EOL, FILE_APPEND);
	$total          = $obj['order']['total'];
	$total_tax      = $obj['order']['total_tax'];
	$sub_total      = $obj['order']['subtotal'];
	$total_shipping = $obj['order']['total_shipping'];
	//$gstin 		= $obj['order']['customer']['gstin'];
	$is_kyc_required =  isset($obj['order']['is_kyc_required']) ? $obj['order']['is_kyc_required'] : null;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'is kyc required:  '.$is_kyc_required.PHP_EOL, FILE_APPEND);
  	$gstin1 = isset($obj['order']['customer']['gstin']) ? $obj['order']['customer']['gstin'] : null;
        $gstin1 = preg_replace("/[^A-Za-z0-9]/", "", $gstin1);


        if (strlen($gstin1) === 15) 
        {
                file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin1.PHP_EOL, FILE_APPEND);
                $gstin=$gstin1;
        }
        else if(strlen($gstin1) === 0)
        {
		file_put_contents($log_file, date("Y-m-d H:i:s") . ' GST IN  Field is Empty-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

        }
        else
        {
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN is INVALID-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

        }
	
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin.PHP_EOL, FILE_APPEND);
	if($total_shipping == "")
		$total_shipping=0;
	$payment_id     = $obj['order']['payment_details']['method_id'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'payment id-'.$payment_id.PHP_EOL, FILE_APPEND);
	$delivery_method = $obj['order']['shipping_methods'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Delivery Method-'.$delivery_method.PHP_EOL, FILE_APPEND);
	if( $payment_id == "payuindia" || $payment_id == "pwa" || $payment_id == "razorpay")
	{
		$payment_mode = "Credit Card" ;
	}
	else if( $payment_id == "bacs" || $payment_id == "cod")
	{
		$payment_mode = "NEFT/IMPS Payment";
	}
	else if( $payment_id == "Prepaid" || $payment_id == "other_payment")
	{
		$payment_mode = "PAD";
		//$link_amt = $total - $total_tax;
		$link_amt = $total;
	}
	else
	{	
		$payment_mode = "Other Payment";
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Payment Mode-'.$payment_mode.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'total-'.$total.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'total_tax-'.$total_tax.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'link_amt-'.$link_amt.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'sub_total-'.$sub_total.PHP_EOL, FILE_APPEND);

	$currencyforcasesource = $obj['order']['currency'];
        if($currencyforcasesource == "INR") {
                 $case_source    = "tsim.in";
        } else{
                 $case_source    = "tsim.mobi";
        }

	//$case_source    = "tsim.in";
	$mnt = count($obj['order']['line_items']);
	for($i=0; $i<$mnt;$i++)
	{
		if($obj['order']['line_items']["$i"]['meta']['0']['value'] == 'Free Item')
		{
			$gift_item = $obj['order']['line_items']["$i"]['name'];
			$items = $items - 1;
			break;
		}
		else
			$gift_item="TSIM Booking";
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Gift Item-'.$gift_item.PHP_EOL, FILE_APPEND);
	$note           =       $obj['order']['note'];
	$note           = trim($note);
	$note           = str_replace(' ', '', $note);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'note-'.$note.PHP_EOL, FILE_APPEND);
	for( $l=0; $l<$items;$l++)
	{
		$quantity       = $obj['order']['line_items']["$l"]['quantity'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Quantity-'.$quantity.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.PHP_EOL, FILE_APPEND);
		for($q=1; $q<=$quantity;$q++)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") . '********item starts********'.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.'-q is-'.$q.PHP_EOL, FILE_APPEND);
			/********************Checking For item is Free Item or Not************************************/
			for($z=0; $z<5;$z++)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.'-q is-'.$q.'-z is -'.$z.PHP_EOL, FILE_APPEND);
				if($obj['order']['line_items']["$l"]['meta']["$z"]['value'] == 'Free Item')
				{
					file_put_contents($log_file, date("Y-m-d H:i:s") . '*******Free Item->Skip********'.PHP_EOL, FILE_APPEND);
					continue 3;
				}
				$itm_type = $obj['order']['line_items']["$l"]['name'];
				file_put_contents($log_file, date("Y-m-d H:i:s").'Item Type-'.$itm_type.PHP_EOL, FILE_APPEND);
				if(strstr($obj['order']['line_items']["$l"]['name'],"Insurance"))
				{
					file_put_contents($log_file, date("Y-m-d H:i:s").'Insurance Item-Skip'.PHP_EOL, FILE_APPEND);
					continue 3;
				}
			}
			$sku            = $obj['order']['line_items']["$l"]['sku'];

			$meta_arr = $obj['order']['line_items'][$l]['meta'];
			$activationDate =  null;
			foreach($meta_arr as $v)
			{
				#if($v['key']==='Activation Date - 1' || $v['key']==='Activation Date')
				if(substr($v['key'],0,15) == 'Activation Date')
				{
					$activationDate =  $v['value'];

				}

			}

			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Activation Date-'.$activationDate.PHP_EOL, FILE_APPEND);

file_put_contents($log_file, date("Y-m-d H:i:s") . ' Activation Date-' . $activationDate . PHP_EOL, FILE_APPEND);

// Log the original date first
file_put_contents($log_file, "Original Activation Date: " . $activationDate . PHP_EOL, FILE_APPEND);

// Ensure the date is not null
if ($activationDate !== null) {
    // Try parsing the date using the 'd-m-Y' format (for a 4-digit year)
    $dateObj = DateTime::createFromFormat('d-m-Y', $activationDate);

    // Check for parsing errors
    if ($dateObj && $dateObj->format('d-m-Y') === $activationDate) {
        // Success: convert to 'Y-m-d H:i:s' format
        $activationDate = $dateObj->format('Y-m-d H:i:s');
    } else {
        // Parsing failed or format mismatch: log error and handle gracefully
        file_put_contents($log_file, "Error: Invalid date format or failed conversion for Activation Date." . PHP_EOL, FILE_APPEND);
        // Handle the error (skip, throw exception, or set a default date)
        // Example: throw an exception or return a fallback value
        throw new Exception("Date format is invalid: expected d-m-Y, got " . $activationDate);
    }
}

// Log the final activation date
file_put_contents($log_file, "Final Activation Date: " . $activationDate . PHP_EOL, FILE_APPEND);















/*
			if($activationDate !== null)
			{
				//	 $activationDate = DateTime::createFromFormat('d-m-y', $activationDate)->format('Y-m-d H:i:s');
				$activationDate = date('Y-m-d H:i:s',strtotime($activationDate));
			}*/

			//$activationDate = date('Y-m-d H:i:s',strtotime($activationDate));
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$activationDate.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU-'.$sku.PHP_EOL, FILE_APPEND);
			/******************************Check for travel Insurance*******************************************/
			$sku_chk = "select * from sku_not_to_map where sku='$sku'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$sku_chk.PHP_EOL, FILE_APPEND);
			$res_sku_chk = pg_query($db, $sku_chk);
			if(!$res_sku_chk)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Failed to execute-Exit'.PHP_EOL, FILE_APPEND);
				exit;
			}
			$num_sku_chk = pg_num_rows($res_sku_chk);
			if($num_sku_chk > 0)
			{
				$only_TI = 1;
				continue;
			}
			else
			{
				$only_TI = 0;
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") .'only_TI----'.$only_TI.PHP_EOL, FILE_APPEND);
			/********************************************************************************************************/
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'case source-'.$case_source.PHP_EOL, FILE_APPEND);
			//$item_cost      = $obj['line_items']["$l"]['price'];
			//file_put_contents($log_file, date("Y-m-d H:i:s") . 'item_cost-'.$item_cost.PHP_EOL, FILE_APPEND);
			/*************************************Added*********************************************************/
			$item_total      = $obj['order']['line_items']["$l"]['total'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Item Total-'.$item_total.PHP_EOL, FILE_APPEND);
			$subtotal      = $obj['order']['line_items']["$l"]['subtotal'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'subtotal-'.$subtotal.PHP_EOL, FILE_APPEND);
			$subtotal      = $subtotal/$quantity;
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Individual amt for entry-'.$subtotal.PHP_EOL, FILE_APPEND);
			$item_cost      = $obj['order']['line_items']["$l"]['price'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'item_cost-'.$item_cost.PHP_EOL, FILE_APPEND);
			/******************************************************************************************************/
			$sk             = substr($sku, 0, strpos($sku, '-'));
			$country_dur_stdcode = explode("_",$sk);
			$country        = $country_dur_stdcode[0];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'country-'.$country.PHP_EOL, FILE_APPEND);
			$duration       = $country_dur_stdcode[1];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Duration-'.$duration.PHP_EOL, FILE_APPEND);


			//			$get_dur_q = "select pack_validity from tsim_pack_details where pack_name='$sku' limit 1";

			$get_dur_q = "SELECT substring(validity from '\d+ Days') AS pack_validity FROM tsim_orders_line_items_test WHERE sku = '$sku' LIMIT 1";
			file_put_contents($log_file, date("Y-m-d H:i:s") . '- '.$get_dur_q.PHP_EOL, FILE_APPEND);
			$e_dur_q = pg_query($db,$get_dur_q);
			$num_dur_q = pg_num_rows($e_dur_q);
			file_put_contents($log_file, date("Y-m-d H:i:s") . ' Num rows - '.$num_dur_q.PHP_EOL, FILE_APPEND);
			if($num_dur_q > 0){
				$f_dur_q = pg_fetch_assoc($e_dur_q);	
				$dur_array = explode(" ",$f_dur_q['pack_validity']);
				file_put_contents($log_file, date("Y-m-d H:i:s") . '- Days -> '.$dur_array[0].PHP_EOL, FILE_APPEND);
				$duration = $dur_array[0];
			}
			$array  = read_config_file('<!--SKU_STD_CODE','--SKU_STD_CODE>');
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Config File Read -'.serialize($array).PHP_EOL, FILE_APPEND);
			if($array[$sku])
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU Present For STD CODE-'.$array[$sku].PHP_EOL, FILE_APPEND);
				$std_code = $array[$sku];

			}
			else if (strpos($sku,'91')!= false && strpos($sku,'91_Data')== false)
			{
				$std_code = '91';
			}
			else if(strpos($sku,'91_Data')== true)
			{
				$std_code = '91_Data';
			}
			else
			{
				$std_code ="";
			}
			if (strpos($sku,'91')!== false || strpos($sku,'91_Data')!== false  || $array[$sku])
			{
				$today  =       date("Y-m-d H:i:s");
				$td     =       explode(" ",$today);
				$dur    =       $duration;
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Duration-'.$dur.PHP_EOL, FILE_APPEND);
				$future_date =  date('Y-m-d', strtotime("$today +360 days"));
				$max_from_date  =       $future_date." ".$td[1];
				$max_to_date    = date('Y-m-d', strtotime("$max_from_date +$dur days"));
				$max_to_date    = $max_to_date." ".$td[1];
			}
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'MAX SET to NULL'.PHP_EOL, FILE_APPEND);
				$max_from_date = null;
				$max_to_date = null;
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'std_code-'.$std_code.PHP_EOL, FILE_APPEND);
			for($i=0 ; $i<5 ;$i++)
			{
				if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'new-card-or-refill')
				{
					$order_type = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
					break;
				}
			}
			if ($order_type =="")
				$order_type='New';
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Order Type-'.$order_type.PHP_EOL, FILE_APPEND);
			if($order_type == "New")
			{
				/*for($i=0 ; $i<5 ;$i++)
				{
					if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'number')
					{
					      if($obj['order']['line_items']["$l"]['meta']["$i"]['value'] != "UK (Cheapest Option)")
					      {
						      $country_specific = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
						      break;
					      }
					      else if(empty($obj['order']['line_items']["$l"]['meta']["$i"]['value']) != null)
					      {
							$country_specific="UK Cheapest"; 
					      }
				      }
				      else
				      {
					      $country_specific="UK Cheapest";
				      }
				}
	 */
	if($role == 'reseller_customer')
	{	
		$count_items = count($obj['order']['line_items']["$l"]['meta']);
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Total number of items in it are -'.$count_items.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") . '$ab -'.$ab.PHP_EOL, FILE_APPEND);
		for($ab = 0; $ab < $count_items; $ab++)
		{
			$key = $obj['order']['line_items']["$l"]['meta']["$ab"]['key'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Key is -'.$key.PHP_EOL, FILE_APPEND);
			#if($key == "Serial Number - $q" || $key ==='Serial Number')
			if (substr($key,0,13) == 'Serial Number')
			{
				$serial_no = $obj['order']['line_items']["$l"]['meta']["$ab"]['value'];
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Serial No is -'.$serial_no.PHP_EOL, FILE_APPEND);
			}
		}
	}
	else
		$country_specific="UK Cheapest";
			}
			else if($order_type == "Refill")
			{
				for($i=0 ; $i<5 ;$i++)
				{
					if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'Current TSIM number')
					{
						$sim_phone_no = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
						break;
					}
				}
				if($sim_phone_no !="" || $sim_phone_no != NULL)
				{
					$sim_phone_no = $sim_phone_no;
				}
				else
				{
					$sim_phone_no = "Not Mentioned";
				}
			}
			else
			{
				$order_type = "";
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'country_specific-'.$country_specific.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . '***************Curl started****************'.PHP_EOL, FILE_APPEND);
			$param = "l=admin&p=chair&a=tsim_client&role=$role&order_type=$order_type&gift_item=$gift_item&uemail=$email&c_name=$clientname&username=$username&domainname=$domainname&company=$company&phone=$phone&c_add1=$clientadd1&c_add2=$clientadd2&c_address_city=$address_city&c_address_state=$address_state&c_address_country=$address_country&postcode=$postcode&shipping_add1=$shipping_add1&shipping_add2=$shipping_add2&shipping_city=$shipping_city&shipping_state=$shipping_state&shipping_country=$shipping_country&shipping_postcode=$shipping_postcode&start_date=$start_date&payment_mode=$payment_mode&orderid=$orderid&status=$status&amt=$total&link_amt=$link_amt&sub_total=$sub_total&subtotal=$subtotal&item_cost=$item_cost&total_tax=$total_tax&curr=$curr&sim_phone_no=$sim_phone_no&case_source=$case_source&sku=$sku&country=$country&country_specific=$country_specific&duration=$duration&std_code=$std_code&max_from_date=$max_from_date&max_to_date=$max_to_date&sk=$sk&delivery_method=$delivery_method&payment_id=$payment_id&l_cname=$l_clientname&l_cphone=$l_phone&serial_no=$serial_no&activation_date=$activationDate&gstin=$gstin&productname=$productname&is_kyc_required=$is_kyc_required";
			$ret_curl = curl_data($param);
		}
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .'only_TI----'.$only_TI.PHP_EOL, FILE_APPEND);
	if($only_TI == 1)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Only Medical Insurance----Create only User---'.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") . '**************Curl Started*****************'.PHP_EOL, FILE_APPEND);
		$param = "l=admin&p=chair&a=create_only_user&order_type=$order_type&gift_item=$gift_item&uemail=$email&c_name=$clientname&username=$username&domainname=$domainname&company=$company&phone=$phone&c_add1=$clientadd1&c_add2=$clientadd2&c_address_city=$address_city&c_address_state=$address_state&c_address_country=$address_country&postcode=$postcode&shipping_add1=$shipping_add1&shipping_add2=$shipping_add2&shipping_city=$shipping_city&shipping_state=$shipping_state&shipping_country=$shipping_country&shipping_postcode=$shipping_postcode&start_date=$start_date&payment_mode=$payment_mode&orderid=$orderid&status=$status&amt=$total&link_amt=$link_amt&sub_total=$sub_total&subtotal=$subtotal&item_cost=$item_cost&total_tax=$total_tax&curr=$curr&sim_phone_no=$sim_phone_no&case_source=$case_source&sku=$sku&country=$country&country_specific=$country_specific&duration=$duration&std_code=$std_code&max_from_date=$max_from_date&max_to_date=$max_to_date&sk=$sk&delivery_method=$delivery_method&payment_id=$payment_id&l_cname=$l_clientname&l_cphone=$l_phone&activation_date=$activationDate&gstin=$gstin&productname=$productname&is_kyc_required=$is_kyc_required";
		$ret_curl = curl_data($param);
	}
	else
	{
		send_mail($obj);
	}
	exit;
}
if(isset($_POST['submit'])== 'SUBMIT' && isset($_POST['new_user']))
{
	file_put_contents($log_file, date("Y-m-d H:i:s") . '--------Inside validation submit to Create a new User---------------------------'.PHP_EOL, FILE_APPEND);
	global $orderid,$items,$email,$phone,$action,$first_name,$last_name,$clientname,$domainname,$company,$clientadd1,$clientadd2,$address_city,$address_state,$address_country,$postcode,$start_date,$payment_id,$status,$curr,$total,$sub_total,$total_shipping,$country_specific,$payment_mode,$link_amt,$total_tax,$username,$subtotal,$mnt,$gift_item,$note,$quantity,$sku,$item_cost,$sk,$country_dur_stdcode,$country,$duration,$std_code,$max_from_date,$max_to_date,$sim_phone_no,$order_type,$country_specific,$action,$shipping_add1,$shipping_add2,$shipping_city,$shipping_state,$shipping_country,$shipping_postcode,$flag_user,$client_mail,$db,$log_file,$first_name2,$last_name2,$clientname2,$l_clientname,$l_phone,$role,$is_kyc_required;
	if($role == '' || $role == null)
		$role   =       $_POST['role'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Role is -'.$role. PHP_EOL, FILE_APPEND);
	$orderid        =       $_POST['order_no'];
	$chk_order = "select order_no from clienttrip where order_no = '$orderid'";
	file_put_contents("$log_file",date("Y-m-d H:i:s").'chk_order-'.$chk_order. PHP_EOL, FILE_APPEND);
	$res_chk_order = pg_query($db, $chk_order);
	$num_chk_order = pg_num_rows($res_chk_order);
	if($num_chk_order >= 1 )
	{
		echo "\nThis link is expired as $orderid is already booked.";
		file_put_contents("$log_file",date("Y-m-d H:i:s").'This order is already booked. Exit'. PHP_EOL, FILE_APPEND);
		exit;
	}
	$get_order = "select * from tsim_orders where order_no = '$orderid'";
	file_put_contents("$log_file",date("Y-m-d H:i:s").'get_order-'.$get_order. PHP_EOL, FILE_APPEND);
	$res_get_order = pg_query($db, $get_order);
	if(!$res_get_order)
	{
		file_put_contents("$log_file",date("Y-m-d H:i:s").'No Json for this Order. Exit'. PHP_EOL, FILE_APPEND);
		exit;
	}
	$rec_get_order = pg_fetch_assoc($res_get_order);
	$json = $rec_get_order['json'];
	$obj = json_decode($json, true);
	$arr = print_r($obj, true);
	$email          = $obj['order']['customer']['email'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'email->'.$email. PHP_EOL, FILE_APPEND);
	$status = $obj['order']['status'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'status->'.$status. PHP_EOL, FILE_APPEND);
	if($status == 'cancelled' || $status == 'failed')
		exit;
	$items = count($obj['order']['line_items']);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'items->'.$items. PHP_EOL, FILE_APPEND);
	$ret_per_info = parse_personal_info($json);
	$phone          = $obj['order']['customer']['billing_address']['phone'];
	$phone1          = str_replace(' ','',$phone);
	if (strpos($phone1, '+') !== false)
	{
		$ph             = explode("+",$phone1);
		$phone          = $ph[1];
	}
	else if (strpos($phone1, '/') !== false)
	{
		$ph_ar =  explode("/",$phone1);
		$phone = $ph_ar[0];
	}
	else $phone = $phone1;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'phone-'.$phone.PHP_EOL, FILE_APPEND);
	$username = create_user('',$phone);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'New Username-'.$username.PHP_EOL, FILE_APPEND);
	$start_date     = $obj['order']['created_at'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$start_date.PHP_EOL, FILE_APPEND);
	$start_date = date('Y-m-d H:i:s',strtotime($start_date));
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$start_date.PHP_EOL, FILE_APPEND);
	$status         = $obj['order']['status'];
	$curr           = $obj['order']['currency'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Currency-'.$curr.PHP_EOL, FILE_APPEND);
	$total          = $obj['order']['total'];
	$total_tax      = $obj['order']['total_tax'];
	$sub_total      = $obj['order']['subtotal'];
	$total_shipping = $obj['order']['total_shipping'];
	//	$gstin 		= $obj['order']['customer']['gstin'];
	$is_kyc_required =  isset($obj['order']['is_kyc_required']) ? $obj['order']['is_kyc_required'] : null;

	file_put_contents($log_file, date("Y-m-d H:i:s") . 'is kyc required:  '.$is_kyc_required.PHP_EOL, FILE_APPEND);
	
	$gstin1 = isset($obj['order']['customer']['gstin']) ? $obj['order']['customer']['gstin'] : null;
        $gstin1 = preg_replace("/[^A-Za-z0-9]/", "", $gstin1);


        if (strlen($gstin1) === 15)
        {
                file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin1.PHP_EOL, FILE_APPEND);
                $gstin=$gstin1;
        }
        else if(strlen($gstin1) === 0)
        {
		file_put_contents($log_file, date("Y-m-d H:i:s") . ' GST IN  Field is Empty-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

        }
        else
        {
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN is INVALID-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

        }



	file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin.PHP_EOL, FILE_APPEND);
	if($total_shipping == "")
		$total_shipping=0;
	$payment_id     = $obj['order']['payment_details']['method_id'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'payment id-'.$payment_id.PHP_EOL, FILE_APPEND);
	$delivery_method = $obj['order']['shipping_methods'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Delivery Method-'.$delivery_method.PHP_EOL, FILE_APPEND);
	if( $payment_id == "payuindia" || $payment_id == "pwa")
	{
		$payment_mode = "Credit Card" ;
	}
	else if( $payment_id == "bacs" || $payment_id == "cod")
	{
		$payment_mode = "NEFT/IMPS Payment";
	}
	else if( $payment_id == "Prepaid" || $payment_id == "other_payment")
	{
		$payment_mode = "PAD";
		$link_amt = $total;
	}
	else
	{
		$payment_mode = "Other Payment";
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Payment Mode-'.$payment_mode.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'total-'.$total.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'total_tax-'.$total_tax.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'link_amt-'.$link_amt.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'sub_total-'.$sub_total.PHP_EOL, FILE_APPEND);

	$currencyforcasesource = $obj['order']['currency'];
        if($currencyforcasesource == "INR") {
                 $case_source    = "tsim.in";
        } else{
                 $case_source    = "tsim.mobi";
        }

	//$case_source    = "tsim.in";
	$productname = $obj['order']['line_items'][0]['name'];

	$mnt = count($obj['order']['line_items']);
	for($i=0; $i<$mnt;$i++)
	{
		if($obj['order']['line_items']["$i"]['meta']['0']['value'] == 'Free Item')
		{
			$gift_item = $obj['order']['line_items']["$i"]['name'];
			$items = $items - 1;
			break;
		}
		else
			$gift_item="TSIM Booking";
		$gift_item="TSIM Booking";
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Gift Item-'.$gift_item.PHP_EOL, FILE_APPEND);
	$note           =       $obj['order']['note'];
	$note           = trim($note);
	$note           = str_replace(' ', '', $note);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'note-'.$note.PHP_EOL, FILE_APPEND);
	for( $l=0; $l<$items;$l++)
	{
		$quantity       = $obj['order']['line_items']["$l"]['quantity'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Quantity-'.$quantity.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.PHP_EOL, FILE_APPEND);
		for($q=1; $q<=$quantity;$q++)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") . '********item starts********'.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.'-q is-'.$q.PHP_EOL, FILE_APPEND);
			/********************Checking For item is Free Item or Not************************************/
			for($z=0; $z<5;$z++)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.'-q is-'.$q.'-z is -'.$z.PHP_EOL, FILE_APPEND);
				if($obj['order']['line_items']["$l"]['meta']["$z"]['value'] == 'Free Item')
				{
					file_put_contents($log_file, date("Y-m-d H:i:s") . '*******Free Item->Skip********'.PHP_EOL, FILE_APPEND);
					continue 3;
				}
				$itm_type = $obj['order']['line_items']["$l"]['name'];
				file_put_contents($log_file, date("Y-m-d H:i:s").'Item Type-'.$itm_type.PHP_EOL, FILE_APPEND);
				if(strstr($obj['order']['line_items']["$l"]['name'],"Insurance"))
				{
					file_put_contents($log_file, date("Y-m-d H:i:s").'Insurance Item-Skip'.PHP_EOL, FILE_APPEND);
					continue 3;
				}

			}
			$sku            = $obj['order']['line_items']["$l"]['sku'];

			$meta_arr = $obj['order']['line_items'][$l]['meta'];
			$activationDate =  null;
			foreach($meta_arr as $v)
			{
				#if($v['key']==='Activation Date - 1'|| $v['key']==='Activation Date')
				if(substr($v['key'],0,15) == 'Activation Date')
				{
					$activationDate =  $v['value'];

				}

			}

			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Activation Date-'.$activationDate.PHP_EOL, FILE_APPEND);

			file_put_contents($log_file, date("Y-m-d H:i:s") . ' Activation Date-' . $activationDate . PHP_EOL, FILE_APPEND);

// Log the original date first
file_put_contents($log_file, "Original Activation Date: " . $activationDate . PHP_EOL, FILE_APPEND);

// Ensure the date is not null
if ($activationDate !== null) {
    // Try parsing the date using the 'd-m-Y' format (for a 4-digit year)
    $dateObj = DateTime::createFromFormat('d-m-Y', $activationDate);

    // Check for parsing errors
    if ($dateObj && $dateObj->format('d-m-Y') === $activationDate) {
        // Success: convert to 'Y-m-d H:i:s' format
        $activationDate = $dateObj->format('Y-m-d H:i:s');
    } else {
        // Parsing failed or format mismatch: log error and handle gracefully
        file_put_contents($log_file, "Error: Invalid date format or failed conversion for Activation Date." . PHP_EOL, FILE_APPEND);
        // Handle the error (skip, throw exception, or set a default date)
        // Example: throw an exception or return a fallback value
        throw new Exception("Date format is invalid: expected d-m-Y, got " . $activationDate);
    }
}

// Log the final activation date
file_put_contents($log_file, "Final Activation Date: " . $activationDate . PHP_EOL, FILE_APPEND);





			/*
			if($activationDate !== null)
			{
				//$activationDate = DateTime::createFromFormat('d-m-y', $activationDate)->format('Y-m-d H:i:s');
				$activationDate = date('Y-m-d H:i:s',strtotime($activationDate));
			}*/

			// $activationDate = date('Y-m-d H:i:s',strtotime($activationDate));
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$activationDate.PHP_EOL, FILE_APPEND);

			file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU-'.$sku.PHP_EOL, FILE_APPEND);
			/******************************Check for travel Insurance*******************************************/
			$sku_chk = "select * from sku_not_to_map where sku='$sku'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$sku_chk.PHP_EOL, FILE_APPEND);
			$res_sku_chk = pg_query($db, $sku_chk);
			if(!$res_sku_chk)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Failed to execute-Exit'.PHP_EOL, FILE_APPEND);
				exit;
			}
			$num_sku_chk = pg_num_rows($res_sku_chk);
			if($num_sku_chk > 0)
			{
				$only_TI = 1;
				continue;
			}
			else
			{
				$only_TI = 0;
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") .'only_TI----'.$only_TI.PHP_EOL, FILE_APPEND);
			/********************************************************************************************************/
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'case source-'.$case_source.PHP_EOL, FILE_APPEND);
			//$item_cost      = $obj['line_items']["$l"]['price'];
			//file_put_contents($log_file, date("Y-m-d H:i:s") . 'item_cost-'.$item_cost.PHP_EOL, FILE_APPEND);
			/*************************************Added*********************************************************/
			$item_total      = $obj['order']['line_items']["$l"]['total'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Item Total-'.$item_total.PHP_EOL, FILE_APPEND);
			$subtotal      = $obj['order']['line_items']["$l"]['subtotal'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'subtotal-'.$subtotal.PHP_EOL, FILE_APPEND);
			$subtotal      = $subtotal/$quantity;
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Individual amt for entry-'.$subtotal.PHP_EOL, FILE_APPEND);
			$item_cost      = $obj['order']['line_items']["$l"]['price'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'item_cost-'.$item_cost.PHP_EOL, FILE_APPEND);
			/******************************************************************************************************/
			$sk             = substr($sku, 0, strpos($sku, '-'));
			$country_dur_stdcode = explode("_",$sk);
			$country        = $country_dur_stdcode[0];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'country-'.$country.PHP_EOL, FILE_APPEND);
			$duration       = $country_dur_stdcode[1];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Duration-'.$duration.PHP_EOL, FILE_APPEND);

//			$get_dur_q = "select pack_validity from tsim_pack_details where pack_name='$sku' limit 1";

			$get_dur_q = "SELECT substring(validity from '\d+ Days') AS pack_validity FROM tsim_orders_line_items_test WHERE sku = '$sku' LIMIT 1";
			file_put_contents($log_file, date("Y-m-d H:i:s") . '- '.$get_dur_q.PHP_EOL, FILE_APPEND);
			$e_dur_q = pg_query($db,$get_dur_q);
			$num_dur_q = pg_num_rows($e_dur_q);
			file_put_contents($log_file, date("Y-m-d H:i:s") . ' Num rows - '.$num_dur_q.PHP_EOL, FILE_APPEND);
			if($num_dur_q > 0){
				$f_dur_q = pg_fetch_assoc($e_dur_q);
				$dur_array = explode(" ",$f_dur_q['pack_validity']);
				file_put_contents($log_file, date("Y-m-d H:i:s") . '- Days -> '.$dur_array[0].PHP_EOL, FILE_APPEND);
				$duration = $dur_array[0];
			}
			$array  = read_config_file('<!--SKU_STD_CODE','--SKU_STD_CODE>');
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Config File Read -'.serialize($array).PHP_EOL, FILE_APPEND);
			if($array[$sku])
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU Present For STD CODE-'.$array[$sku].PHP_EOL, FILE_APPEND);
				$std_code = $array[$sku];

			}
			else

				if (strpos($sku,'91')!= false && strpos($sku,'91_Data')== false)
				{
					$std_code = '91';
				}
				else if(strpos($sku,'91_Data')== true)
				{
					$std_code = '91_Data';
				}
				else
				{
					$std_code ="";
				}
			if (strpos($sku,'91')!== false || strpos($sku,'91_Data')!== false  || $array[$sku])
			{
				$today  =       date("Y-m-d H:i:s");
				$td     =       explode(" ",$today);
				$dur    =       $duration;
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Duration-'.$dur.PHP_EOL, FILE_APPEND);
				$future_date =  date('Y-m-d', strtotime("$today +360 days"));
				$max_from_date  =       $future_date." ".$td[1];
				$max_to_date    = date('Y-m-d', strtotime("$max_from_date +$dur days"));
				$max_to_date    = $max_to_date." ".$td[1];

			}
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'MAX SET to NULL'.PHP_EOL, FILE_APPEND);
				$max_from_date = null;
				$max_to_date = null;
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'std_code-'.$std_code.PHP_EOL, FILE_APPEND);
			for($i=0 ; $i<5 ;$i++)
			{
				if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'new-card-or-refill')
				{
					$order_type = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
					break;
				}
			}
			if ($order_type =="")
				$order_type='New';
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Order Type-'.$order_type.PHP_EOL, FILE_APPEND);
			if($order_type == "New")
			{
				if($role == 'reseller_customer')
				{
					$count_items = count($obj['order']['line_items']["$l"]['meta']);
					file_put_contents($log_file, date("Y-m-d H:i:s") . 'Total number of items in it are -'.$count_items.PHP_EOL, FILE_APPEND);
					file_put_contents($log_file, date("Y-m-d H:i:s") . '$ab -'.$ab.PHP_EOL, FILE_APPEND);
					for($ab = 0; $ab < $count_items; $ab++)
					{
						$key = $obj['order']['line_items']["$l"]['meta']["$ab"]['key'];
						file_put_contents($log_file, date("Y-m-d H:i:s") . 'Key is -'.$key.PHP_EOL, FILE_APPEND);
						#if($key == "Serial Number - $q" || $key =='Serial Number')
						if (substr($key,0,13) == 'Serial Number')
						{
							$serial_no = $obj['order']['line_items']["$l"]['meta']["$ab"]['value'];
							file_put_contents($log_file, date("Y-m-d H:i:s") . 'Serial No is -'.$serial_no.PHP_EOL, FILE_APPEND);
						}
					}
				}
				else
					$country_specific="UK Cheapest";
			}
			else if($order_type == "Refill")
			{
				for($i=0 ; $i<5 ;$i++)
				{
					if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'Current TSIM number')
					{
						$sim_phone_no = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
						break;
					}
				}
				if($sim_phone_no !="" || $sim_phone_no != NULL)
				{
					$sim_phone_no = $sim_phone_no;
				}
				else
				{
					$sim_phone_no = "Not Mentioned";
				}
			}
			else
			{
				$order_type = "";
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'country_specific-'.$country_specific.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . '***************Curl started****************'.PHP_EOL, FILE_APPEND);
			$param = "l=admin&p=chair&a=tsim_client&role=$role&order_type=$order_type&gift_item=$gift_item&uemail=$email&c_name=$clientname&username=$username&domainname=$domainname&company=$company&phone=$phone&c_add1=$clientadd1&c_add2=$clientadd2&c_address_city=$address_city&c_address_state=$address_state&c_address_country=$address_country&postcode=$postcode&shipping_add1=$shipping_add1&shipping_add2=$shipping_add2&shipping_city=$shipping_city&shipping_state=$shipping_state&shipping_country=$shipping_country&shipping_postcode=$shipping_postcode&start_date=$start_date&payment_mode=$payment_mode&orderid=$orderid&status=$status&amt=$total&link_amt=$link_amt&sub_total=$sub_total&subtotal=$subtotal&item_cost=$item_cost&total_tax=$total_tax&curr=$curr&sim_phone_no=$sim_phone_no&case_source=$case_source&sku=$sku&country=$country&country_specific=$country_specific&duration=$duration&std_code=$std_code&max_from_date=$max_from_date&max_to_date=$max_to_date&sk=$sk&delivery_method=$delivery_method&payment_id=$payment_id&l_cname=$l_clientname&l_cphone=$l_phone&serial_no=$serial_no&activation_date=$activationDate&gstin=$gstin&productname=$productname&is_kyc_required=$is_kyc_required";
			$ret_curl = curl_data($param);
		}
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .'only_TI----'.$only_TI.PHP_EOL, FILE_APPEND);
	if($only_TI == 1)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Only Medical Insurance----Create only User---'.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") . '**************Curl Started*****************'.PHP_EOL, FILE_APPEND);
		$param = "l=admin&p=chair&a=create_only_user&order_type=$order_type&gift_item=$gift_item&uemail=$email&c_name=$clientname&username=$username&domainname=$domainname&company=$company&phone=$phone&c_add1=$clientadd1&c_add2=$clientadd2&c_address_city=$address_city&c_address_state=$address_state&c_address_country=$address_country&postcode=$postcode&shipping_add1=$shipping_add1&shipping_add2=$shipping_add2&shipping_city=$shipping_city&shipping_state=$shipping_state&shipping_country=$shipping_country&shipping_postcode=$shipping_postcode&start_date=$start_date&payment_mode=$payment_mode&orderid=$orderid&status=$status&amt=$total&link_amt=$link_amt&sub_total=$sub_total&subtotal=$subtotal&item_cost=$item_cost&total_tax=$total_tax&curr=$curr&sim_phone_no=$sim_phone_no&case_source=$case_source&sku=$sku&country=$country&country_specific=$country_specific&duration=$duration&std_code=$std_code&max_from_date=$max_from_date&max_to_date=$max_to_date&sk=$sk&delivery_method=$delivery_method&payment_id=$payment_id&l_cname=$l_clientname&l_cphone=$l_phone&activation_date=$activationDate&gstin=$gstin&productname=$productname&is_kyc_required=$is_kyc_required";
		$ret_curl = curl_data($param);
	}
	else
	{
		send_mail($obj);
	}
	exit;
}
if($action == 'register')
{
	file_put_contents($log_file, date("Y-m-d H:i:s") . '------------------In side Register-------------------'.PHP_EOL, FILE_APPEND);
	global $orderid,$items,$email,$phone,$action,$first_name,$last_name,$clientname,$domainname,$company,$clientadd1,$clientadd2,$address_city,$address_state,$address_country,$postcode,$start_date,$payment_id,$status,$curr,$total,$sub_total,$total_shipping,$country_specific,$payment_mode,$link_amt,$total_tax,$username,$subtotal,$mnt,$gift_item,$note,$quantity,$sku,$item_cost,$sk,$country_dur_stdcode,$country,$duration,$std_code,$max_from_date,$max_to_date,$sim_phone_no,$order_type,$country_specific,$action,$shipping_add1,$shipping_add2,$shipping_city,$shipping_state,$shipping_country,$shipping_postcode,$flag_user,$client_mail,$db,$log_file,$first_name2,$last_name2,$clientname2,$l_clientname,$l_phone,$role,$is_kyc_required;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Role is -'.$role. PHP_EOL, FILE_APPEND);
	$update_user = "UPDATE tsim_simmis_map SET tsim_username ='$username' where order_no ='$orderid';";
	file_put_contents("$log_file",date("Y-m-d H:i:s").'update_user-'.$update_user . PHP_EOL, FILE_APPEND);
	$res = pg_query($db, $update_user);
	$items = count($obj['order']['line_items']);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'items->'.$items. PHP_EOL, FILE_APPEND);
	$ret_per_info = parse_personal_info($json);
	/*file_put_contents($log_file, date("Y-m-d H:i:s") . 'First Name-'.$first_name.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Last Name-'.$last_name.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Billing Client Name-'.$clientname.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'First Name-'.$first_name2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Last Name-'.$last_name2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Client Name-'.$clientname2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client Name-'.$l_clientname.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") .'l_phone-'.$l_phone.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Company-'.$company.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add1-'.$clientadd1.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add2-'.$clientadd2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_city -'.$address_city.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_state-'.$address_state.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_country-'.$address_country.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'postcode-'.$postcode.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add1-'.$shipping_add1.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add2-'.$shipping_add2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping City-'.$shipping_city.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_state-'.$shipping_state.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_country-'.$shipping_country.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping postcode-'.$shipping_postcode.PHP_EOL, FILE_APPEND);
	 */
	$start_date     = $obj['order']['created_at'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$start_date.PHP_EOL, FILE_APPEND);
	$start_date = date('Y-m-d H:i:s',strtotime($start_date));
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$start_date.PHP_EOL, FILE_APPEND);
	$payment_id     = $obj['order']['payment_details']['method_id'];
	$status         = $obj['order']['status'];
	$curr           = $obj['order']['currency'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Currency-'.$curr.PHP_EOL, FILE_APPEND);
	$total          = $obj['order']['total'];
	$sub_total      = $obj['order']['subtotal'];
	$total_shipping = $obj['order']['total_shipping'];
	$discount	= $obj['order']['coupon_lines']['0']['discount'];
	$is_kyc_required =  isset($obj['order']['is_kyc_required']) ? $obj['order']['is_kyc_required'] : null;

	$gstin1 = isset($obj['order']['customer']['gstin']) ? $obj['order']['customer']['gstin'] : null;
        $gstin1 = preg_replace("/[^A-Za-z0-9]/", "", $gstin1);


        if (strlen($gstin1) === 15) 
        {
                file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin1.PHP_EOL, FILE_APPEND);
                $gstin=$gstin1;
        }
        else if(strlen($gstin1) === 0)
        {
		file_put_contents($log_file, date("Y-m-d H:i:s") . ' GST IN  Field is Empty-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

        }
        else
        {
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN is INVALID-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

        }



	file_put_contents($log_file, date("Y-m-d H:i:s") . ' GST IN -'.$gstin.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . ' is_kyc_required field:  '.$is_kyc_required.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . ' Discount-'.$discount.PHP_EOL, FILE_APPEND);
	$delivery_method = $obj['order']['shipping_methods'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Delivery Method-'.$delivery_method.PHP_EOL, FILE_APPEND);
	if($total_shipping == "")
		$total_shipping=0;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'payment id-'.$payment_id.PHP_EOL, FILE_APPEND);
	if( $payment_id == "payuindia" || $payment_id == "pwa" )
	{
		$payment_mode = "Credit Card" ;
	}
	else if( $payment_id == "bacs" || $payment_id == "cod" )
	{
		$payment_mode = "NEFT/IMPS Payment";
	}
	else if( $payment_id == "Prepaid"  || $payment_id == "other_payment")
	{
		$payment_mode = "PAD";
		#$link_amt = $sub_total + $total_shipping;
		$link_amt = $total;
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Payment Mode-'.$payment_mode.PHP_EOL, FILE_APPEND);
	$total          = $obj['order']['total'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'total-'.$total.PHP_EOL, FILE_APPEND);
	$total_tax      = $obj['order']['total_tax'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'total_tax-'.$total_tax.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'link_amt-'.$link_amt.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'sub_total-'.$sub_total.PHP_EOL, FILE_APPEND);
	
	$currencyforcasesource = $obj['order']['currency'];
        if($currencyforcasesource == "INR") {
                 $case_source    = "tsim.in";
        } else{
                 $case_source    = "tsim.mobi";
        }

	//$case_source    = "tsim.in";
       /* if(empty($username))
	{
	       $uname=explode("@",$email);
	       $username = $uname[0];
	}
	else
	{
	      $username = $obj['order']['customer']['username'];
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Username-'.$username.PHP_EOL, FILE_APPEND);*/
	$productname = $obj['order']['line_items'][0]['name'];

	$mnt = count($obj['order']['line_items']);
	for($i=0; $i<$mnt;$i++)
	{
		if($obj['order']['line_items']["$i"]['meta']['0']['value'] == 'Free Item')
		{
			$gift_item = $obj['order']['line_items']["$i"]['name'];
			break;
		}
		else
			$gift_item="TSIM Booking";
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Gift Item-'.$gift_item.PHP_EOL, FILE_APPEND);
	$note           =       $obj['order']['note'];
	$note           = trim($note);
	//$note           = str_replace(' ', '', $note);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'note-'.$note.PHP_EOL, FILE_APPEND);
	for( $l=0; $l<$items;$l++)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s").'-> l is-'.$l.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Line Items-'.$items.PHP_EOL, FILE_APPEND);
		$quantity       = $obj['order']['line_items']["$l"]['quantity'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Quantity-'.$quantity.PHP_EOL, FILE_APPEND);
		for($q=1; $q<=$quantity;$q++)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s").'->l is-'.$l.'-> q is-'.$q.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . '********item starts********'.PHP_EOL, FILE_APPEND);
			$sku            = $obj['order']['line_items']["$l"]['sku'];
			$meta_arr = $obj['order']['line_items'][$l]['meta'];
			$activationDate =  null;
			foreach($meta_arr as $v)
			{
				#if($v['key']==='Activation Date - 1' || $v['key']==='Activation Date')
				if(substr($v['key'],0,15) == 'Activation Date')
				{
					$activationDate =  $v['value'];

				}

			}

			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Activation Date-'.$activationDate.PHP_EOL, FILE_APPEND);

file_put_contents($log_file, date("Y-m-d H:i:s") . ' Activation Date-' . $activationDate . PHP_EOL, FILE_APPEND);

// Log the original date first
file_put_contents($log_file, "Original Activation Date: " . $activationDate . PHP_EOL, FILE_APPEND);

// Ensure the date is not null
if ($activationDate !== null) {
    // Try parsing the date using the 'd-m-Y' format (for a 4-digit year)
    $dateObj = DateTime::createFromFormat('d-m-Y', $activationDate);

    // Check for parsing errors
    if ($dateObj && $dateObj->format('d-m-Y') === $activationDate) {
        // Success: convert to 'Y-m-d H:i:s' format
        $activationDate = $dateObj->format('Y-m-d H:i:s');
    } else {
        // Parsing failed or format mismatch: log error and handle gracefully
        file_put_contents($log_file, "Error: Invalid date format or failed conversion for Activation Date." . PHP_EOL, FILE_APPEND);
        // Handle the error (skip, throw exception, or set a default date)
        // Example: throw an exception or return a fallback value
        throw new Exception("Date format is invalid: expected d-m-Y, got " . $activationDate);
    }
}

// Log the final activation date
file_put_contents($log_file, "Final Activation Date: " . $activationDate . PHP_EOL, FILE_APPEND);


















			/*
			if($activationDate !== null)
			{
				//$activationDate = DateTime::createFromFormat('d-m-y', $activationDate)->format('Y-m-d H:i:s');
				$activationDate = date('Y-m-d H:i:s',strtotime($activationDate));
			}*/

			// $activationDate = date('Y-m-d H:i:s',strtotime($activationDate));
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$activationDate.PHP_EOL, FILE_APPEND);

			file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU-'.$sku.PHP_EOL, FILE_APPEND);
			/********************Checking For item is Free Item or Not************************************/
			for($z=0; $z<5;$z++)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.'-q is-'.$q.'-z is -'.$z.PHP_EOL, FILE_APPEND);
				if($obj['order']['line_items']["$l"]['meta']["$z"]['value'] == 'Free Item')
				{
					file_put_contents($log_file, date("Y-m-d H:i:s") . '*******Free Item->Skip********'.PHP_EOL, FILE_APPEND);
					continue 3;
				}
				$itm_type = $obj['order']['line_items']["$l"]['name'];
				file_put_contents($log_file, date("Y-m-d H:i:s").'Item Type-'.$itm_type.PHP_EOL, FILE_APPEND);
				if(strstr($obj['order']['line_items']["$l"]['name'],"Insurance"))
				{
					file_put_contents($log_file, date("Y-m-d H:i:s").'Insurance Item-Skip'.PHP_EOL, FILE_APPEND);
					continue 3;
				}
			}
			/******************************Check for travel Insurance*******************************************/
			$sku_chk = "select * from sku_not_to_map where sku='$sku'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$sku_chk.PHP_EOL, FILE_APPEND);
			$res_sku_chk = pg_query($db, $sku_chk);
			if(!$res_sku_chk)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Failed to execute-Exit'.PHP_EOL, FILE_APPEND);
				exit;
			}
			$num_sku_chk = pg_num_rows($res_sku_chk);
			if($num_sku_chk > 0)
			{
				$only_TI = 1;
				continue;
			}
			else
			{
				$only_TI = 0;
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") .'only_TI----'.$only_TI.PHP_EOL, FILE_APPEND);
			/********************************************************************************************************/	
			$item_total      = $obj['order']['line_items']["$l"]['total'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Item Total-'.$item_total.PHP_EOL, FILE_APPEND);
			//file_put_contents($log_file, date("Y-m-d H:i:s") . 'case source-'.$case_source.PHP_EOL, FILE_APPEND);
			$subtotal      = $obj['order']['line_items']["$l"]['subtotal'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'subtotal-'.$subtotal.PHP_EOL, FILE_APPEND);
			$subtotal      = $subtotal/$quantity;
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Individual amt for entry-'.$subtotal.PHP_EOL, FILE_APPEND);
			$item_cost      = $obj['order']['line_items']["$l"]['price'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'item_cost-'.$item_cost.PHP_EOL, FILE_APPEND);
			$sk             = substr($sku, 0, strpos($sku, '-'));

//			$get_dur_q = "select pack_validity from tsim_pack_details where pack_name='$sku' limit 1";
		
			$get_dur_q = "SELECT substring(validity from '\d+ Days') AS pack_validity FROM tsim_orders_line_items_test WHERE sku = '$sku' LIMIT 1";

			file_put_contents($log_file, date("Y-m-d H:i:s") . '- '.$get_dur_q.PHP_EOL, FILE_APPEND);
			$e_dur_q = pg_query($db,$get_dur_q);
			$num_dur_q = pg_num_rows($e_dur_q);
			file_put_contents($log_file, date("Y-m-d H:i:s") . ' Num rows - '.$num_dur_q.PHP_EOL, FILE_APPEND);
			$country_dur_stdcode = explode("_",$sk);
			$country        = $country_dur_stdcode[0];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'country-'.$country.PHP_EOL, FILE_APPEND);
			$duration       = $country_dur_stdcode[1];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Duration-'.$duration.PHP_EOL, FILE_APPEND);
			if($num_dur_q > 0){
				$f_dur_q = pg_fetch_assoc($e_dur_q);
				$dur_array = explode(" ",$f_dur_q['pack_validity']);
				file_put_contents($log_file, date("Y-m-d H:i:s") . '- Days -> '.$dur_array[0].PHP_EOL, FILE_APPEND);
				$duration = $dur_array[0];
			}
			$array  = read_config_file('<!--SKU_STD_CODE','--SKU_STD_CODE>');
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Config File Read -'.serialize($array).PHP_EOL, FILE_APPEND);
			if($array[$sku])
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU Present For STD CODE-'.$array[$sku].PHP_EOL, FILE_APPEND);
				$std_code = $array[$sku];

			}
			else
				if (strpos($sku,'91')!= false && strpos($sku,'91_Data')== false)
				{
					$std_code = '91';
				}
				else if(strpos($sku,'91_Data')== true)
				{
					$std_code = '91_Data';
				}
				else
				{
					$std_code ="";
				}
			if (strpos($sku,'91')!== false || strpos($sku,'91_Data')!== false ||  $array[$sku])
			{
				$today  =       date("Y-m-d H:i:s");
				$td     =       explode(" ",$today);
				$dur    =       $duration;
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Duration-'.$dur.PHP_EOL, FILE_APPEND);
				$future_date =  date('Y-m-d', strtotime("$today +360 days"));
				$max_from_date  =       $future_date." ".$td[1];
				$max_to_date    = date('Y-m-d', strtotime("$max_from_date +$dur days"));
				$max_to_date    = $max_to_date." ".$td[1] ."\n";
				$max_to_date    = rtrim($max_to_date);
			}
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'MAX SET to NULL'.PHP_EOL, FILE_APPEND);
				$max_from_date=null;
				$max_to_date=null;
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'std_code-'.$std_code.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'from date-'.$max_from_date.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'to date-'.$max_to_date.PHP_EOL, FILE_APPEND);
			for($i=0 ; $i<5 ;$i++)
			{
				if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'new-card-or-refill')
				{
					$order_type = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
					break;
				}
			}
			if ($order_type =="")
				$order_type='New';
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Order Type-'.$order_type.PHP_EOL, FILE_APPEND);
			if($order_type == "New")
			{
				/*for($i=0 ; $i<5 ;$i++)
				{
					if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'number')
					{
					      if($obj['order']['line_items']["$l"]['meta']["$i"]['value'] != "UK (Cheapest Option)")
					      {
						      $country_specific = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
						      break;
					      }
					      else if(empty($obj['order']['line_items']["$l"]['meta']["$i"]['value']) != null)
					      {
							$country_specific="UK Cheapest";
					      }
				      }
				      else
				      {
					      $country_specific="UK Cheapest";
				      }
				}
	 */
	if($role == 'reseller_customer')
	{
		$count_items = count($obj['order']['line_items']["$l"]['meta']);
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Total number of items in it are -'.$count_items.PHP_EOL, FILE_APPEND);
		for($ab = 0; $ab < $count_items; $ab++)
		{
			$key = $obj['order']['line_items']["$l"]['meta']["$ab"]['key'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Key is -'.$key.PHP_EOL, FILE_APPEND);
			#if($key == "Serial Number - $q" || $key=='Serial Number')
			if (substr($key,0,13) == 'Serial Number')
			{
				$serial_no = $obj['order']['line_items']["$l"]['meta']["$ab"]['value'];
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Serial No is -'.$serial_no.PHP_EOL, FILE_APPEND);
			}
		}
	}
	else
		$country_specific="UK Cheapest";
			}
			else if($order_type == "Refill")
			{
				for($i=0 ; $i<5 ;$i++)
				{
					if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'Current TSIM number')
					{
						$sim_phone_no = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
						break;
					}
				}
				if($sim_phone_no !="" || $sim_phone_no != NULL)
				{
					$sim_phone_no = $sim_phone_no;
				}
				else
				{
					$sim_phone_no = "Not Mentioned";
				}
			}
			else
			{
				$order_type = "";
			}

			file_put_contents($log_file, date("Y-m-d H:i:s") . 'country_specific-'.$country_specific.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . '**************Curl Started*****************'.PHP_EOL, FILE_APPEND);
			$param = "l=admin&p=chair&a=tsim_client&role=$role&order_type=$order_type&gift_item=$gift_item&uemail=$email&c_name=$clientname&username=$username&domainname=$domainname&company=$company&phone=$phone&c_add1=$clientadd1&c_add2=$clientadd2&c_address_city=$address_city&c_address_state=$address_state&c_address_country=$address_country&postcode=$postcode&shipping_add1=$shipping_add1&shipping_add2=$shipping_add2&shipping_city=$shipping_city&shipping_state=$shipping_state&shipping_country=$shipping_country&shipping_postcode=$shipping_postcode&start_date=$start_date&payment_mode=$payment_mode&orderid=$orderid&status=$status&amt=$total&link_amt=$link_amt&sub_total=$sub_total&subtotal=$subtotal&item_cost=$item_cost&total_tax=$total_tax&curr=$curr&sim_phone_no=$sim_phone_no&case_source=$case_source&sku=$sku&country=$country&country_specific=$country_specific&duration=$duration&std_code=$std_code&max_from_date=$max_from_date&max_to_date=$max_to_date&sk=$sk&delivery_method=$delivery_method&payment_id=$payment_id&l_cname=$l_clientname&l_cphone=$l_phone&serial_no=$serial_no&activation_date=$activationDate&gstin=$gstin&productname=$productname&is_kyc_required=$is_kyc_required";
			$ret_curl = curl_data($param);
		}
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .'only_TI----'.$only_TI.PHP_EOL, FILE_APPEND);
	if($only_TI == 1)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Only Medical Insurance----Create only User---'.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") . '**************Curl Started*****************'.PHP_EOL, FILE_APPEND);
		$param = "l=admin&p=chair&a=create_only_user&order_type=$order_type&gift_item=$gift_item&uemail=$email&c_name=$clientname&username=$username&domainname=$domainname&company=$company&phone=$phone&c_add1=$clientadd1&c_add2=$clientadd2&c_address_city=$address_city&c_address_state=$address_state&c_address_country=$address_country&postcode=$postcode&shipping_add1=$shipping_add1&shipping_add2=$shipping_add2&shipping_city=$shipping_city&shipping_state=$shipping_state&shipping_country=$shipping_country&shipping_postcode=$shipping_postcode&start_date=$start_date&payment_mode=$payment_mode&orderid=$orderid&status=$status&amt=$total&link_amt=$link_amt&sub_total=$sub_total&subtotal=$subtotal&item_cost=$item_cost&total_tax=$total_tax&curr=$curr&sim_phone_no=$sim_phone_no&case_source=$case_source&sku=$sku&country=$country&country_specific=$country_specific&duration=$duration&std_code=$std_code&max_from_date=$max_from_date&max_to_date=$max_to_date&sk=$sk&delivery_method=$delivery_method&payment_id=$payment_id&l_cname=$l_clientname&l_cphone=$l_phone&activation_date=$activationDate&gstin=$gstin&productname=$productname&is_kyc_required=$is_kyc_required";
		$ret_curl = curl_data($param);
	}
	else
	{
		send_mail($obj);
	}
	exit;
}
else if($action == 'user_exists')
{
	file_put_contents($log_file, date("Y-m-d H:i:s") . '-----------------In side user_exists--------------------'.PHP_EOL, FILE_APPEND);
	global $orderid,$items,$email,$phone,$action,$first_name,$last_name,$clientname,$domainname,$company,$clientadd1,$clientadd2,$address_city,$address_state,$address_country,$postcode,$start_date,$payment_id,$status,$curr,$total,$sub_total,$total_shipping,$country_specific,$payment_mode,$link_amt,$total_tax,$username,$subtotal,$mnt,$gift_item,$note,$quantity,$sku,$item_cost,$sk,$country_dur_stdcode,$country,$duration,$std_code,$max_from_date,$max_to_date,$sim_phone_no,$order_type,$country_specific,$action,$shipping_add1,$shipping_add2,$shipping_city,$shipping_state,$shipping_country,$shipping_postcode,$flag_user,$client_mail,$db,$log_file,$first_name2,$last_name2,$clientname2,$l_clientname,$l_phone,$role,$is_kyc_required;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Role is -'.$role. PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Username->'.$username. PHP_EOL, FILE_APPEND);
	$items = count($obj['order']['line_items']);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'items->'.$items. PHP_EOL, FILE_APPEND);
	$ret_per_info = parse_personal_info($json);
	/*file_put_contents($log_file, date("Y-m-d H:i:s") . 'First Name-'.$first_name.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Last Name-'.$last_name.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Billing Client Name-'.$clientname.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'First Name-'.$first_name2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Last Name-'.$last_name2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Client Name-'.$clientname2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client Name-'.$l_clientname.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") .'l_phone-'.$l_phone.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Company-'.$company.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add1-'.$clientadd1.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add2-'.$clientadd2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_city -'.$address_city.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_state-'.$address_state.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_country-'.$address_country.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'postcode-'.$postcode.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add1-'.$shipping_add1.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add2-'.$shipping_add2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping City-'.$shipping_city.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_state-'.$shipping_state.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_country-'.$shipping_country.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping postcode-'.$shipping_postcode.PHP_EOL, FILE_APPEND);
	 */	
	$start_date     = $obj['order']['created_at'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$start_date.PHP_EOL, FILE_APPEND);
	$start_date = date('Y-m-d H:i:s',strtotime($start_date));
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$start_date.PHP_EOL, FILE_APPEND);
	$payment_id     = $obj['order']['payment_details']['method_id'];
	$status         = $obj['order']['status'];
	$curr           = $obj['order']['currency'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Currency-'.$curr.PHP_EOL, FILE_APPEND);
	$total          = $obj['order']['total'];
	$sub_total      = $obj['order']['subtotal'];
	$total_shipping = $obj['order']['total_shipping'];
	$discount       = $obj['order']['coupon_lines']['0']['discount'];
	$is_kyc_required =  isset($obj['order']['is_kyc_required']) ? $obj['order']['is_kyc_required'] : null;
	
	$gstin1 = isset($obj['order']['customer']['gstin']) ? $obj['order']['customer']['gstin'] : null;
        $gstin1 = preg_replace("/[^A-Za-z0-9]/", "", $gstin1);


        if (strlen($gstin1) === 15)
        {
                file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin1.PHP_EOL, FILE_APPEND);
                $gstin=$gstin1;
        }
        else if(strlen($gstin1) === 0)
        {
		file_put_contents($log_file, date("Y-m-d H:i:s") . ' GST IN  Field is Empty-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

        }
        else
        {
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN is INVALID-'.$gstin1.PHP_EOL, FILE_APPEND);
		 $gstin=$gstin1;

        }



	file_put_contents($log_file, date("Y-m-d H:i:s") . 'GST IN -'.$gstin.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Discount-'.$discount.PHP_EOL, FILE_APPEND);
	if($total_shipping == "")
		$total_shipping=0;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'payment id-'.$payment_id.PHP_EOL, FILE_APPEND);
	$delivery_method = $obj['order']['shipping_methods'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Delivery Method-'.$delivery_method.PHP_EOL, FILE_APPEND);
	if( $payment_id == "payuindia" || $payment_id == "pwa" )
	{
		$payment_mode = "Credit Card" ;
	}
	else if( $payment_id == "bacs" || $payment_id == "cod" )
	{
		$payment_mode = "NEFT/IMPS Payment";
	}
	else if( $payment_id == "Prepaid" || $payment_id == "other_payment")
	{
		$payment_mode = "PAD";
		#$link_amt = $sub_total + $total_shipping;
		$link_amt = $total;
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Payment Mode-'.$payment_mode.PHP_EOL, FILE_APPEND);
	$total          = $obj['order']['total'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'total-'.$total.PHP_EOL, FILE_APPEND);
	$total_tax      = $obj['order']['total_tax'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'total_tax-'.$total_tax.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'link_amt-'.$link_amt.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'sub_total-'.$sub_total.PHP_EOL, FILE_APPEND);

	$currencyforcasesource = $obj['order']['currency'];
        if($currencyforcasesource == "INR") {
                 $case_source    = "tsim.in";
        } else{
                 $case_source    = "tsim.mobi";
        }

	//$case_source    = "tsim.in";
	$productname = $obj['order']['line_items'][0]['name'];

	$mnt = count($obj['order']['line_items']);
	for($i=0; $i<$mnt;$i++)
	{
		if($obj['order']['line_items']["$i"]['meta']['0']['value'] == 'Free Item')
		{
			$gift_item = $obj['order']['line_items']["$i"]['name'];
			break;
		}
		else
			$gift_item="TSIM Booking";
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Gift Item-'.$gift_item.PHP_EOL, FILE_APPEND);
	$note           =       $obj['order']['note'];
	$note           = trim($note);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'note-'.$note.PHP_EOL, FILE_APPEND);
	for( $l=0; $l<$items;$l++)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s").'-> l is-'.$l.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Line Items-'.$items.PHP_EOL, FILE_APPEND);
		$quantity       = $obj['order']['line_items']["$l"]['quantity'];
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Quantity-'.$quantity.PHP_EOL, FILE_APPEND);
		for($q=1; $q<=$quantity;$q++)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s").'->l is-'.$l.'-> q is-'.$q.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . '********item starts********'.PHP_EOL, FILE_APPEND);
			/********************Checking For item is Free Item or Not************************************/
			for($z=0; $z<5;$z++)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.'-q is-'.$q.'-z is -'.$z.PHP_EOL, FILE_APPEND);
				if($obj['order']['line_items']["$l"]['meta']["$z"]['value'] == 'Free Item')
				{
					file_put_contents($log_file, date("Y-m-d H:i:s") . '*******Free Item->Skip********'.PHP_EOL, FILE_APPEND);
					continue 3;
				}
				$itm_type = $obj['order']['line_items']["$l"]['name'];
				file_put_contents($log_file, date("Y-m-d H:i:s").'Item Type-'.$itm_type.PHP_EOL, FILE_APPEND);
				if(strstr($obj['order']['line_items']["$l"]['name'],"Insurance"))
				{
					file_put_contents($log_file, date("Y-m-d H:i:s").'Insurance Item-Skip'.PHP_EOL, FILE_APPEND);
					continue 3;
				}
			}
			$sku            = $obj['order']['line_items']["$l"]['sku'];

			$meta_arr = $obj['order']['line_items'][$l]['meta'];
			$activationDate =  null;
			foreach($meta_arr as $v)
			{
				#if($v['key']==='Activation Date - 1' || $v['key']==='Activation Date')
				if(substr($v['key'],0,15) == 'Activation Date')
				{
					$activationDate =  $v['value'];

				}

			}

			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Activation Date-'.$activationDate.PHP_EOL, FILE_APPEND);


file_put_contents($log_file, date("Y-m-d H:i:s") . ' Activation Date-' . $activationDate . PHP_EOL, FILE_APPEND);

// Log the original date first
file_put_contents($log_file, "Original Activation Date: " . $activationDate . PHP_EOL, FILE_APPEND);

// Ensure the date is not null
if ($activationDate !== null) {
    // Try parsing the date using the 'd-m-Y' format (for a 4-digit year)
    $dateObj = DateTime::createFromFormat('d-m-Y', $activationDate);

    // Check for parsing errors
    if ($dateObj && $dateObj->format('d-m-Y') === $activationDate) {
        // Success: convert to 'Y-m-d H:i:s' format
        $activationDate = $dateObj->format('Y-m-d H:i:s');
    } else {
        // Parsing failed or format mismatch: log error and handle gracefully
        file_put_contents($log_file, "Error: Invalid date format or failed conversion for Activation Date." . PHP_EOL, FILE_APPEND);
        // Handle the error (skip, throw exception, or set a default date)
        // Example: throw an exception or return a fallback value
        throw new Exception("Date format is invalid: expected d-m-Y, got " . $activationDate);
    }
}

// Log the final activation date
file_put_contents($log_file, "Final Activation Date: " . $activationDate . PHP_EOL, FILE_APPEND);







			/*
			if($activationDate !== null)
			{
			//	 $activationDate = DateTime::createFromFormat('d-m-y', $activationDate)->format('Y-m-d H:i:s');
				$activationDate = date('Y-m-d H:i:s',strtotime($activationDate));
			}*/

			// $activationDate = date('Y-m-d H:i:s',strtotime($activationDate));
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'start_date-'.$activationDate.PHP_EOL, FILE_APPEND);

			file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU-'.$sku.PHP_EOL, FILE_APPEND);
			/******************************Check for travel Insurance*******************************************/
			$sku_chk = "select * from sku_not_to_map where sku='$sku'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$sku_chk.PHP_EOL, FILE_APPEND);
			$res_sku_chk = pg_query($db, $sku_chk);
			if(!$res_sku_chk)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Failed to execute-Exit'.PHP_EOL, FILE_APPEND);
				exit;
			}
			$num_sku_chk = pg_num_rows($res_sku_chk);
			if($num_sku_chk > 0)
			{
				$only_TI = 1;
				continue;
			}
			else
			{
				$only_TI = 0;
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") .'only_TI----'.$only_TI.PHP_EOL, FILE_APPEND);
			/********************************************************************************************************/
			$item_total      = $obj['order']['line_items']["$l"]['total'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Item Total-'.$item_total.PHP_EOL, FILE_APPEND);
			//file_put_contents($log_file, date("Y-m-d H:i:s") . 'case source-'.$case_source.PHP_EOL, FILE_APPEND);
			$subtotal      = $obj['order']['line_items']["$l"]['subtotal'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'subtotal-'.$subtotal.PHP_EOL, FILE_APPEND);
			$subtotal      = $subtotal/$quantity;
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Individual amt for entry-'.$subtotal.PHP_EOL, FILE_APPEND);
			$item_cost      = $obj['order']['line_items']["$l"]['price'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'item_cost-'.$item_cost.PHP_EOL, FILE_APPEND);
			$sk             = substr($sku, 0, strpos($sku, '-'));

			//$get_dur_q = "select pack_validity from tsim_pack_details where pack_name='$sku' limit 1";

			$get_dur_q = "SELECT substring(validity from '\d+ Days') AS pack_validity FROM tsim_orders_line_items_test WHERE sku = '$sku' LIMIT 1";
			file_put_contents($log_file, date("Y-m-d H:i:s") . '- '.$get_dur_q.PHP_EOL, FILE_APPEND);
			$e_dur_q = pg_query($db,$get_dur_q);
			$num_dur_q = pg_num_rows($e_dur_q);
			file_put_contents($log_file, date("Y-m-d H:i:s") . ' Num rows - '.$num_dur_q.PHP_EOL, FILE_APPEND);

			$country_dur_stdcode = explode("_",$sk);
			$country        = $country_dur_stdcode[0];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'country-'.$country.PHP_EOL, FILE_APPEND);
			$duration       = $country_dur_stdcode[1];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Duration-'.$duration.PHP_EOL, FILE_APPEND);
			if($num_dur_q > 0){
				$f_dur_q = pg_fetch_assoc($e_dur_q);
				$dur_array = explode(" ",$f_dur_q['pack_validity']);
				file_put_contents($log_file, date("Y-m-d H:i:s") . '- Days -> '.$dur_array[0].PHP_EOL, FILE_APPEND);
				$duration = $dur_array[0];
			}
			$array  = read_config_file('<!--SKU_STD_CODE','--SKU_STD_CODE>');
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Config File Read -'.serialize($array).PHP_EOL, FILE_APPEND);

			if($array[$sku])
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU Present For STD CODE-'.$array[$sku].PHP_EOL, FILE_APPEND);
				$std_code = $array[$sku];

			}
			else
				if (strpos($sku,'91')!= false && strpos($sku,'91_Data')== false)
				{
					$std_code = '91';
				}
				else if(strpos($sku,'91_Data')== true)
				{
					$std_code = '91_Data';
				}
				else
				{
					$std_code ="";
				}
			if (strpos($sku,'91')!== false || strpos($sku,'91_Data')!== false || $array[$sku])
			{
				$today  =       date("Y-m-d H:i:s");
				$td     =       explode(" ",$today);
				$dur    =       $duration;
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Duration-'.$dur.PHP_EOL, FILE_APPEND);
				$future_date =  date('Y-m-d', strtotime("$today +360 days"));
				$max_from_date  =       $future_date." ".$td[1];
				$max_to_date    = date('Y-m-d', strtotime("$max_from_date +$dur days"));
				$max_to_date    = $max_to_date." ".$td[1] ."\n";
				$max_to_date    = rtrim($max_to_date);
			}
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'MAX SET to NULL'.PHP_EOL, FILE_APPEND);
				$max_from_date=null;
				$max_to_date=null;
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'std_code-'.$std_code.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'from date-'.$max_from_date.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'to date-'.$max_to_date.PHP_EOL, FILE_APPEND);
			for($i=0 ; $i<5 ;$i++)
			{
				if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'new-card-or-refill')
				{
					$order_type = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
					break;
				}
			}
			if ($order_type =="")
				$order_type='New';
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Order Type-'.$order_type.PHP_EOL, FILE_APPEND);
			if($order_type == "New")
			{
				/*for($i=0 ; $i<5 ;$i++)
				{
					if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'number')
					{
					      if($obj['order']['line_items']["$l"]['meta']["$i"]['value'] != "UK (Cheapest Option)")
					      {
						      $country_specific = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
						      break;
					      }
					      else if(empty($obj['order']['line_items']["$l"]['meta']["$i"]['value']) != null)
					      {
							$country_specific="UK Cheapest";
					      }
				      }
				      else
				      {
					      $country_specific="UK Cheapest";
				      }
				}
	 */
	if($role == 'reseller_customer')
	{
		$count_items = count($obj['order']['line_items']["$l"]['meta']);
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Total number of items in it are -'.$count_items.PHP_EOL, FILE_APPEND);
		for($ab = 0; $ab < $count_items; $ab++)
		{
			$key = $obj['order']['line_items']["$l"]['meta']["$ab"]['key'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Key is -'.$key.PHP_EOL, FILE_APPEND);
			#if($key == "Serial Number - $q" || $key=='Serial Number')
			if (substr($key,0,13) == 'Serial Number')
			{
				$serial_no = $obj['order']['line_items']["$l"]['meta']["$ab"]['value'];
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Serial No is -'.$serial_no.PHP_EOL, FILE_APPEND);
			}
		}
	}
	else
		$country_specific="UK Cheapest";
			}
			else if($order_type == "Refill")
			{
				for($i=0 ; $i<5 ;$i++)
				{
					if($obj['order']['line_items']["$l"]['meta']["$i"]['key'] == 'Current TSIM number')
					{
						$sim_phone_no = $obj['order']['line_items']["$l"]['meta']["$i"]['value'];
						break;
					}
				}
				if($sim_phone_no !="" || $sim_phone_no != NULL)
				{
					$sim_phone_no = $sim_phone_no;
				}
				else
				{
					$sim_phone_no = "Not Mentioned";
				}
			}
			else
			{
				$order_type = "";
			}
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'country_specific-'.$country_specific.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . '**************Curl Started*****************'.PHP_EOL, FILE_APPEND);
			$param = "l=admin&p=chair&a=tsim_client&role=$role&order_type=$order_type&gift_item=$gift_item&uemail=$email&c_name=$clientname&username=$username&domainname=$domainname&company=$company&phone=$phone&c_add1=$clientadd1&c_add2=$clientadd2&c_address_city=$address_city&c_address_state=$address_state&c_address_country=$address_country&postcode=$postcode&shipping_add1=$shipping_add1&shipping_add2=$shipping_add2&shipping_city=$shipping_city&shipping_state=$shipping_state&shipping_country=$shipping_country&shipping_postcode=$shipping_postcode&start_date=$start_date&payment_mode=$payment_mode&orderid=$orderid&status=$status&amt=$total&link_amt=$link_amt&sub_total=$sub_total&subtotal=$subtotal&item_cost=$item_cost&total_tax=$total_tax&curr=$curr&sim_phone_no=$sim_phone_no&case_source=$case_source&sku=$sku&country=$country&country_specific=$country_specific&duration=$duration&std_code=$std_code&max_from_date=$max_from_date&max_to_date=$max_to_date&sk=$sk&delivery_method=$delivery_method&payment_id=$payment_id&l_cname=$l_clientname&l_cphone=$l_phone&serial_no=$serial_no&activation_date=$activationDate&gstin=$gstin&productname=$productname&is_kyc_required=$is_kyc_required";
			$ret_curl = curl_data($param);
		}
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .'only_TI----'.$only_TI.PHP_EOL, FILE_APPEND);
	if($only_TI == 1)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Only Medical Insurance----Create only User---'.PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") . '**************Curl Started*****************'.PHP_EOL, FILE_APPEND);
		$param = "l=admin&p=chair&a=create_only_user&order_type=$order_type&gift_item=$gift_item&uemail=$email&c_name=$clientname&username=$username&domainname=$domainname&company=$company&phone=$phone&c_add1=$clientadd1&c_add2=$clientadd2&c_address_city=$address_city&c_address_state=$address_state&c_address_country=$address_country&postcode=$postcode&shipping_add1=$shipping_add1&shipping_add2=$shipping_add2&shipping_city=$shipping_city&shipping_state=$shipping_state&shipping_country=$shipping_country&shipping_postcode=$shipping_postcode&start_date=$start_date&payment_mode=$payment_mode&orderid=$orderid&status=$status&amt=$total&link_amt=$link_amt&sub_total=$sub_total&subtotal=$subtotal&item_cost=$item_cost&total_tax=$total_tax&curr=$curr&sim_phone_no=$sim_phone_no&case_source=$case_source&sku=$sku&country=$country&country_specific=$country_specific&duration=$duration&std_code=$std_code&max_from_date=$max_from_date&max_to_date=$max_to_date&sk=$sk&delivery_method=$delivery_method&payment_id=$payment_id&l_cname=$l_clientname&l_cphone=$l_phone&activation_date=$activationDate&gstin=$gstin&productname=$productname&is_kyc_required=$is_kyc_required";
		$ret_curl = curl_data($param);
	}
	else
	{
		send_mail($obj);
	}
	exit;
}
else
{
	exit;
}

// Function to create a duplicate entry
function createDuplicateEntry($db, $orderid, $log_file) {

    pg_query($db, "BEGIN");

    try {
        // Insert the values using INSERT ... SELECT
        $dupli_check = "INSERT INTO tsim_order_deduplication(order_no, id) 
                        SELECT order_no, id 
                        FROM tsim_orders_line_items 
                        WHERE order_no='$orderid';";

        $dupli_check_res = pg_query($db, $dupli_check);

        // Log the result of the insert operation
        if ($dupli_check_res) {
            file_put_contents($log_file, date("Y-m-d H:i:s") . ' Duplicate entry created for order: ' . $orderid . PHP_EOL, FILE_APPEND);
            pg_query($db, "COMMIT");
            return true;
        } else {
            file_put_contents($log_file, date("Y-m-d H:i:s") . ' Insert failed for order ' . $orderid . ': ' . pg_last_error($db) . PHP_EOL, FILE_APPEND);
            pg_query($db, "ROLLBACK");
            return false;
        }
    } catch (Exception $e) {
        file_put_contents($log_file, date("Y-m-d H:i:s") . ' Error during duplicate entry creation: ' . $e->getMessage() . PHP_EOL, FILE_APPEND);
        pg_query($db, "ROLLBACK");
        return false;
    }
}
//

function checkDuplicateEntry($db, $orderid, $log_file) {
    //Funtion to check whether entry is present
    pg_query($db, "BEGIN");

    try {
        $dupli_check_query = "SELECT 1 FROM tsim_order_deduplication WHERE order_no = '$orderid' FOR UPDATE;";
        $dupli_check_res = pg_query($db, $dupli_check_query);

        if ($dupli_check_res) {
            if (pg_fetch_row($dupli_check_res)) {
                file_put_contents($log_file, date("Y-m-d H:i:s") . ' Duplicate entry found for order: ' . $orderid . PHP_EOL, FILE_APPEND);
                pg_query($db, "COMMIT");
                return true;
            } else {
                file_put_contents($log_file, date("Y-m-d H:i:s") . ' No duplicate entry found for order: ' . $orderid . PHP_EOL, FILE_APPEND);
                pg_query($db, "COMMIT");
                return false;
            }
        } else {
            file_put_contents($log_file, date("Y-m-d H:i:s") . ' Failed to check for duplicates for order ' . $orderid . ': ' . pg_last_error($db) . PHP_EOL, FILE_APPEND);
            pg_query($db, "ROLLBACK");
            return false;
            }
    } catch (Exception $e) {
        // Log the error
        file_put_contents($log_file, date("Y-m-d H:i:s") . ' Error during duplicate entry check: ' . $e->getMessage() . PHP_EOL, FILE_APPEND);
        pg_query($db, "ROLLBACK");
        return false;
    }
}

function deleteDuplicateEntry($db, $orderid, $log_file) {
    //  Attempt to delete the entry
    $delete_query = "DELETE FROM tsim_order_deduplication WHERE order_no = '$orderid';";
    $delete_res = pg_query($db, $delete_query);

    // Check if deletion was successful
    if ($delete_res) {
        if (pg_affected_rows($delete_res) > 0) {
            file_put_contents($log_file, date("Y-m-d H:i:s") . ' Successfully deleted duplicate entry for order: ' . $orderid . PHP_EOL, FILE_APPEND);
            return true;
        } else {
            file_put_contents($log_file, date("Y-m-d H:i:s") . ' No duplicate entry found to delete for order: ' . $orderid . PHP_EOL, FILE_APPEND);
            return false;
        }
    } else {
        file_put_contents($log_file, date("Y-m-d H:i:s") . ' Failed to delete duplicate entry for order ' . $orderid . ': ' . pg_last_error($db) . PHP_EOL, FILE_APPEND);
        return false;
    }
}


function curl_data($param)
{
	global $db,$log_file;
	file_put_contents($log_file, date("Y-m-d H:i:s") . '**************Curl Started*****************'.PHP_EOL, FILE_APPEND);
	$ch = curl_init();     
	$url = "https://mail.tsim.in/simmisn.e";
	curl_setopt($ch, CURLOPT_URL,$url);
	curl_setopt($ch, CURLOPT_POST, true);	
	file_put_contents($log_file, date("Y-m-d H:i:s") .$param.PHP_EOL, FILE_APPEND);
	curl_setopt($ch, CURLOPT_POSTFIELDS,$param);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
	$output = curl_exec($ch);
	if($output === false)
	{
		echo 'Curl error: ' . curl_error($ch);
		file_put_contents($log_file, date("Y-m-d H:i:s") .curl_error($ch).PHP_EOL, FILE_APPEND);
		$delete1 = deleteDuplicateEntry($db, $orderid, $log_file);
		curl_close ($ch); 
	}
	else
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Operation completed without any errors'.PHP_EOL, FILE_APPEND);
		curl_close ($ch); 
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Curl output-'.$output.PHP_EOL, FILE_APPEND);
	return 1;
}
function parse_personal_info($json)
{
	global $orderid,$items,$email,$phone,$action,$first_name,$last_name,$clientname,$domainname,$company,$clientadd1,$clientadd2,$address_city,$address_state,$address_country,$postcode,$start_date,$payment_id,$status,$curr,$total,$sub_total,$total_shipping,$country_specific,$payment_mode,$link_amt,$total_tax,$username,$subtotal,$mnt,$gift_item,$note,$quantity,$sku,$item_cost,$sk,$country_dur_stdcode,$country,$duration,$std_code,$max_from_date,$max_to_date,$sim_phone_no,$order_type,$country_specific,$action,$shipping_add1,$shipping_add2,$shipping_city,$shipping_state,$shipping_country,$shipping_postcode,$flag_user,$client_mail,$db,$log_file,$first_name2,$last_name2,$clientname2,$l_phone,$l_clientname,$is_kyc_required;
	global $db,$log_file;
	file_put_contents($log_file, date("Y-m-d H:i:s") .'********Inside parse_personal_info function*****'.PHP_EOL, FILE_APPEND);
	$obj = json_decode($json, true);
	$arr = print_r($obj, true);		
	//file_put_contents($log_file, date("Y-m-d H:i:s") . 'json->'.$arr . PHP_EOL, FILE_APPEND);
	$first_name     = $obj['order']['billing_address']['first_name'];
	$first_name     = str_replace("'", "", $first_name);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'First Name-'.$first_name.PHP_EOL, FILE_APPEND);
	$last_name      = $obj['order']['billing_address']['last_name'];
	$last_name      = str_replace("'", "", $last_name);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Last Name-'.$last_name.PHP_EOL, FILE_APPEND);
	$clientname     = $first_name." ".$last_name;
	$clientname 	= str_replace("'","",$clientname);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Billing Client Name-'.$clientname.PHP_EOL, FILE_APPEND);
	$first_name2     = $obj['order']['customer']['shipping_address']['first_name'];
	$first_name2     = str_replace("'", "", $first_name2);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'First Name-'.$first_name2.PHP_EOL, FILE_APPEND);
	$last_name2      = $obj['order']['customer']['shipping_address']['last_name'];
	$last_name2      = str_replace("'", "", $last_name2);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Last Name-'.$last_name2.PHP_EOL, FILE_APPEND);
	$clientname2     = $first_name2." ".$last_name2;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Client Name-'.$clientname2.PHP_EOL, FILE_APPEND);
	if($clientname2 != '' || $clientname2 != null || !empty($clientname2))
	{       
		if(strcmp($clientname,$clientname2))
			$l_clientname = $clientname2;
		else    
			$l_clientname = $clientname;
	}
	else
		$l_clientname = $clientname;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client Name-'.$l_clientname.PHP_EOL, FILE_APPEND);
	$b_phone          = $obj['order']['customer']['billing_address']['phone'];
	$s_phone2         = $obj['order']['customer']['shipping_address']['phone'];
	if($s_phone2 != '' || $s_phone2 != null || !empty($s_phone2))
	{       
		if(!strcmp($b_phone,$s_phone2))
			$l_phone = $phone2;
		else    
			$l_phone = $phone;
	}
	else
		$l_phone = $phone;
	file_put_contents($log_file, date("Y-m-d H:i:s") .'l_phone-'.$l_phone.PHP_EOL, FILE_APPEND);

	$currencyforcheckingdomain = $obj['order']['currency'];
        if($currencyforcheckingdomain == "INR")        {
                $domainname     = "tsim.in";
        } else{
                 $domainname     = "tsim.mobi";
	}

	 file_put_contents($log_file, date("Y-m-d H:i:s") .'********Domain name***** ->'.$domainname.PHP_EOL, FILE_APPEND); 
	 
	//$domainname     = "tsim.in";


	$company        = $obj['order']['customer']['billing_address']['company'];
	$company        = str_replace("'", "", $company);
	if( $company == "")
		$company = $clientname;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Company-'.$company.PHP_EOL, FILE_APPEND);
	$clientadd1     = $obj['order']['customer']['billing_address']['address_1'];
	$clientadd1     = str_replace("'", "", $clientadd1);
	if($clientadd1 == "")
		$clientadd1 == 'Not Provided';
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add1-'.$clientadd1.PHP_EOL, FILE_APPEND);
	$clientadd2     = $obj['order']['customer']['billing_address']['address_2'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add2-'.$clientadd2.PHP_EOL, FILE_APPEND);
	$clientadd2     = str_replace("'", "", $clientadd2);
	if($clientadd2 == "" || $clientadd2 == NULL || empty($clientadd2))
		$clientadd2 = 'Not Provided';
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Client add2-'.$clientadd2.PHP_EOL, FILE_APPEND);
	$address_city   = $obj['order']['customer']['billing_address']['city'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_city -'.$address_city.PHP_EOL, FILE_APPEND);
	$address_state  = $obj['order']['customer']['billing_address']['state'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_state-'.$address_state.PHP_EOL, FILE_APPEND);
	$address_country= $obj['order']['customer']['billing_address']['country'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'address_country-'.$address_country.PHP_EOL, FILE_APPEND);
	$postcode       = $obj['order']['customer']['billing_address']['postcode'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'postcode-'.$postcode.PHP_EOL, FILE_APPEND);
	$shipping_add1  = $obj['order']['customer']['shipping_address']['address_1'];
	$shipping_add1  = str_replace("'", "", $shipping_add1);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add1-'.$shipping_add1.PHP_EOL, FILE_APPEND);
	$shipping_add2  = $obj['order']['customer']['shipping_address']['address_2'];
	$shipping_add2  = str_replace("'", "", $shipping_add2);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add2-'.$shipping_add2.PHP_EOL, FILE_APPEND);
	$shipping_city  = $obj['order']['customer']['shipping_address']['city'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping City-'.$shipping_city.PHP_EOL, FILE_APPEND);
	$shipping_state = $obj['order']['customer']['shipping_address']['state'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_state-'.$shipping_state.PHP_EOL, FILE_APPEND);
	$shipping_country= $obj['order']['customer']['shipping_address']['country'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_country-'.$shipping_country.PHP_EOL, FILE_APPEND);
	$shipping_postcode = $obj['order']['customer']['shipping_address']['postcode'];
	$is_kyc_required =  isset($obj['order']['is_kyc_required']) ? $obj['order']['is_kyc_required'] : null;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping postcode-'.$shipping_postcode.PHP_EOL, FILE_APPEND);
	if($shipping_add1 == "")
		$shipping_add1 = $clientadd1;
	if($shipping_add2 == "")
		$shipping_add2 = $clientadd2;
	if($shipping_city == "")
		$shipping_city = $address_city;
	if($shipping_state == "")
		$shipping_state =  $address_state;
	if($shipping_country == "")
		$shipping_country = $address_country;
	if($shipping_postcode == "")
		$shipping_postcode = $postcode;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add1-'.$shipping_add1.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping Add2-'.$shipping_add2.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping City-'.$shipping_city.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_state-'.$shipping_state.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping_country-'.$shipping_country.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Shipping postcode-'.$shipping_postcode.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . ' is_kyc_required: '.$is_kyc_required.PHP_EOL, FILE_APPEND);
	file_put_contents($log_file, date("Y-m-d H:i:s") . '-----------Returning personal info-----------------'.PHP_EOL, FILE_APPEND);	
	#$per_info = array_push($per_info, $first_name, $last_name, $clientname, $first_name2, $last_name2, $clientname2, 
	return 1;
}	
function create_user($username,$phone)
{

	$db = $GLOBALS['db'];
	$log_file = $GLOBALS['log_file'];
	file_put_contents($log_file, date("Y-m-d H:i:s") .'********Inside create_user function*****'.PHP_EOL, FILE_APPEND);
	#$chk_user   = "SELECT count(username) from sim_user where primary_emailadd like '$username@%' and username not like '1555%'";
	#file_put_contents($log_file, date("Y-m-d H:i:s") .$chk_user.PHP_EOL, FILE_APPEND);
	#$res_chk_user = pg_query($db, $chk_user);
	#$rec_user  = pg_fetch_row($res_chk_user);
	#$num_user = $rec_user['0'];
	#file_put_contents($log_file, date("Y-m-d H:i:s") .'Total user present-'.$num_user .PHP_EOL, FILE_APPEND);
	#if($num_user > 0)
	/*{
		#$cnt = $num_user+1;
		#file_put_contents($log_file, date("Y-m-d H:i:s") .'count is -'.$cnt .PHP_EOL, FILE_APPEND);
		for($d = 0;$d < 100;$d++)
		{
			#$numbers = preg_replace('/[^0-9]/', '', $username);
			$uname = preg_replace('/[^a-zA-Z]/', '', $username);
			$number = rand(15555,9999999);
			file_put_contents($log_file, date("Y-m-d H:i:s") .$number.PHP_EOL, FILE_APPEND);
			#file_put_contents($log_file, date("Y-m-d H:i:s") .$uname.PHP_EOL, FILE_APPEND);
			#$numbers++;
			#file_put_contents($log_file, date("Y-m-d H:i:s") .$numbers.PHP_EOL, FILE_APPEND);
			$username = $uname.$number;
			file_put_contents($log_file, date("Y-m-d H:i:s") .'New username-'.$username.PHP_EOL, FILE_APPEND);
			$chk_new_user ="select username from sim_user where username = '$username'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Chk_new_user-'.$chk_new_user .PHP_EOL, FILE_APPEND);
			$res_chk_new_user = pg_query($db,$chk_new_user);
			if(!$res_chk_new_user)
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Not executes'.PHP_EOL, FILE_APPEND);
			$num_chk_new_user = pg_num_rows($res_chk_new_user);
			file_put_contents($log_file, date("Y-m-d H:i:s") .$num_chk_new_user .PHP_EOL, FILE_APPEND);
			if($num_chk_new_user > 0)
			{
				$flag_user = 1;
				continue;
			}
			else
				break;
		}
		file_put_contents($log_file, date("Y-m-d H:i:s") .'New username-'.$username.PHP_EOL, FILE_APPEND);
	}*/
	$suffix = $phone % 10000000;
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Suffix is-'.$suffix.PHP_EOL, FILE_APPEND);
	$username = "1555".$suffix;
	$chk_new_user ="select username from sim_user where username = '$username'";
	file_put_contents($log_file, date("Y-m-d H:i:s") .'Chk_new_user-'.$chk_new_user .PHP_EOL, FILE_APPEND);
	$res_chk_new_user = pg_query($db,$chk_new_user);
	$num_username = pg_num_rows($res_chk_new_user);
	if(!$res_chk_new_user)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Not executes-Counld Not create User'.PHP_EOL, FILE_APPEND);
	}
	if($num_username > 0)
	{
		do{
			$get_nex_val = "select nextval('simuser_ids')";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$get_nex_val.PHP_EOL, FILE_APPEND);
			$res_get_nex_val = pg_query($db,$get_nex_val);
			$rec_get_nex_val = pg_fetch_assoc($res_get_nex_val);
			$next_val = $rec_get_nex_val['nextval'];
			$username = "1555".$next_val;
			$new_user ="select count(username) from sim_user where username = '$username'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Chk_new_user-'.$chk_new_user .PHP_EOL, FILE_APPEND);
			$res_new_user = pg_query($db,$chk_new_user);
			$rec_new_user = pg_fetch_assoc($res_chk_new_user);
			$count_new_user = $rec_new_user['count'];
		}while($count_new_user!=0);
	}
	file_put_contents($log_file, date("Y-m-d H:i:s") .'New username-'.$username.PHP_EOL, FILE_APPEND);
	return $username;
}
function send_mail($obj)
{
	$orderid        = $obj['order']['order_number'];
	$to = "hoshang@staff.ownmail.com,services@tsim.in,husain@staff.ownmail.com";
	$sub_sells = 'Telesales Mail->TSIM order-'.$orderid .' placed successfully.';
	$sub_client = 'Client Mail->Your order-'.$orderid.' has been placed.';
	$headers = "From: TSIM Team <services@tsim.in>\r\n";
//	$headers.="Bcc: husain@staff.ownmail.com,deven@staff.ownmail.com,taher@staff.ownmail.com\r\n";
//	$headers.="Bcc: husain@staff.ownmail.com\r\n";	
//	$headers .= "Bcc: husain@staff.ownmail.com,binu@staff.ownmail.com,deven@staff.ownmail.com\r\n";
	$header2 = "From: TSIM Team <documents@tsim.in>\r\n";
	$client_mail = $GLOBALS['client_mail'];
	$db = $GLOBALS['db'];
	$log_file = $GLOBALS['log_file'];
	$gift_item = $GLOBALS['gift_item'];
	file_put_contents($log_file, date("Y-m-d H:i:s") .'GIFT ITEM :-'.$gift_item.PHP_EOL, FILE_APPEND);
	$clientname = $GLOBALS['clientname'];	
	$items =	$GLOBALS['items'];
	file_put_contents($log_file, date("Y-m-d H:i:s") .'ITEMS :-'.$items.PHP_EOL, FILE_APPEND);
	$sim_phone_no = $GLOBALS['sim_phone_no'];
	file_put_contents($log_file, date("Y-m-d H:i:s") .'SIM Phone No :-'.$sim_phone_no.PHP_EOL, FILE_APPEND);
	$username = $GLOBALS['username'];	
	$sku = $GLOBALS['sku'];
	$email = $GLOBALS['email'];
	$phone = $GLOBALS['phone'];
	$order_type = $GLOBALS['order_type'];  	
	 file_put_contents($log_file, date("Y-m-d H:i:s") .'ORder type :-'.$order_type.PHP_EOL, FILE_APPEND);	
	$delivery_method = $GLOBALS['delivery_method'];

	file_put_contents($log_file, date("Y-m-d H:i:s") .'****************************Inside New Send Mail After Booking Done********************************'.PHP_EOL, FILE_APPEND);
	$arr = print_r($obj, true);
	file_put_contents($log_file, date("Y-m-d H:i:s") .$sku.PHP_EOL, FILE_APPEND);
	
	$client_mail = "<br>Hello "."Mr/Ms."." $clientname,"."<br>"."Your TSIM order has been sucessfully placed. The SIM Card details are as follows."."<br><br>"."Order number: ".$orderid."<br>";
        $telesells_mail_head = "\nHello,"."\nMr/Ms "."$clientname is registered through TSIM."."\n\n"."Order No-"."\t\t".$orderid."\n"."Username-"."\t\t".$username."\n"."Contact no-"."\t\t"."$phone"."\n"."Email-"."\t\t\t"."$email"."\n"."Delivery-"."\t\t"."$delivery_method"."\n"."Sim card details are as follow"."\n\n";
        $telesells_mail_body = "Tsim No"."\t\t"."Sim serial no"."\t\t"."Serial no"."\r\n";
        $line_items = count($obj['order']['line_items']);
        file_put_contents($log_file, date("Y-m-d H:i:s") .'line_items ='.$line_items.PHP_EOL, FILE_APPEND);

        $line_items_data = $obj['order']['line_items'];
        foreach($line_items_data as $each_line_item)
        {
                $line_item_details = json_encode($each_line_item);
                file_put_contents($log_file, date("Y-m-d H:i:s") .'line items skus ->'.$line_item_details.PHP_EOL, FILE_APPEND);
        }

	

	if($order_type == "New")
	{
		for($i=0; $i<$line_items;$i++)
		{
			if($obj['order']['line_items']["$i"]['meta']['0']['value'] == 'Free Item')
			{
				$gift_item = $obj['order']['line_items']["$i"]['name'];
				break;
			}
			else
				$gift_item="TSIM Booking";
		}
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Gift item-'.$gift_item.PHP_EOL, FILE_APPEND);	

	
		sleep(2);
		$get_details = "/*NO LOAD BALANCE*/select * from clienttrip where order_no ='$orderid'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Get Trip Details-'.$get_details .PHP_EOL, FILE_APPEND);
		$res_get_details = pg_query($db, $get_details);
		$num_get_details = pg_num_rows($res_get_details);
		file_put_contents($log_file, date("Y-m-d H:i:s") .'no of items-'.$items.PHP_EOL, FILE_APPEND);	
		file_put_contents($log_file, date("Y-m-d H:i:s") .'no of Records-'.$num_get_details.PHP_EOL, FILE_APPEND);	
		if($num_get_details<=0)
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Order no '.$orderid.' not in clienttrip, please verify in your database/simmis.'.PHP_EOL, FILE_APPEND);
			mail("aditya@staff.ownmail.com","failed to search in clienttrip","Order no $orderid not in clienttrip, please verify in your database/simmis.");
			exit;
		}


		$get_booked_skus = "select sku from tsim_orders_line_items where order_no ='$orderid'";
        	file_put_contents($log_file, date("Y-m-d H:i:s") .'Get booked skus-'.$get_booked_skus .PHP_EOL, FILE_APPEND);
		$exe_get_booked_skus = pg_query($db, $get_booked_skus);
       		$fetch_get_booked_skus = pg_fetch_all($exe_get_booked_skus);
       		file_put_contents($log_file, date("Y-m-d H:i:s") .'booked skus-'.serialize($fetch_get_booked_skus) .PHP_EOL, FILE_APPEND);
	 	
		foreach($fetch_get_booked_skus as $eachsku)
		{
			$fetch_get_booked_sku = $eachsku['sku'];



		for( $l=0; $l<$items;$l++)
		{

		

			$quantity       = $obj['order']['line_items']["$l"]['quantity'];
			file_put_contents($log_file, date("Y-m-d H:i:s") .' l ='.$l.PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Quantity-'.$quantity.PHP_EOL, FILE_APPEND); 

			for($q=0; $q<$quantity;$q++)
			{
				$meta_value = "";
                        	$meta_validity = "";
                        	$meta_validity_label = "";
                        	$pack_details = "";			
				file_put_contents($log_file, date("Y-m-d H:i:s") .' l ='.$l.PHP_EOL, FILE_APPEND);
				file_put_contents($log_file, date("Y-m-d H:i:s") .' q ='.$q.PHP_EOL, FILE_APPEND);
				if($l==0)
					$sku            = $obj['order']['line_items']['sku'];
				else
					$sku            = $obj['order']['line_items']["$l"]['sku'];


				for($t = 0; $t<pg_num_rows($exe_get_booked_skus);$t++)
				{
//					file_put_contents($log_file, date("Y-m-d H:i:s") .' SKU details - '.$fetch_get_booked_skus[$t]['pack_name'].PHP_EOL, FILE_APPEND);
					file_put_contents($log_file, date("Y-m-d H:i:s") .' SKU details - '.$fetch_get_booked_sku.PHP_EOL, FILE_APPEND);					
					file_put_contents($log_file, date("Y-m-d H:i:s") .'ABOVE THE IF CONDITION COMPARING THE BELOW TWO SKUs'.PHP_EOL, FILE_APPEND);
					file_put_contents($log_file, date("Y-m-d H:i:s") .$sku.PHP_EOL, FILE_APPEND);
					file_put_contents($log_file, date("Y-m-d H:i:s") .$fetch_get_booked_skus[$t]['sku'].PHP_EOL, FILE_APPEND);

					$sku            = $obj['order']['line_items']["$t"]['sku'];
					file_put_contents($log_file, date("Y-m-d H:i:s") .$sku.PHP_EOL, FILE_APPEND);


					//if($sku==$fetch_get_booked_skus[$t]['sku'])
					if($sku==$fetch_get_booked_sku)
					{

						$meta_count = count($obj['order']['line_items']["$l"]['meta']);	
						$meta_arr = $obj['order']['line_items'][$l]['meta'];
						$activationDate =  null;
						foreach($meta_arr as $v)
						{

							#if($v['key']==='Activation Date - 1' || $v['key']==='Activation Date')
							if(substr($v['key'],0,15) == 'Activation Date')
							{
								$activationDate =  $v['value'];

							}

						}



						for($z=0; $z<5;$z++)
						{

							file_put_contents($log_file, date("Y-m-d H:i:s").'-l is-'.$l.'-q is-'.$q.'-z is -'.$z.PHP_EOL, FILE_APPEND);
							if($obj['order']['line_items']["$l"]['meta']["$z"]['value'] == 'Free Item')
							{
								file_put_contents($log_file, date("Y-m-d H:i:s") . '*******Free Item->Skip********'.PHP_EOL, FILE_APPEND);
								continue 3;
							}
						}

						$rec_get_details = pg_fetch_assoc($res_get_details);
						$sim_phone_no = $rec_get_details['sim_phone_no'];
						if(strlen($sim_phone_no)>14)
							$tab = "\t";
						else if (strlen($sim_phone_no) == 0) 
							continue;
						else
							$tab = "\t\t";	

						file_put_contents($log_file, date("Y-m-d H:i:s") .'sim_phone_no-'.$sim_phone_no.PHP_EOL, FILE_APPEND);
						$get_sim_details = "select * from sim_stock where sim_phone_no = '$sim_phone_no'";
						file_put_contents($log_file, date("Y-m-d H:i:s") .'get_sim_details-'.$get_sim_details.PHP_EOL, FILE_APPEND);
						$res_get_sim_details = pg_query($db, $get_sim_details);

						if(!$res_get_sim_details)
							exit;

						$rec_get_sim_details = pg_fetch_assoc($res_get_sim_details);
						$sim_no = $rec_get_sim_details['simno'];
						$serial_no = $rec_get_sim_details['srno'];


						if($meta_count == 0)
						{
							$pack_details = $obj['order']['line_items']["$l"]['name'];
						}
						else
						{




							file_put_contents($log_file, date("Y-m-d H:i:s") .' more metas'.PHP_EOL, FILE_APPEND);
							$pack_details = $obj['order']['line_items']["$l"]['name'];
                                                        $meta_arr2 = $obj['order']['line_items']["$l"]['meta'];


							foreach($meta_arr2 as $v)
							{
								file_put_contents($log_file, date("Y-m-d H:i:s") .' Key - '.$v['key'].' Value - '.$v['value'].' Label - '.$v['label'].PHP_EOL, FILE_APPEND);
								file_put_contents($log_file, date("Y-m-d H:i:s") .' STring operation - '.stripos($v['key'],"validity",0).PHP_EOL, FILE_APPEND);

								if(stripos($v['value'],"Data")!== false || stripos($v['value'],"minutes")!== false || stripos($v['value'],"GB")!== false || stripos($v['value'],"calls")!==false || stripos($v['value'],"unlimited")!== false || stripos($v['value'],"hotspot")!==false)
								{
									$meta_value = $v['value'];
								}
								elseif(is_numeric(stripos($v['key'],"Validity",0)))
								{
									$meta_validity = $v['value'];
									$meta_validity_label = $v['label'];
								}

								file_put_contents($log_file, date("Y-m-d H:i:s") .' Pack label - '.$meta_validity_label.' Pack validity - '.$meta_validity.' meta value - '.$meta_value.PHP_EOL, FILE_APPEND);
							
							}      


							
							if($meta_value != "")
							{
								$pack_details = $pack_details." ,".$meta_value;
							}
							else
							{
								$pack_details = $obj['order']['line_items']["$l"]['name'];
							}

							if($meta_validity != "")
							{
								$pack_details = $pack_details." , ".$meta_validity_label." - ".$meta_validity;
							}
						}
						
						file_put_contents($log_file, date("Y-m-d H:i:s") .' ---------------------------------------------------------------------------------------'.PHP_EOL, FILE_APPEND);
			                        file_put_contents($log_file, date("Y-m-d H:i:s") .' Pack details - '.$pack_details.PHP_EOL, FILE_APPEND);
						file_put_contents($log_file, date("Y-m-d H:i:s") .' ---------------------------------------------------------------------------------------'.PHP_EOL, FILE_APPEND);





	 					$getsimphonenoquery = "SELECT sim_phone_no FROM sim_stock WHERE  simno <> sim_phone_no AND LENGTH(REPLACE(REPLACE(REPLACE(sim_phone_no, ',', ''), '.', ''), '''', '')) >= 8 AND LENGTH(REPLACE(REPLACE(REPLACE(sim_phone_no, ',', ''), '.', ''), '''', '')) <= 14 AND REPLACE(REPLACE(REPLACE(sim_phone_no, ',', ''), '.', ''), '''', '') NOT LIKE '%\_%' ESCAPE '\' AND availability = false AND active = true and simno ='$sim_no'";

                        			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Query for Getting SimPhoneNo - >'.$getsimphonenoquery .PHP_EOL, FILE_APPEND);
                        			$getsimphoneno = pg_query($db,$getsimphonenoquery);
                        			if($getsimphoneno)
                        			{
                                			$gotSimphoneno = pg_fetch_assoc($getsimphoneno);
                                			$simPhoneNo = $gotSimphoneno['sim_phone_no'];
                                			file_put_contents($log_file, date("Y-m-d H:i:s") . ' Sim_Phone_no ->' . $simPhoneNo .PHP_EOL, FILE_APPEND);
                                			if(is_numeric($simPhoneNo) && $simPhoneNo > 0)
                                			{
								$client_mail = $client_mail."Phone number: ".$simPhoneNo."<br>";
							}	
						}
						else
						{
				 			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Sim no is same as simPhone Number thats why simPhone number is not sending.' .PHP_EOL, FILE_APPEND);
						}











						
						$queryforcheckingthesimno = "select * from sim_stock s join clienttrip c on s.sim_phone_no=c.sim_phone_no where c.order_no like '$sim_no%'";
                                                file_put_contents($log_file, date("Y-m-d H:i:s") . 'Query for Getting SimNo - >'.$queryforcheckingthesimno .PHP_EOL, FILE_APPEND);
                                                $getsimnocheck = pg_query($db,$queryforcheckingthesimno);
                                                if($getsimnocheck)
                                                {
                                                        $gotsimno = pg_num_rows($getsimnocheck);
                                                        file_put_contents($log_file, date("Y-m-d H:i:s") . 'Got rows - >'.$gotsimno .PHP_EOL, FILE_APPEND);
                                                        if($gotsimpno > 0)
                                                        {

								file_put_contents($log_file, date("Y-m-d H:i:s") . 'Got rows inside if - >'.$gotsimno .PHP_EOL, FILE_APPEND);
                                                                file_put_contents($log_file, date("Y-m-d H:i:s") . 'Sim no is same as orderno thats why simno number is not sending.' .PHP_EOL, FILE_APPEND);
                                                        }
                                                        else
                                                        {
								file_put_contents($log_file, date("Y-m-d H:i:s") . 'Got rows inside else - >'.$gotsimno .PHP_EOL, FILE_APPEND);
                                                                $client_mail = $client_mail."Sim serial number: ".$sim_no."<br>";
                                                        }
                                                }














						$client_mail = $client_mail."Plan Details: ".$pack_details."<br>";

						//$client_mail = $client_mail."Sim serial number: ".$sim_no."\n"."Plan Details: ".$pack_details."\n\n";


						$telesells_mail_body = $telesells_mail_body."\r".$sim_phone_no."$tab".$sim_no."\t\t".$serial_no."\r\n";
					}
					else
					{
						file_put_contents($log_file, date("Y-m-d H:i:s") .'Came to else condition '.PHP_EOL, FILE_APPEND);
					}

					unset($pack_details,$meta_value,$meta_validity_label,$meta_validity,$meta_arr2);	
				}
			}
		}

		}

		$telesells_mail_body .= "Please upload your documents (Passport front and back page, Visa) using the following link \n";
		$telesells_mail_body .= "https://mail.tsim.in/docs.php?o=".$orderid."\n";

//		$client_mail .= "Please upload your documents (Passport front and back page, Visa) using the following link \n";
//		$client_mail .= "https://mail.tsim.in/docs.php?o=".$orderid."\n";
//		$client_mail = $client_mail."\r\n"."\nWish You a Very Happy journey.\n"."\nThank You"."\nRegards,"."\n"."TSIM Team";
//		$client_mail .= "Please upload your documents (Passport front and back page, Visa) using the following link : \n\n";


		/*
                $client_mail .= "Please upload your documents (Passport front and back page, visa or ticket if visa is on arrival) using the following link : \n\n";
                $client_mail .= "https://mail.tsim.in/docs.php?o=".$orderid."\n\n";
		$client_mail = $client_mail."Wish You a Very Happy journey.\n"."\nThank You"."\nRegards,"."\n"."TSIM Team";
	 */
		$client_mail .= "Please upload your documents (Passport front and back page, visa or ticket if visa is on arrival) using the following link : <br><br>";
                #$client_mail .= "https://mail004.ownmail.com/docs.php?o=".$orderid."<br><br>";
                $client_mail .= "https://mail.tsim.in/docs.php?o=".$orderid."<br><br>";
                $client_mail = $client_mail."Wish You a Very Happy journey.<br>"."<br>Thank You"."<br>Regards,"."<br>"."TSIM Team";

                $telesells_mail = $telesells_mail_head.$telesells_mail_body."\n\nRegards,"."\n"."TSIM Team";	
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Send Mail to-'.$to.PHP_EOL, FILE_APPEND);	
		file_put_contents($log_file, date("Y-m-d H:i:s") .$client_mail.PHP_EOL, FILE_APPEND);	
		
		file_put_contents($log_file, date("Y-m-d H:i:s") .$telesells_mail.PHP_EOL, FILE_APPEND);
		//mail($to,$sub_sells,$telesells_mail,$headers);
		//mail($email,$sub_client,$client_mail,$headers,'-fservices@tsim.in');

		 $client_mail = escapeForSQL($client_mail);
                $query = sprintf(
                "INSERT INTO tsim_order_notifications (email, notification_type, subject_content, message_content, status, priority, from_email) 
                VALUES ('%s', '{%s}', '{%s}', '{%s}', '{%s}', '%s', '%s')",
                $email, "email", $sub_client, $client_mail, "email_pending", "normal", "services@tsim.in");
		
		
                $result = pg_query($db,$query);
                if($result){
                file_put_contents($log_file, date("Y-m-d H:i:s") .'Query successful for notification order placement'.PHP_EOL, FILE_APPEND);
                        }
                else{
                file_put_contents($log_file, date("Y-m-d H:i:s") .'Query failed for notification-'.PHP_EOL, FILE_APPEND);
                }
                file_put_contents($log_file, date("Y-m-d H:i:s") .'This is query for notification'.$query.PHP_EOL, FILE_APPEND);


//		mail("hoshang@staff.ownmail.com",$sub_sells,$telesells_mail,$headers,'-fservices@tsim.in');
//		mail("hoshang@staff.ownmail.com",$sub_client,$client_mail,$headers,'-fservices@tsim.in');
//		mail("services@tsim.in",$sub_sells,$telesells_mail,$headers,'-fservices@tsim.in');
//		mail("services@tsim.in",$sub_client,$client_mail,$headers,'-fservices@tsim.in');
//		mail("husain@staff.ownmail.com",$sub_sells,$telesells_mail,$headers,'-fservices@tsim.in');
//		mail("husain@staff.ownmail.com",$sub_client,$client_mail,$headers,'-fservices@tsim.in');
//		mail("deven@staff.ownmail.com",$sub_client,$client_mail,$headers,'-fservices@tsim.in');
//		mail("husain@staff.ownmail.com",$sub_client,$client_mail,$headers,'-fservices@tsim.in');
//		mail("taher@staff.ownmail.com",$sub_client,$client_mail,$headers,'-fservices@tsim.in');

		//ISSUE 4447****************************
		$Query = "select is_kyc_required from clienttrip where order_no='$orderid'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .' query to check booking and get iskycrequired flag - '.$Query.PHP_EOL, FILE_APPEND);
		$ExecuteQuery = pg_query($db,$Query);
		$getiskycflag = pg_fetch_all($ExecuteQuery);
		$is_kyc_required = $getiskycflag[0]['is_kyc_required'];
		file_put_contents($log_file, date("Y-m-d H:i:s") .' is Kyc Required flag -------------- '.$is_kyc_required.PHP_EOL, FILE_APPEND);
		
		if($is_kyc_required == true){
		$sql_upload = "insert into tsim_order_documents(order_no,documents_uploaded,no_of_documents_uploaded) values ('$orderid','f','0')";
		file_put_contents($log_file, date("Y-m-d H:i:s") .' Tsim order documents - '.$sql_upload.PHP_EOL, FILE_APPEND);
		$res_sql_upload = pg_query($db,$sql_upload);
		if(!$res_sql_upload)
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Error in inserting'. PHP_EOL, FILE_APPEND);
		else
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Executed'. PHP_EOL, FILE_APPEND);
		
		}

		//file_put_contents($log_file, date("Y-m-d H:i:s") .'********************Uploading document*****************'.PHP_EOL, FILE_APPEND);
		//GLOBAL $no_of_doc_uploaded;
		//$GLOBALS['no_of_doc_uploaded'] = 0;
	/*	for($j=1; $j<=3; $j++)
		{
		$order = $orderid."_$j";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'File name-'.$order.PHP_EOL, FILE_APPEND);
		#$ret_val = exec("ssh -C -i /home/staff/gsg-key-mumbai.pem ubuntu@13.126.194.28 ls /opt/bitnami/apps/wordpress/htdocs/wp-content/uploads/woocommerce_uploads/wcj_uploads/checkout_files_upload | grep '$order' ");
		$cmd = "/usr/bin/ssh -C -i /home/domains/gsg-key-mumbai.pem ubuntu@tsim.in ls /opt/bitnami/apps/wordpress/htdocs/wp-content/uploads/woocommerce_uploads/wcj_uploads/checkout_files_upload | grep '$order'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'cmd-'.$cmd.PHP_EOL, FILE_APPEND);
		$ret_val = exec("/usr/bin/ssh -C -i /home/domains/gsg-key-mumbai.pem ubuntu@tsim.in ls /opt/bitnami/apps/wordpress/htdocs/wp-content/uploads/woocommerce_uploads/wcj_uploads/checkout_files_upload | grep '$order' ");
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Ret Val-'.$ret_val.PHP_EOL, FILE_APPEND);
		if($ret_val == NULL || $ret_val == "")
		{
			//file_put_contents($log_file, date("Y-m-d H:i:s") .'******************Send mail to Client***************'.PHP_EOL, FILE_APPEND);
			//$GLOBALS['no_of_doc_uploaded'] = 0;
			continue;
		}
		else
		{
			#$return  = exec("scp -i /home/staff/gsg-key-mumbai.pem ubuntu@13.126.194.28:/opt/bitnami/apps/wordpress/htdocs/wp-content/uploads/woocommerce_uploads/wcj_uploads/checkout_files_upload/$ret_val /home/domains/briefcase/in/tsim/.briefcase/t/tsim_doc/.",$ret);
			$new_file_name = $username."_".$orderid."_$j".".jpg";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'New_file name-'.$new_file_name.PHP_EOL, FILE_APPEND);
			$return  = exec("/usr/bin/scp -i /home/domains/gsg-key-mumbai.pem ubuntu@tsim.in:/opt/bitnami/apps/wordpress/htdocs/wp-content/uploads/woocommerce_uploads/wcj_uploads/checkout_files_upload/$ret_val /home/domains/briefcase/in/tsim/.briefcase/s/services/tsim_doc/$new_file_name",$ret);
			$rr  = print_r($ret);
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Return Val-'.$rr.PHP_EOL, FILE_APPEND);
			if($rr == 1)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'File has been copied successfully'.PHP_EOL, FILE_APPEND);
				$GLOBALS['no_of_doc_uploaded'] = $GLOBALS['no_of_doc_uploaded'] + 1;
				file_put_contents($log_file, date("Y-m-d H:i:s") .'No of Documnet uploaded is -'.$no_of_doc_uploaded.PHP_EOL, FILE_APPEND);
				$del = "/usr/bin/ssh -C -i /home/domains/gsg-key-mumbai.pem ubuntu@tsim.in sudo rm -r /opt/bitnami/apps/wordpress/htdocs/wp-content/uploads/woocommerce_uploads/wcj_uploads/checkout_files_upload/$ret_val";
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Deleting uploaded File from TSIM'.PHP_EOL, FILE_APPEND);
				file_put_contents($log_file, date("Y-m-d H:i:s") .$del.PHP_EOL, FILE_APPEND);
				$ret_del = exec("/usr/bin/ssh -C -i /home/domains/gsg-key-mumbai.pem ubuntu@tsim.in sudo rm -r /opt/bitnami/apps/wordpress/htdocs/wp-content/uploads/woocommerce_uploads/wcj_uploads/checkout_files_upload/$ret_val");
				file_put_contents($log_file, date("Y-m-d H:i:s") .'ret_del-'.$ret_del.PHP_EOL, FILE_APPEND);
			}
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'File has not been copied successfully'.PHP_EOL, FILE_APPEND);
			}
		}
	}
	if($j>=3 && $no_of_doc_uploaded == 1)
		$no_of_doc_uploaded = 1;
	else if($j>=3 && $no_of_doc_uploaded == 2)
		$no_of_doc_uploaded = 2;
	else if($j>=3 && $no_of_doc_uploaded == 3)
		$no_of_doc_uploaded = 3;
	else   
		$no_of_doc_uploaded = 0;
	file_put_contents($log_file, date("Y-m-d H:i:s") .'No of Documnet uploaded is -'.$no_of_doc_uploaded.PHP_EOL, FILE_APPEND);
	if($no_of_doc_uploaded == 0)
	{
		$get_passport = "SELECT passport_no from sim_user where username='$username'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$get_passport .PHP_EOL, FILE_APPEND);
		$res_get_passport = pg_query($db, $get_passport);
		$rec_get_passport = pg_fetch_assoc($res_get_passport);
		$pp_no = $rec_get_passport['passport_no'];
		file_put_contents($log_file, date("Y-m-d H:i:s") .'Passport No-'.$pp_no.PHP_EOL, FILE_APPEND);
		if(!strcmp($pp_no,'X1550000'))
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'--Documents not uploaded and MIS also not Updated---'.PHP_EOL, FILE_APPEND);
			$subj = "Document Required $orderid- TSIM - ".date("Y/m/d");
			//$toaddr = "hoshang@staff.ownmail.com,$email";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$subj.PHP_EOL, FILE_APPEND);
			//$msg = "Hello,\nMr/Mrs/Miss $clientname, \nWe have received your order $orderid. But you have not uploaded required documents while placing order. In order to activate your sim card please attach the softcopy of passport and visa or flight ticket if visa is on arrival in reply to this mail before start of your journey.\nNote- Ignore if uploaded.\n\n\nRegards,\nTSIM Team.";
			$msg = "Hello,\nMr/Mrs/Miss $clientname, \nWe have received your order $orderid. But you have not uploaded required documents while placing order. In order to activate your sim card please attach the softcopy of Front and Last page of Passport ,visa(if not on arrival) and flight ticket in reply to this mail before start of your journey.\nNote- Ignore if uploaded.\n\n\nRegards,\nTSIM Team.";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$msg.PHP_EOL, FILE_APPEND);
			if(!mail($email,$subj,$msg,$header2))
			{
				mail("hoshang@staff.ownmail.com",$subj,$msg,$header2);
				mail("aditya@staff.ownmail.com",$subj,$msg,$header2);
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Mail not sent to client. Alert Services'.PHP_EOL, FILE_APPEND);
			}
			else
			{
				//mail($email,$subj,$msg,$header2);
				mail("hoshang@staff.ownmail.com",$subj,$msg,$header2);
				mail("aditya@staff.ownmail.com",$subj,$msg,$header2);
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Mail sent to clientt'.PHP_EOL, FILE_APPEND);
			}
			$sql_order = "select * from tsim_order_documents where order_no ='$orderid'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_order-'.$sql_order.PHP_EOL, FILE_APPEND);
			$res_sql_order = pg_query($db, $sql_order);
			if(!$res_sql_order)
				exit;
			$num_or = pg_num_rows($res_sql_order);
			if($num_or <= 0)
			{      
				$sql_upload = "insert into tsim_order_documents values ('$orderid','f','$no_of_doc_uploaded')";
				file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_upload-'.$sql_upload.PHP_EOL, FILE_APPEND);
				$res_sql_upload = pg_query($db,$sql_upload);
				if(!$res_sql_upload)
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Error in inserting'. PHP_EOL, FILE_APPEND);
				else
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Executed'. PHP_EOL, FILE_APPEND);
			}
			else
			{
				$sql_update = "update tsim_order_documents set no_of_documents_uploaded = '$no_of_doc_uploaded', documents_uploaded='f' where order_no ='$orderid'";
				file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_update-'.$sql_update.PHP_EOL, FILE_APPEND);
				$res_sql_update = pg_query($db,$sql_update);
				if(!$res_sql_update)
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Error in updating'. PHP_EOL, FILE_APPEND);
				else
					file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Executed'. PHP_EOL, FILE_APPEND);
			}
			exit;
		}
		else
		{		
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Passport No is already updated in MIS'.PHP_EOL, FILE_APPEND);
			$subj = "Passport No already existed in MIS- $orderid";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$subj.PHP_EOL, FILE_APPEND);
			$body = "Hello,\nMr/Mrs/Miss $clientname has placed order $orderid.\nPassport no is already updated in MIS.\n\n\nRegards,\nTSIM Team.";
			file_put_contents($log_file, date("Y-m-d H:i:s") .$body.PHP_EOL, FILE_APPEND);
			if(!mail("services@tsim.in",$subj,$body,$headers))
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Mail not sent Services'.PHP_EOL, FILE_APPEND);
				mail("hoshang@staff.ownmail.com","Undelievered Alert-$subj",$body,$headers);
				mail("aditya@staff.ownmail.com","Undelievered Alert-$subj",$body,$headers);
			}
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Mail sent to Services'.PHP_EOL, FILE_APPEND);
				mail("hoshang@staff.ownmail.com",$subj,$body,$headers);
				mail("aditya@staff.ownmail.com",$subj,$body,$headers);
			}
		}
	}
	else if ($no_of_doc_uploaded > 0 && $no_of_doc_uploaded<=2)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") .'******************Send mail to Services***************'.PHP_EOL, FILE_APPEND);
		$subj = "Some documents of order $orderid received.";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$subj.PHP_EOL, FILE_APPEND);
		$body = "Hello,\nMr/Mrs/Miss $clientname has placed order $orderid and also uploaded some documents.\nPlease communicate with client for the all required documents (Visa)..\n\n\nRegards,\nTSIM Team.";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$body.PHP_EOL, FILE_APPEND);
		if(!mail("services@tsim.in",$subj,$body,$headers))
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Mail not sent Services'.PHP_EOL, FILE_APPEND);
			mail("hoshang@staff.ownmail.com","Undelievered Alert-$subj",$body,$headers);
			mail("aditya@staff.ownmail.com","Undelievered Alert-$subj",$body,$headers);
		}
		else
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Mail sent to Services'.PHP_EOL, FILE_APPEND);
			mail("hoshang@staff.ownmail.com",$subj,$body,$headers);
			mail("aditya@staff.ownmail.com",$subj,$body,$headers);
		}
		$sql_order = "select * from tsim_order_documents where order_no ='$orderid'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_order-'.$sql_order.PHP_EOL, FILE_APPEND);
		$res_sql_order = pg_query($db, $sql_order);
		if(!$res_sql_order)    
			exit;
		$num_or = pg_num_rows($res_sql_order);
		if($num_or <= 0)
		{
			$sql_upload = "insert into tsim_order_documents values ('$orderid','t','$no_of_doc_uploaded')";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_upload-'.$sql_upload.PHP_EOL, FILE_APPEND);
			$res_sql_upload = pg_query($db,$sql_upload);
			if(!$res_sql_upload)
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Error in inserting'. PHP_EOL, FILE_APPEND);
			else
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Executed'. PHP_EOL, FILE_APPEND);
		}
		else
		{
			$sql_update = "update tsim_order_documents set no_of_documents_uploaded = '$no_of_doc_uploaded', documents_uploaded='t' where order_no ='$orderid'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_update-'.$sql_update.PHP_EOL, FILE_APPEND);
			$res_sql_update = pg_query($db,$sql_update);
			if(!$res_sql_update)
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Error in updating'. PHP_EOL, FILE_APPEND);
			else
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Executed'. PHP_EOL, FILE_APPEND);
		}
		exit;
	}
	else if($no_of_doc_uploaded > 2)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") .'******************Send mail to Services***************'.PHP_EOL, FILE_APPEND);
		$subj = "3 Documents of Order $orderid received";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$subj.PHP_EOL, FILE_APPEND);
		$body = "Hello,\nMr/Mrs/Miss $clientname has placed order $orderid and also uploaded 3 documents i.e. passport and visa.\nUpdate the passport number in the MIS.\n\n\nRegards,\nTSIM Team.";
		file_put_contents($log_file, date("Y-m-d H:i:s") .$body.PHP_EOL, FILE_APPEND);
		if(!mail("services@tsim.in",$subj,$body,$headers))
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Mail not sent Services'.PHP_EOL, FILE_APPEND);
			mail("hoshang@staff.ownmail.com","Undelievered Alert-$subj",$body,$headers);
			mail("aditya@staff.ownmail.com","Undelievered Alert-$subj",$body,$headers);
		}
		else
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") .'Mail sent to Services'.PHP_EOL, FILE_APPEND);
			mail("aditya@staff.ownmail.com",$subj,$body,$headers);
			mail("hoshang@staff.ownmail.com",$subj,$body,$headers);
		}
		$sql_order = "select * from tsim_order_documents where order_no ='$orderid'";
		file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_order-'.$sql_order.PHP_EOL, FILE_APPEND);
		$res_sql_order = pg_query($db, $sql_order);
		if(!$res_sql_order)
			exit;
		$num_or = pg_num_rows($res_sql_order);
		if($num_or <= 0)
		{
			$sql_upload = "insert into tsim_order_documents values ('$orderid','t','$no_of_doc_uploaded')";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_upload-'.$sql_upload.PHP_EOL, FILE_APPEND);
			$res_sql_upload = pg_query($db,$sql_upload);
			if(!$res_sql_upload)
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Error in inserting'. PHP_EOL, FILE_APPEND);
			else
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Executed'. PHP_EOL, FILE_APPEND);
		}
		else
		{
			$sql_update = "update tsim_order_documents set no_of_documents_uploaded = '$no_of_doc_uploaded', documents_uploaded='t' where order_no ='$orderid'";
			file_put_contents($log_file, date("Y-m-d H:i:s") .'sql_update-'.$sql_update.PHP_EOL, FILE_APPEND);
			$res_sql_update = pg_query($db,$sql_update);
			if(!$res_sql_update)
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Error in updating'. PHP_EOL, FILE_APPEND);
			else
				file_put_contents($log_file, date("Y-m-d H:i:s") .'Query Executed'. PHP_EOL, FILE_APPEND);
		}
		exit;
	}**/
	
	exit;
	}
}

function escapeForSQL($input) {
    if ($input === null) {
        return null;
    }
                
    // Initialize the escaped string
    $escaped = '';
    $input_len = strlen($input);
                
    // Iterate over each character in the input
    for ($i = 0; $i < $input_len; ++$i) {
        if ($input[$i] === "'") {
            $escaped .= "''"; // Escape single quote
        } elseif ($input[$i] === '\\') {
            $escaped .= "\\\\"; // Escape backslash
        } elseif ($input[$i] === '"') {
            $escaped .= "\\\""; // Escape double quote
        } elseif ($input[$i] === ',') {
            $escaped .= "\\,"; // Escape comma
        } else {
            $escaped .= $input[$i]; // Regular character
        }
    }

    return $escaped;
}

//refill_order :
function refill_order($orderid)                 
{                                              
	$log_file = '/tmp/tsim_integration.log';
	$headers = "From: TSIM Team <services@tsim.in>\r\n";
	file_put_contents($log_file, date("Y-m-d H:i:s") ."Refill order".PHP_EOL, FILE_APPEND);
	$msg_body = "Hello,"."\n"."A refill order has been placed. Order no- $orderid. Please process further."."\n\n\nThank You"."\nRegards,"."\n"."TSIM Team";
	file_put_contents($log_file, date("Y-m-d H:i:s") .$msg_body.PHP_EOL, FILE_APPEND);
	mail("hoshang@staff.ownmail.com","TSIM refill order $orderid placed.",$msg_body,$headers);
	mail("husain@staff.ownmail.com","TSIM refill order $orderid placed.",$msg_body,$headers);
	mail("services@tsim.in","TSIM refill order $orderid placed.",$msg_body,$headers);
	//exit;
	return 1;
}
?>
