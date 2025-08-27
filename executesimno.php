<?php
date_default_timezone_set("Asia/Calcutta");

global $DBConn,$log_file;

$log_file = "/tmp/executesimno.log";
//$log_file = "/tmp/simmis.log";

header('Content-Type: application/json');

// Start output buffering to capture any errors or unwanted output
ob_start();

$response = [];



function logMessage($message, $details = null) {
	$log_file='/tmp/executesimno.log';
	$log_message = date("Y-m-d H:i:s") . " [PID: " . getmypid() . ", IP: " . $_SERVER['REMOTE_ADDR'] . "] " . $message . PHP_EOL;
	if ($details !== null) {
		foreach ($details as $key => $value) {
			$log_message .= date("Y-m-d H:i:s") . " $key: $value\n";
		}
	}
	file_put_contents($log_file, $log_message, FILE_APPEND);
}



//DB CONNECTION
$DBConn = pg_connect("dbname='e2fax' user='domains'");

if(!$DBConn)
{
	//echo "Error : Unable to open database\n";
	logMessage(' Error : Unable to open database');

}
else
{
	//  echo "Opened database successfully\n";
	logMessage(' DB Connection Established');
}








logMessage( '*********************' . 'START'.'********************** ');

// Check if all fields are set
if (isset($_POST['currentSimNo'], $_POST['currentSimPhoneNo'], $_POST['newSimNo'], $_POST['newSimPhoneNo'])) 
{
	logMessage('Came Inside if condition with 4 parameters');

	$currentSimNo = trim($_POST['currentSimNo']);
	$currentSimPhoneNo = trim($_POST['currentSimPhoneNo']);
	$newSimNo = trim($_POST['newSimNo']);
	$newSimPhoneNo = trim($_POST['newSimPhoneNo']);

	logMessage("\nCurrent SIM No:$currentSimNo \nCurrent SIM Phone No:$currentSimPhoneNo \nNew SIM No: $newSimNo \nNew SIM Phone No: $newSimPhoneNo");

	$checkExistanceQuery = "select * from sim_stock where simno='$newSimNo'";
	logMessage("Sim stock Existence check query -> ".$checkExistanceQuery);
	$ExecutechecksimStockQuery = pg_query($DBConn,$checkExistanceQuery);
	$issimavailable = pg_num_rows($ExecutechecksimStockQuery);
	if($issimavailable ==0)
	{
		logMessage("Simno is NOT available in SIMMIS, starting to book in sim stock updating the dummy number, simno old->$currentSimNo new->$newSimNo");

		UpdateSim($currentSimNo,$newSimNo,$currentSimPhoneNo,$newSimPhoneNo);
	}
	else
	{

		$checksimStockQuery2 = "select * from sim_stock where availability=true and simno='$newSimNo'";
		logMessage("Sim stock check query -> ".$checksimStockQuery2);
		$ExecutechecksimStockQuery2 = pg_query($DBConn,$checksimStockQuery2);
		$isavailable = pg_num_rows($ExecutechecksimStockQuery2);
		if($isavailable > 0)
		{
			logMessage("Simno is available to book in sim stock simno old->$currentSimNo new->$newSimNo");
			UpdateSim($currentSimNo,$newSimNo,$currentSimPhoneNo,$newSimPhoneNo);

		}
		else
		{
			logMessage("simno and sim phone number updating failed because its not available for simno old->$currentSimNo new->$newSimNo ");
			echo json_encode([
				"status" => "error",
				"message" => "Simno and sim is not available for booking"
			]);
			exit;

		}
	}


} 
else 
{
	// Return an error if data is missing
	http_response_code(400);

	$response = [
		"status" => "error",
		"message" => "Missing required fields. Please check your input."
	];
}

ob_end_clean();
echo json_encode($response);
exit;





