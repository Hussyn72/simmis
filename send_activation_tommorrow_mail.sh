#!/bin/bash

# Query the database for the client name, activation date, and email address
#query="SELECT COALESCE(ct.clientname, su.clientname) AS clientname,ct.sim_phone_no,TO_CHAR(ct.from_date, 'YYYY-MM-DD') AS activation_date,COALESCE(ct.emailadd, su.primary_emailadd) AS email_address,ct.order_no FROM clienttrip ct LEFT JOIN sim_user su ON ct.username = su.username WHERE ct.from_date BETWEEN NOW() AND (NOW() + INTERVAL '1 DAY') AND ct.activated = false AND ct.status in ('Delivery pending','Delivered') and ct.sim_phone_no in (select sim_phone_no from sim_stock where activation_reqd = true);"
query="SELECT COALESCE(ct.clientname, su.clientname) AS clientname,ct.sim_phone_no,TO_CHAR(ct.from_date, 'YYYY-MM-DD') AS activation_date,CASE WHEN ct.emailadd IS NOT NULL THEN ct.emailadd WHEN ct.emailadd IS NULL AND su.primary_emailadd = 'japan001@tsim.in' THEN NULL ELSE su.primary_emailadd END AS email_address,ct.order_no FROM clienttrip ct LEFT JOIN sim_user su ON ct.username = su.username WHERE ct.from_date BETWEEN NOW() AND (NOW() + INTERVAL '1 DAY') AND ct.activated = false AND ct.status IN ('Delivery pending', 'Delivered') AND ct.sim_phone_no IN (SELECT sim_phone_no FROM sim_stock WHERE activation_reqd = true) AND (ct.emailadd IS NOT NULL OR su.primary_emailadd != 'japan001@tsim.in');"
result=$(echo "$query" | /usr/local/pgsql/bin/psql -q -t -A e2fax)
echo "result $result";

# Iterate over each row returned by the query
echo "$result" | while read line; do
  client_name=$(echo "$line" | cut -d "|" -f 1)
  sim_phone_no=$(echo "$line" | cut -d "|" -f 2)
  activation_date=$(echo "$line" | cut -d "|" -f 3)
  email_address=$(echo "$line" | cut -d "|" -f 4)
  order_no=$(echo "$line" | cut -d "|" -f 5)
  echo "$client_name $activation_date $email_address $order_no"

  # Calculate the date 24 hours before the activation date
  reminder_date=$(date -d "$activation_date -24 hours" "+%F")
  echo "$reminder_date"


query1="select simno from sim_stock where sim_phone_no = '$sim_phone_no'";
serial_no=$(/usr/local/pgsql/bin/psql -q -t -A e2fax -c "$query1")
echo "serial_no $serial_no"




  # Construct the email message
  subject="SIM Card Activation Status"
  body="Dear $client_name,\n\nWe hope this email finds you well. We are writing to inform you that your SIM card activation is currently under process.The SIM card activation will be completed by end of day tomorrow $activation_date.\nYou will receive a notification via email as soon as your SIM card is activated of serial No: $serial_no.\nWe appreciate your patience during this process. Should you have any questions or concerns, please do not hesitate to contact our customer support team.\nThank you for choosing TSIM Communication as your mobile service provider.\n\nWe look forward to serving you.\n\nPlease reply to this message for assistance \n\nBest regards,\nTSIM Team"


  # Send the email if the reminder date is today
  if [ "$(date +%Y-%m-%d)" == "$reminder_date" ]; then
    echo "sending email"
	
    if [ -n "$order_no" ]; then
       from_address="services@tsim.in"
    else
       from_address="services@tsim.mobi"
    fi

    echo -e "$body" | mail -s "$subject" -r "TSIM Communication Services <$from_address>"  "$email_address" #"husain@staff.ownmail.com"        #"$email_address"      # "husain@staff.ownmail.com"  # "$email_address"
	    if [ -n "$order_no" ]; then
		echo -e "From: services@tsim.in\r\nTo: $email_address\r\nSubject: SIM Card Activation Status\r\nX-PutIn: Sent\r\n$body" | /usr/bin/sendmail services@tsim.in
	    else
	    	echo -e "From: services@tsim.mobi\r\nTo: $email_address\r\nSubject: SIM Card Activation Status\r\nX-PutIn: Sent\r\n$body" | /usr/bin/sendmail services@tsim.mobi
	    fi	    
  else
    echo "not sending email"
  fi
done
