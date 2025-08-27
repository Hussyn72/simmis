<?php
date_default_timezone_set("Asia/Kolkata");
$log_file = '/tmp/check_active.log';
$okMessage = "Your form has been successfully submitted. Thank you! We will reach out to you via email shortly.";
$errorMessage = 'There was an error while submitting the form. Please try again later';
$DBConn = pg_connect("dbname='e2fax' user='domains'");

if (!$DBConn){
    echo "Error : Unable to open database\n";
}
else{
}


function logMessage($message, $details = null) {
    $log_file='/tmp/check_active.log';
    $log_message = date("Y-m-d H:i:s") . " [PID: " . getmypid() . ", IP: " . $_SERVER['REMOTE_ADDR'] . "] " . $message . PHP_EOL;
    if ($details !== null) {
        foreach ($details as $key => $value) {
            $log_message .= date("Y-m-d H:i:s") . " $key: $value\n";
        }
    }
    file_put_contents($log_file, $log_message, FILE_APPEND);
}

if (isset($_POST['checking_num']) && strlen($_POST['checking_num'])>=5) {
    logMessage("-------------- USER STARTS TYPING ---------------");
    $num = $_POST['checking_num'];
    $num = trim($num);
    $safe_num = pg_escape_string($num);
    logMessage("Entered value is : ". $safe_num);
    
    // Check if the last character is alphabet
    if (ctype_alpha(substr($safe_num, -1))) {
        // Trim the last character from the input
        $safe_num = substr($safe_num, 0, -1);
    }

    if (!preg_match('/^(?:TA)?[\d-]+$/', $safe_num)) {
        logMessage("Invalid input: contains non-integer characters");
        echo json_encode(['code' => 404, 'dmsg' => "<font color=red>Please enter only numbers and dashes</font>", 'msg' => '']);
        return;
    }
    $checking_num_query = "SELECT c.sim_phone_no,st.simno,c.from_date,c.order_no,c.activated,c.podno,c.handset_model,c.domainname,c.status,c.qrtext,c.is_kyc_required  FROM clienttrip c  JOIN sim_stock st ON st.sim_phone_no = c.sim_phone_no  WHERE c.status in ('Delivered', 'Delivery pending','esimDelivered','Pending','Shipped','Unshipped') AND ((c.domainname = 'tsim.mobi' AND st.simno LIKE '$safe_num%') OR (c.domainname != 'tsim.mobi' AND (st.simno LIKE '$safe_num%' OR c.order_no LIKE '$safe_num%'))) AND st.active = TRUE AND st.availability =FALSE AND c.booking_date >= (CURRENT_TIMESTAMP - INTERVAL '24 month');"; 
    $check = pg_query($checking_num_query);
    logMessage("Query in checking_num condition is : ".$checking_num_query);
    $query = pg_num_rows($check);
//CHecking COunt
	$count_query = $query;//no 1
//	 echo json_encode(['count_query' => $count_query]);
//	 echo json_encode(['code' => 200,'msg' => $count_query, 'dmsg' => '']);
  
  logMessage("Number of records the query returned is : ".$query);
    if($query >= 1){
        $all_rows = pg_fetch_all($check);
        $is_safe_num_found = false;
        foreach ($all_rows as $row) {
            if ($row['simno'] === $safe_num || $row['order_no'] === $safe_num) {
                $query_result = $row;
                $is_safe_num_found = true;
                break;
            }
        }
        if ($is_safe_num_found) {
            logMessage("Serial number is Complete");
            if ($query_result['handset_model'] == 'IN' && empty($query_result['order_no']) && strpos($query_result['podno'], 'FBA') === 0) {
                // Generate new order number starting with FBA and timestamp combination
                $new_order_no = 'FBA-' . date("YmdHis") . rand(1000, 9999) . '-' . $query_result['sim_phone_no'];
                // Update clienttrip table with the new order number
                $update_clienttrip_query = pg_query_params("UPDATE clienttrip SET order_no = $1 WHERE sim_phone_no = $2", array($new_order_no, $query_result['sim_phone_no']));
                $fba_order_insert_query = pg_query_params("INSERT INTO tsim_order_documents (order_no, documents_uploaded, no_of_documents_uploaded) VALUES ($1, 'f', 0)", array($new_order_no));
                logMessage("New Order Number: ".$new_order_no);
                logMessage("Update clienttrip query result: " . $update_clienttrip_query);
                logMessage("FBA order insert query result: " . $fba_order_insert_query);
                if ($update_clienttrip_query && $fba_order_insert_query) {
                    logMessage("Serial number is Complete, this is FBA customer new Orderid is generated: ".$new_order_no);
                    echo json_encode(['code' => 200, 'dmsg' => "<font color=green>Serial number is Complete</font>", 'res' => $query_result,'count_query' => $count_query, 'new_order_no' => $new_order_no]);
                    return;
                }
            }
            if(!empty($query_result["order_no"])){

                $amazonorder_check = "SELECT a.amazonorderid, c.sim_phone_no, c.qrtext, a.title  FROM clienttrip c  RIGHT JOIN amazon_order_items_view a ON c.order_no = a.amazonorderid  WHERE  a.amazonorderid = '" . $query_result["order_no"] . "' and a.emailqrcode=true and c.qrtext is not null  ORDER BY c.qrtext";
		
                $check_1 = pg_query($amazonorder_check);
                logMessage("Query in amazon_num_query condition is : ".$amazonorder_check);
                $amazonquery = pg_num_rows($check_1);
                logMessage("Number of records the amazonquery returned is : ".$amazonquery);
		if($amazonquery >= 1){
                    logMessage("Response code is 202");
                    echo json_encode(['code' => 202]);
                    return;
                }
		else	
		{
			//$amazonorder_check1 = "SELECT a.amazonorderid,c.simno,c.sim_phone_no, a.title,a.sellersku,c.from_date  FROM sim_stock_trip c  RIGHT JOIN amazon_order_items_view a ON c.order_no = a.amazonorderid  WHERE  a.amazonorderid = '".$query_result["order_no"]."' and a.sellersku in (select distinct sellersku from amazon_inventory_latest where sellersku ~* 'mob|tmo|att|unlimited-usa-sim-card-t|unlimited-usa-sim-t|unlimited-usa-can-mex-sim' and sellersku !~* 'lyca' and  sellersku ~ 'esim') order by c.simno";
			//$amazonorder_check1 = "SELECT DISTINCT a.amazonorderid, c.simno, c.sim_phone_no, a.title, a.sellersku, c.from_date FROM sim_stock_trip c RIGHT JOIN amazon_order_items_view a ON c.order_no = a.amazonorderid WHERE a.amazonorderid = '" . $query_result["order_no"] . "' AND (sellersku ~* 'mob|tmo|att|unlimited-usa-sim-card-t|unlimited-usa-sim-t|unlimited-usa-can-mex-sim' and sellersku !~* 'lyca' and  sellersku ~ 'esim') ORDER BY c.simno";

			 $amazonorder_check1 = "SELECT DISTINCT a.amazonorderid, c.simno, c.sim_phone_no, a.title, a.sellersku, c.from_date FROM sim_stock_trip c RIGHT JOIN amazon_order_items_view a ON c.order_no = a.amazonorderid and c.country=a.country WHERE a.amazonorderid = '" . $query_result["order_no"] . "' AND (sellersku ~* 'mob|tmo|att|unlimited-usa-sim-card-t|unlimited-usa-sim-t|unlimited-usa-can-mex-sim' and sellersku !~* 'lyca' and  sellersku ~ 'esim') ORDER BY c.simno";

			$check_2 = pg_query($amazonorder_check1);
	                logMessage("Query in amazon_num_query condition is : ".$amazonorder_check1);
        	        $amazonquery1 = pg_num_rows($check_2);
                	logMessage("Number of records the amazonquery for otasimcards returned is : ".$amazonquery1);
			
			$all_rows = pg_fetch_all($check_2);
			$is_safe_num_found = false;
			foreach ($all_rows as $row) {
				logMessage("Row: " . json_encode($row));
		        

			if($amazonquery1 == 1)
			{
		        	logMessage("Response code is 200 one order item record found");
                		echo json_encode(['code' => 200,  'dmsg' => "<font color=green>Correct</font>", 'res' => $row, 'count_query' => $amazonquery1]);
		                return;
			}
	                else if($amazonquery1 > 1)
			{
					
			            logMessage("Amazon Order Id is Complete.");
				    logMessage("Multiple Order Items found for the entered order number.");
			  	    logMessage("Code is 210, allowing user to select the order item to proceed furthur.");
			            echo json_encode(['code' => 210, 'dmsg' => "<font color=green>Entered number is valid</font>", 'count_query' => $amazonquery1, 'res' => $all_rows]);
		        	    return;
			}

			}//foreach loop
		}

            }
         logMessage("response is 200");
		if($count_query==1){
                      echo json_encode(['code' => 200, 'dmsg' => "<font color=green>Entered number is valid</font>",'count_query' => $count_query, 'res' => $query_result]);
        } else{
                        
			echo json_encode(['code' => 200, 'dmsg' => "<font color=green></font>", 'count_query' => $count_query ,'res' => $query_result]);
                  }
                return;
//if($count_query==1){
//    $count_response = array(
//        'code' => 200,
//        'dmsg' => "<font color=green>Entered number is valid</font>",
//        'count_query' => $count_query,
//        'res' => $query_result
//    );
//   
//    $count_response['dmsg'] .= "<font color=green> Correct</font>";
//    echo json_encode($count_response);
//} else {
//    echo json_encode(['code' => 200, 'dmsg' => "<font color=green></font>", 'count_query' => $count_query ,'res' => $query_result]);
//}
//return;
        } else {
            logMessage("Serial Number is not complete");
		if($count_query>=1){
            echo json_encode(['code' => 404, 'dmsg' => "<font color=green>Serial Number is not complete</font>",'count_query' => $count_query,  'msg' => '']);
            }
		else{
		echo json_encode(['code' => 404, 'dmsg' => "<font color=green>Serial Number is not complete</font>",'count_query' => $count_query,  'msg' => '']);
		}
		return;
        }
    }else {
        logMessage("Serial number is not valid");
        echo json_encode(['code' => 404, 'dmsg' => "<font color=red>Entered Number is not valid</font>",'count_query' => $count_query , 'msg' => '']);
        return;
    }
}