function UpdateSim($currentSimNo,$newSimNo,$currentSimPhoneNo,$newSimPhoneNo)
{
	global $DBConn,$log_file;

	//Master Stock Check
	$MasterStockCheckQuery = "select * from tsim_blank_sims where iccid = '$newSimNo' and available=true and active=true";
	logMessage("MasterStockCheckQuery  -> ".$MasterStockCheckQuery );	
	$ExecuteMasterStockCheckQuery = pg_query($DBConn,$MasterStockCheckQuery);
	$getnumberofrows = pg_num_rows($ExecuteMasterStockCheckQuery);
	if($getnumberofrows > 0)
	{
		logMessage("Sim is available in master stock");

		//vendor check
		$newSimNo = trim($newSimNo);

		// Prepare your query (single row using subqueries)
		$VendorCheckQuery = "SELECT (SELECT vendor FROM tsim_blank_sims WHERE iccid = '$newSimNo') AS blank_stock_vendor,(SELECT operator FROM tsim_blank_sims WHERE iccid = '$newSimNo') AS blank_stock_operator,(SELECT vendor FROM sim_stock WHERE simno = '$currentSimNo') AS sim_stock_vendor,(SELECT operator FROM sim_stock WHERE simno = '$currentSimNo') AS sim_stock_operator, (SELECT pin1 FROM tsim_blank_sims WHERE iccid = '$newSimNo') AS blank_stock_pinno, (SELECT puk1 FROM tsim_blank_sims WHERE iccid = '$newSimNo') AS blank_stock_puk,";

		logMessage(" Check Vendor Query -> ".$VendorCheckQuery);

		$ExecuteVendorCheckQuery = pg_query($DBConn,$VendorCheckQuery);
		if($ExecuteVendorCheckQuery)
		{
			$rowCount = pg_num_rows($ExecuteVendorCheckQuery);
			logMessage("Row count returned: " . $rowCount);

			if ($rowCount > 0) 
			{
				$row = pg_fetch_assoc($ExecuteVendorCheckQuery);
				logMessage("Fetch result: " . json_encode($row));


				// Assign the values to PHP variables
				$blankStockVendor = $row['blank_stock_vendor'] ?? null;
				$blankStockOperator = $row['blank_stock_operator'] ?? null;
				$simStockVendor = $row['sim_stock_vendor'] ?? null;
				$simStockOperator = $row['sim_stock_operator'] ?? null;
				$blankStockpinno = $row['blank_stock_pinno'] ?? null; 
				$blankStockpuk = $row['blank_stock_puk'] ?? null;


				logMessage("Blank_stock_vendor -> " . $blankStockVendor);
				logMessage("Blank_stock_Operator -> " . $blankStockOperator);

				logMessage("Sim_stock_vendor -> " . $simStockVendor);
				logMessage("Sim_stock_Operator -> " . $simStockOperator);

				logMessage("blank_stock_pinno -> " . $blankStockpinno);
                                logMessage("blank_stock_puk -> " . $blankStockpuk);


				if(strcasecmp($blankStockVendor, $simStockVendor) === 0)
				{
					UpdateOperation($currentSimNo,$newSimNo,$currentSimPhoneNo,$newSimPhoneNo,$blankStockpinno,$blankStockpuk);
				}
				elseif(strcasecmp($blankStockOperator, $simStockVendor) === 0)
				{
					UpdateOperation($currentSimNo,$newSimNo,$currentSimPhoneNo,$newSimPhoneNo,$blankStockpinno,$blankStockpuk);
				}
				else
				{
					//Vendor is not same case.
					logMessage("Entering the Simno vendor is not matching.");
					echo json_encode([
						"status" => "error",
						"message" => "Vendor is not matching with the blank sim and sim stock Contact R n D"
					]);
					exit;
				}
			}
			else
			{
				//checking vendor query failed case.
				logMessage("Checking Vendor Query Execution Failed ");
			}

		}else
		{
			logMessage("No vendors found for given SIM.");
		}	}
		else
		{
			logMessage("Enetered Sim no is not present in master stock ");
			echo json_encode([
				"status" => "error",
				"message" => "Enetered Sim no is not present in master stock  Contact R n D"
			]);
			exit;
		}

}


