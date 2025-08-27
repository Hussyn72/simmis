<?php
date_default_timezone_set("Asia/Calcutta");
$db=pg_connect("dbname=e2fax user=domains");
$log = "/tmp/tsim_customer_feedback.log";
//$test_to = "aditya@staff.ownmail.com";
//$subject = "We need your feedback! Help us create a better shopping experience for you.";
//$headers = "MIME-Version: 1.0" . "\r\n";
//$headers .= "Content-type:text/html;charset=UTF-8" . "\r\n";
$headers = "From: services@tsim.mobi\n";
//$headers .= "Bcc: services@tsim.in,services@tsim.mobi,husain@staff.ownmail.com,deven@staff.ownmail.com";
$headers .=  "Content-type: text/plain; charset=UTF-8\r\n";

file_put_contents($log, date("Y-m-d H:i:s").'->' .'------------------START--------------------'. PHP_EOL, FILE_APPEND);
//$get_customers = "select username,from_date,clientname,emailadd from clienttrip where username like 'amazon_client_%' and from_date = current_date - interval '30 Days'";
$get_customers = "select c.username,c.from_date,c.clientname,c.emailadd,a.saleschannel,a.asin from clienttrip c join amazon_order_items_view a on c.order_no=a.amazonorderid  where c.username like 'amazon_client_%' and c.from_date = current_date - interval '30 Days'";
file_put_contents($log, date("Y-m-d H:i:s").'->' .' Get customers query : '.$get_customers. PHP_EOL, FILE_APPEND);
$run_q = pg_query($db,$get_customers);
$num_q = pg_num_rows($run_q);
file_put_contents($log, date("Y-m-d H:i:s").'->' .' Number of customers : '.$num_q. PHP_EOL, FILE_APPEND);
$fetch_q = pg_fetch_all($run_q);
if($num_q > 0){
	for($i=0;$i<sizeof($fetch_q);$i++){
		file_put_contents($log, date("Y-m-d H:i:s").'->' .'********** customer start **********'. PHP_EOL, FILE_APPEND);
		$usr = $fetch_q[$i]['username'];
		$act_date = $fetch_q[$i]['from_date'];
		$name = $fetch_q[$i]['clientname'];
		$email = $fetch_q[$i]['emailadd'];
		$saleschannel = $fetch_q[$i]['saleschannel'];
		$asin = $fetch_q[$i]['asin'];
		file_put_contents($log, date("Y-m-d H:i:s").'->' .' Customer info : '.$usr." ".$name." ".$email. PHP_EOL, FILE_APPEND);
/*		$content = "<html><body>";
		$content .= "<p>Hello, ".$name."</p>";
		$content .= "<p>Thank you for using our SIM card on your recent trip. We are constantly striving to provide the ideal experience for our customers, and your input helps us to define that experience. Please tell us how we are doing by going to <a href='https://www.amazon.com/gp/css/order-history'>https://www.amazon.com/gp/css/order-history</a> and clicking on \"Leave seller feedback\" to the right of your order details.</p>";
		$content .= "<p>We hope to see you again soon!</p>";
		$content .= "<p>Thanks,<br>TSIM Team</p></body></html>";		
 */
		if($email!=""){
			//$content = "Hello $name, \n\nThank you for using our SIM card on your recent trip. We are constantly striving to provide the ideal experience for our customers, and your input helps us to define that experience. Please tell us how we are doing by sharing your experience by writing a product review. Simply click the link below and leave your feedback going to https://$saleschannel/dp/$asin.  \nWe hope to see you again soon!\n\nThanks,\nTSIM Team";
		//$content = "Hello $name, \n\nThank you for choosing our SIM card for your recent trip! We always strive to provide the best experience for our customers, and your feedback helps us improve. We’d love to hear about your experience—please take a moment to share your thoughts by writing a product review. Simply click the link below to leave your feedback: \n\nhttps://$saleschannel/dp/$asin \n\nWe truly appreciate your time and look forward to serving you again soon! \n\nThanks,\nTSIM Team";
			
			//get mail message from database
			$query="SELECT value FROM ( SELECT value, 1 AS priority FROM config_values  WHERE name = 'amazon_product_feedback_msg' AND key = '$saleschannel' UNION SELECT value, 2 AS priority FROM config_values WHERE name = 'amazon_product_feedback_msg'  AND key = 'default' ) AS subquery ORDER BY priority LIMIT 1";		
			file_put_contents('/tmp/tsim_customer_feedback.log', date("Y-m-d H:i:s") . '---Query For Getting Mail Message :- >'.$query . PHP_EOL, FILE_APPEND);
                	$ExecuteQueryforMsg = pg_query($db,$query);
	                $GetMessage = pg_fetch_assoc($ExecuteQueryforMsg);
        	        $message = $GetMessage['value'];

	                file_put_contents('/tmp/tsim_customer_feedback.log', date("Y-m-d H:i:s") . '--- Mail Message :- >'.$message . PHP_EOL, FILE_APPEND);

			$message = str_replace('\n',"\n", $message); // Convert literal '\n' to actual newline characters
        	        $message = str_replace('<name>',"$name", $message);
	                $message = str_replace('<saleschannel>', $saleschannel, $message);
 			$message = str_replace('<asin>', $asin, $message);


			file_put_contents('/tmp/tsim_customer_feedback.log', date("Y-m-d H:i:s") . '--- Mail Message :- >'.$message . PHP_EOL, FILE_APPEND);


	
			//get mail subject from database
			$query="SELECT value FROM ( SELECT value, 1 AS priority FROM config_values  WHERE name = 'amazon_product_feedback_sub' AND key = '$saleschannel' UNION SELECT value, 2 AS priority FROM config_values WHERE name = 'amazon_product_feedback_sub'  AND key = 'default' ) AS subquery ORDER BY priority LIMIT 1";
        	        file_put_contents('/tmp/tsim_customer_feedback.log', date("Y-m-d H:i:s") . '---Query For Getting Mail Subject :- >'.$query . PHP_EOL, FILE_APPEND);
	                $ExecuteQueryforSub = pg_query($db,$query);
        	        $GetSubject = pg_fetch_assoc($ExecuteQueryforSub);
                	$subject = $GetSubject['value'];

	                file_put_contents('/tmp/tsim_customer_feedback.log', date("Y-m-d H:i:s") . '--- Mail Subject :- >'.$subject . PHP_EOL, FILE_APPEND);

		


		//$content = "Hello $name, \n\nThank you for choosing our SIM for your recent trip! We’d love to hear about your experience - please spare a moment to write a product review. Your feedback not only helps other travelers but also supports our small business. Click the link below to leave your feedback: \n\nhttps://$saleschannel/dp/$asin \n\nWe truly appreciate your time and look forward to serving you again soon! \n\nThanks,\nTSIM Team";

		
		$mailop = mail("husain@staff.ownmail.com",$subject,$message,$headers,'-fservices@tsim.mobi');
		}
		else{
		file_put_contents($log, date("Y-m-d H:i:s").'->' .'There is no email address present in db'. PHP_EOL, FILE_APPEND);
		}
		file_put_contents($log, date("Y-m-d H:i:s").'->' .'********** customer end **********'. PHP_EOL, FILE_APPEND);
	}	
}
?>
