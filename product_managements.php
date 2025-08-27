<?php
date_default_timezone_set("Asia/Calcutta");
$log_file = '/tmp/product_mangement.log';
file_put_contents($log_file, date("Y-m-d H:i:s") . '*********************' . 'Product Management Script Log'.'********************** '.  PHP_EOL, FILE_APPEND);

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

// Check if the request is a POST request
if ($_SERVER['REQUEST_METHOD'] === 'POST') 
{
	file_put_contents($log_file, date("Y-m-d H:i:s") . '***********************Begin************************'. PHP_EOL, FILE_APPEND);

    	// Capture the raw JSON data
    	$json = file_get_contents('php://input');

    	// Decode the JSON data
    	$obj = json_decode($json, true);

    	// Check the action type based on different indicators
    	$action = '';

    	if (isset($obj['status'])) 
	{
		if ($obj['status'] === 'publish') 
		{
            		$action = 'Product Published';
        	} 
		elseif ($obj['status'] === 'private') 
		{
            		$action = 'Product updated to private';
        	}
    	}
	else
	{
		 $action = 'Product Deleted';	
	}
 

    	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Action: ' . $action . PHP_EOL, FILE_APPEND);
    	file_put_contents($log_file, date("Y-m-d H:i:s") . 'JSON -> ' . $json . PHP_EOL, FILE_APPEND);
	
	//file_put_contents($log_file, date("Y-m-d H:i:s") . 'OBJ -> ' . $obj . PHP_EOL, FILE_APPEND);	
   	//Log the decoded data
  	//file_put_contents($log_file, date("Y-m-d H:i:s") . 'Decoded -> ' . print_r($obj, true) . PHP_EOL, FILE_APPEND);


	$sku = isset($obj['sku'])?$obj['sku']:null;
        file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU -> '.$sku . PHP_EOL, FILE_APPEND);
        $price = isset($obj['price'])?$obj['price']:0;
        file_put_contents($log_file, date("Y-m-d H:i:s") . 'Price -> '.$price . PHP_EOL, FILE_APPEND);
	$productId = $obj['id'];
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Product ID -> '.$productId . PHP_EOL, FILE_APPEND);

	if (isset($obj['attributes']) && !empty($obj['attributes'])) 
	{
    		// Loop through the 'attributes' array
    		foreach ($obj['attributes'] as $attribute) {
        	// Get the value of the 'option' attribute
        	$optionValue = $attribute['option'];
        	// Output the option value
      		//  echo "Option Value: $optionValue";
    		}
	} 
	else 
	{
    		echo "No attributes found.";
	}

	file_put_contents($log_file, date("Y-m-d H:i:s") . 'OPTION VALUE HERE ->       '.$optionValue . PHP_EOL, FILE_APPEND);

	if($action=='Product Published')
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Inside Product Publish IF' . PHP_EOL, FILE_APPEND);
		checksku($obj);	
			
	}
	elseif($action=='Product updated to private')
	{
		
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Inside Procudt Update IF' . PHP_EOL, FILE_APPEND);
		checksku($obj);
	
	}
	elseif($action=='Product Deleted')
	{	
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Inside Procudt DELETE IF' . PHP_EOL, FILE_APPEND);
		
		$query = "select sku_id_in_woocommerce from tsim_pack_details where pack_name='$sku'";
		file_put_contents($log_file, date("Y-m-d H:i:s") . $query . PHP_EOL, FILE_APPEND);
		$ExecuteQuery = pg_query($DBConn,$query);
		if($ExecuteQuery)
		{
			$checkproductId = pg_fetch_result['sku_id_in_woocommerce'];
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Product Id got to be deleted is ->'. $checkproductId . PHP_EOL, FILE_APPEND);
			if($checkproductId)
			{
				deleteSku($productId);
			}
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Product ID is not mapped and present in tsim pack_details for this sku ->'.$sku. PHP_EOL, FILE_APPEND); 
			}

		}		
		else
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Query execution error'. PHP_EOL, FILE_APPEND);	
		}
	}
	else
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Inside Else IF' . PHP_EOL, FILE_APPEND);
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'New case status :'. $obj['status']    . PHP_EOL, FILE_APPEND);		
	}
	
}