if ((isset($_POST['order_no']) && isset($_POST['t']))){
    logMessage("GET: " . json_encode($_GET));
logMessage("POST: " . json_encode($_POST));
	$order = pg_escape_string($_POST['order_no']);
    logMessage("OrderNumber is :" .$order);
    logMessage("value of t is: ".$_POST['t']);
   	 $count_query = $_POST['count_query'];
	 $get_simno = pg_query($DBConn, "select simno from sim_stock where sim_phone_no in (select ct.sim_phone_no from clienttrip ct join sim_stock st on st.sim_phone_no = ct.sim_phone_no where ct.order_no='$order' and ct.status in ('Delivery pending','Delivered','Shipped','Pending','Unshipped') limit 1);");
    logMessage("select simno from sim_stock where sim_phone_no in (select ct.sim_phone_no from clienttrip ct join sim_stock st on st.sim_phone_no = ct.sim_phone_no where ct.order_no='$order' and ct.status in ('Delivery pending','Delivered','Shipped','Pending','Unshipped') limit 1);");
    $fetch_simno = pg_fetch_assoc($get_simno);
    $simno = $fetch_simno['simno'];
    $row = array(
        "simno" => $simno
    );
	logMessage('t');
	logMessage($simno);
	logMessage($row);
logMessage("WOrks");
    if(empty($row['simno'])){
        logMessage("101");
	echo json_encode(['code' => 101]);
        return;
    }
    $docs = pg_query($DBConn, "SELECT distinct no_of_documents_uploaded,documents_uploaded from tsim_order_documents where order_no='$order' limit 1");
    $fetchdocs = pg_fetch_assoc($docs);
    logMessage("Fetch docs query: SELECT distinct no_of_documents_uploaded,documents_uploaded from tsim_order_documents where order_no ='$order' limit 1");
    $imei_eid_query = pg_query($DBConn, "SELECT ct.imei, ct.eid, ct.tripid FROM clienttrip ct JOIN sim_stock st ON st.sim_phone_no = ct.sim_phone_no  WHERE  ct.order_no ='$order' and st.simno = '{$row['simno']}'");
    $fetch_imei_eid = pg_fetch_assoc($imei_eid_query);
    logMessage("SELECT ct.imei, ct.eid, ct.tripid  FROM clienttrip ct  JOIN sim_stock st ON st.sim_phone_no = ct.sim_phone_no  WHERE  ct.order_no ='$order' and st.simno = '{$row['simno']}'");
    if ($fetchdocs['documents_uploaded'] == 't'){
        logMessage("Documents are Verfied...... , value is t for documents_uploaded column");
        echo json_encode(['code' => 209,'msg' => $row, 'count_query' => $count_query ,'info' => $fetch_imei_eid['tripid']]);
        return;
    }
    else if ($fetchdocs['no_of_documents_uploaded'] == 3 && $fetchdocs['documents_uploaded'] == 'f' ){
        if ($_POST['t'] == 'amazon'){
            $get_details = pg_query($DBConn, "select from_date,clientname,emailadd,mobno from clienttrip where order_no='$order' and status not in ('Canceled','Returned')");
            $fetch_details = pg_fetch_assoc($get_details);
            $clientname2 = $fetch_details['clientname'];
            $mob2 = $fetch_details['mobno'];
            $emaill_add2 = $fetch_details['emailadd'];
            $row = array(
                "t" => "amazon",
                "from_date" => $fetch_details['from_date'],
                "simno" => $fetch_simno['simno'],
                "clientname" => $clientname2,
                "emailadd" => $emaill_add2,
                "mobno" => $mob2
            );
        }
        elseif ($_POST['t'] == 'tsim'){
            $get_details = pg_query($DBConn, "select json from tsim_orders where order_no='$order'");
            $fetch_details = pg_fetch_assoc($get_details);
            $jsondata = json_decode($fetch_details['json'], true);
            $clientname2 = $jsondata['order']['billing_address']['first_name'] . " " . $jsondata['order']['billing_address']['last_name'];
            $emaill_add2 = $jsondata['order']['billing_address']['email'];
            $mob2 = $jsondata['order']['billing_address']['phone'];
	   	
            $row = array(
                "t" => "tsim",
                "simno" => $fetch_simno['simno'],
                "clientname" => $clientname2,
                "emailadd" => $emaill_add2,
                "mobno" => $mob2,
		"total_esims" => $total_esims
            );
        }
        $get_data1 = sprintf("SELECT distinct order_no, documents_uploaded, no_of_documents_uploaded, last_update, comments, doc1, doc2, doc3  from tsim_order_documents where order_no in (select order_no from clienttrip where sim_phone_no in ( select sim_phone_no from sim_stock where simno = '%s') and status not in ('Canceled','Returned'))", $row['simno']);
        logMessage($get_data1);
        $getResult1 = pg_query($get_data1);
        $row1 = pg_fetch_assoc($getResult1);
        $count_docs=0;
        if($row1['doc1']){
            $doc1 = $row1['doc1'];
            $count_docs++;
        }
        if($row1['doc2']){
            $doc2 = $row1['doc2'];
            $count_docs++;
        }
        if($row1['doc3']){
            $doc3 = $row1['doc3'];
            $count_docs++;
        }
        $row['pass1'] = $doc1;
        $row['pass2'] = $doc2;
        $row['visa'] = $doc3;
        if($count_docs === 3){
            logMessage("Response code is 201");
            echo json_encode(['code' => 201, 'msg' => $row, 'count_query'=> $count_query , 'info' => $fetch_imei_eid['tripid']]);
            return;//removed here
        }
        else{
            logMessage("Response code is 2 208");
            echo json_encode(['code' => 208,'test' => '1','count_query'=> $count_query, 'msg' => $row, 'info' => $fetch_imei_eid['tripid']]);
            return;//removed here
        }
    }
    else if ($fetchdocs['no_of_documents_uploaded'] < 3 && $fetchdocs['documents_uploaded'] == 'f'){
        logMessage("Response code is 3 208");
logMessage("THIS IS MSG".$row);

        echo json_encode(['code' => 208 ,'count_query'=> $count_query , 'test' => '1','msg' => $row, 'info' => $fetch_imei_eid['tripid']]);
        return;//removed here
    }
    else{
        logMessage("Response code is 101");
        echo json_encode(['code' => 101]);
        return;
    }
}

