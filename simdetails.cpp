#include "errno.h"
#include "parsestring.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include<string.h>
#include<stdlib.h>
#include <chrono>
#include <ctime>
#include<stdio.h>
#include "fax_database.c"
#include "cgi-util.c"
#include <fstream>
#include "coredump.h"
#include "linklist.h"
#include <libxml/xmlreader.h>
#include <sstream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <thread>
#include <algorithm>
#include <cctype>
int do_log(char * );
#include "send_dd_mail.h"
#include "taginfo.h"
#define E2FAXDB "e2fax"
#define CONFIG_DATA_FILE "/usr/local/ownmail/data/sim_config"
#include <iostream>

#include <algorithm>

#include <vector>

int do_log(char * );
char DBSERV[201];
char * domainname;
int create_csv_file(char * from, char * to, string status_filter = "all_but_canceled");
int stock_entry(int argc, char ** argv, char ** env); //int stock_entry(char *fname,char *user,char *country);
int get_parameter_from_xml(char * inputfile, char * parameter, char * val);

int do_log(char * log) {
    FILE * fp = fopen("/tmp/sim_trip_detail_log.log", "a");

    if (fp) {
        time_t t = time(NULL);
        struct tm * c_date = localtime( & t);
        char * t_now = strdup(asctime(c_date));
        t_now[strlen(t_now) - 1] = 0;
        fprintf(fp, "[TimeStamp: %s  PID: %d] : %s\n", t_now, getpid(), log);
        fclose(fp);
        return 1;
    }
    return -1;
}

char * get_simmis_data_from_table(char * key, char * dfault) {
    fax_database db("e2fax");
    char query[1024];
    sprintf(query, "select value from config_values where name='simmis' and key='%s'", key);
    do_log(query);

    if (db.ExecTuplesOk(query)) {
        return db.get_field(0, "value");
    } else {
        return dfault;
    }
}

void removeSpaces(char * str) {
    int length = strlen(str);
    int j = 0;

    for (int i = 0; i < length; ++i) {
        if (str[i] != ' ') {
            str[j++] = str[i];
        }
    }

    str[j] = '\0'; // Add null terminator at the end
}

void removeSpacesfromlineitem(char * str) {
    int length = strlen(str);
    bool withinQuotes = false; // Flag to track if within quotes
    int j = 0;

    for (int i = 0; i < length; ++i) {
        // Check for quotes
        if (str[i] == '"') {
            withinQuotes = !withinQuotes; // Toggle the withinQuotes flag
            str[j++] = str[i]; // Keep the quote
        }
        // Check for spaces, but only if not within quotes
        else if (!withinQuotes && str[i] == ' ') {
            // Skip spaces
        }
        // For all other characters
        else {
            str[j++] = str[i];
        }
    }

    str[j] = '\0'; // Add null terminator at the end
}

/*
int isnumber(char *value)
{
    	char ch;
    	if (value == NULL) return 0;

    	while ((ch = value[0])) 
	{
        	if (ch >= '0' && ch <= '9') 
		{
            		value++;
        	}
        	else return 0;
    	}
    	return 1;
}*/

int isnumber(char * value) {
    char ch;
    if (value == NULL) return 0;

    bool foundDigit = false;

    while ((ch = value[0])) {
        if (ch >= '0' && ch <= '9') {
            foundDigit = true;
            value++;
        } else if (ch == '-' || ch == '_') {
            value++; // Skip hyphens and underscores
        } else {
            return 0; // If any other non-digit character is found, return 0
        }
    }
    return foundDigit ? 1 : 0;
}

char * hide_cc_number(char * cc, char * op) {
    char cc_num[5];
    strncpy(cc_num, cc, 4);
    cc_num[4] = '\0';
    char * p = cc;
    p += strlen(cc) - 4;
    sprintf(op, "%sXXXXXXXX%s", cc_num, p);
    return op;
}


int main(int argc, char ** argv, char ** env) {
    do_log("Main function is started");
    int cgi_err;
    char * domain_name = getenv("ALTERNATE_DOMAIN");
    char command[1024];
    char * f = NULL;
    int i, len;
    char * action;

    if ((get_tag_value(CONFIG_DATA_FILE, "<!-- DBSERVER", "-- DBSERVER>", "dbserv", DBSERV) != 1) || (DBSERV == NULL || DBSERV[0] == 0)) {
        do_log("Could not open config file.");
        printf("Error occured while opening config file.");
        strcpy(DBSERV, "50.16.226.255");
    }

    /*	    
    	char* data = get_simmis_data_from_table("DBSERVER_dbserv","50.16.226.255");
    	strcpy(DBSERV,data);
    */

    fax_database db(E2FAXDB, DBSERV);
    if (domain_name == NULL || domain_name[0] == 0) {
        domain_name = getenv("HTTP_HOST");
    }
    do_log("start");
    if (argc > 1) {
        do_log("Filename is: ");
        do_log(argv[1]); //filename
        do_log("Username is : ");
        do_log(argv[2]); //username
        do_log("Type is : ");
        do_log(argv[3]); //type
        do_log("SENDING AMAZON FLAG");
        do_log(argv[4]); //amazon Flag
        do_log("Calling the Stock Entry function from main");
        int result = stock_entry(sizeof(argv) / sizeof(argv[0]), argv, env); //int result=stock_entry(argv[1],argv[2],argv[3]);
        if (result == 1) {
            printf("Success \n");
        } else if (result == 0) {
            printf("Fail \n");
        } else if (result == 3) {
            printf("-Partial \n");
        } else {
            printf("NotDone \n");
        }
        cgi_exit(0);
    }
    if (((cgi_err = cgi_init()) != CGIERR_NONE)) {
        printf("Content-type: text/plain\n\n");
        do_log("INSIDE CGI ERROR");
        printf("%s", cgi_strerror(cgi_err));
        cgi_exit(0);
    }

    action = cgi_getentrystr("a");
    printf("Content-type: text/html\n\n");
    if (action == NULL) {

        printf("Content-type: text/html\n\n");
        printf("<html>\n");
        printf("<head>\n");
        draw_style();
        printf("</head>\n");
        printf("<body>\n");
        printf("<script type=\"text/javascript\" src=\"simtrip.js\"></script>\n");
        printf("<form name=sim_trip action=simtrip.e  method=post>\n");
        printf("<input type=hidden name=a>");
        printf("<br><br><br><br><br><br><br><br>");
        printf("<br><br><label>Enter from Date:</label><input type=text name=txtfrom onFocus=\"GetDate(this);\" onclick=\"GetDate(this);\"onkeypress=\"DestroyCalendar();\"><br>");
        printf("<script type=\"text/javascript\" src=\"calendar.js\">\n");
        printf("</script>\n");
        printf("<style type=\"text/css\" src=\"calendar.css\">\n");
        printf("</style>\n");
        printf("<br><label>Enter to Date:</label><input type=text name=txtto onFocus=\"GetDate(this);\" onclick=\"GetDate(this);\" onkeypress=\"DestroyCalendar();\"><br>");
        printf("<input type=submit onclick=\"return createcsv();\">");
        printf("</form>");
        printf("</body>");
        printf("</html>");
    } else if (!(strcmp(action, "tripdet"))) {
        char * fromdt = cgi_getentrystr("txtfrom");
        char * todt = cgi_getentrystr("txtto");
        string status_filter = cgi_getentrystr("status_filter");
        int result = create_csv_file(replacestring(fromdt, "/", "-"), replacestring(todt, "/", "-"), status_filter);
        if (result == 0) {
            printf("Content-type: text/html\n\n");
            draw_header(4);
            printf("<br><center><b>No Records found for the specified period.</b></center>");
            printf("\n<br><center><input type=button name=back value=Back onclick=\"moveback();\"></center>");
            draw_end();
        }
    } else if (!(strcmp(action, "stock"))) {
        printf("Content-type:text/html\n\n");
        printf("<html>");
        draw_header(5);
        draw_style();
        printf("<body>");
        int i;
        printf("<form name=sim_stock action=simtrip.e method=post >");
        printf("<input type=hidden name=a>");
        sprintf(command, "select countryname from countrydata");
        int count = db.ExecTuplesOk(command);
        printf("<script type=\"text/javascript\" src=\"simtrip.js\"></script>\n");
        printf("<label>Select a country</label>");
        printf(" <Select name=optcountry id=\"ioptcountry\" title=\"Select country from the list.\" >");
        printf("<option value='None' Selected>Select</option>\n");
        for (i = 0; i < count; i++) {
            printf("<option value='%s'>%s</option>\n", db.get_field(i, "countryname"), db.get_field(i, "countryname"));
        }
        printf("</Select><br><br>\n");
        printf("<label>Select a csv file:</label>");
        printf("<input type=file name=filepath size=40>\n<br>");
        //printf("<INPUT TYPE=SUBMIT VALUE=\"Submit\">");
        printf("<input type=submit name=insert onclick=\"return sinsert();\">\n");
        printf("</form>");
    } else if (!(strcmp(action, "stock_insert"))) {

        //char *fname=cgi_getentrystr("filepath");
        char * country = cgi_getentrystr("optcountry");
        int result;
        //do_log(fname);
        do_log("stock entry");
        char fname[1024];
        strcpy(fname, "/tmp/global_stock.csv");
        //      char arr[] = {fname,"sarita",country,"all"};
        char arr[] = {
            fname,
            "sarita",
            "all"
        };
        char * arr1 = arr;
        result = stock_entry(3, & arr1, env); //result=stock_entry(fname,"saritha",country);
        //result = stock_entry(4,&arr1,env);
        printf("Content-type:text/html\n\n");
        printf("<html>");
        printf("<body>");

        if (result == 1) {
            printf("Sim Stock entry was successfully done.");
            do_log("Sim Stock entry was successfully done.");
        } else if (result == 2) {
            printf("Stock entry already done for this file");
        } else if (result == 3) {
            printf("stock entry done but with sime errors");

        } else {
            printf("Sim Stock entry failed");
            do_log("Sim Stock entry failed");
        }
        printf("<script type=\"text/javascript\" src=\"simtrip.js\"></script>\n");
        printf("<input type=button name=back value=\"BACK\" ONCLICK=\"return refpage();\">\n");
        printf("</body></html>");
    }
    return 0;
}