function checksku($obj)
{
	global $DBConn,$log_file;
	
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Inside Check SKU Function ' . PHP_EOL, FILE_APPEND);	

	$sku = isset($obj['sku'])?$obj['sku']:null;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU -> '.$sku . PHP_EOL, FILE_APPEND);
	$price = isset($obj['price'])?$obj['price']:0;
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Price -> '.$price . PHP_EOL, FILE_APPEND);
	$productId = $obj['id'];
        file_put_contents($log_file, date("Y-m-d H:i:s") . 'Product ID -> '.$productId . PHP_EOL, FILE_APPEND);

	 if (isset($obj['attributes']) && !empty($obj['attributes']))
        {
                // Loop through the 'attributes' array
                foreach ($obj['attributes'] as $attribute) {
                // Get the value of the 'option' attribute
                $optionValue = $attribute['option'];
                // Output the option value
                //  echo "Option Value: $optionValue";
                }
        }
        else
        {
                echo "No attributes found.";
        }

        file_put_contents($log_file, date("Y-m-d H:i:s") . 'OPTION VALUE HERE ->       '.$optionValue . PHP_EOL, FILE_APPEND);


        // Split the string by comma to get individual components
        $components = explode(", ", $optionValue);

        // Extract the Days and short description
        $days = "";
        $shortDescription = "";

        // Iterate through the components
        foreach ($components as $component)
        {
                // Check if the component contains "Days"
                if (strpos($component, "Days") !== false)
                {
                        // Extract the Days
                        $days = trim($component);
                }
                else
                {
                        // Add the component to short description
                        $shortDescription .= $component . ", ";
                }
        }

        // Remove trailing comma and space from short description
        $short_description = rtrim($shortDescription, ", ");
	$validity = $days;

	 file_put_contents($log_file, date("Y-m-d H:i:s") . 'Validity  -> ' . $validity.'  Short Description -> '.$short_description . PHP_EOL, FILE_APPEND);



	
	if($sku)	
	{
		$query = "select * from tsim_pack_details where pack_name='$sku';";
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Query -> ' . $query . PHP_EOL, FILE_APPEND);	
		$ExecQuery = pg_query($DBConn,$query);
		if($ExecQuery)
		{
			$getpack = pg_fetch_assoc($ExecQuery);
			$pack_name = $getpack['pack_name'];
			$pack_cost = $getpack['pack_cost'];
			
			if($pack_name != null && $pack_cost != null)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Pack_name -> ' . $pack_name.'  Pack_cost -> '.$pack_cost . PHP_EOL, FILE_APPEND);
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU is present updating the sku entry in tsim_pack_details. ' . PHP_EOL, FILE_APPEND);
				updateSku($sku,$price,$short_description,$validity,$productId);
			}
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU NOT PRESENT IN TSIM_PACK_DETAILS ' . PHP_EOL, FILE_APPEND);
				skuEntry($sku,$price,$short_description,$validity,$productId);	
			}		
		
		}
		else
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Query Execution got failed '. PHP_EOL, FILE_APPEND);
		}
		
	}
	else
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'SKU IS EMPTY IN PRODUCT JSON DATA ' . PHP_EOL, FILE_APPEND);
	}

}