//DB checking
if (isset($_POST['serial_no']) && ((strlen($_POST['serial_no']) >= 5 && (strlen($_POST['serial_no']) <= 40)))){
    logMessage("----------------- Serial number if Condition check starts here ---------------------");
    $number = $_POST['serial_no'];
    $order_number = $_POST['orderNumber'];
    logMessage("Serial Number: ".$_POST['serial_no']);
    logMessage("Number: ".$number);
	$count_query = $_POST['count_query']; //no 2
	$countQuery = $_POST['count_query'];	
	
	 logMessage("Incoming POST: " . json_encode($_POST));
    $count_query = isset($_POST['count_query']) ? $_POST['count_query'] : null;
    $query="SELECT c.sim_phone_no, st.simno, c.from_date, c.order_no, c.tripid, st.activation_reqd, c.is_kyc_required FROM clienttrip c join sim_stock st on c.sim_phone_no = st.sim_phone_no  WHERE st.sim_phone_no = c.sim_phone_no  AND c.status IN ('Delivered', 'Delivery pending','Shipped','Pending','Unshipped')  AND st.simno='$number' AND st.active = true AND st.availability = false";
   
    $check = pg_query($query);
    logMessage("------------Query---------------------:".$query);
    $query = pg_num_rows($check);
	$count_query = $_POST['count_query'];  
    $details_fetch = pg_fetch_assoc($check);
    $ono = $details_fetch['order_no'];
    if ($query == 1 && $details_fetch['is_kyc_required'] == 't') {
        logMessage(" is_kyc_required ---------------> True");
        logMessage("This is INDIA user");
        $simno_db = $details_fetch['simno'];
        if ($simno_db == $number){
            $getData = sprintf("SELECT c.activated, s.activation_reqd, st.passport_exp_date, st.clientname AS passportname, st.passport_no, st.passport_city, c.order_no, s.simno, COALESCE(c.clientname, st.clientname) AS clientname, COALESCE(c.emailadd, st.emailadd) AS emailadd, COALESCE(c.mobno, st.mobno) AS mobno, c.from_date, c.mis_user,c.tripid,c.imei,c.eid FROM clienttrip c  JOIN sim_stock s ON s.sim_phone_no = c.sim_phone_no  JOIN sim_user st ON st.username = c.username  WHERE c.status NOT IN ('Canceled', 'Returned') AND s.simno='%s' and c.order_no='%s'",$number,$ono);
            logMessage($getData);
            $getResult = pg_query($getData);
            $qu = pg_num_rows($getResult);
            if ($qu == 1){
                if ($_POST['t'] == 'tsim'){
                    $get_order = pg_fetch_assoc($getResult);
                    $order_id = $get_order['order_no'];
                    $pp_no = $get_order['passport_no'];
                    $pp_city = $get_order['passport_city'];
                    $act_flag = $get_order['activation_reqd'];
                    $mis_user = $get_order['mis_user'];
                    if ($mis_user == "services"){
                        $email_add = $get_order['emailadd'];
                        $mob = $get_order['mobno'];
                    }
                    else{
                        $Query = "select json from tsim_orders where order_no='$order_id'";
                        logMessage("---------------------------------------------------");
                        logMessage("Query: ".$Query);
                        logMessage("---------------------------------------------------");
                        $get_details = pg_query($DBConn, $Query);
                        $fetch_details = pg_fetch_assoc($get_details);
                        logMessage("JSON: ". $fetch_details);
                        $jsondata = json_decode($fetch_details['json'], true);
                        $email_add = $jsondata['order']['billing_address']['email'];
                        $mob = $jsondata['order']['billing_address']['phone'];
                    }
                    $clientname = $get_order['passportname'];
                    $p_exp = $get_order['passport_exp_date'];
                    $total_items = $jsondata['order']['total_line_items_quantity'];
		    logMessage("Total items below:");
		    logMessage("total items: ".$total_items);
		    $esim_sku=array();
		    if($total_items >1)
		    {

foreach ($jsondata['order']['line_items'] as $index => $item) {
    if (strpos($item['sku'], 'esim') !== false) {
        $sku = $item['sku'];
        $id = $item['id']; // Assuming 'id' is present in the line item
        logMessage("Found eSIM SKU: " . $sku . " with ID: " . $id);

        // Add details for this SKU to an indexed array
        $esim_details[] = [
            'sku' => $sku,
            'id' => $id,
            'constructed_name' => '',
            'country' => '',
        ];
        $total_esims++;
    }
}

// Second loop: Only process SKUs if total eSIMs > 1

if ($total_esims > 1) {
    foreach ($esim_details as $index => $details) { // Use $index for array traversal
        $sku = $details['sku'];
        $id = $details['id'];

        // Query to check if the SKU and ID combination exists
        $query = sprintf(
            "SELECT * FROM tsim_orders_line_items WHERE sku ~* 'mob|tmo|att|unlimited-usa-sim-card-t|unlimited-usa-sim-t|unlimited-usa-can-mex-sim' AND sku !~* 'lyca' AND order_no = '%s' AND id = '%s';",
            pg_escape_string($order_id),
            pg_escape_string($id)
        );
        logMessage("Executing query for SKU: {$sku}, ID: {$id}");
        logMessage("query is:" . $query);

        $result = pg_query($query);

        if ($result && pg_num_rows($result) > 0) {
            logMessage("Query returned results for SKU: {$sku}, ID: {$id}");

            // Construct the SIM card name
            foreach ($jsondata['order']['line_items'] as $item) {
                if ($item['sku'] === $sku && $item['id'] === $id) {
                    $name = $item['name'] ?? '';
                    $validity_label = '';
                    $validity_value = '';
                    $data_label = '';
                    $data_value = '';

                    foreach ($item['meta'] as $meta) {
                        if (isset($meta['label'], $meta['value'])) {
                           // logMessage("Processing meta: " . json_encode($meta));
                            if (stripos($meta['label'], 'validity') !== false) {
                                $validity_label = $meta['label'];
                                $validity_value = $meta['value'];
                            } elseif (stripos($meta['label'], 'data') !== false) {
                                $data_label = $meta['label'];
                                $data_value = $meta['value'];
                            }
                        }
                    }

                    // Prepare the constructed name
                    $validity_string = $validity_label ? "{$validity_label}: {$validity_value}" : '';
                    $data_string = $data_label ? "{$data_label}: {$data_value}" : '';
                    $constructed_name = $name;
                    if ($validity_string && stripos($name, $validity_string) === false) {
                        $constructed_name .= ', ' . $validity_string;
                    }
                    if ($data_string && stripos($name, $data_string) === false) {
                        $constructed_name .= ', ' . $data_string;
                    }

                    // Update the constructed name in the array
		logMessage("INDEX IS ------------------".$index."or".$esim_details[$index]);
                    $esim_details[$index]['constructed_name'] = $constructed_name;
                    logMessage("Final constructed name for SKU {$sku} (ID: {$id}): {$constructed_name}");
                }
            }
        } else {
            logMessage("No valid results for SKU: {$sku}, ID: {$id}. Skipping label generation.");
            unset($esim_details[$index]); // Remove the item by index
        }
    }
}
logMessage("Count of eSIM details: " . count($esim_details));

if (count($esim_details)>1){
foreach ($esim_details as $index => $details) {
logMessage("Index: {$index}");
foreach ($details as $key => $value) {
    if (is_array($value)) {
	logMessage("  {$key}: " . json_encode($value)); // Log arrays as JSON
    } else {
	logMessage("  {$key}: {$value}"); // Log scalar values
    }
}
logMessage("--------------"); // Separator for readability
}

//run query select sim_sku_country('%sku');
    // Step 2: Query the database for details using the SKUs   
			//fetch sku from esim_details and loop all sku and store it in a list

foreach ($esim_details as &$details) { // Iterate through each detail
    $sku = $details['sku'];

    // Run the query for each SKU and get the country
    $sku_query = sprintf("SELECT sim_sku_country('%s')", pg_escape_string($sku));
    logMessage("This is the query for SKU: " . $sku_query);

    $sku_res = pg_query($sku_query);
    if (!$sku_res) {
        logMessage("Query failed for SKU {$sku}");
        continue;
    }

    $row = pg_fetch_assoc($sku_res);

    // Store the country in the 'country' field of the corresponding eSIM detail
    $details['country'] = $row['sim_sku_country'] ?? '';
}

$query = "SELECT s.simno, c.tripid, c.imei, c.eid, s.country
          FROM clienttrip c 
          JOIN sim_stock s ON s.sim_phone_no = c.sim_phone_no  
          JOIN sim_user st ON st.username = c.username 
          WHERE c.status NOT IN ('Canceled', 'Returned') 
          AND c.order_no = '$order_id'";

logMessage("Executing query for: " . $query);
$res = pg_query($query);

if (!$res) {
    logMessage("Query failed to fetch details for order {$order_id}");
    return;
}

$details_by_country = []; // Initialize an indexed array to group by country

while ($row = pg_fetch_assoc($res)) {
    $country = $row['country'];

    // Initialize the country entry if it doesn't exist in the array
    if (!isset($details_by_country[$country])) {
        $details_by_country[$country] = [
            'tripid' => [],
            'imei' => [],
            'eid' => [],
            'constructed_name' => [] // Store constructed names as a list (array)
        ];
    }

    // Add details to the country entry
    $details_by_country[$country]['tripid'][] = $row['tripid'];
    $details_by_country[$country]['imei'][] = $row['imei'];
    logMessage("Imei is ".$row['imei']."eid is".$row['eid']."trip id is ".$row['tripid']);
	$details_by_country[$country]['eid'][] = $row['eid'];

    // Find the constructed_name from $esim_details for this country
    foreach ($esim_details as $esim_detail) {
        // Check for matching country and ensure constructed_name is set
        //logMessage("First value_________________ " . $esim_detail['country']);
		//logMessage("CONSTRUCTED NAME_________________ " . $constructed_name);
		//logMessage("CONSTRUCTED NAME_________________ " . $constructed_name);

		if ($esim_detail['country'] === $country && !empty($esim_detail['constructed_name'])) {
            $constructed_name = $esim_detail['constructed_name'];
            //logMessage("CONSTRUCTED NAME_________________ " . $constructed_name);

            // Add the constructed name to the list for this country if not already present
            if (!in_array($constructed_name, $details_by_country[$country]['constructed_name'])) {
                $details_by_country[$country]['constructed_name'][] = $constructed_name; // Avoid duplicates
            }
        }
    } 
    logMessage("Mapped details for country {$country}: " . json_encode($row));
}
// Initialize accumulators as empty arrays
$all_tripids = $all_imeis = $all_eids = $all_constructed_names = [];

foreach ($details_by_country as $country => $details) {
    // Check for empty elements and mark them
    $tripid = $details['tripid'];
    $imei = $details['imei'];
    $eid = $details['eid'];
    $constructed_name = $details['constructed_name'];

    $all_tripids[] = !empty($tripid) ? implode(",", $tripid) : "0";
    $all_imeis[] = !empty($imei) ? implode("_", $imei) : "0";
    $all_eids[] = !empty($eid) ? implode("_", $eid) : "0";
    $all_constructed_names[] = !empty($constructed_name) ? implode("_", $constructed_name) : "0";
}

// Combine all accumulated values into single strings
$tripid_csv = implode(",", $all_tripids);
$imei_csv = implode("_", $all_imeis);
$eid_csv = implode("_", $all_eids);
$labels_csv = implode("_", $all_constructed_names);

// Log the final results with empty markers
logMessage("SUPER FINAL DETAILS:");
logMessage("Trip IDs: " . ($tripid ?: 'None'));
logMessage("IMEIs: " . ($imei ?: 'None'));
logMessage("EIDs: " . ($eid ?: 'None'));
logMessage("Constructed Names: " . ($labels ?: 'None'));
}
}
    
		$log_details = array(
                        'order_id' => $order_id,
                        'SIM Number' => $get_order['simno'],
                        'Client Name' => $clientname,
                        'Email' => $email_add,
                        'Mobile Number' => $mob,
                        'From Date' => $get_order['from_date'],
                        'Passport Expiry Date' => $p_exp,
                        'Tripid' => $get_order['tripid'],
                        'IMEI' => $get_order['imei'],
                        'EID' => $get_order['eid'],
			'total Esims' => count($esim_details),
			'All esim Sku' => $esim_sku
			
                    );
//pointer1
	            logMessage('Tsim Data Details:', $log_details);
                    $activated = $get_order['activated'];
                    $imei = $get_order['imei'];
                    $eid = $get_order['eid'];
                    $tripid = $get_order['tripid'];
                    $row = array(
                        "t" => "tsim",
                        "from_date" => $get_order['from_date'],
                        "passport_exp_date" => $p_exp,
                        "clientname" => $clientname,
                        "emailadd" => $email_add,
                        "mobno" => $mob,
                        "simno" => $get_order['simno'],
                        "passport_no" => $pp_no,
                        "passport_city" => $pp_city,
                        "act_flag" => $act_flag,
			"total_esims" => count($esim_details),
			"esim_sku" => $constructed_names
                    );
                }
                else{
                    $row = pg_fetch_assoc($getResult);
                    $activated = $row['activated'];
			$imei = $row['imei'];
                    	$eid = $row['eid'];
                    	$tripid = $row['tripid'];
                                      $log_details = array(
                        'SIM Number' => $get_order['simno'],
                        'Client Name' => $clientname,
                        'Email' => $email_add,
                        'Mobile Number' => $mob,
                        'From Date' => $get_order['from_date'],
                        'Tripid' => $get_order['tripid'],
                        'IMEI' => $get_order['imei'],
                        'EID' => $get_order['eid']
                    );
                    logMessage('Details:', $log_details);
                }
                $getData2 = sprintf("SELECT DISTINCT t.order_no, t.documents_uploaded, t.no_of_documents_uploaded, t.last_update, t.comments, t.doc1, t.doc2, t.doc3 FROM tsim_order_documents t JOIN clienttrip ct ON t.order_no = ct.order_no JOIN sim_stock ss ON ct.sim_phone_no = ss.sim_phone_no WHERE ss.simno = '%s' and ct.order_no='%s' AND ct.status NOT IN ('Canceled', 'Returned')",$row['simno'],$ono);
                $getResult2 = pg_query($getData2);
                logMessage("Query is: ". $getData2);
                $qu2 = pg_num_rows($getResult2);
                if ($qu2 == 1){
                    $row2 = pg_fetch_assoc($getResult2);
                    $cnt=0;
                    if($row2['doc1']){
                        $doc1 = $row2['doc1'];
                        $cnt=$cnt+1;
                    }
                    if($row2['doc2']){
                        $doc2 = $row2['doc2'];
                        $cnt=$cnt+1;
                    }
                    if($row2['doc3']){
                        $doc3 = $row2['doc3'];
                        $cnt=$cnt+1;
                    }
                    $row['pass1'] = $doc1;
                    $row['pass2'] = $doc2;
                    $row['visa'] = $doc3;
					if($total_esims > 1){
					$row['tripid'] = $tripid_csv;
                    $row['imei'] = $imei_csv;
                    $row['eid']=$eid_csv;
					$row['labels']=$labels_csv;

					}
					else{
                    $row['tripid'] = $tripid;
                    $row['imei'] = $imei;
                    $row['eid']=$eid;
					}
                    $row['order_no']=$order_id;
                    $log_details = array(
                        'orderNumber' => $order_id,
                        'no_of_documents_uploaded' => $row2['no_of_documents_uploaded'],
                        'documents_uploaded' => $row2['documents_uploaded'],
                        'emailadd' => $row['emailadd'],
                        'clientname' => $row['clientname'],
                        'from_date' => $row['from_date'],
                        'activated' => $activated,
                        'pass1' => $row['pass1'],
                        'pass2' => $row['pass2'],
                        'visa' => $row['visa'],
                        'Tripid' => $row['tripid'],
                        'IMEI' => $row['imei'],
                        'EID' => $row['eid'],
						'total_esims' => $row['total_esims'],
						'esim_sku' => $esim_sku
                    );
                    logMessage("", $log_details);
                    
                    if($activated == 't' && $row2['documents_uploaded'] == 't'){
                        logMessage("Response code is 205");
                        logMessage("Sim is Activated".json_encode($row2,JSON_PRINT_PRETTY));
                        echo json_encode(['code' => 205, 'dmsg' => '', 'msg' => $row]);
                        exit();
                    }
                    else if($row2['documents_uploaded'] == 't' &&($cnt == $row2['no_of_documents_uploaded'])
                        && !empty($row['emailadd']) && !empty($row['clientname']) && !empty($row['from_date'])){
                        logMessage("Response code is 205");
                        logMessage(' Checked All data Present in DB And Uploaded Files '. json_encode($row2, JSON_PRETTY_PRINT));
                        echo json_encode(['code' => 205, 'dmsg' => '', 'msg' => $row]);
                        exit();
                    }
                    else if (!empty($row['emailadd']) || !empty($row['clientname'])){
                    
			  logMessage("Response code is 203");
                        if($countquery==1){
                        logMessage("Unique query",$countquery);
                        echo json_encode(['code' => 203,'count_query'=>$count_query,'dmsg' => '<font color= green>Correct</font>', 'msg' => $row]);
                    }
                        else

                        {
                        logMessage("More than 1 Similar Query found :",$countquery);
                        echo json_encode(['code' => 203,'count_query'=>$count_query,'msg' => $row]);
                        }}

			else{
                        logMessage("Response code is 200");
                        echo json_encode(['code' => 200,'count_query'=>$count_query , 'dmsg' => '<font color= green>Correct</font>', 'msg' => $row, 'activated'=> $activated]);
                    }
                }
                else{
                    logMessage("Response code is 200");
                    echo json_encode(['code' => 200,'count_query'=>$count_query , 'dmsg' => '<font color=green>Correct</font>', 'msg' => '']);
                }
            }
            else{
                logMessage("Response code is 202");
                echo json_encode(['code' => 202, 'msg' => 'Data fetching problem, please try again later']);
            }
        }
        else{
            $details = array(
                'Number' => $number,
                'Simno_db' => $simno_db,
                'Comparison Result' => ($simno_db != $number ? "Not Equal" : "Equal")
            );
            logMessage("",$details);
            echo json_encode(['code' => 201, 'dmsg' => "<font color=green>Serial number is not Complete</font>", 'msg' => '']);
        }
    }
    else if($query == 1 && $details_fetch['is_kyc_required'] == 'f'){
        logMessage(" is_kyc_required ---------------> False");
        logMessage("-------------MOBI USER-----------------");
        $simno_db = $details_fetch['simno'];
        if($simno_db==$number){
            $activation_reqd=$details_fetch['activation_reqd'];
            if ($activation_reqd == 'f'){
                echo json_encode(['code'=>403, 'dmsg'=>'This card is preloaded, no need to activate.','msg'=>'']);
                return;
            }
            else{
                logMessage("Is it USA checking for that ---------> ");
                logMessage("Sim NO: ".$details_fetch['simno']);
                logMessage("From Date: ".$details_fetch['from_date']);
                logMessage("Trip ID: ".$details_fetch['tripid']);
                $usacheck;
                if(strpos( strtolower($details_fetch['tripid']), 'usa' ) !== false){
                    logMessage("This is USA as per tripid");
                    $usacheck='usa';
                }
                $getData = sprintf("SELECT ct.sim_phone_no, ct.clientname, ct.emailadd, ct.mobno, ct.from_date, ct.add_pinno, ct.activated, ct.tripid,ct.imei,ct.eid,ct.order_no  FROM clienttrip AS ct JOIN sim_stock AS ss ON ct.sim_phone_no = ss.sim_phone_no WHERE ss.simno='%s' AND ct.status IN ('Delivered','Delivery pending','Shipped','Pending','Unshipped')", $number);
                logMessage(' Query: ' . $getData);
                $getResult = pg_query($getData);
                $qu = pg_num_rows($getResult);
                $row = pg_fetch_assoc($getResult);
/*	
		$getCount = sprintf("SELECT tripid from clienttrip where order_no =%s",$row['order_no']);
		//$getCount = sprintf("SELECT tripid from clienttrip where sim_phone_no='%s' and status in ('Delivered','Delivery pending')",$row['sim_phone_no']);
		logMessage("select query".$getCount);
		$result = pg_fetch_assoc($getCount);

		if ($result && $result->num_rows > 0) {
		    // If eSIM order exists, proceed
		    $esimCount = $result->num_rows; // Assuming this determines the total eSIMs
		    logMessage("Total eSIMs found: " . $esimCount);
		}
 */
                if($qu==1 && $row[activated] == 'f'){
                    logMessage('Your Data Already Present:');
                    logMessage("Sim Phone No: " . $row['sim_phone_no']);
                    logMessage("Client Name: " . $row['clientname']);
                    logMessage("Email: " . $row['emailadd']);
                    logMessage("Mobile No: " . $row['mobno']);
                    logMessage("From Date: " . $row['from_date']);
                    logMessage("Pin No: " . $row['add_pinno']);
                    logMessage("Tripid: " . $row['tripid']);
                    logMessage("IMEI: " . $row['imei']);
                    logMessage("EID: " . $row['eid']);
                    logMessage("Amazonorderid : ". $row['order_no']);
                    logMessage('----------------------------');
                    if(!empty($row['emailadd']) && !empty($row['clientname']) && !empty($row['from_date'])){
                        logMessage("Correct , response code is 203");
                        echo json_encode(['code'=>203, 'msg'=>$row,'dmsg'=>"<font color=green>Correct</font>"]);
                        return;
                    }
			else{
//                          if ($count_query) {
//                                  logMessage($query);
//                                   logMessage("Correct, response code is 200 only 1 query");
//                                  echo json_encode(['code' => 200,'dmsg'=>"<font color=green>Correct</font>", 'info' => $row, 'msg' => $usacheck ]);
//                                  }
//                          return;
  				if($count_query==1) {
                          	logMessage("Correct , response code is 200");
                          	echo json_encode(['code'=>200, 'info'=>$row, 'dmsg'=>"<font color=green>Correct</font>",'msg'=>$usacheck]);
                          	return;
					}
				else{
					logMessage("Correct , response code is 200");
                                echo json_encode(['code'=>200, 'info'=>$row, 'msg'=>$usacheck]);
                                return;
                      }
                  }
}
		else{
                    logMessage('Data Already Present:');
                    logMessage("Sim Phone No: " . $row['sim_phone_no']);
                    logMessage("Client Name: " . $row['clientname']);
                    logMessage("Email: " . $row['emailadd']);
                    logMessage("Mobile No: " . $row['mobno']);
                    logMessage("From Date: " . $row['from_date']);
                    logMessage("Pin No: " . $row['add_pinno']);
                    logMessage("IMEI: " . $row['imei']);
                    logMessage("EID: " . $row['eid']);
                    logMessage('Sim is already activated');
                    echo json_encode(['code'=>202, 'dmsg'=>"<font color=red>Sim card is already activated </font>",'msg'=>'']);
                    return;
                }
            }
        }
        else{
            logMessage("Serial Number is not Complete");
            echo json_encode(['code'=>201,'dmsg'=>"<font color=green>Serial number is not complete</font>",'msg'=>'']);
            return;
        }
    }
    else{
        logMessage('$query!=1 && Serial number is not valid ');
        echo json_encode(['code' => 201, 'dmsg' => "<font color=green>Serial number is not valid</font>", 'msg' => '']);
    }
    exit();
}
else if (strlen($_POST['serial_no']) >= 40){
    logMessage("Serial Number is not valid .length is above 40");
    echo json_encode(['code' => 400, 'dmsg' => 'Serial number is not valid', 'msg' => '']);
}