void trim(char* str) {
    // Trim trailing spaces
    int length = strlen(str);
    while (length > 0 && (str[length - 1] == ' ' || str[length - 1] == '\t' || str[length - 1] == '\n' || str[length - 1] == '\r')) {
        str[--length] = '\0';
    }
}
int stock_entry(int argc, char ** argv, char ** env) {
    do_log("Stock Entry function");
    do_log("This is File Name");
    char * fname = argv[1];
    do_log(fname);
    do_log("This is User");
    char * user = argv[2];
    do_log(user);
    do_log("This is Type");
    char * type = argv[3];
    do_log(type);

    do_log("AMAZON FLAG CHECK ");

    do_log("This is AmazonEntryFlag");
    char * isAmazonEntry = argv[4];
    do_log(isAmazonEntry);
    /*do_log("THIS IS ASIN");
      char *ASIN = argv[6];
      do_log(ASIN);
      do_log("This is Shipment ID");
      char *shipmentID = argv[7];
      do_log(shipmentID);
      do_log("This is Amazon Market Place");
      char *amazonMarketPlace = argv[8];
      do_log(amazonMarketPlace);*/

    if (isAmazonEntry || isAmazonEntry == 1) {
        do_log("Sending Stock to Amazon ");
    } else {
        do_log("This is not an Amazon Entry");
    }

    char query[1024];
    struct timeval tv1;
    gettimeofday( & tv1, 0);
    FILE * f;
    char fp[1024];
    char err[1024];
    char line[1024];
    char scode[1024];
    char seq_table[1024];
    char unsavedno[1024];
    fax_database sin(E2FAXDB, DBSERV);
    int i, l;
    i = 0;
    l = 0;
    /*
            if (sin.ExecCommandOk("begin work") != 1) 
            {
                    do_log("102-Could not begin work");
                    //redirect(102, "System Error", url);
            }
    */
    do_log(fname);
    do_log(user);
    do_log(isAmazonEntry);
    int isAmazonEntryInt;
    if (isAmazonEntry != NULL) {
	isAmazonEntryInt = atoi(isAmazonEntry);
	do_log("isAmazonEntry is a not  null pointer.");
    }

    if ((f = fopen(fname, "r")) == NULL) {
        do_log("file cannot be opened");
        return 0;
    }
    do_log("file opened");

    printf("Content-type:text/html\n\n");
    printf("Processing...\n");

    std::cout << "Content-type: text/html\n\n";
    std::cout << "Processing... Please wait.<br>\n" << std::flush;

    while (fgets(line, sizeof(line), f)) {
	line[strcspn(line, "\n")] = '\0';
	 // Trim the line
        trim(line);
	if (line[0] == '\0') {
            continue; // Skip empty lines
        }
        char simno[1024];
        char sim_phone[1024];
	char order_no[30]; // 19 digits + 1 for the null terminator
        char srno[1024];
        char vendor[1024];
        char pin[1024];
        char puk[1024];
        char autobook[10];
        char activation_reqd[10];
        //NEW
	char reseller[1024];
	char operator_name[1024];
	char plan_loaded[10];
	char purchase_price[1024];
	char expiry_date[21];//11+1 for null terminator
	//
	char ASIN[13];
        char shipmentID[13];
	char sku[13];
	char tripid_min_sku[1024];
        char amazonMarketPlace[4];
        char country[128];
        char comment[1024];
	strcpy(comment,"");
        char simnocheck[128];
        char sim_phone_check[128];
        char pin_check[128];
        char puk_check[128];
	char qrtext[256];
	strcpy(qrtext,"");//Simtype
	char sim_type[50];
	strcpy(sim_type,"");
	std::vector<std::string> values;
	std::string value;
        char * templine = strip_char(line, '"');
        strcpy(line, templine);
        l = l + 1;
        do_log("getting file");
        do_log("this is line");
        do_log(line);
        removeSpacesfromlineitem(line);
        do_log("Below is modified Line");
        do_log(line);
        char line_copy[1024];
        strcpy(line_copy, line);
        do_log("this is user");
        do_log(user);
        std::istringstream ss(line_copy);
         const char *base_query =
        	                            "insert into sim_stock (simno, sim_phone_no, srno, availability, country, vendor, entry_by_user, "
 	                  	            "dateandtime, active, pinno, puk1, reseller, operator, plan_loaded, purchase_price, expiry_date, "
                	                    "autobook, activation_reqd";

	char buyer_email[1024];
	// std::string value1;
        // std::string value2;
        // std::string prevValue;

        // while (std::getline(ss, value1, ','))
        // {
        // 	prevValue = value2;
        // 	value2 = value1; // Store the previous value before overwriting	
        // }

        //    // Store the last item in 'country'
        // strcpy(country,value2.c_str());

        // // Store the second last item in 'comment' for amazon_order_fetch 
        // strcpy(comment,prevValue.c_str());
        
	while (std::getline(ss, value, ',')) {
           // cout << value << endl;
	   values.push_back(value);
}

        //if (values.size() == 18) {}
	if (isAmazonEntryInt == 2){
       do_log("In if condition");
            std::string var2 = values[values.size() - 3];
            std::string var3 = values[values.size() - 2];
            std::string var4 = values[values.size() - 1];
            //std::cout << "Variables (18 values): " << var2 << ", " << var3 << ", " << var4 << std::endl;
            //strcpy(comment,var1);
            ////do_log(comment);
            strcpy(comment, var2.c_str());
            do_log(comment);
            strcpy(country, var3.c_str());
            do_log(country);
            strcpy(buyer_email, var4.c_str());
            do_log("Buyer email is present : ");
            removeSpaces(buyer_email);
            do_log(buyer_email); 

		/*	    
	    char command2[1024];
            sprintf(command2,"select sim_country_sku('%s') as sku;",country);
	    do_log(command2);
            sin.ExecTuplesOk(command2);
            sprintf(sku, "%s", sin.get_field(0, "sku"));
            do_log("SKU is : ");
            do_log(sku);
	    char *tripid_min = sku;
		*/


char command2[1024];
sprintf(command2, "select sim_country_sku('%s') as sku;", country);
do_log(command2);

if (!sin.ExecTuplesOk(command2)) {
    do_log("Error executing SQL query");
    return; // or handle the error appropriately
}

char* sku_value = sin.get_field(0, "sku");
if (sku_value == NULL) {
    do_log("Error retrieving SKU");
    return; // or handle the error appropriately
}

char sku[strlen(sku_value)+1];
strcpy(sku,sku_value);


do_log("SKU is : ");
do_log(sku);

strcpy(tripid_min_sku,sku);

do_log("tripid_min_sku : ");
do_log(tripid_min_sku);



	    } 
	else if (isAmazonEntryInt == 1){
	    do_log("In else if condition for amazon 1");	
		 do_log("in else");
              //for old
              std::string var8 = values[values.size() - 11];
              std::string var7 = values[values.size() - 10];
              std::string var6 = values[values.size() - 9];
              std::string var5 = values[values.size() - 8];
              std::string var4 = values[values.size() - 7];
              //for new
              std::string var3 = values[values.size() - 3];
              std::string var2 = values[values.size() - 2];
              std::string var1 = values[values.size() - 1];
              //std::cout << "Variables (17 values): " << var3 << ", " << var4 << std::endl;
              //New values 
              do_log("Reseller is : ");
             strcpy(reseller, var8.c_str());
             do_log(reseller);
  
              do_log("operator_name is : ");
              strcpy(operator_name, var7.c_str());
              do_log(operator_name);
  
              do_log("plan_loaded is : ");
              strcpy(plan_loaded, var6.c_str());
              do_log(plan_loaded);
  
              do_log("purchase_price is : ");
              strcpy(purchase_price, var5.c_str());
              do_log(purchase_price);
  
  
              do_log("expiry_date is : ");
              strcpy(expiry_date, var4.c_str());
              do_log(expiry_date);
  
              //
  
  
              do_log("sim_type is : ");
              strcpy(sim_type, var3.c_str());
              do_log(sim_type);
              //sim_type
              do_log("qrtext is : ");
              strcpy(qrtext, var2.c_str());
              do_log(qrtext);
              do_log("Country is : ");
              strcpy(country, var1.c_str());
              do_log(country);	
		
/*	      char command0[1024]; 
      	      sprintf(command0,"select sim_country_sku('%s') as sku;",country);	
	      do_log(command0);
	      sin.ExecTuplesOk(command0); 
	      sprintf(sku, "%s", sin.get_field(0, "sku"));
	      do_log("SKU is : ");
	      do_log(sku);
*/

char command2[1024];
sprintf(command2, "select sim_country_sku('%s') as sku;", country);
do_log(command2);

if (!sin.ExecTuplesOk(command2)) {
    do_log("Error executing SQL query");
    return; // or handle the error appropriately
}

char* sku_value = sin.get_field(0, "sku");
if (sku_value == NULL) {
    do_log("Error retrieving SKU");
    return; // or handle the error appropriately
}

char sku[strlen(sku_value)+1];
strcpy(sku,sku_value);


do_log("SKU is : ");
do_log(sku);

strcpy(tripid_min_sku,sku);

do_log("tripid_min_sku : ");
do_log(tripid_min_sku);

	
	
	
	}



	else {
            do_log("in else");
	    //for old
	    std::string var8 = values[values.size() - 8];
	    std::string var7 = values[values.size() - 7];
	    std::string var6 = values[values.size() - 6];
	    std::string var5 = values[values.size() - 5];
	    std::string var4 = values[values.size() - 4];
	    //for new
	    std::string var3 = values[values.size() - 3];
            std::string var2 = values[values.size() - 2];
            std::string var1 = values[values.size() - 1];
            //std::cout << "Variables (17 values): " << var3 << ", " << var4 << std::endl;
            //New values 
	    do_log("Reseller is : ");
	   strcpy(reseller, var8.c_str());
	   do_log(reseller);

	    do_log("operator_name is : ");
	    strcpy(operator_name, var7.c_str());
	    do_log(operator_name);

	    do_log("plan_loaded is : ");
	    strcpy(plan_loaded, var6.c_str());
	    do_log(plan_loaded);

	    do_log("purchase_price is : ");
	    strcpy(purchase_price, var5.c_str());
	    do_log(purchase_price);
		

	    do_log("expiry_date is : ");
	    strcpy(expiry_date, var4.c_str());
	    do_log(expiry_date); 

	    //
	    
	    
	    do_log("sim_type is : ");
	    strcpy(sim_type, var3.c_str());
	    do_log(sim_type);
	    //sim_type
	    do_log("qrtext is : ");
	    strcpy(qrtext, var2.c_str());
            do_log(qrtext);
	    do_log("Country is : ");
            strcpy(country, var1.c_str());
            do_log(country);

/*
	    char command1[1024];
            sprintf(command1,"select sim_country_sku('%s') as sku;",country);
	    do_log(command1);
            sin.ExecTuplesOk(command1);
            sprintf(sku, "%s", sin.get_field(0, "sku"));
            do_log("SKU is : ");
            do_log(sku);
	    */


char command2[1024];
sprintf(command2, "select sim_country_sku('%s') as sku;", country);
do_log(command2);

if (!sin.ExecTuplesOk(command2)) {
    do_log("Error executing SQL query");
    return; // or handle the error appropriately
}

char* sku_value = sin.get_field(0, "sku");
if (sku_value == NULL) {
    do_log("Error retrieving SKU");
    return; // or handle the error appropriately
}

char sku[strlen(sku_value)+1];
strcpy(sku,sku_value);


do_log("SKU is : ");
do_log(sku);

strcpy(tripid_min_sku,sku);

do_log("tripid_min_sku : ");
do_log(tripid_min_sku);



            //do_log(var1);
        } 
	/*else {
            std::cerr << "Error: input string does not have at least 17 comma-separated values" << std::endl;
            do_log("Error: input string does not have at least 17 comma-separated values"); 
	    continue;
        }*/
	do_log("Below is the operator");
	do_log(operator_name);
	do_log("Below is the reseller");
	do_log(reseller);
	do_log("Below is the plan_loaded");
	do_log(plan_loaded);
	do_log("Below is the purchase_price");
	do_log(purchase_price);
	do_log("Below is the expiry_date");
	do_log(expiry_date);
	do_log("Below is the sim_type");
	do_log(sim_type);
        do_log("Below is the Country");
        do_log(country);
        do_log("Below is the Comment");
        do_log(comment);
        do_log("Below is the qrtext");
        do_log(qrtext);
	do_log("tripid_min_sku : ");
	do_log(tripid_min_sku);

        // Assign the comment value and ensure it's not empty
	/*
        if (comment == "") {
            comment == "   ";
        }
	*/
        std::stringstream ss1(line_copy);
        std::string value12;
        int count = 0;

        // Read each value separated by comma
        while (std::getline(ss1, value12, ',') && count < 4) {
            switch (count) {
            case 0:
                strcpy(simnocheck, value12.c_str());
                break;
            case 1:
                strcpy(sim_phone_check, value12.c_str());
                break;
            case 2:
                strcpy(pin_check, value12.c_str());
                break;
            case 3:
                strcpy(puk_check, value12.c_str());
                break;
            }
            count++;
        }

        do_log("Below is the simnocheck");
        do_log(simnocheck);
	/* char* token1 = strtok(sim_phone_check, "_");

                            while (token1 != nullptr) {
                                strncpy(order_no, token1, sizeof(order_no) - 1);
                                order_no[sizeof(order_no)-1] = '\0';
                                token1 = strtok(nullptr, "_");
                            }
	do_log(order_no);
        */
	do_log("Below is the sim_phone_check");
        do_log(sim_phone_check);
        do_log("Below is the pin_check");
        do_log(pin_check);
        do_log("Below is the puk_check");
        do_log(puk_check);
	do_log("tripid_min_sku : ");
	do_log(tripid_min_sku);

        //check simno,sim_phone_no,puk,pin for numeric values, if they are not do not add to stock
        if (!isnumber(simnocheck) || !isnumber(sim_phone_check) || !isnumber(pin_check) || !isnumber(puk_check)) {
            if (!isnumber(simnocheck)) {
                do_log("The entry in simno is not a number.");
            }
            if (!isnumber(sim_phone_check)) {
                do_log("The entry in sim_phone_no is not a number.");
            }
            if (!isnumber(pin_check)) {
                do_log("The entry in pin is not a number.");
            }
            if (!isnumber(puk_check)) {
                do_log("The entry in puk is not a number.");
            }
            do_log("One of the entries in simno,sim_phone_no,puk,pin is not a number, may be the header line, just ignore it");
            continue;
        }
        if (sin.ExecCommandOk("begin work") != 1) {
            do_log("102-Could not begin work");
        }

        // Assuming 'country' is declared as char* & comment also
        std::string country_str(country);

        // Erase any trailing line breaks or whitespace from the string
        country_str.erase(country_str.find_last_not_of("\r\n") + 1);

        // Convert the string back to char* if needed
        const char * country_cstr = country_str.c_str();

        sprintf(query, "select srno, seq_table,countryname from countrydata where countryname ilike '%s';", country_cstr);

        do_log("++++++UPDATED COUNTRY BELOW++++++++++++++");
        do_log(country_cstr);

        removeSpaces(country);

        do_log(country);
        do_log(comment);
        do_log(query);

        if ((sin.ExecTuplesOk(query)) <= 0){
		if (isAmazonEntryInt == 2){ //This came from amazon_order_fetch.py
			do_log("country not present in countrydata, but this is from amazon_order_fetch, creating the same");
			sprintf(query,"insert into countrydata_base (countrycode,countryname,active,srno,seq_table,postpaid_prepaid,is_dd_enabled,charge_rent,card_type,add_credit_on_booking,api_code,add_e_credit_entry_on_booking,add_reg_charge,activation_country_mapping) values('%s','%s',true,'%s','other_sim_seq','prepaid',0,false,'country_specific',0::bit,'dummy',0::bit,0::bit,'OTHERS')",country_cstr,country_cstr,country_cstr);
			do_log(query);
			int res = sin.ExecCommandOk(query);
			if (!res){
				do_log("Failed to create country, still proceeding with the booking");
				char subj[101];
				char msg[501];
				sprintf(msg,"Hello,\n\nThe following query failed.\n\n%s\n\n\nPlease do the needful.",query);
				sprintf(subj,"Country creation failed in add stock");
				send_dd_mail("services@tsim.in",msg,subj);
			}
			sprintf(scode, "%s",country_cstr);
			sprintf(seq_table, "other_sim_seq");
		}
		else{
			do_log("country not present in countrydata");
			do_log("roll back");
			sin.ExecCommandOk("rollback work");
			return 0;
		}
        }
	else{
        	sprintf(scode, "%s", sin.get_field(0, "srno"));
        	sprintf(seq_table, "%s", sin.get_field(0, "seq_table"));
        	sprintf(country_cstr, "%s", sin.get_field(0, "countryname"));
	}
        do_log(scode);
        do_log(seq_table);
        do_log(country_cstr);
	do_log(sku);
	do_log(tripid_min_sku); 

        do_log("this is seq_table");
        do_log(seq_table);
        sprintf(err, "%d", errno);
        do_log(err);
        //printf("%d",errno);
        if (!strcmp(line, "")) {
            return 0;
        }
        char letter[1024];
        sprintf(letter, "%c", line[0]);
        do_log("Before Letter");
        do_log(letter);

        //*********1911**********checking add_credit_on_booking flag is present in countrydataTable and then adding voice user and sim_user entries
        char query1[1024];
        int add_credit_on_booking_flag;
        do_log("Checking  add credit and booking flag ");
        sprintf(query1, "select add_credit_on_booking from countrydata where countryname='%s'and add_credit_on_booking=1::bit", country_cstr);
        do_log(query1);
        if (sin.ExecTuplesOk(query1)) {
            do_log("The Add credit on booking flag is ONN setting the flag to 1");
            add_credit_on_booking_flag = 1;
        } else {
            do_log("The add credit on booking flag is OFF setting the flag to 0");
            add_credit_on_booking_flag = 0;
        }

        //if(((strstr(line,",,")!=NULL))||(!strcmp(letter,","))||(!strcmp(line,"E+")))
        //HERE
	/*
	if ((!strcmp(letter, ",")) || (!strcmp(line, "E+"))) {
            
		do_log("Cannot insert");
            if (!strcmp(unsavedno, "")) {
                do_log(line);
                sprintf(unsavedno, "%d", l);
            } else {
                sprintf(unsavedno, "%s,%d", unsavedno, l);

            }
        }
       */
	/*
	if (letter == "," || line == "E+") {
    do_log("Cannot insert");

    // If unsavedno is empty, start a new list of unsaved numbers
    if (unsavedno[0] == '\0') {  // Check if the char array is empty
       		do_log("line1");
	    do_log(line);
        snprintf(unsavedno, sizeof(unsavedno), "%ld", l); // Store 'l' as a string in unsavedno
    } else {
	    do_log("line2");
        // Append the new 'l' to the existing unsavedno, separated by a comma
        char temp[128];  // Temporary buffer to hold the new value
        snprintf(temp, sizeof(temp), ",%ld", l);
        strncat(unsavedno, temp, sizeof(unsavedno) - strlen(unsavedno) - 1); // Safe concatenation
    }
}
*/

//	else {
            do_log("Before affecting the line ==================================");
            do_log(line);

            sprintf(simno, "%s", replacestring(strtok(line, ","), " ", ""));
            removeSpaces(simno);
            do_log(simno);

            do_log("After affecting the line ++++++++++++++++++++++++++++++++++++");
            do_log(line);

            if (!strcmp(type, "all")) {
                sprintf(sim_phone, "%s", replacestring(strtok(NULL, ","), " ", ""));
                removeSpaces(sim_phone);
                do_log(sim_phone);
            } else {
                do_log("call curl");
                char agentusername[20] = "trikon";
                char agentpassword[20] = "K1N065qnoNE";
                char command[1024];
                char fname[50];
                char result[10] = "";
                sprintf(fname, "/tmp/pin_%s.xml", simno);
                do_log(fname);
                char curl_cmd[1024];
                sprintf(curl_cmd, "http://openapi.globalsimsupport.com/webapi/version400.aspx?requestxml=<AssignMsisdn><sim><iccid>%s</iccid></sim><Authentication><Username>%s</Username><Password>%s</Password></Authentication></AssignMsisdn>", simno, agentusername, agentpassword);
                //do_log(curl_cmd);
                snprintf(command, sizeof(command), "/usr/bin/curl -dump --connect-timeout 10 \"%s\">%s 2>/dev/null", curl_cmd, fname);
                do_log(command);
                FILE * pp = NULL;
                if (pp = popen(command, "r")) {
                    char * fl = strdupa(fname);
                    pclose(pp);
                    get_parameter_from_xml(fl, "REQUEST_STATUS", result);
                    do_log("Result");
                    do_log(result);
                    if (!strcmp(result, "1")) {
                        //get_parameter_from_xml(fl,"MSISDN",sim_phone);
                        get_parameter_from_xml(fl, "msisdn", sim_phone);
                        do_log(sim_phone);
                        remove(fname);
                    } else {
                        do_log("Failed to fetch the MSISDN");
                        remove(fname);
                        if (!strcmp(unsavedno, "")) {
                            sprintf(unsavedno, "%d", l);
                        } else {
                            sprintf(unsavedno, "%s,%d", unsavedno, l);
                        }
                        continue;
                        //return 0;
                    }
                } else {
                    do_log("Fail...");
                    continue;
                }
            }

            sprintf(pin, "%s", strtok(NULL, ","));
            removeSpaces(pin);
            do_log(pin);
            if (!strcmp(type, "iccid")) {
                sprintf(puk, "%s", replacestring(strtok(NULL, ","), "\n", ""));
            } else {
                sprintf(puk, "%s", strtok(NULL, ","));
            }
            removeSpaces(puk);
            do_log(puk);
            if (!strcmp(type, "all")) {
                sprintf(vendor, "%s", replacestring(strtok(NULL, ","), "\n", ""));
            } else {
                sprintf(vendor, "cloud9");
            }
            do_log("before vendor");
            removeSpaces(vendor);
            do_log(vendor);
            do_log("before tmp_autobook");
            char * tmp_autobook = strtok(NULL, ",");
            do_log("aft tmp_autobook");
            if (tmp_autobook == NULL) strcpy(autobook, "t");
            else strcpy(autobook, replacestring(tmp_autobook, "\n", ""));
            removeSpaces(autobook);
            sprintf(query, "autobook -%s-", autobook);
            do_log(query);
            char * tmp_activation_reqd = strtok(NULL, ",");
            do_log("aft tmp_activation_reqd");
            if (tmp_activation_reqd == NULL) strcpy(activation_reqd, "f");
            else strcpy(activation_reqd, replacestring(tmp_activation_reqd, "\n", ""));
            removeSpaces(activation_reqd);
	    
            sprintf(query, "activation_reqd -%s-", activation_reqd);
	    do_log(query);
	    //NEW VALUES
	    //
	    sprintf(reseller,"%s",replacestring(strtok(NULL, ","),"\n",""));
	    do_log(reseller);
	    
	    sprintf(operator_name,"%s",replacestring(strtok(NULL, ","),"\n",""));
            do_log(operator_name);
            
	    sprintf(plan_loaded,"%s",replacestring(strtok(NULL, ","),"\n",""));
            do_log(plan_loaded);    
            
	    sprintf(purchase_price,"%s",replacestring(strtok(NULL, ","),"\n",""));
            do_log(purchase_price);
	    
	   sprintf(expiry_date,"%s",replacestring(strtok(NULL, ","),"\n",""));
              do_log(expiry_date);

	    //
            /***********************************************/
            do_log(line_copy);

            if (isAmazonEntry || isAmazonEntry == 1) {
                istringstream ss(line_copy);
                string value;

                // Move the input stream to the 13th comma-separated value
                for (int i = 0; i < 12; i++) {
                    getline(ss, value, ',');
                }

                // Retrieve the last three comma-separated values and assign them to variables
                string var1, var2, var3, var4;
                if (!std::getline(ss, var1, ',') || !std::getline(ss, var2, ',') || !std::getline(ss, var3, ',') || !std::getline(ss, var4, ',')) {
                    std::cerr << "Error: input string does not have at least 16 comma-separated values" << std::endl;
		    sin.ExecCommandOk("rollback work");
                    do_log("roll back");
                    return 0;
                }

                strcpy(shipmentID, var1.substr(0, 12).c_str());
                strcpy(ASIN, var2.substr(0, 12).c_str());
                strcpy(amazonMarketPlace, var3.substr(0, 3).c_str());
                //strcpy(comment, var4.c_str());

                do_log("This is Shipment ID");
                removeSpaces(shipmentID);
                do_log(shipmentID);

                do_log("THIS IS ASIN");
                removeSpaces(ASIN);
                do_log(ASIN);

                do_log("This is Amazon Market Place");
                removeSpaces(amazonMarketPlace);
                do_log(amazonMarketPlace);

                // Assign the comment value and ensure it's not empty                         
               /* if (comment == "") {
                    strcpy(comment, "   ");
                }*/

                do_log("This is Comment");
                do_log(comment);
            }

            //check simno,sim_phone_no,puk,pin for numeric values, if they are not do not add to stock
            if (!isnumber(simno) || !isnumber(sim_phone) || !isnumber(pin) || !isnumber(puk)) {
                if (!isnumber(simnocheck)) {
                    do_log("The entry in simno is not a number.");
                }
                if (!isnumber(sim_phone_check)) {
                    do_log("The entry in sim_phone_no is not a number.");
                }
                if (!isnumber(pin_check)) {
                    do_log("The entry in pin is not a number.");
                }
                if (!isnumber(puk_check)) {
                    do_log("The entry in puk is not a number.");
                }
                do_log("One of the entries in simno,sim_phone_no,puk,pin is not a number, may be the header line, just ignore it");
                if (!strcmp(unsavedno, "")) {
                    sprintf(unsavedno, "%d", l);
                } else {
                    sprintf(unsavedno, "%s,%d", unsavedno, l);
                }
                continue;
            }

            //			sprintf(query,"/*NO LOAD BALANCE*/select * from sim_stock where simno='%s';",simno);
            sprintf(query, "select * from sim_stock where simno='%s';", simno);
            do_log(query);
            if ((sin.ExecTuplesOk(query)) <= 0) {
                sprintf(query, "select nextval('%s');", seq_table);
                do_log(query);
                if ((sin.ExecTuplesOk(query)) <= 0) {
                    do_log("102-ExecCommandOk Failed");
                    do_log("102-System Error");
                    do_log("roll back");
                    sin.ExecCommandOk("rollback work");
                    return 0;
                }
                sprintf(srno, "%s", sin.get_field(0, "nextval"));
                do_log(srno);
                do_log(simno);
                do_log(sim_phone);
                do_log(pin);
                do_log(puk);
                do_log(vendor);
                do_log(country);
                do_log(shipmentID);
                do_log(ASIN);
                do_log(amazonMarketPlace);
                do_log(comment);
		do_log(sku);
		do_log("Tripid_min_sku");
		do_log(tripid_min_sku);
                if ((strcmp(sim_phone, "")) || (strcmp(simno, ""))) {
                    if (!strcmp(country, "USA Euro Data SIM")) {
                        sprintf(query, "select overlay('%s' placing '4477' from 1 for 4)", sim_phone);
                        do_log(query);
                        char disp[20];
                        if (sin.ExecTuplesOk(query))
                            strcpy(disp, sin.get_field(0, "overlay"));
                        do_log(disp);
			do_log("initial");
                        sprintf(query, "insert into sim_stock (simno,sim_phone_no,srno,availability,country,vendor,entry_by_user,dateandtime,active,pinno,puk1,comment,sim_phone_no_disp,comment) values('%s','%s','%s-%s',true,'%s','%s','%s','now',true,'%s','%s',' ','%s','%s');", replacestring(simno, " ", ""), replacestring(sim_phone, " ", ""), scode, srno, country_cstr, replacestring(vendor, "\r", ""), user, pin, puk, disp, comment);
                    } else {
			    do_log("initial else");

                        if (strlen(autobook) == 0) strcpy(autobook,"t");
			if (strlen(activation_reqd) == 0) strcpy(activation_reqd,"f");
			if (isAmazonEntryInt == 2)
				if (strlen(qrtext)>0)
                            		sprintf(query, "insert into sim_stock (simno,sim_phone_no,srno,availability,country,vendor,entry_by_user,dateandtime,active,pinno,puk1,comment,qrtext,autobook,activation_reqd) values('%s','%s','%s-%s',true,'%s','%s','%s','now',true,'%s','%s','%s','%s','%s','%s');", replacestring(simno, " ", ""), replacestring(sim_phone, " ", ""), scode, srno, country_cstr, replacestring(vendor, "\r", ""), user, pin, puk, comment,qrtext,autobook,activation_reqd);

				else
                            		sprintf(query, "insert into sim_stock (simno,sim_phone_no,srno,availability,country,vendor,entry_by_user,dateandtime,active,pinno,puk1,comment,autobook,activation_reqd) values('%s','%s','%s-%s',true,'%s','%s','%s','now',true,'%s','%s','%s','%s','%s');", replacestring(simno, " ", ""), replacestring(sim_phone, " ", ""), scode, srno, country_cstr, replacestring(vendor, "\r", ""), user, pin, puk, comment, autobook, activation_reqd);
			else
				/*if (strlen(qrtext)>0)
                            		sprintf(query, "simno, sim_phone_no, srno, availability, country, vendor, entry_by_user, dateandtime, active, pinno, puk1, reseller, operator ,plan_loaded, purchase_price, expiry_date,autobook, activation_reqdnsert into sim_stock (simno,sim_phone_no,srno,availability,country,vendor,entry_by_user,dateandtime,active,pinno,puk1,qrtext,autobook,activation_reqd) values('%s','%s','%s-%s',true,'%s','%s','%s','now',true,'%s','%s','%s','%s','%s');", replacestring(simno, " ", ""), replacestring(sim_phone, " ", ""), scode, srno, country_cstr, replacestring(vendor, "\r", ""), user, pin, puk, qrtext,autobook,activation_reqd);
                        	else
                            		sprintf(query, "insert into sim_stock (simno,sim_phone_no,srno,availability,country,vendor,entry_by_user,dateandtime,active,pinno,puk1,autobook,activation_reqd) values('%s','%s','%s-%s',true,'%s','%s','%s','now',true,'%s','%s','%s','%s');", replacestring(simno, " ", ""), replacestring(sim_phone, " ", ""), scode, srno, country_cstr, replacestring(vendor, "\r", ""), user, pin, puk, autobook, activation_reqd);
*/
				
/*					const char *base_query = 
						    "insert into sim_stock (simno, sim_phone_no, srno, availability, country, vendor, entry_by_user, "
						    "dateandtime, active, pinno, puk1, reseller, operator, plan_loaded, purchase_price, expiry_date, "
						    "autobook, activation_reqd";
*/
		  				    do_log(buyer_email);
				do_log("Above is buyer_email");	
					 if (strlen (qrtext)> 0 && strlen(sim_type) > 0) {
					 sprintf(query,
							"%s, sim_type, qrtext) "
							"values ('%s', '%s', '%s-%s', true, '%s', '%s', '%s', "
							"'now', true, '%s', '%s', '%s', '%s', '%s', '%s', '%s', "
							"'%s', '%s', '%s', '%s');",
							base_query,
							replacestring(simno, " ", ""),
							replacestring(sim_phone, " ", ""),
							scode, srno, country_cstr,
							replacestring(vendor, "\r", ""),
							user, pin, puk, reseller, operator_name, plan_loaded, purchase_price, expiry_date,
							autobook, activation_reqd,
							sim_type, qrtext);
												
					}
					else if (strlen (qrtext) > 0) {
						sprintf(query,
							"%s, qrtext) "
							"values ('%s', '%s', '%s-%s', true, '%s', '%s', '%s', "
							"'now', true, '%s', '%s', '%s', '%s', '%s', '%s', '%s', "
							"'%s', '%s', '%s');",
							base_query,
							replacestring(simno, " ", ""),
							replacestring(sim_phone, " ", ""),
							scode, srno, country_cstr,
							replacestring(vendor, "\r", ""),
							user, pin, puk, reseller, operator_name, plan_loaded, purchase_price, expiry_date,
							autobook, activation_reqd,
							qrtext);
						do_log(sim_type);	
					}
					else if (strlen(sim_type) > 0) {
						sprintf(query,
							"%s, sim_type) "
							"values ('%s', '%s', '%s-%s', true, '%s', '%s', '%s', "
							"'now', true, '%s', '%s', '%s', '%s', '%s', '%s', '%s', "
							"'%s', '%s', '%s');",
							base_query,
							replacestring(simno, " ", ""),
							replacestring(sim_phone, " ", ""),
							scode, srno, country_cstr,
							replacestring(vendor, "\r", ""),
							user, pin, puk, reseller, operator_name, plan_loaded, purchase_price, expiry_date,
							autobook, activation_reqd,
							sim_type);
					}
					 else {
                                                      sprintf(query,
                                                          "%s) "
                                                          "values ('%s', '%s', '%s-%s', true, '%s', '%s', '%s', "
                                                          "'now', true, '%s', '%s', '%s', '%s', '%s', '%s', '%s', "
                                                          "'%s', '%s');",
                                                          base_query,
                                                          replacestring(simno, " ", ""),
                                                          replacestring(sim_phone, " ", ""),
                                                          scode, srno, country_cstr,
                                                          replacestring(vendor, "\r", ""),
                                                          user, pin, puk, reseller, operator_name, plan_loaded, purchase_price, expiry_date,
                                                          autobook, activation_reqd);
					 }

					
				 
				/*
				if (strlen(qrtext) > 0 && strlen(sim_type) > 0) {
					do_log("qrtext and sim_type");
				    sprintf(query,
					"insert into sim_stock (simno, sim_phone_no, srno, availability, country, vendor, entry_by_user, "
					"dateandtime, active, pinno, puk1, reseller, operator ,plan_loaded, purchase_price, expiry_date,"
					"sim_type, qrtext, autobook, activation_reqd) "
					"values ('%s', '%s', '%s-%s', true, '%s', '%s', '%s',"
				       	"'now', true, '%s','%s','%s', '%s', '%s', '%s', '%s',"
					" '%s', '%s', '%s', '%s');",
					replacestring(simno, " ", ""),
					replacestring(sim_phone, " ", ""),
					scode, srno, country_cstr,
					replacestring(vendor, "\r", ""),
					user, pin, puk, reseller, operator_name, plan_loaded, purchase_price, expiry_date,
					sim_type, qrtext, autobook, activation_reqd);
			}	
			
				else if (strlen(qrtext) > 0) {
					do_log("qrtext");
				    sprintf(query,
					"insert into sim_stock (simno, sim_phone_no, srno, availability, country, vendor, entry_by_user, "
					"dateandtime, active, pinno, puk1, reseller, operator ,plan_loaded, purchase_price, expiry_date,"
					"qrtext, autobook, activation_reqd) "
					"values ('%s', '%s', '%s-%s', true, '%s', '%s', '%s',"
				       	"'now', true, '%s','%s','%s', '%s', '%s', '%s', '%s',"
					"'%s', '%s', '%s');",
					replacestring(simno, " ", ""),
					replacestring(sim_phone, " ", ""),
					scode, srno, country_cstr,
					replacestring(vendor, "\r", ""),
					user, pin, puk, reseller, operator_name, plan_loaded, purchase_price, expiry_date,
					qrtext, autobook, activation_reqd);
					}	

			 	else if (strlen(sim_type) > 0) {                   
                                          do_log("sim_type");                         
                                      sprintf(query,                                  
                                          "insert into sim_stock (simno, sim_phone_no, srno, availability, country, vendor, entry_by_user, "
                                          "dateandtime, active, pinno, puk1, reseller, operator ,plan_loaded, purchase_price, expiry_date,"
                                          "sim_type, autobook, activation_reqd) "                                           
                                          "values ('%s', '%s', '%s-%s', true, '%s', '%s', '%s',"                            
                                          "'now', true, '%s','%s','%s', '%s', '%s', '%s', '%s',"                            
                                          "'%s', '%s', '%s');",                                                             
                                          replacestring(simno, " ", ""),                                                    
                                          replacestring(sim_phone, " ", ""),                                                
                                          scode, srno, country_cstr,                                                        
                                          replacestring(vendor, "\r", ""),                                                  
                                          user, pin, puk, reseller, operator_name, plan_loaded, purchase_price, expiry_date,
                                          sim_type, autobook, activation_reqd);                                             
                                          }    


				else {
					do_log("default");
				    sprintf(query,
					"insert into sim_stock (simno, sim_phone_no, srno, availability, country, vendor, entry_by_user, "
					"dateandtime, active, pinno, puk1, reseller, operator ,plan_loaded, purchase_price, expiry_date,"
					"autobook, activation_reqd) "
					"values ('%s', '%s', '%s-%s', true, '%s', '%s', '%s',"
				       	"'now', true, '%s','%s','%s', '%s', '%s', '%s', '%s',"
					"'%s', '%s');",
					replacestring(simno, " ", ""),
					replacestring(sim_phone, " ", ""),
					scode, srno, country_cstr,
					replacestring(vendor, "\r", ""),
					user, pin, puk, reseller, operator_name, plan_loaded, purchase_price, expiry_date,
					autobook, activation_reqd);
				}
				*/
                    }
                    do_log(query);
		    do_log("Test");

                    if (sin.ExecCommandOk(query) != 1) {
                        do_log("102-ExecCommandOk Failed");
                        do_log("102-System Error");
                        do_log("roll back");
                        sin.ExecCommandOk("rollback work");
                        //return 0;
                        if (!strcmp(unsavedno, "")) {
                            sprintf(unsavedno, "%d", l);
                        } else {
                            sprintf(unsavedno, "%s,%d", unsavedno, l);
                        }
                    } else {

                        do_log("SIM STOCK ENTRY DONE");
                        do_log("amazon flag");


                        // // Check if isAmazonEntry is a null-terminated string
                        // int i = 0;
                        // while (isAmazonEntry[i] != '\0') {
                        //     i++;
                        // }

                        // if (i == 0) {
                        //     do_log("Error: isAmazonEntry is not a null-terminated string.");
                        //     return 1; // Return an error code
                        // }

                        //int isAmazonEntryInt = atoi(isAmazonEntry);

                        //do_log(isAmazonEntryInt);
                        do_log(isAmazonEntry);
                        //*********1911****** if Amazon Entry insert into clienttrip and update Stock
                        if (isAmazonEntry || isAmazonEntry == 1) {

                            do_log("Its an Amazon Entry");
                            //clienttrip Entry
                            do_log("doing Clinetrip entry");
                            char from_date[11];
                            char to_date[11];
                            char username[100];
                            char base_user[20];
                            char username_prefix[20];
                            char tripid[20];

                            time_t now = time(nullptr); // get the current time
                            tm * tm_now = localtime( & now); // convert to local time

                            if (isAmazonEntryInt == 2) {
                                tm today_date = * tm_now;
				tm tm_plus_1_years = * tm_now;
                                mktime( & today_date);
				tm_plus_1_years.tm_year += 1;
                                mktime( & tm_plus_1_years);
                                strftime(from_date, sizeof(from_date), "%Y-%m-%d", & today_date);
                                strftime(to_date, sizeof(to_date), "%Y-%m-%d", &tm_plus_1_years );
                                do_log("Amazon entry is 2, From date in clinettrip is : ");
                                do_log(from_date);
                                do_log("Amazon entry is 2, To date in clienttrip is : ");
                                do_log(to_date);
                            } else {
                                tm tm_plus_2_years = * tm_now;
                                tm_plus_2_years.tm_year += 2;
                                mktime( & tm_plus_2_years);
                                tm tm_plus_3_years = * tm_now;
                                tm_plus_3_years.tm_year += 3;
                                mktime( & tm_plus_3_years);
                                strftime(from_date, sizeof(from_date), "%Y-%m-%d", & tm_plus_2_years);
                                strftime(to_date, sizeof(to_date), "%Y-%m-%d", & tm_plus_3_years);
                                do_log("From date in clienttrip is :");
                                do_log(from_date);
                                do_log("To date in clienttrip is : ");
                                do_log(to_date);
                            }
                            strcpy(base_user, "1555510323");
                            do_log("base_user");
                            do_log(base_user);
                            strcpy(username_prefix, "amazon_client");
                            do_log("username_prefix");
                            do_log(username_prefix);
                            //getting sim
                            do_log("getting sim_phone");
                            do_log(sim_phone);
			    // Copy the first 19 characters from sim_phone to order_no
			    /*strncpy(order_no, sim_phone, 19);
		            // Ensure the order_no string is null-terminated
			    order_no[19] = '\0';
			    */
			       /* char* token = strtok(sim_phone, "_");

			    while (token != nullptr) {
				strncpy(order_no, token, sizeof(order_no) - 1);  
				order_no[sizeof(order_no)-1] = '\0';
				token = strtok(nullptr, "_");
			    }
			    */
			    int i = 0;
			    while (sim_phone[i] != '_' && sim_phone[i] != '\0' ) {
				    order_no[i] = sim_phone[i];  
				    i++;  
				}

				// Null-terminate the order_no string
				order_no[i] = '\0';

		            do_log(order_no);
                            //getting username
                            auto now1 = chrono::system_clock::now();
                            time_t now_c = chrono::system_clock::to_time_t(now1);
                            struct tm * parts = localtime( & now_c);
                            char buf[80];
                            strftime(buf, 80, "%Y-%m-%d", parts);
                            string date(buf);
                            const char * date_cstr = date.data(); // convert char array to const char*
                            strcpy(username, "");
                            strcat(username, username_prefix);
                            strcat(username, "_");
                            strcat(username, sim_phone);
                            strcat(username, "_");
                            strcat(username, date_cstr);
                            do_log("getting username");
                            do_log(username);

                            //getting tripid        
                            do_log("getting tripid");
                            char tripid_idx[1024];
                            char tripid_query[1024];
                            char tripid_min[1024];
                            char tripid_half[1024];

                            sprintf(tripid_idx, "select nextval('trip_ids')");
                            do_log(tripid_idx);
                            if (sin.ExecTuplesOk(tripid_idx) <= 0) {
                                do_log("103-ExecTuplesOk Failed");
                                do_log("couldnt get trip_ids nextval");
                                do_log("roll back");
                                sin.ExecCommandOk("rollback work");
                                return 0;

                            }
                            strcpy(tripid_min, sin.get_field(0, "nextval"));
			    do_log(tripid_min);

			    do_log("Tripid_min_sku is below :");
                            do_log(tripid_min_sku);

                            strcpy(tripid_half, country_cstr);
                            do_log(tripid_half);

                            sprintf(tripid, "%s%s", tripid_half, tripid_min);
                            do_log("below is the trip id");
                            do_log(tripid);

                            char clientTripQuery[1024];

                            if (amazonMarketPlace[0] == 'I' && amazonMarketPlace[1] == 'N')
                                sprintf(clientTripQuery, "insert into clienttrip(username,tripid,domainname ,sim_phone_no,from_date,to_date,sim_deliver_date,booking_date,comment,status,tripid_min,handset_model,payment_mode,podno,is_kyc_required) values('%s','%s','tsim.in','%s','%s','%s','now','now','%s','Delivery pending','%s','%s','%s','%s',true)", username, tripid, sim_phone, from_date, to_date,comment, tripid_min_sku, amazonMarketPlace, ASIN, shipmentID);
                            else{
				if(isAmazonEntryInt == 2){
					sprintf(clientTripQuery, "insert into clienttrip(username,tripid,domainname ,sim_phone_no,from_date,to_date,sim_deliver_date,booking_date,comment,status,tripid_min,handset_model,payment_mode,podno,order_no) values('%s','%s','tsim.mobi','%s','%s','%s','now','now','%s','Delivery pending','%s','%s','%s','%s','%s')", username, tripid, sim_phone, from_date, to_date,comment, tripid_min_sku, amazonMarketPlace, ASIN, shipmentID,order_no);
				}
				else{
                                	sprintf(clientTripQuery, "insert into clienttrip(username,tripid,domainname ,sim_phone_no,from_date,to_date,sim_deliver_date,booking_date,comment,status,tripid_min,handset_model,payment_mode,podno) values('%s','%s','tsim.mobi','%s','%s','%s','now','now','%s','Delivery pending','%s','%s','%s','%s')", username, tripid, sim_phone, from_date, to_date,comment, tripid_min_sku, amazonMarketPlace, ASIN, shipmentID);
			    	}
			    }
                            do_log("Clienttrip insert query is : ");

                            do_log(clientTripQuery);
                            if (sin.ExecCommandOk(clientTripQuery) <= 0) {
                                do_log("103-ExecCommandOk Failed");
                                do_log("could not get insert into ClientTrip");
                                do_log("roll back");
                                sin.ExecCommandOk("rollback work");
                                return 0;
                            }
                            do_log("Data inserted into Client Trip");

                            //updating sim Stock
                            do_log("Now updating sim stock");
                            char updateClientTripQuery[1024];
                            sprintf(updateClientTripQuery, "UPDATE sim_stock set availability =false where sim_phone_no='%s'", sim_phone);
                            do_log(updateClientTripQuery);
                            if (sin.ExecCommandOk(updateClientTripQuery) <= 0) {
                                do_log("103-ExecCommandOk Failed");
                                do_log("could not update sim stock");
                                do_log("roll back");
                                sin.ExecCommandOk("rollback work");
                                return 0;
                            }
                            do_log("sim_Stock Updated");
                            //**********************************************************************************************************************************
                            if (add_credit_on_booking_flag || add_credit_on_booking_flag == 1) {
                                do_log("add credit on booking Flag is ONNNN");

                                //Random Alphanumeric Password Generator
                                string pwd = "";
                                string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
                                srand(time(nullptr));
                                for (int i = 0; i < 14; i++) {
                                    pwd += base64_chars[rand() % base64_chars.size()];
                                }
                                do_log("generated random password");
                                do_log(pwd.c_str());
                                //Password Generator End

                                /*
                                 	                                                       //add user
                                        	                                                string command = "/usr/local/ownmail/bin/add_voiceuser " +string(username) + " " +string(pwd) + " tsim.in Plan_SIM_Jul_2017 None services@tsim.in 0 " + string(pwd);
                                                	                                        do_log(command.c_str());
                                                        	                                system(command.c_str());
                                */

                                char command[1024];

                                if (amazonMarketPlace[0] == 'I' && amazonMarketPlace[1] == 'N')
                                    domainname = "tsim.in";
                                else
                                    domainname = "tsim.mobi";

                                sprintf(command, "select * from users where username ='%s' and domainname='%s'", username, domainname);
                                if (sin.ExecCommandOk(command) <= 0) {
				    do_log("Inserting into Users table ");
                                    sprintf(command, "INSERT INTO users (username,domainname,signed_on,last_update,credit,usage,country_code,currency,preferences) values ('%s','%s','now','now',0,0,'EF','INR',0)", username, domainname);
                                    do_log(command);
                                    if (sin.ExecCommandOk(command) <= 0)
                                        do_log("user adding failed");
                                    else
                                        do_log("user added");

                                    do_log("Adding user to gkusers");

                                    char gkuserQuery[1024];
                                    //									sprintf (gkuserQuery, "INSERT INTO gkusers values ('%s','%s','%s','00','1','55521#',true,false);",username,domainname,pwd);
                                    sprintf(gkuserQuery, "INSERT INTO gkusers (username,domainname,password,ext,rewrite_prefix,rewrite,enable,postpay) values ('%s','%s','%s','00','1','55561#',true,false);", username, domainname, pwd.c_str());
                                    do_log(gkuserQuery);
                                    if (sin.ExecCommandOk(gkuserQuery) != 1)
                                        do_log("gkuser adding failed");
                                    else
                                        do_log("gkuser added");
                                }

                                /*
                                                                                      	  	//gk users
                                                                                        	do_log("now updating gkuser");
                                                                      		               	char gkuserQuery[1024];
                                                                                        	sprintf(gkuserQuery,"UPDATE gkusers set rewrite='55561#' where username='%s'",username);
                                                                                        	do_log(gkuserQuery);
                                                                                        	if(sin.ExecCommandOk(gkuserQuery)<=0)
                                                                                        	{
                                                                                                	do_log("103-ExecCommandOk Failed");
                                                                                                	do_log("fail to update gkuser");
                                                                                                	do_log("roll back");
                                									sin.ExecCommandOk("rollback work");
                                                                                                	return 0;
                                                                                        	}
                                                                                        	do_log("gkuser updated");
                                */

                            } else {
                                do_log("add credit on booking Flag is OFFFFF");
                            }

                            if (amazonMarketPlace[0] == 'I' && amazonMarketPlace[1] == 'N')
                                domainname = "tsim.in";
                            else
                                domainname = "tsim.mobi";

                            //Sim User Entry
                            do_log("doing sim user entry");
                            char simuserQuery[1024];
                            if(buyer_email[0] == '\0'){
				    do_log("username in sim user is;");
				    do_log(username);
				    do_log("domainname in sim user is;");
				    do_log(domainname);
				    do_log("base_user in sim user is;");
				    do_log(base_user);
                                	do_log("Inserting into sim_user table without buyer email");
				    sprintf(simuserQuery, "insert into sim_user select '%s','%s',clientname,add_building,add_street,add_city,add_state,add_country,add_pinno,telno,mobno,emailadd,creditcard_no,expdate,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,sales_person,passport_no,companyname,salutation,cc_holder_name,passport_city,active,category,prev_category,primary_emailadd,reseller_sp from sim_user where username='%s'", username, domainname, base_user);
                            }
                            else{
				    do_log("username in sim user is;");
				    do_log(username);
				    do_log("domainname in sim user is;");
				    do_log(domainname);
				    do_log("buyer_email in sim user is;");
				    do_log(buyer_email);
				    do_log("base_user in sim user is;");
				    do_log(base_user);
				    do_log("Inserting into sim_user table with buyer email");
                                 sprintf(simuserQuery, "insert into sim_user select '%s','%s',clientname,add_building,add_street,add_city,add_state,add_country,add_pinno,telno,mobno,'%s',creditcard_no,expdate,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,sales_person,passport_no,companyname,salutation,cc_holder_name,passport_city,active,category,prev_category,primary_emailadd,reseller_sp from sim_user where username='%s'", username, domainname,buyer_email, base_user);
                            }
                           
                            do_log(simuserQuery);
                            if (sin.ExecCommandOk(simuserQuery) <= 0) {
                                do_log("103-ExecCommandOk Failed");
                                do_log("fail to insert sim user");
                                do_log("roll back");
                                sin.ExecCommandOk("rollback work");
                                return 0;
                            }
                            do_log("sim user Entered");
                        }

                        i = i + 1;
                        char result[1024];
                        sprintf(result, "Successful");
                        do_log(result);
                        sin.ExecCommandOk("commit work");
                        do_log("commit work");
                    }
                } else {
                    if (!strcmp(unsavedno, "")) {
                        sprintf(unsavedno, "%d", l);
                    } else {
                        sprintf(unsavedno, "%s,%d", unsavedno, l);
                    }
			 sin.ExecCommandOk("rollback work");
			 do_log("roll back");
                }
            }
	    else
	    {
			 sin.ExecCommandOk("rollback work");
			 do_log("roll back");
		    do_log("this simno is already present in the sim stock ");
		    do_log(simno);
		    cout << "This simno is already present in the sim stock " << simno << endl;

	    }
      // }


	  // **Step 1: Send HTTP Headers**
	   std::cout << "Content-type: text/html\n\n";
	   std::cout << "<html><body>\n";
	   std::cout << "<h2>Processing started... Please wait.</h2>\n" << std::flush;

	   // **Step 2: Start Processing Without Delays**
	   for (int i = 1; i <= 20; i++) {  
	       // **Print status updates**
	       std::cout << "<p>Processing step " << i << "...</p>\n" << std::flush;

	       // **Force immediate output**
	       printf("Step %d completed.<br>\n", i);
	       fflush(stdout);

	       // Your actual processing logic here (no sleep)
	       // Example: Call a system command or function
	       system("echo Running process step > /dev/null");  // Replace with actual logic
	   }

	   // **Step 3: Send Final Response**
	   std::cout << "<h2>Processing completed successfully!</h2>\n";
    		std::cout << "</body></html>\n" << std::flush;

    }

    if (strcmp(unsavedno, "")) {
        printf("%s", unsavedno);
        do_log(unsavedno);
    }

    fclose(f);
    if ((i <= 0) && (!strcmp(unsavedno, ""))) {
        return 2;
    } else if ((strcmp(unsavedno, "")) && (i <= 0)) {
        return 0;
    } else if ((strcmp(unsavedno, "")) && (i > 0)) {
        return 3;
    }
    return 1;
}