function UpdateOperation($currentSimNo,$newSimNo,$currentSimPhoneNo,$newSimPhoneNo,$blankStockpinno,$blankStockpuk)
{
	global $DBConn,$log_file;

	if($blankStockpinno == NULL && $blankStockpuk == NULL)
		$simStockUpdateQuery = "Update sim_stock set sim_phone_no='$newSimPhoneNo',simno='$newSimNo',availability=false where simno='$currentSimNo' and sim_phone_no='$currentSimPhoneNo'";
	else
		$simStockUpdateQuery = "Update sim_stock set sim_phone_no='$newSimPhoneNo',simno='$newSimNo',availability=false,pinno='$blankStockpinno',puk1='$blankStockpuk' where simno='$currentSimNo' and sim_phone_no='$currentSimPhoneNo'";
	logMessage("Sim stock update query -> ".$simStockUpdateQuery);
	$ExecutesimStockUpdateQuery = pg_query($DBConn,$simStockUpdateQuery);
	if($ExecutesimStockUpdateQuery)
	{
		logMessage("Simno and sim phone number updated successfully in sim stock simno old->$currentSimNo new->$newSimNo ");

		$BlankcardUpdateQuery = "Update tsim_blank_sims set available=false where iccid='$newSimNo'";
		logMessage("Blank card update query -> ".$BlankcardUpdateQuery);
		$ExecuteBlankcardUpdateQuery = pg_query($DBConn,$BlankcardUpdateQuery);
		if($ExecuteBlankcardUpdateQuery)
			logMessage("Tsim blank sims table updating successful");
		else
			logMessage("Tsim blank sims table updating failed");

	}
	else
	{
		logMessage("simno and sim phone number updating failed for simno old->$currentSimNo new->$newSimNo ");
		echo json_encode([
			"status" => "error",
			"message" => "Simno and sim phone no not updated in sim stock table contact R n D"
		]);
		exit;
	}


	$ClienttripUpdateQuery = "Update clienttrip set sim_phone_no='$newSimPhoneNo' where sim_phone_no='$currentSimPhoneNo'";
	logMessage("Clienttrip update query -> ".$ClienttripUpdateQuery);
	$ExecuteClienttripUpdateQuery = pg_query($DBConn,$ClienttripUpdateQuery);
	if($ExecuteClienttripUpdateQuery)
	{
		logMessage("Sim phone number updated successfully in Clienttrip sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
	}
	else
	{
		logMessage("Sim phone number updating Failed in Clienttrip sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
		echo json_encode([
			"status" => "error",
			"message" => "sim phone no not updated in clienttrip table contact R n D"
		]);
		exit;
	}




	$simPhonenoLockUpdateQuery = "Update sim_phone_no_lock set sim_phone_no='$newSimPhoneNo' where sim_phone_no='$currentSimPhoneNo'";
	logMessage("Sim_phone_no_lock update query -> ".$simPhonenoLockUpdateQuery);
	$ExecutesimPhonenoLockUpdateQuery = pg_query($DBConn,$simPhonenoLockUpdateQuery);
	if($ExecutesimPhonenoLockUpdateQuery)
	{
		logMessage("Sim phone number updated successfully in sim_phone_no_lock sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
	}
	else
	{
		logMessage("Sim phone number updating Failed in sim_phone_no_lock sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
		echo json_encode([
			"status" => "error",
			"message" => "sim phone no not updated in sim phone no lock table contact R n D"
		]);
		exit;
	}



	$global_sim_cc_payer_detSelectQuery = "select * from global_sim_cc_payer_det where sim_phone_no='$currentSimPhoneNo'";
	logMessage("global_sim_cc_payer_det update query -> ".$global_sim_cc_payer_detSelectQuery);
	$Executeglobal_sim_cc_payer_detUpdateQuery = pg_query($DBConn,$global_sim_cc_payer_detSelectQuery);
	$get_count_of_global_sim_cc_payer_det = pg_num_rows($Executeglobal_sim_cc_payer_detUpdateQuery);
	if($get_count_of_global_sim_cc_payer_det > 0)
	{
		$simPhonenoUpdateQuery = "Update global_sim_cc_payer_det set sim_phone_no='$newSimPhoneNo' where sim_phone_no='$currentSimPhoneNo'";
		logMessage("global_sim_cc_payer_det update query -> ".$simPhonenoUpdateQuery);
		$ExecutesimPhonenoUpdateQuery = pg_query($DBConn,$simPhonenoUpdateQuery);
		if($ExecutesimPhonenoUpdateQuery)
		{
			logMessage("Sim phone number updated successfully in global_sim_cc_payer_det table  sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
		}
		else
		{
			logMessage("Sim phone number updating Failed in global_sim_cc_payer_det table sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");

			echo json_encode([
				"status" => "error",
				"message" => "sim phone no not updated in global_sim_cc_payer_det table contact R n D"
			]);
			exit;
		}
	}
	else
	{
		logMessage("Sim phone number is not present in global_sim_cc_payer_det table sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
	}



	$global_sim_cc_sim_amount_detSelectQuery = "select * from global_sim_cc_sim_amount_det where sim_phone_no='$currentSimPhoneNo'";
	logMessage("global_sim_cc_sim_amount_det update query -> ".$global_sim_cc_sim_amount_detSelectQuery);
	$Executeglobal_sim_cc_sim_amount_detUpdateQuery = pg_query($DBConn,$global_sim_cc_sim_amount_detSelectQuery);
	$get_count_of_global_sim_cc_sim_amount_det = pg_num_rows($Executeglobal_sim_cc_sim_amount_detUpdateQuery);
	if($get_count_of_global_sim_cc_sim_amount_det > 0)
	{
		$simPhonenoUpdateQuery = "Update global_sim_cc_sim_amount_det set sim_phone_no='$newSimPhoneNo' where sim_phone_no='$currentSimPhoneNo'";
		logMessage("global_sim_cc_sim_amount_det update query -> ".$simPhonenoUpdateQuery);
		$ExecutesimPhonenoUpdateQuery = pg_query($DBConn,$simPhonenoUpdateQuery);
		if($ExecutesimPhonenoUpdateQuery)
		{
			logMessage("Sim phone number updated successfully in global_sim_cc_sim_amount_det table  sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
		}
		else
		{
			logMessage("Sim phone number updating Failed in global_sim_cc_sim_amount_det table sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");

			echo json_encode([
				"status" => "error",
				"message" => "sim phone no not updated in global_sim_cc_sim_amount_det table contact R n D"
			]);
			exit;
		}
	}
	else
	{
		logMessage("Sim phone number is not present in global_sim_cc_sim_amount_det table sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
	}



	$checkCallerIDSelectQuery = "select * from callerid_user where sim_phone_no='$currentSimPhoneNo'";
	logMessage("check Caller ID select query -> ".$checkCallerIDSelectQuery);
	$ExecutecheckCallerIDQuery = pg_query($DBConn,$checkCallerIDSelectQuery);
	$get_count_of_checkCallerID = pg_num_rows($ExecutecheckCallerIDQuery);
	if($get_count_of_checkCallerID > 0)
	{
		$simPhonenoUpdateQuery = "Update callerid_user set sim_phone_no='$newSimPhoneNo' where sim_phone_no='$currentSimPhoneNo'";
		logMessage("checkCallerID update query -> ".$simPhonenoUpdateQuery);
		$ExecutesimPhonenoUpdateQuery = pg_query($DBConn,$simPhonenoUpdateQuery);
		if($ExecutesimPhonenoUpdateQuery)
		{
			logMessage("Sim phone number updated successfully in callerid_user table  sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
		}
		else
		{
			logMessage("Sim phone number updating Failed in callerid_user table sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");

			echo json_encode([
				"status" => "error",
				"message" => "sim phone no not updated in callerid_user table, contact R n D"
			]);
			exit;
		}
	}
	else
	{
		logMessage("Sim phone number is not present in callerid_user table sim Phone no old->$currentSimPhoneNo new->$newSimPhoneNo ");
		$selectQuery = "select username from clienttrip where sim_phone_no='$newSimPhoneNo'";
		logMessage("get username query -> ".$selectQuery);
		$ExecuteselectQuery = pg_query($DBConn,$selectQuery);
		$getusername = pg_fetch_all($ExecuteselectQuery);
		$username = $getusername[0]['username'];


		$insertCallerIDQuery = "insert into callerid_user (username,regd_number) values ('$username','$newSimPhoneNo');";
		logMessage("Inserting Callerid query".$insertCallerIDQuery);
		$ExecuteinsertQuery = pg_query($DBConn,$insertCallerIDQuery);
		if($ExecuteinsertQuery)
			logMessage("Insert Query of callerid successfully");
		else
			logMessage("Insert Query of callerid_user table Failed");
	}



	// Return success message
	$response = [
		"status" => "success",
		"message" => "SIM information received successfully."
	];


	// Return a success message to the client
	//echo "SIM information received successfully.";



	ob_end_clean();
	echo json_encode($response);
	exit;

}




?>