function isValidInteger($value) {
    return ctype_digit($value);
}

function isValidMobileNumber($value) {
    return preg_match('/^[0-9+\-]+$/', $value);
}


function sanitizeVariable($value) {
    // Remove HTML and PHP tags
    $value = strip_tags($value);
    // Replace specific characters with an empty string
    $value = str_replace(array("'", '"', ';', '<', '>','<<', '>>', '--','?','{','}','[',']','(',')','&','$'), '', $value);
    // Optionally, you can also encode special characters to HTML entities
    $value = htmlspecialchars($value, ENT_QUOTES, 'UTF-8');
    // Remove any extra spaces
    $value = trim($value);
    return $value;
}

if (isset($_POST['json'])){
    $data = json_decode($_POST['json'], true);
    logMessage(' POST Data- ' . json_encode($_POST['json'],JSON_PRETTY_PRINT));

	$passport_no = sanitizeVariable($data['passport_no']);
    $passport_city = sanitizeVariable($data['passport_city']);
    $email = filter_var($data['email'], FILTER_SANITIZE_EMAIL);
    $simSerialNo = $data['serialNo'];
    $simSerialNo = trim($simSerialNo);
    $ts = $data['ts'];
	if($ts){
	$total_esims = $data['total_esims'];
	logMessage("total_esims: " . $total_esims);	
	if($total_esims>1)
	$eid=$data['eid'];
	$imei=$data['imei'];
	$tripid=$data['tripid'];
	
    logMessage("Tripid: " . $tripid);
    logMessage("IMEI: " . $imei);
    logMessage("EID: " . $eid);
	}
	$Query = "SELECT c.domainname, c.from_date FROM clienttrip c JOIN sim_stock s ON c.sim_phone_no = s.sim_phone_no WHERE s.simno = '$simSerialNo'";
    logMessage('Query for getting activation date - ' . $Query);
    $ExecuteQuery = pg_query($DBConn, $Query);
    if ($ExecuteQuery) 
    {
    	$gotrow = pg_fetch_assoc($ExecuteQuery);

    	if ($gotrow) 
	{
        	$Activation_date_from_db = $gotrow['from_date'];
	        $Domain = $gotrow['domainname'];

	        logMessage('Activation Date from database : ' . $Activation_date_from_db);
        	logMessage('Domain name : ' . $Domain);

	/*	{
            		$activationDate = $Activation_date_from_db;
    			$arr = explode("-", $activationDate);
			//Pointer 1
			
        	} */

	        if ($ts != 'tsim') 
		{
            		$activationDate = $data['date'];
			logMessage('It is not tsim getting date from json Activation Date : '.$activationDate);
			logMessage('Ts is :'.$ts);
			$dateObject = DateTime::createFromFormat('m/d/Y', $activationDate);
			if ($dateObject) {
			    $activationDate = $dateObject->format('Y-m-d');
			    logMessage( "PostgreSQL Date Format: " . $activationDate);
			} else {
			    logMessage( "Invalid date format!");
			}

				
		}
		else
		{
			$activationDate = $Activation_date_from_db;
			logMessage('Domain is tsim.in activation date does not matter but this is date :'.$activationDate);
		}
        	
    	} 
	else 
	{
        	logMessage('No data found for the query.');
    	}
     } 
     else 
     {
    	logMessage('Query execution failed: ' . pg_last_error($DBConn));
     }

    $imei = $data['imei'];
    $eid = $data['eid'];
    $mobileno = $data['mobileno'];
    $fname = sanitizeVariable($data['fname']);
    $expdate = $data['expdate'];
    logMessage('Date : ' . $activationDate);
    /*logMessage('expiry date - ' . $expdate);
    logMessage('arr 2 - ' . $arr[2]);
    logMessage('arr 0 - ' . $arr[0]);
    logMessage('arr 1 - ' . $arr[1]);
	*/
    if($total_esims >1)
	{	logMessage("Inside multiple esims");
		$imeiValues = explode('_', $imei);
		$eidValues = explode('_', $eid);
		$tripidValues = explode(',',$tripid);
		logMessage("IMEI VALUES".$imeiValues);
		logMessage("EID VALUES".$eidValues);
		logMessage("TRIPID VALUES".$tripidValues);

		// Validate each IMEI value
		foreach ($imeiValues as $imeiValue) {
			if (!ctype_digit($imeiValue) || strlen($imeiValue) < 15) {
				$errors[] = 'Invalid IMEI';
				break; // If one IMEI is invalid, stop further checks
			}
		}

		// Validate each EID value
		foreach ($eidValues as $eidValue) {
			if (!ctype_digit($eidValue) || strlen($eidValue) < 32) {
				$errors[] = 'Invalid EID';
				break; // If one EID is invalid, stop further checks
			}
			}
	}
	else{
		logMessage("Inside single esims");

		if ($imei!='' && (!ctype_digit($imei) || strlen($imei)<15)) {
        	$errors[] = 'Invalid IMEI';
    		}
    
    	if ($eid!='' && (!ctype_digit($eid) || strlen($eid)<32)) {
        	$errors[] = 'Invalid EID ';
    		}
    	}
    if ($mobileno!='' && !isValidMobileNumber($mobileno)) {
        $errors[] = 'Invalid Mobile Number: Must contain only digits, +, or -.';
    	}
    
    if (!empty($errors)) {
        foreach ($errors as $error) {
            echo htmlspecialchars($error, ENT_QUOTES, 'UTF-8') . '<br>';
            logMessage($error);
        }
        exit;
    }
    /*
    if ($ts != 'tsim'){
        $activationDate = $arr[2] . '-' . $arr[0] . '-' . $arr[1] . ' 00:00:00';
    }
    else{
        $activationDate = $arr[2] . '-' . $arr[0] . '-' . $arr[1];
    }
*/
    logMessage(' Modified Date - ' . $activationDate);
    logMessage(' POST Data- ' . json_encode($_POST['json'],JSON_PRETTY_PRINT));
    if (!empty($passport_no) && !empty($passport_city) && (!empty($email) && !empty($fname) && !empty($activationDate)) || (!empty($email) && !empty($fname)) || (!empty($fname) && !empty($activationDate)))
    {
        if (!empty($expdate) && strtotime($expdate) !== false){
            	$update_sim_user = sprintf("update sim_user set passport_exp_date='%s',passport_no='%s',passport_city='%s' where username in (select username from clienttrip where sim_phone_no in (select sim_phone_no from sim_stock where simno = '%s') and status not in ('Canceled','cardlost'))", $expdate, $passport_no, $passport_city, $simSerialNo);
            	logMessage(' Sim user query  ' . $update_sim_user);
            	$execute_sim_user = pg_query($update_sim_user);
	 	if($execute_sim_user)
		{logMessage("Query successful");}
		else
		{logMessage("Query failed");}
        }
        else{
            logMessage("Show him alert for expiry");
	}
	/*
        	if($ts=='tsim'){
            	$update_clientname = sprintf("update sim_user set clientname='%s' where username in (select username from clienttrip where sim_phone_no in (select sim_phone_no from sim_stock where simno = '%s') and status not in ('Canceled','Returned'))",$fname,$simSerialNo);
            	logMessage('update client name '.$update_clientname);
		$result = pg_query($update_clientname);
		if ($result)
		{logMessage("Query successful");}
		else{logMessage("Query failed");}

		if($total_esims>1){
			logMessage("Myltiple update");
			//loop for no.of esims and update based on their tripid
			foreach ($imeiValues as $index => $imei) {
            $eid = $eidValues[$index] ?? null;
            $tripid = $tripidValues[$index] ?? null;
			if (!$imei || !$eid || !$tripid) {
                logMessage("Missing data for index $index: IMEI ($imei), EID ($eid), TripID ($tripid)");
                continue;
            }
         			$update_clienttrip = sprintf("
    UPDATE clienttrip 
    SET comment = CONCAT(comment, ' Tsim customer, IMEI: %s, EID: %s'),
        imei = '%s', 
        eid = '%s' 
    WHERE tripid = '%s' 
    AND status NOT IN ('Canceled', 'cardlost')
", $imei, $eid, $imei, $eid, $tripid);
			logMessage('Executing query: ' . $update_clienttrip);	

		$result1 = pg_query($update_clienttrip);
    		 if ($result1) {logMessage("Update successful for tripid: $tripid");} 
			else {logMessage("Update failed for tripid: $tripid, Error: " . pg_last_error());}

			}
		}//multiple esim
		else{
				logMessage("Singel update");	
            	$result1 = sprintf("update clienttrip SET comment=concat(comment,' Tsim customer,IMEI: %s, EID: %s'),imei='%s',eid='%s' where sim_phone_no in ( select sim_phone_no from sim_stock where simno = '%s') and status not in ('Canceled','cardlost')",$imei,$eid,$imei,$eid,$simSerialNo);
        		logMessage('update query is :   ' . $result1);
        		$result = pg_query($result1);
		}
		}//tsim

        else{
            $result1 = sprintf("update clienttrip SET clientname='%s',emailadd='%s',mobno='%s',from_date='%s',comment=concat(comment,' Amazon India,IMEI: %s, EID: %s'),imei='%s',eid='%s' where sim_phone_no in ( select sim_phone_no from sim_stock where simno = '%s') and status not in ('Canceled','cardlost')",$fname,$email,$mobileno,$activationDate,$imei,$eid,$imei,$eid,$simSerialNo);
	    logMessage($result1);
	    $result2 = pg_query($result1);
	    if ($result2) {logMessage("Update successful for single eSIM");} 
else {logMessage("Update failed for single eSIM, Error: " . pg_last_error());
        }
		}
	 */	

if ($ts == 'tsim') {
    // Update client name in sim_user table
    $update_clientname = sprintf("UPDATE sim_user SET clientname='%s' WHERE username IN (SELECT username FROM clienttrip WHERE sim_phone_no IN (SELECT sim_phone_no FROM sim_stock WHERE simno='%s') AND status NOT IN ('Canceled','Returned'))", $fname, $simSerialNo);
    logMessage('Updating client name: ' . $update_clientname);

logMessage("here is result query");
    $result = pg_query($update_clientname);
    if ($result) {
        logMessage("Query successful: Client name updated.");
    } else {
        logMessage("Query failed: " . pg_last_error());
    }

    // Handle multiple eSIMs
    if ($total_esims > 1) {
        logMessage("Multiple eSIMs detected. Updating based on trip ID.");

        foreach ($imeiValues as $index => $imei) {
            $eid = $eidValues[$index] ?? null;
            $tripid = $tripidValues[$index] ?? null;

            if (!$imei || !$eid || !$tripid) {
                logMessage("Missing data for index $index: IMEI ($imei), EID ($eid), TripID ($tripid)");
                continue;
            }

            $update_clienttrip = sprintf("UPDATE clienttrip SET comment=CONCAT(comment, ' Tsim customer, IMEI: %s, EID: %s '), imei='%s', eid='%s' WHERE tripid='%s' AND status NOT IN ('Canceled','cardlost')", $imei, $eid, $imei, $eid, $tripid);
            logMessage('Executing query: ' . $update_clienttrip);

            $result1 = pg_query($update_clienttrip);
            if ($result1) {
                logMessage("Update successful for tripid: $tripid");
            } else {
                logMessage("Update failed for tripid: $tripid, Error: " . pg_last_error());
            }
        }
    } else {
        // Handle single eSIM
        logMessage("Single eSIM detected. Updating clienttrip.");

        $update_query = sprintf("UPDATE clienttrip SET comment=CONCAT(comment, ' Tsim customer, IMEI: %s, EID: %s '), imei='%s', eid='%s' WHERE sim_phone_no IN (SELECT sim_phone_no FROM sim_stock WHERE simno='%s') AND status NOT IN ('Canceled','cardlost')", $imei, $eid, $imei, $eid, $simSerialNo);
        logMessage('Executing query: ' . $update_query);
logMessage("Here is result query");
        $result = pg_query($update_query);
        if ($result) {
            logMessage("Update successful for single eSIM.");
        } else {
            logMessage("Update failed for single eSIM, Error: " . pg_last_error());
        }
    }
} else {
    // Handle non-tsim case
logMessage("NON Tsim case");
    $update_query = sprintf("UPDATE clienttrip SET clientname='%s', emailadd='%s', mobno='%s', from_date='%s', comment=CONCAT(comment, ' Amazon India, IMEI: %s, EID: %s '), imei='%s', eid='%s' WHERE sim_phone_no IN (SELECT sim_phone_no FROM sim_stock WHERE simno='%s') AND status NOT IN ('Canceled','cardlost')", $fname, $email, $mobileno, $activationDate, $imei, $eid, $imei, $eid, $simSerialNo);
    logMessage('Executing query: ' . $update_query);

    $result = pg_query($update_query);
    if ($result) {
        logMessage("Update successful for non-tsim case.");
    } else {
        logMessage("Update failed for non-tsim case, Error: " . pg_last_error());
    }
}


        if ($result){
            logMessage('sim_user update query executed successfully ....');
            $log_message =  'Client Name: ' . $fname . PHP_EOL;
            $log_message .= 'Email Address: ' . $email . PHP_EOL;
            $log_message .= 'Mobile Number: ' . $mobileno . PHP_EOL;
            $log_message .= 'From Date: ' . $activationDate . PHP_EOL;
            $log_message .= 'Sim serial number : ' . $simSerialNo . PHP_EOL;
            $log_message .= 'Comment: ' . $comment . PHP_EOL;
            $log_message .= 'IMEI: ' . $imei . PHP_EOL;
            $log_message .= 'EID: ' . $eid . PHP_EOL;
            logMessage($log_message);
            echo json_encode(['code' => 200, 'msg' => "Firstname,Email,Mobile number, From Date ,comment='Amazon India' updated successfully "]);
            logMessage('Record updated for fieldset 2');
            exit();
        }
        else{
            echo json_encode(['code' => 404, 'msg' => $errorMessage]);
            logMessage('Unable to update Record fieldset 2 ');
            exit();
        }
    }
    else{
        echo json_encode(['code' => 200, 'msg' => $okMessage]);
        logMessage('Data not present blank data it is ');
    }
    exit();
}