int create_csv_file(char * from, char * to, string status_filter) {
    char query[1024];
    fax_database db(E2FAXDB, DBSERV);
    char fname[1024];
    //strcpy(fname,"simdet.csv");
    sprintf(fname, "sim_detail%s%s.csv", replacestring(from, "-", ""), replacestring(to, "-", ""));
    do_log(fname);
    int i;

    /*For filtering based on status*/
    string status_filter_clause = (status_filter == "all_but_canceled" ? "not like 'Canceled'" : "like '" + status_filter + "'");

    //sprintf(query,"select c.username,clientname,companyname,sales_person,status,sim_phone_no,booking_date,from_date,to_date from clienttrip c,sim_user s where c.username=s.username and status not like 'Canceled' and booking_date>'%s' and booking_date<'%s' order by booking_date ",from,to);
    //sprintf(query,"select c.username,clientname,companyname,sales_person,status,sim_phone_no,booking_date,from_date,to_date,creditcard_no,expdate,REPLACE(TRANSLATE(tripid,REPLACE(TRANSLATE(tripid, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ ',RPAD('#',LENGTH(tripid),'#')),'#',''),RPAD('#',LENGTH(tripid),'#')),'#','') AS country,c.handset_model from clienttrip c,sim_user s where c.username=s.username and status not like 'Canceled' and booking_date>'%s' and booking_date< timestamp_pl_interval('%s','86400') order by booking_date ",from,to);
    //sprintf(query,"select c.username,clientname,companyname,sales_person,status,sim_phone_no,booking_date,from_date,to_date,creditcard_no,expdate,translate(tripid,'[0123456789]','') AS country,c.handset_model from clienttrip c,sim_user s where c.username=s.username and status not like 'Canceled' and booking_date>'%s' and booking_date< timestamp_pl_interval('%s','86400') order by booking_date ",from,to);
    sprintf(query, "select c.username,clientname,companyname,sales_person,status,c.sim_phone_no,booking_date,from_date,to_date,creditcard_no,expdate,country,c.handset_model from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status %s and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') order by booking_date", status_filter_clause.data(), from, to);
    do_log(query);
    //int count=1;//sel.ExecTuplesOk(query);
    int count = db.ExecTuplesOk(query);
    if (count > 0) {
        printf("Content-disposition: attachment; filename=%s\n\n", fname);
        printf("Usernname,Clientname,Company Name,Sales Person,Status,Sim Phone No,Booking Date,From Date,To date,Creditcard No.,Creditcard ExpDate,Country,Handset\n");
        for (i = 0; i < count; i++) {
            //                      do_log("file opened");
            //                       printf("Content-disposition: attachment; filename=%s\n\n", fname);
            char cc_num[31];
            char cc_num_show[31];
            //if (!(db.get_field(i,"creditcard_no") == NULL || db.get_field(i,"creditcard_no") == "") || strlen(db.get_field(i,"creditcard_no")) == 16 ){}
            if (strlen(db.get_field(i, "creditcard_no")) == 16) {
                strcpy(cc_num, db.get_field(i, "creditcard_no"));
                hide_cc_number(cc_num, cc_num_show);
            } else
                strcpy(cc_num_show, "");
            printf("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", db.get_field(i, "username"), db.get_field(i, "clientname"), db.get_field(i, "companyname"), db.get_field(i, "sales_person"), db.get_field(i, "status"), db.get_field(i, "sim_phone_no"), db.get_field(i, "booking_date"), db.get_field(i, "from_date"), db.get_field(i, "to_date"), cc_num_show, db.get_field(i, "expdate"), db.get_field(i, "country"), db.get_field(i, "handset_model"));
            //                      do_log("file closed");
        }
        return 1;
    } else
        return 0;
}