function skuEntry($sku,$price,$short_description,$validity,$productId)
{
	global $DBConn,$log_file;
	
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Inside SKU Entry Function  ' . PHP_EOL, FILE_APPEND);
	

	
	$price_without_gst = $price/1.18;
	$price_without_gst = number_format($price_without_gst, 3);
	$price_without_gst = str_replace(',', '', $price_without_gst);

	file_put_contents($log_file, date("Y-m-d H:i:s") .'PRICE WITHOUT GST -> '.$price_without_gst . PHP_EOL, FILE_APPEND);

	$insert1 ="insert into tsim_pack_details (countryname,pack_name,pack_cost,pack_details,pack_validity,sku_id_in_woocommerce) values ('$sku','$sku','$price_without_gst','$short_description','$validity','$productId')";	
	file_put_contents($log_file, date("Y-m-d H:i:s") . $insert1 . PHP_EOL, FILE_APPEND);
	$Execquery1 = pg_query($DBConn,$insert1);
		
	if($Execquery1)
	{			
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Insert Sucessfully in tsim_pack_details table -> sku '.$sku . PHP_EOL, FILE_APPEND);
	}
	else
	{
		 file_put_contents($log_file, date("Y-m-d H:i:s") . 'Insert Failed in tsim_pack_details table -> sku  ' . $sku  . PHP_EOL, FILE_APPEND);
	}	
	
}


function updateSku($sku,$price,$short_description,$validity,$productId)
{
	global $DBConn,$log_file;
	
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Inside SKU Update Function  ' . PHP_EOL, FILE_APPEND);
		
	$query2 = "select * from tsim_pack_details where pack_name='$sku';";
	file_put_contents($log_file, date("Y-m-d H:i:s") . $query2 . PHP_EOL, FILE_APPEND);	
	$Execquery2 = pg_query($DBConn,$query2);
	
	if($Execquery2)
        {
		$getsku = pg_fetch_assoc($Execquery2);
		$checksku = $getsku['pack_name'];
		$checkprice = $getsku['pack_cost'];
		if($checksku)
		{
                	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Found in  tsim_pack_details table -> sku '.$checksku . PHP_EOL, FILE_APPEND);
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Updating SKU in  tsim_pack_details table -> sku '.$checksku  . PHP_EOL, FILE_APPEND);			
			$updatequery = "update tsim_pack_details set countryname='$sku',pack_name='$sku',pack_cost='$price',pack_validity='$validity',pack_details='$short_description',sku_id_in_woocommerce='$productId' where pack_name='$checksku'";
			file_put_contents($log_file, date("Y-m-d H:i:s") . $updatequery . PHP_EOL, FILE_APPEND);	
			$Execupdatequery = pg_query($DBConn,$updatequery);
			if($Execupdatequery)
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Updated'  . PHP_EOL, FILE_APPEND);
			}	
			else
			{
				file_put_contents($log_file, date("Y-m-d H:i:s") . 'Not Updated' . PHP_EOL, FILE_APPEND);
			}
								
		}	
		else
		{
			file_put_contents($log_file, date("Y-m-d H:i:s") . 'Error getting SKU in  tsim_pack_details table -> sku  ' . $sku  . PHP_EOL, FILE_APPEND);
		}
				

        }
        else
        {
        	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Not Found in  tsim_pack_details table -> sku  ' . $sku  . PHP_EOL, FILE_APPEND);
        }
	
}



function deleteSku($productID)
{
	global $DBConn,$log_file;

	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Inside SKU Delete Function  ' . PHP_EOL, FILE_APPEND);
	
	$Query = "delete from tsim_pack_details where sku_id_in_woocommerce='$productID'";
	file_put_contents($log_file, date("Y-m-d H:i:s") . 'Delete Query ->'.$Query  . PHP_EOL, FILE_APPEND);	
	$ExecuteQuery = pg_query($DBConn,$Query);	
	
	if($ExecuteQuery)
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . 'Deleted Sku sucessfully Product ID -> '.  $productID . PHP_EOL, FILE_APPEND);
	}
	else
	{
		file_put_contents($log_file, date("Y-m-d H:i:s") . ' Deleted Sku failed Product ID ->'. $productID  . PHP_EOL, FILE_APPEND);
	} 
}


?>