function MobiEmailBody($fname, $data, $simSerialNo, $simPhoneNo, $country){
    if (strpos($country, 'esim') !== false) {
        $activationInstructions = "<p>1) You will receive another message when your eSIM is activated. <br>2) Validity period of the eSIM will begin from the activation date.</p>";
    } else {
        $activationInstructions = ($country == 'EU-Economy-2GB-114196' || $country == 'EU-Economy-5GB-211684' || $country == 'EU-Economy-30d-59478') ?
            "<p>1) Please keep the SIM card inserted into your unlocked phone on this date to activate and start using it. <br>2) Validity period of the SIM card will begin from the activation date.</p>" :
            "<p>1) You will receive another message when your SIM card is activated. <br>2) Validity period of the SIM card will begin from the activation date.</p>";
    }
    $emailTextHtml = "<html><body><p>Hello, <b>$fname</b></p>";
    $emailTextHtml .= "<p>Your TSIM SIM activation request is registered:</p>";
    $emailTextHtml .= "Activation Date: <b>" . date("jS F Y", strtotime($data['date_mobi'])) . "</b><br>";



    //$emailTextHtml .= "Sim Serial No: <b>$simSerialNo</b><br>";

	if (strpos($simSerialNo, '_') !== false) {
    logMessage('Sim serial number contains an underscore, so it will not be included in the email.');
} else {
    $emailTextHtml .= "Sim Serial No: <b>$simSerialNo</b><br>";
}



    $emailTextHtml .= $activationInstructions;
    if(empty($simPhoneNo)){
        $emailTextHtml .= "<p>Your mobile number will be sent to you in a separate email on the activation date.</p>";
    }
    $emailTextHtml .= "<p>Please reply to this message for assistance<br><br>Thank you,<br>TSIM team</p>";
    $emailTextHtml .= "</body></html>";

    return $emailTextHtml;
}