int get_parameter_from_xml(char * inputfile, char * parameter, char * val) {
    char * fl = strdup(inputfile);
    xmlDocPtr doc = xmlParseFile(fl);

    if (doc == NULL) {
        return 0;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (strstr((char * ) cur -> name, "Error")) {
        char err_msg[1024];
        cur = cur -> xmlChildrenNode;
        while (cur != NULL) {
            xmlNodePtr cur1 = cur;
            while (cur1 != NULL) {
                if (!xmlStrcmp(cur1 -> name,
                        (const xmlChar * )
                        "Error_Message")) {
                    xmlChar * key =
                        xmlNodeListGetString(doc,
                            cur1 ->
                            xmlChildrenNode,
                            1);
                    if (key != NULL)
                        do_log((char * ) key); //set_error_msg((char *)key);
                    xmlFree(key);
                }
                cur1 = cur1 -> next;
            }
            cur = cur -> next;
        }
        return 0;
    }
    cur = cur -> xmlChildrenNode;
    while (cur != NULL) {
        //      xmlNodePtr cur1 = xmlDocGetRootElement(cur);
        //xmlNodePtr cur1 = cur->xmlChildrenNode;
        xmlNodePtr cur1 = cur;
        cur1 = cur1 -> xmlChildrenNode;
        while (cur1 != NULL) {
            if (!xmlStrcmp(cur1 -> name,
                    (const xmlChar * ) parameter)) {
                xmlChar * key =
                    xmlNodeListGetString(doc,
                        cur1 ->
                        xmlChildrenNode,
                        1);
                if (key != NULL)
                    strcpy(val,
                        (const char * ) key);
                xmlFree(key);
            }
            cur1 = cur1 -> next;
        }
        cur = cur -> next;
    }
    xmlFreeDoc(doc);
}