if(isset($_POST['mobi_json']) && !empty($_POST['mobi_json'])){
    logMessage("Submit button is clicked by Mobi user for activation");
    $data = json_decode($_POST['mobi_json'], true);
    if ($data === null && json_last_error() !== JSON_ERROR_NONE) {
        echo "Error decoding JSON data: " . json_last_error_msg();
        exit;
    }
    logMessage('json data is ---> '.json_encode($_POST['mobi_json'],JSON_PRETTY_PRINT));
    $email = filter_var($data['email_mobi'], FILTER_SANITIZE_EMAIL);
    $simSerialNo = $data['serialNo'];
    $actDate = $data['date_mobi'];
    $mobileno = $data['mobileno_mobi'];
    $fname = sanitizeVariable($data['fname_mobi']);
    $amazon_orderid = $data['amz_ord_mobi'];
    $zipcode = $data['zipcode_mobi'];
    $imei = $data['imei_mobi'];
    $eid = $data['eid_mobi'];

    if (empty($fname)) {
        $errors[] = 'First Name is required';
    }
    if (empty($email)) {
        $errors[] = 'Email Address is required';
    }
    if ($imei!='' && (!ctype_digit($imei) || strlen($imei)!=15)) {
        $errors[] = 'Invalid IMEI ';
    }

    if ($amazon_orderid!='' && !preg_match('/^[\d-]+$/', $amazon_orderid)) {
        $errors[] = 'Invalid Amazon Order ID ';
    }
    
    if ($eid!='' && (!ctype_digit($eid) || strlen($eid)!=32)) {
        $errors[] = 'Invalid EID ';
    }

    if ($mobileno!='' && !isValidMobileNumber($mobileno)) {
        $errors[] = 'Invalid Mobile Number: Must contain only digits, +, or -.';
    }
    
    if (!empty($errors)) {
        foreach ($errors as $error) {
            echo htmlspecialchars($error, ENT_QUOTES, 'UTF-8') . '<br>';
            logMessage($error);
        }
        echo json_encode(['code' => 404, 'msg' => $errorMessage]);
        exit;
    }

    logMessage('Extracted Data: ');
    logMessage("Email: " . $email);
    logMessage("SerialNo: " . $simSerialNo);
    logMessage("Activation Date: " . $actDate);
    logMessage("Mobile No: " . $mobileno);
    logMessage("First Name: " . $fname);
    logMessage("Amazon Order ID: " . $amazon_orderid);
    logMessage("Zipcode: " . $zipcode);
    logMessage("IMEI: " . $imei);
    logMessage("EID: " . $eid);

    


	$checkClienttrip_flagEntry = "select * from clienttrip_flags where simno='$simSerialNo'";
	logMessage("Select query: ".$checkClienttrip_flagEntry);
	$execcheck = pg_query($checkClienttrip_flagEntry);
	if($execcheck)
	{
	        logMessage($checkClienttrip_flagEntry);
        	$rowscount = pg_fetch_all($execcheck);
		if($rowscount > 0)
		{
			logMessage("This record is already present in clienttrip flags table");
		}
		else
		{
			$insertQueryofClienttripFlag = "insert into clienttrip_flags (order_no, simno, sim_phone_no, tripid) values ('$amazon_orderid', '$simSerialNo',(select sim_phone_no from clienttrip where sim_phone_no = (select sim_phone_no from sim_stock where simno='$simSerialNo') LIMIT 1),(select tripid from clienttrip where sim_phone_no = (select sim_phone_no from sim_stock where simno='$simSerialNo') LIMIT 1))"; 
    
		    	logMessage($insertQueryofClienttripFlag);
		    	$ExecuteInsertQuery = pg_query($insertQueryofClienttripFlag);
		      	if($ExecuteInsertQuery)
	       		{
        			logMessage("Insert Query of clienttrip flag done successfully for this order".$amazon_orderid." and this simno ".$simSerialNo);
	        	}
		        else
        		{
		        	logMessage("Insert Query of clienttrip flag Failed for this order ".$amazon_orderid." and this simno ".$simSerialNo);
				 logMessage("Error: " . pg_last_error());
	        	}
		}
	}
	else
	{
		logMessage("QueryExecution Failed");
		logMessage("Error: " . pg_last_error());

	}	






    if (strpos($actDate, '/') !== false) {
        $arr = explode("/", $actDate);
        $actDate = $arr[2] . '-' . $arr[0] . '-' . $arr[1] . ' 00:00:00';
    }

    logMessage('Activation Date :  '. $actDate);
    logMessage('POST Data- '.json_encode($data,JSON_PRETTY_PRINT));

    $activationDate = new DateTime($actDate);
    if ($activationDate->format('w') == 0) {
        logMessage('Activation is on of Sunday Date . Thats why rejecting and not processing further  '.$activationDate);
        // It's Sunday, reject the request
        echo "<script>alert('Sorry, we do not process activations on Sundays, Please select one day prior or next day')</script>";
        exit();
    }
    else {

        $check_num = sprintf("select sim_phone_no,country from sim_stock where simno='%s'",$simSerialNo);
        $exe_check = pg_query($check_num);
	logMessage($check_num);
        $affectedrows = pg_affected_rows($exe_check);

        if($affectedrows > 0){
            $row1 = pg_fetch_assoc($exe_check);
            $country = $row1['country'];
            logMessage(' Country is : ' . $country);
            //from AmazonUSA comment removed from here
            if(!empty($amazon_orderid)){
                $result1 = sprintf("UPDATE clienttrip SET clientname='%s', emailadd='%s', mobno='%s', from_date='%s',comment=CONCAT(comment,'imei: %s, eid: %s'), add_pinno='%s', imei='%s', eid='%s',order_no='%s' WHERE sim_phone_no=(SELECT sim_phone_no FROM sim_stock WHERE simno='%s') AND status IN ('Delivery pending','Delivered','Unshipped','Pending','Shipped')",$fname, $email, $mobileno, $actDate, $imei, $eid, $zipcode, $imei, $eid,$amazon_orderid, $simSerialNo);
            }
            else{
                $result1 = sprintf("UPDATE clienttrip SET clientname='%s', emailadd='%s', mobno='%s', from_date='%s',comment=CONCAT(comment,'imei: %s, eid: %s'), add_pinno='%s', imei='%s', eid='%s' WHERE sim_phone_no=(SELECT sim_phone_no FROM sim_stock WHERE simno='%s') AND status IN ('Delivery pending','Delivered','Shipped','Pending','Unshipped')",$fname, $email, $mobileno, $actDate, $imei, $eid, $zipcode, $imei, $eid, $simSerialNo);
            }
            logMessage('update query for mobi---->'.$result1);
            $result = pg_query($result1) or die("Cannot Execute Query:\n".pg_last_error());
            $updatedrows = pg_affected_rows($result);
            logMessage("Affected ---> ".$updatedrows);
            if ($updatedrows > 0) {
                if (!empty($amazon_orderid)) {
                    logMessage("----------------------------------------------------------------------");
                    $amazon_num_query = "SELECT a.amazonorderid, c.sim_phone_no, c.qrtext, a.title from sim_stock_trip c right join amazon_order_items_view a on c.order_no=a.amazonorderid  where a.amazonorderid='$amazon_orderid' and a.emailqrcode=true and c.qrtext is not null  order by c.qrtext";
                    $check_amazonorderid = pg_query($amazon_num_query);
                    logMessage("Query in amazon_num_query condition is : ".$amazon_num_query);
                    $amazon_query = pg_num_rows($check_amazonorderid);
                    logMessage("Number of records the amazon_query returned is : ".$amazon_query);
                    if($amazon_query >= 1){
                        $all_rows = pg_fetch_all($check_amazonorderid);
                        $is_safe_num_found = false;
                        foreach ($all_rows as $row) {
                            if ($row['amazonorderid'] === $amazon_orderid) {
                                $query_result = $row;
                                $is_safe_num_found = true;
                                $matched_rows[] = $row;
                            }
                        }
                        if ($is_safe_num_found) {
                            logMessage("Amazon Order Id is Complete");
                            //echo json_encode(['code' => 202, 'msg'=> $matched_rows]);
			    echo json_encode(['code' => 200, 'msg'=> $okMessage]);

			    logMessage("Response code: 202");
                            logMessage("since qr already exists for this user: ".$simSerialNo);
                            $update_user_details=sprintf("update clienttrip set status='esimDelivered' where sim_phone_no='%s'",$simSerialNo);
                            logMessage("update query for changing the status if qr is already present for that amazonorder id is: ".$update_user_details);
                            $update_user_exec=pg_query($update_user_details);
                            $user_updatedrows = pg_affected_rows($update_user_exec);
                            logMessage("Affected ---> ".$user_updatedrows);
                            exit();
                            return;
                        } 
                    }
                }

		



		if(!empty($amazon_orderid))
		{

		$checkitemscountquery = "SELECT s.simno FROM sim_stock_trip s LEFT JOIN clienttrip_flags ctf ON s.simno = ctf.simno AND ctf.activation_schedule = true WHERE s.order_no = '$amazon_orderid' AND s.simno != '$simSerialNo' AND ctf.simno IS NULL";	
		$check_amazonorderitemscount = pg_query($checkitemscountquery);
		if($check_amazonorderitemscount)
		{
	                logMessage("Query in amazon_order_items_count for multiple ordeitems check :".$checkitemscountquery);
        	        $itemslistcountexcludingcurrent = pg_fetch_all($check_amazonorderitemscount);
                	logMessage("Number of records left is : ".count($itemslistcountexcludingcurrent));
				
			if($itemslistcountexcludingcurrent > 0)
			{
				$updatescheduleflag = "update clienttrip_flags set activation_schedule=true where simno='$simSerialNo' and order_no='$amazon_orderid'";
				logMessage($updatescheduleflag);		
				$ExecuteUpdatequery = pg_query($updatescheduleflag);
				if($ExecuteUpdatequery)
				{
					logMessage($simSerialNo."Sim card is scheduled updated Sucessfully. for order no".$amazon_orderid);	
				}
				else
				{	
					logMessage($simSerialNo."Sim card scheduling update Query Failed. for order no".$amazon_orderid);
				}

	
				$ScheduleNextMessage = "You need to schedule the activation date for ".count($itemslistcountexcludingcurrent) ." more card.";
				echo json_encode(['code' => 207,'msg' => $ScheduleNextMessage]);
			
			}
			else
			{ 		

				 $updatescheduleflag = "update clienttrip_flags set activation_schedule=true where simno='$simSerialNo' and order_no='$amazon_orderid'";
                                logMessage($updatescheduleflag);
                                $ExecuteUpdatequery = pg_query($updatescheduleflag);
                                if($ExecuteUpdatequery)
                                {
                                        logMessage($simSerialNo."Sim card is scheduled updated Sucessfully. for order no".$amazon_orderid);
                                }
                                else
                                {
                                        logMessage($simSerialNo."Sim card scheduling update Query Failed. for order no".$amazon_orderid);
                                }

 
                		echo json_encode(['code' => 200, 'msg' => $okMessage]);
			}
		}
		else
		{
			logMessage("Query Execution Failed");
			logMessage("Error executing query: " . pg_last_error());
		}
		
	    	
		
	}
	else
	{
		logMessage("Came in else because order number is empty here");
		echo json_encode(['code' => 200, 'msg' => $okMessage]);

	}



		//if sim phone number available then send query and code.
                $getsimphonenoquery = "SELECT sim_phone_no FROM sim_stock WHERE dateandtime >= (CURRENT_TIMESTAMP - INTERVAL '1 year') AND dateandtime <= CURRENT_TIMESTAMP AND simno <> sim_phone_no AND activation_reqd = true AND LENGTH(REPLACE(REPLACE(REPLACE(sim_phone_no, ',', ''), '.', ''), '''', '')) >= 8 AND LENGTH(REPLACE(REPLACE(REPLACE(sim_phone_no, ',', ''), '.', ''), '''', '')) <= 14 AND REPLACE(REPLACE(REPLACE(sim_phone_no, ',', ''), '.', ''), '''', '') NOT LIKE '%\_%' ESCAPE '\' AND availability = false AND active = true AND activation_reqd=true AND simno ='$simSerialNo'";
                logMessage($getsimphonenoquery);
                $getsimphoneno = pg_query($DBConn, $getsimphonenoquery);
                if ($getsimphoneno) {
                    $gotSimphoneno = pg_fetch_assoc($getsimphoneno);
                    $simPhoneNo = $gotSimphoneno['sim_phone_no'];
                    logMessage("Sim Phone NUmber is present for this sim serial number: ".$simPhoneNo);
                    $emailTextHtml = MobiEmailBody($fname, $data, $simSerialNo, $simPhoneNo, $country);
                    // Send email
                    if ($emailTextHtml) {
                        $to = $email;
                        $subject = "Activation Request To TSIM Mobi";
                        $headers = "From: TSIM Team <services@tsim.mobi>\r\n";
                        $headers .= "Content-type: text/html; charset=UTF-8\r\n";
                        $mailop = mail($to, $subject, $emailTextHtml, $headers, '-fservices@tsim.mobi');

                        if ($mailop) {
                            logMessage('Mail sent: ' . json_encode($mailop,JSON_PRETTY_PRINT));
                            logMessage('Record updated ');
                            logMessage($emailTextHtml);
                        } else {
                            logMessage('Sending mail failed ');
                        }
                        // Save mail to sent folder
                        $command = "echo -e \"From: Tsim Team <services@tsim.mobi>\r\nTo: $to\r\nSubject: $subject\r\nX-PutIn: Sent\r\nContent-type: text/html; charset=UTF-8\r\n\r\n$emailTextHtml\" | /usr/bin/sendmail -t -i services@tsim.mobi";
                        $savetosent = shell_exec($command);
                        logMessage(' Mail sent to Sent folder ');
                    } else {
                        echo json_encode(['code' => 404, 'msg' => $errorMessage]);
                        logMessage('Unable to send mail to client ');
                    }
                }
                else{
                    logMessage("Sim Phone NUmber is not present for this sim serial number: ".$simSerialNo);
                    $simPhoneNo='';
                    $emailTextHtml = MobiEmailBody($fname, $data, $simSerialNo, $simPhoneNo, $country);
                    // Send email
                    if ($emailTextHtml) {
                        $to = $email;
                        $subject = "Activation Request To TSIM Mobi";
                        $headers = "From: TSIM Team <services@tsim.mobi>\r\n";
                        $headers .= "Content-type: text/html; charset=UTF-8\r\n";
                        $mailop = mail($to, $subject, $emailTextHtml, $headers, '-fservices@tsim.mobi');

                        if ($mailop) {
                            logMessage('Mail sent: ' . json_encode($mailop,JSON_PRETTY_PRINT));
                            logMessage('Record updated ');
                            logMessage($emailTextHtml);
                        } else {
                            logMessage('Sending mail failed ');
                        }
                        // Save mail to sent folder
                        $command = "echo -e \"From: Tsim Team <services@tsim.mobi>\r\nTo: $to\r\nSubject: $subject\r\nX-PutIn: Sent\r\nContent-type: text/html; charset=UTF-8\r\n\r\n$emailTextHtml\" | /usr/bin/sendmail -t -i services@tsim.mobi";
                        $savetosent = shell_exec($command);
                        logMessage(' Mail sent to Sent folder ');
                    } else {
                        echo json_encode(['code' => 404, 'msg' => $errorMessage]);
                        logMessage('Unable to send mail to client ');
                    }
                }
            } else {
                echo json_encode(['code' => 404, 'msg' => $errorMessage]);
                logMessage('Unable to update Record ');
            }
        }
        else{
            echo json_encode(['code'=>404, 'msg'=>$errorMessage]);
            logMessage('Unable to update Record ');
        }

      exit();
    }
}


if( !empty($_POST['qrCodeImage']) && !empty($_POST['email']) && !empty($_POST['phoneNumber']) && !empty($_POST['qrtext'])) {
    logMessage("eSIM qr code sending to user, process started");
    $qrCodeImage = $_POST['qrCodeImage'];
    $email = $_POST['email'];
    $phoneNumber = $_POST['phoneNumber'];
    $qrtext=$_POST['qrtext'];
    $emailTextHtml = "
    <p>Hello,</p>
    <p>Please find the attached eSIM QR image. This is a one-time scan, so please scan it only once you reach your destination.</p>
    <h3>Activating eSIM on Your Device</h3>
    
    <h4>For iPhone XR, XS, and Newer Models</h4>
    <p><b>Option 1:</b> Click this link: 
        <a href='https://esimsetup.apple.com/esim_qrcode_provisioning?carddata=".$qrtext."'>Activate eSIM</a>
    </p>
    <p><b>Option 2:</b></p>
    <ul>
        <li>Go to <b>Settings</b>, then <b>Cellular</b>.</li>
        <li>Select <b>Add a Plan</b>, and the Camera app will open.</li>
        <li>Point your camera at the eSIM QR image until you see a message that says 'Installing cellular plan'.</li>
    </ul>
    
    <h4>For Android Devices (2021 and Newer)</h4>
    <ul>
        <li>Go to <b>Settings</b> > <b>Connections</b> > <b>SIM Card Manager</b>.</li>
        <li>Tap <b>Add Mobile Plan</b>.</li>
        <li>Tap <b>Add Using QR Image</b>.</li>
        <li>Scan the eSIM QR image.</li>
    </ul>
    
    <br><br><br>
    <p>Best Regards,</p>
    <p>Team Tsim</p>
    ";

    // Generate a boundary string
    $boundary = md5(time());

    // Define the headers
    $headers = "From: TSIM Team <services@tsim.mobi>\r\n";
    $headers .= "MIME-Version: 1.0\r\n";
    $headers .= "Content-Type: multipart/mixed; boundary=\"$boundary\"\r\n";

    // Define the body
    $body = "--$boundary\r\n";
    $body .= "Content-Type: text/html; charset=UTF-8\r\n";
    $body .= "Content-Transfer-Encoding: 7bit\r\n\r\n";
    $body .= $emailTextHtml . "\r\n";
    $body .= "--$boundary\r\n";
    $body .= "Content-Type: image/png; name=\"${phoneNumber}_esim_qr_code.png\"\r\n";
    $body .= "Content-Transfer-Encoding: base64\r\n";
    $body .= "Content-Disposition: attachment; filename=\"${phoneNumber}_esim_qr_code.png\"\r\n\r\n";
    $body .= chunk_split(base64_encode(file_get_contents($qrCodeImage))) . "\r\n";
    $body .= "--$boundary--";

    // Send email
    $to = $email;
    $subject = "Your Esim QR Code";
    $mailop = mail($to, $subject, $body, $headers, '-fservices@tsim.mobi');

    if ($mailop) {
        logMessage("Mail body is : ");
        logMessage($emailTextHtml);
        logMessage("Mail sent successfully");
        logMessage('Mail sent: ' . json_encode($mailop, JSON_PRETTY_PRINT));
        logMessage('Record updated ');
        
    } else {
        logMessage('Sending mail failed ');
    }
} 

if (!empty($_POST['fname']) && !empty($_POST['email']) && !empty($_POST['date']) && $_FILES['pass1'] && $_FILES['pass2']){
    logMessage("Submit button is clicked by indian user who uploaded docs");
    $email = $_POST['email'];
    $simSerialNo =$_POST['serialNo'];





   $Query = "SELECT c.domainname, c.from_date FROM clienttrip c JOIN sim_stock s ON c.sim_phone_no = s.sim_phone_no WHERE s.simno = '$simSerialNo'";
    logMessage('Query for getting activation date - ' . $Query);
    $ExecuteQuery = pg_query($DBConn, $Query);
    if ($ExecuteQuery)
    {
        $gotrow = pg_fetch_assoc($ExecuteQuery);

        if ($gotrow)
        {
                $Activation_date_from_db = $gotrow['from_date'];
                $Domain = $gotrow['domainname'];

                logMessage('Activation Date from database - ' . $Activation_date_from_db);
                logMessage('Domain name - ' . $Domain);

                if ($Domain == 'tsim.in')
                {
                        $activationDate = $Activation_date_from_db;
			$activationDate = (new DateTime($activationDate))->format('Y-m-d');
			$activationDate = date("jS F Y", strtotime($activationDate));
                }
                else
                {
    			$activationDate = $_POST['date'];
                }
        }
        else
        {
                logMessage('No data found for the query.');
        }
     }
     else
     {
        logMessage('Query execution failed: ' . pg_last_error($DBConn));
     }














    $mobileno = $_POST['mobileno'];
    $fname = $_POST['fname'];
    $file_arr = array();
    $pass1 = $_FILES["pass1"]["name"];
    $pass2 = $_FILES["pass2"]["name"];
    $visa;
    $order_number = $_POST['ordNo'];

    if ($_FILES["pass1"]["name"] != ''){
        array_push($file_arr, "pass1");
    }
    if ($_FILES["pass2"]["name"] != ''){
        array_push($file_arr, "pass2");
    }
    if ($_FILES["visa"]["name"] != ''){
        $visa = $_FILES["visa"]["name"];
        array_push($file_arr, "visa");
    }
   // $arr = explode("/", $activationDate);
   // $activationDate = $arr[2] . '-' . $arr[0] . '-' . $arr[1] . ' 00:00:00';
    logMessage('Final Data With Documents- ' . json_encode($_POST, JSON_PRETTY_PRINT));

    $file_contents = '  Email: ' . $email;
    $file_contents .= ' SimSerialNo: ' . $simSerialNo;
    $file_contents .= ' ActivationDate: ' . $activationDate;
    $file_contents .= ' Mobileno: ' . $mobileno;
    $file_contents .= ' First Name: ' . $fname;
    $file_contents .= ' Pass1: pass_1_' . $pass1."\n";
    $file_contents .= ' Pass2: pass_2_' . $pass2."\n";
    $file_contents .= ' Visa: visa_' . $visa."\n";
    logMessage($file_contents);
    $extensions = array('jpg', 'jpeg', 'png', 'pdf', 'doc', 'docx');
    $target_file = array();
    $target_non_save_file = array();
    $current_dir = "/var/www/htdocs/ownmail/tsim/tsim_docs_uploads/";
    $target_dir = "/home/briefcase/in/tsim/.briefcase/s/services/tsim_doc/";
    logMessage(' Target Address = ' . $target_dir);
    $file_contents =' Array = ' . json_encode($file_arr, JSON_PRETTY_PRINT);
    logMessage($file_contents);
    $errors = array();

    for ($i = 0;$i < count($file_arr);$i++){
        //$file_ext = strtolower(end(explode('.', $_FILES[$file_arr[$i]]['name'])));

	$temp = explode('.', $_FILES[$file_arr[$i]]['name']);
	$file_ext = strtolower(end($temp));

        logMessage(' File extension file_ext = ' . $file_ext);
        if (!in_array($file_ext, $extensions)){
            $errors[] = ' Extension not allowed: ' . $_FILES[$file_arr[$i]]['name'] . ' ' . $_FILES[$file_arr[$i]]['type'];
        }
    }

    if ($errors){
        $file_contents = ' Error  Check: ' . json_encode($errors, JSON_PRETTY_PRINT);
        logMessage($file_contents);
        print_r($errors);
        exit();
    }
    logMessage(' File array size = ' . count($file_arr));

	foreach ($file_arr as $key) {
	    $fileName = $_FILES[$key]['name'];
	    $fileTmp = $_FILES[$key]['tmp_name'];
	    $fileError = $_FILES[$key]['error'];
	    $fileSize = $_FILES[$key]['size'];
	    $sizeInMB = round($fileSize / (1024 * 1024), 2);

	//    logMessage("Checking file key = $key | Name = $fileName | Tmp = $fileTmp | Error = $fileError | Size = {$fileSize} bytes (~{$sizeInMB} MB)");

	    $errorDescription = '';
	    if ($fileError === 1) {
	        $errorDescription = 'UPLOAD_ERR_INI_SIZE (File exceeds upload_max_filesize)';
	    } elseif ($fileError === 2) {
	        $errorDescription = 'UPLOAD_ERR_FORM_SIZE (File exceeds MAX_FILE_SIZE in HTML form)';
	    } elseif ($fileError === 3) {
	        $errorDescription = 'UPLOAD_ERR_PARTIAL (File only partially uploaded)';
	    } elseif ($fileError === 4) {
	        $errorDescription = 'UPLOAD_ERR_NO_FILE (No file uploaded)';
	    }

	    logMessage("Checking file key = $key | Name = $fileName | Tmp = $fileTmp | Error = $fileError $errorDescription | Size = {$fileSize} bytes (~{$sizeInMB} MB)");
	}

		

    for ($i = 0; $i < count($file_arr); $i++) {
        logMessage(' File Number = ' . $i . ' ' . $_FILES[$file_arr[$i]]['name']);

        //$file_ext = strtolower(end(explode('.', $_FILES[$file_arr[$i]]['name'])));
	
	$temp = explode('.', $_FILES[$file_arr[$i]]['name']);
        $file_ext = strtolower(end($temp));


        $file_tmp = $_FILES[$file_arr[$i]]['tmp_name'];
	logMessage(' File TMP name ----> ' .$file_tmp);

        $total_files_count = count($file_arr);
        // Generate year-month subfolder based on the current date
        $current_month_year = date("Y-m");
        $target_dir = "/home/briefcase/in/tsim/.briefcase/s/services/tsim_doc/";
        $tar_dir = "/home/briefcase/in/tsim/.briefcase/s/services/tsim_doc/" . $current_month_year . "/";
        // Create year-month subfolder if it does not exist
        if (!is_dir($tar_dir)) {
            mkdir($tar_dir, 0777, true);
        }

        if ($_POST['Tsim'] == "tsim" || $_POST['Tsim'] == "amazon") {
            $get = pg_query($DBConn, "select order_no,username from clienttrip where  sim_phone_no in (select sim_phone_no from sim_stock where simno = '$simSerialNo') and status not in ('Canceled','Returned') and order_no='$order_number'");
            logMessage("select order_no,username from clienttrip where  sim_phone_no in (select sim_phone_no from sim_stock where simno = '$simSerialNo') and status not in ('Canceled','Returned') and order_no='$order_number'");
            $fetch = pg_fetch_assoc($get);
            $ord = $fetch['order_no'];
            $usr = $fetch['username'];
            if($tar_dir){
                logMessage('inside New target directory condtion');
                $file_name = $current_month_year.'/'.$usr . '_' . $ord . '_' . $file_arr[$i] . '.' . $file_ext;
            }
            else{
                $file_name = $usr . '_' . $ord . '_' . $file_arr[$i] . '.' . $file_ext;
            }
        }else {
            $file_name = $current_month_year.'/'.$simSerialNo . '_' . $file_arr[$i] . '.' . $file_ext;
        }
        logMessage('File New Name = ' . ($i+1) . ' ' . $file_name);
        $file = $target_dir . $file_name;
        logMessage('File New Name = ' . ($i+1) . ' ' . $file);

        if (empty($errors)) {
            // Construct the filename
            $check_file_name = $usr . '_' . $ord . '_' . $file_arr[$i] . '.' . $file_ext;
            $fname2 = basename($check_file_name, $file_ext);
            logMessage(' fname2 is : '. $fname2);
            $search_pattern = $tar_dir . $fname2 . '*';
            $existing_files = glob($search_pattern);
            logMessage($search_pattern);
            $existing_count = count($existing_files);
            if ($existing_count > 0 && $fname2 !== "") {
                foreach ($existing_files as $existing_file) {
                    logMessage('file deleted in directory: '.$existing_file);
                    unlink($existing_file);
                }
            }
            switch ($file_arr[$i]) {
                case 'pass1':
                    $column = 'doc1';
                    break;
                case 'pass2':
                    $column = 'doc2';
                    break;
                case 'visa':
                    $column = 'doc3';
                    break;
                default:
                    break;
            }

  	    logMessage("Temp file path for {$file_arr[$i]}: " . $_FILES[$file_arr[$i]]['tmp_name']);
            $moved = move_uploaded_file($file_tmp, $file);
            $up_query = sprintf("UPDATE tsim_order_documents SET %s='%s' WHERE order_no='%s'", $column, $file_name, $ord);
            logMessage('Docs column update query is : '. $up_query);
            pg_query($up_query);
            if ($moved) {
                logMessage("File uploaded successfully to the directory: ". $tar_dir);
                logMessage('File Uploaded = ' . ($i + 1) . ' ' . $file_name);
            }else {
                logMessage('Cant Upload File = ' . ($i + 1) . ' ' . $file_name);
		$upload_error = error_get_last();
		logMessage("❌ move_uploaded_file failed. Error: " . json_encode($upload_error));
    		logMessage("From: $file_tmp, To: $file");
                echo json_encode(array('msg' => 'Document File Upload Failed'));
                exit();
            }
        }
    }
    $update_time = sprintf("update tsim_order_documents set last_update = now() where order_no in (select order_no from clienttrip where status not in ('Canceled','Returned') and sim_phone_no in (select sim_phone_no from sim_stock where simno='%s'))", $simSerialNo);
    logMessage(' update last time in tsim_order_documents table query is  :' . $update_time);
    $update_time_res = pg_query($update_time);

    if (!$update_time_res) {
        logMessage('Error updating the last_update in database.');
    } else {
        logMessage('$update_time query is updated succesfully in DB.');
    }
     if($ord && !empty($ord)){
        $docs_count_query =sprintf("SELECT order_no,
            (CASE 
                WHEN TRIM(doc1) IS NOT NULL AND TRIM(doc1) <> '' THEN 1 
                ELSE 0 
            END +
            CASE 
                WHEN TRIM(doc2) IS NOT NULL AND TRIM(doc2) <> '' THEN 1 
                ELSE 0 
            END +
            CASE 
                WHEN TRIM(doc3) IS NOT NULL AND TRIM(doc3) <> '' THEN 1 
                ELSE 0 
            END) AS actual_count_of_documents
        FROM tsim_order_documents
        WHERE order_no = '%s'",$ord);
        logMessage($docs_count_query);
        $docs_count_query_result=pg_query($docs_count_query);
        if (!$docs_count_query_result) {
            logMessage('Error updating database for this order id : '.$ord);
            echo json_encode(array('msg' => $errorMessage));
            exit();
        } else {
            $uploaded_count_row = pg_fetch_assoc($docs_count_query_result);
            $update_count_query = sprintf("UPDATE tsim_order_documents SET no_of_documents_uploaded=%d WHERE order_no='%s'",$uploaded_count_row['actual_count_of_documents'], $ord);
            logMessage("Documents uploaded count query is : ". $update_count_query);
            $update_count_result = pg_query($update_count_query);
            if (!$update_count_result) {
                logMessage('Error updating database for this order id : '.$ord);
                echo json_encode(array('msg' => $errorMessage));
                exit();
            } else {
            }
        }
    }
    echo json_encode(array('msg' => $okMessage));
    logMessage('Message ' . $okMessage);
    $queryforCheckingActivationtype = "select activation_reqd from sim_stock where simno ='$simSerialNo'";
    logMessage("query is : ". $queryforCheckingActivationtype);
    $execquery = pg_query($DBConn, $queryforCheckingActivationtype);
    $checkActivationReqd = pg_fetch_assoc($execquery);
    $activation_reqd = $checkActivationReqd['activation_reqd'];
    logMessage('Activation Require -> ' . $activation_reqd);

    $getsimphonenoquery = "SELECT sim_phone_no FROM sim_stock WHERE dateandtime >= (CURRENT_TIMESTAMP - INTERVAL '1 year') AND dateandtime <= CURRENT_TIMESTAMP AND simno <> sim_phone_no";
    if ($activation_reqd === 't') {
        $getsimphonenoquery .= " AND activation_reqd = true";
    }
    $getsimphonenoquery .= " AND LENGTH(REPLACE(REPLACE(REPLACE(sim_phone_no, ',', ''), '.', ''), '''', '')) BETWEEN 8 AND 14 AND REPLACE(REPLACE(REPLACE(sim_phone_no, ',', ''), '.', ''), '''', '') NOT LIKE '%\_%' ESCAPE '\\' AND availability = false AND active = true AND simno ='$simSerialNo'";
    logMessage('Query for Getting SimPhoneNo - >' . $getsimphonenoquery);

    $getsimphoneno = pg_query($DBConn, $getsimphonenoquery);
    logMessage('Get simphoneno query --> ' . $getsimphonenoquery);
    if ($getsimphoneno) {
        $gotSimphoneno = pg_fetch_assoc($getsimphoneno);
        $simPhoneNo = $gotSimphoneno['sim_phone_no'];
        $emailTextHtml = "<html><body><p></p>";
        $emailTextHtml .= "<p>Hello  <b>$fname</b>,</p>";


        $EsimcheckQuery="select count(*) from sim_stock where country ilike '%esim%' and simno ='$simSerialNo'";
        logMessage(" EsimCheckquery is : ". $EsimcheckQuery);
        $execquery = pg_query($DBConn, $EsimcheckQuery);

        // Fetch the count
        $checkEsim = pg_fetch_row($execquery);

/*	
        // Check if count is greater than 0
        if ($checkEsim[0] > 0){
            $emailTextHtml .= "<p>Your TSIM SIM activation request is registered: </p>";
        }
        else{
            $emailTextHtml .= "<p>Your TSIM SIM Card activation request is registered: </p>";
        }
        if ($activation_reqd === 't') {
            $emailTextHtml .= "Activation Date: <b>$activationDate</b><br>";
        }
        $emailTextHtml .= "Sim Serial No: <b>$simSerialNo</b><br>";
        if (!empty($simPhoneNo)) {
            $emailTextHtml .= "Sim Phone No: <b>$simPhoneNo</b><br>";
        }
        $emailTextHtml .= "Documents Uploded: <b>" . count($file_arr) . "</b><br>";
        if ($activation_reqd === 't'){
            if ($checkEsim[0] > 0){
                $emailTextHtml .= "<p>1) You will receive another message when your eSIM is activated.<br>2) Validity period of the eSIM will begin from the activation date. </p>";
            }
            else{
                $emailTextHtml .= "<p>1) You will receive another message when your card is activated.<br>2) Validity period of the SIM card will begin from the activation date. </p>";
            }
        }
        else{
            if ($checkEsim[0] > 0){
                $emailTextHtml .=  "<br>";
            }
            else{
                $emailTextHtml .= "<p>1) Please keep the SIM card inserted into your unlocked phone to start using it.<br>2) Validity period of the SIM card will begin upon activation. </p>";
            }
        }
        $emailTextHtml .= "Please reply to this message for assistance<br><br>";
	$emailTextHtml .= "Thank you,<br>TSIM team</body></html>";
 */
	$emailTextHtml .= "<p>Thank you for submitting your documents. We have successfully received them and will review them shortly. If any additional information is needed, we will contact you.</p>";
        $emailTextHtml .= "<p>Documents Uploded: <b>" . count($file_arr) . "</b></p>";
        $emailTextHtml .= "<p>If you have any questions, feel free to reply to this message for assistance.<br><br>Thank you,<br>TSIM Team</p></body></html>";


        $headers = "MIME-Version: 1.0\r\n";
        $headers .= "Content-type:text/html;charset=UTF-8\r\n";
        $headers .= "From: TSIM Team <services@tsim.in>\r\n";
        $headers .= "Disposition-Notification-To: services@tsim.in\r\n";
	//$subject = "Activation Request To TSIM India";
	$subject = "Confirmation: Your Documents Have Been Received";

        $mailop = mail($email, $subject, $emailTextHtml, $headers, '-fservices@tsim.in');

        if ($mailop) {
            logMessage('Mail Check: ' . json_encode($mailop, JSON_PRETTY_PRINT));
            logMessage('Record updated ');
            logMessage($emailTextHtml);
        } else {
            logMessage('Sending mail failed');
        }
    }
    exit();
}










?>


