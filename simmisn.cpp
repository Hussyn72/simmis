#include <iostream>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
//#include <algorithm>
#include "parsestring.h"
#include "verify.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "add_what.h"
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "fax_database.c"
#include "cgi-util.c"
#include "coredump.h"
#include "doadmin.h"
#include "flaginfo.h"
#define MAX_FIELDS_STR 20
#define MAX_LEN_STR 251
#include "split_string.h"
#include <unistd.h> // For getcwd


using namespace std;
char mis_user[201];
bool isAdmin=0;
bool canUpgrade=0;
bool canchange_sales=0;
bool can_change_stock = false;		//permission to access 'edit stock' section
bool return_date_restrict = true;		//Imposing 7 day limit on sim return date
bool can_view_cc=0;					//Permission to view Credit Card number
char *agentusername;
char *psswd;
char DBSERV[201];
char DBSERV_R[201];
int do_log(char *log);
#define CURL_CMD "/usr/bin/curl --insecure"
#include "send_dd_mailn.h"
//#include "send_dd_mail_test.h"
#include "taginfo.h"
//#define E2FAXDB "e2fax_test"
#define E2FAXDB "e2fax"
#define ADD_PROG "/usr/local/ownmail/bin/add_voiceuser"
#define CONFIG_DATA_FILE "/usr/local/ownmail/data/sim_config"
//#define CONFIG_DATA_FILE "/usr/local/ownmail/data/sim_config_test"
#define EMAIL_TO_CUSTOMER "/usr/local/ownmail/data/.email_sim_customer_on_booking"
//#define DBSERV "50.16.226.255"
#include "date_ops.h"
#include "gsim.h"
#include "simmisn.h"

#define SMS_FROM "919833057630"
//#define SMS_FROM "+919870145601"
#define CURL_PATH "/usr/bin/curl --insecure --silent"
#define SMS_URL "https://www.siptraffic.com/myaccount/sendsms.php"
#define SMS_UNAME "trikonelec"
#define SMS_PWD "blhebrgu"
#define SMS_POOL "/usr/local/ownmail/data/sms_spool/"
#define SMS_SPOOL_TMP "temp"
#define UNLIMITED_SECOND_LEG "91"
#define ECS "Europe Country Specific"
#define CASE_SRC_CNT 10
#define CRM_OPT_CNT 7
#define CRM_CASE_OF_CNT 2
#define CREATE_RZP_LINK	"/usr/local/ownmail/bin/create_rz_pay_link.php"
#define PHP_CMD_P "/usr/local/bin/php"
char *trimwhitespace(char *str);

int show_status(fax_database *db,char *simno);
int draw_tripdetailpage(fax_database *db);
int draw_ddpage(fax_database *db);
int draw_tripdetails(char *frm,char *country);		//int draw_tripdetails(const char *frm,const char *country);
int draw_summary(fax_database *db, bool dnld=0);
void draw_month(char const *objname);
void draw_date(char const *objname);
void draw_year(char const *objname);
int edituser(fax_database *db);
//int editstock();
int show_client(fax_database *db);
int show_stock(fax_database *db);
int draw_booking(fax_database *db);
//int advance_booking(fax_database *db);
int add_ddno(fax_database *db);
int draw_simstatus_page(fax_database *db);
void status_alert(char *simno);
//void get_clientinfo(const char *salutation,const char *clientname,const char *companyname,const char *add_bldg,const char *add_street,const char *add_city,const char *add_pinno,const char *add_state,const char *add_country,const char *deliver_add_bldg,const char *deliver_add_street,const char *deliver_add_city,const char *deliver_add_pinno,const char *deliver_add_state,const char *deliver_add_country,const char *telno,const char *mobno,const char *emailadd,const char *creditcard_no,const char *cc_holder_name,const char *expdate,const char *sales_person,char *passport_no,char *passport_city, char *domainname, char *categ);
void get_clientinfo(const char *salutation,const char *clientname,const char *companyname,const char *add_bldg,const char *add_street,const char *add_city,const char *add_pinno,const char *add_state,const char *add_country,const char *deliver_add_bldg,const char *deliver_add_street,const char *deliver_add_city,const char *deliver_add_pinno,const char *deliver_add_state,const char *deliver_add_country,const char *telno,const char *mobno,const char *emailadd,const char *creditcard_no,const char *cc_holder_name,const char *expdate,const char *sales_person,char *passport_no,char *passport_city, char *domainname, char *categ,const char *reseller_sp);
char *getnew_username(fax_database *db,const char *mobno,const char *domainname);
int booktrip(fax_database *db,char *country,char *userid,char *emailid);
int simstatus(fax_database *db);
int booking_alert(fax_database *db,const char *userid,const char *simno,const char *bdate, char *country);
int creditcard_alert(fax_database *db,const char *userid,const char *simno,const char *bdate);
int advancebooking_alert(fax_database *db,const char *userid,const char *country,const char *tsdate,const char *tedate,const char *tddate);
int savedetails(fax_database *db);
int mail_ddno(fax_database *db);
int addcredit_sim(fax_database *db,const char *user, const char *domain, const char *max_red, char *order_no, const char *f_mon,const char *total_amt, const char *cr_to_add,const char *tax, const char *tripid, const char *narration, const char *chqno, const char *chqdate,const char *bname,const char *curr,const char *rate,char *stype,char *tid, char *sim_phone_number);
//int addcredit_sim(const char *user, const char *domain, const char *max_red, const char *f_mon, const char *total_amt,const char *cr_to_add,const char *tax, const char *tripid, const char *narration, const char *curr,const char *rate, char *stype);
char *sim_phone_number;
int reg_process_fail(char *order_no,char msg[1024]);
int app_user_register_fail(char *email, char msg[1024]);
int send_mail(char *toaddr,char *msg,char *subj, int savetosentfolder=0);
int create_sms_file(char *msg, char *dest);
int send_sms(char *msg, char *dest);
char *urlencode(char *msg);
char *hide_cc_number(char *, char *);
char *change_date_format(char *old_frmt,char *new_frmt);
int draw_global_upgrade_report_form();
int draw_global_upg_rep();
int draw_global_ref_report_form();
//int draw_global_ref_rep();
char* payment_link(fax_database *db,char* order_no,char* sku,char* link_amt,char* item_cost,char* subtotal,char* sim_phone_no,char* username);
int register_tsim_client(char* role,char* serial_no,char *sku,char *c_name,char *c_phone,char *company,char *email,char *order_type,char *username,char *domainname,char *c_add1,char *c_add2,char *c_add_city,char *c_add_state,char *c_add_country,char *c_pincode,char *shipping_methods,char *dd_add1,char *dd_add2,char *dd_city,char *dd_state,char *dd_country,char *dd_pincode,char *c_start_date,char *c_activation_date,char *c_payment_mode,char *order_no,char *status,char *total,char *sim_phone_no,char *c_case_sourse,char *sk,char *payment_id,char *cr_to_add,char *total_tax,char *max_from_date,char *max_to_date,char *country_specific,char *c_duration,char *c_stdcode,char *gift_item,char *delivery_method,fax_database *db,char *l_cname,char *l_cphone,char* is_kyc_required,char* new_username);
void draw_page(string path, int section_no);	//Include a page and draw it
string warning(string message);
string feedback(string mesasge, string status);
void draw_warning_page(int section_no, string message = "Not authorised to access this section");
string file_download_headers(string filename);

/* Database class for exposing PgDatabase object in fax_database */
class sim_database : public fax_database
{
	public:
	sim_database(char *db, char *host) : fax_database(db, host) {}
	PgDatabase* getPgDb(){ return p_db; }
};

class queryResult
{
	string query;
	sim_database *dbr;
	int totalRows, totalCols;
	public:	
	queryResult(string queryString);
	string toJSON(void);
	string toCSV(void);
};

//Reports
void client_report(unsigned short int format);

//for retrieving table metadata
struct column_info
{
	string name;
	string data_type;
};
struct column_info* get_column_info(fax_database *db,string table_name, int &count);

//functions for retrieving sim info
string getApiCode(fax_database *db,string phone_no);
string getSimNo(fax_database *db,string phone_no);

//Backend update
bool backend_update_status(fax_database *db,string phone_no, bool active);
bool backend_replace_sim(fax_database *db,string lost_iccid, string new_iccid);

//Functions for sim stock editor
void send_countries(fax_database *db);
void sim_search(fax_database *db);
bool valid_search_data(void);
void update_sim_stock(fax_database *db);
void sim_save_changes(fax_database *db);
bool valid_sim_data(fax_database *db,string &error_msg);
bool valid_country(fax_database *db,string country);
string* get_countries(fax_database *db,int &count);

typedef string (*tuple_export)(fax_database *db, int tuple_inddex, column_info *cols, int column_count);
string tuple_to_html(fax_database *db, int tuple_index, column_info *cols, int column_count);
string tuple_to_csv(fax_database *db, int tuple_index, column_info *cols, int column_count);
bool is_number(const std::string& s);	//Check if a string is numeric
vector<string> splitString(string sequence, char delimiter);
string read_tag_attribute(string tag_name, string attribute_name);

//Functions for stock_summary
void query_sim_stock(fax_database *db,tuple_export);
void stock_download(void);

//Sim Replace
void replace_sim(fax_database *db);

//Misc
char* to_c_string(string cpp_string);		//string obj to null terminated string
inline const string boolToString(bool b);	//bool to string
string runcmd(string command);			//run shell cmd and return output

//Case source crm options
void draw_crm_case_source_options(void);


int do_log(char *log)
{
    FILE *fp = fopen("/var/log/ownmail/tsim_integration.log", "a");

    if (fp) {
        time_t t = time(NULL);
        struct tm *c_date = localtime(&t);
        char *t_now = strdup(asctime(c_date));
        t_now[strlen(t_now) - 1] = 0;
        fprintf(fp, "[Timestamp: %s  MIS_USER: %s PID: %d ]: %s\n", t_now, mis_user, getpid(), log);
        fclose(fp);
        return 1;
    }
    return -1;
}

void log_s(string s){do_log(to_c_string(s));}
   



char* get_simmis_data_from_table(char* key, char *dfault)
{
fax_database db("e2fax");
char query[1024];
sprintf(query,"select value from config_values where name='simmis' and key='%s'",key);
do_log(query);

        if (db.ExecTuplesOk(query))
        {
                return db.get_field(0,"value");
        }
        else
        {
                return dfault;
        }
}


/*
char* escapeForSQL(const char* input) {                                 
      if (input == nullptr) return nullptr;                               
                                                                                      
      // Calculate the required size for the escaped string                           
      size_t input_len = std::strlen(input);                                          
      size_t escaped_len = 0;                                                         
                                                                                      
      // First pass to calculate the length of the escaped string
      for (size_t i = 0; i < input_len; ++i) {                            
          if (input[i] == '\'' || input[i] == '\\' || input[i] == '"') {  
              escaped_len += 2;  // Two characters for each escape sequence
          } else {                                                        
              escaped_len += 1;  // Regular character                     
          }                                                               
      }                                                           
  
      // Allocate memory for the escaped string           
      char* escaped = new char[escaped_len + 1];  // +1 for null terminator
      size_t j = 0;
                                                  
      // Second pass to fill in the escaped string
      for (size_t i = 0; i < input_len; ++i) {    
          if (input[i] == '\'') {                         
              escaped[j++] = '\'';  // Add the first quote
              escaped[j++] = '\'';  // Add the second quote for escaping
          } else if (input[i] == '\\') {                  
              escaped[j++] = '\\';  // Add the backslash  
              escaped[j++] = '\\';  // Add the second backslash for escaping
          } else if (input[i] == '"') {
              escaped[j++] = '\\';  // Escape the double quote
              escaped[j++] = '"';   // Add the escaped double quote
          }
	  else if (input[i] == ',') {
            escaped[j++] = '\\';  // Add the backslash before the comma
            escaped[j++] = ',';   // Add the comma
	  }
	    else {
              escaped[j++] = input[i];  // Regular character
          }
      }
  
      escaped[j] = '\0';  // Null-terminate the escaped string
  
      return escaped;  // Caller is responsible for deleting the allocated memory
  }
*/
char* escapeForSQL(const char* input) {
    if (input == nullptr) {
        do_log("in escape function input was null");
        return nullptr;
    }

    do_log("Inside escapeForSQL Function");

    // Calculate the required size for the escaped string
    size_t input_len = std::strlen(input);
    size_t escaped_len = 0;
    do_log("This is the first print above");

    // First pass to calculate the length of the escaped string
    for (size_t i = 0; i < input_len; ++i) {
        if (input[i] == '\'' || input[i] == '\\' || input[i] == '"' || input[i] == ',') {
            escaped_len += 2; // Two characters for each escape sequence
        } else {
            escaped_len += 1; // Regular character
        }
    }
    do_log("completed loop");

    // Allocate memory for the escaped string
    char* escaped = new char[escaped_len + 1]; // +1 for null terminator
    size_t j = 0;
    do_log("before second pass");

    // Second pass to fill in the escaped string
    for (size_t i = 0; i < input_len; ++i) {
        char log_buffer[256];
        sprintf(log_buffer, "Processing input[%zu]: '%c', j: %zu", i, input[i], j);
        //do_log(log_buffer);

        if (input[i] == '\'') {
            escaped[j++] = '\''; // Add the first quote
            escaped[j++] = '\''; // Add the second quote for escaping
        } else if (input[i] == '\\') {
            escaped[j++] = '\\'; // Add the backslash
            escaped[j++] = '\\'; // Add the second backslash for escaping
        } else if (input[i] == '"') {
            escaped[j++] = '\\'; // Escape the double quote
            escaped[j++] = '"';  // Add the escaped double quote
        } else if (input[i] == ',') {
            escaped[j++] = '\\'; // Add the backslash before the comma
            escaped[j++] = ',';  // Add the comma
        } else {
            escaped[j++] = input[i]; // Regular character
        }

        sprintf(log_buffer, "After processing input[%zu], j: %zu", i, j);
        //do_log(log_buffer);
    }

    do_log("completed second pass");

    char log_final_indices[256];
    sprintf(log_final_indices, "Final values: i: %zu, j: %zu, escaped_len: %zu", input_len, j, escaped_len);
    do_log(log_final_indices);

    escaped[j] = '\0'; // Null-terminate the escaped string
    do_log("at the end");
    do_log(escaped);

    return escaped; // Caller is responsible for deleting the allocated memory
}





int main(int argc, char **argv, char **env)
{
    do_log("Starting main function............");
    //return (0);
    if((get_tag_value(CONFIG_DATA_FILE,"<!-- DBSERVER","-- DBSERVER>","dbserv",DBSERV)!=1) || (DBSERV==NULL || DBSERV[0]==0)){
	do_log("Could not open config file.");
	printf("Error occured while opening config file.");
	strcpy(DBSERV,"50.16.226.255");
    }
    if((get_tag_value(CONFIG_DATA_FILE,"<!-- DBSERVER","-- DBSERVER>","dbserv_r",DBSERV_R)!=1) || (DBSERV_R==NULL || DBSERV_R[0]==0)){
	do_log("Could not open config file.");
	printf("Error occured while opening config file.");
	strcpy(DBSERV_R,"localhost");
    }
    int cgi_err;
    char *domain_name;
    char command[1024];
    char *action,*username;
    int i;
    fax_database db(E2FAXDB,DBSERV);
    fax_database del(E2FAXDB,DBSERV);
    fax_database db_r(E2FAXDB,DBSERV_R);
    envelope *envlp;
    envlp=(envelope *)malloc(sizeof(envelope));
    char *f=NULL;
    if ((cgi_err = cgi_init()) != CGIERR_NONE) {
        //printf("Content-type:text/plain\n\n");
	printf("Content-type:text/html\n\n");
        printf("%s", cgi_strerror(cgi_err));
	do_log("\tcgi_error");
        cgi_exit(0);
    }
    char * d_user=cgi_getentrystr("l");
    char * d_pass=cgi_getentrystr("p");

if(argc==1){
	if (!d_user || d_user[0]==0 || !d_pass || d_pass[0]==0){
                do_log("d1");
            domain_name = getenv("ALTERNATE_DOMAIN");
            if (domain_name == NULL || domain_name[0] == 0) {
                        do_log("d2");
                    domain_name = getenv("HTTP_HOST");
            }
                do_log("verify");
                        do_log(domain_name);
                        do_log(f);
                        do_log("envlp");
            if (!verify_passport(envlp, &f, domain_name)) {
                        do_log("d3");
                    exit(0);
            }
                do_log("<verify111>");
            username = envlp->getusername();
                do_log("d4");
            char *tmp = NULL;
    }
    else{
        do_log("d5");
           if(!strcmp(d_user,"admin") && !strcmp(d_pass,"chair")){
                        do_log("d6");
                   char *tmp = NULL;
                   *envlp=envelope("admin","ownmail.com",which_dot("ownmail.com"));
		   username = envlp->getusername();
            }
            else{do_log("d7");
                   printf ("Content-type: text/plain\n\n");
                   printf ("You almost cracked the world's most secure software\n");
                   do_log("Invalid user");
                   cgi_exit(0);
            }
     }
	//strcpy(current_user,username);

}
    char can_view_cc_users[101];
/*    if((get_tag_value(CONFIG_DATA_FILE,"<!-- CAN_VIEW_CC","-- CAN_VIEW_CC>","users",can_view_cc_users)!=1) || (can_view_cc_users==NULL || can_view_cc_users[0]==0)){
    	strcpy(can_view_cc_users,"vinanti");
    }*/


    char* data0 = get_simmis_data_from_table("CAN_VIEW_CC_users","vinanti");
    strcpy(can_view_cc_users,data0);


	



    strcpy(mis_user,username);   
    if (strstr(can_view_cc_users,mis_user))
    	can_view_cc=1;
    else
    	can_view_cc=0;
    admininfo_long *my_inf = envlp->get_admin_info();
    if (!(my_inf->email & (USER_ADD|USER_MODIFY|USER_DELETE)) && strcmp(mis_user, "admin")) {

	do_log("normal user");
	isAdmin = 0;
    }
    else{
	do_log("user with admin rights");
	isAdmin = 1;
    }

//Access rights flags

	//FILTER_MODIFY => return_date_restrict = false
	//FILTER_DELETE => can_change_stock = true
	//FILTER_ADD => canchange_sales = true

	canchange_sales = (my_inf->email & (FILTER_ADD)) ? 1 : 0;
	can_change_stock = (my_inf->email & (FILTER_DELETE) || isAdmin) ? true : false;
	return_date_restrict = (my_inf->email & (FILTER_MODIFY)) ? false : true;
	canUpgrade = (isAdmin || canchange_sales) ? true : false;

	string flags = "FLAGS";
	flags += ": canUpgrade = " + boolToString(canUpgrade);
	flags += ", return_date_restrict = " + boolToString(return_date_restrict);
	flags += ", can_change_stock = " + boolToString(can_change_stock);
	flags += ", canchange_sales = " + boolToString(canchange_sales);
	do_log(to_c_string(flags.data()));
	
	//For stock summary download
	bool download_mode = cgi_getentryint("format") == 1 ? true : false;

	action = cgi_getentrystr("a");
	do_log(action);

    do_log("<main>");
    if (action==NULL || action[0]==0) printf("Content-type:text/html\n\n");
    else if (strcmp(action,"global_upg_rep") && strcmp(action,"global_ref_rep") && !download_mode) printf("Content-type:text/html\n\n");
    do_log("</main>");
    if((action==NULL)||(!strcmp(action,"x")))
    	{
		draw_header(0);	
		printf("<div id=mh>");
		if(argc>1){
                      //  advance_booking(&db);
                }
		printf("</div>");
		draw_end();
	}
	/*******************************tsim_order******************************************************/
	else if(!strcmp(action,"tsim_order"))
	{
		do_log("Order Json Received");
		char *json = cgi_getentrystr("json");
		do_log(json);
	}
	/*************************************chkusrinfo******************************************************/	
	else if(!strcmp(action,"create_only_user"))
	{
		do_log("*********************Inside Insurance User creation**************************************");
                char *email             =       cgi_getentrystr("uemail");
                char *order_type        =       cgi_getentrystr("order_type");
                char *username          =       cgi_getentrystr("username");
                char *domainname        =       cgi_getentrystr("domainname");
                char *company           =       cgi_getentrystr("company");
                char *c_phone           =       cgi_getentrystr("phone");
                char *c_name            =       cgi_getentrystr("c_name");
                char *c_add1            =       cgi_getentrystr("c_add1");
                char *c_add2            =       cgi_getentrystr("c_add2");
                char *c_add_city        =       cgi_getentrystr("c_address_city");
                char *c_add_state       =       cgi_getentrystr("c_address_state");
                char *c_add_country     =       cgi_getentrystr("c_address_country");
                char *c_pincode         =       cgi_getentrystr("postcode");
                char *shipping_methods  =       cgi_getentrystr("delivery_method");
                char *dd_add1           =       cgi_getentrystr("shipping_add1");
                char *dd_add2           =       cgi_getentrystr("shipping_add2");
                char *dd_city           =       cgi_getentrystr("shipping_city");
                char *dd_state          =       cgi_getentrystr("shipping_state");
                char *dd_country        =       cgi_getentrystr("shipping_country");
                char *dd_pincode        =       cgi_getentrystr("shipping_postcode");
		char *c_start_date      =       cgi_getentrystr("start_date");
                char *c_activation_date    =    cgi_getentrystr("activation_date");
                char *c_payment_mode    =       cgi_getentrystr("payment_mode");
                char *order_no          =       cgi_getentrystr("orderid");
                char *status            =       cgi_getentrystr("status");
                char *total             =       cgi_getentrystr("amt");
                char *sim_phone_no      =       cgi_getentrystr("sim_phone_no");
                char *c_case_sourse     =       cgi_getentrystr("case_source");
                char *sku               =       cgi_getentrystr("sku");
                char *sk                =       cgi_getentrystr("sk");
		char *total_tax         =       cgi_getentrystr("total_tax");
		char *gstin   		=  	cgi_getentrystr("gstin");
                char tripid[1024],pwd[1024],tariff[1024],newdomain[201],mailto[1024],pack[1024],msg[1024],subject[1024];
		do_log("Username");
		do_log(username);
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_TARIFF","--SIM_TARIFF>","tariff",tariff)!=1) || (tariff==NULL || tariff[0]==0))
		{
			do_log("could not open config file.");
			do_log("Rollback work");
			db.ExecCommandOk("rollback work");
			sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
			int result = reg_process_fail(order_no,msg);
			return 0;
		}*/

		  char* data1 = get_simmis_data_from_table("SIM_TARIFF","");
                if(data1[0]==0 || data1==NULL)
                {
                        do_log("could not get config data1.");
                        do_log("Rollback work");
                        db.ExecCommandOk("rollback work");
                        sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                        int result = reg_process_fail(order_no,msg);
                        return 0;
                }
                strcpy(tariff,data1);






		do_log(tariff);
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_DOMAIN","--SIM_DOMAIN>","domainname",domainname)!=1) || (domainname==NULL || domainname[0]==0))
		{
			do_log("could not open config file.");
			do_log("Rollback work");
			db.ExecCommandOk("rollback work");
			sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
			int result = reg_process_fail(order_no,msg);
			return 0;
		}*/

		   char* data2 = get_simmis_data_from_table("SIM_DOMAIN_domainname","");
                if(data2[0]==0 || data2==NULL)
                {
                        do_log("could not get config data2.");
                        do_log("Rollback work");
                        db.ExecCommandOk("rollback work");
                        sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                        int result = reg_process_fail(order_no,msg);
                        return 0;

                }
                strcpy(domainname,data2);




		do_log(domainname);
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_DOMAIN","--SIM_DOMAIN>","newdomain",newdomain)!=1) || (newdomain==NULL || newdomain[0]==0))
		{
			do_log("could not open config file.");
			do_log("Rollback work");
			db.ExecCommandOk("rollback work");
			sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
			int result = reg_process_fail(order_no,msg);
			return 0;
		}*/

		  char* data3 = get_simmis_data_from_table("SIM_DOMAIN_newdomain","");
                if(data3[0]==0 || data3==NULL)
                {
                        do_log("could not get config data3.");
                        do_log("Rollback work");
                        db.ExecCommandOk("rollback work");
                        sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                        int result = reg_process_fail(order_no,msg);
                        return 0;

                }
                strcpy(newdomain,data3);






		do_log(newdomain);	
		strcpy(domainname,newdomain);
		srand ( time(NULL) );
		int j=rand() % (9999 - 1000 + 1) + 1000;
		sprintf(pwd,"%d",j);
		do_log(pwd);
		sprintf (command, "select * from users where username ='%s' and domainname='%s'", username, domainname);
		do_log(command);
		if (!db.ExecTuplesOk(command))
		{
			if (!add_voiceuser_new(&db,username,pwd,domainname,tariff,"none",email,0,pwd,c_name,"55561#",0))
			{
				do_log("add voice user failed.rollback");
				sprintf(msg,"Order Booking of Order No  %s is Failed. User can not be created. Do the NeedFul",order_no);
				int result = reg_process_fail(order_no,msg);
				db.ExecCommandOk("rollback work");
				return 0;
			}
			else
			{
				sprintf(command,"insert into sim_user(username,domainname,clientname,companyname,add_building,add_street,add_city,add_state,add_country,add_pinno,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,mobno,emailadd,primary_emailadd,passport_no,passport_city,gstin)values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','X1550000','NA','%s')",username,domainname,c_name,company,c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,c_phone,email,email,gstin);
				do_log(command);
				int cnt=db.ExecCommandOk(command);
				if (cnt>0)
				{
					do_log("user created");
				}
				else
				{
					do_log("failed to create sim_user");
					do_log("rollback work");
					sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
					int result = reg_process_fail(order_no,msg);
					db.ExecCommandOk("rollback work");
					return 0;
				}
			}
		}
		else
		{
			do_log("User already present-update the records");
			sprintf(command,"update sim_user set add_building='%s', add_street='%s', add_city='%s', add_state='%s', add_country='%s', add_pinno='%s', deliver_add_building='%s', deliver_add_street='%s', deliver_add_city='%s', deliver_add_state='%s', deliver_add_country='%s', deliver_add_pinno='%s', active='t', primary_emailadd='%s' where username='%s'",c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,email,username);
			do_log(command);
			if(db.ExecCommandOk(command))
			{
				do_log("existing user's details updated");
			}
			else
			{
				do_log("failed to update the details");
				do_log("rollback work");
				db.ExecCommandOk("rollback work");
				sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
				int result = reg_process_fail(order_no,msg);
				return 0;
			}
					
		}
		if(strstr(c_payment_mode, "PAD")!= NULL)
	        {
        	        do_log("Payment Link to Be created");
			char *link_amt = cgi_getentrystr("link_amt");
			char *subtotal = cgi_getentrystr("subtotal");
			char *productname = cgi_getentrystr("productname");


			subtotal = strdup(subtotal);
			do_log("subtotal");
			do_log(subtotal);
			//char *item_cost= cgi_getentrystr("subtotal");  
			char *item_cost;
			char *total  = cgi_getentrystr("amt");
			link_amt = strdup(link_amt);
			//item_cost = strdup(item_cost);
			total = strdup(total);
			//char *subtotal = cgi_getentrystr("subtotal");  
			do_log("Total Amount");
			do_log(total);
			do_log("Link Amt");
			do_log(link_amt);
			//do_log("item_cost");
			//do_log(item_cost);
			struct timeval tv1;
			float exch_rate;
			//flag_link++;
			agentusername = strdupa("trikon");
			psswd = strdupa("k1n065qnone");
			char *cartid=NULL;
			char *st_per_val=NULL;
			char *surcharge_val=NULL;
			char *st_curr=NULL;
			float service_tax_percent;
			float surcharge;
			float service_tax;
			char *stax_narr;
			char *sur_chg_narr;
			do_log("Begin work");
			db.ExecCommandOk("begin work");
			sprintf(command,"select cartid from global_sim_cc_payer_det  where order_no = '%s'",order_no);
			do_log(command);
			if(db.ExecTuplesOk(command))
			{
				cartid=db.get_field(0,"cartid");
				do_log(cartid);
				cartid=strdup(cartid);
			}
			else   
			{
				do_log("no result new");
			}
			do_log("cart id");
			do_log(cartid);
			if (cartid == NULL || cartid[0] == 0)
			{
				time_t now;
				time(&now);
				struct tm* now_tm;
				now_tm = localtime(&now);
				char cartid[80];
				strftime (cartid, 80, "%Y%m%d%H%m%s", now_tm);
				do_log(cartid);
				sprintf(command,"select rate from exchange_rate order by date desc limit 1");
				do_log(command);
				int mm = db.ExecTuplesOk(command);
				char buffer[1024];
				sprintf (buffer, "cnt is  %d", mm);
				do_log(buffer);
				char *exch;
				if(mm>0)
				{
					exch=db.get_field(0,"rate");
				}
				do_log(exch);
				exch_rate = atof(exch);
				exch_rate = roundf(exch_rate * 100) / 100;
				sprintf(buffer,"Exch_rate is %f",exch_rate);
				do_log(buffer);
				float ftemp = atof(link_amt);
				sprintf(buffer,"Link Amount is %f  Rs.",ftemp);
				do_log(buffer);
				do_log("*********************************Testing GST Calculations*****************************************");
				sprintf(command,"SELECT * FROM billing_config where code='100' and datefrom<=now() and dateto>=now()");
				do_log(command);
				if(db.ExecTuplesOk(command)>0){
					st_per_val=db.get_field(0, "value");
					stax_narr = db.get_field(0, "descr");
					if (stax_narr == NULL || stax_narr[0] == 0) stax_narr = strdupa("Service Tax");
					else stax_narr = strdupa(stax_narr);
				}
				else{
					st_per_val=strdup("14");
					stax_narr = strdupa("Service Tax");
				}
				service_tax_percent = atof(st_per_val);
				sprintf(command,"service_tax_percent is %f",service_tax_percent);
				do_log(command);
				sprintf(command,"SELECT * FROM billing_config where code='101' and datefrom<=now() and dateto>=now();");
				do_log(command);
				if (db.ExecTuplesOk(command)>0){
					surcharge_val=db.get_field(0, "value");
					sur_chg_narr = db.get_field(0, "descr");
					if (sur_chg_narr == NULL || sur_chg_narr[0] == 0) sur_chg_narr = strdupa("Education Cess and surcharge");
					else sur_chg_narr = strdupa(sur_chg_narr);
				}
				else{
					surcharge_val=strdup("0");
					sur_chg_narr = strdupa("Education Cess and surcharge");
				}
				surcharge = atof(surcharge_val);
				sprintf(command,"Surcharge to be added to the tax is %f",surcharge);
				do_log(command);
				service_tax_percent = service_tax_percent + ((surcharge / 100) * service_tax_percent);
				sprintf(command,"Total Tax to be levied is %f",service_tax_percent);
				do_log(command);
				float val_in_cal = 100 + service_tax_percent;
				sprintf(command,"Value to be taken in calculation is %f",val_in_cal);
				do_log(command);
				sprintf(command,"Link Amount is %f  Rs.",ftemp);
				do_log(command);
/*				float link_amount = (ftemp/val_in_cal)*100/exch_rate;
				sprintf(command,"Link Amount is %f  US$",link_amount);
				do_log(command);
				float link_amount_dollor = (link_amount * 100) / 100;
				sprintf(command,"Link Amount is %f  US$",link_amount_dollor);
				do_log(command);
				char buff[100];
				sprintf(buff,"%.3f",link_amount_dollor);
				do_log(buff);*/

				do_log("******************************Testing ends here***************************************************");
//				sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no,username,domainname) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%s','pending','%s',1,mobno,'%s','%s','tsim.in' from sim_user where username ='%s');","Insurance",ftemp,cartid,order_no,username,username);
				 sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no,username,domainname) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%s','pending','%s',1,mobno,'%s','%s','%s' from sim_user where username ='%s');","Insurance",subtotal,cartid,order_no,username,domainname,username);

				do_log(command);
				int mk = db.ExecCommandOk(command);
				sprintf (buffer, "mk is  %d", mk);
				do_log(buffer);
				if(mk>0)
				{
					do_log("link created successfully");
				}
				else
				{
					do_log("Could Not Create Payment Link. Do the Needful");
					do_log("Rollback work");
					sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s \nOrder_no %s Amount %s \n",order_no,username,link_amt);
					db.ExecCommandOk("rollback work");
					int result = reg_process_fail(order_no,msg);
					return 0;
					/***************************************No Longer Needed**********************/
				}
				do_log("Client Mail for Payment Link");
				do_log(email);
				char link_mail[1024]="services@tsim.in,";
				//char link_mail[1024]="hoshang@staff.ownmail.com,";
				strcat(link_mail,email);
//				char link[1024]="https://mail.tsim.in/tsim/wor_l.php?c=";
				char link[1024]="https://mail.tsim.in/tsim/slp.php?c=";
				strcat(link, cartid);
				strcat(link, "&a=ti_det_link");
				do_log("link is 1");
				do_log(link);
				sprintf(command,"Hello,<br>Mr/Ms. %s,<br>Credit card payment link for your order %s is %s <br>Kindly do the payment. Wish you a very happy journey.<br><br>Regards,<br>TSIM Team.",c_name,order_no,link);
				do_log(command);
				do_log("commit work");
				db.ExecCommandOk("commit work");
				char subject[1024];
				sprintf(subject,"Client Mail-> Tsim Order-%s is placed.(Payment Link)",order_no);
				//Mark5
				 char* escapedMessage = escapeForSQL(command);

				do_log("The escaped message is :");
				do_log(escapedMessage);
				/*
				sprintf(command,  "INSERT INTO tsim_order_notifications "
						" (username,order_no,email,mobno,notification_type, "
						"subject_content,message_content,status,priority,from_email) "
						"VALUES('%s','%s','%s','%s','{email}', "
						"'{%s}','{%s}','{email_pending}','normal','services@tsim.in');"
						,username,order_no,email,c_phone,subject,escapedMessage);
				do_log(sql_query);
				*/
				sprintf(command,            "INSERT INTO tsim_order_notifications "
                                                            "(username, email, notification_type, subject_content, message_content, status, priority, from_email, mobno) "
                                                            "VALUES ('%s', '%s',  '{%s}', '{%s}', '{%s}', '{%s}', '%s', '%s', '%s')",
                                                            username, email, "email",subject, escapedMessage, "email_pending", "normal", "services@tsim.in", c_phone);


				int query_result = db.ExecCommandOk(command);
				if (query_result > 0)
					do_log("Insertion successfull to insert into tsim_order_notification 1");
				else
				{
					do_log("Failed to insert in tsim_order_notification");
					send_mail("hoshang@staff.ownmail.com",command,subject);
                                        send_mail("dhaval@staff.ownmail.com",command,subject);

//				if (!send_mail(link_mail,command,subject))
//				{
					do_log("Contact No");
					do_log(c_phone);
					do_log("failed to send");
					sprintf(command,"This is testing Hello,\nMr/Ms. %s, has placed order through TSIM.\nOrder No- %s \nClientname- %s \nUsername- %s \nContact no- %s \nEmail id- %s.\nPayment link is not sent.This is testing.This is the query\n %s \nKindly Do the needful.",c_name,order_no,c_name,username,c_phone,email,query);
					do_log(command);
					sprintf(subject,"Client Mail-> Tsim Order-%s is placed.(Payment Link)",order_no);
					send_mail("services@tsim.in",command,subject);
					
//				}
/*				else
				{
					do_log("Mail sent to Client");
					do_log("Contact No");
					sprintf(subject,"Telesales Mail->Tsim Order-%s is placed.(Payment Link)",order_no);
					sprintf(command,"Hello,\n\nMr/Ms. %s,\nCredit card payment link for your order %s is %s \n\nKindly do the payment. Wish you a very happy journey.\n\nRegards,\nTSIM Team.",c_name,order_no,link);
					do_log(command);
					//send_mail("hoshang@staff.ownmail.com",command,subject);
					//send_mail("husain@staff.ownmail.com",command,subject);
				}*/
				}
				delete[] escapedMessage;
			}
		}
		else if(strstr(c_payment_mode, "Credit Card")!= NULL)
		{
			do_log("--------------Payment Done....Don't Create Payment Link.------------------");
			do_log("Payment Method is PayU  or Amazon");
			char stype[25],billno[50];
			char *pp;
			char *st_per_val=NULL;
			char *surcharge_val=NULL;
			char *st_curr=NULL;
			float service_tax_percent;
			float surcharge;
			float service_tax;
			char *stax_narr;
			char *sur_chg_narr;
			do_log("Get Payment Details");
			do_log("Before Subtotal");
			char *subtotal = cgi_getentrystr("subtotal");
			do_log(subtotal);
			do_log("After Subtotal");
			subtotal = strdup(subtotal);
			do_log("subtotal");
			do_log(subtotal);
			char *item_cost;
			char *rate;char *rate1,*type,*sim_phone_number,*chqno,*chqdate,*bname,*tid,*bankname,*narration,*userid,*strdomain;
			char *mode=c_payment_mode;
			int credit;float exch_rate;
			do_log("Payment Mode");
			do_log(mode);
			//do_log("Individual item cost Amount");
			//do_log(item_cost);
			do_log("Total Amount");
			do_log(total);
			do_log("tax");
			do_log(total_tax);
			char *cartid=NULL;	
			//The below line may be giving segfault as item_cost is a NULL pointer and also this variable is not being used here, commented
			//item_cost = strdup(item_cost);
			do_log("Begin work");
			db.ExecCommandOk("begin work");
			sprintf(command,"select * from global_sim_cc_payer_det  where order_no = '%s'",order_no);
			do_log(command);
			if(db.ExecTuplesOk(command))
			{
				do_log("Record for order is present");
			}
			else
			{
				do_log("No Record for this order, Insert the Record");
				sprintf(command,"select rate from exchange_rate order by date desc limit 1");
				do_log(command);
				int mm = db.ExecTuplesOk(command);
				char buffer[1024];
				sprintf (buffer, "cnt is  %d", mm);
				do_log(buffer);
				char *exch;
				if(mm>0)
				{
					exch=db.get_field(0,"rate");
				}
				do_log(exch);
				exch_rate = atof(exch);
				exch_rate = roundf(exch_rate * 100) / 100;
				sprintf(buffer,"Exch_rate is %f",exch_rate);
				do_log(buffer);
				float ftemp = atof(total);
				sprintf(buffer,"Total Amount is  %f  Rs.",ftemp);
				do_log(buffer);
				do_log("******************Amount Calculation Hardcoded**********************");
				float amt_p=(ftemp/118)*100/exch_rate;
				sprintf(buffer,"Total Amount in $ is %f  US$",amt_p);
				do_log(buffer);
				float nnn = (amt_p * 100) / 100;
				char buff[100];
				sprintf(buff,"%.3f",nnn);
				do_log(buff);
				do_log("*********************************Testing GST Calculations for PayU*****************************************");
				sprintf(command,"SELECT * FROM billing_config where code='100' and datefrom<=now() and dateto>=now()");
				do_log(command);
				if(db.ExecTuplesOk(command)>0){
					st_per_val=db.get_field(0, "value");
					stax_narr = db.get_field(0, "descr");
					if (stax_narr == NULL || stax_narr[0] == 0) stax_narr = strdupa("Service Tax");
					else stax_narr = strdupa(stax_narr);
				}
				else{
					st_per_val=strdup("14");
					stax_narr = strdupa("Service Tax");
				}
				service_tax_percent = atof(st_per_val);
				sprintf(command,"service_tax_percent is %f",service_tax_percent);
				do_log(command);
				sprintf(command,"SELECT * FROM billing_config where code='101' and datefrom<=now() and dateto>=now();");
				do_log(command);
				if (db.ExecTuplesOk(command)>0){
					surcharge_val=db.get_field(0, "value");
					sur_chg_narr = db.get_field(0, "descr");
					if (sur_chg_narr == NULL || sur_chg_narr[0] == 0) sur_chg_narr = strdupa("Education Cess and surcharge");
					else sur_chg_narr = strdupa(sur_chg_narr);
				}
				else{
					surcharge_val=strdup("0");
					sur_chg_narr = strdupa("Education Cess and surcharge");
				}
				surcharge = atof(surcharge_val);
				sprintf(command,"Surcharge to be added to the tax is %f",surcharge);
				do_log(command);
				service_tax_percent = service_tax_percent + ((surcharge / 100) * service_tax_percent);
				sprintf(command,"Total Tax to be levied is %f",service_tax_percent);
				do_log(command);
				float val_in_cal = 100 + service_tax_percent;
				sprintf(command,"Value to be taken in calculation is %f",val_in_cal);
				do_log(command);
				sprintf(command,"Total Amount is %f  Rs.",ftemp);
/*				do_log(command);
				float t_amount = (ftemp/val_in_cal)*100/exch_rate;
				sprintf(command,"Total Amount is %f  US$",t_amount);
				do_log(command);
				char buff_test[100];
				sprintf(buff_test,"%.3f",t_amount);
				do_log(buff_test);*/

				do_log("******************************Testing ends here***************************************************");
//				sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%s','pending','%s',1,mobno,'%s' from sim_user where username ='%s');","Insurance",ftemp,order_no,order_no,username);
				
				 sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%s','pending','%s',1,mobno,'%s' from sim_user where username ='%s');","Insurance",subtotal,order_no,order_no,username)
	 ;
				do_log(command);
				int mk = db.ExecCommandOk(command);
				sprintf (buffer, "mk is  %d", mk);
				do_log(buffer);
				if (mk)
				{
					do_log("Record  inserted for this order");
				}
				else
				{
					do_log("Failed to insert.");
					do_log("Rollback work");
					db.ExecCommandOk("rollback work");
				}
			}
		}
		else
		{
			do_log("------------Payment method is other than PAD or PAYu----------------");
		}	
	}
	else if(!strcmp(action, "test"))
	{
		do_log("****************Inside test Curl****************");
		return 1;
	}
	else if(!strcmp(action, "tsim_client"))
        {
                do_log("*********************Inside tsim_client**************************************");
                char *email		=	cgi_getentrystr("uemail");
				char *order_type	=	cgi_getentrystr("order_type");
                char *username 		= 	cgi_getentrystr("username");
                char *domainname 	= 	cgi_getentrystr("domainname");
				char *company		= 	cgi_getentrystr("company");
                char *c_phone		= 	cgi_getentrystr("phone");
                char *c_name 		= 	cgi_getentrystr("c_name");
                char *c_add1 		= 	cgi_getentrystr("c_add1");
                char *c_add2 		= 	cgi_getentrystr("c_add2");
                char *c_add_city 	= 	cgi_getentrystr("c_address_city");
                char *c_add_state 	= 	cgi_getentrystr("c_address_state");
                char *c_add_country 	= 	cgi_getentrystr("c_address_country");
                char *c_pincode 	= 	cgi_getentrystr("postcode");
				char *shipping_methods  =       cgi_getentrystr("delivery_method");
				char *dd_add1           =       cgi_getentrystr("shipping_add1");
                char *dd_add2           =       cgi_getentrystr("shipping_add2");
                char *dd_city           =       cgi_getentrystr("shipping_city");
                char *dd_state       	=       cgi_getentrystr("shipping_state");
                char *dd_country     	=       cgi_getentrystr("shipping_country");
                char *dd_pincode        =       cgi_getentrystr("shipping_postcode");
				char *c_start_date      =       cgi_getentrystr("start_date");
				char *c_activation_date    =    cgi_getentrystr("activation_date");
				char *c_payment_mode    =       cgi_getentrystr("payment_mode");
                char *payment_id        =       cgi_getentrystr("payment_id");
                char *order_no          =       cgi_getentrystr("orderid");
                char *status            =       cgi_getentrystr("status");
                char *total             =       cgi_getentrystr("amt");
                char *sim_phone_no      =       cgi_getentrystr("sim_phone_no");
                char *c_case_sourse     =       cgi_getentrystr("case_source");
                char *sku               =       cgi_getentrystr("sku");
                char *sk                =       cgi_getentrystr("sk");
                char *cr_to_add         =       cgi_getentrystr("item_cost");
                char *total_tax         =       cgi_getentrystr("total_tax");
                char *max_from_date     =       cgi_getentrystr("max_from_date");
                char *max_to_date       =       cgi_getentrystr("max_to_date");
                char *country_specific  =       cgi_getentrystr("country_specific");
                char *c_duration        =       cgi_getentrystr("duration");
                char *c_stdcode         =       cgi_getentrystr("std_code");
                char *gift_item         =       cgi_getentrystr("gift_item");
                char *delivery_method   =       cgi_getentrystr("delivery_method");
                char *l_cname           =       cgi_getentrystr("l_cname");
                char *l_cphone          =       cgi_getentrystr("l_cphone");
                char *role              =       cgi_getentrystr("role");
                char *serial_no         =       cgi_getentrystr("serial_no");
				char *gstin 		= 	cgi_getentrystr("gstin");
				char* is_kyc_required = cgi_getentrystr("is_kyc_required");
				if (is_kyc_required == NULL || strlen(is_kyc_required) == 0) {
					if (strcmp(c_add_country,"India") == 0 || strcmp(c_add_country,"IN") == 0) {
						is_kyc_required = "true";
					} else {
						is_kyc_required = "false";
					}
				}

	/*	
		if(sim_phone_no == NULL || strlen(sim_phone_no) == 0)
		{
			sim_phone_no = serial_no;
		}*/

                char *c_sim_phone_no,*seqno;
                char tripid[1024],pwd[1024],tariff[1024],newdomain[201],mailto[1024],pack[1024],msg[1024],subject[1024];
                int flag_link = 0;
                do_log("Order No");
                do_log(order_no);
                do_log("********Role is***************");
                do_log(role);
                do_log("Email");
                do_log(email);
                do_log("Order_type");
                do_log(order_type);
                do_log("username");
                do_log(username);
		char new_username [124];
                strcpy(new_username,username);
                do_log("New Username is below");
                do_log(new_username);
                do_log("domainname");
                do_log(domainname);
                do_log("company");
                do_log(company);
                do_log("c_phone");
                do_log(c_name);
                do_log("c_add1");
                do_log(c_add1);
                do_log("c_add2");
                do_log(c_add2);
                do_log("c_add_city");
                do_log(c_add_city);
                do_log("c_add_state");
                do_log(c_add_state);
                do_log("c_add_country");
                do_log(c_add_country);
                do_log("c_pincode");
                do_log(c_pincode);
                do_log("shipping_methods");
                do_log(shipping_methods);
                do_log("dd_add1");
                do_log(dd_add1);
                do_log("dd_add2");
                do_log(dd_add2);
                do_log("dd_city");
                do_log(dd_city);
                do_log("dd_state");
                do_log(dd_state);
                do_log("dd_country");
                do_log(dd_country);
                do_log("dd_pincode");
                do_log(dd_pincode);
                do_log("c_start_date");
                do_log(c_start_date);
                do_log("c_payment_mode");
                do_log(c_payment_mode);
                do_log("payment_id");
                do_log(payment_id);
                do_log("order_no");
                do_log(order_no);
                do_log("status");
                do_log(status);
                do_log("total");
                do_log(total);
                do_log("sim_phone_no");
                do_log(sim_phone_no);
                do_log("c_case_sourse");
                do_log(c_case_sourse);
                do_log("sku");
                do_log(sku);
                do_log("sk");
                do_log(sk);
                do_log("cr_to_add");
                do_log(cr_to_add);
                do_log("total_tax");
                do_log(total_tax);
                do_log("max_from_date");
                do_log(max_from_date);
                do_log("max_to_date");
                do_log(max_to_date);
                do_log("c_duration");
                do_log(c_duration);
                do_log("c_stdcode");
                do_log(c_stdcode);
                do_log("serial_no");
                do_log(serial_no);
		do_log("gstin");
		do_log(gstin);
		do_log("is kyc required field: ");
		do_log(is_kyc_required);
		do_log("-------company Name---------");
		do_log(company);	
		do_log("sku");
		do_log(sku);
		do_log("number");
		do_log(c_phone);
		do_log("Booking Date");
		do_log(c_start_date);
		do_log("Shipping Method");
		do_log(shipping_methods);
		do_log("Total Amount");
		do_log(total);

		if(strstr(order_type, "New")!= NULL)
		{
			do_log("New Order Request Arrived");
			//sprintf(command,"select * from sim_user where username = '%s' and emailadd='%s'",username,email);
			//sprintf(command,"select * from sim_user where username = '%s' and domainname='tsim.in'",username);
			sprintf(command,"select * from sim_user where username = '%s' and domainname='%s'",username,domainname);
        	        do_log(command);
                	int count = db.ExecTuplesOk(command);
			sprintf(command,"Count is %d",count);
			do_log(command);
               		if (count>0)
			{
				do_log("record found");
		                char *mis_username= db.get_field(0,"username");
        		        //do_log(mis_username);
				do_log("Order Number already Exist");
				do_log("Begin work");
				db.ExecCommandOk("begin work");
				do_log("update the User details");
				sprintf(command,"update sim_user set deliver_add_building='%s', deliver_add_street='%s', deliver_add_city='%s', deliver_add_state='%s', deliver_add_country='%s', deliver_add_pinno='%s', active='t', primary_emailadd='%s' where username='%s'",dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,email,mis_username);
				do_log(command);
				if(db.ExecCommandOk(command))
				{
					do_log("existing user's details updated");
					do_log("***************Calling Function*********");
					//int res = register_tsim_client(role,serial_no,sku,c_name,c_phone,company,email,order_type,username,domainname,c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,shipping_methods,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,c_start_date,c_activation_date,c_payment_mode,order_no,status,total,sim_phone_no,c_case_sourse,sk,payment_id,total,total_tax,max_from_date,max_to_date,country_specific,c_duration,c_stdcode,gift_item,delivery_method,&db,l_cname,l_cphone);
					//changed the total to cr_to_add
					int res = register_tsim_client(role,serial_no,sku,c_name,c_phone,company,email,order_type,username,domainname,c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,shipping_methods,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,c_start_date,c_activation_date,c_payment_mode,order_no,status,total,sim_phone_no,c_case_sourse,sk,payment_id,cr_to_add,total_tax,max_from_date,max_to_date,country_specific,c_duration,c_stdcode,gift_item,delivery_method,&db,l_cname,l_cphone,is_kyc_required,new_username);
					sprintf(command,"register_tsim_client returned %d",res);
					do_log(command);
					return res;
				}
				else
				{
					do_log("failed to update the details");
					do_log("rollback work");
					db.ExecCommandOk("rollback work");
					sprintf(msg,"Order Booking of Order No  %s is Failed. Existing user's details counld not be updated. Do the NeedFul",order_no);
					int result = reg_process_fail(order_no,msg);
					return 0;
				}
			}
                	else
                	{
                        	/***********************************calling add_voice_user function************/
                        	do_log("Record not found");
				do_log("create new sim_user");
				char *old_mobno=NULL;
              		        char *oldclientname=NULL;
				char oldusername[151];
				char *nextval;
				bool is_dd_enabled = 0;
				do_log("Begin work");
				db.ExecCommandOk("begin work");
				do_log("Username");
				do_log(username);
				char new_username [124];
				strcpy(new_username,username);	
				do_log("New Username is below");	
				do_log(new_username);

				/*
				 * Commented by Husain on 7th March2025 no Longer needed. 
				char* data4 = get_simmis_data_from_table("SIM_TARIFF","");
				if(data4[0]==0 || data4==NULL)
				{
						do_log("could not get config data4.");
						do_log("Rollback work");
						db.ExecCommandOk("rollback work");
						sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
						int result = reg_process_fail(order_no,msg);
						return 0;
				}
				strcpy(tariff,data4);






                		_log(tariff);


				  	char* data5 = get_simmis_data_from_table("SIM_DOMAIN_domainname","");
					if(data5[0]==0 || data5==NULL)
					{
							do_log("could not get config data5.");
							do_log("Rollback work");
							db.ExecCommandOk("rollback work");
							sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
							int result = reg_process_fail(order_no,msg);
							return 0;

					}
					strcpy(domainname,data5);
                	        do_log(domainname);



				 	char* data6 = get_simmis_data_from_table("SIM_DOMAIN_newdomain","");
                                if(data6[0]==0 || data6==NULL)
                                {
                                         do_log("could not get config data6.");
                                         do_log("Rollback work");
                                         db.ExecCommandOk("rollback work");
                                         sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                                         int result = reg_process_fail(order_no,msg);
                                         return 0;

                                }
                                strcpy(newdomain,data6);





			       do_log(newdomain);
				*/


				/*******************************************************************************************/
			       do_log("-----------------new user id---------------------");	
			       //strcpy(domainname,newdomain);
			       srand ( time(NULL) );
			       int j=rand() % (9999 - 1000 + 1) + 1000;
			       sprintf(pwd,"%d",j);
			       do_log(pwd);
			       char command[1024]; 
			       sprintf (command, "/*NO LOAD BALANCE*/select * from users where username ='%s' and domainname='%s'", username, domainname);
			       do_log(command);
			       if (!db.ExecTuplesOk(command))
			       {	
				       if (!add_voiceuser_new(&db,username,pwd,domainname,tariff,"none",email,0,pwd,c_name,"55561#",0))
				       {
					       do_log("add voice user failed.rollback");
					       sprintf(msg,"Order Booking of Order No  %s is Failed. User can not be created. Do the NeedFul",order_no);
					       int result = reg_process_fail(order_no,msg);
					       db.ExecCommandOk("rollback work");
					       return 0;
				       }
				       else
				       {
						do_log("Below are the Username");
						do_log(username);
						do_log(new_username);
					       sprintf(command,"insert into sim_user(username,domainname,clientname,companyname,add_building,add_street,add_city,add_state,add_country,add_pinno,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,mobno,emailadd,primary_emailadd,passport_no,passport_city,gstin)values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','X1550000','NA','%s')",new_username,domainname,c_name,company,c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,c_phone,email,email,gstin);
					       do_log(command);
					       int cnt=db.ExecCommandOk(command);
					       if (cnt>0)
					       {						
						       do_log("new user created");
						       do_log("*******************Calling Function***************");              
						       do_log("Phone No is");
						       do_log(c_phone); 
						       int res = register_tsim_client(role,serial_no,sku,c_name,c_phone,company,email,order_type,username,domainname,c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,shipping_methods,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,c_start_date,c_activation_date,c_payment_mode,order_no,status,total,sim_phone_no,c_case_sourse,sk,payment_id,total,total_tax,max_from_date,max_to_date,country_specific,c_duration,c_stdcode,gift_item,delivery_method,&db,l_cname,l_cphone,is_kyc_required,new_username);
						       sprintf(command,"register_tsim_client returned %d",res);
						       do_log(command);
						       return res;
						       /****************************Adding credit on new user creation**************************************
							 char app_cred[2];
							 double dbl_app_cred = 0;
							 if((get_tag_value(CONFIG_DATA_FILE,"<!-- APP_PROMO","-- APP_PROMO>","on_order",app_cred)!=1) || (app_cred==NULL || app_cred[0]==0))										     								{
							 strcpy(app_cred,"20");
							 }
							 dbl_app_cred = atof(app_cred);
							 char narr[21];
							 sprintf(narr,"Promo Credit for App");
							 char senderr[21];
							 sprintf(senderr,"Promo Credit");
							 int res = addCredit(username,domainname,narr,dbl_app_cred,senderr,"USD",1,1);
							**********************************************************************************************************/
					       }	
					       else
					       {
						       do_log("failed to create sim_user");
						       do_log("rollback work");
						       sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
						       int result = reg_process_fail(order_no,msg);
						       db.ExecCommandOk("rollback work");
						       return 0; 
					       }
				       }
				}
			}
		}	
		else if(strstr(order_type, "Refill") != NULL)
		{
			do_log("Request for Refill Placed");
			do_log(order_no);
			do_log(total);
			do_log(sim_phone_no);
			//do_log(note);
			sprintf(command,"SELECT * from tsim_simmis_map where order_no='%s'",order_no);
                        do_log(command);
                        if(!db.ExecTuplesOk(command))
                        {
                                 sprintf(command,"insert into tsim_simmis_map (tsim_username,email,contact,order_no,status) values ('%s','%s','%s','%s','%s')",username,email,c_phone,order_no,status);
                                 do_log(command);
                                 int count=db.ExecCommandOk(command);
                                 if(count>0)
                                 {
					do_log("Sending Mail");
					sprintf(command,"Hello,\nA refill order  %s  of Rs.%s  been placed.\nKindly do the needful.\n\n\nRegards,\nTSIM Team.",order_no,total);
                	        	do_log(command);
					char mailto[1024];
					
/*			                if((get_tag_value(CONFIG_DATA_FILE,"<!-- TSIM_ORDER_REFILL","-- TSIM_ORDER_REFILL>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0))
			        	{
       					        do_log("Could not open config file.");
                                        	sprintf(msg,"Refill Order No  %s is Failed. Do the NeedFul",order_no);
                                                int result = reg_process_fail(order_no,msg);
						return 0;
			 		}
		        		else
					{
						sprintf(subject,"TSIM refill order %s placed",order_no);
				               if (!send_mail(mailto,command,subject))
						{
				                       do_log("failed to send");
	                     			}
		                        	else    do_log("sent");
							//send_mail("hoshang@staff.ownmail.com",command,subject);
					}*/

					  char* data7 = get_simmis_data_from_table("TSIM_ORDER_REFILL_mailto","");
                                        if(data7[0]==0 || data7==NULL)
                                        {
                                                do_log("Could not open config file data7.");
                                                sprintf(msg,"Refill Order No  %s is Failed. Do the NeedFul",order_no);
                                                int result = reg_process_fail(order_no,msg);
                                                return 0;

                                        }
					//TSIM mail
                                        strcpy(mailto,data7);
                                         sprintf(subject,"TSIM refill order %s placed",order_no);
                                               if (!send_mail(mailto,command,subject))
                                                {
                                                       do_log("failed to send");
                                                }
                                                else    do_log("sent");
                                                        //send_mail("hoshang@staff.ownmail.com",command,subject);









					sprintf(subject,"TSIM refill order %s placed",order_no);
				        //send_mail("hoshang@staff.ownmail.com",command,subject);	
				        send_mail("husain@staff.ownmail.com",command,subject);	
					return 1;		
		
                                 }
                                 else
                                 {
                                            do_log("failed to insert into tsim_mis_map");
                                            do_log("rollback work");
					    db.ExecCommandOk("rollback work");	
                                            sprintf(msg,"Order %s booking failed. Do the NeedFul",order_no);
                                            int result = reg_process_fail(order_no,msg);
					    return 0;
                                 }
			}
			else
			{
					do_log("Order is in Tsim_mis_map. Send Mail");
					sprintf(subject,"TSIM refill order %s placed",order_no);
					sprintf(command,"Hello,\nA refill order  %s  of Rs.%s  been placed.\nKindly do the needful.\n\nRegards,\nTSIM Team.",order_no,total);
				        //send_mail("hoshang@staff.ownmail.com",command,subject);	
				        send_mail("husain@staff.ownmail.com",command,subject);	
					send_mail("services@tsim.in",command,subject);
			}
		}
		else
		{
			do_log("No order Placed");
			return 0;
		}
	}//Action closed
	else if(!strcmp(action, "reseller_customer"))
	{
		do_log("*****************Inside Reseller Customer Action*********************");
		char *email             =       cgi_getentrystr("uemail");
                char *order_type        =       cgi_getentrystr("order_type");
                char *username          =       cgi_getentrystr("username");
                char *domainname        =       cgi_getentrystr("domainname");
                char *company           =       cgi_getentrystr("company");
                char *c_phone           =       cgi_getentrystr("phone");
                char *c_name            =       cgi_getentrystr("c_name");
                char *c_add1            =       cgi_getentrystr("c_add1");
                char *c_add2            =       cgi_getentrystr("c_add2");
                char *c_add_city        =       cgi_getentrystr("c_address_city");
                char *c_add_state       =       cgi_getentrystr("c_address_state");
                char *c_add_country     =       cgi_getentrystr("c_address_country");
                char *c_pincode         =       cgi_getentrystr("postcode");
                char *shipping_methods  =       cgi_getentrystr("delivery_method");
                char *dd_add1           =       cgi_getentrystr("shipping_add1");
                char *dd_add2           =       cgi_getentrystr("shipping_add2");
                char *dd_city           =       cgi_getentrystr("shipping_city");
                char *dd_state          =       cgi_getentrystr("shipping_state");
                char *dd_country        =       cgi_getentrystr("shipping_country");
                char *dd_pincode        =       cgi_getentrystr("shipping_postcode");
		char *c_start_date      =       cgi_getentrystr("start_date");
		char *c_activation_date =	cgi_getentrystr("activation_date"); 
                char *c_payment_mode    =       cgi_getentrystr("payment_mode");
                char *payment_id        =       cgi_getentrystr("payment_id");
                char *order_no          =       cgi_getentrystr("orderid");
                char *status            =       cgi_getentrystr("status");
                char *total             =       cgi_getentrystr("amt");
                char *sim_phone_no      =       cgi_getentrystr("sim_phone_no");
                char *c_case_sourse     =       cgi_getentrystr("case_source");
                char *sku               =       cgi_getentrystr("sku");
                char *sk                =       cgi_getentrystr("sk");
		char *cr_to_add         =       cgi_getentrystr("item_cost");
		char *total_tax         =       cgi_getentrystr("total_tax");
		char *max_from_date     =       cgi_getentrystr("max_from_date");
		char *max_to_date       =       cgi_getentrystr("max_to_date");
		char *c_duration        =       cgi_getentrystr("duration");
		char *c_stdcode         =       cgi_getentrystr("std_code");
		char *gift_item         =       cgi_getentrystr("gift_item");
		char *delivery_method   =       cgi_getentrystr("delivery_method");
		char *l_cname           =       cgi_getentrystr("l_cname");
		char *l_cphone          =       cgi_getentrystr("l_cphone");	
		char *serial_no         =       cgi_getentrystr("serial_no");
		char *country_specific  =       cgi_getentrystr("country_specific");
		char* role		= 	cgi_getentrystr("role");
		char* gstin 		=	cgi_getentrystr("gstin");
		char* is_kyc_required = cgi_getentrystr("is_kyc_required");
		if (is_kyc_required == NULL || strlen(is_kyc_required) == 0) {
			if (strcmp(c_add_country,"India") == 0 || strcmp(c_add_country,"IN") == 0 ) {
				is_kyc_required = "true";
			} else {
				is_kyc_required = "false";
			}
		}

		do_log("********Role is***************");
		do_log(role);
		do_log("Email");
		do_log(email);
		do_log("Order_type");
		do_log(order_type);
		do_log("username");
		do_log(username);
 		char new_username [124];
                strcpy(new_username,username);
                do_log("New Username is below");
                do_log(new_username);
		do_log("domainname");
		do_log(domainname);		
		do_log("company");
		do_log(company);	
		do_log("c_phone");             
                do_log(c_name);
                do_log("c_add1");
                do_log(c_add1);
                do_log("c_add2");
                do_log(c_add2);
		do_log("c_add_city");             
                do_log(c_add_city);
                do_log("c_add_state");
                do_log(c_add_state);
                do_log("c_add_country");
                do_log(c_add_country);
		do_log("c_pincode");             
                do_log(c_pincode);
                do_log("shipping_methods");
                do_log(shipping_methods);
                do_log("dd_add1");
                do_log(dd_add1);
		do_log("dd_add2");
		do_log(dd_add2);
		do_log("dd_city");
                do_log(dd_city);
		do_log("dd_state");
                do_log(dd_state);
		do_log("dd_country");
                do_log(dd_country);
		do_log("dd_pincode");
                do_log(dd_pincode);
		do_log("c_start_date");
                do_log(c_start_date);
		do_log("c_payment_mode");
                do_log(c_payment_mode);
		do_log("payment_id");
                do_log(payment_id);
		do_log("order_no");
                do_log(order_no);
		do_log("status");
                do_log(status);
		do_log("total");
                do_log(total);
		do_log("sim_phone_no");
                do_log(sim_phone_no);
		do_log("c_case_sourse");
                do_log(c_case_sourse);
		do_log("sku");
		do_log(sku);
		do_log("sk");     
                do_log(sk);
		do_log("cr_to_add");     
                do_log(cr_to_add);
		do_log("total_tax");     
                do_log(total_tax);
		do_log("max_from_date");     
                do_log(max_from_date);
		do_log("max_to_date");     
                do_log(max_to_date);
		do_log("c_duration");     
                do_log(c_duration);
		do_log("c_stdcode");     
                do_log(c_stdcode);
		do_log("serial_no");     
                do_log(serial_no);	
		do_log("gstin");
		do_log(gstin);
		do_log("is kyc required: ");
		do_log(is_kyc_required);
		char tripid[1024],pwd[1024],tariff[1024],newdomain[201],mailto[1024],pack[1024],msg[1024],subject[1024];	
		//char* link = payment_link(&db,order_no,sku,link_amt,item_cost,subtotal,sim_phone_no,username);
		//char* link = payment_link(&db,order_no,sku,total,cr_to_add,cr_to_add,serial_no,"hoshangkaranjekar1");
		//do_log("Payment link");
		//do_log(link);
		//return 0;
		if(strstr(order_type, "New")!= NULL)
                {

                        do_log("New Order Request Arrived");
			//sprintf(command,"select * from sim_user where username = '%s' and domainname='tsim.in'",username);
			sprintf(command,"select * from sim_user where username = '%s' and domainname='%s'",username,domainname);
                        do_log(command);
                        int count = db.ExecTuplesOk(command);
                        sprintf(command,"Count is %d",count);
                        do_log(command);
                        if (count>0)
                        {
                                do_log("record found");
				char *mis_username= db.get_field(0,"username");
				do_log("Begin work");
				db.ExecCommandOk("begin work");
				do_log("update the User details");
				sprintf(command,"update sim_user set deliver_add_building='%s', deliver_add_street='%s', deliver_add_city='%s', deliver_add_state='%s', deliver_add_country='%s', deliver_add_pinno='%s', active='t', primary_emailadd='%s' where username='%s'",dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,email,mis_username);
				do_log(command);
				if(db.ExecCommandOk(command))
				{
					do_log("existing user's details updated");
					do_log("***************Calling Function*********");
					int res = register_tsim_client(role,serial_no,sku,c_name,c_phone,company,email,order_type,username,domainname,c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,shipping_methods,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,c_start_date,c_activation_date,c_payment_mode,order_no,status,total,sim_phone_no,c_case_sourse,sk,payment_id,cr_to_add,total_tax,max_from_date,max_to_date,country_specific,c_duration,c_stdcode,gift_item,delivery_method,&db,l_cname,l_cphone,is_kyc_required,new_username);
					sprintf(command,"register_tsim_client returned %d",res);
					do_log(command);
					return res;
					
				}
				else
				{
					do_log("failed to update the details");
					do_log("rollback work");
					db.ExecCommandOk("rollback work");
					sprintf(msg,"Order Booking of Order No  %s is Failed. Existing user's details counld not be updated. Do the NeedFul",order_no);
					int result = reg_process_fail(order_no,msg);
					return 0;
				}
                        }
                        else
                        {
                                /***********************************calling add_voice_user function************/
				do_log("Record not found");
                                do_log("create new sim_user");
                                char *old_mobno=NULL;
                                char *oldclientname=NULL;
                                char oldusername[151];
                                char *userid = "new";
                                char *nextval;
                                bool is_dd_enabled = 0;
                                do_log("Begin work");
                                db.ExecCommandOk("begin work");
                                do_log("Username");
                                do_log(username);
				  char new_username [124];
                                strcpy(new_username,username);
                                do_log("New Username is below");
                                do_log(new_username);

/*                                if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_TARIFF","--SIM_TARIFF>","tariff",tariff)!=1) || (tariff==NULL || tariff[0]==0))
                                {
                                        do_log("could not open config file.");
                                        do_log("Rollback work");
                                        db.ExecCommandOk("rollback work");
                                        sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                                        int result = reg_process_fail(order_no,msg);
                                        return 0;
                                }*/

				     char* data8 = get_simmis_data_from_table("SIM_TARIFF","");
                                 if(data8[0]==0 || data8==NULL)
                                 {
                                          do_log("could not get config data8.");
                                          do_log("Rollback work");
                                          db.ExecCommandOk("rollback work");
                                          sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                                          int result = reg_process_fail(order_no,msg);
                                          return 0;
                                 }
                                strcpy(tariff,data8);


                                do_log(tariff);
/*                                if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_DOMAIN","--SIM_DOMAIN>","domainname",domainname)!=1) || (domainname==NULL || domainname[0]==0))
                                {
                                        do_log("could not open config file.");
                                        do_log("Rollback work");
                                        db.ExecCommandOk("rollback work");
                                        sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                                        int result = reg_process_fail(order_no,msg);
                                        return 0;
                                }*/

				  char* data9 = get_simmis_data_from_table("SIM_DOMAIN_domainname","");
                                if(data9[0]==0 || data9==NULL)
                                {
                                        do_log("could not get config data9.");
                                        do_log("Rollback work");
                                        db.ExecCommandOk("rollback work");
                                        sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                                        int result = reg_process_fail(order_no,msg);
                                        return 0;

                                }
                                strcpy(domainname,data9);





                                do_log(domainname);
/*				if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_DOMAIN","--SIM_DOMAIN>","newdomain",newdomain)!=1) || (newdomain==NULL || newdomain[0]==0))
                                {
                                        do_log("could not open config file .");
                                        do_log("Rollback work");
                                        db.ExecCommandOk("rollback work");
                                        sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                                        int result = reg_process_fail(order_no,msg);
                                        return 0;
                                }*/

				 char* data10 = get_simmis_data_from_table("SIM_DOMAIN_newdomain","");
                                if(data10[0]==0 || data10==NULL)
                                {
                                         do_log("could not get config data10.");
                                         do_log("Rollback work");
                                         db.ExecCommandOk("rollback work");
                                         sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
                                         int result = reg_process_fail(order_no,msg);
                                         return 0;

                                }
                                strcpy(newdomain,data10);












				do_log(newdomain);
				do_log("-----------------new user id---------------------");
				strcpy(domainname,newdomain);
				srand ( time(NULL) );
				int j=rand() % (9999 - 1000 + 1) + 1000;
				sprintf(pwd,"%d",j);
				do_log(pwd);
				do_log("***************Calling add_voice_user function**************************");
				if (!add_voiceuser_new(&db,username,pwd,domainname,tariff,"none",email,0,pwd,c_name,"55561#",0))
				{
					do_log("add voice user failed.rollback");
					sprintf(msg,"Order Booking of Order No  %s is Failed. User can not be created. Do the NeedFul",order_no);
					int result = reg_process_fail(order_no,msg);
					db.ExecCommandOk("rollback work");
					return 0;
				}
				else
				{
					 do_log("Below are the Username");
                                                do_log(username);
                                                do_log(new_username);

					sprintf(command,"insert into sim_user(username,domainname,clientname,companyname,add_building,add_street,add_city,add_state,add_country,add_pinno,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,mobno,emailadd,primary_emailadd,passport_no,passport_city,gstin)values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','X1550000','NA','%s')",new_username,domainname,c_name,company,c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,c_phone,email,email,gstin);
					do_log(command);
					int cnt=db.ExecCommandOk(command);
					if (cnt>0)
					{
						do_log("new user created");
						do_log("*******************Calling Function***************");
						do_log("Phone No is");
						do_log(c_phone);
						int res = register_tsim_client(role,serial_no,sku,c_name,c_phone,company,email,order_type,username,domainname,c_add1,c_add2,c_add_city,c_add_state,c_add_country,c_pincode,shipping_methods,dd_add1,dd_add2,dd_city,dd_state,dd_country,dd_pincode,c_start_date,c_activation_date,c_payment_mode,order_no,status,total,sim_phone_no,c_case_sourse,sk,payment_id,cr_to_add,total_tax,max_from_date,max_to_date,country_specific,c_duration,c_stdcode,gift_item,delivery_method,&db,l_cname,l_cphone,is_kyc_required,new_username);
						sprintf(command,"register_tsim_client returned %d",res);
						do_log(command);
						return res;
					}
					else
					{
						do_log("failed to create sim_user");
						do_log("rollback work");
						sprintf(msg,"Order Booking of Order No  %s is Failed. Do the NeedFul",order_no);
						int result = reg_process_fail(order_no,msg);
						db.ExecCommandOk("rollback work");
						return 0;
					}
				}
			}
		}
		else if(strstr(order_type, "Refill") != NULL)
                {
                        do_log("Request for Refill Placed");
                        do_log(order_no);
                        do_log(total);
                        do_log(sim_phone_no);
			char mailto[1024];
/*			if((get_tag_value(CONFIG_DATA_FILE,"<!-- TSIM_ORDER_REFILL","-- TSIM_ORDER_REFILL>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0))
			{
				do_log("Could not open config file.");
				sprintf(msg,"Refill Order No  %s is Failed. Do the NeedFul",order_no);
				int result = reg_process_fail(order_no,msg);
				return 0;
			}
			else
			{
				sprintf(subject,"TSIM refill order %s placed",order_no);
				if (!send_mail(mailto,command,subject))
				{
					do_log("failed to send");
				}
				else    do_log("sent");
				//send_mail("hoshang@staff.ownmail.com",command,subject);
			}*/
			
			  char* data11 = get_simmis_data_from_table("TSIM_ORDER_REFILL_mailto","");
                         if(data11[0]==0 || data11==NULL)
                           {
                                   do_log("Could not open config file data11.");
                                   sprintf(msg,"Refill Order No  %s is Failed. Do the NeedFul",order_no);
                                   int result = reg_process_fail(order_no,msg);
                                  return 0;
                           }
                              strcpy(mailto,data11);
                              sprintf(subject,"TSIM refill order %s placed",order_no);
                               if (!send_mail(mailto,command,subject))
                               {
                                             do_log("failed to send");
                               }
                                             else    do_log("sent");
                                     //send_mail("hoshang@staff.ownmail.com",command,subject);





			sprintf(subject,"TSIM refill order %s placed",order_no);
			//send_mail("hoshang@staff.ownmail.com",command,subject);
			send_mail("husain@staff.ownmail.com",command,subject);
			return 1;
                }	
                else
                {
                        do_log("No order Placed");
                        return 0;
                }                
		return 1;	
	}	
	/****************************************************************************************************************************************************/
	else if(!strcmp(action, "app_user"))
	{
		do_log("*********************Only App User creation Action**************************************");
                char *email             =       cgi_getentrystr("email");
                char *username          =       cgi_getentrystr("username");
		char *domainname	= 	cgi_getentrystr("domainname");
		char *clientname	= 	cgi_getentrystr("clientname");
		char *phone		= 	cgi_getentrystr("phone");
		char *pass		=	cgi_getentrystr("pwd");
		email	 	= strdupa(email);
		username 	= strdupa(username);
		domainname	= strdupa(domainname);
		clientname	= strdupa(clientname);
		phone		= strdupa(phone);
		pass		= strdupa(pass);
		sprintf(command,"%s",email);
		do_log(command);
		sprintf(command,"%s",username);
		do_log(command);
		sprintf(command,"%s",domainname);
		do_log(command);
		sprintf(command,"%s",clientname);
		do_log(command);
		sprintf(command,"%s",phone);
		do_log(command);
		sprintf(command,"%s",pass);
		do_log(command);
		char tripid[1024],pwd[1024],tariff[1024],newdomain[201],msg[1024];
		strcpy(pwd,pass);
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_TARIFF","--SIM_TARIFF>","tariff",tariff)!=1) || (tariff==NULL || tariff[0]==0))
		{
			do_log("could not open config file.");
			do_log("Rollback work");
			db.ExecCommandOk("rollback work");
			sprintf(msg,"User creation failed for emailadd  %s . Do the NeedFul",email);
			int result = app_user_register_fail(email, msg);
			return 0;
		}*/
		
		 char* data12 = get_simmis_data_from_table("SIM_TARIFF","");
                if(data12[0]==0 || data12==NULL)
                {
                                          do_log("could not get config data12.");
                                          do_log("Rollback work");
                                          db.ExecCommandOk("rollback work");
                                          sprintf(msg,"User creation failed for emailadd  %s . Do the NeedFul",email);
                                          int result = app_user_register_fail(email, msg);
                                          return 0;
                }
                strcpy(tariff,data12);



		do_log(tariff);
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_DOMAIN","--SIM_DOMAIN>","domainname",domainname)!=1) || (domainname==NULL || domainname[0]==0))
		{
			do_log("could not open config file.");
			do_log("Rollback work");
			db.ExecCommandOk("rollback work");
			sprintf(msg,"User creation failed for emailadd  %s . Do the NeedFul",email);
			int result = app_user_register_fail(email, msg);
			return 0;
		}*/

		    char* data13 = get_simmis_data_from_table("SIM_DOMAIN_domainname","");
               if(data13[0]==0 || data13==NULL)
               {
                                        do_log("could not get config data13.");
                                        do_log("Rollback work");
                                        db.ExecCommandOk("rollback work");
                                        sprintf(msg,"User creation failed for emailadd  %s . Do the NeedFul",email);
                                        int result = app_user_register_fail(email, msg);
                                        return 0;

                }
                strcpy(domainname,data13);







		do_log(domainname);
/*                if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_DOMAIN","--SIM_DOMAIN>","newdomain",newdomain)!=1) || (newdomain==NULL || newdomain[0]==0))
		{
			do_log("could not open config file.");
			do_log("Rollback work");
			db.ExecCommandOk("rollback work");
			sprintf(msg,"User creation failed for emailadd  %s . Do the NeedFul",email);
			int result = app_user_register_fail(email, msg);
			return 0;
		}*/

		  char* data14 = get_simmis_data_from_table("SIM_DOMAIN_newdomain","");
                if(data14[0]==0 || data14==NULL)
                {
                              do_log("could not get config data14.");
                              do_log("Rollback work");
                              db.ExecCommandOk("rollback work");
                              sprintf(msg,"User creation failed for emailadd  %s . Do the NeedFul",email);
                              int result = app_user_register_fail(email, msg);
                              return 0;

                 }
                strcpy(newdomain,data14);


		do_log(newdomain);               
		char currency[1024];
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- APP_CURRENCY","--APP_CURRENCY>","currency",currency)!=1) || (currency==NULL || currency[0]==0))
                {
                        do_log("could not open config file.");
			strcpy(currency,"USD");
                }*/


		char* data15 = get_simmis_data_from_table("APP_CURRENCY_currency","USD");
                strcpy(currency,data15);












		do_log(currency);
		strcpy(domainname,newdomain);
		/*srand ( time(NULL) );
		int j=rand() % (9999 - 1000 + 1) + 1000;
		sprintf(pwd,"%d",j);
		do_log(pwd);*/
		sprintf(command,"begin");
		do_log(command);
		if(!db.ExecCommandOk(command))
		{
			do_log("Failed to begin");
			return 0;	
		}
		sprintf (command, "select * from users where username ='%s' and domainname='%s'", username, domainname);
		do_log(command);
		char command1[1024];
		sprintf(command1,"select * from gkusers where username ='%s' and domainname='%s'", username, domainname);
		do_log(command1);
		if(db.ExecTuplesOk(command))
		{
			do_log("Entry present in the users. Check for gkusers");
                        if (db.ExecTuplesOk(command1))
                        {
				do_log("Entry present in the gkusers");
				do_log("Get the exsting username");	
				username= db.get_field(0,"username");
                        }
			else
			{
				do_log("Entry not present in the gkusers");
				sprintf (command, "INSERT INTO gkusers(username,domainname,password,enable,ext,rewrite_prefix,rewrite,postpay) values ('%s','%s','%s',true,'00','1','55561#',0);",username,domainname,pwd);
		                do_log(command);
                		if (!db.ExecCommandOk(command))
				{
		                        do_log("INSERT failed");
					sprintf(command,"rollback");
					db.ExecCommandOk(command);
					int result = app_user_register_fail(email, msg);
                		        return 0;
                		}
				else
				{	
					sprintf(command,"commit");
					do_log(command);
					if(!db.ExecCommandOk(command))
					{       
						do_log("Failed to commit");
						return 0;      
					}
				}
			}
		}
		else if(db.ExecTuplesOk(command1))
		{
			do_log("Entry not present in the users but present in the gkusers.");
		        sprintf (command, "INSERT INTO users (username,domainname,signed_on,last_update,credit,usage,country_code,currency,preferences) values ('%s','%s','now','now','500',0,'%s','%s',0)",username,domainname,tariff,currency);
                	do_log(command);
			if (!db.ExecCommandOk(command))
			{
                        	do_log("INSERT failed");
				sprintf(command,"rollback");
                                db.ExecCommandOk(command);
	                        int result = app_user_register_fail(email, msg);
				return 0;
        		}
			else
			{
				sprintf(command,"commit");
				do_log(command);
				if(!db.ExecCommandOk(command))
				{
					do_log("Failed to commit");
					return 0;
				}
				do_log("Inserted into users");
				do_log("get the username");
				username = db.get_field(0,"username");	
			}
		}
		else
		{ 
			sprintf(command,"rollback");
			do_log(command);
			if(!db.ExecCommandOk(command))
			{
				do_log("Failed to rollback");
				return 0;
			}
			do_log("No entry in users and gkusers. Call add_voiceuser()");
			if (!add_voiceuser_new(&db,username,pwd,domainname,tariff,"none",email,0,pwd,clientname,"55561#",0))
			{
				do_log("add voice user failed.rollback");
				sprintf(msg,"User creation failed for emailadd  %s . Do the NeedFul",email);
				sprintf(command,"rollback");
                                db.ExecCommandOk(command);
				int result = app_user_register_fail(email, msg);
				return 0;
			}
		}
		/*char app_cred[2];
		double dbl_app_cred = 0;
		if((get_tag_value(CONFIG_DATA_FILE,"<!-- APP_PROMO","-- APP_PROMO>","on_creation",app_cred)!=1) || (app_cred==NULL || app_cred[0]==0)){
			strcpy(app_cred,"3");
		}
		dbl_app_cred = atof(app_cred);
		char narr[21],subject[1024];
		sprintf(narr,"Promo Credit for App");
		char senderr[21];
		sprintf(senderr,"Promo Credit");
		int res = addCredit(username,domainname,narr,dbl_app_cred,senderr,"USD",1,1);
		if(res > 0)
		{
			do_log("Failed to add credit");
			sprintf(command,"rollback");
                        db.ExecCommandOk(command);
			sprintf(msg,"Add credit failed for emailadd  %s . Do the NeedFul",email);
			sprintf(command,"rollback");
			db.ExecCommandOk(command);
			int result = app_user_register_fail(email, msg);
		}
		else
		{
			do_log("Credit Added Successfully");
		}*/
		/********************************************************************************************************************/
		sprintf(command,"select credit-usage as balance from users where username='%s' and domainname='%s'",username,domainname);
		do_log(command);
		char subject[1024];
		int bb = db.ExecTuplesOk(command);
		if(bb == 0)
		{
			char app_cred[2];
			double dbl_app_cred = 0;
/*			if((get_tag_value(CONFIG_DATA_FILE,"<!-- APP_PROMO","-- APP_PROMO>","on_creation",app_cred)!=1) || (app_cred==NULL || app_cred[0]==0)){
				strcpy(app_cred,"3");
			}*/

			 char* data16 = get_simmis_data_from_table("APP_PROMO_on_creation","3");
                        strcpy(app_cred,data16);



			dbl_app_cred = atof(app_cred);
			char narr[21];
			sprintf(narr,"Promo Credit for App");
			char senderr[21];
			sprintf(senderr,"Promo Credit");
			int res = addCredit(username,domainname,narr,dbl_app_cred,senderr,"USD",1,1);
			if(res > 0)
			{
				sprintf(command,"Add app credit of %f failed for username %s",dbl_app_cred,username);
				sprintf(subject,"Add app credit failed for username %s",username);	
				//send_mail("hoshang@staff.ownmail.com",command,subject);				
			}
			else
			{
				sprintf(command,"Add app credit of %f success for username %s",dbl_app_cred,username);
                                sprintf(subject,"Add app credit success for username %s",username);      
                                //send_mail("hoshang@staff.ownmail.com",command,subject);
			}
		}
		else
		{
			char narr[21];
			sprintf(narr,"Promo Credit for App");
			char senderr[21];
			sprintf(senderr,"Promo Credit");
			char *user_bal = db.get_field(0,"balance");
			do_log(user_bal);
			float u_bal = atof(user_bal);
			sprintf(command,"U_bal is %f",u_bal);
			do_log(command);
			double dbl_app_cred = 3;
			sprintf(command,"Amount to be credited is %f",dbl_app_cred);
			do_log(command);
			int res = addCredit(username,domainname,narr,dbl_app_cred,senderr,"INR",1,1);
			//int res = addCredit(username,domainname,narr,dbl_app_cred,senderr,"USD",1,1);
                        if(res > 0)
                        {
				sprintf(command,"Add app credit of %f failed for username %s",dbl_app_cred,username);
                                sprintf(subject,"Add app credit failed for username %s",username); 
                                //send_mail("hoshang@staff.ownmail.com",command,subject); 
                        }
                        else
                        {
				sprintf(command,"Add app credit of %f success for username %s",dbl_app_cred,username);
                                sprintf(subject,"Add app credit success for username %s",username); 
                                //send_mail("hoshang@staff.ownmail.com",command,subject);
                        }
			
				//sprintf(command,"Add app credit of %f Failed for username %s",dbl_app_cred,username);
                                //sprintf(subject,"Add app credit Failed for username %s becaus of Code Error",username); 
                                //send_mail("hoshang@staff.ownmail.com",command,subject);
		}
		/**************************************************************************************************************************/
		/*********************************Inserting in sim_user*************************************/
		sprintf(command,"select username, domainname from sim_user where username ='%s' and domainname='%s'",username,domainname);
		do_log(command);
		int result = db.ExecTuplesOk(command);
		if(!result)
		{
			sprintf(command,"insert into sim_user(username,domainname,clientname,companyname,add_building,add_street,add_city,add_state,add_country,add_pinno,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,mobno,emailadd,primary_emailadd,passport_no,passport_city)values('%s','%s','%s','%s','Not Provided','Not Provided','Not Provided','Not Provided','Not Provided','Not Provided','Not Provided','Not Provided','Not Provided','Not Provided','Not Provided','Not Provided','%s','%s','%s','X1550000','NA')",username,domainname,clientname,clientname,phone,email,email);
			do_log(command);
			int cnt = db.ExecCommandOk(command);
			if(!cnt)
			{
				do_log("Failed to insert into sim_user");
				sprintf(command,"rollback");
                                db.ExecCommandOk(command);
			}
			else
			{
				do_log("Inserted into sim_user");
				sprintf(command,"commit");
				do_log(command);	
				db.ExecCommandOk(command);
				sprintf(subject,"App user registration success for %s",email);
				sprintf(command,"New TSIM app user registeres successfully. \nemail - %s \nusername - %s \npassward - %s ",email,username,pwd);
				do_log(subject);
				do_log(command);
				//User creation mail Cust mail
				//if (!send_mail("hoshang@staff.ownmail.com",command,subject))				
				if (!send_mail("husain@staff.ownmail.com",command,subject))				
				{
					do_log("Failed to send mail on new user creation");
				}
				else
				{
					//send_mail("binu@staff.ownmail.com",command,subject);
					do_log("Mail send");
				}
			}
		}
		else
		{
			do_log("username and domainname already present");
		}	
		/********************************Sending Mess**********************************************/
		/*char sms[1024];
		sprintf(sms,"Registration successfull. Username- %s pwd- %s. App download link is http://bit.ly/2qhvMKk",username,pwd );
		do_log(sms);
		///usr/bin/curl --insecure --silent --connect-timeout 10 "https://www.siptraffic.com/myaccount/sendsms.php?username=trikonelec&password=blhebrgu&from=13474558912&to=919870145601&text=testsms9"
		char curlurl[4024];
		//sprintf(curlurl,"%s -dump --connect-timeout 10 \"%s?cmd=%s&at=%s&tx=%s\"",CURL_PATH,SMS_URL,"_notify-synch",PP_TOKEN,tx);
		sprintf(curlurl,"%s --connect-timeout 10 \"https://www.siptraffic.com/myaccount/sendsms.php?username=trikonelec&password=blhebrgu&from=13474558912&to=%s&text=%s\"",CURL_PATH,phone,sms);
		do_log(curlurl);

		FILE *fpp=NULL;
		char response[1024];
		char response_all[9999];
		if (!(fpp=popen(curlurl,"r"))){
			do_log("Could not start curl");
		}
		else{
			while (fgets(response,1024,fpp) ){
				if(response[strlen(response)-1]=='\n') response[strlen(response)-1]=' ';
				strcat(response_all,response);
			}
			pclose(fpp);
		}
		do_log(response_all);
		  else
		  {
		  do_log("User entry  in users table already exists. Check for gkusers");
		  }*/
		do_log("*********************Only App User creation Action End**************************************");
	}
	/****************************************************************************************************************************************************/
	else if(!strcmp(action, "sim_edit"))	//Sim Editor
	{
		if(!can_change_stock)	//Checking access permission
		{
			draw_warning_page(5);
		}
		else				//Authorized user
		{
			string sub_action = cgi_getentrystr("b");
			do_log(to_c_string(sub_action));

			if(sub_action == "draw")	//Draw editor page
			{
				draw_page("html_parts/edit_sim.html", 5);
			}
			else if(sub_action == "get_countries")	//Send list of countries (ajax)
			{
				send_countries(&db_r);
			}
			else if(sub_action == "search")	//Search and send single sim_stock tuple
			{
				sim_search(&db_r);
			}
			else if(sub_action == "save_changes")	//Save changes made in edit form
			{
				sim_save_changes(&db);
			}
		}
	}
	else if(!strcmp(action, "replace_sim"))
	{
		if(!can_change_stock)
		{
			draw_warning_page(5);
		}
		else
		{
			string sub_action = cgi_getentrystr("b");
			do_log(to_c_string(sub_action));

			if(sub_action == "draw")	//Draw replace page
			{
				draw_page("/srv/httpd/htdocs/ownmailtest/html_parts/replace_sim.html", 5);
			}
			else if(sub_action == "commit")	//Save changes, update backend
			{
				replace_sim(&db);
			}
		}
	}
	else if(!strcmp(action, "stock_summary"))
	{
		if(!can_change_stock)	//Checking access permission
		{
			draw_warning_page(5);
		}
		else
		{
			string sub_action = cgi_getentrystr("b");
			log_s(sub_action);

			if(sub_action == "draw")
			{
				draw_page("html_parts/sim_stock_summary.html", 5);
			}
			else if(sub_action == "query")
			{
				int format = cgi_getentryint("format");	// 1 => CSV else => html
				query_sim_stock(&db,format == 1 ? tuple_to_csv : tuple_to_html);	//Pass appropriate function pointer
			}
		}
	}
	else if(!(strcmp( action, "client_report" )))
	{
		string sub_action = cgi_getentrystr("b");
		log_s(sub_action);
		if(sub_action == "draw")
		{
			draw_page("html_parts/client_report.html", 5);
		}
		else if(sub_action == "queryJSON")
		{
			client_report(1);
		}
		else if(sub_action == "queryCSV")
		{
			client_report(2);
		}
	}
	else if(!strcmp(action, "test"))
	{
		draw_header(0);
		cout << warning("Test and debug section.");
		//backend_update_status("66877197702", false);

		/*
		//Combination of read_tag_attribute() and splitString() will return a vector
		string s = read_tag_attribute("CASE_SOURCE", "case_src");
		cout << warning(s);
		vector<string> v = splitString(s, ',');
		
		//Calling Function will handle output
		for(vector<string>::const_iterator i = v.begin(); i != v.end(); ++i)
		{
			cout << "<div>" << *i << "</div>";
		}
		*/
		cout << "<pre>";	
		/*
		string fields_init[] = { "username", "clientname", "companyname", "sales_person", "reseller_sp", "case_source", "status", "sim_phone_no", "telno", "mobno", "booking_date", "from_date", "to_date", "country", "add_city", "category"};
		vector<string> fields( fields_init, fields_init + ( sizeof ( fields_init ) /  sizeof ( string ) ) );
		for(vector<string>::iterator j = fields.begin(); j != fields.end(); ++j) {
			cout << *j << ", ";
		}
		*/
		queryResult q("SELECT * FROM sim_stock LIMIT 10;");
		cout << q.toCSV();
		cout << "</pre>";
		draw_end();
	}
	else if(!strcmp(action,"availsim"))
	{
		draw_header(1);
		printf("<div id=mh style=""width:99%;height:95%"">");
		printf("<form align=center name=checksim action=simmisn.e method=post>");
		printf("<br><br>The given box shows list of countries for which sim cards are available.\n You can select a country of your travel from this list:\n");
		//sprintf(command,"select distinct country from sim_stock where active =true and availability=true;");
		sprintf(command,"select distinct country from sim_stock where active =true and availability=true and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) and country in (select countryname from countrydata where active=true) order by country");
		do_log(command);
		int count=db_r.ExecTuplesOk(command);
		if (count>0){
			//printf("<select name=selcountry id=selcountry onchange=\"new net.ContentLoader('simmisn.e?a=get_count&b='+this.value,parseMyData,sim_cnt,null,'Get');\">");
			printf("<select name=selcountry id=selcountry>"); 
			printf("<option value=none>Select</option>");
			for (i=0;i<count;i++)
			{
				printf("<option value='%s'>%s</option>",db_r.get_field(i,"country"),db_r.get_field(i,"country"));
			}
			printf("</select><br>");
		}
		printf("\n<div id=\"sim_cnt\">");
		//printf("<b><label name=lbl1></label>");
		printf("\n</div>");
		//printf("\n<input type=button name=btnback value=\"Back\" onclick=\"moveback();\">");
		if (isAdmin){
			printf("\nEnter User id (if existing client) or email id to book the card.<br>\n");
			printf("\n<input type=radio name=optsearch value=optuser onclick=\"enabletext(this.value);\">User Id : &nbsp &nbsp <input type=text name=txtuser disabled><br>");
			printf("\n<input type=radio name=optsearch value=optemail onclick=\"enabletext(this.value);\">Email Id : &nbsp &nbsp <input type=text name=txtemail disabled><br>");
			printf("\n<input type=hidden name=a value=\"\">\n");
			printf("<input type=hidden name=chkadv value=\"off\">");
			printf("\n<input type=submit name=search value=Search onclick=\"return call_booking();\">\n");
			printf("</form>");
			printf("</div>");
		}
		else	printf("\n<input type=button name=btnback value=\"Back\" onclick=\"moveback();\">");
		draw_end();
      }
     else if(!strcmp(action,"get_count"))
	{
		char *country=cgi_getentrystr("b");
		sprintf(command,"select count(*) from sim_stock where country='%s' and active=true and availability=true",country);
		do_log(command);
		int count=db_r.ExecTuplesOk(command);
                if (count>0){
			printf("<b>Count=%s</b>",db_r.get_field(0,"count"));
			
		}
	}
	else if(!strcmp(action,"book"))
	{
		if (isAdmin){
			i=draw_booking(&db_r);
			if(i==0)
			return 0;
		}
	}
	else if(!(strcmp(action,"newtrip")))
	{	
		if (isAdmin){
			char *country=cgi_getentrystr("b");
        		char *userid=cgi_getentrystr("c");
        		char *emailid=cgi_getentrystr("d");
			char *chkadv=cgi_getentrystr("advance");
			draw_header(1);	
			printf("<div id=mh style=""width:99%;height:95%"">");	
			printf("<br><br><br><br><br><br>");
			printf("<form name=frmexistinguser action=simmisn.e method=post>");
			printf("\n<input type=hidden name=advance value='%s'>",chkadv);
			printf("\n<input type=hidden name=a value=%s>",emailid);
	        	printf("\n<input type=hidden name=b value=%s>",userid);
        		printf("\n<input type=hidden name=c>");
        		printf("\n<input type=hidden name=d>");
        		printf("\n<input type=hidden name=e>");
        		printf("\n<input type=hidden name=f>");
        		printf("\n<input type=hidden name=g>");
			draw_tripdetails("frmexistinguser",country);
			printf("\n<input type=submit name=sbtdone id=\"sbtdone\" disabled=true value=Done onclick=\"return booktrip('%s','%s','%s','frmexistinguser');\">",country,userid,emailid);
			printf("</form>");
			printf("</div>");
			draw_end();
		}
	}
	else if(!strcmp(action,"done"))
	{
		if (isAdmin){
			char *country=cgi_getentrystr("b");
			char *userid=cgi_getentrystr("c");
			do_log(userid);
			int result;
			if(userid==NULL||userid[0]==0)
				userid=" ";
			else
				userid=strdup(userid);
			char *emailid=cgi_getentrystr("d");
			do_log(emailid);
                	draw_header(1);
                	printf("<body id=""om"" onload=\"hide_cal();\" >");
			printf("<div id=mh style=""width:99%;height:95%"">");
                	printf("<form name=frmdone action=simmisn.e method=post>");
                	printf("<pre>");
                	printf("\n<input type=hidden name=a>");
			printf("\n<input type=hidden name=b>");
			if(!strcmp(userid," ")) 
			{
				result=booktrip(&db,country,"new",emailid); 
			}
			else{
				result=booktrip(&db,country,userid,emailid);	
			}
			if(result>0){
				printf("\n<center><b>	Sim card booking done.</b></center>");
			}
			else{
				printf("\n<font color=\"red\"><center><b>	Sim card booking failed.</b></center></font>");	
			}
			printf("</pre>");
			printf("</form>");
			printf("</div>");
			draw_end();
			cgi_exit(0);
		}
	}
	else if(!(strcmp(action,"fillstate")))
	{	char *country=cgi_getentrystr("b");
		char *dname=cgi_getentrystr("c");
		char *name=cgi_getentrystr("d");
		char *city;
		do_log(country);
		sprintf(command,"select distinct state from country_state_city where country='%s' order by state",country);
		do_log(command);
		
		if(!strcmp(dname,"dcity"))
			city="selcity";
		else
			city="seldcity";
		printf("\n<label>State:</label>");
		printf("\n<select name='%s' onchange=\"new net.ContentLoader('simmisn.e?a=fillcity&b='+this.value+'&c=%s',parseMyData,'%s',null,'Get');\">",name,city,dname);
		printf("<option value=none>Select</option>");
		int cnt = db_r.ExecTuplesOk(command);
		for(i=0;i<cnt;i++)
		{
			printf("<option value=\"%s\">%s</option>",db_r.get_field(i,"state"),db_r.get_field(i,"state"));
		}
                printf("</select>");
	}
	else if(!(strcmp(action,"fillcity")))
	{
		char *state=cgi_getentrystr("b");
		char *name=cgi_getentrystr("c");
          do_log(state);
		sprintf(command,"select distinct city from country_state_city where state='%s' order by city",state);
		do_log(command);
		printf("\n<label>City:</label>");
		printf("\n<select name='%s' onchange=\"if (this.value!= 'none')new net.ContentLoader('simmisn.e?a=fillisdcode&b='+this.value,parseMyData,'ddphonenums',null,'Get'); else {document.frmuserdetails.stdcode.value='';}\">",name);
          printf("<option value=none>Select</option>");
	  int cnt = db_r.ExecTuplesOk(command);
          for(i=0;i<cnt;i++)
          {
			printf("<option value=\"%s\">%s</option>",db_r.get_field(i,"city"),db_r.get_field(i,"city"));
          }
          printf("</select>");
	}
	else if(!(strcmp(action,"fillisdcode")))
	{
		char *city=cgi_getentrystr("b");
                do_log(city);
                sprintf(command,"select * from country_state_city where city='%s' order by city limit 1",city);
                do_log(command);
		if (db_r.ExecTuplesOk(command)){
			printf("\n<br><label>Phone no:</label><input type=text name=stdcode size=5 value=%s%s readonly>&nbsp;&nbsp;<input type=text name=txtphone size=15 onblur=\"return validphone(this.value,'Phone number',document.frmuserdetails.isdcode.value+document.frmuserdetails.stdcode.value);\" >",db_r.get_field(0,"isd_code"),db_r.get_field(0,"std_code"));
			printf("\n<br><label>Mobile no:</label><input type=text name=isdcode size=5 value=%s readonly>&nbsp;&nbsp;<input type=text name=txtmobno size=15 onblur=\"return validphone(this.value,'Mobile number',document.frmuserdetails.isdcode.value );\">",db_r.get_field(0,"isd_code"));

		}
		else{
			printf("\n<br><label>Phone no:</label><input type=text name=stdcode size=5 readonly>&nbsp;&nbsp;<input type=text name=txtphone size=15 onblur=\"return validphone(this.value,'Phone number',document.frmuserdetails.isdcode.value+document.frmuserdetails.stdcode.value);\" >");
			printf("\n<br><label>Mobile no:</label><input type=text name=isdcode size=5 readonly>&nbsp;&nbsp;<input type=text name=txtmobno size=15 onblur=\"return validphone(this.value,'Mobile number',document.frmuserdetails.isdcode.value );\">");
		}

	}
	else if(!(strcmp(action,"gettripdetails")))
	{
		draw_tripdetailpage(&db_r);
	}
	else if(!(strcmp(action,"gupgrep")))
	{
		draw_global_upgrade_report_form();
	}
	else if(!(strcmp(action,"grefrep")))
	{
		draw_global_ref_report_form();
	}
	else if(!(strcmp(action,"global_upg_rep")))
	{
		draw_global_upg_rep();
	}
	else if(!(strcmp(action,"global_ref_rep")))
	{
		//draw_global_ref_rep();
	}
	else if(!(strcmp(action,"summary")))
	{
		draw_summary(&db_r,download_mode);
	}
	else if(!(strcmp(action,"simstatus")))
	{
		if (isAdmin)
			draw_simstatus_page(&db_r);
		else{
			printf("<h3>You are not authorised</h3>");
		}
	}
	else if(!(strcmp(action,"getbdate")))
	{
		char *simno=cgi_getentrystr("b");
		char *status=cgi_getentrystr("c");
		if((!strcmp(status,"Delivered"))||(!strcmp(status,"Canceled"))||(!strcmp(status,"cardlost")))
			sprintf(command,"select to_char(booking_date,'yyyy-mm-dd') as bdate from clienttrip where sim_phone_no='%s' order by booking_date desc",simno);
		else if(!strcmp(status,"Returned"))
			sprintf(command,"select to_char(actual_deliver_date,'yyyy-mm-dd') as bdate from clienttrip where sim_phone_no='%s' order by booking_date desc",simno);
		if(db_r.ExecTuplesOk(command)>0){
			printf("<input type=hidden value='%s' name=bdate>",db_r.get_field(0,"bdate"));
		}
		else
			printf("<input type=hidden value='' name=bdate>");
	}
	else if(!(strcmp(action,"getsimno")))
	{
		char *country=cgi_getentrystr("b");
		printf(" &nbsp &nbsp <select name=selphone id=selphone onfocus=\"locktext('txtselphone','selphone');\">");
		printf("<option value=none>Select Sim Phone No.</option>");
		sprintf(command,"select sim_phone_no from sim_stock where country='%s' order by sim_phone_no",country);
		int cnt = db_r.ExecTuplesOk(command);
		for(i=0;i<cnt;i++)
		{
			printf("<option value='%s'>%s</option>",db_r.get_field(i,"sim_phone_no"),db_r.get_field(i,"sim_phone_no"));
		} 
		printf("</select><br>");
	}
	else if(!(strcmp(action,"checkstatus")))
	{
		if (isAdmin){
			char *simphoneno=cgi_getentrystr("b");
			do_log(simphoneno);
			draw_header(3);
			printf("<div id=mh style=""width:99%;height:95%"">");
			printf("<br>");
			char *simno=strtok(simphoneno,","); //If there are multiplt phone_no separated by commas
			show_status(&db_r,simno);
			while(simno != NULL)
			{
				simno = strtok(NULL, ",");
				if(simno!=NULL)
				show_status(&db_r,simno);
			}
			printf("</div>");
			draw_end();
		}
	}
	else if(!strcmp(action,"updatesimstatus"))
	{	
		if (isAdmin){
                	draw_header(3);
			printf("<div id=mh style=""width:99%;height:95%"">");
                	printf("<br><br>");
			simstatus(&db);
			printf("\n<input type=button name=btnback value=\"Back\" onclick=\"moveback();\">");
			printf("</div>");
			draw_end();
		}
	}
	else if(!strcmp(action,"searchclient"))
	{
		draw_header(2);
		printf("\n<div id=mh style=""width:99%;height:95%"">");
		printf("\n<br><br>");
		printf("\n<form name=frmsearch action=simmisn.e method=post>");
	 	printf("\n<label>Search by:</label>");
		printf("\n &nbsp &nbsp <select name=selsearch onchange=\"new net.ContentLoader('simmisn.e?a=search&b='+this.value,parseMyData,'dsearch',null,'GET');\">");
		printf("<option value=none>Select any one.</option>");
		printf("<option value=username>User ID</option>");
		printf("<option value=emailadd>Email ID</option>");
		printf("<option value=clientname>Client Name</option>");
		printf("<option value=companyname>Company Name</option>");
		printf("<option value=sim_phone_no>Sim Phone Number</option>");
		printf("<option value=mobno>Mobile Number</option>");
		printf("</select>");	
		printf("\n<div id=\"dsearch\">");
		printf("</div>");
		printf("</form>");
		printf("</div>");
		draw_end();	
	}
	else if(!strcmp(action,"search"))
	{
		char *criteria=cgi_getentrystr("b");
		if(!strcmp(criteria,"username"))
			criteria="User ID";
		else if(!strcmp(criteria,"emailadd"))
			criteria="Email ID";
		else if(!strcmp(criteria,"clientname"))
			criteria="Client Name";
		else if(!strcmp(criteria,"companyname"))
			criteria="Company Name";
		else if(!strcmp(criteria,"sim_phone_no"))
			criteria="Sim Phone Number";
		else if(!strcmp(criteria,"mobno"))
			criteria="Mobile Number";
		printf("\n<input type=hidden name=a>");
		printf("\n<label>Enter %s:</label>",criteria);
		printf("\n &nbsp &nbsp <input type=text name=txtcriteria>");
		printf("\n<br><input type=submit name=sbtsearch value=Search onclick=\"return showclient();\">");
	}
	else if(!strcmp(action,"showclient"))
	{
		show_client(&db_r);
	}
	else if(!strcmp(action,"showtrip"))
	{
		do_log("Entered in Showtrip");
		char *tripid=cgi_getentrystr("b");
		if(strcmp(tripid,"none")){
			char bill_status[251];
			bill_status[0]='\0';
			char ret_status[21];
			char str_country[51];
			char country_code_arr[51]="BB BIS WWSM EDC UEData GLN GLO WWSTD EDPP GLP USWW";
			char country_code[6];
			strcpy(bill_status,"Not known");
			int is_dd_enabled = 0;
			sprintf(command,"select c.*,sales_person,st.simno,country,is_dd_enabled,st.srno,countrycode,g.password from clienttrip c,sim_user s, sim_stock st,countrydata cn,gkusers g where c.username=s.username and tripid='%s' and c.sim_phone_no=st.sim_phone_no and st.country=cn.countryname and c.username=g.username and g.domainname='tsim.in'",tripid);
			do_log(command);
			if(db_r.ExecTuplesOk(command)<=0){
				printf("Sorry ! could not get data for the selected tripid");
				return 0;
			}
			strcpy(country_code,db_r.get_field(0,"countrycode"));
			is_dd_enabled = atoi(db_r.get_field(0,"is_dd_enabled"));
			printf("<br>\n<b>Username</b>              : %s",db_r.get_field(0,"username"));
			printf("<br>\n<b>Password</b>              : %s",db_r.get_field(0,"password"));
			printf("<br><br>\n<b>Trip ID</b>               : %s",db_r.get_field(0,"tripid"));
			printf("<br><br>\n<b>Country</b>               : %s",db_r.get_field(0,"country"));
			strcpy(str_country,db_r.get_field(0,"country"));
			printf("<br><br>\n<b>Sim number</b>      : %s",db_r.get_field(0,"simno"));
			printf("<br><br>\n<b>Sim number</b>      : Check it");
			do_log(db_r.get_field(0,"simno"));
			printf("<br><br>\n<b>Sim Phone number</b>      : %s",db_r.get_field(0,"sim_phone_no"));
			
			printf("<br><br>\n<b>Serial Number</b>      : %s",db_r.get_field(0,"srno"));
			printf("<br><br>\n<b>Unlimited Calling</b>     :");
			//fax_database db_un(E2FAXDB,DBSERV_R);
			//fax_database db_unlim(E2FAXDB,DBSERV_R);
			if (is_dd_enabled)
				sprintf(command,"select * from max_reduction_codes where tripid='%s' and std_code <> '%s' order by from_date desc",tripid,UNLIMITED_SECOND_LEG);
			else
				sprintf(command,"select *,to_date::date-from_date::date+1 as dura1 from max_reduction_codes where tripid='%s' order by from_date desc",tripid);
			do_log(command);
					//printf("<div id=duc>");
			char dt[20];
			int un_cnt = db.ExecTuplesOk(command);
			int dura1=0;
			cout << "<div id='duc'>";	//net.ContenLoader fails in absence of this div
			if ( un_cnt > 0 )
			{
					//printf("<div id=duc>\n");
				char std_c[51];
				printf("<ul>\n");
				for(int i = un_cnt-1; i>=0; i--)
				{
					printf("<li><b>From Date</b>    : %s</li>",db.get_field(i,"from_date"));
					printf("<li><b>To Date</b>      : %s</li>",db.get_field(i,"to_date"));
					if (!is_dd_enabled){
						dura1 = atoi(db.get_field(i,"dura1"));
						strcpy(std_c,db.get_field(i,"std_code"));
						//sprintf(command,"SELECT value from config_values where name ='tsim_packs' and key='%s_%d_%s'",str_country,dura1,std_c);
						sprintf(command,"SELECT get_tsim_pack_cost('%s','%s') as value",tripid,db.get_field(i,"from_date"));
						do_log(command);
						fax_database dbrates(E2FAXDB,DBSERV_R);
						if(dbrates.ExecTuplesOk(command)<=0){
							printf("Sorry ! could not get rates for the selected pack");
							//return 0;
							continue;
						}
						char *pack_cost = NULL;
						pack_cost = dbrates.get_field(0,"value");
						if (pack_cost == NULL || pack_cost[0] ==0){
							printf("Sorry ! could not get rates for the selected pack NULL");
							//return 0;
							continue;
						}
						pack_cost = strdupa(pack_cost);
						if (!strcmp(std_c,"91_Data")){
							printf("<li><b>Voice + Data Pack (Cost : USD %s)</li>",pack_cost);
						}
						else if(!strcmp(std_c,"91")){
							printf("<li><b>Voice only Pack (Cost : USD %s)</li>",pack_cost);
						}
						else
						{
							 printf("<li><b>Voice + Data Pack (Cost : USD %s)</li>",pack_cost);
						}
					}
				}
				printf("</ul>\n");
				//printf("<div id=duc>\n");
			}
					//if(isAdmin && !(strstr(country_code_arr,country_code)))
			char msg[56];
			sprintf(msg, "\ti is %d\tto_date is %s", i, db.get_field(i,"to_date"));
			do_log(msg);
			printf("<a "
				"href=\"Javascript:edit_unlimited_calling('%s','%s','%s','%s','%s','%s','%s','%s');\">"
				"Extend</a>",
				db_r.get_field(0,"status"),
				db_r.get_field(0,"tripid"),
				db_r.get_field(0,"to_date"),
				db_r.get_field(0,"username"),
				db_r.get_field(0,"domainname"),
				db_r.get_field(0,"is_dd_enabled"),
				db_r.get_field(0,"sim_phone_no"),
				db.get_field(i,"to_date") ? db.get_field(i,"to_date") : to_c_string(runcmd("date +'%Y-%m-%d'"))
				/*db.get_field(i,"to_date")*/
			);
			if ( un_cnt > 0 ){
				if (canchange_sales) {
					printf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
				printf("<a "
					"href=\"Javascript:delete_unlimited_calling('%s','%s','%s','%s','%s','%s','%s','%s');\">"
					"Delete</a>",
					db_r.get_field(0,"status"),
					db_r.get_field(0,"tripid"),
					db_r.get_field(0,"to_date"),
					db_r.get_field(0,"username"),
					db_r.get_field(0,"domainname"),
					db_r.get_field(0,"is_dd_enabled"),
					db_r.get_field(0,"sim_phone_no"),
					db.get_field(i,"from_date") ? db.get_field(i,"from_date") : to_c_string(runcmd("date +'%Y-%m-%d'"))
					/*db.get_field(i,"to_date")*/
				);
				}
			}

			sprintf(command,"select * from max_reduction_codes m,clienttrip c where m.tripid='%s' and m.from_date='now' and c.status in ('Delivered','Delivery pending') and c.tripid=m.tripid",tripid);
                        do_log(command);
                        int unlim_cnt=del.ExecTuplesOk(command);
			if(unlim_cnt > 0)
			{
				sprintf(command,"select * from unlimited_calling where tripid='%s'",tripid);
				do_log(command);
				if(del.ExecTuplesOk(command) ==0)
					printf("<div id=enable_now><br>&nbsp &nbsp &nbsp &nbsp <a href=\"Javascript:enable_now('%s','%s');\">Enable Now</a></div>",tripid,db.get_field(i,"from_date"));
			}
			printf("</div>");
			printf("<div id=hset>\n");
			printf("\n<br><b>Handset Model</b>      	: %s",db_r.get_field(0,"handset_model"));
			if (isAdmin)
				printf(" &nbsp &nbsp &nbsp &nbsp <a href=\"Javascript:edithandset(encodeURIComponent('%s'),'%s');\">Edit</a>",db_r.get_field(0,"tripid"),db_r.get_field(0,"handset_model"));
			printf("</div>");
			printf("<div id=\"dpymt\">");
			printf("<br>\n<b>Payment Mode</b>      	: %s",db_r.get_field(0,"payment_mode"));
			if(isAdmin)
				printf(" &nbsp &nbsp &nbsp &nbsp <a href=\"Javascript:editpayment('%s','%s','%s');\">Edit</a>",db_r.get_field(0,"tripid"),db_r.get_field(0,"status"),replacestring(db_r.get_field(0,"to_date")," ","%20"));
			printf("</div>");
			
			cout << "<div id = 'dfromdate'>";
			printf("<br>\n<b>From Date</b>             : %s",db_r.get_field(0,"from_date"));
			if (isAdmin)	//Edit from Date
				printf(" &nbsp &nbsp &nbsp &nbsp <a href=\"Javascript:editfromdate('%s','%s','%s','%s');\">Edit</a>",replacestring(db_r.get_field(0,"from_date")," ","%20"),db_r.get_field(0,"tripid"),db_r.get_field(0,"status"),db_r.get_field(0,"to_date"));	
			cout << "</div>";

			printf("<div id=dtodate>");
			printf("<br>\n<b>To Date</b>               : %s",db_r.get_field(0,"to_date"));
			if (isAdmin)
				printf(" &nbsp &nbsp &nbsp &nbsp <a href=\"Javascript:edittodate('%s','%s','%s','%s');\">Edit</a>",replacestring(db_r.get_field(0,"to_date")," ","%20"),db_r.get_field(0,"tripid"),db_r.get_field(0,"status"),db_r.get_field(0,"from_date")); 
			printf("</div>");
			printf("<br>\n<b>Booking Date</b>          : %s",db_r.get_field(0,"booking_date"));
			printf("<br><br>\n<b>Sim Card Delivery Date</b>: %s",db_r.get_field(0,"sim_deliver_date"));
			printf("<br><br>\n<b>Actual Delivery Date</b>  : %s",db_r.get_field(0,"actual_deliver_date"));
			printf("<br><br>\n<b>Return Date</b>           : %s",db_r.get_field(0,"return_date"));
			printf("<br><br>\n<b>Status</b>                : %s",db_r.get_field(0,"status"));
			strcpy(ret_status,db_r.get_field(0,"status"));
			printf("<br><br>\n<b>Sales Person</b>          : %s",db_r.get_field(0,"sales_person"));
			//printf("<br><br>\n<b>Comment</b>		   : %s",db_r.get_field(0,"comment"));
			printf("<br><br>\n<b>MIS User</b>  :  %s",db_r.get_field(0,"mis_user"));

			printf("<br><br>\n<b>Accessories Alloted</b>   :");
			/*
			sprintf(command,"select * from sim_accessories_stock where tripid='%s' and active=true",db_r.get_field(0,"tripid"));
			do_log(command);
			int cnt=db.ExecTuplesOk(command);
			//for(i=0;i<db.ExecTuplesOk(command);i++){
			for(i=0;i<cnt;i++){
				if(i==0)
				{
					//printf("%s(<a href=""simmisn.e?a=getaccstatus&b=%s"">%s-%s</a>)",db.get_field(i,"type"),db.get_field(i,"serialno"),db.get_field(i,"model"),db.get_field(i,"serialno"));
					printf("%s(<a href=""simmisn.e?a=getaccstatus&b=%s"">%s</a>)",db.get_field(i,"type"),urlencode(db.get_field(i,"display_name")),db.get_field(i,"display_name"));
				}
				else
				{
					//printf(",%s(<a href=""simmisn.e?a=getaccstatus&b=%s"">%s-%s</a>)",db.get_field(i,"type"),db.get_field(i,"serialno"),db.get_field(i,"model"),db.get_field(i,"serialno"));
					printf(",%s(<a href=""simmisn.e?a=getaccstatus&b=%s"">%s</a>)",db.get_field(i,"type"),db.get_field(i,"display_name"),db.get_field(i,"display_name"));
				}
			}
			*/
			sprintf(command,"select * from countrydata where countryname='%s' and postpaid_prepaid='postpaid'",str_country);
			do_log(command);
			if (db.ExecTuplesOk(command)){
				if (!strcmp(ret_status,"Returned")){
				/*
					sprintf(command,"select bill_no,closingbalance,to_char(updated_on,'dd/mm/yyyy') as updated_on from sim_trip_last_bill where tripid='%s'",tripid);
					do_log(command);
					if (db.ExecTuplesOk(command))
						sprintf(bill_status,"Last bill run on %s, closing balance is %.2f",db.get_field(0,"updated_on"),atof(db.get_field(0,"closingbalance")));
					else{


					}
				*/
					strcpy(bill_status,return_bill_status(tripid,str_country));
				}
				else{
					strcpy(bill_status,"Card not returned");
				}
				if (strlen(bill_status)>0)
					printf("<br><br>\n<b>Bill Status</b>                : %s",bill_status);
					//printf("<br><br>\n<b>Closing Balance</b>            :");

			}
			//Check if the user has given DD numbers.
			sprintf(command,"SELECT * from cb_addressbook where username ='%s'",db_r.get_field(0,"username"));
			do_log(command);
			if(db.ExecTuplesOk(command)>0)
			{
				printf("<br><br><b>Are DD numbers Inserted : </b> Yes");
			}
			else
			{
				printf("<br><br><b>Are DD numbers Inserted : </b> No");
			}
			//Check if There are any Clubbed Trips. If yes then display their numbers.
			sprintf(command,"SELECT sim_phone_no  from clienttrip where tripid_min = (SELECT tripid_min from clienttrip where tripid='%s')",tripid);
			do_log(command);
			int club_cnt=db.ExecTuplesOk(command);
			if(club_cnt > 1)
			{
				printf("<br><br><b>Clubbed Trips : </b> Yes");
				printf("<br><br><b>Clubbed numbers are: </b>");
				for(i=0;i<club_cnt;i++)
				{
					printf("%s, ",db.get_field(i,"sim_phone_no"));
				}
			}
			else
			{
				printf("<br><br><b>Clubbed Trips : </b> No");
			}
			printf("<div id=dcase>");
			printf("<br><b>Case Source</b>:           %s",db_r.get_field(0,"case_source"));
			if (isAdmin)
                                printf(" &nbsp &nbsp &nbsp &nbsp <a href=\"Javascript:editcase('%s','%s');\">Change</a>",db_r.get_field(0,"status"),db_r.get_field(0,"tripid"));
			printf("</div>");
			printf("<div id=dcomment>");
			printf("<br><b>Comment:</b>:           %s",db_r.get_field(0,"comment"));
			if (isAdmin)
                                printf(" &nbsp &nbsp &nbsp &nbsp <a href=\"Javascript:editcomment('%s','%s','%s');\">Edit</a>",db_r.get_field(0,"status"),db_r.get_field(0,"tripid"),db_r.get_field(0,"comment"));
			printf("</div>");
			printf("<div id=dcomment>");
                        printf("<br><b>Comment:</b>:           %s",db_r.get_field(0,"comment"));
                        if (isAdmin)
                                printf(" &nbsp &nbsp &nbsp &nbsp <a href=\"Javascript:editcomment('%s','%s','%s');\">Edit</a>",db_r.get_field(0,"status"),db_r.get_field(0,"tripid"),db_r.get_field(0,"comment"));
                        printf("</div>");
                        printf("<div id=dordernum>");
                        printf("<br><b>Order number:</b>:%s",db_r.get_field(0,"order_no"));
                        printf("</div>");
                        /*printf("<div id=dlabel>");
			sprintf(command,"select shipping_link from tsim_shipping_details where order_no ='%s'",db_r.get_field(0,"order_no"));
			do_log(command);
			int get_link = db.ExecTuplesOk(command);
			if(get_link > 1)
			{
                        	printf("<br><b>Shipping label:</b>:%s",db.get_field(0,"shipping_link"));
			}
			else
			{
				printf("<br><b>Shipping label:</b>:");
			}
                        printf("</div>");
			*/
		}
	}
	else if(!strcmp(action,"edit_payment"))
	{
		do_log("inside edit payment");
		char *tripid=cgi_getentrystr("b");
		char *todate=cgi_getentrystr("c");
		printf("<br><b>Payment Mode:</b>");
		printf("<input type=radio name=pm id=ccno value=\"Credit Card\" checked=\"checked\" onchange=\"eval_radio_trip_details()\">Credit Card");
		printf("<input type=radio name=pm value=\"Corporate\" onchange=\"eval_radio_trip_details()\">Corporate");
		printf("<input type=radio name=pm value=\"Advanced Payment\" onchange=\"eval_radio_trip_details()\">Advanced payment");
		printf("<br><label>Credit Card No:</label><input type=text name=txtccno onblur=\"checkccno(this.value);\">");
		printf("<br><label>Name On Credit Card:</label><input type=text name=txtnccno>");
		printf("<br><label>Credit Card Expiry Date:</label>");
		draw_month("selexpmonth");
		draw_year("selexpyear");
		printf("<br><pre id=lblccno>(Enter Credit Card Number to Continue)</pre>");
		printf("<input type=button name=\"sbtdone\" id=\"sbtdone\" value=\"Save\"  disabled=\"true\" onclick=\"Javascript:return savepymt(encodeURIComponent('%s'),'%s')\">",tripid,todate);
                printf("<input type=button name=\"btn_pymt_cancel\" id=\"btn_pymt_cancel\" value=\"Cancel\" onclick =\"Javascript:calltrip(encodeURIComponent('%s'));\">",tripid);
	}
	else if(!strcmp(action,"save_payment"))
	{
		char *tripid=cgi_getentrystr("b");
		char *iscc=cgi_getentrystr("c");
		char *sel_opt=cgi_getentrystr("d");
		if(!strcmp(iscc,"yes"))
		{
			char *ccno=cgi_getentrystr("e");
			char *nccno=cgi_getentrystr("f");
			char *expiry=cgi_getentrystr("g");
			sprintf(command,"update sim_user set creditcard_no='%s',cc_holder_name='%s',expdate='%s' where username=(select username from clienttrip where tripid='%s')",ccno,nccno,replacestring(expiry,"/","-"),tripid);
			do_log(command);
			if(db.ExecCommandOk(command)<=0){
                	        printf("FAIL.");
                	        do_log("Update on sim_user failed.");
                	}
			//execute command
		}
		sprintf(command,"update clienttrip set payment_mode='%s' where tripid='%s'",sel_opt,tripid);
		do_log(command);
		if(db.ExecCommandOk(command)<=0){
			printf("FAIL.");
			do_log("Update on sim_user failed.");
		}
		
	}
	else if(!strcmp(action,"edit_comment")){
		do_log("inside edit comment");
		char *tripid=cgi_getentrystr("b");
		char *comment=cgi_getentrystr("c");
		printf("<br><b>Comment:</b>");
		printf("<input type=\"text\" name=\"txtcomment\" id=\"txtcomment\" value=\"%s\">",comment);
		printf("<input type=button name=\"btn_comment_save\" id=\"btn_comment_save\" value=\"Save\" onclick=\"Javascript:return save_comment(encodeURIComponent('%s'))\">",tripid);
		printf("<input type=button name=\"btn_comment_cancel\" id=\"btn_comment_cancel\" value=\"Cancel\" onclick =\"Javascript:calltrip(encodeURIComponent('%s'));\">",tripid);
	}
	else if(!(strcmp(action,"save_comment")))
	{
		char *comment=cgi_getentrystr("b");
		char *tripid=cgi_getentrystr("c");
		sprintf(command,"update clienttrip set comment='%s' where tripid='%s'",comment,tripid);
		do_log(command);
		if(db.ExecCommandOk(command)<=0){
                        printf("FAIL.");
                        do_log("Update on clienttrip failed.");
                }
                else
                {
                        do_log("Update on clienttrip success");
                }
	}
	else if(!strcmp(action,"edit_case_src")){
		char *tripid=cgi_getentrystr("b");
		char *frm="right_form";
		char case_src[1024];
		char case_src_array[CASE_SRC_CNT][251];
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- CASE_SOURCE","-- CASE_SOURCE>","case_src",case_src)!=1) || (case_src==NULL || case_src[0]==0)){
			do_log("Could not open config file.");
			printf("Error occured while opening config file.");
		}*/
		
		   char* data17 = get_simmis_data_from_table("CASE_SOURCE_case_src","");
                        if(data17[0]==0 || data17==NULL)
                        {
                                  printf("Error occured while opening config file.");
                                do_log ("ERROR OCCURED WHILE GETTING CASE_SRC data17");
                        }
                        strcpy(case_src,data17);




		//do_log(case_src);
		if(str_split(',',case_src,case_src_array)<1){
			do_log("failed to split");
			printf("There was an error, please try after some time\n");
		}
		printf("<br><b>Case Source</b>:");
		printf("<select name=\"sel_case\" id=\"sel_case\" onchange=\"toggle_crm_options('%s');\">",frm);
		printf("<option value=none>Select Case Source</option>");
		int length1=sizeof(case_src_array)/sizeof(case_src_array[0]);
		for (int i=0;i<length1;i++){
			if (strlen(case_src_array[i])>0)
				printf("<option value=\"%s\">%s\n",case_src_array[i],case_src_array[i]);
		}
		printf("</select>");
          
		char crm_opt[1024];
		char crm_opt_array[CRM_OPT_CNT][251];
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- CASE_SOURCE","-- CASE_SOURCE>","crm_opt",crm_opt)!=1) || (crm_opt==NULL || crm_opt[0]==0)){
			do_log("Could not open config file.");
			printf("Error occured while opening config file.");
		}*/

		 char* data18 = get_simmis_data_from_table("CASE_SOURCE_crm_opt","");
                        if(data18[0]==0 || data18==NULL)
                        {
                                  printf("Error occured while opening config file.");
                                do_log("ERROR OCCURED WHILE GETTING CRM_OPT data18");
                        }
                        strcpy(crm_opt,data18);







		//do_log(crm_opt);
		if(str_split(',',crm_opt,crm_opt_array)<1){
			do_log("failed to split");
			printf("There was an error, please try after some time\n");
		}
		printf("<select name=\"sel_case_crm\" id=\"sel_case_crm\" style=\"visibility: hidden;\">");
		printf("<option value=none>Select Campaign Name</option>");
		int length2=sizeof(crm_opt_array)/sizeof(crm_opt_array[0]);
		for (int i=0;i<length2;i++){
			if (strlen(crm_opt_array[i])>0)
				printf("<option value=\"%s\">%s\n",crm_opt_array[i],crm_opt_array[i]);
		}
		printf("</select>");
	    
		//Last week / This week option
		draw_crm_case_source_options();
		
		printf("<input type='button' name='btn_case_save' id='btn_case_save' value='Save' onclick=\"Javascript:return save_case_src(encodeURIComponent('%s'))\">",tripid);
		printf("<input type='button' name='btn_case_cancel' id='btn_case_cancel' value='Cancel' onclick =\"Javascript:calltrip(encodeURIComponent('%s'));\">",tripid);
	}
	else if(!strcmp(action,"save_case_src")){
		char *tripid=cgi_getentrystr("c");
		char *src=cgi_getentrystr("b");
		do_log(tripid);
		do_log(src);
		sprintf(command,"update clienttrip set case_source='%s' where tripid='%s'",src,tripid);
		do_log(command);
		if(db.ExecCommandOk(command)<=0){
                        printf("FAIL.");
                        do_log("Update on clienttrip failed.");
		}
		else
		{
			do_log("Update on clienttrip success");
		}
	}
	else if(!strcmp(action,"edit_unlimited_calling"))
	{
		if(isAdmin){
		
		do_log("\tInside edit u call");

		char *tripid=cgi_getentrystr("b");
		char *to_date=cgi_getentrystr("c");
		char *username=cgi_getentrystr("d");
		char *domainname=cgi_getentrystr("e");
		char *is_dd_enabled=cgi_getentrystr("f");
		char *sim_phone_no=cgi_getentrystr("g");
		char *uc_to_date=cgi_getentrystr("h");
		//char *on_lu_update=cgi_getentrystr("olu");
			/* NOTE Debuging code ermove when done
			if(!strcmp(uc_to_date, ""))
			{
				cout << warning("uc");
				strcpy(uc_to_date, "2015-03-15");
			}
			else
			{
				cout << "<div>uc_to_date is " << uc_to_date << "</div>";
			}
			*/
		char command[200];
		//sprintf(command,"SELECT countrycode from countrydata where countryname =regexp_replace('%s','[^a-zA-Z ]','','g')",tripid);
		sprintf(command,"SELECT countrycode from countrydata where countryname =(select country from sim_stock where sim_phone_no='%s')",sim_phone_no);
		do_log(command);

		/*
		char c_arr[200]="WWSM EDC UEData GLN GLO WWSTD EDPP GLP 44 GUNP USWW 1";
		char c_arr2[51]="852 66 60 86 65 62";
		char ccode[5];
		*/
		if(db.ExecTuplesOk(command)>0)
		{
                	string ccode = db.get_field(0,"countrycode");
                	do_log(to_c_string(string("\tCountry Code is ") + ccode));
//			string s = read_tag_attribute("UNLIMITED_CALLING", ccode);
				
			  char query1[1024];
                          sprintf(query1,"UNLIMITED_CALLING_%s",ccode);
                          string s = get_simmis_data_from_table(query1,"");



			//(s == "") ? warning("Error Opening Config file");
			vector<string> v = splitString(s, ',');
			
			time_t current_time;
		    	char* c_time_string;
	    		current_time = time(NULL);
	    		c_time_string = ctime(&current_time);
	 		//printf("Current time is %s", c_time_string);		 
		        do_log(c_time_string);	 

		
			char buff[20];
			time_t now = time(NULL);
			strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
			
			/********************************Earlier Coorected Code****************
			printf("\n<br><b>From Date:</b><input type=text id=from_date value=%s readonly onclick=\"NewCal('from_date','yyyymmdd');\" onkeypress=\"newwindow.document.close();\" value=''><a href=""javascript:NewCal('from_date','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>",uc_to_date);
			cout << "\n<br><span>Duration</span>\n";
			for(vector<string>::const_iterator i = v.begin(); i != v.end(); ++i)
			{
				cout << "\n<input type='radio' id='ucop' name='uc_op' value='" + *i + "' checked='checked' />"
				     << "\n<span>" << *i << " days </span>";
			}
			s="";
			string data_plan = read_tag_attribute("UNLIMITED_DATA", ccode);
			do_log("data_plan");
			do_log(to_c_string(data_plan));
			if (data_plan.compare("1") == 0){
				cout << "\n\n<br>";
				printf("\nUnlimited Data:"); 
				do_log("UNLIMITED_DATA enabled");
				printf("<input type='checkbox' name=ucdata id ='ucdata' value='yes' checked='checked'><br>\n"); 
			}
			printf(" &nbsp &nbsp <input type=button name=btnsfrom_date value=Save onclick=\"Javascript:return save_unlimited_calling(document.getElementById('from_date').value,'%s','%s','%s','%s','%s','%s','%s');\">",to_date,tripid,username,domainname,is_dd_enabled,sim_phone_no,uc_to_date);
                	printf("<input type='button' name='btncan' value='Cancel' onclick =\"Javascript:calltrip(encodeURIComponent('%s'));\">",tripid);
			**************************************Ends Here*******************/	
			/**********************My Code Starts Here******************/
			cout << "\n<br><span>Duration</span>\n";
                        for(vector<string>::const_iterator i = v.begin(); i != v.end(); ++i)
                        {
                             cout << "\n<input type='radio' id='ucop' name='uc_op' value='" + *i + "' checked='checked' />"
                              << "\n<span>" << *i << " days </span>";
                        }
                        s="";
//                        string data_plan = read_tag_attribute("UNLIMITED_DATA", ccode);
			
			   char query2[1024];
                           sprintf(query2,"UNLIMITED_DATA_%s",ccode.c_str());
                           string data_plan = get_simmis_data_from_table(query2,"1");


                        do_log("data_plan");
                        do_log(to_c_string(data_plan));
                        if (data_plan.compare("1") == 0)
			{
                        	cout << "\n\n<br>";
	                        printf("\nUnlimited Data:"); 
        	                do_log("UNLIMITED_DATA enabled");
                	        printf("<input type='checkbox' name=ucdata id ='ucdata' value='yes' checked='checked'><br>\n"); 
                 	}
			
			//printf("\n<input type=radio name=uc_frm_date id=fixed_from_date onclick=\"document.getElementById('from_date').disabled=false;\">Fixed Date : &nbsp &nbsp <input type=text name=from_date id=from_date value=%s onclick=\"NewCal('from_date','yyyymmdd');\" onkeypress=\"newwindow.document.close();\" value='' disabled><a href=""javascript:NewCal('from_date','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>",uc_to_date);
			printf("<br><br><b>Select Date for Extended Plan</b>");
			printf("\n<br><br><input type=radio name=uc_frm_date id=fixed_from_date onclick=\"document.getElementById('from_date').disabled=false;\">Fixed Date : &nbsp &nbsp <input type=text name=from_date id=from_date value=%s onclick=\"NewCal('from_date','yyyymmdd');\" onkeypress=\"newwindow.document.close();\" value='' disabled><a href=""javascript:NewCal('from_date','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" id=""date_cal"" border=""0"" alt=""Pick a date""></a>",uc_to_date);
		       	//printf("</b><input type=radio name=uc_frm_date id=future_from_date value=%s onclick=\"document.getElementById('from_date').disabled=true;\">On Lu Update<br><br>",buff);
		       	printf("</b><br><input type=radio name=uc_frm_date id=future_from_date value='' checked onclick=\"document.getElementById('from_date').disabled=true;\">On Lu Update<br><br>");

			printf(" &nbsp &nbsp <input type=button name=btnsfrom_date value=Save onclick=\"get_from_date();Javascript:return save_unlimited_calling(final_date,'%s','%s','%s','%s','%s','%s','%s');\">",to_date,tripid,username,domainname,is_dd_enabled,sim_phone_no,uc_to_date);
                	printf("<input type='button' name='btncan' value='Cancel' onclick =\"Javascript:calltrip(encodeURIComponent('%s'));\">",tripid);
			/**************************************Ends Here*******************/	
		}
		do_log("\tExiting edit u call");
		}
	}
   else if(!strcmp(action,"delete_unlimited_calling"))
	{
		do_log("delete_unlimited_calling");
		//a=delete_unlimited_calling&b='+tripid+'&c='+uc_from_date+'&d='+username+'&e='+domainname+'&f='+is_dd_enabled+'&g='+sim_phone_no
		if (canchange_sales){
			char *tripid=cgi_getentrystr("b");
			char *uc_from_date=cgi_getentrystr("c");
			char *username=cgi_getentrystr("d");
			char *domainname=cgi_getentrystr("e");
			char *is_dd_enabled=cgi_getentrystr("f");
			char *sim_phone_no=cgi_getentrystr("g");
			char command[1024];
			sprintf(command,"delete from max_reduction_codes where tripid ='%s' and from_date='%s'",tripid,uc_from_date);
			do_log(command);
			if(db.ExecCommandOk(command)<=0) printf("FAIL");
			else printf("Done");
		}

	}
   else if(!strcmp(action,"save_unlimited_calling"))
   {
	if(isAdmin){
		char *username=cgi_getentrystr("b");
		char *domainname=cgi_getentrystr("c");
		char *tripid=cgi_getentrystr("d");
		char *from_date=cgi_getentrystr("e");
		char *to_date=cgi_getentrystr("f");
		char *is_dd_enabled=cgi_getentrystr("g");
		char *sim_phone_no=cgi_getentrystr("h");
		char *data_enabled=cgi_getentrystr("dt");
		//char *on_lu_update=cgi_getentrystr("olu");
		data_enabled = strdupa(data_enabled);
		is_dd_enabled = strdupa(is_dd_enabled);
				
		do_log(username);
		do_log(domainname);
		do_log(tripid);
		do_log(from_date);
		do_log(is_dd_enabled);
		do_log(sim_phone_no);
		//do_log(on_lu_update);
		//do_log(dt);
		do_log(data_enabled);
		//exit(0);
		printf("<br>%s<br>%s<br>%s<br>%s<br>%s<br>%s<br>%s",username,domainname,tripid,from_date,to_date,is_dd_enabled,sim_phone_no);
		sprintf(command,"select * from max_reduction_codes where tripid ='%s' and from_date='%s'",tripid,from_date);
		do_log(command);
		if (!db.ExecTuplesOk(command)){
		if(!strcmp(is_dd_enabled,"1"))
		{
			//printf("<br>Success<br>");
			sprintf(command,"INSERT INTO max_reduction_codes VALUES('%s','%s','%s','%s','%s','%s')",username,domainname,tripid,sim_phone_no,from_date,to_date);
			//printf("<br>%s",command);
			do_log(command);
			if(db.ExecCommandOk(command)<=0){
                        printf("FAIL.");
                        do_log("Insert in max_reduction failed.");
                	}
		}
		if (!strcmp(data_enabled,"yes"))
			sprintf(command,"INSERT INTO max_reduction_codes VALUES('%s','%s','%s','%s_%s','%s','%s')",username,domainname,tripid,UNLIMITED_SECOND_LEG,"Data",from_date,to_date);
		else{
			sprintf(command,"SELECT * from countrydata where countryname =(select country from sim_stock where sim_phone_no='%s')",sim_phone_no);
			do_log(command);
			if (!db.ExecTuplesOk(command)){
				printf("FAIL.");
				do_log("select from countrydata failed");
			}
			else{
//				string data_plan = read_tag_attribute("DEFAULT_DATA", db.get_field(0,"countrycode"));
				
				 string countrycode = db.get_field(0,"countrycode");

                                 char query3[1024];
                                 sprintf(query3,"DEFAULT_DATA_%s",countrycode);
                                 string data_plan = get_simmis_data_from_table(query3,"1");


				do_log("data_plan");
				do_log(to_c_string(data_plan));
				if (data_plan.compare("1") == 0){
					cout << "\n\n<br>";
					do_log("DEFAULT_DATA plan enabled");
					sprintf(command,"insert into max_reduction_codes(username,domainname,tripid,std_code,from_date,to_date) VALUES('%s','%s','%s','%s_%s','%s','%s')",username,domainname,tripid,UNLIMITED_SECOND_LEG,"Data",from_date,to_date);
				}
				else
					sprintf(command,"INSERT INTO max_reduction_codes VALUES('%s','%s','%s','%s','%s','%s')",username,domainname,tripid,UNLIMITED_SECOND_LEG,from_date,to_date);
			}
		}
		//printf("<br>%s",command);
		do_log(command);
		if(db.ExecCommandOk(command)<=0){
                        printf("FAIL.");
                        do_log("Insert in max_reduction failed.");
                }
		//printf("<br><input type=button name=btnback value=\"View Details\" onclick =\"Javascript:calltrip('%s');\">",tripid);
		char curr_dt[11];
		char from_date_for[11];
		strcpy(curr_dt,get_curr_date());
		sprintf(from_date_for,"%04d-%02d-%02d",return_year(from_date),return_month(from_date),return_day(from_date));
		sprintf(command,"curr_dt-%s-, from_date-%s-, from_date_for-%s-",curr_dt,from_date,from_date_for);
		do_log(command);
		if (!strcmp(curr_dt,from_date_for)){
			do_log("from_date is today, should enable now");
			//system("/usr/local/ownmail/bin/uc_cloud9_telna.sh prepaid_uc now 1>/dev/null 2>/dev/null");
				sprintf(command,"/usr/local/ownmail/bin/uc_cloud9_telna.sh enable_packs \"%s\" \"%s\" now 1>/dev/null 2>/dev/null",tripid,from_date);
				system(command);
		}
		}
		}
	   }
	   else if(!strcmp(action,"enable_now"))
	   {
		//system("/usr/local/ownmail/bin/uc_cloud9_telna.sh prepaid_uc now 1>/dev/null 2>/dev/null");
		char *tripid=cgi_getentrystr("t");
		char *from_date=cgi_getentrystr("fr");
		sprintf(command,"/usr/local/ownmail/bin/uc_cloud9_telna.sh enable_packs \"%s\" \"%s\" now 1>/dev/null 2>/dev/null",tripid,from_date);
		system(command);
	//printf("<br>&nbsp &nbsp &nbsp &nbsp Done");
   }
   else if(!strcmp(action,"edittodate"))
   {
	if (isAdmin)
	{
		char *todate=cgi_getentrystr("b");
		char *tripid=cgi_getentrystr("c");
		char *fromdt=cgi_getentrystr("d");
		printf("<br><b>To date:</b><input type=text value='%s' id=todate readonly onclick=\"NewCal('todate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('todate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>",todate);
		printf(" &nbsp &nbsp <input type=button name=btnstodate value=Save onclick=\"Javascript:return savetodate(document.getElementById('todate').value,encodeURIComponent('%s'),'%s');\">",tripid,fromdt);
		printf("<input type=button name=btnctodate value=Cancel onclick =\"Javascript:calltrip(encodeURIComponent('%s'));\">",tripid);
	}
   }
   else if(!strcmp(action,"savetodate"))
   {
	if (isAdmin){
		char *todate=cgi_getentrystr("b");
                char *tripid=cgi_getentrystr("c");
		sprintf(command,"update clienttrip set to_date='%s' where tripid='%s';",todate,tripid);
		do_log(command);
		if(db.ExecCommandOk(command)<=0)
		{
			printf("FAIL.");
			do_log("Update in clienttrip failed.");
		}
	}
   }
   else if(!strcmp(action,"editfromdate"))	/*From Date: the two cases that follow are exact replicas of the two preceeding cases with fromdate & todate interchanged*/
   {
	if (isAdmin)
	{
		char *fromdate=cgi_getentrystr("b");
		char *tripid=cgi_getentrystr("c");
		char *todate=cgi_getentrystr("d");
		printf("<br><b>From date:</b><input type=text value='%s' id='fromdate' readonly onclick=\"NewCal('fromdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('fromdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>",fromdate);
		printf("&nbsp &nbsp <input type='button' name='btnstodate' value='Save' onclick=\"Javascript:return savefromdate(document.getElementById('fromdate').value,encodeURIComponent('%s'),'%s');\">", tripid, todate);
		printf("<input type=button name=btncfromdate value=Cancel onclick =\"Javascript:calltrip(encodeURIComponent('%s'));\">",tripid);
	}
   }
   else if(!strcmp(action,"savefromdate"))
   {
	if (isAdmin)
	{
		char *fromdate=cgi_getentrystr("b");
                char *tripid=cgi_getentrystr("c");
		sprintf(command,"update clienttrip set from_date='%s' where tripid='%s';",fromdate,tripid);
		do_log(command);
		if(db.ExecCommandOk(command)<=0)
		{
			printf("FAIL.");
			do_log("Update in clienttrip failed.");
		}
	}
   }

   else if(!strcmp(action,"edithandset"))
   {
	if (isAdmin)
	{
		char *tripid = cgi_getentrystr("t");
		char *hset = cgi_getentrystr("h");
		printf("<br><b>Handset Model : <input type=text value='%s' id=hsettt>",hset);
		printf(" &nbsp &nbsp <input type=button name=btnshandset value=Save onclick=\"Javascript:return savehandset(encodeURIComponent('%s'),document.getElementById('hsettt').value);\">",tripid);
		printf("<input type=button name=btnchset value=Cancel onclick =\"Javascript:calltrip(encodeURIComponent('%s'));\">",tripid);
	}
   }
   else if(!strcmp(action,"savehandset"))
   {
	if (isAdmin){
		char *tripid = cgi_getentrystr("t");
		char *hset = cgi_getentrystr("h");
		sprintf(command,"update clienttrip set handset_model='%s' where tripid='%s';",hset,tripid);
		do_log(command);
		if(db.ExecCommandOk(command)<=0){	
			printf("FAIL.");
			do_log("Update in clienttrip failed.");
		}
	}
   }
   else if(!strcmp(action,"setupdd"))
   {
		char *user=cgi_getentrystr("b");
		do_log(user);
		draw_header(2);
		printf("<div id=mh style=""width:99%;height:95%"">");
		printf("<pre>");
		printf("\n<h4><b>  Set up DD numbers</b></h4>");
		printf("<form name=frmsetupdd action=simmisn.e method=post>");
		printf("\n<input type=hidden name=a>");
		printf("\n<input type=hidden name=b>");
		printf("\n				<a href =\"Javascript:showdd(%s);\">Show substitute numbers</a>",user);
		printf("\n<div id=\"dsetdd\" style=\"display:none\">");
		printf("</div>");
		if (isAdmin){
			printf("<br>\nEnter EmailId : &nbsp &nbsp <input type=text name=txtemail onblur=\"return substitute_email(this.value);\">");
			printf("<br>\n<input type=checkbox name=chkmail>Mark a copy to sales person.");
			printf("<br>\n<input type=submit name=sbtmail value=Mail onclick=\"return mailddno(%s);\">",user);
		}
		printf("</form>");
		printf("</pre>");
		printf("</div>");
		draw_end();
   }
   else if(!strcmp(action,"showdd"))
   {
	   draw_ddpage(&db_r);
   }
   else if(!strcmp(action,"addddno"))
	{
		if (isAdmin)
			add_ddno(&db);
	}
   else if(!strcmp(action,"mailddno"))
	{
		if (isAdmin)
			mail_ddno(&db_r);
	}
   else if(!strcmp(action,"edituser"))
	{
		if (isAdmin)
			edituser(&db_r);
	}
   /*else if(!strcmp(action,"editstock"))
	{
		if(isAdmin)
			editstock();
	}*/
   else if(!strcmp(action,"saveuserdetails"))
	{	do_log("Inside saveuserdetails");	
		if (isAdmin)
			savedetails(&db);
	}
   else if(!strcmp(action,"searchstock"))
	{
		draw_header(5);
                printf("\n<div id=mh style=""width:99%;height:95%"">");
                printf("\n<br><br>");
                printf("\n<form name=frmsearch action=simmisn.e method=post>");
                printf("\n<label>Search by:</label>");
                printf("\n &nbsp &nbsp <select name=selsearch onchange=\"new net.ContentLoader('simmisn.e?a=searchsim&b='+this.value,parseMyData,'dsearch',null,'GET');\">");
                printf("<option value=none>Select any one.</option>");
                printf("<option value=simno>Sim Number</option>");
                printf("<option value=sim_phone_no>Sim Phone Number</option>");
                printf("<option value=srno>Serial Number</option>");
                printf("</select>");
                printf("\n<div id=\"dsearch\">");
                printf("</div>");
                printf("</form>");
                printf("</div>");
                draw_end();
	}
   else if(!strcmp(action,"searchsim"))
	{
		char *criteria=cgi_getentrystr("b");
                if(!strcmp(criteria,"simno"))
			criteria="Sim Number";
		else if(!strcmp(criteria,"sim_phone_no"))
			criteria="Sim Phone Number";
		else if(!strcmp(criteria,"srno"))
			criteria="Serial Number";
		printf("\n<input type=hidden name=a>");
                printf("\n<label>Enter %s:</label>",criteria);
                printf("\n &nbsp &nbsp <input type=text name=txtcriteria>");
                printf("\n<br><input type=submit name=sbtsearch value=Search onclick=\"return showstock();\">");
	}
   else if(!strcmp(action,"showstock"))
        {
                show_stock(&db_r);
        }

   else if(!strcmp(action,"simstock"))
	{
	    if(isAdmin){
		char *type=cgi_getentrystr("b");
		draw_header(5);
		printf("\n<div id=mh style=""width:99%;height:95%"">");
		printf("\n<br><br>");
//The file format
//serialnum,sim_phone_num,pinno,pukno,vendorname
		printf("\n<form action=""upload_stock.pl"" method=""post"" enctype=""multipart/form-data"">");
		if(!strcmp(type,"all"))
			printf("\n<label>Select a CSV file to Upload:</label><input type=""file"" name=""filepath""> The CSV file should be in the following format&nbsp;&nbsp;->&nbsp;&nbsp;<b>(\"serialnum,sim_phone_num,pinno,pukno,vendorname\")</b><br>\n");
		else
			printf("\n<label>Select a CSV file to Upload:</label><input type=""file"" name=""filepath""> The CSV file should be in the following format&nbsp;&nbsp;->&nbsp;&nbsp;<b>(\"serialnum,pinno,pukno\")</b><br>\n");
		printf("<br>");
		//sprintf(command,"select distinct country from sim_stock order by country");
		sprintf(command,"select countryname as country from countrydata order by country");
		printf("<input type=hidden name=a value=\"%s\">",type);
		printf("\n<label>Select a country :</label> <Select name=optcountry id=\"ioptcountry\" title=\"Select country from the list.\">");
		printf("<option value='None' Selected>Select</option>");
		int cnt = db_r.ExecTuplesOk(command);
		for(i=0;i<cnt;i++)
			printf("<option value=\"%s\">%s</option>",db_r.get_field(i,"country"),db_r.get_field(i,"country"));
		printf("</select>");
		printf("<br>");
		printf("\n<input type=\"submit\" name=\"Submit\" value=\"Submit Form\">");
		printf("\n</form>");
		printf("\n</div>");		
		draw_end();
	   }
	   else printf("<h3>You are not authorised</h3>");
	}
  else if(!strcmp(action,"simstock_success"))
	{
		if(isAdmin){
                draw_header(5);
                printf("\n<div id=mh style=""width:99%;height:95%"">");
                printf("\n<br><br>");
//The file format
//serialnum,sim_phone_num,pinno,pukno,vendorname
                printf("\n<form action=""upload_stock.pl"" method=""post"" enctype=""multipart/form-data"">");
                printf("\n<label>Select a CSV file to Upload:</label><input type=""file"" name=""filepath""> The CSV file should be in the following format&nbsp;&nbsp;->&nbsp;&nbsp;<b>(\"serialnum,sim_phone_num,pinno,pukno,vendorname\")</b><br>\n");
                printf("<br>");
                sprintf(command,"select countryname as country from countrydata order by country");
                printf("\n<label>Select a country :</label> <Select name=optcountry id=\"ioptcountry\" title=\"Select country from the list.\">");
                printf("<option value='None' Selected>Select</option>");
		int cnt = db_r.ExecTuplesOk(command);
                for(i=0;i<cnt;i++)
                        printf("<option value=\"%s\">%s</option>",db_r.get_field(i,"country"),db_r.get_field(i,"country"));
                printf("</select>");
                printf("<br>");
                printf("\n<input type=\"submit\" name=\"Submit\" value=\"Submit Form\">");
		printf("<br><br><br><br>");
		printf("\nStock entry was successfully done !!!!");
		printf("\n<br>The details of entered stock are as follows:<br><br>");
		sprintf(command,"select * from sim_stock where dateandtime::date='now'");
		int count=db.ExecTuplesOk(command);
		printf("\n<table>");
		printf("<tr class=Mh><th>Sr.</th><th>Serial Number</th><th>Sim Phone Number</th><th>Sim Number</th><th>Country</th>");
		if(count>0)
		{
			for(int i=0;i<count;i++)
			{
				printf("\n<tr class=ML>");
				printf("\n<td>%d</td>",(i+1));
				printf("\n<td>%s</td>",db.get_field(i,"srno"));
				printf("\n<td>%s</td>",db.get_field(i,"sim_phone_no"));
				printf("\n<td>%s</td>",db.get_field(i,"simno"));
				printf("\n<td>%s</td>",db.get_field(i,"country"));
				printf("</tr>");
			}
		}
		printf("</table>");
                printf("\n</form>");
                printf("\n</div>");
                draw_end();
           }
           else printf("<h3>You are not authorised</h3>");
	}
  else if(!strcmp(action,"accessorystatus"))
       {
	   if(isAdmin){
		draw_header(3);
		printf("\n<div id=mh style=""width:99%;height:95%"">");
              /*  printf("\n<br><br>");
		printf("\n<form name=""frmaccstat"" method=""post"" action=simmisn.e>");
		printf("<input type=hidden name=a>");
		printf("<input type=hidden name=b>");
		printf("\n<label>Select instrument type :</label><select name=seltype onchange=\"new net.ContentLoader('simmisn.e?a=showopt&b='+seltype.value,parseMyData,'daccstat',null,'GET');\">");
		printf("\n<option value=none>-Select-</option>");
		sprintf(command,"select distinct(type) from sim_accessories_stock ;");
		int cnt = db_r.ExecTuplesOk(command);
		for(i=0;i<cnt;i++)
		printf("\n<option value='%s'>%s</option>",db_r.get_field(i,"type"),db_r.get_field(i,"type"));
		printf("</select>");
		printf("<div id=\"daccstat\">");
		printf("<label>Serial no.:</label>");
		printf("<select name=selsrno>");
                printf("<option value=none>-Select-</option>");
		printf("</select>");
		printf("</div>");
		//printf("\n <br> &nbsp &nbsp OR");
		//printf("<br><label>Enter Serial No.:</label><input type=text name=txtsrno id=txtsrno onfocus=\"locksel('selsrno',this.name);\">");
		printf("<input type=submit name=sbtgetstat value=Get Status onclick=\"return getaccstatus();\">");
		printf("\n</form>");*/
                printf("\n</div>");
                draw_end();
	    }
	    else printf("<h3>You are not authorised</h3>");
       } 
  else if(!strcmp(action,"showopt"))
       {	char *type=cgi_getentrystr("b");
/*		printf("<label>Serial No.:</label>");
		printf("<select name=selsrno id=selsrno onfocus=\"locktext('txtsrno',this.name);\">");
		printf("<option value=none>-Select-</option>");
		sprintf(command,"select display_name,model,serialno from sim_accessories_stock where type='%s' and active=true order by model,serialno;",type);
		int cnt = db_r.ExecTuplesOk(command);
		for(i=0;i<cnt;i++){
			printf("\n<option value='%s'>%s</option>",db_r.get_field(i,"display_name"),db_r.get_field(i,"display_name"));
		}
		printf("</select>");*/
       }
  else if(!strcmp(action,"getaccstatus"))
       {
		//char *srno=urlencode(cgi_getentrystr("b"));
		char *srno=cgi_getentrystr("b");
	/*	do_log(srno);
		draw_header(3);
                printf("\n<div id=mh style=""width:99%;height:95%"">");
                printf("\n<br><br>");
                printf("\n<form name=""frmshowstat"" method=""post"" action=simmisn.e>");	
		sprintf(command,"select username,tripid,sim_phone_no from clienttrip where tripid in(select tripid from sim_accessories_stock where display_name='%s' and active=true) order by booking_date desc;",srno);	
		do_log(command);
		if(db_r.ExecTuplesOk(command)>0){
			printf("<b><center>%s has been alloted to user id:%s for trip id:%s(%s).</b></center>",srno,db_r.get_field(0,"username"),db_r.get_field(0,"tripid"),db_r.get_field(0,"sim_phone_no"));
		}
		else{*/
		/*	sprintf(command,"select username,booking_id from advance_booking where booking_id in(select tripid from sim_accessories_stock where display_name='%s');",srno);
                	do_log(command);
                	if(db_r.ExecTuplesOk(command)>0)
                        	printf("<b><center>%s has been alloted to user id:%s for trip id:%s.(Advance Booking)</b></center>",srno,db_r.get_field(0,"username"),db_r.get_field(0,"booking_id"));
			else{
				sprintf(command,"select * from sim_accessories_stock where display_name='%s';",srno);
				do_log(command);
				if(db_r.ExecTuplesOk(command)>0){
					if((!strcmp(db_r.get_field(0,"alloted"),"f"))&&(!strcmp(db_r.get_field(0,"active"),"t")))
						printf("<b><center>%s is available for booking.</center></b>",srno);
					else if((!strcmp(db_r.get_field(0,"active"),"f")))
						printf("<b><center>%s is not active.</center></b>",srno);			
				}
				else
					printf("<b><center>Incorrect Serial Number.</center></b>");
			}
		}*/
		printf("\n<br><label>Select Status</label>");
		printf("<select name=selaccstat id=selaccstat onchange=\"check_stat(this.value);\">");
		printf("<option value=none>--Status--</option>");
		printf("<option value=Delivered>Delivered</option>");
                printf("<option value=Returned>Returned</option>");
                printf("<option value=\"lost\">Lost</option>");
		printf("</select>");
		printf("<div id=\"dacc_delivered\" style=\"visibility:hidden\">");
		printf("\n<br><label>Enter Username:</label>");
		printf("\n<input type=text id=\"txt_acc_username\">");
		printf("\n<input type=button id=\"btnfind\" name=\"btnfind\" value=Find title=\"click to search\" onclick=\"find_user_trips()\">");
		printf("\n<div id=dinnerdiv>");
		printf("\n<br><label>Select Tripid:</label>");
		printf("<select name=seltid id=seltid >");
                printf("<option value=none>Select Tripid:</option>");
		printf("</select>");
		printf("\n</div>");
		printf("\n</div>");
		printf("\n<input type=hidden name=a>");
		//printf("\n<input type=hidden name=b value='%s'>",srno);
		printf("\n<input type=hidden name=c>");
		printf("\n<br><input type=submit name=sbtupdate value=\"Update Status\" onclick=\"return updaccstat();\">");
		printf("\n<input type=button name=back value=Back onclick=\"moveback();\">");
		printf("\n</form>");
                printf("\n</div>");
                draw_end();	
       }
   else if(!strcmp(action,"fill_tripid")){
		do_log("fill_tripid");
		char *username=cgi_getentrystr("b");
		//sprintf(command,"select tripid from clienttrip where username='%s' and status in ('Delivered','Delivery pending') order by booking_date desc",username);
		sprintf(command,"select tripid ,c.sim_phone_no,country from clienttrip c,sim_stock s where username='%s' and status in ('Delivered','Delivery pending') and c.sim_phone_no=s.sim_phone_no order by booking_date desc",username);
		do_log(command);
		//fax_database sel(E2FAXDB,DBSERV_R);
		printf("\n<br><label>Select Tripid:</label>");
		printf("<select name=seltid id=seltid >");
		int count=db_r.ExecTuplesOk(command);
		if(count>0){
			printf("<option value=none>Select Tripid:</option>");
			for(i=0;i<count;i++)
			{
				printf("<option value='%s'>%s-%s</option>\n", db_r.get_field(i,"tripid"), db_r.get_field(i,"country"),db_r.get_field(i,"sim_phone_no"));
			}
		}
		printf("</select>");
	}
   else if(!strcmp(action,"updaccstat")){
		do_log("update status");
      		char *srno=cgi_getentrystr("b");
		char *status=cgi_getentrystr("selaccstat");
		char *tripid=cgi_getentrystr("c");
		char desc[1024],query[1024],query2[1024];
		//fax_database sel(E2FAXDB,DBSERV_R);
		int result;
		draw_header(3);
                printf("\n<div id=mh style=""width:99%;height:95%"">");
                printf("\n<br><br>");
		printf("<center>");

	/*	sprintf(command,"select * from sim_accessories_stock  where  display_name='%s' and active=true;",srno);
		do_log(command);
		if(db_r.ExecTuplesOk(command)>0){
			if((!strcmp(status,"Returned"))&&(!strcmp(db_r.get_field(0,"alloted"),"t"))){
				sprintf(query,"update sim_accessories_stock set alloted=false,tripid=null where display_name='%s'",srno);
			}
			else if(!strcmp(status,"lost")){
				sprintf(desc,"%s lost charges",db_r.get_field(0,"type"));
				do_log(desc);
				sprintf(command,"select username,domainname,tripid from clienttrip where tripid='%s';",db_r.get_field(0,"tripid"));
				do_log(command);
				if(db_r.ExecTuplesOk(command)>0){
                        		result=add_charges(&db,db_r.get_field(0,"username"),db_r.get_field(0,"domainname"),"Handset,Adapter and Charger Lost Charges",desc,"-99",db_r.get_field(0,"tripid"));
                        		if(result<=0){
                                     		printf("<b> &nbsp&nbsp&nbsp&nbsp %s lost charges failed.</b>",db_r.get_field(i,"type"));
                                     		return 0;
	                        	}
				}
				sprintf(query,"update sim_accessories_stock set active=false where display_name='%s'",srno);
				
			}
			else if(!strcmp(status,"Delivered"))
			{
				do_log("Inside Delivered");
				do_log(tripid);
				sprintf(query2,"update sim_accessories_stock set tripid='%s',alloted=true where display_name='%s' and alloted=false and active=true and tripid is null and '%s'not in (select distinct tripid from sim_accessories_stock where tripid is not null) RETURNING tripid",tripid,srno,tripid);
				do_log(query2);
				//if(db.ExecCommandOk(query2)>0)
				if(db.ExecTuplesOk(query2)>0)
				{
					do_log("Handset allocated");
					printf("<br><b>Status of %s has been updated to %s.</b><br>",srno,status);
					sprintf(query2,"UPDATE clienttrip SET handset_model ='Office Handset - %s' where tripid='%s'",srno,tripid);
					do_log(query2);
					if(db.ExecCommandOk(query2)>0)
					{
						do_log("Handset updated for the trip");
					}
					else
					{
						do_log("Failed to update handset details for the trip");
						printf("<br><font color=red><b>Failed to update handset details for the trip</b></font><br>");
					}
				}
				else
				{
					printf("<br><b><font color=red>Failed to allocate handset</font></b><br>");
					do_log("Failed to allocate handset");
				}
			}
			else{
				printf("<br><b><front color=red>Status of %s cannot be updated to %s.</font></b><br>",srno,status);
				printf("<br><input type=button name=back value=Back onclick=\"moveback();\">");
				return 0;
			}
			do_log(query);
			result=db.ExecCommandOk(query);
			if(result>0){
				do_log("Status updated");
                                printf("<b>Status of %s has been updated to %s.</b>",srno,status);
                        }
		}
		else
			printf("<b>Status of %s cannot be updated to %s.</b>",srno,status);*/
		printf("<br><input type=button name=back value=Back onclick=\"moveback();\">");
		printf("</center>");
		printf("\n</div>");
                draw_end();
      }
    else if(!strcmp(action,"showacc"))
	{
		do_log("show allot");
		char *tripid=cgi_getentrystr("b");
		char *status=NULL;
		fax_database db1(E2FAXDB,DBSERV_R);
		draw_header(2);
		printf("\n<div id=mh style=""width:99%;height:95%"">");
                printf("\n<br><br>");	
		printf("<form name=frmallot action=simmisn.e method=post>");
		printf("<center>");
		sprintf(command,"select status from clienttrip where tripid='%s'",tripid);
                if(db_r.ExecTuplesOk(command)>0)
                        status=db_r.get_field(0,"status");
		if((!strcmp(status,"Returned"))||(!strcmp(status,"Canceled"))||(!strcmp(status,"cardlost"))){
                        printf("<b>Accessories Cannot be alloted as the status of card is %s.</b>",status);
                        printf("\n<input type=button name=back value=Back onclick=\"moveback();\">");
                        return 0;
                }
		
		/*sprintf(command,"select distinct type from sim_accessories_stock where active=true and alloted=false;");
	        int count=db_r.ExecTuplesOk(command);
		if(count>0){
        	for(i=0;i<count;i++){
                if(!strcmp(db_r.get_field(i,"type"),"handset")){
                        printf("\n<br><input type=checkbox name=chk_%s id=chk_%s onclick=\"enablesel('chk_%s','sbtallot');\">%s",db_r.get_field(i,"type"),db_r.get_field(i,"type"),db_r.get_field(i,"type"),db_r.get_field(i,"type"));
                        int j;
                        printf("\n<select name=selhandset id=selhandset disabled onchange=\"document.getElementById('sbtallot').disabled=false;\">");
                        printf("<option value='none'>--Select Handset--</option>");
                        sprintf(command,"select * from sim_accessories_stock where type='handset' and active=true and alloted=false;");
                        int res=db.ExecTuplesOk(command);
                        for(j=0;j<res;j++)
                                printf("<option value='%s'>%s-%s</option>",db.get_field(j,"serialno"),db.get_field(j,"model"),db.get_field(j,"serialno"));
                        printf("</select>");
                }
                else
                        printf("\n<br><input type=checkbox name=chk_%s>%s",db_r.get_field(i,"type"),db_r.get_field(i,"type"));
		}
		
		printf("\n<input type=hidden name=a>");
		printf("\n<input type=hidden name=b value='%s'>",tripid);
		printf("\n<br><input type=submit name=sbtallot value=\"Allot\" onclick=\"document.frmallot.a.value='allot';\">");
		}
		else
			printf("<b><center>No Accessories are available to allot.</center></b>");*/
		printf("\n<input type=button name=back value=Back onclick=\"moveback();\">");
		printf("</form>");
		printf("\n</div>");
                draw_end();
	}
    else if(!strcmp(action,"allot"))
	{
		int count;
		char *tripid=cgi_getentrystr("b");
		draw_header(2);
		printf("\n<div id=mh style=""width:99%;height:95%"">");
                printf("\n<br><br>");
		/*
		sprintf(command,"select distinct on (type)type,serialno from sim_accessories_stock where active=true and alloted=false;");
                do_log(command);
                count=db_r.ExecTuplesOk(command);
                for(i=0;i<count;i++){
                        char *type=db_r.get_field(i,"type");
                        char *serial=db_r.get_field(i,"serialno");
                        char chktype[1024];
                        sprintf(chktype,"chk_%s",type);
                        do_log(chktype);
                        char *ischeck=cgi_getentrystr(chktype);
                        if(ischeck==NULL||ischeck[0]==0)
                                ischeck="off";
                        else
                                ischeck=strdup(ischeck);
                        do_log(ischeck);
                        if(!strcmp(ischeck,"on")){
                                sprintf(command,"update sim_accessories_stock set alloted=true,tripid='%s' where type='%s' and serialno='%s';",tripid,type,serial);
                                do_log(command);
                                if(db.ExecCommandOk(command)<=0)
                                {
					printf("<b><center>Update in sim_accessories_stock failed.</center></b>");
                                        return 0;
                                }
				printf("<center>%s - %s</center>",type,serial);
                        }
                }*/
		printf("<b><center>Accessories alloted successfully.</center></b>");
		printf("<center><input type=button name=back value=Back onclick=\"moveback();\"></center>");
		printf("\n</div>");
                draw_end();
	}
    else if(!strcmp(action,"chkmob")) //CHECK IF THE MOBILE No ENTERED IS A PROPER ONE
	{
		do_log("in chkmob");
		char *mobno = cgi_getentrystr("mobno");
		char *isdcode = cgi_getentrystr("isdcode");
		if ( mobno == NULL || mobno[0]==0 ||  isdcode==NULL || isdcode[0]==0 ){
			printf("no");
		}
		else{
			char mob[5];
			strncpy(mob,mobno,4);
			mob[4]='\0';
			/*
			sprintf(command,"select * from std_codes where isd_code = '%s' and std_code like '%s%%' and ismobile=true limit 1",isdcode,mob);
			do_log(command);
			if(db_r.ExecTuplesOk(command)>0){
				sprintf(command,"select * from number_length where country_name= '%s'",db_r.get_field(0,"country"));
				do_log(command);
				if(db_r.ExecTuplesOk(command)>0){
					int len = atoi(db_r.get_field(0,"num_length"));
					if (strlen(mobno)+strlen(isdcode) != len) printf("no");
					else printf("yes");
				}
				else printf("yes");
			}
			else
				printf("no"); */
		}

	}
    else if(!strcmp(action,"clubtripform"))
	{
                draw_header(8);
                printf("\n<div id=mh style=""width:99%"">");
                if (!isAdmin && !canUpgrade){
                        printf("<br><br>You are not authorised to do this<br><br>");
                        cgi_exit(0);
                }
                printf
                    ("<script type=\"text/javascript\" src=\"refill_validate.js\"></script>\n");
                printf
                    ("<form name=refill action=simmisn.e  method=post>\n");
                printf("<input type=hidden name=a value=\"\">\n");
                printf("<label>Search Options</label>");
                printf
                    ("<input type=radio name=search1 value=username checked onclick=\"searchby('gsim_refill.e?a=search','searchopt='+this.value + '&club=yes');\">User ID\n");
                printf
                    ("<input type=radio name=search1 value=clientname onclick=\"searchby('gsim_refill.e?a=search','searchopt='+this.value + '&club=yes');\">Client Name\n");
                printf
                    ("<input type=radio name=search1 value=sim_phone_no onclick=\"searchby('gsim_refill.e?a=search','searchopt='+this.value + '&club=yes');\" >Sim Phone Number\n");
                printf ("<input type=radio name=search1 value=emailadd onclick=\"searchby('gsim_refill.e?a=search','searchopt='+this.value + '&club=yes');\" >Email Id\n");
                printf("<br><br>");
                printf("<div id=search>\n");
                printf("<label>Enter User Id:</label>\n");
                printf
                    ("<input type=text name=txtfind id=\"txtfind\" title=\"Enter user id to search\" onfocus=this.select();>\n");
                printf
                    ("<input type=button name=btnfind value=Find title=\"click to search\" onclick=\"find_user_trips('username',document.frmtripclub.txtfind.value);\">\n");
                printf("<br><br>\n");
                printf("<div id=\"select\">\n");
                printf("<label>Select User ID:</label>\n");
                printf
                    ("<select name=optuser id=\"optuser\" title=\"Select a client from the list\" >\n");
                printf
                    ("<option value=none selected>Select User Id</option>\n");
                printf("</select>\n");
                printf("</div>\n");
                printf("</div>\n");
		printf("<br>\n");
		printf("<div id=triplist></div>\n");
		printf("<input type=hidden name=min_tripid value=\"\">\n");
		printf("<input type=hidden name=sel_trips value=\"\">\n");
		printf("<br><br>");
		printf("<div id=sbclub>\n");
                printf
                    ("<input type=submit name=\"gettrips\" value=\"Club trips\" onclick=\"return club_trips(document.refill.trip);\">\n");
		printf("</div>\n");
		printf("</form>\n");
	}
    else if(!strcmp(action,"performclubtrip"))
	{
                draw_header(8);
                printf("\n<div id=mh style=""width:99%"">");
                if (!isAdmin &&  !canUpgrade){
                        printf("<br><br>You are not authorised to do this<br><br>");
                        cgi_exit(0);
                }
		char *min_tripid = cgi_getentrystr("min_tripid");
		char *sel_trips  = cgi_getentrystr("sel_trips");
		if (min_tripid == NULL || min_tripid[0] == 0){
			printf("No Trips selected");
			return -1;
		}
		if (sel_trips == NULL || sel_trips[0] == 0){
			printf("No Trips selected");
			return -1;
		}
		sel_trips = strdupa(sel_trips);
		min_tripid= strdupa(min_tripid);
		sprintf(command,"update clienttrip set tripid_min='%s' where tripid in (%s)",min_tripid,sel_trips);
		do_log(command);
		if(db.ExecCommandOk(command)<=0){	
			printf("<font color=\"red\">FAILED</font>");
			do_log("Failed to club");
		}
		else{
			printf("Done");
			do_log("updated");
		}
	}
    else if(!strcmp(action,"draw_inum_booking"))
	{
		draw_header(9);
		printf("\n<div id=mh style=""width:99%"">");
                if (!isAdmin &&  !canUpgrade){
                        printf("<br><br>You are not authorised to do this<br><br>");
                        cgi_exit(0);
                }
		//if(!strcmp(domain_name,"sim.trikon.in"))
		//{
		//	printf("<br><center><b><font color=\"red\">Please Log in using <u>mail.tsim.in</u> inorder to use this feature</font></b></center><br>");
		//}
		//else
		//{}
			//printf("<center><b> Success <b></center>");
			//printf("<center><b> Domain Name : %s <b></center>",domain_name);
			printf("<input type=hidden name=a>");
			printf("<form name=inum_usr_details method=post action=simmisn.e>");
			printf("<br>");
			printf("<center><b><u>New Client</u></b></center>");
                	printf("\n<br><label>Contact Person Name:</label>");
                	printf("\n<select name=selsal>");
                	printf("<option value=none>Select</option>");
                	printf("<option value=Dr.>Dr.</option>");
                	printf("<option value=Ms.>Ms.</option>");
                	printf("<option value=Mrs.>Mrs.</option>");
                	printf("<option value=Mr.>Mr.</option>");
                	printf("</select>");
                	printf("\n<input type=text name=txtname onblur=\"return validname(this.value);\">\n");
                	printf("\n<br><label>Company Name:</label><input type=text name=txtcompany>(optional)");
                	printf("\n<br><br><b><u>Billing Address:</u></b><br>");
                	printf("\n<br><label>Building Name:</label><input type=text name=txtbldg onblur=\"return validaddress(this.value);\" >");
                	printf("\n<br><label>Street:</label><input type=text name=txtstreet onblur=\"return validaddress(this.value);\" >");
                	printf("\n<br><label>Pin Code:</label><input type=text name=txtpin onblur=\"return validpinno(this.value);\">");
                	printf("\n<br><label>Country:</label><select name=selcountry onchange=\"new net.ContentLoader('simmisn.e?a=fillstate&c=dcity&b='+this.value+'&d=selstate',parseMyData,'dstate',null,'Get');\">");
                	printf("<option value=none>Select</option>");
                	sprintf(command,"select distinct country from country_state_city");
                	do_log(command);
			int rec_cnt = db.ExecTuplesOk(command);
                	for(i=0;i<rec_cnt;i++)
                	{
                        	printf("<option value=%s>%s</option>",db.get_field(i,"country"),db.get_field(i,"country"));
                	}
                	printf("</select>");
                	printf("\n<div id=\"dstate\">");
                	printf("\n<label>State:</label>");
                	printf("<select name=selstate>");
                	printf("<option value=none>Select</option>");
                	printf("</select>");
                	printf("\n</div>");
                	printf("\n<div id=\"dcity\">");
                	printf("\n<label>City:</label>");
                	printf("<select name=selcity>");
                	printf("<option value=none>Select</option>");
                	printf("</select>");
                	printf("\n</div>");
                	printf("\n<input type=checkbox name=chkaddr onclick=\"toggledeladdr('inum_usr_details');\" onchange=\"toggledeladdr(this.value);\">Click here if delivery address is same as billing address.");
                	printf("\n<div id=daddress>");
                	printf("<br><br><b><u>Delivery address:</u></b><br>");
                	printf("\n<br><label>Building Name:</label><input type=text name=txtdbldg onblur=\"return validaddress(this.value);\">");
                	printf("\n<br><label>Street:</label><input type=text name=txtdstreet onblur=\"return validaddress(this.value);\">");
                	printf("\n<br><label>Pin Code:</label><input type=text name=txtdpin onblur=\"return validpinno(this.value);\">");
                	printf("\n<br><label>Country:</label><select name=dselcountry onchange=\"new net.ContentLoader('simmisn.e?a=fillstate&c=ddcity&b='+this.value+'&d=seldstate',parseMyData,'ddstate',null,'Get');\">");
			printf("<option value=none>Select</option>");
                	sprintf(command,"select distinct country from country_state_city");
                	do_log(command);
			rec_cnt = db.ExecTuplesOk(command);
                	for(i=0;i<rec_cnt;i++)
                	{
                	        printf("<option value=%s>%s</option>",db.get_field(i,"country"),db.get_field(i,"country"));
                	}
                	printf("</select>");
                	printf("\n<div id=\"ddstate\">");
                	printf("\n<label>State:</label><select name=seldstate onchange=\"new net.ContentLoader('simmisn.e?a=fillcity&b='+this.value,parseMyData,'ddcity',null,'Get');\">");
                	printf("<option value=none>Select</option>");
                	printf("\n</select>");
                	printf("\n</div>");
                	printf("\n<div id=\"ddcity\">");
                	printf("\n<label>City:</label><select name=seldcity onchange=\"if (this.value != 'none') new net.ContentLoader('simmisn.e?a=fillisdcode&b='+this.value,parseMyData,'ddphonenums',null,'Get');else {document.frmuserdetails.stdcode.value='';}\">");
                	printf("<option value=none>Select</option>");
                	printf("\n</select>");
                	printf("\n</div>");
                	printf("\n</div>");
                	printf("\n<br><b><u>Contact details:</u></b><br>");
                	printf("\n<div id=\"ddphonenums\">");
                	printf("\n<br><label>Phone no:</label><input type=text name=stdcode size=5 readonly>&nbsp;&nbsp;<input type=text name=txtphone size=15 onblur=\"return validphone(this.value,'Phone number',document.frmuserdetails.isdcode.value+document.frmuserdetails.stdcode.value);\" >");
	                printf("\n<br><label>Mobile no:</label><input type=text name=isdcode size=5 readonly>&nbsp;&nbsp;<input type=text name=txtmobno size=15 onblur=\"return validphone(this.value,'Mobile number',document.frmuserdetails.isdcode.value);\">");
                	printf("\n</div>");

                	printf("\n<br><b><u>Email Address:</u></b><br>");
                	printf("\n<br><label>Primary Email:</label><input type=text name=primary_email onblur=\"return valid_emailid(this.value);\">");
	                printf("\n<br><input type=checkbox name=chkemail onclick=\"togglecommemail('inum_usr_details');\" onchange=\"togglecommemail(this.value);\">Click here if Communication email is same as Primary email.");
                	printf("\n<div id=\"dcommemail\">");
                	printf("\n<br><label>Communication Email:</label><input type=text name=comm_email onblur=\"return valid_emailid(this.value);\">");
                	printf("\n</div>");
                	printf("<br><br>");
                	//draw_tripdetails("frmuserdetails",country);
                	printf("\n<br><label>Sales Person Name:</label>\n");
                	printf("\n<select name=selsalep id=\"selsalp\" multiple size=4>");
                	printf("<option value=none selected>Select</option>");
                	sprintf(command,"select sp_name from salesperson where domainname='sim.trikon.in' order by sp_name");
                	do_log(command);
			int count=db.ExecTuplesOk(command);
                	for(i=0;i<count;i++){
                	        printf("<option value='%s'>%s</option>",db.get_field(i,"sp_name"),db.get_field(i,"sp_name"));
                	}
                	printf("\n</select>");
			printf("<br>");
			printf("<input type=submit value=Continue onclick=\"return inum_booking();\">");
			printf("</form>");
	//	}
	}
	else if(!strcmp(action,"generate"))
	{
		printf("Generating user id for the user");
	}
	
	do_log("end"); 
	return 0;
}


int process_stock_entry(const char* sku, const char* order_no, fax_database *db, char* unique_order_no_out) 
{
    	do_log("---------------starting the process_stock_entry function-------------");
    	int stock_entry_done = 0;
    	char command[1024];
    	char seq_query[1024];
    	char serialnumber1[1024];
    	char serialnumber2[1024];
    	char sequence_table[1024];
    	char country_cstr[1024];
    	char simstock_query[2048];
   
    	do_log("SKU is Below");
    	do_log(sku);
    	do_log("Order no is below");
    	do_log(order_no);


    	//if ((strstr(sku, "japan") != NULL && strstr(sku, "esim") != NULL) || (strstr(sku, "esim_") != NULL && strstr(sku, "-eg") != NULL) || (strstr(sku, "ar-esim") != NULL) || (strstr(sku, "-esim") != NULL)) 
	//{


        	sprintf(command, "SELECT srno, seq_table, countryname FROM countrydata WHERE countryname ILIKE '%s';", sku);
	        do_log(command);
	        int num_rows = db->ExecTuplesOk(command);
	        if (num_rows > 0) 
		{
	        	sprintf(serialnumber1, "%s", db->get_field(0, "srno"));
	            	sprintf(sequence_table, "%s", db->get_field(0, "seq_table"));
            		sprintf(country_cstr, "%s", db->get_field(0, "countryname"));
		}
		else 
		{
			do_log("country not present in countrydata, creating the same");
			sprintf(command,"insert into countrydata_base (countrycode,countryname,active,srno,seq_table,postpaid_prepaid,is_dd_enabled,charge_rent,card_type,add_credit_on_booking,api_code,add_e_credit_entry_on_booking,add_reg_charge,activation_country_mapping) values('%s','%s',true,'%s','other_sim_seq','prepaid',0,false,'country_specific',0::bit,'dummy',0::bit,0::bit,'OTHERS')",sku,sku,sku);
			do_log(command);
			int res = db->ExecCommandOk(command);
			if (!res)
			{
				do_log("Failed to create country, still proceeding with the booking");
				char subj[101];
				char msg[501];
				sprintf(msg,"Hello,\n\nThe following query failed.\n\n%s\n\n\nPlease do the needful.",command);
				sprintf(subj,"Country creation failed in autobooking");
				//tsim mail
				send_mail("services@tsim.in",msg,subj);
			}
			sprintf(serialnumber1, "%s",sku);
			sprintf(sequence_table, "other_sim_seq");
			sprintf(country_cstr, "%s",sku);
		}
            	do_log(serialnumber1);
            	do_log(sequence_table);
            	do_log(country_cstr);
	    	char vendor[128];
	    	char getvendor[1024];
	    	sprintf(getvendor,"select sim_sku_vendor('%s');",sku);
	    	do_log(getvendor);
	    	db->ExecTuplesOk(getvendor);
	    	sprintf(vendor, "%s", db->get_field(0, "sim_sku_vendor"));
	    	do_log(vendor);

	    	char country[1024];
            	char getcountry[1024];
            	sprintf(getcountry,"select sim_sku_country('%s');",sku);
            	do_log(getcountry);
	    	db->ExecTuplesOk(getcountry);
            	sprintf(country, "%s", db->get_field(0, "sim_sku_country"));
            	do_log(country); 	

            	sprintf(seq_query, "SELECT nextval('%s');", sequence_table);
            	do_log(seq_query);
            	int seq_result = db->ExecTuplesOk(seq_query);

            	if (seq_result >= 1) 
		{
                	sprintf(serialnumber2, "%s", db->get_field(0, "nextval"));
			do_log("next value is : ");
                	do_log(serialnumber2);

			// ** Check for existing sim number or serial number **
        		int suffix = 0;
        		char unique_order_no[1024];
        		sprintf(unique_order_no, "%s", order_no);

        		while (1) 
			{
           			// Query to check if the order number exists in `sim_stock`
            			sprintf(command, "SELECT COUNT(*) FROM sim_stock WHERE simno = '%s';", unique_order_no);
            			do_log(command);
            			int order_exists = db->ExecTuplesOk(command);
            			int count = atoi(db->get_field(0, "count"));

            			if (count == 0) {
                			// No existing order found, unique number confirmed
                			break;
            			} 
				else 
				{
                			// Order number already exists, append suffix and try again
                			suffix++;
                			sprintf(unique_order_no, "%s_%d", order_no, suffix);
                			do_log("Order number already exists, trying: ");
                			do_log(unique_order_no);
            			}
        		}




                	sprintf(simstock_query, "INSERT INTO sim_stock (simno, sim_phone_no, srno, availability, country, vendor, entry_by_user, dateandtime, active, autobook, activation_reqd) VALUES ('%s', '%s', '%s-%s', false, '%s', '%s','services', 'now', true, false, true);", unique_order_no,unique_order_no, serialnumber1, serialnumber2, country, vendor);
			do_log(simstock_query);
                	int simstock_query_result = db->ExecCommandOk(simstock_query);
			sprintf(command,"value after execution is %d",simstock_query_result);
			do_log(command);
                	if (simstock_query_result >0) 
			{
				db->ExecCommandOk("commit work");
                    		stock_entry_done = 1;
				sprintf(unique_order_no_out, "%s", unique_order_no);
                	}
            	}
    	//}



	sprintf(command,"process stock entry returned %d",stock_entry_done);
	do_log(command);
    	do_log("---------------ending the process_stock_entry function-------------");
    	return stock_entry_done;
}


int register_tsim_client(char* role,char *serial_no,char *sku,char *c_name,char *c_phone,char *company,char *email,char *order_type,char *username,char *domainname,char *c_add1,char *c_add2,char *c_add_city,char *c_add_state,char *c_add_country,char *c_pincode,char *shipping_methods,char *dd_add1,char *dd_add2,char *dd_city,char *dd_state,char *dd_country,char *dd_pincode,char *c_start_date, char *c_activation_date,char *c_payment_mode,char *order_no,char *status,char *total,char *sim_phone_no,char *c_case_sourse,char *sk,char *payment_id,char *cr_to_add,char *total_tax,char *max_from_date,char *max_to_date,char *country_specific,char *c_duration,char *c_stdcode,char *gift_item,char *delivery_method,fax_database *db,char *l_cname,char *l_cphone,char* is_kyc_required,char* new_username)
{//sku in this block
	do_log("----------------------Starting register_tsim_client Function---------------------------");
	char *c_sim_phone_no,*simno,*srno,*seqno;
	char tripid[1024],pwd[1024],command[1024],tariff[1024],newdomain[201],mailto[1024],pack[1024],msg[1024],subject[1024];
	int flag_max,flag_link = 1;
	bool send_sms_flag = 1;
	sprintf(command,"l_cname is %s",l_cname);
	do_log(command);
	sprintf(command,"l_cphone is %s",l_cphone);
	do_log(command);
    char shipping_link[2048]="http://mail.tsim.in/tsim/Shiprocket_shipping_service.php/?";
	char bluedart_shipping_link[2048]="http://mail.tsim.in/tsim/CallAwbService.php/?";
	char shipping_link_det[2048];
	char bluedart_shipping_link_det[2048];
	char client_det[2048];
	char b_client_det[2048];
	do_log("client name");
	do_log(c_name);
	do_log("company name");
	do_log(company);
	do_log("phone no");
	do_log(c_phone);
	do_log("Below is new username variable");
	if (new_username == NULL) {
 	   do_log("new_username is NULL");
	} else if (strlen(new_username) == 0) {
	    do_log("new_username is an empty string");
	} else {
	    do_log(new_username);
	}
	char shipping_add[2048];
	char b_shipping_add[2048];
	fax_database db_r(E2FAXDB,DBSERV);
	fax_database db_w(E2FAXDB,DBSERV);
	do_log("Shipping Method");
	do_log(delivery_method);
	do_log("payment id");
	do_log(payment_id);
	do_log("sku");
	do_log(sku);
	do_log("Contact No");
	do_log(c_phone);
	do_log("is kyc required field: ");
	do_log(is_kyc_required);
	do_log("serial no");
	do_log(serial_no);
	char kkk[150];
	strcpy(kkk,sku);
	char *c_country;
	char unique_order_no[1024];
	int stock_entry_done=0;
	do_log("*******************mapping sku for No Process*************************");
	sprintf(command,"select sku from sku_not_to_map where sku = '%s'",sku);
	do_log(command);
	int ssss = db->ExecTuplesOk(command);
	sprintf(command,"Sku %s is %d",sku,ssss);
	do_log(command);
	if(ssss>=1)
	{
		sprintf(command,"Order no %s not to be processed as SKU is %s",order_no,sku);
		do_log(command);	
		sprintf(subject,"Order no %s not Booked",order_no);
		do_log(subject);
		//send_mail("hoshang@staff.ownmail.com",command,subject);
		send_mail("husain@staff.ownmail.com",command,subject);
		return 0;	
	}
	sprintf(command,"Role is %s",role);
	do_log(command);
	if(strstr(role,"reseller_customer"))
	{
		do_log("Role is Reseller");
		sprintf(command,"select sim_phone_no,simno,srno,country from sim_stock where simno = '%s' and availability='t' and active='t' and autobook ='f' and sim_phone_no not in (select sim_phone_no from clienttrip where status!='Canceled') and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by dateandtime limit 1",serial_no);
		do_log(command);
		if(db->ExecTuplesOk(command))
		{
			do_log("Number exists");
			c_sim_phone_no = db->get_field(0,"sim_phone_no");
			simno = db->get_field(0,"simno");
			srno = db->get_field(0,"srno");
			c_country = db->get_field(0,"country");
			c_sim_phone_no = strdupa(c_sim_phone_no);
			simno = strdupa(simno);
			srno = strdupa(srno);
			c_country = strdupa(c_country);
			if((c_sim_phone_no==NULL)||(c_sim_phone_no[0]==0))
			{	
				stock_entry_done = process_stock_entry(sku, order_no,db,unique_order_no);
				if(stock_entry_done == 1){

					do_log("value is 1");
					do_log("Below is the unique order no outside the process stock function");
					do_log(unique_order_no);
					c_sim_phone_no = (char*)malloc(strlen(unique_order_no) + 1);  // +1 for the null terminator
					if (c_sim_phone_no == NULL) {
						do_log("Memory allocation failed");
						return 1;
					}
					do_log("copying order_no to c_sim_phone_no");
					//strcpy(c_sim_phone_no, order_no);
					strcpy(c_sim_phone_no, unique_order_no);
				}
				else{
					c_sim_phone_no="";
					send_sms_flag = 0;
					sprintf(msg,"Order %s booking failed. Don't have %s number available in stock. Do the NeedFul",order_no,c_country);
					int result = reg_process_fail(order_no,msg);
					db->ExecCommandOk("rollback work");
					return 0;
				}
			}       
			else    
			{       
				do_log("Got a fresh number");
				c_sim_phone_no = strdup(c_sim_phone_no);
				simno = strdup(simno);
				srno= strdup(srno);
				do_log(c_sim_phone_no);
				do_log(simno);
				do_log(srno);
			}		
		}
		else
		{
			stock_entry_done = process_stock_entry(sku, order_no,db,unique_order_no);
			if(stock_entry_done == 1){
				do_log("value is 1");
				do_log("Below is the unique order no outside the process stock function");
                                do_log(unique_order_no);
				c_sim_phone_no = (char*)malloc(strlen(unique_order_no) + 1);  // +1 for the null terminator
				if (c_sim_phone_no == NULL) {
					do_log("Memory allocation failed");
					return 1;
				}
				do_log("copying order_no to c_sim_phone_no");
				//strcpy(c_sim_phone_no, order_no);
				 strcpy(c_sim_phone_no, unique_order_no);

			}
			else{
				c_sim_phone_no="";
				sprintf(msg,"Order %s booking failed. Don't have sim_no for serial no %s available in stock. Do the NeedFul",order_no,serial_no);
				int result = reg_process_fail(order_no,msg);
				db->ExecCommandOk("rollback work");
				return 0;
			}
		}
	}
	else if(strstr(role,"customer") && !strstr(role,"reseller_customer"))
	{
		do_log("Role is Customer");
		do_log("Mapping the countryname from config file");		
		char getsku[1024];

		/*sprintf(getsku,"SKU_COUNTRY_MAP_%s",kkk);
		char* data19 = get_simmis_data_from_table(getsku,"");	
		char* data19;	
		sprintf(getsku,"select sim_sku_country('%s')",kkk);
		do_log(getsku);
		if (db->ExecTuplesOk(getsku))
	        {
        	    	data19 =  db->get_field(0,"value");
	        }
		do_log(data19);*/

		
		char* data19 = nullptr;  // Initialize the pointer
		sprintf(getsku, "select sim_sku_country('%s')", kkk);
		do_log(getsku);

		if (db->ExecTuplesOk(getsku))
		{
		    data19 = db->get_field(0, "sim_sku_country");
		    if (data19 != nullptr)  // Check if data19 is valid
		    {
		        do_log(data19);
		    }
		    else
		    {
		        do_log("No data returned or value is null");
		    }
		}
		else
		{
		    do_log("Query execution failed");
		}





		if(data19[0]==0 || data19==NULL)
		{
			do_log("could not get config file data from table.data19");
			send_sms_flag = 0;
			sprintf(msg,"Hello,\nOrder %s  booking Failed As SKU- %s mapping Failed. Do the NeedFul.\n\nRegards,\nTSIM Team.",order_no,kkk);
			int result = reg_process_fail(order_no,msg);
			return 0;
		}
		strcpy(sku,data19);
		c_country=sku;
		do_log(c_country);

		
		do_log("selecting the sim_phone_no based on the country travelling");
		do_log("get a fresh sim_phone_no for new user");
	
			
		sprintf(command,"select * from sim_stock where country  in (select countryname from countrydata where country='%s' and active = true)  and availability='t' and active='t' and autobook ='t' and sim_phone_no not in (select sim_phone_no from clienttrip where status!='Canceled') and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by dateandtime asc limit 1",c_country);
		do_log(command);
		int num_rows = db->ExecTuplesOk(command);
		if(num_rows > 0)
		{
			c_sim_phone_no = db->get_field(0,"sim_phone_no");
			simno = db->get_field(0,"simno");
			srno = db->get_field(0,"srno");
			if((c_sim_phone_no==NULL)||(c_sim_phone_no[0]==0))
			{
				stock_entry_done = process_stock_entry(sku, order_no,db,unique_order_no);
				if(stock_entry_done == 1){
					do_log("value is 1");
					do_log("Below is the unique order no outside the process stock function");
                                        do_log(unique_order_no);
					c_sim_phone_no = (char*)malloc(strlen(unique_order_no) + 1);  // +1 for the null terminator
					if (c_sim_phone_no == NULL) {
						do_log("Memory allocation failed");
						return 1;
					}
					do_log("copy above");
					//strcpy(c_sim_phone_no, order_no);
					 strcpy(c_sim_phone_no, unique_order_no);

				}
				else{
					/*
					c_sim_phone_no="";
					send_sms_flag = 0;
					sprintf(msg,"Order %s booking failed. Don't have %s number available in stock. Do the NeedFul",order_no,c_country);
					int result = reg_process_fail(order_no,msg);
					db->ExecCommandOk("rollback work");
					return 0;
					*/
					do_log("Came in else part line number 4347");

				}
			}
			else
			{
				do_log("Got a fresh number");
				c_sim_phone_no = strdup(c_sim_phone_no);
				simno = strdup(simno);
				srno= strdup(srno);
				do_log(c_sim_phone_no);
				do_log(simno);
				do_log(srno);
			}
		}
		else
		{
			sprintf(command,"select * from sim_stock st, clienttrip c where st.country  in (select countryname from countrydata where country='%s' and active = true) and st.availability='t' and st.active='t' and st.autobook ='t' and st.sim_phone_no=c.sim_phone_no and c.status in ('Canceled','Returned') and c.status not in ('Delivered','Delivery pending') and st.sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by return_date",c_country);
			do_log(command);
			int rows = db->ExecTuplesOk(command);
			if(rows > 0 )
			{
				c_sim_phone_no = db->get_field(0,"sim_phone_no");
				simno = db->get_field(0,"simno");
				srno = db->get_field(0,"srno");
				if((c_sim_phone_no==NULL)||(c_sim_phone_no[0]==0))
				{
					stock_entry_done = process_stock_entry(sku, order_no,db,unique_order_no);
					if(stock_entry_done == 1){
						do_log("value is 1");
						do_log("Below is the unique order no outside the process stock function");
   		                                do_log(unique_order_no);
						c_sim_phone_no = (char*)malloc(strlen(unique_order_no) + 1);  // +1 for the null terminator
						if (c_sim_phone_no == NULL) {
							do_log("Memory allocation failed");
							return 1;
						}
						do_log("copy above");
						//strcpy(c_sim_phone_no, order_no);
						 strcpy(c_sim_phone_no, unique_order_no);

					}
					else{
						/*
						sprintf(msg,"Order %s booking failed. Don't have %s number available in stock. Do the NeedFul",order_no,c_country);
						int result = reg_process_fail(order_no,msg);
						db->ExecCommandOk("rollback work");
						return 0;
						*/
						do_log("Came in else part line number 4392");
					}
				}
				else
				{
					do_log("Got a fresh number");
					c_sim_phone_no = strdup(c_sim_phone_no);
					simno = strdup(simno);
					srno= strdup(srno);
					do_log(c_sim_phone_no);
					do_log(simno);
					do_log(srno);
				}
			}
			else
			{
				stock_entry_done = process_stock_entry(sku, order_no,db,unique_order_no);
				if(stock_entry_done == 1){
					do_log("value is 1");
					do_log("Below is the unique order no outside the process stock function");
                                        do_log(unique_order_no);
					c_sim_phone_no = (char*)malloc(strlen(unique_order_no) + 1);  // +1 for the null terminator
					if (c_sim_phone_no == NULL) {
						do_log("Memory allocation failed");
						return 1;
					}
					do_log("copy above");
					//strcpy(c_sim_phone_no, order_no);
					 strcpy(c_sim_phone_no, unique_order_no);
				}
				else{
					/*
					c_sim_phone_no="";
					sprintf(msg,"Order %s booking failed. Don't have %s number available in stock. Do the NeedFul",order_no,c_country);
					int result = reg_process_fail(order_no,msg);
					db->ExecCommandOk("rollback work");
					return 0;
					*/
					do_log("Came in else part line number 4427");
				}
			}
		}


		
	}
	else
	{
		do_log("Not an authorised role to go for auto booking");
		sprintf(msg,"Order %s booking failed. Not authorised role to book the sim card. Do the NeedFul",order_no,c_country);
		int result = reg_process_fail(order_no,msg);
		db->ExecCommandOk("rollback work");
		return 0;
	}
	
	sprintf(command,"insert into sim_phone_no_lock(sim_phone_no,added_on) values('%s',now())",c_sim_phone_no);
	do_log(command);
        if(!db->ExecCommandOk(command)){
    	        do_log("failed to add to sim_phone_no_lock");
        	do_log("rollback work");
		send_sms_flag = 0;
	        sprintf(msg,"Order %s booking failed. Add Sim_Phone_no_lock failed. Do the needFul",order_no);
                int result = reg_process_fail(order_no,msg);
		db->ExecCommandOk("rollback work");
        	return 0;
        }
        do_log("sim_phone_no_lock Added");
	do_log("*************************************Checking is_dd_enabled**************************");
	sprintf(command,"/*NO LOAD BALANCE*/SELECT * from countrydata where countryname='%s'",c_country);
        do_log(command);
        if(!db->ExecTuplesOk(command)){
                do_log("No country from countrydata");
		sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",c_sim_phone_no);
                do_log(command);
                db->ExecCommandOk(command);
                do_log("rollback");
                db->ExecCommandOk("rollback work");
		sprintf(msg,"Order %s booking failed. Country is not created for this ,create this country  %s. Do the needFul",order_no,c_country);
                int result = reg_process_fail(order_no,msg);
                return 0;
        }
	bool is_dd_enabled = 0;
	if (!strcmp(db->get_field(0,"is_dd_enabled"),"1")) is_dd_enabled = 1;
        else is_dd_enabled = 0;
        if (is_dd_enabled) do_log("is_dd_enabled is 1");
        else do_log("is_dd_enabled is 0");
	if(is_dd_enabled){
		sprintf(command,"select regd_number from callerid_user where regd_number='%s';",c_sim_phone_no);
		do_log(command);
		if(db->ExecTuplesOk(command)>0)
			sprintf(command,"update callerid_user set username='%s' where regd_number='%s';",username,c_sim_phone_no);
		else
			sprintf(command,"insert into callerid_user (regd_number,username) values('%s','%s');",c_sim_phone_no,username);
		do_log(command);
		if(db->ExecCommandOk(command)<=0){
			do_log("rollback work");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			db->ExecCommandOk("rollback work");
			//return 0;
		}
	}
	do_log("***********************is_dd_enabled checking finished*********************************");			
	sprintf(command,"update sim_stock set availability=false where sim_phone_no='%s'",c_sim_phone_no);
        do_log(command);
        if(db->ExecCommandOk(command)<=0)
        {
			do_log("Update Failed");
			do_log("rollback work");
			send_sms_flag = 0;
        	sprintf(msg,"Order %s booking failed. Update sim_stock failed. Do the needFul",order_no);
			int result = reg_process_fail(order_no,msg);
			db->ExecCommandOk("rollback work");
			return 0;
        }
	
       
	do_log("fetch the trip_id");
	sprintf(command,"select nextval('trip_ids')");
	do_log(command);
	if(db->ExecTuplesOk(command)>0)
	{       
		seqno = db->get_field(0,"nextval");
		sprintf(tripid,"%s%s",c_country,seqno);
		do_log(tripid);
	}
	else
	{       
		do_log("rollback work");
		send_sms_flag = 0;
		sprintf(msg,"Order %s booking failed.Tripid cannot be generated.Do the needful",order_no);
		int result = reg_process_fail(order_no,msg);
		db->ExecCommandOk("rollback work");
		return 0;
	}
        
		do_log(c_activation_date);
		char *data_c ;
		string data_string;
		data_c = cgi_getentrystr(to_c_string("activation_date"));
		data_string = string(data_c);
		if(data_string.empty()){
			do_log(c_start_date);	
			c_activation_date = NULL;
			do_log("First If ");
		}
		else{
			do_log(c_start_date);
			do_log("Come in Else");
		}
		

		do_log("cr_to_add");     
        do_log(cr_to_add);
		if (c_activation_date == NULL && strncmp(order_no,"TA",2)){
			char empty_activation_date_comment[201];
			char* empty_activation_date_comment_txt = get_simmis_data_from_table("empty_activation_date_comment","The activation date has been set to order date due to missing activation date");
			c_activation_date =  c_start_date;
			strcpy(empty_activation_date_comment,empty_activation_date_comment_txt);
			sprintf(command,"insert into clienttrip (username,domainname,sim_phone_no,from_date,booking_date,comment,status,tripid,tripid_min,payment_mode,mis_user,case_source,order_no,item_cost,is_kyc_required) values('%s','%s','%s','%s','%s','%s, %s','Delivery pending','%s','%s','%s','AUTO','%s','%s','%s',%s)",new_username,domainname,c_sim_phone_no,c_activation_date,c_start_date,gift_item,empty_activation_date_comment,tripid,sku,c_payment_mode,c_case_sourse,order_no,cr_to_add,is_kyc_required);
		}
		else{
			c_activation_date =  (c_activation_date == NULL) ?  c_start_date : c_activation_date;
			sprintf(command,"insert into clienttrip (username,domainname,sim_phone_no,from_date,booking_date,comment,status,tripid,tripid_min,payment_mode,mis_user,case_source,order_no,item_cost,is_kyc_required) values('%s','%s','%s','%s','%s','%s','Delivery pending','%s','%s','%s','AUTO','%s','%s','%s',%s)",new_username,domainname,c_sim_phone_no,c_activation_date,c_start_date,gift_item,tripid,sku,c_payment_mode,c_case_sourse,order_no,cr_to_add,is_kyc_required);
		}
      
        do_log(command);
        int ct=db->ExecCommandOk(command);
	sprintf(command,"ct is %d",ct);
	do_log(command);
        if(ct > 0)
        {       
			do_log("Inserted into client trip");
			do_log("commit work");
			db->ExecCommandOk("commit work");
    			const char *item_type = (strstr(tripid, "esim") != nullptr) ? "Esim" : "physical sim";
			do_log(item_type);
			sprintf(command,"select * from tsim_shipping_details where order_no='%s' and item_type='%s'",order_no,item_type);
/*			
			char base_order_no[20];  // Array to hold the base order number

                                // Check if the order_no contains an underscore and split it
                                char *underscore_pos = strchr(order_no, '_');
                                if (underscore_pos != NULL) {
                                    // Extract the part before the first underscore
                                    strncpy(base_order_no, order_no, underscore_pos - order_no);
                                    base_order_no[underscore_pos - order_no] = '\0';  // Add null terminator
                                } else {
                                    // No underscore found, use the entire order_no
                                    strcpy(base_order_no, order_no);
                                }


			sprintf(command,"select * from tsim_shipping_details where order_no = '%s' and item_type='%s'",base_order_no,item_type);*/
			do_log(command);
			int llll = db->ExecTuplesOk(command);
			sprintf(command,"no of shipping link is %d",llll);
			do_log(command);	
			if(llll <= 0)
			{
				if(strstr(role,"customer") && !strstr(role,"reseller_customer"))
				{	
					do_log("Shipping Link");
					do_log(shipping_link);
					do_log("Bluedart Shipping Link");
					do_log(bluedart_shipping_link);
					sprintf(b_client_det,"c_name=%s&c_mob=%s&",urlencode(c_name),c_phone);
									do_log("bluedart client det");
					do_log(b_client_det);
					sprintf(b_shipping_add,"c_add1=%s&c_add2=%s&city=%s&state=%s&pin=%s",urlencode(dd_add1),urlencode(dd_add2),urlencode(dd_city),dd_state,dd_pincode);
					do_log("bluedart shipping address");
					do_log(b_shipping_add);
					strcat(b_client_det,b_shipping_add);
					do_log("appended bluedart client_det");
									do_log(b_client_det);
					sprintf(bluedart_shipping_link_det,"&crn=%sA&t=%s&o=%s&d=%s&p=%s&g=%s&f=%s&amt=%s&",order_no,urlencode(tripid),order_no,urlencode(delivery_method),payment_id,urlencode(gift_item),"false",total);
					do_log(bluedart_shipping_link_det);			
					strcat(bluedart_shipping_link_det,b_client_det);
					strcat(bluedart_shipping_link,bluedart_shipping_link_det);
					do_log(bluedart_shipping_link);
					//***************************
					strcat(shipping_link,bluedart_shipping_link_det);
									do_log(shipping_link);
					do_log("****inserting into tsim_shipping_details table***** *************************** Why this");
					sprintf(command,"INSERT into tsim_shipping_details (tripid,order_no,shipping_methods,shipping_link,payment_method,gift_item,bluedart_waybill,item_type) values('%s','%s','%s','%s','%s','%s','%s','%s')",tripid,order_no,delivery_method,shipping_link,payment_id,gift_item,bluedart_shipping_link,item_type);

					do_log(command);
					if(!db->ExecCommandOk(command))
						do_log("Failed to insert into tsim_shipping_details");
					else
						do_log("Inserted successfully");
				}
				else
				{
					do_log("No need to create shippikng link");
				}
			}
        }
        else
        {       
			do_log("Failed to insert");
		  	do_log("rollback work");
		  	send_sms_flag = 0;
			sprintf(msg,"Order %s booking failed.Clienttrip entry failed. Do the NeedFul",order_no);
			int result = reg_process_fail(order_no,msg);	
			db->ExecCommandOk("rollback work");
			return 0;
        }
	/**************************Send SMS to the Customer For his Successfull Bokking************************/
	if (send_sms_flag)
	{
       		
                char donotsend[9999];

		
		   char* data20 = get_simmis_data_from_table("SMS_FILTER_donotsend","");
			if(data20[0]==0 || data20==NULL){
			do_log("CANNOT GET THE SMS_FILTER NUMBERS data20");}
			strcpy(donotsend,data20);





                //else if (!strstr(donotsend,mobile_number))
                if (!strstr(donotsend,c_phone))
		{
                	sprintf(command,"select * from salesperson where mobileno='%s'",c_phone);
                	if(!db->ExecTuplesOk(command))
			{
                		do_log("sending sms to India number");
				sprintf(command,"Your order %s with TSIM  has been booked. Please save the sender (+%s) as TSIM, and note that this number is also available on Whatsapp. You can upload your documents using this link - https://mail.tsim.in/docs.php?o=%s",order_no,SMS_FROM,order_no);
                		do_log(command);
                		int res=0;
				res = create_sms_file(command,c_phone);
                		if (res==1) do_log("successfully sent SMS");
				else
					do_log("Falied to send sms");
			}
		}
	}
	/******************************Send Mail to customer code no  longer needed************************************/
	/*******************************Adding credit in users on successfull booking and user creation************************/
        sprintf(command,"select credit-usage as balance from users where username='%s' and domainname='%s'",username,domainname);
	do_log(command);
	int bb = db->ExecTuplesOk(command);
	if(bb == 0)
	{
		char app_cred[3];
		double dbl_app_cred = 0;
		char* data21 = get_simmis_data_from_table("APP_PROMO_on_order","500");
		strcpy(app_cred,data21);
		dbl_app_cred = atof(app_cred);
		char narr[21];
		sprintf(narr,"Promo Credit for App");
		char senderr[21];
		sprintf(senderr,"Promo Credit");
		int res = addCredit(username,domainname,narr,dbl_app_cred,senderr,"USD",1,1);
        }
	else
	{
		char narr[21];
		sprintf(narr,"Promo Credit for App");
		char senderr[21];
		sprintf(senderr,"Promo Credit");
		char *user_bal = db->get_field(0,"balance");
		do_log(user_bal);
		float u_bal = atof(user_bal);
		sprintf(command,"U_bal is %f",u_bal);
		do_log(command);
		double dbl_app_cred = 500 - u_bal;
		sprintf(command,"Amount to be credited is %f",dbl_app_cred);
		do_log(command);
		int res = addCredit(username,domainname,narr,dbl_app_cred,senderr,"INR",1,1);
	}
	/***********************Code to check the unliited pack opted or not***********************************/

	max_from_date=strdupa(max_from_date);
	max_to_date=strdupa(max_to_date);
	char *stdc;
	stdc = new char[251];
	char query[1024];
	sprintf(query,"SKU_STD_CODE_%s",kkk);
	char* data22 = get_simmis_data_from_table(query,"");
	if(data22[0]==0 || data22==NULL){
		do_log("Could not get config data22.");
		do_log("Might be sku not present in table");
		printf("Error occured while getting config table data.");
	}
	strcpy(stdc,data22);
	do_log("Line No-4139");
	do_log(stdc);
	stdc = trimwhitespace(stdc);
	do_log("******************* * ////STDC// ***************");
	do_log(stdc);

	if(strstr(kkk, "_91") != NULL or strstr(kkk,"91_Data")!=NULL or stdc[0] != '\0' )
	{ 
		do_log("inserting into max_reduction_code table");
		do_log("Begin work");
		db->ExecCommandOk("begin work");
	        sprintf(command,"select * from max_reduction_codes where tripid ='%s' and from_date='%s'",tripid,max_from_date);
        	do_log(command);
	        int r = db->ExecTuplesOk(command);
        	char buffer[1024];
	        sprintf (buffer, "cnt is  %d", r);
        	do_log(buffer);
	        if(r<=0)
        	{
		    do_log("c_stdcode");
		    do_log(c_stdcode);
		   	 if(stdc[0] != '\0' && strcmp(c_stdcode,stdc)== 0)
			 {
			do_log("Its From SKU ");
			
                              strcpy(pack,"Voice and Data Both");
                              sprintf(command,"insert into max_reduction_codes values('%s','%s','%s','%s','%s','%s')",username,domainname,tripid,stdc,max_from_date,max_to_date);
                              do_log(command);	
  		         }
		else
        	    if(strcmp(c_stdcode,"91_Data")== 0)
                    {
                              do_log("in 91_data");
                     	      strcpy(pack,"Voice and Data Both");
	                      sprintf(command,"insert into max_reduction_codes values('%s','%s','%s','91_Data','%s','%s')",username,domainname,tripid,max_from_date,max_to_date);
        	              do_log(command);
                    }
		    else if(strcmp(c_stdcode,"91") == 0)
                    {
                            do_log("in 91");
                            strcpy(pack,"Voice only");
                            sprintf(command,"insert into max_reduction_codes values('%s','%s','%s','91','%s','%s')",username,domainname,tripid,max_from_date,max_to_date);
	                    do_log(command);
                    }
                    else
                    {
                              	 do_log("not ellegible for unlimited calling");
			      	flag_max = 1;
                    }
                    if(db->ExecCommandOk(command)<=0)
                    {
                       		flag_max = 0;
                       		do_log("Insert in max_reduction failed.");
                      		do_log("rollback work");
                       		sprintf(msg,"Hello,\nOrder No %s is Booked.Adding of UNLIMITTED PACK is failed to Username %s for tripid  %s . Please Add the Pack with the following details.\n Duration  '%s' \n. Pack  '%s'",order_no,username, tripid, c_duration, pack);
				int result = reg_process_fail(order_no,msg);
				db->ExecCommandOk("rollback work");
				return 0;
                    }
		    else
		    {
		        flag_max++;
		 	do_log("commit work");
			db->ExecCommandOk("commit work;");
		    }
        	}
        	else
        	{
                 	do_log("Record already present in max_reduction_code");
        	}
	}
	else
	{
		do_log("Not Elligible for unlimited pack. No entry in max_reduction");
	}	
	/**/
	do_log("Payment mode-");
	do_log(c_payment_mode);
        /**********************creating the payment card link*********************/
        if(strstr(c_payment_mode, "PAD")!= NULL)
	{
	        do_log("Payment Link to Be created");
		char *link_amt = cgi_getentrystr("link_amt");
	      	char *subtotal = cgi_getentrystr("subtotal");
                subtotal = strdup(subtotal);
                do_log("subtotal");
                do_log(subtotal);
		//char *item_cost= cgi_getentrystr("subtotal");  
	      	char *item_cost;  
	      	char *total  = cgi_getentrystr("amt");
		link_amt = strdup(link_amt);
		//item_cost = strdup(item_cost);
		total = strdup(total);	
		//char *subtotal = cgi_getentrystr("subtotal");  
		do_log("Total Amount");
		do_log(total);
		do_log("Link Amt");
		do_log(link_amt);
		//do_log("item_cost");
		//do_log(item_cost);
		struct timeval tv1;
		float exch_rate;
		//flag_link++;
		do_log("add e credit for global card");
        	char e_amount[21];
	        agentusername = strdupa("trikon");
        	psswd = strdupa("k1n065qnone");
	        gsim GSIM;
        	int res = GSIM.init_gsim_with_sim_phone_no(db,c_sim_phone_no);
	        if (!res)
        	{
                	do_log("failed to init_gsim_with_sim_phone_no");
			//sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,c_sim_phone_no);
			sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s Amount %s \nTSIM No %s",order_no,username,link_amt,c_sim_phone_no);
			int result = reg_process_fail(order_no,msg);	
			return 0;
        	}
	        sprintf(command,"select * from countrydata  where countryname like '%%%s%%'",c_country);
        	do_log(command);
	        char api_code[20];
        	bool add_e_credit_entry_on_booking = 0;
	        if(db->ExecTuplesOk(command))
        	{
                	//do_log("here");
	                strcpy(api_code,db->get_field(0,"api_code"));
        	}else
                	do_log("no result");
	        do_log(api_code);
       		if (!strcmp(db->get_field(0,"add_e_credit_entry_on_booking"),"1"))
	             add_e_credit_entry_on_booking = 1;
	        else
         	    add_e_credit_entry_on_booking = 0;


		 char globalcountry[1024];
                sprintf(globalcountry,"GLOBAL_E_CREDIT_%s",c_country);
                char* data23 = get_simmis_data_from_table(globalcountry,"58.9");
                strcpy(e_amount,data23);





		do_log("e_amount");
		do_log(e_amount);
		if (add_e_credit_entry_on_booking )
       		{
	               do_log("e credit entry is 1");
        	      // res = GSIM.add_e_credit(atof(e_amount),0,api_code,c_sim_phone_no);
       		}
	        else
       		{
	               do_log("e credit entry is 0");
		      // res = GSIM.debit_from_prepaid(atof(e_amount),api_code);
       		}
		/*if(!res)
		{ 
			sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,c_sim_phone_no);
			int result =  reg_process_fail(order_no,msg);  
			return 0;	
		}
		/****************************No Longer Needed*************************************/
		char *cartid=NULL;
		char *st_per_val=NULL;
		char *surcharge_val=NULL;
		char *st_curr=NULL;
		float service_tax_percent;
		float surcharge;
		float service_tax;
		char *stax_narr;
		char *sur_chg_narr;	
		do_log("Begin work");
		db->ExecCommandOk("begin work");
		sprintf(command,"select cartid from global_sim_cc_payer_det  where order_no = '%s'",order_no);
                do_log(command);
		if(db->ExecTuplesOk(command))
		{
			 cartid=db->get_field(0,"cartid");
			 do_log(cartid);
			 cartid=strdup(cartid);
		}
		else	
		{
		        do_log("no result new");
		}
		do_log("cart id");
		do_log(cartid);
		if (cartid == NULL || cartid[0] == 0)					
		{
		  /*	time_t now;
			time(&now);
			struct tm* now_tm;
			now_tm = localtime(&now);
			char cartid[80];
			strftime (cartid, 80, "%Y%m%d%H%m%s", now_tm);
			do_log(cartid);
			sprintf(command,"select rate from exchange_rate order by date desc limit 1");
			do_log(command);
			int mm = db->ExecTuplesOk(command);
			char buffer[1024];
			sprintf (buffer, "cnt is  %d", mm);
			do_log(buffer);
			char *exch;
			if(mm>0)
			{
				do_log("inside if of er");
				exch=db->get_field(0,"rate");
			}
			do_log(exch);
			exch_rate = atof(exch);
			exch_rate = roundf(exch_rate * 100) / 100;
			sprintf(buffer,"Exch_rate is %f",exch_rate);
			do_log(buffer);	*/
			char buffer[1024];
			cartid=order_no;
			float ftemp = atof(link_amt);
			sprintf(buffer,"Link Amount is %f  Rs.",ftemp);
			do_log(buffer);
				
			/*float amt_p=(ftemp/115)*100/exch_rate;
			sprintf(buffer,"Link Amount is %f  US$",amt_p);
			do_log(buffer);
			float nnn = (amt_p * 100) / 100;
			char buff[100];
			sprintf(buff,"%.3f",nnn);
			do_log(buff);*/
			/***************************************GST implementation***********************************/

			do_log("************************GST Calculations from the database***********************************");
			sprintf(command,"SELECT * FROM billing_config where code='100' and datefrom<=now() and dateto>=now()");
                        do_log(command);
                        if(db->ExecTuplesOk(command)>0){
				do_log("inside if of 100");
                                st_per_val=db->get_field(0, "value");
                                stax_narr = db->get_field(0, "descr");
                                if (stax_narr == NULL || stax_narr[0] == 0) stax_narr = strdupa("Service Tax");
                                else stax_narr = strdupa(stax_narr);
                        }
                        else{
				do_log("inside else of 100");
                                st_per_val=db->get_field(0, "value");
                                st_per_val=strdup("14");
                                stax_narr = strdupa("Service Tax");
                        }
                        service_tax_percent = atof(st_per_val);
			sprintf(command,"service_tax_percent is %f",service_tax_percent);
			do_log(command);
                        sprintf(command,"SELECT * FROM billing_config where code='101' and datefrom<=now() and dateto>=now();");
                        do_log(command);
                        if (db->ExecTuplesOk(command)>0){
				do_log("inside if of st");
                                surcharge_val=db->get_field(0, "value");
                                sur_chg_narr = db->get_field(0, "descr");
                                if (sur_chg_narr == NULL || sur_chg_narr[0] == 0) sur_chg_narr = strdupa("Education Cess and surcharge");
                                else sur_chg_narr = strdupa(sur_chg_narr);
                        }
                        else{
				do_log("inside else of st");
                                surcharge_val=strdup("0");
                                sur_chg_narr = strdupa("Education Cess and surcharge");
                        }
                        surcharge = atof(surcharge_val);
			sprintf(command,"Surcharge to be added to the tax is %f",surcharge);
			do_log(command);
                        service_tax_percent = service_tax_percent + ((surcharge / 100) * service_tax_percent);
			sprintf(command,"Total Tax to be levied is %f",service_tax_percent);
			do_log(command);

			item_cost = link_amt;
			item_cost = strdup(item_cost);
			do_log(item_cost);

			do_log("CHANGES I MADE BELOW THIS ******************************");
			float amount_without_gst = atof(link_amt)/(1 + (service_tax_percent)/100);
			sprintf(command,"amount_without_gst is %f",amount_without_gst);
			do_log(command);	

//			float item_cost_with_gst = round(((atof(item_cost) * service_tax_percent)/100) + atof(item_cost));
//                        sprintf(command,"Item cost including gst is %f",item_cost_with_gst);
			 sprintf(command,"Item cost including gst is %s",item_cost);
			do_log(command);
			float val_in_cal = 100 + service_tax_percent;
			sprintf(command,"Value to be taken in calculation is %f",val_in_cal);
			do_log(command);	
			sprintf(command,"Link Amount is %f  Rs.",ftemp);
			do_log(command);

/*			float link_amount = (ftemp/val_in_cal)*100/exch_rate;
			sprintf(command,"Link Amount is %f  US$",link_amount);
			do_log(command);
			float link_amount_dollor = (link_amount * 100) / 100;
			sprintf(command,"Link Amount is %f  US$",link_amount_dollor);
			do_log(command);
			char buff[100];
			sprintf(buff,"%.3f",link_amount_dollor);
			do_log(buff);*/

			do_log("******************************GST Calculation ends here*************************************");	
			/********************************************************************************************************************************/
//			sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no,username,domainname) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%s','pending','%s',1,mobno,'%s','%s','tsim.in' from sim_user where username ='%s');",c_sim_phone_no,item_cost,cartid,order_no,username,username);
			
			 sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no,username,domainname) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%.3f','pending','%s',1,mobno,'%s','%s','%s' from sim_user where username ='%s');",c_sim_phone_no,amount_without_gst,cartid,order_no,username,domainname,username);


			do_log(command);
			int mk = db->ExecCommandOk(command);
			sprintf (buffer, "mk is  %d", mk);
			do_log(buffer);
			char *nextval = NULL;
			struct timeval tv1;
			gettimeofday(&tv1, 0);
			if(mk>0)
			{
				do_log("link created successfully");
//				sprintf(command,"select pack_cost from tsim_pack_details where pack_name = '%s'",kkk);

/*				char base_order_no[20];  // Array to hold the base order number

				// Check if the order_no contains an underscore and split it
				char *underscore_pos = strchr(order_no, '_');
				if (underscore_pos != NULL) {
				    // Extract the part before the first underscore
				    strncpy(base_order_no, order_no, underscore_pos - order_no);
				    base_order_no[underscore_pos - order_no] = '\0';  // Add null terminator
				} else {
				    // No underscore found, use the entire order_no
				    strcpy(base_order_no, order_no);
				}

				sprintf(command,"select round(price+total_tax/quantity) as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",base_order_no,kkk);*/

				sprintf(command,"select subtotal+ subtotal_tax as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",order_no,kkk);				
				do_log(command);
				int jj = db->ExecTuplesOk(command);
				if(jj>0)
				{		
					item_cost = db->get_field(0,"price");
					item_cost = strdup(item_cost);
					//if (!item_cost || (item_cost == NULL))
					if (item_cost == NULL || item_cost[0]== 0)
					{
						do_log("Pack Details not Present");
						//item_cost = cgi_getentrystr("subtotal");
						//strcpy(item_cost,subtotal);
						strcpy(item_cost,total);
						do_log("item_cost");
						do_log(item_cost);	
					}
					else
					{
						do_log("Pack Details  present");
						do_log("pack_cost");    
						do_log(item_cost);	
					}	
				}
				else
				{
					do_log("No Such Plan Available");
					sprintf(msg,"Order no %s has booked in MIS. No %s Plan Available. Do the Needful.\nUsername %s \nOrder_no %s \nTSIM No %s",order_no,kkk,username,order_no,c_sim_phone_no);
					int result = reg_process_fail(order_no,msg);
					return 0;
				}
				item_cost = strdup(item_cost);
				do_log("Item_cost 1");
				do_log(item_cost);
/*				float item_cost_with_gst = round(((atof(item_cost) * service_tax_percent)/100) + atof(item_cost));
                        	sprintf(command,"Item cost including gst is %f",item_cost_with_gst);*/
				sprintf(command,"Item cost including gst is %s",item_cost);
//				sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%d') ;",cartid,c_sim_phone_no,(int)item_cost_with_gst);
				sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%s') ;",cartid,c_sim_phone_no,item_cost);
				do_log(command);
				int hk = db->ExecCommandOk(command);
				sprintf (buffer, "hk is  %d", hk);
				do_log(buffer);
				if(hk > 0)
				{
					do_log("Inserted Successfully in global_sim_cc_sim_amount_det");
				} 
				else
				{
					do_log("Not Inserted in global_sim_cc_sim_amount_det");
				}	   
			}
			else
			{
				do_log("Could Not Create Payment Link. Do the Needful");
				do_log("Rollback work");
				sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,c_sim_phone_no);
				db->ExecCommandOk("rollback work");
				int result = reg_process_fail(order_no,msg);
				return 0;	 	
				/***************************************No Longer Needed***************************/
			}
			//Added to create payment link for RazorPay
			//char* payment_link = get_simmis_data_from_table("payment_link","");
			char* payment_link = get_simmis_data_from_table("payment_link","");
			if (payment_link == NULL) {
				do_log("Failed to retrieve payment_link");
				return;
			}

			
			do_log(command);
			char link[1024];
			do_log("Client Mail for Payment Link");
			do_log(email);	
			char link_mail[1024]="services@tsim.in,";
			strcat(link_mail,email);	
			do_log(link_mail);
			//payment_link=strdupa(payment_link);
			//sprintf(command,"payment_link -%s-",payment_link);
			// Sanitize the payment_link to avoid format string vulnerabilities
			char sanitized_payment_link[1024];
			snprintf(sanitized_payment_link, sizeof(sanitized_payment_link), "%s", payment_link);

			// Safely format the command
			//sprintf(command, "payment_link -%s-", sanitized_payment_link);
			do_log(sanitized_payment_link);
			if (!strcmp(sanitized_payment_link,"razorpay")){
				sprintf(command,"select clientname,mobno,emailadd from sim_user where username ='%s';",username);
				do_log(command);
				int re=db->ExecTuplesOk(command);
				if (!re){
					do_log("Failed to get cust data");
					do_log("Could Not Create Payment Link. Do the Needful");
					do_log("Rollback work");
					sprintf(msg,"Order no %s has booked in MIS. Creating Payment link failed. Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,c_sim_phone_no);
					db->ExecCommandOk("rollback work");
					int result = reg_process_fail(order_no,msg);
					return 0;	 	
				}
				char *cname=db->get_field(0,"clientname");
				char *mobno=db->get_field(0,"mobno");
				char *emailadd=db->get_field(0,"emailadd");
				if (cname == NULL || cname[0] ==0) cname=strdupa("NA");
				else cname=strdupa(cname);
				if (mobno== NULL || mobno[0] ==0) mobno=strdupa("NA");
				else mobno=strdupa(mobno);
				if (emailadd== NULL || emailadd[0] ==0) emailadd=strdupa("NA");
				else emailadd=strdupa(emailadd);
				sprintf(command,"%s %s %s \"%s\" %f %s %s %s %s",PHP_CMD_P,CREATE_RZP_LINK,order_no,cname,round(ftemp),c_sim_phone_no,emailadd,mobno,cartid); 
				do_log("create razorpay payment link");
				do_log(command);
				FILE *fp=popen(command,"r");
				if (!fp){
					do_log("Failed to Create Payment Link. Do the Needful");
					do_log("Rollback work");
					sprintf(msg,"Order no %s has booked in MIS. Creating Payment link failed. Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,c_sim_phone_no);
					db->ExecCommandOk("rollback work");
					int result = reg_process_fail(order_no,msg);
					return 0;	 	
				}
				char line[1024];
				char ress[2048];
				ress[0]='\0';
				// while(fgets(line,sizeof(line),fp)){
				// 	line[strlen(line)-1]='\0';
				// 	if (strlen(ress)==0) strcpy(ress,line);
				// 	else strcat(ress,line);
				// }
				while (fgets(line, sizeof(line), fp)) {
					// Remove the newline character if it exists
					size_t len = strlen(line);
					if (len > 0 && line[len - 1] == '\n') {
						line[len - 1] = '\0';
					}
					if (strlen(ress) == 0) {
						strcpy(ress, line);
					} else {
						strcat(ress, line);
					}
				}
				pclose(fp);
				sprintf(command,"output is %s",ress);
				do_log(command);
				if (!strcmp(ress,"Failed")){
					do_log("Failed to Create Payment Link. Do the Needful");
					do_log("Rollback work");
					sprintf(msg,"Order no %s has booked in MIS. Creating Payment link failed. Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,c_sim_phone_no);
					db->ExecCommandOk("rollback work");
					int result = reg_process_fail(order_no,msg);
					return 0;	 	
				}
				/*
				char link[1024]="https://mail.tsim.in/tsim/slp.php?c=";
				strcat(link, cartid);
				strcat(link, "&a=det_link");
				*/
				strcpy(link,ress);
			}
			else{
				sprintf(link,"https://mail.tsim.in/tsim/slp.php?c=%s&a=det_link",cartid);
			}
			do_log("link is 2");
			do_log(link);
			sprintf(command,"Hello,<br>Mr/Ms. %s,<br><br>Credit card payment link for your order %s is %s <br><br>Kindly do the payment. Wish you a very happy journey.<br><br>Regards,<br>TSIM Team.",c_name,order_no,link);
			do_log(command);
			do_log("commit work");
			db->ExecCommandOk("commit work");
			char subject[1024];
			sprintf(subject,"Client Mail->Tsim Order-%s is placed.(Payment Link)",order_no);	 
			//sprintf(command,"Hello,<br>Mr/Ms. %s, is Registered through TSIM.<br>Order No- %s <br>Clientname- %s <br>Username- %s <br>TSIM No- %s <br>simno- %s <br>Serial no- %s <br>Contact no- %s <br>Email id- %s.<br>Payment link is not sent. Do the needful.",c_name,order_no,c_name,username,c_sim_phone_no,simno,srno,c_phone,email);                       
			do_log(command);
			do_log("inserting values in tsim_order_notification -----------------------------");
			//Mark5i
			char* escapedMessage = escapeForSQL(command);
			do_log("escaped message is :");
			do_log(escapedMessage);
				
			sprintf(query,	"INSERT INTO tsim_order_notifications "
					"(username,order_no,email,mobno,notification_type, "
					"subject_content,message_content,status,priority,from_email) "
					"VALUES('%s','%s','%s','%s','{email}', "
					"'{%s}','{%s}','{email_pending}','normal','services@tsim.in');"
					,c_name,order_no,email,c_phone,subject,escapedMessage);
			do_log(query);
			int query_result = db->ExecCommandOk(query);
			if (query_result > 0)
				do_log("Successfully inserted into tsim_order_notification-----------------------------");
			else 
			{
				do_log("Failed to insert into tsim_order_notification-----------------------------------");	
				do_log("Contact No");
                                  do_log(c_phone);
                                  do_log("failed to send");
                                  sprintf(command,"Hello,\nMr/Ms. %s, is Registered through TSIM.\nOrder No- %s \nClientname- %s \nUsername- %s \nTSIM No- %s \nsimno- %s \nSerial no- %s \nContact no- %s \nEmail id- %s.\nPayment link is   not sent. Do the needful.",c_name,order_no,c_name,username,c_sim_phone_no,simno,srno,c_phone,email);
                                  do_log(command);
                                  send_mail("dhaval@staff.ownmail.com",command,subject);

			//if (!send_mail(link_mail,command,subject))
                	 // {
				do_log("Contact No");
			        do_log(c_phone);
                        	do_log("failed to send");
				sprintf(command,"Hello,\nMr/Ms. %s, is Registered through TSIM.\nOrmder No- %s \nClientname- %s \nUsername- %s \nTSIM No- %s \nsimno- %s \nSerial no- %s \nContact no- %s \nEmail id- %s.\nPayment link is not sent. Do the needful.",c_name,order_no,c_name,username,c_sim_phone_no,simno,srno,c_phone,email);                       
				do_log(command);
                                //send_mail("services@tsim.in",command,subject);
	                  //}
        	         /* else
                	  {
                        	do_log("Mail sent to Client");
				do_log("Contact No");
				sprintf(subject,"Telesales Mail->Tsim Order-%s is placed.(Payment Link)",order_no);
        	                sprintf(command,"Hello,\n\nMr/Ms. %s,\nCredit card payment link for your order %s is %s \n\nKindly do the payment. Wish you a very happy journey.\n\nRegards,\nTSIM Team.",c_name,order_no,link);
				do_log(command);
                                //send_mail("hoshang@staff.ownmail.com",command,subject);
                                //send_mail("husain@staff.ownmail.com",command,subject);
                                //send_mail(link_mail,command,subject);
                	  }*/
			}
		      delete[] escapedMessage;
	        }	
		else
		{
			do_log("Cart id for existing order is present");
//        		sprintf(command,"select  pack_cost from tsim_pack_details where pack_name = '%s'",kkk);
		
/*			char base_order_no[20];  // Array to hold the base order number
                                
                                // Check if the order_no contains an underscore and split it
                                char *underscore_pos = strchr(order_no, '_');
                                if (underscore_pos != NULL) {
                                    // Extract the part before the first underscore
                                    strncpy(base_order_no, order_no, underscore_pos - order_no);
                                    base_order_no[underscore_pos - order_no] = '\0';  // Add null terminator
                                } else {
                                    // No underscore found, use the entire order_no
                                    strcpy(base_order_no, order_no);
                                }
	
			sprintf(command,"select round(price+total_tax/quantity) as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",base_order_no,kkk);*/


			sprintf(command,"select subtotal+ subtotal_tax as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",order_no,kkk);
	                do_log(command);
			int jj = db->ExecTuplesOk(command);
			if(jj>0)
			{		
				item_cost = db->get_field(0,"price");
				item_cost = strdup(item_cost);
				//if (!item_cost || (item_cost == NULL))
				if (item_cost == NULL || item_cost[0]== 0)
				{
					//item_cost = cgi_getentrystr("subtotal");
					item_cost = cgi_getentrystr("total");
					do_log("Pack Details Not present");
					do_log("item_cost");
					do_log(item_cost);	
				}
				else
				{
					do_log("Pack Details Present");
					do_log("pack_cost");    
        	                	do_log(item_cost);	
				}	
			}
			else
			{
				do_log("No Such Plan Available");
				sprintf(msg,"Order no %s has booked in MIS. No %s Plan Available. Do the Needful.\nUsername %s \nOrder_no %s \nTSIM No %s",order_no,kkk,username,order_no,c_sim_phone_no);
	                        int result = reg_process_fail(order_no,msg);
        	                return 0;
			}
			item_cost = strdup(item_cost);

			sprintf(command,"SELECT * FROM billing_config where code='100' and datefrom<=now() and dateto>=now()");
                        do_log(command);
                        if(db->ExecTuplesOk(command)>0){
                                do_log("inside if of 100");
                                st_per_val=db->get_field(0, "value");
                                stax_narr = db->get_field(0, "descr");
                                if (stax_narr == NULL || stax_narr[0] == 0) stax_narr = strdupa("Service Tax");
                                else stax_narr = strdupa(stax_narr);
                        }
                        else{
                                do_log("inside else of 100");
                                st_per_val=db->get_field(0, "value");
                                st_per_val=strdup("14");
                                stax_narr = strdupa("Service Tax");
                        }
                        service_tax_percent = atof(st_per_val);
                        sprintf(command,"service_tax_percent is %f",service_tax_percent);
                        do_log(command);
			item_cost = strdup(item_cost);
			do_log("Item_cost 2");
                        do_log(item_cost);
/*			float item_cost_with_gst = round(((atof(item_cost) * service_tax_percent)/100) + atof(item_cost));
                        sprintf(command,"Item cost including gst is %f",item_cost_with_gst);	*/
			sprintf(command,"Item cost including gst is %s",item_cost); 
			do_log(command);
//			sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%d') ;",cartid,c_sim_phone_no,(int)item_cost_with_gst);
			sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%s') ;",cartid,c_sim_phone_no,item_cost);
			do_log(command);
                        char buffer[1024];
                        int hk = db->ExecCommandOk(command);
                        sprintf (buffer, "hk is  %d", hk);
		        do_log(buffer);
                        if(hk!=0)
                        {
                                 do_log("Inserted Successfully");
				 do_log("commit work");
				 db->ExecCommandOk("commit work");
		   	}
	                else
        	        {
                              do_log("Not Inserted");
			      do_log("Rollback work");
                              sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,c_sim_phone_no);
                              db->ExecCommandOk("rollback work");
                              int result = reg_process_fail(order_no,msg);
                              return 0;
                        }				
	       }
	//return 0;	
	}
	else if(strstr(c_payment_mode, "Credit Card")!= NULL)
	{
		do_log("--------------Payment Done....Don't Create Payment Link.------------------");
		do_log("Payment Method is PayU  or Amazon");
		char stype[25],billno[50];
		char *pp;
		char *st_per_val=NULL;
		char *surcharge_val=NULL;
		char *st_curr=NULL;
		float service_tax_percent;
		float surcharge;
		float service_tax;
		char *stax_narr;
		char *sur_chg_narr;
		do_log("Get Payment Details");
		do_log("Before Subtotal");
		char *subtotal = cgi_getentrystr("subtotal");
		do_log(subtotal);
		do_log("After Subtotal");
		subtotal = strdup(subtotal);
		do_log("subtotal");
		do_log(subtotal);
		char *item_cost;
		char *rate;char *rate1,*type,*sim_phone_number,*chqno,*chqdate,*bname,*tid,*bankname,*narration,*userid,*strdomain;
		char *mode=c_payment_mode;
		int credit;float exch_rate;
		do_log("Payment Mode");
		do_log(mode);
		//do_log(item_cost);
		do_log("Total Amount");
		do_log(total);	
		do_log("tax");
		do_log(total_tax);
		char *cartid=NULL;
//        	sprintf(command,"select * from tsim_pack_details where pack_name = '%s'",kkk);

		/*
		char base_order_no[20];  // Array to hold the base order number
                                
                                // Check if the order_no contains an underscore and split it
                                char *underscore_pos = strchr(order_no, '_');
                                if (underscore_pos != NULL) {
                                    // Extract the part before the first underscore
                                    strncpy(base_order_no, order_no, underscore_pos - order_no);
                                    base_order_no[underscore_pos - order_no] = '\0';  // Add null terminator
                                } else {
                                    // No underscore found, use the entire order_no
                                    strcpy(base_order_no, order_no);
                                }

		sprintf(command,"select round(price+total_tax/quantity) as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",base_order_no,kkk);*/


		sprintf(command,"select subtotal+ subtotal_tax as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",order_no,kkk);
		do_log(command);
		int jj = db->ExecTuplesOk(command);
		if(jj>0)
		{		
			item_cost = db->get_field(0,"price");
			item_cost = strdup(item_cost);
			//if (!item_cost || (item_cost == NULL))
			if(item_cost == NULL || item_cost[0]==0)
			{
				do_log("Pack Details Not present");
				//strcpy(item_cost,subtotal);
				strcpy(item_cost,total); //changed it to total
				do_log("item_cost");
				do_log(item_cost);	
			}
			else
			{
				do_log("Pack Details Present");
				do_log("pack_cost");    
        	                do_log(item_cost);	
			}	
		}
		else
		{
			do_log("No Such Plan Available");
			sprintf(msg,"Order no %s has booked in MIS. No %s Plan Available. Do the Needful.\nUsername %s \nOrder_no %s \nTSIM No %s",order_no,kkk,username,order_no,c_sim_phone_no);
                        int result = reg_process_fail(order_no,msg);
                        return 0;
	
		}
		item_cost = strdup(item_cost);

		sprintf(command,"SELECT * FROM billing_config where code='100' and datefrom<=now() and dateto>=now()");
		do_log(command);
		if(db->ExecTuplesOk(command)>0){
				st_per_val=db->get_field(0, "value");
				stax_narr = db->get_field(0, "descr");
				if (stax_narr == NULL || stax_narr[0] == 0) stax_narr = strdupa("Service Tax");
				else stax_narr = strdupa(stax_narr);
		}
		else{
				st_per_val=strdup("14");
				stax_narr = strdupa("Service Tax");
		}
		service_tax_percent = atof(st_per_val);
		sprintf(command,"service_tax_percent is %f",service_tax_percent);
		do_log(command);
		item_cost = strdup(item_cost);
		do_log("Item_cost 3");
		do_log(item_cost);
/*		float item_cost_with_gst = round(((atof(item_cost) * service_tax_percent)/100) + atof(item_cost));
                        sprintf(command,"Item cost including gst is %f",item_cost_with_gst);*/
		sprintf(command,"Item cost including gst is %s",item_cost);
		do_log(command);
		do_log("Begin work");
		db->ExecCommandOk("begin work");
		sprintf(command,"select * from global_sim_cc_payer_det  where order_no = '%s'",order_no);
			do_log(command);
		if(db->ExecTuplesOk(command))
		{
		do_log("Record for order is present");
			
//			sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%d') ;",order_no,c_sim_phone_no,(int)item_cost_with_gst);
		sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%s') ;",order_no,c_sim_phone_no,item_cost);
		do_log(command);
					char buffer[1024];
					int hk = db->ExecCommandOk(command);
				sprintf (buffer, "hk is  %d", hk);
			do_log(buffer);
					if(hk!=0)
					{
								do_log("Inserted Successfully");
				do_log("commit work");
				db->ExecCommandOk("commit work");
		}
				else
				{
					do_log("Not Inserted");
				do_log("Rollback work");
							sprintf(msg,"Order no %s has booked in MIS. Adding entry in global_sim_cc_sim_amount_det Failed. Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,order_no,item_cost,c_sim_phone_no);
						db->ExecCommandOk("rollback work");
							int result = reg_process_fail(order_no,msg);
							return 0;
					}
		}
        	else
	        {
				do_log("No Record for this order, Insert the Record");
				sprintf(command,"select rate from exchange_rate order by date desc limit 1");
				do_log(command);
				int mm = db->ExecTuplesOk(command);
				char buffer[1024];
				sprintf (buffer, "cnt is  %d", mm);
				do_log(buffer);
				char *exch;
				if(mm>0)
				{
							exch=db->get_field(0,"rate");
				}
				do_log(exch);
				exch_rate = atof(exch);
				exch_rate = roundf(exch_rate * 100) / 100;
				sprintf(buffer,"Exch_rate is %f",exch_rate);
				do_log(buffer);	
				float ftemp = atof(total);
				sprintf(buffer,"Total Amount is  %f  Rs.",ftemp);
				do_log(buffer);
				do_log("******************Amount Calculation Hardcoded**********************");
				float amt_p=(ftemp/118)*100/exch_rate;
				sprintf(buffer,"Total Amount in $ is %f  US$",amt_p);
		 		do_log(buffer);
				float nnn = (amt_p * 100) / 100;
				char buff[100];
				sprintf(buff,"%.3f",nnn);
				do_log(buff);
				do_log("*********************************Testing GST Calculations for PayU*****************************************");
				sprintf(command,"SELECT * FROM billing_config where code='100' and datefrom<=now() and dateto>=now()");
				do_log(command);
				if(db->ExecTuplesOk(command)>0){
						st_per_val=db->get_field(0, "value");
						stax_narr = db->get_field(0, "descr");
						if (stax_narr == NULL || stax_narr[0] == 0) stax_narr = strdupa("Service Tax");
						else stax_narr = strdupa(stax_narr);
				}
				else{
						st_per_val=strdup("14");
						stax_narr = strdupa("Service Tax");
				}
				service_tax_percent = atof(st_per_val);
				sprintf(command,"service_tax_percent is %f",service_tax_percent);
				do_log(command);
				sprintf(command,"SELECT * FROM billing_config where code='101' and datefrom<=now() and dateto>=now();");
				do_log(command);
				if (db->ExecTuplesOk(command)>0){
						surcharge_val=db->get_field(0, "value");
						sur_chg_narr = db->get_field(0, "descr");
						if (sur_chg_narr == NULL || sur_chg_narr[0] == 0) sur_chg_narr = strdupa("Education Cess and surcharge");
						else sur_chg_narr = strdupa(sur_chg_narr);
				}
				else{
						surcharge_val=strdup("0");
						sur_chg_narr = strdupa("Education Cess and surcharge");
				}
				item_cost = strdup(item_cost);
				do_log(item_cost);
				sprintf(command,"Item cost including gst is %s",item_cost);
				do_log(command);
				sprintf(command,"Total Amount is %s  Rs.",item_cost);
                        do_log(command);
                        do_log("******************************Testing ends here***************************************************");

			
			 sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no,username,domainname) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%s','pending','%s',1,mobno,'%s','%s','%s' from sim_user where username ='%s');",c_sim_phone_no,subtotal,order_no,order_no,username,domainname,username);
			do_log(command);
			int mk = db->ExecCommandOk(command);
			sprintf (buffer, "mk is  %d", mk);
			do_log(buffer); 
			if (mk){
				do_log("Record for this order is present");
				sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%s') ;",order_no,c_sim_phone_no,item_cost);
				do_log(command);
                	        char buffer[1024];
                        	int hk = db->ExecCommandOk(command);
	                        sprintf (buffer, "hk is  %d", hk);
			        do_log(buffer);
                	        if(hk!=0)
                        	{
                                	 do_log("Inserted Successfully");
					 do_log("commit work");
					 db->ExecCommandOk("commit work");
			   	}
	                	else
        	        	{
		                        do_log("Not Inserted");
				        do_log("Rollback work");
                        	        sprintf(msg,"Order no %s has booked in MIS. Adding entry in global_sim_cc_sim_amount_det Failed. Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,order_no,item_cost,c_sim_phone_no);
                              		db->ExecCommandOk("rollback work");
                              		int result = reg_process_fail(order_no,msg);
                              		return 0;
				}
                        }
                        else
                        {
                                do_log("Failed to insert.");
				do_log("Rollback work");
				db->ExecCommandOk("rollback work");
                        }
		}
        }
	else
	{
		do_log("Payment mode is different than PAD and Credit Card");
		do_log("add e credit for global card");
                char e_amount[21];
                agentusername = strdupa("trikon");
                psswd = strdupa("k1n065qnone");
                gsim GSIM;
                int res = GSIM.init_gsim_with_sim_phone_no(db,c_sim_phone_no);
                if (!res)
                {
                        do_log("failed to init_gsim_with_sim_phone_no");
			do_log("Rollback");
                        sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed.Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,order_no,e_amount,c_sim_phone_no);
                        db->ExecCommandOk("rollback work");
                        int result = reg_process_fail(order_no,msg);
			 return 0; 
                }
                //sprintf(command,"select * from countrydata  where countryname like '%%%s%%'",c_country);
                sprintf(command,"select * from countrydata  where countryname ='%s'",c_country);
                do_log(command);
                char api_code[20];
                bool add_e_credit_entry_on_booking = 0;
                if(db->ExecTuplesOk(command))
                {
			//do_log("here");
                        strcpy(api_code,db->get_field(0,"api_code"));
                }else
                        do_log("no result");
                do_log(api_code);
                if (!strcmp(db->get_field(0,"add_e_credit_entry_on_booking"),"1"))
                     add_e_credit_entry_on_booking = 1;
                else
                    add_e_credit_entry_on_booking = 0;
/*                if((get_tag_value(CONFIG_DATA_FILE,"<!-- GLOBAL_E_CREDIT","-- GLOBAL_E_CREDIT>",c_country,e_amount)!=1) || (e_amount==NULL || e_amount[0]==0)){
                                if (!strcmp(c_country, "GLOBAL POSTPAID")) strcpy(e_amount,"298.9");
                                else if (!strcmp(c_country,"GLOBAL NEW")) strcpy(e_amount,"98.9");
                                else if (!strcmp(c_country,"UK")) strcpy(e_amount,"298.9");
                                else if (!strcmp(c_country,"USA")) strcpy(e_amount,"298.9");
                                //else if (!strcmp(country,"USA Euro Data SIM")) strcpy(e_amount,"48.9");
                                //else if (!strcmp(country,"USA WORLDWIDE")) strcpy(e_amount,"58.9");
                                else strcpy(e_amount,"58.9");
                }*/

		 char globalcountry[1024];
                sprintf(globalcountry,"GLOBAL_E_CREDIT_%s",c_country);
                char* data24 = get_simmis_data_from_table(globalcountry,"58.9");
                strcpy(e_amount,data24);


                do_log("e_amount");
				do_log(e_amount);
                if (add_e_credit_entry_on_booking )
                {
                       do_log("e credit entry is 1");
                       //res = GSIM.add_e_credit(atof(e_amount),0,api_code,c_sim_phone_no);
                }
                else
                {
                       do_log("e credit entry is 0");
		       do_log(api_code);
                       //res = GSIM.debit_from_prepaid(atof(e_amount),api_code);
                }
                if (!res)
                {
                        do_log("failed to add_e_credit");
			do_log("Rollback work");
                        sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed.Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,order_no,e_amount,c_sim_phone_no);
                        db->ExecCommandOk("rollback work");
                        int result = reg_process_fail(order_no,msg);
                        return 0;
                        /****************************************No Longer Needed******************/ 
                }
	}
	do_log("------------Ending the register_tsim_client function with return value as 1 --------------------------");
	return 1;
}




/***********************************Function to return the payment link*****************************************************/
char* get_sim_phone_no(fax_database *db,char* country)
{
	
}


char* payment_link(fax_database *db,char* order_no,char* sku,char* link_amt,char* item_cost,char* subtotal,char* sim_phone_no,char* username)
{
        char msg[1024],command[1024];
        printf("link amount is %s",link_amt);
        printf("item_cost is %s",item_cost);
        printf("sim_phone_no is %s",sim_phone_no);
        printf("username is %s",username);
        struct timeval tv1;
        float exch_rate;
        char *cartid=NULL;char *st_per_val=NULL;char *surcharge_val=NULL;char *st_curr=NULL;float service_tax_percent;float surcharge;float service_tax;char *stax_narr;char *sur_chg_narr;
        do_log("Begin work");
        db->ExecCommandOk("begin work");
        sprintf(command,"select cartid from global_sim_cc_payer_det  where order_no = '%s'",order_no);
        do_log(command);
        if(db->ExecTuplesOk(command))
        {       
                cartid=db->get_field(0,"cartid");
                do_log(cartid);
                cartid=strdup(cartid);
        }
        else
        {       
                do_log("no result new");
        }
        do_log("cart id");
        do_log(cartid);
        if (cartid == NULL || cartid[0] == 0)
        {
                time_t now;
                time(&now);
                struct tm* now_tm;
                now_tm = localtime(&now);
                char cartid[80];
                strftime (cartid, 80, "%Y%m%d%H%m%s", now_tm);
                do_log(cartid);
                sprintf(command,"select rate from exchange_rate order by date desc limit 1");
                do_log(command);
                int mm = db->ExecTuplesOk(command);
                char buffer[1024];
                sprintf (buffer, "cnt is  %d", mm);
                do_log(buffer);
                char *exch;
                if(mm>0)
                {
                        exch=db->get_field(0,"rate");
                }
                do_log(exch);
                exch_rate = atof(exch);
                exch_rate = roundf(exch_rate * 100) / 100;
                sprintf(buffer,"Exch_rate is %f",exch_rate);
                do_log(buffer);
                float ftemp = atof(link_amt);
                sprintf(buffer,"Link Amount is %f  Rs.",ftemp);
                do_log(buffer);
                do_log("*********************************Testing GST Calculations*****************************************");
		sprintf(command,"SELECT * FROM billing_config where code='100' and datefrom<=now() and dateto>=now()");
                do_log(command);
                if(db->ExecTuplesOk(command)>0){
                        st_per_val=db->get_field(0, "value");
                        stax_narr = db->get_field(0, "descr");
                        if (stax_narr == NULL || stax_narr[0] == 0) stax_narr = strdupa("Service Tax");
                        else stax_narr = strdupa(stax_narr);
                }
                else{
                        st_per_val=strdup("14");
                        stax_narr = strdupa("Service Tax");
                }
                service_tax_percent = atof(st_per_val);
                sprintf(command,"service_tax_percent is %f",service_tax_percent);
                do_log(command);
                sprintf(command,"SELECT * FROM billing_config where code='101' and datefrom<=now() and dateto>=now();");
                do_log(command);
                if (db->ExecTuplesOk(command)>0){
                        surcharge_val=db->get_field(0, "value");
                        sur_chg_narr = db->get_field(0, "descr");
                        if (sur_chg_narr == NULL || sur_chg_narr[0] == 0) sur_chg_narr = strdupa("Education Cess and surcharge");
                        else sur_chg_narr = strdupa(sur_chg_narr);
                }
                else{
                        surcharge_val=strdup("0");
                        sur_chg_narr = strdupa("Education Cess and surcharge");
                }
		item_cost = strdup(item_cost);
		do_log("Item_cost 4");
                        do_log(item_cost);
	//	float item_cost_with_gst = round(((atof(item_cost) * service_tax_percent)/100) + atof(item_cost));
          //              sprintf(command,"Item cost including gst is %f",item_cost_with_gst);
          		sprintf(command,"Item cost including gst is %s",item_cost);
               do_log(command);
	/*	 surcharge = atof(surcharge_val);
                sprintf(command,"Surcharge to be added to the tax is %f",surcharge);
                do_log(command);
                service_tax_percent = service_tax_percent + ((surcharge / 100) * service_tax_percent);
                sprintf(command,"Total Tax to be levied is %f",service_tax_percent);
                do_log(command);
                float val_in_cal = 100 + service_tax_percent;
                sprintf(command,"Value to be taken in calculation is %f",val_in_cal);
                do_log(command);
                sprintf(command,"Link Amount is %f  Rs.",ftemp);
                do_log(command);
                float link_amount = (ftemp/val_in_cal)*100/exch_rate;
                sprintf(command,"Link Amount is %f  US$",link_amount);
                do_log(command);
                float link_amount_dollor = (link_amount * 100) / 100;
                sprintf(command,"Link Amount is %f  US$",link_amount_dollor);
                do_log(command);
                char buff[100];
                sprintf(buff,"%.3f",link_amount_dollor);
                do_log(buff);*/
	        sprintf(command,"Link Amount is %s  Rs.",item_cost);
                do_log(command);
//                sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no,username,domainname) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%s','pending','%s',1,mobno,'%s','%s','tsim.in' from sim_user where username ='%s');",sim_phone_no,item_cost,cartid,order_no,username,username);
		
		sprintf(command,"insert into global_sim_cc_payer_det (fname,bldg,street,city,state,zip,country,mobno,email,sim_phone_no,amount,status,cartid,auth,telno,order_no,username,domainname) (select clientname,add_building,add_street,add_city,add_state,add_pinno,add_country,mobno,emailadd,'%s','%s','pending','%s',1,mobno,'%s','%s','tsim.in' from sim_user where username ='%s');",sim_phone_no,subtotal,cartid,order_no,username,username);

                do_log(command);
                int mk = db->ExecCommandOk(command);
                sprintf (buffer, "mk is  %d", mk);
                do_log(buffer);
                char *nextval = NULL;
                struct timeval tv1;
                gettimeofday(&tv1, 0);
                if(mk>0)
                {

			do_log("link created successfully");
//                        sprintf(command,"select pack_cost from tsim_pack_details where pack_name = '%s'",sku);
/*			char base_order_no[20];  // Array to hold the base order number
                                
                                // Check if the order_no contains an underscore and split it
                                char *underscore_pos = strchr(order_no, '_');
                                if (underscore_pos != NULL) {
                                    // Extract the part before the first underscore
                                    strncpy(base_order_no, order_no, underscore_pos - order_no);
                                    base_order_no[underscore_pos - order_no] = '\0';  // Add null terminator
                                } else {
                                    // No underscore found, use the entire order_no
                                    strcpy(base_order_no, order_no);
                                }

			sprintf(command,"select round(price+total_tax/quantity) as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",base_order_no,sku);*/

			sprintf(command,"select subtotal+ subtotal_tax as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",order_no,sku);
                        do_log(command);
                        int jj = db->ExecTuplesOk(command);
                        if(jj>0)
                        {
                                item_cost = db->get_field(0,"price");
                                item_cost = strdup(item_cost);
                                if (item_cost == NULL || item_cost[0]== 0)
                                {
                                        do_log("Pack Details not Present");
                                        strcpy(item_cost,link_amt);
                                        do_log("item_cost");
                                        do_log(item_cost);
                                }
                                else
                                {
                                        do_log("Pack Details  present");
                                        do_log("pack_cost");
                                        do_log(item_cost);
                                }
                        }
                        else
                        {
                                do_log("No Such Plan Available");
                                sprintf(msg,"Order no %s has booked in MIS. No %s Plan Available. Do the Needful.\nUsername %s \nOrder_no %s \nTSIM No %s",order_no,sku,username,order_no,sim_phone_no);
                                int result = reg_process_fail(order_no,msg);
                                return 0;
                        }
/*			float item_cost_with_gst = round(((atof(item_cost) * service_tax_percent)/100) + atof(item_cost));
                        sprintf(command,"Item cost including gst is %f",item_cost_with_gst);*/
			sprintf(command,"Item cost including gst is %s",item_cost);
               do_log(command);
                        item_cost = strdup(item_cost);
			do_log("Item_cost 5");
			do_log(item_cost);
//		        sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%d') ;",cartid,sim_phone_no,(int)item_cost_with_gst);
			sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%s') ;",cartid,sim_phone_no,item_cost);		        
                        do_log(command);
                        int hk = db->ExecCommandOk(command);
                        sprintf (buffer, "hk is  %d", hk);
                        do_log(buffer);
                        if(hk > 0)
                        {
                                do_log("Inserted Successfully in global_sim_cc_sim_amount_det");
				db->ExecCommandOk("commit work");
				do_log("commit work");
                        }
                        else
                        {
                                do_log("Not Inserted in global_sim_cc_sim_amount_det");
                        }
                }
                else
                {
                        do_log("Could Not Create Payment Link. Do the Needful");
                        do_log("Rollback work");
                        sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,sim_phone_no);
                        db->ExecCommandOk("rollback work");
                        int result = reg_process_fail(order_no,msg);
                        return 0;
                }
        }
	else
	{
		do_log("Cart id for existing order is present");
//		sprintf(command,"select pack_cost from tsim_pack_details where pack_name = '%s'",sku);
/*		char base_order_no[20];  // Array to hold the base order number
                                
                                // Check if the order_no contains an underscore and split it
                                char *underscore_pos = strchr(order_no, '_');
                                if (underscore_pos != NULL) {
                                    // Extract the part before the first underscore
                                    strncpy(base_order_no, order_no, underscore_pos - order_no);
                                    base_order_no[underscore_pos - order_no] = '\0';  // Add null terminator
                                } else {
                                    // No underscore found, use the entire order_no
                                    strcpy(base_order_no, order_no);
                                }

	
		sprintf(command,"select round(price+total_tax/quantity) as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",base_order_no,sku);*/

		sprintf(command,"select subtotal+ subtotal_tax as price from tsim_orders_line_items where order_no ='%s' and sku='%s';",order_no,sku);
		do_log(command);
		int jj = db->ExecTuplesOk(command);
		if(jj>0)
		{
			item_cost = db->get_field(0,"price");
			item_cost = strdup(item_cost);
			if (item_cost == NULL || item_cost[0]== 0)
			{
				item_cost = cgi_getentrystr("subtotal");
				do_log("Pack Details Not present");
				do_log("item_cost");
				do_log(item_cost);
			}
			else
			{
				do_log("Pack Details Present");
				do_log("pack_cost");
				do_log(item_cost);
			}
		}
		else
		{
			do_log("No Such Plan Available");
			sprintf(msg,"Order no %s has booked in MIS. No %s Plan Available. Do the Needful.\nUsername %s \nOrder_no %s \nTSIM No %s",order_no,sku,username,order_no,sim_phone_no);
			int result = reg_process_fail(order_no,msg);
			return 0;
		}
		item_cost = strdup(item_cost);
		sprintf(command,"SELECT * FROM billing_config where code='100' and datefrom<=now() and dateto>=now()");
                do_log(command);
                if(db->ExecTuplesOk(command)>0){
                        st_per_val=db->get_field(0, "value");
                        stax_narr = db->get_field(0, "descr");
                        if (stax_narr == NULL || stax_narr[0] == 0) stax_narr = strdupa("Service Tax");
                        else stax_narr = strdupa(stax_narr);
                }
                else{
                        st_per_val=strdup("14");
                        stax_narr = strdupa("Service Tax");
                }
                service_tax_percent = atof(st_per_val);
                sprintf(command,"service_tax_percent is %f",service_tax_percent);
		item_cost = strdup(item_cost);
                        do_log(item_cost);
			do_log("Item_cost 6");
                        do_log(item_cost);

/*		float item_cost_with_gst = round(((atof(item_cost) * service_tax_percent)/100) + atof(item_cost));
                sprintf(command,"Item cost including gst is %f",item_cost_with_gst);*/
		sprintf(command,"Item cost including gst is %s",item_cost);
		do_log(command);
//		sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%d') ;",cartid,sim_phone_no,(int)item_cost_with_gst);
		sprintf(command,"insert into global_sim_cc_sim_amount_det (cartid,sim_phone_no,amount) values ('%s','%s','%s') ;",cartid,sim_phone_no,item_cost);
		do_log(command);
		char buffer[1024];
		int hk = db->ExecCommandOk(command);
		sprintf (buffer, "hk is  %d", hk);
		do_log(buffer);
		if(hk!=0)
		{
			do_log("Inserted Successfully");
			do_log("commit work");
			db->ExecCommandOk("commit work");
		}
		else
		{
			do_log("Not Inserted");
			do_log("Rollback work");
			sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s \nOrder_no %s Amount %s \nTSIM No %s",order_no,username,link_amt,sim_phone_no);
			db->ExecCommandOk("rollback work");
			int result = reg_process_fail(order_no,msg);
			return 0;
		}
	}	
//        char link[1024]="https://mail.tsim.in/tsim/wor_l.php?c=";
	char link[1024]="https://mail.tsim.in/tsim/slp.php?c=";
        strcat(link, cartid);
        strcat(link, "&a=det_link");
        do_log("link");
        do_log(link);
	char* p_link= NULL;
	strcpy(p_link,link);
        return p_link;
}
int addcredit_sim(fax_database *addcredit, const char *user, const char *domain, const char *max_red, char *order_no, const char *f_mon, const char *total_amt, const char *cr_to_add,const char *tax, const char *tripid, const char *narration, const char *chqno, const char *chqdate,const char *bname,const char *curr,const char *rate,char *stype,char *tid,char *sim_phone_number)
{
        do_log("in addcredit function");
	char command[1024],msg[1024];
        sprintf(command,"Username= %s, rate -%s- atof -%.2f-",user,rate,atof(rate));
        do_log(command);
        //fax_database addcredit(E2FAXDB,DBSERV);
        //fax_database addcredit1(E2FAXDB,DBSERV);
        int i;
        float amt,e_rate;
        struct timeval tv1;
	double pack_amt;
        char *nextval = NULL;
        max_red = f_mon;
        if (cr_to_add && cr_to_add[0] != 0 && is_no(cr_to_add))
        {
       		do_log("Credit to Be Added");
		double credit = atof(cr_to_add);
		double tax_deducted = atof(tax);
		double total_amt_paid = atof(total_amt);
		do_log("Begin work");
		if (addcredit->ExecCommandOk("begin work") != 1) 
		{
                        do_log("102-Could not begin work");
                }
                if (!strcmp(stype,"global"))
                {
                 	do_log("stype is GLOABAL");

			/* Find if there are any packs enabled, if yes add the amount corresponding to the pack amount as credit in the card */
                        if (!strcmp(max_red,"Credit Card") && cr_to_add > 0)
			{
                                sprintf(command,"select * from max_reduction_codes where tripid ='%s'  and to_date>='now'",tripid);
                                do_log(command);
                                if (addcredit->ExecTuplesOk(command))
				{
                                        sprintf(command,"SELECT coalesce(get_tsim_pack_cost('%s'),'0') as pack_amount",tripid);
                                        do_log(command);
                                        if (addcredit->ExecTuplesOk(command)){
                                                pack_amt = atof(addcredit->get_field(0,"pack_amount"));
                                                if (pack_amt>0){
                                                        sprintf(command,"Pack amount is %f",pack_amt);
                                                        do_log(command);
                                                        //dblcr_to_add = pack_amt * atof(rate);
                                                }
                                                else{
                                                        sprintf(command,"No pack it seems, adding whatever as received");
                                                        do_log(command);
                                                }
                                        }
                                        else{
                                                sprintf(command,"No pack it seems, adding whatever as received");
                                                do_log(command);
                                        }
                                }
                        	/* If there is no pack, check if its a refill */
                       		 if (pack_amt == 0)
				{
                                	sprintf(command,"SELECT coalesce(get_tsim_refill_amount('%s','%d'),'0') as refill_amount",tripid,(int)credit);
	                                do_log(command);
        	                        if (addcredit->ExecTuplesOk(command))
					{
                	                        pack_amt = atof(addcredit->get_field(0,"refill_amount"));
                        	                if (pack_amt>0){
                                	                 sprintf(command,"Refill amount is %f",pack_amt);
                                                 do_log(command);
                                        	}
	                                        else{
        	                                        sprintf(command,"No refill too, adding whatever as received");
                	                                do_log(command);
                        	                }
                                	}
                                	else{
                                        	sprintf(command,"No refill too, adding whatever as received");
	                                        do_log(command);
        	                        }
                	        }
                        }			
                        if (credit/atof(rate) !=0 )
                        {
				agentusername = strdupa("trikon");
                                psswd = strdupa("K1N065qnoNE");
                                //Refill the sim card 
                                gsim GS;
                                sprintf(command,"select * from countrydata where countryname= (select country from sim_stock where sim_phone_no in (SELECT sim_phone_no from clienttrip where tripid ='%s'))",tripid);
                                do_log(command);
                                do_log(sim_phone_number);
				char api_code[20];
                                if(addcredit->ExecTuplesOk(command))
                                {
                                         strcpy(api_code,addcredit->get_field(0,"api_code"));
                                }
                                else
                                         do_log("No result");
                                do_log(api_code);
                                int res = GS.init_gsim_with_sim_phone_no(addcredit,sim_phone_number);
                                if (!res)
                                {
                                         do_log("init_gsim_with_sim_phone_no failed ");
                                         do_log("Rollback work");
                                  	 sprintf(msg,"Order has booked in MIS. Adding credit to the Sim Card Failed.Do the Needful.\nUsername %s \nAmount %s \nTSIM No %s",user,credit,sim_phone_number);
	                                 addcredit->ExecCommandOk("rollback work");
        	                         int result = reg_process_fail(order_no,msg);
                	                 return 0;					   
                                 }
                                 if(!strcmp(api_code,"telna"))      //Complete method added by Neekhil Dedhia
                                 {
                                        char call_php[100];
                                        sprintf(call_php,"%s %s",PHP_CMD_P,PHP_ADD_CREDIT_PATH);
                                        char param1[12];
                                        char param2[10];
                                        strcpy(param1,sim_phone_number);
                                        strcat(param1," ");
                                        if (pack_amt>0) sprintf(param2,"%.2f",pack_amt);
                                        else sprintf(param2,"%.2f",((credit/atof(rate))));
                                        strcat(param1,param2);
					char buf[7];
					sprintf(buf," %d",getpid());
                                        strcat(param1,buf);
                                        strcat(call_php,param1);
                                        do_log(call_php);
                                        system(call_php);
                                        char trans_status[20];
                                        char transfilename[30]="/tmp/transaction_status_";char buf2[7];sprintf(buf2,"%d",getpid());strcat(transfilename,buf2);do_log(transfilename);
                                        get_tag_value(transfilename,"<!-- TRANSACTION","--TRANSACTION>","Status",trans_status);
                                        if(!strcmp(trans_status,"OK"))
                                        	res=1;
                                        else
                                                res=0;
                                        remove(transfilename);
                                 }
                                 else
				 {
					sprintf(command,"cr_to_add -%.2f- rate -%.2f- pack_amt -%.2f-",credit,atof(rate),pack_amt);
                                        do_log(command);
                                        double tmpamt = 0;
                                        if (pack_amt>0) tmpamt = pack_amt;
                                        else tmpamt = credit/atof(rate);
                                        sprintf(command,"tmpamt -%.2f-%f-",tmpamt,round(tmpamt));
                                        do_log(command);
                                        //if (tmpamt!=0.00) res = GS.debit_from_prepaid(round(tmpamt),api_code);
                                         if (round(tmpamt) == 0.000000) res =1 ;
                                         else res = GS.debit_from_prepaid(tmpamt,api_code);
                                 }
                                 if (!res)
				 {
                                        do_log("refilling the sim_phone failed ");
                                        do_log("Rollback work");
                                        sprintf(msg,"Order no %s has booked in MIS. Adding e_credit Failed & Payment link Cant Not Created Do the Needful.\nUsername %s \nAmount %s \nTSIM No %s",user,credit,sim_phone_number);
                                       addcredit->ExecCommandOk("rollback work");
                                       int result = reg_process_fail(order_no,msg);
                                       return 0;	
                                 }
				 /*if(strcmp(api_code,"telna"))
                                 {
                                        sprintf(command,"Payment received: USD %.2f, Amount refilled: USD %.2f, Current balance: USD %.2f",credit/atof(rate),GS.get_refill_amount(),GS.get_new_balance());
                                        do_log(command);
                                        int res1 = GS.send_USSD(command);
                                        if (res1) do_log("USSD sent");
                                        else do_log("failed to send USSD");
                                 }
                                 else
                                 {
                                        char file_name[100];
                                        strcpy(file_name,sim_phone_number);
                                        strcat(file_name,"_");
                                        strcat(file_name,mis_user);
                                        do_log(file_name);
                                        char php_cmd[100];
                                        sprintf(php_cmd,"%s %s %s",PHP_CMD_P,PHP_FILE_WRITE_PATH,file_name);
                                        system(php_cmd);char temp_bal[10];
                                        char read_file[20];
                                        strcpy(read_file,"/tmp/");
                                        strcat(read_file,file_name);
                                        get_tag_value(read_file,"<!-- SIMDETAILS","--SIMDETAILS>","Balance",temp_bal);
                                        sprintf(command,"Payment received: USD %.2f, Amount refilled: USD: %.2f. The balance now is %s",credit/atof(rate),(credit/atof(rate)),temp_bal);
                                        do_log(command);
                                        remove(read_file);
                                 }*/
                         }
	                sprintf(command,"credit is %f tax is %f",credit,tax_deducted);
			do_log(command);
                	sprintf(command,"Total Amt is %f",total_amt_paid);
			do_log(command);
			gettimeofday(&tv1, 0);
       			if(!strcmp(f_mon, "Credit Card"))
			{
		  		do_log("Credit Card Case");
				sprintf(command, "SELECT nextval('trkn_append_seq');");
        	                do_log(command);
				if (addcredit->ExecTuplesOk(command) > 0)
                        	{
                               		nextval = addcredit->get_field(0, "nextval");
		                        if (!nextval || (nextval = strdup(nextval)) == NULL)
        		                {
                	                        //addcredit.ExecCommandOk("rollback work");
                        	                do_log("could not get nextval adding default");
                               		        nextval = "12345";
                               		}
                        	}
				/************** First Entry in Faxed with Total Amount********************************/
		                do_log("**************************First Entry in the Faxes****************************");
				sprintf(command, "insert into faxes(trkn,username,domainname,status,accept_time,receive_time,currency,sender) values ('%ld%s','%s','%s','[Pin:%s] [Value = %.2f]','now','now','%s','%s');",tv1.tv_sec, nextval, user, domain, max_red, total_amt_paid/atof(rate), "USD", f_mon);
        		        do_log(command);
				if (addcredit->ExecCommandOk(command) != 1)
                        	{
                                	do_log("102-ExecCommandOk Failed");
		                        do_log("102-System Error");
				        do_log("Rollback work");
		                        sprintf(msg,"Order no %s has booked in MIS. Adding credit to Sim Card Failed.Do the Needful.\nUsername %s \nAmount %s \nTSIM No %s",user,credit,sim_phone_number);
                                	int result = reg_process_fail(order_no,msg);
                		        addcredit->ExecCommandOk("rollback work");
                        		return 0;
                        	}
	                        else
        	                {
					/******************************First Entry in Transaction table with Total Amount********************/
		                        do_log("************************First Entry in the Transactions*********************");
	                	        sprintf(command,"insert into transactions values('%ld%s',0,%f,(Select credit-usage from users where username='%s' and domainname='%s'),(Select accept_time from faxes where trkn='%ld%s'),'%s','%s','%s');", tv1.tv_sec, nextval, total_amt_paid/atof(rate), user, domain, tv1.tv_sec, nextval, f_mon, user,domain);
					do_log(command);
					if (addcredit->ExecCommandOk(command) != 1)
                        	        {
		                                do_log("Could not add to transactions table\n");
						sprintf(msg,"Order has booked in MIS. Adding credit to Sim Card Failed.Do the Needful.\nUsername %s \nAmount %s \nTSIM No %s",user,credit,sim_phone_number);
	                                        int result = reg_process_fail(order_no,msg);
        	                                addcredit->ExecCommandOk("rollback work");
						return 0;
                		        }
                        		else
                                	{
                                        	sprintf(command,"cr_to_add is %f total_amount is %f",cr_to_add,total_amt_paid);
		                                do_log(command);
					        do_log("Credit Card Case");	
						/*********************Second Entry in Faxed Only Taxes**********************/
						sprintf(command, "SELECT nextval('trkn_append_seq');");
                                                do_log(command);
                                                if (addcredit->ExecTuplesOk(command) > 0)
                                                {
                                                       nextval = addcredit->get_field(0, "nextval");
                                                       if (!nextval || (nextval = strdup(nextval)) == NULL)
                                                       {
                                                                 addcredit->ExecCommandOk("rollback work");
                                                                 do_log("could not get nextval adding default");
                                                                 nextval = "12345";
                                                       }
                                                }
		                        	do_log("************************Second Entry in the Faxes***********************");
						sprintf(command, "insert into faxes(trkn,username,domainname,status,accept_time,receive_time,currency,sender) values('%ld%s','%s','%s','[Pin:%s] [Value = %.2f]','now','now','%s','%s');",tv1.tv_sec, nextval, user, domain, max_red, tax_deducted/atof(rate), "USD", "Service Tax");
			        	        do_log(command);
						if (addcredit->ExecCommandOk(command) != 1)
					        {
						        do_log("102-ExecCommandOk Failed");
						        do_log("102-System Error");
		                                        sprintf(msg,"Order has booked in MIS. Adding credit to Sim Card Failed.Do the Needful.\nUsername %s \nAmount %s \nTSIM No %s",user,credit,sim_phone_number);
						        int result = reg_process_fail(order_no,msg);
	                	                        addcredit->ExecCommandOk("rollback work");
					                return 0;
						}
						else
						{
		                        		do_log("**************************Second Entry in the Transactions***********************");
							/*********************Second Entry in Transaction******************/
							sprintf(command,"insert into transactions values('%ld%s',0,%f,(Select credit-usage from users where username='%s' and domainname='%s'),(Select accept_time from faxes where trkn='%ld%s'),'%s','%s','%s');", tv1.tv_sec, nextval, tax_deducted/atof(rate), user, domain, tv1.tv_sec, nextval, f_mon, user,domain);
						        do_log(command);
						        if (addcredit->ExecCommandOk(command) != 1)
						        {
						       	        do_log("rollback work");
						               	do_log("Could not add to transactions table\n");
								sprintf(msg,"Order has booked in MIS. Adding credit to Sim Card Failed.Do the Needful.\nUsername %s \nAmount %s \nTSIM No %s",user,credit,sim_phone_number);
        	                                                int result = reg_process_fail(order_no,msg);
	                                                        addcredit->ExecCommandOk("rollback work");
								return 0;
						        }
						        else
						        {
			        	                      	do_log("Entry in the Payment_discount");
								//sprintf(command,"insert into payment_discount(username,trip_id,amount,type,naration,accept_time,trkn,sim_card_type,trans_id,actual_payment_recd) values('%s','%s',%f,'%s','%s',(Select accept_time from faxes where trkn='%ld%s'),'%ld%s','%s','%s',%s)", user, tripid, credit/atof(rate),f_mon, narration, tv1.tv_sec, nextval, tv1.tv_sec, nextval,stype,"NA",credit);				
								sprintf(command,"insert into payment_discount(username,trip_id,amount,type,naration,accept_time,trkn,sim_card_type,actual_payment_recd) values('%s','%s',%f,'%s','%s',(Select accept_time from faxes where trkn='%ld%s'),'%ld%s','%s','%f')", user, tripid, credit/atof(rate),f_mon, narration, tv1.tv_sec, nextval, tv1.tv_sec, nextval,stype,credit);			
									
								do_log(command);
								if (addcredit->ExecCommandOk(command) != 1)							
								{
						               	      	do_log("rollback work");
					                		do_log("Could not add to payment_discount table\n");
									sprintf(msg,"Order has booked in MIS. Adding credit to Sim Card Failed.Do the Needful.\nUsername %s \nAmount %s \nTSIM No %s",user,credit,sim_phone_number);
					                                int result = reg_process_fail(order_no,msg);
					                                addcredit->ExecCommandOk("rollback work");
									return 0;
								}										                                        
							}
						}
					}		
                               	}
                	}
			else
			{
                	      	do_log("rollback work");
                      		do_log("Could not add to payment_discount table\n");
				sprintf(msg,"Order has booked in MIS. Adding credit to Sim Card Failed.Do the Needful.\nUsername %s \nAmount %s \nTSIM No %s",user,credit,sim_phone_number);
                                int result = reg_process_fail(order_no,msg);
                                addcredit->ExecCommandOk("rollback work");
				return 0;
			}										                                        
		}
        }
	else
        {
                do_log("101-ExecCommandOk Invalid or no credit");
                return 0;
        }
		
        char result[1024];
        sprintf(result, "Successful[trkn:%ld%s]", tv1.tv_sec, nextval);
        do_log(result);
        do_log("exit credit");
        do_log("commit work");
        addcredit->ExecCommandOk("commit work");
        return 1;
}

int reg_process_fail(char *order_no, char msg[1024])    
{       
	char command[1024],mailto[1024],subject[1024];
	fax_database db(E2FAXDB,DBSERV);
	sprintf(command,"update tsim_simmis_map set booking_done ='f' where order_no='%s'",order_no);
	do_log(command);
	if(db.ExecCommandOk(command))
		do_log("Flag Updated");
	else
		do_log("Flag Not updated");
	strcpy(command,msg);    
	do_log(command);
	sprintf(subject,"MIS booking failed for Order no  %s",order_no);
	if (!send_mail("husain@staff.ownmail.com",command,subject))
	{
		do_log("failed to send");
		send_mail("husain@staff.ownmail.com",command,subject);
		send_mail("taher@staff.ownmail.com",command,subject);
	}
	else
	{
		do_log("Mail sent to Telesales");
		send_mail("services@tsim.in",command,subject);
		send_mail("taher@staff.ownmail.com",command,subject);
	}     
}


int app_user_register_fail(char *email, char msg[1024])
{
	char command[1024],subject[1024];
	strcpy(command,msg);
	do_log(command);
	sprintf(subject,"App User registration failed for %s",email);
	if (!send_mail("husain@staff.ownmail.com",command,subject))
	{
		do_log("failed to send");
		send_mail("husain@staff.ownmail.com",command,subject);
		//send_mail("binu@staff.ownmail.com",command,subject);
	}
	else
	{
		do_log("Mail sent to R n D");
		//send_mail("binu@staff.ownmail.com",command,subject);
	}
}
/************************************************************************************************************/																
int draw_tripdetailpage(fax_database *db){
	int i;
        do_log("inside gettripdetails");
        draw_header(4);
        do_log("header");
        printf("\n<div id=mh style=""width:99%;height:95%"">");
        printf("\n<form name=frmtripdetails action=simmisn.e method=post>");
	printf("\n<input type=hidden name=a>");
	printf("\n<input type=hidden name=b>");
	printf("\n<input type=hidden name=c>");
	printf("\n<input type=hidden name=format>");
        do_log("creating form");
        printf("\n<b>Trip Summary</b>:<br>");
	printf("<br><br>");
        printf("\n<label>From Date  : </label>  \n");
	printf("<input id=""tfdate"" name=""tfdate"" type=""text"" readonly onclick=\"NewCal('tfdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('tfdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
	printf("\n<br><label>To Date  : </label>  \n");
	printf("<input id=""ttdate"" name=""ttdate"" type=""text"" readonly onclick=\"NewCal('ttdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('ttdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
		
	//fax_database db(E2FAXDB, DBSERV_R);
	string query = "SELECT DISTINCT status FROM clienttrip where status is not NULL;";
	int status_count = db->ExecTuplesOk(to_c_string(query));
	cout 	<< "<div>"
		<< "\n<label for='status_flter'>Status</label>"
		<< "\n\n<select id='status_filter' name = 'status_filter'>"
		<< "\n\t<option value = 'all_but_canceled' selected >All</option>";
	for(int i = 0; i < status_count; i++)
	{
		cout 	<< "\n\t<option value = '" << db->get_field(i, "status") << "' >"
			<< db->get_field(i, "status")
			<< "</option>";
	}
	cout << "\n</select>\n</div>\n";
	
	printf("\n<div>\n\t<label>Include Stats  : </label>\n\t<input type='checkbox' name='stats' value=\"yes\" />\n</div>");
	printf("\n<div>\n\t<label>Download: </label>\n\t<input type='checkbox' name='dnld' value=\"yes\" />\n</div>");

	printf("\n<input type=button name=btnsummary value=\"Show summary\" onclick=\"callsummary(document.frmtripdetails.tfdate.value,document.frmtripdetails.ttdate.value);\">");
        //printf("\n<a href=\"Javascript:newwindow(document.frmtripdetails.tfdate.value,document.frmtripdetails.ttdate.value);\">Download</a>");
        printf("\n<div id=\"dsummary\">");
        printf("\n</div>");
        printf("\n</div>");
        printf("\n</form>");
        draw_end();	
return 0;
}

int draw_global_upgrade_report_form(){
	int i;
        do_log("inside global_upgrade_report");
        draw_header(7);
        do_log("header");
        printf("\n<div id=mh style=""width:99%;height:95%"">");
        printf("\n<form name=frmglobalupgrep action=simmisn.e method=post>");
	printf("\n<input type=hidden name =a value =global_upg_rep>\n");
	printf("\n<input type=hidden name =fdt value = \"\">\n");
	printf("\n<input type=hidden name =tdt value = \"\">\n");
        do_log("creating form");
	printf("<br><br>");
        printf("\n<label>From Date  : </label>  \n");
	printf("<input id=""tfdate"" name=""tfdate"" type=""text"" readonly onclick=\"NewCal('tfdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('tfdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
	printf("\n<br><label>To Date  : </label>  \n");
	printf("<input id=""ttdate"" name=""ttdate"" type=""text"" readonly onclick=\"NewCal('ttdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('ttdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
	printf("\n<input type=button name=btnsummary value=Show summary onclick=\"global_upg_rep(document.frmglobalupgrep.tfdate.value,document.frmglobalupgrep.ttdate.value,document.frmglobalupgrep.dwnldupg.value);\">");
	printf("\n<input type=checkbox name =\"dwnldupg\" value =\"dwnld\"> Download CSV<br>\n");
        printf("\n<div id=\"gupgsummary\">");
        printf("\n<b>Global Upgrade Summary</b>:<br>");
        printf("\n</div>");
        printf("\n</div>");
        printf("\n</form>");
        draw_end();	
	return 0;
}

int draw_global_ref_report_form(){
	int i;
        do_log("inside global_upgrade_report");
        draw_header(7);
        do_log("header");
        printf("\n<div id=mh style=""width:99%;height:95%"">");
        printf("\n<form name=frmglobalrefrep action=simmisn.e method=post>");
	printf("\n<input type=hidden name =a value =global_ref_rep>\n");
	printf("\n<input type=hidden name =fdt value = \"\">\n");
	printf("\n<input type=hidden name =tdt value = \"\">\n");
        do_log("creating form");
	printf("<br><br>");
        printf("\n<label>From Date  : </label>  \n");
	printf("<input id=""tfdate"" name=""tfdate"" type=""text"" readonly onclick=\"NewCal('tfdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('tfdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
	printf("\n<br><label>To Date  : </label>  \n");
	printf("<input id=""ttdate"" name=""ttdate"" type=""text"" readonly onclick=\"NewCal('ttdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('ttdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
	printf("\n<input type=button name=btnsummary value=\"Show summary\" onclick=\"global_ref_rep(document.frmglobalrefrep.tfdate.value,document.frmglobalrefrep.ttdate.value,document.frmglobalrefrep.dwnldref.value);\">");
	printf("\n<input type=checkbox name =\"dwnldref\" value =\"dwnld\"> Download CSV<br>\n");
        printf("\n<div id=\"grefsummary\">");
        printf("\n<b>Global Refill Summary</b>:<br>");
        printf("\n</div>");
        printf("\n</div>");
        printf("\n</form>");
        draw_end();	
	return 0;
}

int draw_ddpage(fax_database *db){
	char command[1024];
        int i;
        //fax_database db(E2FAXDB,DBSERV_R);
	char *user=cgi_getentrystr("b");
        printf("\n<hr>");
        printf("   <b>\n<u>Your phone Info:</u></b><br><br>");
        printf("\n<table  width=99%>");
        printf("\n<b><tr class=mh ><th>NAME</th><th>SUBSTITUTE NO.</th><th>DESTINATION NO.</th></tr></b>");
        sprintf(command,"select * from cb_addressbook where username='%s' order by acc_num",user);
	do_log(command);
        printf("<tbody>");
	int cnt=db->ExecTuplesOk(command);
        for(i=0;i<cnt;i++)
        {
              printf("<tr class=Ml_nob>");
              printf("<td align=center>%s</td><td align=center>+%s</td><td align=center>%s</td>",db->get_field(i,"name"),db->get_field(i,"acc_num"),db->get_field(i,"destination"));
              if (isAdmin){
		printf("<td><a href=""Javascript:editddno('%s','%s','%s','%s');"">Edit</a></td>",user,db->get_field(i,"acc_num"),db->get_field(i,"destination"),replacestring(db->get_field(i,"name")," ","%20"));
		printf("<td><a href=""Javascript:delddno('%s','%s','%s');"">Delete</a></td>",user,db->get_field(i,"acc_num"),db->get_field(i,"destination"));
	      }
              printf("</tr>");
         }
         printf("\n</tbody>");
         printf("\n</table>");
         printf("\n<br>");
	if (isAdmin){
         sprintf(command,"select * from incoming_numbers where type='pub_callback' and number not in(select acc_num from cb_addressbook where username='%s') order by number",user);
         do_log(command);
         printf("        <input type=text name=txtname>");
         printf("                        <select name=selnumber>");
         printf("<option value=none>Select substitute no.</option>");
	 int rec_cnt=db->ExecTuplesOk(command);
         for(i=0;i<rec_cnt;i++)
         {
               printf("<option value='%s'>+%s</option>",db->get_field(i,"number"),db->get_field(i,"number"));
         } 
         printf("</select>");
         printf("                        <input type=text name=txtdestination >");
         printf("        <input type=button name=btnadd value=ADD onclick=\"return addddno(%s);\">",user);
         printf("\n<div id=\"dadd\">");
         printf("<input type=hidden name=cdest id=\"cdest\" value="">");
         printf("</div>");
         printf("<br><hr><br>");
	if(cnt>=1)
	{
		sprintf(command,"SELECT max(dateandtime) from cb_addressbook where username ='%s'",user);
		//sprintf(command,"select to_char((SELECT max(dateandtime)::timestamp from cb_addressbook where username ='%s'),'yyyy-mm-dd HH24:MI:SS') as max",user);
		do_log(command);
		db->ExecTuplesOk(command);
		printf("<b><u>Last Update Date:</b></u> %s",db->get_field(0,"max"));
	}
         printf("<br><br><hr>");
	}
	 return 0;
}
//void get_clientinfo(const char *salutation,const char *clientname,const char *companyname,const char *add_bldg,const char *add_street,const char *add_city,const char *add_pinno,const char *add_state,const char *add_country,const char *deliver_add_bldg,const char *deliver_add_street,const char *deliver_add_city,const char *deliver_add_pinno,const char *deliver_add_state,const char *deliver_add_country,const char *telno,const char *mobno,const char *emailadd,const char *creditcard_no,const char *cc_holder_name,const char *expdate,const char *sales_person,char *passport_no,char *passport_city,char *domainname,char *categ){
void get_clientinfo(const char *salutation,const char *clientname,const char *companyname,const char *add_bldg,const char *add_street,const char *add_city,const char *add_pinno,const char *add_state,const char *add_country,const char *deliver_add_bldg,const char *deliver_add_street,const char *deliver_add_city,const char *deliver_add_pinno,const char *deliver_add_state,const char *deliver_add_country,const char *telno,const char *mobno,const char *emailadd,const char *creditcard_no,const char *cc_holder_name,const char *expdate,const char *sales_person,char *passport_no,char *passport_city,char *domainname,char *categ,const char *reseller_sp){
	printf("<label>Category</label><input type=text name=categ value=\"%s\" readonly>\n",categ);
 	printf("\n<br><label>Domain</label><input type=text name=txtname value=\"%s\" readonly>",domainname);
 	printf("\n<br><label>Contact Person Name:</label><input type=text name=txtname value=\"%s%s\" readonly>",salutation,clientname);
	if((companyname==NULL)||companyname[0]==0)
		companyname="";	
	printf("\n<br><label>Company Name:</label><input type=text  name=txtcompany  value=\"%s\" readonly>",companyname);
	if((add_bldg==NULL)||add_bldg[0]==0)
		add_bldg="";
        printf("\n<br><label>Building Name:</label><input type=text name=txtbldg value=\"%s\" readonly>",add_bldg);
        if((add_street==NULL)||add_street[0]==0)
               	add_street="";
        printf("\n<br><label>Street:</label><input type=text name=txtstreet value=\"%s\" readonly> ",add_street);
        if((add_city==NULL)||add_city[0]==0)
               	add_city="";
	printf("\n<br><label>City:</label><input type=text name=txtcity value=\"%s\" readonly>",add_city);
        if((add_pinno==NULL)||add_pinno[0]==0)
               	add_pinno="";
	printf("\n<br><label>Pin Code:</label><input type=text name=txtpin value=\"%s\" readonly>",add_pinno);
        if((add_state==NULL)||add_state[0]==0)
               	add_state="";
	printf("\n<br><label>State:</label><input type=text name=txtstate value=\"%s\" readonly>",add_state);
        if((add_country==NULL)||add_country[0]==0)
		add_country="";
	printf("\n<br><label>Country:</label><input type=text name=txtcountry value=\"%s\" readonly>",add_country);
        printf("\n<br><br><b><u>Delivery address:</u></b><br>");
        if((deliver_add_bldg==NULL)||deliver_add_bldg[0]==0)
               	deliver_add_bldg="";
	printf("\n<br><label>Building Name:</label><input type=text name=txtdbldg value=\"%s\" readonly>",deliver_add_bldg);
        if((deliver_add_street==NULL)||deliver_add_street[0]==0)
               	deliver_add_street="";
	printf("\n<br><label>Street:</label><input type=text name=txtdstreet value=\"%s\" readonly>",deliver_add_street);
        if((deliver_add_city==NULL)||deliver_add_city[0]==0)
               	deliver_add_city="";
	printf("\n<br><label>City:</label><input type=text name=txtdcity value=\"%s\" readonly>",deliver_add_city);
        if((deliver_add_pinno==NULL)||deliver_add_pinno[0]==0)
               	deliver_add_pinno="";
	printf("\n<br><label>Pin Code:</label><input type=text name=txtpin value=\"%s\" readonly>",deliver_add_pinno);
        if((deliver_add_state==NULL)||deliver_add_state[0]==0)
               	deliver_add_state="";
	printf("\n<br><label>State:</label><input type=text name=txtdstate value=\"%s\" readonly>",deliver_add_state);
        if((deliver_add_country==NULL)||deliver_add_country[0]==0)
               	deliver_add_country="";
	printf("\n<br><label>Country:</label><input type=text name=txtcountry value=\"%s\" readonly>",deliver_add_country);
        if((telno==NULL)||telno[0]==0)
               	telno="";
	printf("\n<br><br><b><u>Contact details:</u></b><br>");
	printf("\n<br><label>Phone no:</label><input type=text name=txtphone value=\"%s\" readonly>",telno);
        if((mobno==NULL)||mobno[0]==0)
              	mobno="";
	printf("\n<br><label>Mobile no:</label><input type=text name=txtmobno value=\"%s\" readonly>",mobno);
        if((emailadd==NULL)||emailadd[0]==0)
               	emailadd="";
	printf("\n<br><label>Email Id:</label><input type=text name=txtemail value=\"%s\" readonly >",emailadd);
        printf("\n<br><br><b><u>Credit Card Details:</u></b><br>");
        if((creditcard_no==NULL)||creditcard_no[0]==0)
               	creditcard_no="";
	else if(!can_view_cc){	//else if (!isAdmin){} 
		char cc_num[31];
		char cc_num_show[31];
		strcpy(cc_num,creditcard_no);
		creditcard_no=hide_cc_number(cc_num,cc_num_show);
	}
	printf("\n<br><label>Credit Card No:</label><input type=text name=txtccno value=\"%s\" readonly>",creditcard_no);
        if((cc_holder_name==NULL)||cc_holder_name[0]==0)
               	cc_holder_name="";
	printf("\n<br><label>Name On Credit Card:</label><input type=text name=txtnccno value=\"%s\" readonly>",cc_holder_name);
        if((expdate==NULL)||(expdate[0]==0))
               	expdate="";
	printf("\n<br><label>Credit Card Expiry Date:</label><input type=text name=txtexp value=\"%s\" readonly>",expdate);
        printf("\n<br><br><b><u>Passport Details:</u></b><br>");
	printf("\n<br><label>Passport No:</label><input type=text name=pno value=\"%s\" readonly>",passport_no);
	printf("\n<br><label>City of issue:</label><input type=text name=ppcity  value=\"%s\" readonly>",passport_city);
	 printf("\n<br><br><b><u>Sales:</u></b><br>");
        if((sales_person==NULL)||sales_person[0]==0)
               	sales_person="";
	printf("\n<br><label>Sales Person / Reseller :</label><input type=text name=txtsalesp value=\"%s\" readonly>",sales_person);
	if( (reseller_sp == NULL ) || (reseller_sp[0]==0) )
		reseller_sp="";
	printf("\n<br><label>Reseller Sales Person:</label><input type=text name=txtresellersp value=\"%s\" readonly>",reseller_sp);
}
int show_status(fax_database *simdb,char *simno){
	char query[1024];
	//fax_database simdb(E2FAXDB,DBSERV_R);
	int i;
	char *country=cgi_getentrystr("selcountry");
	//do_log(country);
	if(!strcmp(country,"none")){
		sprintf(query,"select country from sim_stock where sim_phone_no='%s'",simno); 
		if ((simdb->ExecTuplesOk(query))>0)
			country=simdb->get_field(0,"country");
	}	
	do_log(country);
        printf("\n<form name=frmupdatesimstatus action=simmisn.e method=post>");
	sprintf(query,"select status,tripid,comment from clienttrip where sim_phone_no in(select sim_phone_no from sim_stock where active=true and sim_phone_no='%s')order by booking_date desc",simno);
	do_log(query);
	if ((simdb->ExecTuplesOk(query))>0){
		printf("\n<b><center>Status for %s is  %s</center><br></b>",simno,simdb->get_field(0,"status"));
		printf("\n<br><label>Comment:</label>");
	        printf("\n &nbsp &nbsp <input type=textarea name=txtcomment value=\"%s\"><br>",simdb->get_field(0,"comment"));
        	printf("\n<label>Pod No:</label>");
	        printf("\n &nbsp &nbsp <input type=text name=txtpod><br>");
        	printf("\n<label>Select Sim Status:</label>");
    	    	printf("\n &nbsp &nbsp <select name=selstatus onchange=\"new net.ContentLoader('simmisn.e?a=getbdate&b=%s&c='+this.value,parseMyData,'dbdate',null,'Get');\">",simno);
        	printf("<option value=none>Select Sim Card Status</option>");
        	printf("<option value=Delivered>Sim Card Delivered</option>");
        	printf("<option value=Returned>Sim Card Returned</option>");
        	printf("<option value=Canceled>Trip Canceled</option>");
        	printf("<option value=\"cardlost\">Sim Card Lost</option>");
        	printf("<option value=\"Faulty\">Faulty</option>");
        	printf("</select><br>");

		/*
		sprintf(query,"select * from sim_accessories_stock where tripid='%s' and active=true;",simdb->get_field(0,"tripid"));
		do_log(query);
		for(i=0;i<simdb->ExecTuplesOk(query);i++){
				printf("<label>%s status</label>",simdb->get_field(i,"type"));
				printf("\n &nbsp &nbsp <select name=%sstatus>",simdb->get_field(i,"type"));
				printf("<option value=none>--alloted--</option>");
				printf("<option value=Returned>Returned</option>");
				printf("<option value=\"lost\">Lost</option>");
				printf("</select><br>");
		}
		*/
		printf("\n<input type=hidden name=selphone value=%s>",simno);
		printf("\n<input type=hidden name=selcountry value='%s'>",country);
		printf("\n<input type=hidden name=b>");
		printf("\n<input type=hidden name=a>");
		printf("\n<input type=hidden name=return_date_restrict value=%d>", return_date_restrict);	//flag for imposing 7 day condition
        	printf("\n<label>Date Of Update:</label>");
        	printf(" &nbsp &nbsp <input id=""tupddate"" name=""tupddate"" type=""text"" readonly onclick=\"NewCal('tupddate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('tupddate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
        	printf("\n<div id=\"dbdate\"><input type=hidden name=bdate></div>");
        	printf("\n<br><input type=submit name=subupdate value=Update onclick=\" return updatetrip();\">");
		
	}
	else{
		sprintf(query,"select active,availability from sim_stock where sim_phone_no='%s'",simno);
		do_log(query);
		if((simdb->ExecTuplesOk(query))>0){
			if(!strcmp(simdb->get_field(0,"active"),"t")&&(!strcmp(simdb->get_field(0,"availability"),"t")))
				printf("\n<b> %s is active and available for booking</b><br>",simno);
			else if((!strcmp(simdb->get_field(0,"active"),"f")))
				printf("\n<b> %s is not active<br></b>",simno);
			else if(!strcmp(simdb->get_field(0,"active"),"t")&&(!strcmp(simdb->get_field(0,"availability"),"f")))
				printf("\n<b>%s is active and not available for booking but no trips are registered against it.</b>",simno); 	
		}
		else
			printf("\n<b>Could not find %s in sim stock</b><br>",simno);
	}
	printf("<input type=button name=back value=Back onclick=\"moveback();\">");
	printf("\n</form>");
	return 0;
}
void draw_date(char const *objname){
	int i;
	printf("\n<select name=objname>");
        printf("<option value=none>Day</option>");
        for(i=1;i<32;i++)
                printf("<option value=%d>%d</option>",i,i);
        printf("</select>");
}
void draw_year(char const *objname){
	int i;
	printf("\n<select id=%s name=%s>",objname,objname);
        printf("<option value=none>Year</option>");
        for(i=2010;i<2030;i++)
                printf("<option value=%d>%d</option>",i,i);
        printf("</select>");
}
void draw_month(char const *objname){
	char command[1024];
        int i;
        fax_database db(E2FAXDB,DBSERV);
	printf("\n<select id=%s name=%s>",objname,objname);
	char *expmonth[12]={"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};
        printf("<option value=none>Month</option>");
	for(i=0;i<12;i++)
        {
                        printf("<option value=%d>%s</option>",i+1,expmonth[i]);
        }
	printf("</select>");
}
int draw_tripdetails(char *frm,char *country){ 		//int draw_tripdetails(const char *frm,const char *country)
	char command[1024];
	int i;
	fax_database db(E2FAXDB,DBSERV_R);
	fax_database db1(E2FAXDB,DBSERV_R);
	do_log("form name:");
	do_log(frm);
	char *userid=cgi_getentrystr("c");
	//do_log("User ID:");
	//do_log(userid);
	//sprintf(command,"SELECT X.bal from (SELECT min(closingbalance) as bal from billdetails where username ='%s')X where X.bal < 0",userid);
	//sprintf(command,"SELECT coalesce((SELECT abs((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT closingbalance as bal from billdetails where username ='%s' order by generatedon desc limit 1)X where X.bal < 0)) as numeric), 2)))),0) as bal",userid);
	sprintf(command,"SELECT coalesce((SELECT abs((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT (SELECT sum(amount)  from payment_discount p where p.username ='%s' and sim_card_type='country' and accept_time >(select max(generatedon) from billdetails where username='%s')) - (SELECT abs((SELECT closingbalance from billdetails where username='%s' order by generatedon desc limit 1))) as bal)X where X.bal < 0)) as numeric), 2)))),0) as bal",userid,userid,userid);
	do_log(command);
        //printf("\n<b><u>Outstanding Payment:</u></b><br>");
        //printf("\n<br><label>Your Payment Due (in USD):</label>");
	db.ExecTuplesOk(command);
	if(strcmp(db.get_field(0,"bal"),"0"))
	{
		printf("\n<b><u>Outstanding Payment:</u></b><br>");
        	printf("\n<br><label>Your Payment Due (in USD):</label>");
		//printf("<input type=text name =bal_due value=\"%.2f\" disabled><br>",atof(db.get_field(0,"bal")));
		printf("<font color=\"red\" size=3>&nbsp &nbsp%.2f &nbsp$</font><br>",atof(db.get_field(0,"bal")));
	}
	printf("\n<br><b><u>Handset details:</u></b><br>");
	printf("\n<br><label>Use office Handset:</label>");
	printf("\n<input type=checkbox name=ofc_handset value=\"ofc_handset\" onclick=\"toggleofchandset('%s');\">",frm);
	printf("\n<select id=selhandset name=selhandset style=\"visibility:hidden\" onchange=\"txthandset('%s');\">",frm);
	printf("\n<option value='none'>Select Handset</option>");
	/*
	sprintf(command,"SELECT * from sim_accessories_stock where alloted =false and active=true and type='Handset' order by model,serialno");
	do_log(command);
	int cnt=db.ExecTuplesOk(command);
	//do_log((char *)cnt);
	if(cnt>0)
	{
		//printf("inside if");
		//for(i=0;i<db.ExecTuplesOk(command);i++)
		for(i=0;i<cnt;i++)
		{
			printf("<option value='%s'>%s</option>",db.get_field(i,"display_name"),db.get_field(i,"display_name"));
		}
	}
	else
	{
		printf("<b>No Handsets are available</b>");
	}*/
	printf("</select>");
	printf("&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; <b id=\"lbl_provide_app\" style=\"visibility:hidden\">Provide App:</b>");
	printf("\n<input type=checkbox id=\"chk_provide_app\" name=provide_app value=\"provide_app\" style=\"visibility:hidden\">");
	printf("\n<br><label>Enter the Handset model:</label>");
	printf("<input id=txt_hs type=text name=hs value=\"\"><br>\n");
	//if(!strcmp(ECS,country))				//Code for Local number generation Uncomment this if needed
	//{
	//	printf("\n<br><b><u>Travelling Countries:</u></b><br>");
	//	printf("<br><label>Select The Countries to Travel:</label>");
	//	printf("\n<select name=\"sel_europe\" id=\"sel_europe\" multiple=\"multiple\">");
	//	sprintf(command,"SELECT distinct country from travelling_country order by country");
	//	for(i=0;i<db.ExecTuplesOk(command);i++)
	//	{
	//		 printf("<option value=\"%s\">%s</option>",db.get_field(i,"country"),db.get_field(i,"country"));
	//	}
	//	 printf("</select>"); 
	//}
	printf("\n<br><b><u>Trip details:</u></b><br>");
	printf("\n<br><label>Select Sim Phone Number to be alloted:</label>");
	printf("\n<select name=selsimno>");
	printf("<option value='none'>select number</option>");
	sprintf(command,"select sim_phone_no from sim_stock where country='%s' and active=true and availability=true order by sim_phone_no;",country);
	int cnt=db.ExecTuplesOk(command);
	//for(i=0;i<db.ExecTuplesOk(command);i++)
	for(i=0;i<cnt;i++)
	{
		printf("<option value='%s'>%s</option>",db.get_field(i,"sim_phone_no"),db.get_field(i,"sim_phone_no"));
	}
	printf("</select>(optional)");
	printf("\n<input type=hidden name=expdate>");
        printf("\n<br><label>Trip Start Date in(yyyy-mm-dd) format:</label><input id=""tsdate"" name=""tsdate"" type=""text"" readonly onclick=\"NewCal('tsdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('tsdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>"); 
        printf("<br><label>Trip End Date in(yyyy-mm-dd) format:</label><input type=text id=tedate name=tedate readonly onclick=\"NewCal('tedate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('tedate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
	printf("\n<br><label>Sim Card Delivery Date in(yyyy-mm-dd) format:</label><input type=text id=tddate name=tddate readonly onclick=\"NewCal('tddate','yyyymmdd');\"><a href=""javascript:NewCal('tddate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
	printf("<div></div>\n");

//ADD CODE to get UNLIMITED options
	printf("<input type=hidden name=ucs>\n");
	string query = string("SELECT countrycode FROM countrydata WHERE countryname = '") + country + "';";
	do_log(to_c_string(query));
	if(db.ExecTuplesOk(to_c_string(query)))
	{
		//Read the tag
		bool print_fdate = 0;
		string country_code = db.get_field(0, "countrycode");
		do_log(to_c_string(country_code));

//		string sequence = read_tag_attribute("UNLIMITED_CALLING", country_code);
		
		char query1[1024];
                sprintf(query1,"UNLIMITED_CALLING_%s",to_c_string(country_code));
                string sequence = get_simmis_data_from_table(query1,"");



		vector<string> tokens = splitString(sequence, ',');
		int ts = tokens.size();
		do_log(to_c_string(string("--Tokens read: " + ts)));
		if(!(ts == 1 && tokens[0] == "0"))
		{
			print_fdate = 1;
			cout << "\n\n<br><div>";
			printf("\n<br><label>Enable Unlimited Calling options:</label>");
			printf("<input type='checkbox' name=uc id='uc' value='uc' onclick=\"check_uc(document.getElementById('tsdate').value,'%s','uc');\"><br>\n",frm);
		//	printf("<input type='checkbox' name=uc id='uc' value='uc' onclick=\"check_uc(document.getElementById('f_date').value,'%s','uc');\"><br>\n",frm);
			cout << "<label>Duration</label>";
			for(vector<string>::const_iterator i = tokens.begin(); i != tokens.end(); ++i)
			{
				do_log(to_c_string(*i));
				cout	<< "\n\t<input type='radio' name='uc_op' value='" + *i + "' />"
					<< "<span>" + *i + " days</span>";	
			}
		}
		sequence="";
//		string data_plan = read_tag_attribute("UNLIMITED_DATA", country_code);

		 char query2[1024];
                 sprintf(query2,"UNLIMITED_DATA_%s",country_code.c_str());
                 string data_plan = get_simmis_data_from_table(query2,"1");



		do_log("data_plan");
		do_log(to_c_string(data_plan));
		if (data_plan.compare("1") == 0){
			print_fdate = 1;
			cout << "\n\n<br>";
			printf("\n<label>Unlimited Data:</label>"); 
 			printf("<input type=hidden name=udata>\n");
			do_log("UNLIMITED_DATA enabled");
			printf("<input type='checkbox' name=ucdata id ='ucdata' onclick=\"check_uc(document.getElementById('tsdate').value,'%s','udata');\"><br>\n",frm); 
			//printf("<input type='checkbox' name=ucdata id ='ucdata' onclick=\"get_f_date();check_uc(document.getElementById('f_date').value,'%s','udata');\"><br>\n",frm); 
		}
		/************************************Earlier Correct COde****************************
		if (print_fdate){
			printf("\n<br>\n<label>From Date:</label>\n<input type=text id=uc_fdate name=uc_fdate readonly onclick=\"NewCal('uc_fdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\">\n<a href=""javascript:NewCal('uc_fdate','yyyymmdd');"">\n\t<img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date"">\n</a>");
			cout << "\n</div>\n";
		}****************************************Ends Here & My Code Starts***********************************
		if (print_fdate){
			
		       	printf("</b><input type=radio name=uc_frm_date id=future_from_date value='' onclick=\"document.getElementById('uc_fdate').disabled=true;get_f_date();\">On Lu Update<br><br>");
			printf("\n<br><br><input type=radio name=uc_frm_date id=fixed_from_date onclick=\"document.getElementById('uc_fdate').disabled=false;get_f_date();\">Fixed Date : &nbsp &nbsp <input type=text id=uc_fdate name=uc_fdate disabled onclick=\"NewCal('uc_fdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();get_f_date();\">\n<a href=""javascript:NewCal('uc_fdate','yyyymmdd');"">\n\t<img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date"">\n</a>");
		
		     ///Hidden field  	
			printf("</b><input type=hidded name=uc_fdate id=uc_fdate value=''");
			//printf("</b><input type=radio name=uc_frm_date id=future_from_date value='' onclick=\"document.getElementById('uc_fdate').disabled=true;get_f_date();\">On Lu Update<br><br>");
		}/***************************************My Code Ends*******************************/

		/*************************Another trial**********************************************/

		if (print_fdate){
			printf("\n<br><br><b><u>Choose Plan Activation Date</u></b>");
			printf("\n<br><br><input type=radio name=from_date id=fixed_date onclick=\"document.getElementById('future_date').disabled=false;get_f_date_temp();\">Fixed Date : &nbsp &nbsp <input type=text id=date name=date onclick=\"NewCal('date','yyyymmdd');\" onkeypress=\"newwindow.document.close();\">\n<a href=""javascript:NewCal('date','yyyymmdd');"">\n\t<img src=""cal.gif"" width=""16"" height=""16"" border=""0"" id=cal alt=""Pick a date"" hidden >\n</a>");
			//printf("\n<br><br><input type=radio name=from_date id=fixed_date onclick=\"document.getElementById('future_date').disabled=false;get_f_date_temp();\">Fixed Date : &nbsp &nbsp <input type=text id=datepick name=datepick>\n\t<img src=""cal.gif"" width=""16"" height=""16"" border=""0"" id=""cal"" alt=""Pick a date"" hidden >\n</a>");
                        //printf("<input type=hidden id=dp >");
			printf("</b><br><input type=radio name=from_date id=future_date value='' onclick=\"document.getElementById('date').disabled=true;get_f_date_temp();\">On Lu Update<br>");
			    ///Hidden field    
                       printf("<input type=hidden name=uc_fdate id=uc_fdate value=''>");

		}
		/*******************************End Here*********************************************/
		else
			cout << "\n</div>\n";
	}
	
	
	
	/*
	sprintf(command,"SELECT * from countrydata where countryname ='%s' and postpaid_prepaid='postpaid' and card_type='country_specific'",country);
	do_log(command);
	if (db.ExecTuplesOk(command))
	{
		printf("<input type=hidden name=ucs>\n");
		printf("\n<br><label>Enable Unlimited Calling options:</label>");
		printf("<input type=checkbox name=uc id =uc value=\"uc\" onclick=\"check_uc(document.getElementById('tsdate').value,'%s');\">\n",frm);
		char country_code_arr[51]="852 66 60 86 65 62";			//Country code of South East Asian countries
                char ccode[5];
                strcpy(ccode,db.get_field(0,"countrycode"));
                do_log(ccode);
                if (strstr(country_code_arr,ccode))
		{
			printf("<br><label>Duration:</label><input type=radio name=uc_op value=\"8\" checked=checked>8 Days <input type=radio name=uc_op value=\"15\">15 Days\n");
		}
		else
		{
			printf("<br><label>Duration:</label><input type=radio name=uc_op value=\"15\" checked=checked>15 Days <input type=radio name=uc_op value=\"30\">30 Days\n");
		}
		printf("<br><label>From Date:</label><input type=text id=uc_fdate name=uc_fdate readonly onclick=\"NewCal('uc_fdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('uc_fdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
	}
	else					//Unlimited Calling option for Prepaid Clients
	{
		sprintf(command,"SELECT * from countrydata where countryname ='%s'",country);
		db.ExecTuplesOk(command);
		char country_code_arr[51]="BB BIS";
		char ccode[5];
		strcpy(ccode,db.get_field(0,"countrycode"));
		do_log(ccode);
		if (!strstr(country_code_arr,ccode))
		{
                	printf("<input type=hidden name=ucs>\n");
                	printf("\n<br><label>Enable Unlimited Calling options:</label>");
                	printf("<input type=checkbox name=uc id =uc value=\"uc\" onclick=\"check_uc(document.getElementById('tsdate').value,'%s');\">\n",frm);
                	printf("<br><label>Duration:</label><input type=radio name=uc_op value=\"15\" checked=checked>15 Days\n");// <input type=radio name=uc_op value=\"30\">30 Days\n");
                	printf("<br><label>From Date:</label><input type=text id=uc_fdate name=uc_fdate readonly onclick=\"NewCal('uc_fdate','yyyymmdd');\" onkeypress=\"newwindow.document.close();\"><a href=""javascript:NewCal('uc_fdate','yyyymmdd');""><img src=""cal.gif"" width=""16"" height=""16"" border=""0"" alt=""Pick a date""></a>");
		}
	}
	*/
	printf("<div></div>\n");
	printf("<br><br><b><u>Case Source:</u></b><br>\n");
	char case_src[1024];
	char case_src_array[CASE_SRC_CNT][251];
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- CASE_SOURCE","-- CASE_SOURCE>","case_src",case_src)!=1) || (case_src==NULL || case_src[0]==0)){
		do_log("Could not open config file.");
		printf("Error occured while opening config file.");
	}*/

	      char* data25 = get_simmis_data_from_table("CASE_SOURCE_case_src","");
                        if(data25[0]==0 || data25==NULL)
                        {
                                  printf("Error occured while opening config file.");
                                do_log ("ERROR OCCURED WHILE GETTING CASE_SRC data25");
                        }
                        strcpy(case_src,data25);


	do_log(case_src);
	if(str_split(',',case_src,case_src_array)<1){
		do_log("failed to split");
		printf("There was an error, please try after some time\n");
	}
	printf("\n<label>Case Source</label>:");
	printf("\n<select name=\"sel_case\" id=\"sel_case\" onchange=\"set_final_fdate();toggle_crm_options('%s');\">",frm);
	//printf("\n<select name=\"sel_case\" id=\"sel_case\" onchange=\"toggle_crm_options('%s');\">",frm);
	printf("\n<option value=none>Select Case Source</option>");
	int length1=sizeof(case_src_array)/sizeof(case_src_array[0]);
	for (int i=0;i<length1;i++){
		if (strlen(case_src_array[i])>0)
			printf("\n<option value=\"%s\">%s\n",case_src_array[i],case_src_array[i]);
	}
	printf("\n</select>");

	char crm_opt[1024];
	char crm_opt_array[CRM_OPT_CNT][251];
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- CASE_SOURCE","-- CASE_SOURCE>","crm_opt",crm_opt)!=1) || (crm_opt==NULL || crm_opt[0]==0)){
	do_log("Could not open config file.");
		printf("Error occured while opening config file.");
	}*/

	  char* data26 = get_simmis_data_from_table("CASE_SOURCE_crm_opt","");
                        if(data26[0]==0 || data26==NULL)
                        {
                                  printf("Error occured while opening config file.");
                                do_log("ERROR OCCURED WHILE GETTING CRM_OPT data26");
                        }
                        strcpy(crm_opt,data26);






	do_log(crm_opt);
	if(str_split(',',crm_opt,crm_opt_array)<1){
		do_log("failed to split");
		printf("There was an error, please try after some time\n");
	}
	printf("<select name=\"sel_case_crm\" id=\"sel_case_crm\" style=\"visibility: hidden;\">");
	printf("<option value=none>Select Campaign Name</option>");
	int length2=sizeof(crm_opt_array)/sizeof(crm_opt_array[0]);
	for (int i=0;i<length2;i++){
		if (strlen(crm_opt_array[i])>0)
			printf("<option value=\"%s\">%s\n",crm_opt_array[i],crm_opt_array[i]);
	}
	printf("</select>");

	//CRM week option
     draw_crm_case_source_options();

//ADD CODE to get UNLIMITED options

	printf("\n<br><br><b><u>Payment Details:</u></b><br>\n");	
        printf("\n<br><label>Payment mode:</label><input type=radio name=pm id=ccno value=\"Credit Card\" checked=\"checked\" onchange=\"eval_radio('%s')\">Credit Card<input type=radio name=pm value=\"Corporate\" onchange=\"eval_radio('%s')\">Corporate<input type=radio name=pm value=\"Advanced Payment\" onchange=\"eval_radio('%s')\">Advanced payment<br>",frm,frm,frm);
        printf("\n<br><label>Credit Card No:</label><input type=text name=txtccno onblur=\"checkccno(this.value);\">");
        printf("\n<br><label>Name On Credit Card:</label><input type=text name=txtnccno>");
        printf("\n<br><label>Credit Card Expiry Date:</label>");//<input type=text name=expdate onfocus=\"GetDate(this);\" onclick=\"GetDate(this);\" Onkeypress=\"DestroyCalendar();\">");
	draw_month("selexpmonth");
	draw_year("selexpyear");
	printf("\n<br><pre id=lblccno>(Enter Credit Card Number to Continue)</pre>");
	
        if(!strcmp(frm,"frmexistinguser"))
	{
		char *email=cgi_getentrystr("d");
        	printf("\n<br><br><b><u>Email ID for receiving alerts:</u></b><br>\n");
        	printf("\n<br><label>Email ID:</label><input type=text size=30 id=txtemail value=\"%s\" onblur=\"valid_emailid(this.value);\" disabled>",email);
        	printf("\n<input type=button value=Change onclick=\"change_email('%s');\">",frm);
		printf("\n<input type=button id=\"btncancel\" value=Cancel onclick=\"disable_email();\" disabled>");
        	printf("\n<br><br>");
	}

	/*sprintf(command,"select distinct type from sim_accessories_stock where active=true and alloted=false;");
	int count=db.ExecTuplesOk(command);
	for(i=0;i<count;i++){
		if(!strcmp(db.get_field(i,"type"),"handset")){
			printf("\n<br><input type=checkbox name=chk_%s id=chk_%s onclick=\"enablesel('chk_%s','sbtdone');\">%s",db.get_field(i,"type"),db.get_field(i,"type"),db.get_field(i,"type"),db.get_field(i,"type"));
			int j;
			printf("\n<select name=selhandset id=selhandset disabled onchange=\"document.getElementById('sbtdone').disabled=false;\">");
			printf("<option value='none'>--Select Handset--</option>");
			sprintf(command,"select * from sim_accessories_stock where type='handset' and active=true and alloted=false;");
			int res=db1.ExecTuplesOk(command);
			for(j=0;j<res;j++)
				printf("<option value='%s'>%s-%s</option>",db1.get_field(j,"serialno"),db1.get_field(j,"model"),db1.get_field(j,"serialno"));
			printf("</select>");
		}
		else
			printf("\n<br><input type=checkbox name=chk_%s>%s",db.get_field(i,"type"),db.get_field(i,"type"));
	}*/

	return 0;
}
char *getnew_username(fax_database *dbuser,const char *mobno,const char *domainname)
{
	do_log("Inside getnew_username Function");
	char username[1024];
	char *newuser;
	char suffix[1024];
	char query[1024];
	char *prefix="15555";
	//fax_database dbuser(E2FAXDB,DBSERV);
	do_log((char*)mobno);
	sprintf(suffix,"%.*s",4,&mobno[strlen(mobno)-4]);
	do_log((char *)suffix);
	sprintf(username,"%s%s",prefix,suffix);
	do_log((char*)username);
	newuser=strdup((char *)username);
	sprintf(query,"select username from users where username='%s'",newuser);
	do_log(query);
	if(dbuser->ExecTuplesOk(query)<=0)
		return (char *)newuser;	
	else{
		while(true){
		sprintf(query,"select nextval('simuser_ids')");
		do_log(query);
		if(dbuser->ExecTuplesOk(query)>0){
			sprintf(suffix,"%s",dbuser->get_field(0,"nextval"));
			do_log((char *)suffix);	
			sprintf(username,"%s%s",prefix,suffix);
			do_log((char*)username);
			newuser=strdup((char *)username);
			sprintf(query,"select username from users where username='%s'",newuser);
			do_log(query);
			if(dbuser->ExecTuplesOk(query)<=0)
			{	
				do_log("getnew_username executed");
				return (char *)newuser;	
			}	
		}	
		}
	}
}
int booktrip(fax_database *db,char *country,char *userid,char *emailid){
	int sales_cnt=0;
	char sales[5][101];
	char *seqno;
	int count,i;
	char command[1024];
	char *username;
	char tariff[1024];
        sprintf(tariff,"");
	char domainname[201];
	char newdomain[201];
	sprintf(domainname,"");
	sprintf(newdomain,"");
	char tripid[1024],pwd[1024];
	char *simno=cgi_getentrystr("selsimno");
	char *isadv=cgi_getentrystr("advance");
	char accessories[1024];
	char *advid=cgi_getentrystr("advid");
	char clientname_shortened[21];
	bool run_uc_now = 0;
	char country_code[11];
	if(advid==NULL||advid[0]==0)
		advid=" ";
	else
		advid=strdup(advid);
	do_log(isadv);
	if(simno==NULL||simno[0]==0||(!strcmp(simno,"none")))
		simno="none";
	else
		simno=strdup(simno);
	do_log(simno);
	int j;
	//fax_database db(E2FAXDB,DBSERV);
	fax_database cb(E2FAXDB,DBSERV);
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_TARIFF","--SIM_TARIFF>","tariff",tariff)!=1) || (tariff==NULL || tariff[0]==0)){
                do_log("Could not open config file.");
                printf("Error occured while opening config file.");
        }*/

	  char* data27 = get_simmis_data_from_table("SIM_TARIFF","");
         if(data27[0]==0 || data27==NULL)
         {
                   printf("Error occured while GETTING DATA FROM CONFIG_TABLE.");
                   do_log("ERROR OCCURED WHILE GETTING SIM_TARIFF data27");
         }
         strcpy(tariff,data27);





	do_log(tariff);
	//char *primary_email=cgi_getentrystr("primary_email");
	//do_log(primary_email);
	char *expdate=cgi_getentrystr("expdate");
	if(expdate==NULL||expdate[0]==0)
		expdate=" ";
	else
		expdate=strdup(expdate);
	char *cc_holder_name=cgi_getentrystr("txtnccno");
	char *creditcard_no=cgi_getentrystr("txtccno");
	char *sales_person=cgi_getentrystr("e");
	int result=0;
	if(cc_holder_name==NULL||cc_holder_name[0]==0)
		cc_holder_name=" ";
	else
		cc_holder_name=strdup(cc_holder_name);
	if(creditcard_no==NULL||creditcard_no[0]==0)
		creditcard_no=" ";
	else
		creditcard_no=strdup(creditcard_no);
	if((sales_person==NULL)||(sales_person[0]==0))
		sales_person=" ";
	else
		sales_person=strdup(sales_person);

	char *fdate=cgi_getentrystr("tsdate");
	char *tdate=cgi_getentrystr("tedate");
	char *ddate=cgi_getentrystr("tddate");
	char *chkhandset=cgi_getentrystr("ofc_handset");
	char *provideapp=cgi_getentrystr("provide_app");
	char *chkreseller=cgi_getentrystr("chk_reseller");
	char *resellername;
	if (provideapp ==NULL || provideapp[0] == 0){
		provideapp=strdupa("no");
	}
	else
	{
		provideapp=strdupa("yes");
	}
	sprintf(command,"PROVIDE APP=%s",provideapp);
	do_log(command);
	if(chkreseller ==NULL || chkreseller[0]==0){
		do_log("chkreseller no");
		chkreseller=strdup("no");
	}
	else
	{
		do_log("chkreseller yes");
		chkreseller=cgi_getentrystr("e");
		resellername=cgi_getentrystr("selreseller");
		sales_person=strdupa(resellername);
	sprintf(command,"RESELLER SP: %s   RESELLER NAME:%s    SALES_PERSON:%s",chkreseller,resellername,sales_person);
	do_log(command);
	}
	//sprintf(command,"PROVIDE APP=%s",provideapp);
        //do_log(command);
	char *case_src=cgi_getentrystr("g");
	sprintf(command,"Case Source=%s",case_src);
	do_log(command);
	sprintf(command,"CHK HANDSET=%s",chkhandset);
        do_log(command);
	if(chkhandset ==NULL || chkhandset[0]==0)
	{
		do_log("User has not opted for office handset");
		chkhandset=strdupa("no");
	}
	char *country_travelling=cgi_getentrystr("f");
	sprintf(command,"CHK HANDSET=%s",chkhandset);
	do_log(command);
	char *passport_no = NULL;
	char *passport_city = NULL;
	if(!strcmp(userid,"new")){
		passport_no=cgi_getentrystr("pno");
		if (passport_no ==NULL || passport_no[0] == 0){
			do_log("passport_no missing");
			printf("passport_no is missing");
			return -1;
		}
		passport_city=cgi_getentrystr("ppcity");
		if (passport_city ==NULL || passport_city[0] == 0){
			do_log("passport_city missing");
			printf("passport_city is missing");
			return -1;
		}
		passport_no = strdupa(passport_no);
		passport_city = strdupa(passport_city);
	}
	if (cb.ExecCommandOk("begin work") != 1) {
		do_log("102-Could not begin work");
		printf("<b>Sorry!Could not connect to the server</b>");
		return 0;
	}
	if(!strcmp(simno,"none")){
		if(strcmp(userid,"new")){
			do_log("Line No-2900");
			//sprintf(command,"select sim_phone_no from sim_stock where country='%s' and availability='t' and active='t' and sim_phone_no in (select sim_phone_no from clienttrip where username='%s' order by booking_date desc) and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) limit 1",country,userid);
			sprintf(command,"select sim_phone_no from sim_stock where country in (select countryname from countrydata where country='%s' and active = true) and availability='t' and active='t' and sim_phone_no in (select sim_phone_no from clienttrip where username='%s' order by booking_date desc) and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) limit 1",country,userid);
			do_log(command);
			if(db->ExecTuplesOk(command)>0){
				simno=db->get_field(0,"sim_phone_no");
				if((simno==NULL)||(simno[0]==0))
					simno="";
				else
					simno=strdup(simno);
			}
			else{
				do_log("Line No-2911");
				//sprintf(command,"select sim_phone_no from sim_stock where country='%s' and availability='t' and active='t' and sim_phone_no not in(select sim_phone_no from clienttrip where status ='Returned') and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by dateandtime asc limit 1",country);
				sprintf(command,"select sim_phone_no from sim_stock where country in (select countryname from countrydata where country='%s' and active = true)  and availability='t' and active='t' and sim_phone_no not in(select sim_phone_no from clienttrip where status ='Returned') and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by dateandtime asc limit 1",country);
				do_log(command);
				if(db->ExecTuplesOk(command)!=1){
					do_log("line No-2915");
					sprintf(command,"select sim_phone_no from  sim_stock where sim_phone_no in(select sim_phone_no from clienttrip where return_date in (select max(return_date) from clienttrip group by sim_phone_no)order by return_date) and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) and availability=true and active=true and country in (select countryname from countrydata where country='%s' and active = true) limit 1;",country);
					do_log(command);
				}
				if(db->ExecTuplesOk(command)>0){
					simno=db->get_field(0,"sim_phone_no");
					if((simno==NULL)||(simno[0]==0))
						simno="";
					else
						simno=strdup(simno);
				}
			}
		}
		else{
			do_log("Line No-2929");
			//sprintf(command,"select  sim_phone_no from sim_stock where country in (select countryname from countrydata where country='%s') and availability='t' and active='t' and sim_phone_no not in(select sim_phone_no from clienttrip where status ='Returned') and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by dateandtime asc limit 1",country);
			sprintf(command,"select  sim_phone_no from sim_stock where country in (select countryname from countrydata where country='%s' and active = true) and availability='t' and active='t' and sim_phone_no not in(select sim_phone_no from clienttrip where status ='Returned') and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by dateandtime asc limit 1",country);
			do_log(command);
			if(db->ExecTuplesOk(command)!=1){
				do_log("Line No-2933");
				//sprintf(command,"select sim_phone_no from  sim_stock where sim_phone_no in(select sim_phone_no from clienttrip where return_date in (select max(return_date) from clienttrip group by sim_phone_no) and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by return_date) and availability=true and active=true and country='%s' limit 1;",country);
				sprintf(command,"select sim_phone_no from  sim_stock where sim_phone_no in (select sim_phone_no from clienttrip where return_date in (select max(return_date) from clienttrip group by sim_phone_no) and sim_phone_no not in (select sim_phone_no from sim_phone_no_lock) order by return_date) and availability=true and active=true and country in (select countryname from countrydata where country='%s' and active = true) limit 1;",country);
				do_log(command);
			  }
			if(db->ExecTuplesOk(command)>0){
				simno=db->get_field(0,"sim_phone_no");
				if((simno==NULL)||(simno[0]==0))
					simno="";
				else
					simno=strdup(simno);
			}
		}
	}
	else{
		do_log("Line No-2947");
		sprintf(command,"select * from sim_phone_no_lock where sim_phone_no='%s'",simno);
		do_log(command);
		if (db->ExecTuplesOk(command)){
			do_log("already booked in some other id");
			printf("The sim phone number %s is not available for booking now. Please select another card.",simno);
			do_log("rollback");
			cb.ExecCommandOk("rollback work");
			return 0; 
		}
	}
	do_log("Line No-2958");
	sprintf(command,"insert into sim_phone_no_lock(sim_phone_no) values('%s')",simno);
	do_log(command);
	if(!db->ExecCommandOk(command)){
		do_log("failed to add to sim_phone_no_lock");
		do_log("rollback");
		cb.ExecCommandOk("rollback work");
		return 0;
	}
	do_log("sim_phone_no_lock Added");
	char *hset = cgi_getentrystr("hs");
	if(!strcmp(isadv,"off")){
		do_log("updating Sim_stock");
		sprintf(command,"update sim_stock set availability=false where sim_phone_no='%s'",simno);
		do_log(command);
		if(cb.ExecCommandOk(command)<=0)
		{
			do_log("Failed TO Update Sim_stock");
			do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			cb.ExecCommandOk("rollback work");
			return 0;
		}
		do_log("sim_stock Updated");
		sprintf(command,"select nextval('trip_ids');");
		do_log(command);
		if(db->ExecTuplesOk(command)>0){
			seqno=db->get_field(0,"nextval");
			sprintf(tripid,"%s%s",country,seqno);
 		}
 		else{
			do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			db->ExecCommandOk("rollback work");
			return 0;
      		}
		do_log("before ofc_handset");
		if(!strcmp(chkhandset,"ofc_handset"))//Added this condition to check if office handset is given.
		{
			do_log("inside ofc_handset");
			char *selhset=cgi_getentrystr("selhandset");
			sprintf(command,"SELECTED HANDSET=%s",selhset);
			do_log(command);
			if (hset == NULL || hset[0]==0)
			{
				hset=strdupa(selhset);
			}
	/*	
			sprintf(command,"update sim_accessories_stock set alloted=true,tripid='%s' where display_name='%s'",tripid,selhset);
			do_log(command);
			if(cb.ExecCommandOk(command)<=0)
                        { 
                                do_log("rollback");
                                sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
                                do_log(command);
                                db->ExecCommandOk(command);
                                cb.ExecCommandOk("rollback work");
                                return 0;
                        }
			else
			{
				do_log("Update Successful.Handset Alloted");
			}*/
			/*sprintf(command,"update sim_accessories_stock set tripid='%s' where tripid='%s';",tripid,advid);
			do_log(command);
	                if(cb.ExecCommandOk(command)<=0)
	                { 
				do_log("rollback");
				sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
				do_log(command);
				db->ExecCommandOk(command);
				cb.ExecCommandOk("rollback work");
				return 0;
			}
			sprintf(command,"select distinct on (type) type,serialno from sim_accessories_stock where active=true and alloted=false;");
			do_log(command);
			count=db->ExecTuplesOk(command);
			for(i=0;i<count;i++)
			{
				char *type=db->get_field(i,"type");
				char *serial=db->get_field(i,"serialno");
				char chktype[1024];
				sprintf(chktype,"chk_%s",type);
				do_log(chktype);
				char *ischeck=cgi_getentrystr(chktype);
				if(ischeck==NULL||ischeck[0]==0)
					ischeck=strdup("off");
				else
					ischeck=strdup(ischeck);
				do_log(ischeck);
				if(!strcmp(ischeck,"on"))
				{
					sprintf(command,"update sim_accessories_stock set alloted=true,tripid='%s' where type='%s' and serialno='%s';",tripid,type,serial);
					do_log(command);
					if(cb.ExecCommandOk(command)<=0)
			                {
                			        do_log("rollback");
						sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
						do_log(command);
						db->ExecCommandOk(command);
                        			cb.ExecCommandOk("rollback work");
                        			return 0;
                			}
				}
        		}*/
		}
		do_log("After ofc Handset");
	}
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_DOMAIN","--SIM_DOMAIN>","domainname",domainname)!=1) || (domainname==NULL || domainname[0]==0)){
                        do_log("Could not open config file.");
                        printf("Error occured while opening config file.");
        }*/
	
	  char* data28 = get_simmis_data_from_table("SIM_DOMAIN_domainname","");
         if(data28[0]==0 || data28==NULL)
         {
                   printf("Error occured while GETTING DOMAINNAME FROM CONFIG_TABLE.");
                   do_log("ERROR OCCURED WHILE GETTING DOMAINNAME data28");
         }
         strcpy(domainname,data28);


	do_log("Line No-3074");
        do_log(domainname);
	char* domain_name = trimwhitespace(domainname);
	do_log(domain_name);
        //exit(0);
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- SIM_DOMAIN","--SIM_DOMAIN>","newdomain",newdomain)!=1) || (newdomain==NULL || newdomain[0]==0)){
                        do_log("Could not open config file.");
                        printf("Error occured while opening config file.");
        }*/
	
	  char* data29 = get_simmis_data_from_table("SIM_DOMAIN_newdomain","");
         if(data29[0]==0 || data29==NULL)
         {
                   printf("Error occured while GETTING NEWDOMAIN  FROM CONFIG_TABLE.");
                   do_log("ERROR OCCURED WHILE GETTING NEWDOMAIN data29");
         }
         strcpy(newdomain,data29);





        do_log(newdomain);
	char n_domain[201];
	int c=0,d=0;
	/***************************************My Code***************************************/
	while (newdomain[c] != '\0')
	{
	      if (!(newdomain[c] == ' ' && newdomain[c+1] == ' ')) {
	        n_domain[d] = newdomain[c];
	        d++;
	          }
	         c++;
	}
	n_domain[d] = '\0';
	strcpy(newdomain, n_domain);
	/****************************************Ends************************************************/
	do_log(newdomain);
	//exit(0);
	char *old_mobno=NULL;
	char *oldclientname=NULL;
	char oldusername[151];
	bool is_country_postpaid = 0;
	bool is_dd_enabled = 0;
	bool add_credit_on_booking = 0;
	bool add_e_credit_entry_on_booking = 0;
	bool add_reg_charge = 0;
	char *uc_fdate=NULL;
	sprintf(command,"SELECT * from countrydata where countryname='%s'",country);
	do_log(command);
	if(!db->ExecTuplesOk(command)){
		do_log("No country from countrydata");
		do_log("rollback");
		sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
		do_log(command);
		db->ExecCommandOk(command);
		cb.ExecCommandOk("rollback work");
		return 0;
	}
	strcpy(country_code,db->get_field(0,"countrycode"));
	if (!strcmp(db->get_field(0,"postpaid_prepaid"),"postpaid")) is_country_postpaid = 1;
	else is_country_postpaid = 0;
	if (!strcmp(db->get_field(0,"is_dd_enabled"),"1")) is_dd_enabled = 1;
	else is_dd_enabled = 0;
	if (is_dd_enabled) do_log("is_dd_enabled is 1");
	else do_log("is_dd_enabled is 0");
	if (!strcmp(db->get_field(0,"add_credit_on_booking"),"1")) add_credit_on_booking = 1;
	else add_credit_on_booking = 0;
	if (!strcmp(db->get_field(0,"add_e_credit_entry_on_booking"),"1")) add_e_credit_entry_on_booking = 1;
	else add_e_credit_entry_on_booking = 0;	
	if (!strcmp(db->get_field(0,"add_reg_charge"),"1")) add_reg_charge = 1;
	else add_reg_charge = 0;
	if (!strcmp(userid,"new")) strcpy(domainname,newdomain);
	else if (strcmp(domainname,newdomain))
	{
		sprintf(command,"select * from sim_user where username='%s'",userid);
		do_log(command);
		if (!db->ExecTuplesOk(command))
		{
			do_log("No user found");
			do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			cb.ExecCommandOk("rollback work");
			return 0;
		}
		oldclientname = db->get_field(0,"clientname");
		oldclientname = strdupa(oldclientname);
		old_mobno = strdupa(db->get_field(0,"mobno"));
		if(strcmp(db->get_field(0,"domainname"),newdomain)){
			bool copy_trikonids_tsim = 0;
			//check if there is a trikon - tsim map
			sprintf(command,"select * from trikon_tsim_map where oldusername='%s' and olddomainname='%s'",userid,domainname);
			do_log(command);
			if (db->ExecTuplesOk(command))
			{
				char *newusername=strdupa(db->get_field(0,"newusername"));
				sprintf(command,"SELECT * from clienttrip where username ='%s' and status in ('Delivered','Delivery pending') and sim_phone_no in (select sim_phone_no from sim_stock where country in ( select countryname from countrydata where postpaid_prepaid='postpaid'))",userid);
				do_log(command);
				if (!db->ExecTuplesOk(command))
				{
					strcpy(oldusername,userid);
					sprintf(command,"select * from callerid_user where username='%s'",userid);
					do_log(command);
					if (db->ExecTuplesOk(command))
					{
						sprintf(command,"UPDATE callerid_user set username='%s' where username='%s'",newusername,userid);
	 					do_log(command); 
	 					if(cb.ExecCommandOk(command)<=0)
						{
							do_log("rollback");
							sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
							do_log(command);
							db->ExecCommandOk(command);
							cb.ExecCommandOk("rollback work");
							return 0;
	 					}
					}
					sprintf(command,"select * from cb_addressbook where username='%s' and domainname='%s'",userid,domainname);
					do_log(command);
					if (db->ExecTuplesOk(command))
					{
						sprintf(command,"UPDATE cb_addressbook set username='%s',domainname='%s' where username='%s' and domainname='%s'",newusername,newdomain,userid,domainname);
	 					do_log(command); 
	 					if(cb.ExecCommandOk(command)<=0)
						{
							do_log("rollback");
							sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
							do_log(command);
							db->ExecCommandOk(command);
							cb.ExecCommandOk("rollback work");
							return 0;
	 					}
					}
					sprintf(command,"UPDATE sim_user set mobno=NULL where username='%s' and domainname='%s'",userid,domainname);
	 				do_log(command); 
	 				if(cb.ExecCommandOk(command)<=0)
					{
						do_log("rollback");
						sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
						do_log(command);
						db->ExecCommandOk(command);
						cb.ExecCommandOk("rollback work");
						return 0;
	 				}
				}
				strcpy(userid,newusername);
				strcpy(domainname,newdomain);
			}
			else{
				if (is_dd_enabled == 0)
				{
					copy_trikonids_tsim = 1;
				}
				else
				{
					sprintf(command," SELECT * from clienttrip where username ='%s' and status in ('Delivered','Delivery pending') and sim_phone_no in (select sim_phone_no from sim_stock where country in ( select countryname from countrydata where postpaid_prepaid='postpaid'))",userid);
					do_log(command);
					if (!db->ExecTuplesOk(command))
					{
						copy_trikonids_tsim = 1;
					}
				}
			}
			if (copy_trikonids_tsim)
			{
				username=getnew_username(db,old_mobno,newdomain);
				username=strdup(username);
				do_log(username);
				strcpy(oldusername,userid);
				srand ( time(NULL) );
				j=rand() % (9999 - 1000 + 1) + 1000;
				sprintf(pwd,"%d",j);
				do_log(pwd);
				printf("<input type=hidden name=b value=%s>",username);
				/*
				sprintf(command,"%s %s %s %s %s %s %s %d %s \"%s\" 1>/dev/null 2>/dev/null",ADD_PROG,username,pwd,newdomain,tariff,"None",emailid,0,pwd,oldclientname);
				do_log(command); 
				if(system(command)!=0){
					do_log("add voice user failed.rollback");
					db->ExecCommandOk("rollback work");
					return 0;
	 			}
	 			sprintf(command,"update gkusers set rewrite='55561#' where username='%s' and domainname='%s'",username,newdomain);
	 			do_log(command); 
	 			if(cb.ExecCommandOk(command)<=0){
					do_log("rollback");
					cb.ExecCommandOk("rollback work");
					return 0;
	 			}
				*/
				//printf("<div id=hideit style=\"display:none;\">\n");
				if (!add_voiceuser_new(db,username,pwd,newdomain,tariff,"None",emailid,0,pwd,oldclientname,"55561#",1))
				{
					do_log("add voice user failed.rollback");
					sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
					do_log(command);
					db->ExecCommandOk(command);
					db->ExecCommandOk("rollback work");
					return 0;
				}
				//printf("</div>\n");
				sprintf(command,"INSERT INTO sim_user(username,domainname,clientname,add_building,add_street,add_city,add_state,add_country,add_pinno,telno,mobno,emailadd,creditcard_no,expdate,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,sales_person,passport_no,companyname,salutation,cc_holder_name,passport_city,active) select '%s','%s',clientname,add_building,add_street,add_city,add_state,add_country,add_pinno,telno,mobno,emailadd,creditcard_no,expdate,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,sales_person,passport_no,companyname,salutation,cc_holder_name,passport_city,active from sim_user where username='%s' and domainname='%s'",username,newdomain,userid,domainname);
	 			do_log(command); 
	 			if(cb.ExecCommandOk(command)<=0){
					do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
					cb.ExecCommandOk("rollback work");
					return 0;
	 			}
				sprintf(command,"select * from csreferrals where username='%s@%s'",userid,domainname);
				do_log(command);
				int int_csref=db->ExecTuplesOk(command);
				if (int_csref>0){
					sales_cnt=int_csref;
					for(int i=0;i<int_csref;i++){
						sprintf(command,"INSERT into csreferrals(seq,username,referredby,rootid) VALUES ('%s','%s@%s','%s','%s')",db->get_field(i,"seq"),username,newdomain,db->get_field(i,"referredby"),db->get_field(i,"rootid"));
	 					do_log(command); 
	 					if(cb.ExecCommandOk(command)<=0){
							do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
							cb.ExecCommandOk("rollback work");
							return 0;
	 					}
						 strcpy(sales[i],db->get_field(i,"rootid"));
					}
				}
				if (is_dd_enabled){
					sprintf(command,"UPDATE callerid_user set username='%s' where username='%s'",username,userid);
	 				do_log(command); 
	 				if(cb.ExecCommandOk(command)<=0){
						do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
						cb.ExecCommandOk("rollback work");
						return 0;
	 				}
					sprintf(command,"UPDATE cb_addressbook set username='%s',domainname='%s' where username='%s' and domainname='%s'",username,newdomain,userid,domainname);
	 				do_log(command); 
	 				if(cb.ExecCommandOk(command)<=0){
						do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
						cb.ExecCommandOk("rollback work");
						return 0;
	 				}
					sprintf(command,"UPDATE sim_user set mobno=NULL where username='%s' and domainname='%s'",userid,domainname);
	 				do_log(command); 
	 				if(cb.ExecCommandOk(command)<=0){
						do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
						cb.ExecCommandOk("rollback work");
						return 0;
	 				}
				}
				sprintf(command,"INSERT into trikon_tsim_map(oldusername,olddomainname,newusername,newdomainname) VALUES('%s','%s','%s','%s')",userid,domainname,username,newdomain);
	 			do_log(command); 
	 			if(cb.ExecCommandOk(command)<=0){
					do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
					cb.ExecCommandOk("rollback work");
					return 0;
	 			}
				strcpy(userid,username);
				strcpy(domainname,newdomain);
			}
		}
		else{
			//check if there is a trikon - tsim map
			sprintf(command,"select * from trikon_tsim_map where newusername='%s' and newdomainname='%s'",userid,newdomain);
			do_log(command);
			if (db->ExecTuplesOk(command)){
				strcpy(oldusername,db->get_field(0,"oldusername"));
				sprintf(command,"SELECT * from clienttrip where username ='%s' and status in ('Delivered','Delivery pending') and sim_phone_no in (select sim_phone_no from sim_stock where country in ( select countryname from countrydata where postpaid_prepaid='postpaid'))",oldusername);
				do_log(command);
				if (!db->ExecTuplesOk(command)){
					sprintf(command,"select * from callerid_user where username='%s'",oldusername);
					do_log(command);
					if (db->ExecTuplesOk(command)){
						sprintf(command,"UPDATE callerid_user set username='%s' where username='%s'",userid,oldusername);
	 					do_log(command); 
	 					if(cb.ExecCommandOk(command)<=0){
							do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
							cb.ExecCommandOk("rollback work");
							return 0;
	 					}
					}
					sprintf(command,"select * from cb_addressbook where username='%s' and domainname='%s'",userid,newdomain);
					do_log(command);
					if (db->ExecTuplesOk(command)){
						sprintf(command,"UPDATE cb_addressbook set username='%s',domainname='%s' where username='%s' and domainname='%s'",userid,newdomain,oldusername,domainname);
	 					do_log(command); 
	 					if(cb.ExecCommandOk(command)<=0){
							do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
							cb.ExecCommandOk("rollback work");
							return 0;
	 					}
					}
					sprintf(command,"UPDATE sim_user set mobno=NULL where username='%s' and domainname='%s'",oldusername,domainname);
	 				do_log(command); 
	 				if(cb.ExecCommandOk(command)<=0){
						do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
						cb.ExecCommandOk("rollback work");
						return 0;
	 				}
					strcpy(domainname,newdomain);
				}
				else{
					strcpy(userid,oldusername);
				}
			}
			else strcpy(domainname,newdomain);
		}
	}

	sprintf(command,"select to_char(now(),'dd/mm/yyyy') as ptime;");
        char *bdate;
	bool send_sms_flag = 0;
	char mobile_number[1024];
        if(db->ExecTuplesOk(command)>0)
        bdate=db->get_field(0,"ptime");
	if(bdate==NULL||bdate[0]==0)
		bdate="";
	else
		bdate=strdup(bdate);
	if(!strcmp(userid,"new")){
		char *clientname=cgi_getentrystr("txtname");
		strncpy(clientname_shortened,clientname,20);
		char *salutation=cgi_getentrystr("selsal");
		char *companyname=cgi_getentrystr("txtcompany");
		char *add_bldg=cgi_getentrystr("txtbldg");
		char *add_street=cgi_getentrystr("txtstreet");
		char *add_pinno=cgi_getentrystr("txtpin");
		char *add_country=cgi_getentrystr("selcountry");
		char *add_city=cgi_getentrystr("selcity");
		char *add_state=cgi_getentrystr("selstate");
		char *deliver_add_bldg=cgi_getentrystr("txtdbldg");
		char *primary_email=cgi_getentrystr("primary_email");
        	do_log("Line No-3386");
		do_log(primary_email);
		if((primary_email==NULL)||primary_email[0]==0)
			primary_email="";
		else
			primary_email=strdup(primary_email);
		if((deliver_add_bldg==NULL)||deliver_add_bldg[0]==0)
			deliver_add_bldg="";
		else
			deliver_add_bldg=strdup(deliver_add_bldg);
		char *deliver_add_street=cgi_getentrystr("txtdstreet");
		if((deliver_add_street==NULL)||deliver_add_street[0]==0)
			 deliver_add_street="";
		else
			deliver_add_street=strdup(deliver_add_street);
		char *deliver_add_pinno=cgi_getentrystr("txtdpin");
		if((deliver_add_pinno==NULL)||deliver_add_pinno[0]==0)
			deliver_add_pinno="";
		else
			deliver_add_pinno=strdup(deliver_add_pinno);
		char *deliver_add_country=cgi_getentrystr("dselcountry");
		if((deliver_add_country==NULL)||deliver_add_country[0]==0)
			deliver_add_country="";
		else
			deliver_add_country=strdup(deliver_add_country);
		char *deliver_add_city=cgi_getentrystr("seldcity");
		if((deliver_add_city==NULL)||deliver_add_city[0]==0)
			deliver_add_city="";
		else
			deliver_add_city=strdup(deliver_add_city);
		char *deliver_add_state=cgi_getentrystr("seldstate");
		if((deliver_add_state==NULL)||deliver_add_state[0]==0)
			deliver_add_state="";
		else
			deliver_add_state=strdup(deliver_add_state);
		if(strcmp(deliver_add_bldg,"'"))
			deliver_add_bldg=replacestring(deliver_add_bldg,"'","''");
		if(strcmp(deliver_add_street,"'"))
			deliver_add_street=replacestring(deliver_add_street,"'","''");
		if(strcmp(add_bldg,"'"))
			add_bldg=replacestring(add_bldg,"'","''");
		if(strcmp(add_street,"'"))
			add_street=replacestring(add_street,"'","''");
		char *mobno=cgi_getentrystr("txtmobno");
		if((mobno==NULL)||(mobno[0]==0)){
			mobno="";
			do_log("mobile number not present");
			printf("\n<center><b>   Please enter Mobile number</center></b>");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			return 0;
		}
		else
			mobno=strdup(mobno);
		if (is_dd_enabled){
			char *mob_part = replacestring(replacestring(mobno,"-",""),"+","");
			mob_part += strlen(mob_part)-8;
			sprintf(command,"select * from sim_user where mobno like '%%%s'",mob_part);
			do_log(command);
			if (db->ExecTuplesOk(command)){
				do_log("Already the mobile number is regd");
				printf("\n<center><b> The mobile number %s is already registered under the username %s, clientname %s</center></b>",replacestring(replacestring(mobno,"-",""),"+",""),db->get_field(0,"username"),db->get_field(0,"clientname"));
				do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
				cb.ExecCommandOk("rollback work");
				return 0;
			}
		}
		char *telno=cgi_getentrystr("txtphone");
		char *stdcode=cgi_getentrystr("stdcode");
		char *isdcode=cgi_getentrystr("isdcode");
		char mob[6];
		strncpy(mob,mobno,4); 
		mob[4]='\0';
		do_log("Line No-3462");
		/*
		sprintf(command,"select * from std_codes where isd_code = '%s' and std_code like '%s%%' and ismobile=true limit 1",isdcode,mob);
		do_log(command);
		if (!db->ExecTuplesOk(command)){
			printf("\n<center><b>   Please enter a valid Mobile number</center></b>");
			do_log("Not a Valid Mobile number");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			return 0;
		}
		*/

        	do_log("Line No-3475");
		char tt[101];
		sprintf(tt,"%s%s",isdcode,mobno);
		strcpy(mobno,tt);
		sprintf(tt,"%s%s",stdcode,telno);
		strcpy(telno,tt);
		char *chkaddr=cgi_getentrystr("chkaddr");
		if(chkaddr==NULL)
			chkaddr=strdup("off");
		if(!strcmp(chkaddr,"on")){
			deliver_add_bldg=add_bldg;
			deliver_add_street=add_street;
			deliver_add_pinno=add_pinno;
			deliver_add_country=add_country;
			deliver_add_city=add_city;
			deliver_add_state=add_state;
		}
		username=getnew_username(db,replacestring(replacestring(mobno,"-",""),"+",""),domainname);
		username=strdup(username);
		do_log(username);
		srand ( time(NULL) );
		j=rand() % (9999 - 1000 + 1) + 1000;
		sprintf(pwd,"%d",j);
		do_log("Password");
		do_log(pwd);
		printf("<input type=hidden name=b value=%s>",username);
		/*
		sprintf(command,"%s %s %s %s %s %s %s %d %s \"%s\" 1>/dev/null 2>/dev/null",ADD_PROG,username,pwd,domainname,tariff,"None",emailid,0,pwd,clientname);
		do_log(command); 
		if(system(command)!=0){
			do_log("add voice user failed.rollback");
			db->ExecCommandOk("rollback work");
			return 0;
	 	}
	 	sprintf(command,"update gkusers set rewrite='55561#' where username='%s' and domainname='%s'",username,domainname);
	 	do_log(command); 
	 	if(cb.ExecCommandOk(command)<=0){
			do_log("rollback");
			cb.ExecCommandOk("rollback work");
			return 0;
	 	}
		*/
		printf("<div id=hideit style=\"display:none;\">\n");
		if (!add_voiceuser_new(db,username,pwd,domainname,tariff,"None",emailid,0,pwd,clientname,"55561#",1)){
			do_log("add voice user failed.rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			cb.ExecCommandOk("rollback work");
			return 0;
		}
		printf("</div>\n");
		if((strcmp(expdate," "))&&(!strcmp(cc_holder_name," ")))
			cc_holder_name=clientname;
		if(!strcmp(companyname,""))
			companyname=clientname;
		companyname=strdupa(companyname);
		sprintf(command,"expdate -%s-",expdate);
		do_log(command);
	 	if(!strcmp(expdate," "))
			if(!strcmp(chkreseller,"no"))
				sprintf(command,"insert into sim_user(username,domainname,clientname,add_building,add_street,add_city,add_state,add_country,add_pinno,telno,mobno,emailadd,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,sales_person,companyname,salutation,passport_no,passport_city,primary_emailadd)values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');",username,domainname,clientname,add_bldg,add_street,add_city,add_state,add_country,add_pinno,replacestring(replacestring(telno,"-",""),"+",""),replacestring(replacestring(mobno,"-",""),"+",""),emailid,deliver_add_bldg,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,replacestring(sales_person,",","/"),replacestring(companyname,"'","''"),salutation,passport_no,passport_city,primary_email);
			else
				sprintf(command,"insert into sim_user(username,domainname,clientname,add_building,add_street,add_city,add_state,add_country,add_pinno,telno,mobno,emailadd,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,sales_person,companyname,salutation,passport_no,passport_city,primary_emailadd,reseller_sp)values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');",username,domainname,clientname,add_bldg,add_street,add_city,add_state,add_country,add_pinno,replacestring(replacestring(telno,"-",""),"+",""),replacestring(replacestring(mobno,"-",""),"+",""),emailid,deliver_add_bldg,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,replacestring(sales_person,",","/"),replacestring(companyname,"'","''"),salutation,passport_no,passport_city,primary_email,chkreseller);
	 	else	
		{	
			if(!strcmp(chkreseller,"no"))
			 	sprintf(command,"insert into sim_user (username,domainname,clientname,add_building,add_street,add_city,add_state,add_country,add_pinno,telno,mobno,emailadd,creditcard_no,expdate,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,sales_person,companyname,salutation,cc_holder_name,passport_no,passport_city,primary_emailadd) values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",username,domainname,clientname,add_bldg,add_street,add_city,add_state,add_country,add_pinno,replacestring(replacestring(telno,"-",""),"+",""),replacestring(replacestring(mobno,"-",""),"+",""),emailid,creditcard_no,replacestring(expdate,"/","-"),deliver_add_bldg,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,replacestring(sales_person,",","/"),replacestring(companyname,"'","''"),salutation,cc_holder_name,passport_no,passport_city,primary_email);
			else
				sprintf(command,"insert into sim_user(username,domainname,clientname,add_building,add_street,add_city,add_state,add_country,add_pinno,telno,mobno,emailadd,deliver_add_building,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,sales_person,companyname,salutation,passport_no,passport_city,primary_emailadd,reseller_sp)values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');",username,domainname,clientname,add_bldg,add_street,add_city,add_state,add_country,add_pinno,replacestring(replacestring(telno,"-",""),"+",""),replacestring(replacestring(mobno,"-",""),"+",""),emailid,deliver_add_bldg,deliver_add_street,deliver_add_city,deliver_add_state,deliver_add_country,deliver_add_pinno,replacestring(sales_person,",","/"),replacestring(companyname,"'","''"),salutation,passport_no,passport_city,primary_email,chkreseller);

		}
		do_log(command);
		if(cb.ExecCommandOk(command)<=0){
			do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			cb.ExecCommandOk("rollback work");
			return 0;
		}
		time_t t;
		struct tm *tmst;
		time(&t);
		tmst = localtime(&t);
		char seq[41];
		int k=1;
		char *sale=strtok((char *)sales_person,",");
		strcpy(sales[sales_cnt++],sale);
		sprintf(seq, "%04d%02d%02d%02d%02d%02d%02d", 1900+tmst->tm_year, tmst->tm_mon+1, tmst->tm_mday, tmst->tm_hour,tmst->tm_min, tmst->tm_sec,k);
		sprintf(command,"insert into csreferrals (seq,username,referredby,rootid)values('%s','%s@%s','%s@%s','%s@%s')",seq,username,domainname,sale,domainname,sale,domainname);
		k++;
		do_log(command);
		if(cb.ExecCommandOk(command)<=0){
			 do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
		 	cb.ExecCommandOk("rollback work");
		 	return 0;
	 	}
		while(sale != NULL)
		{	
			sale = strtok(NULL, ",");
			if(sale!=NULL){
				strcpy(sales[sales_cnt++],sale);
				sprintf(seq, "%04d%02d%02d%02d%02d%02d%02d", 1900+tmst->tm_year, tmst->tm_mon+1, tmst->tm_mday, tmst->tm_hour, tmst->tm_min, tmst->tm_sec,k);
				sprintf(command,"insert into csreferrals (seq,username,referredby,rootid)values('%s','%s@%s','%s@%s','%s@%s')",seq,username,domainname,sale,domainname,sale,domainname);
				k++;
				do_log(command);
				if(cb.ExecCommandOk(command)<=0){
					do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
					cb.ExecCommandOk("rollback work");
					return 0;
				}	
			}
		}
//<Added by BINU for registering India side mobile numbers to enable customers to send VCards>
	if (is_dd_enabled){
		do_log("adding mobilenum as caller id");
//replacestring(replacestring(mobno,"-","")
		strcpy(mobile_number,replacestring(replacestring(mobno,"-",""),"+",""));
		char *pp=mobile_number;
		pp += strlen(pp)-8;
		sprintf(command,"select * from callerid_user where regd_number like '%%%s';",pp);
		do_log(command);
		if (!db->ExecTuplesOk(command)){
			send_sms_flag = 1;
			sprintf(command,"INSERT INTO callerid_user (regd_number,username) VALUES ('%s','%s')",mobile_number,username);
			do_log(command);
			if(cb.ExecCommandOk(command)<=0){
				do_log("rollback work");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
				cb.ExecCommandOk("rollback work");
				return 0;
	 		}
		}
		else{
			sprintf(command,"oldusername -%s- username -%s-",oldusername,db->get_field(0,"username"));
			do_log(command);
			if (strcmp(oldusername,db->get_field(0,"username"))){
			sprintf(command,"The India number %s already exists as registered number for %s, sending mail",mobile_number,db->get_field(0,"username"));
			do_log(command);
			/*
			sprintf(command,"Hello,\n\nFor the SIM Card customer %s (username %s), system could not register the India number %s as it is already registered under the username %s.\n\nPlease do the needful.",clientname,username,mobile_number,db->get_field(0,"username"));
			char mailto[1024];
			if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
				do_log("Could not open config file.");
				printf("Error occured while opening config file.");
			}
			else{
				if (!send_mail(mailto,command,"Registering India number failed")){
					do_log("failed to send");
				}
				else	do_log("sent");
			}
			*/
			printf("The mobile number %s is already registered under the username %s",mobile_number,db->get_field(0,"username"));
			do_log("rollback work");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			cb.ExecCommandOk("rollback work");
			return 0;
			}
		}
	}
//</Added by BINU>
	}
	else{
		username=strdup(userid);
		do_log("userid");
		do_log("emailid");
		do_log(emailid);
		sprintf(command,"select emailadd from sim_user where username='%s'",username);
		do_log(command);
		if(db->ExecTuplesOk(command))
			if(!strcmp(db->get_field(0,"emailadd"),emailid))
				do_log("do not update");
			else
			{
				sprintf(command,"update sim_user set emailadd='%s' where username='%s'",emailid,username);
				do_log(command);
				if(cb.ExecCommandOk(command)<=0){
					do_log("rollback");
					cb.ExecCommandOk("rollback work");
        		                return 0;
               			}
			}
		if((!strcmp(cc_holder_name," "))&&(strcmp(creditcard_no," "))&&(strcmp(expdate," ")))
	 		sprintf(command,"update sim_user set expdate='%s',creditcard_no='%s',cc_holder_name=cc_holder_name where username='%s'",replacestring(expdate,"/","-"),creditcard_no,username);
		else if(!(strcmp(expdate," "))&&(strcmp(creditcard_no," "))&&(strcmp(cc_holder_name," ")))
			sprintf(command,"update sim_user set expdate=expdate ,creditcard_no=creditcard_no,cc_holder_name='%s' where username='%s'",creditcard_no,cc_holder_name,username);
		else if((!strcmp(creditcard_no," ")))
			sprintf(command,"update sim_user set expdate=expdate,creditcard_no=creditcard_no,cc_holder_name=cc_holder_name where username='%s'",username);
		else if((!strcmp(expdate," "))&&(!strcmp(cc_holder_name," "))&&(!strcmp(creditcard_no," ")))
			sprintf(command,"update sim_user set expdate='%s',creditcard_no='%s',cc_holder_name='%s' where username='%s'",replacestring(expdate,"/","-"),creditcard_no,cc_holder_name,username);
		else
			 sprintf(command,"update sim_user set expdate='%s',creditcard_no='%s',cc_holder_name='%s' where username='%s'",replacestring(expdate,"/","-"),creditcard_no,cc_holder_name,username);
		do_log(command);
		if(cb.ExecCommandOk(command)<=0){
			 do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
		 	cb.ExecCommandOk("rollback work");
		 	return 0;
		}
		if (strlen(old_mobno)>0){
			strcpy(mobile_number,old_mobno);
			strncpy(clientname_shortened,oldclientname,20);
		}
		else{
			sprintf(command,"select mobno,clientname from sim_user where username='%s'",username);
			if (db->ExecTuplesOk(command)){
				strcpy(mobile_number,db->get_field(0,"mobno"));
				strncpy(clientname_shortened,db->get_field(0,"clientname"),20);
			}
		}
//<Added by BINU for registering India side mobile numbers to enable customers to send VCards>
		if (is_dd_enabled){
			bool bdont_book = 0;
			bool bfill_sales = 0;
			char mail_msg[1024];
			if (strlen(mobile_number)>8 ){
				char *pp=mobile_number;
				pp += strlen(pp)-8;
				sprintf(command,"select * from callerid_user where regd_number like '%%%s';",pp);
				do_log(command);
				if (!db->ExecTuplesOk(command)){
					send_sms_flag = 1;
					sprintf(command,"INSERT INTO callerid_user (regd_number,username) VALUES ('%s','%s')",mobile_number,username);
					do_log(command);
					if(cb.ExecCommandOk(command)<=0){
						do_log("rollback work");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
						cb.ExecCommandOk("rollback work");
						return 0;
	 				}
					bfill_sales = 1;
				}
				else{
					sprintf(command,"username -%s- username -%s-",username,db->get_field(0,"username"));
					do_log(command);
					if (strcmp(username,db->get_field(0,"username"))){
						sprintf(command,"select * from trikon_tsim_map where oldusername='%s' and newusername='%s'",db->get_field(0,"username"),username);
						do_log(command);
						if (!cb.ExecTuplesOk(command)) bdont_book = 1;
						sprintf(mail_msg,"The mobile number %s is already registered under the username %s",mobile_number,db->get_field(0,"username"));
						/*
						sprintf(command,"The India number %s already exists as registered number for %s, sending mail",mobile_number,db->get_field(0,"username"));
						do_log(command);
						sprintf(mail_msg,"Hello,\n\nFor the existing SIM Card customer (username %s), system could not register the India number %s as it is already registered under the username %s.\n\nPlease do the needful.",username,mobile_number,db->get_field(0,"username"));
						bsend_mail = 1;
						*/
					}
					else{
						bfill_sales = 1;
						send_sms_flag = 1;
					}
				}
			}
			else{
				do_log("The India number is improper");
				bdont_book = 1;
				sprintf(mail_msg,"The mobile number %s is not proper",mobile_number);
			}
			if (bdont_book){
				printf("%s",mail_msg);
				do_log("rollback work");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
				cb.ExecCommandOk("rollback work");
				return 0;
				
			}
			if (bfill_sales){
				sprintf(command,"SELECT split_part(rootid,'@',1) as sales from csreferrals where username like '%s%%'",username);
				do_log(command);
				sales_cnt = db->ExecTuplesOk(command);
				if (sales_cnt > 0){
					for(int cc=0; cc<sales_cnt; cc++){
						strcpy(sales[cc],db->get_field(cc,"sales"));
					}
				}
			}
		}
//</Added by BINU>
	}
	if(!(strcmp(isadv,"off"))){
		do_log("caller id");
		sprintf(command,"select regd_number from callerid_user where regd_number='%s';",simno);
		do_log(command);
		if(db->ExecTuplesOk(command)>0)
			sprintf(command,"update callerid_user set username='%s' where regd_number='%s';",username,simno);
		else
			sprintf(command,"insert into callerid_user (regd_number,username) values('%s','%s');",simno,username);
		do_log(command);
		if(cb.ExecCommandOk(command)<=0){
			do_log("rollback work");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			cb.ExecCommandOk("rollback work");
			return 0;
		}
		//char *hset = cgi_getentrystr("hs");
		char *pm = cgi_getentrystr("pm");
		if (pm == NULL || pm[0] == 0) pm = strdupa("");
		else pm = strdupa(pm);
		if (hset == NULL || hset[0]==0) 
			sprintf(command,"insert into clienttrip(username,tripid,domainname ,sim_phone_no,from_date,to_date,sim_deliver_date,booking_date,status,tripid_min,payment_mode,mis_user,case_source,app_provided) values('%s','%s','%s','%s','%s','%s','%s','now','Delivery pending','%s','%s','%s','%s','%s')",username,tripid,domainname,simno,replacestring(fdate,"/","-"),replacestring(tdate,"/","-"),replacestring(ddate,"/","-"),tripid,pm,mis_user,case_src,provideapp);
		else
		{
			if(!strcmp(chkhandset,"ofc_handset"))
				sprintf(command,"insert into clienttrip(username,tripid,domainname ,sim_phone_no,from_date,to_date,sim_deliver_date,booking_date,status,tripid_min,handset_model,payment_mode,mis_user,case_source,app_provided) values('%s','%s','%s','%s','%s','%s','%s','now','Delivery pending','%s','Office Handset - %s','%s','%s','%s','%s')",username,tripid,domainname,simno,replacestring(fdate,"/","-"),replacestring(tdate,"/","-"),replacestring(ddate,"/","-"),tripid,hset,pm,mis_user,case_src,provideapp);
			else
			{
				hset = strdupa(hset);
				sprintf(command,"insert into clienttrip(username,tripid,domainname ,sim_phone_no,from_date,to_date,sim_deliver_date,booking_date,status,tripid_min,handset_model,payment_mode,mis_user,case_source,app_provided) values('%s','%s','%s','%s','%s','%s','%s','now','Delivery pending','%s','%s','%s','%s','%s','%s')",username,tripid,domainname,simno,replacestring(fdate,"/","-"),replacestring(tdate,"/","-"),replacestring(ddate,"/","-"),tripid,hset,pm,mis_user,case_src,provideapp);
			}
		}
		do_log(command);
		if(cb.ExecCommandOk(command)<=0){
			do_log("rollback work");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
			cb.ExecCommandOk("rollback work");
			return 0;
	 	}
		
		sprintf(command,"COUNTRIES TRAVELLING:%s",country_travelling);
		do_log(command);
		if(!(country_travelling == NULL || country_travelling[0]==0))
		{
			do_log("Coutries Selected");
			sprintf(command,"INSERT INTO local_number_generation (tripid,country) SELECT * from (select '%s'as tripid) A cross join (SELECT unnest(string_to_array('%s',','))) B",tripid,country_travelling);
			do_log(command);
			if(!db->ExecCommandOk(command))
			{
				printf("Failed to add Travelling Countries");
				do_log("Failed to add Travelling Countries");
			}
		}
		else
		{
			do_log("No Countries selected");
		}
		char *ucs = cgi_getentrystr("ucs");
		if (!(ucs == NULL || ucs[0] == 0)){
			ucs = strdupa(ucs);
			if (!strcmp(ucs,"yes")){
				do_log("unlimited calling opted");
				uc_fdate = cgi_getentrystr("uc_fdate");
				char *uc_op = cgi_getentrystr("uc_op");
				if (uc_fdate == NULL || uc_fdate[0] == 0){
					do_log("uc_fdate is empty");
                        		do_log("rollback");
					sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
					do_log(command);
					db->ExecCommandOk(command);
                        		cb.ExecCommandOk("rollback work");
                       			return 0;
				}
				uc_fdate = strdupa(uc_fdate);
				if (uc_op == NULL || uc_op[0] == 0){
					do_log("uc_op is empty");
                        		do_log("rollback");
					sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
					do_log(command);
					db->ExecCommandOk(command);
                        		cb.ExecCommandOk("rollback work");
                       			return 0;
				}
				uc_op = strdupa(uc_op);
				sprintf(command,"ucs -%s-, uc_fdate -%s-,uc_op -%s-",ucs,uc_fdate,uc_op);
				do_log(command);
				char uc_tdate[21];
				add_days(uc_fdate,atoi(uc_op)-1,uc_tdate);
				if (is_dd_enabled){
					sprintf(command,"insert into max_reduction_codes(username,domainname,tripid,std_code,from_date,to_date) VALUES('%s','%s','%s','%s','%s','%s')",username,domainname,tripid,simno,uc_fdate,uc_tdate);
					do_log(command);
					if (!cb.ExecCommandOk(command)){
						do_log("rollback work");
						sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
						do_log(command);
						db->ExecCommandOk(command);
						cb.ExecCommandOk("rollback work");
						return 0;
	 				}
				}
				char *udata = cgi_getentrystr("udata");
				if (udata == NULL || udata[0] == 0){
//					string data_plan = read_tag_attribute("DEFAULT_DATA", country_code);
	
					char query3[1024];
                                        sprintf(query3,"DEFAULT_DATA_%s",country_code);
                                        string data_plan = get_simmis_data_from_table(query3,"1");



					do_log("data_plan");
					do_log(to_c_string(data_plan));
					if (data_plan.compare("1") == 0){
						cout << "\n\n<br>";
						do_log("DEFAULT_DATA plan enabled");
						sprintf(command,"insert into max_reduction_codes(username,domainname,tripid,std_code,from_date,to_date) VALUES('%s','%s','%s','%s_%s','%s','%s')",username,domainname,tripid,UNLIMITED_SECOND_LEG,"Data",uc_fdate,uc_tdate);
					}
					else
						sprintf(command,"insert into max_reduction_codes(username,domainname,tripid,std_code,from_date,to_date) VALUES('%s','%s','%s','%s','%s','%s')",username,domainname,tripid,UNLIMITED_SECOND_LEG,uc_fdate,uc_tdate);

				}
				else{
					udata = strdupa(udata);
					if (!strcmp(udata,"yes"))
						sprintf(command,"insert into max_reduction_codes(username,domainname,tripid,std_code,from_date,to_date) VALUES('%s','%s','%s','%s_%s','%s','%s')",username,domainname,tripid,UNLIMITED_SECOND_LEG,"Data",uc_fdate,uc_tdate);
					else
						sprintf(command,"insert into max_reduction_codes(username,domainname,tripid,std_code,from_date,to_date) VALUES('%s','%s','%s','%s','%s','%s')",username,domainname,tripid,UNLIMITED_SECOND_LEG,uc_fdate,uc_tdate);
				}
				do_log(command);
				if (!cb.ExecCommandOk(command)){
					do_log("rollback work");
					sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
					do_log(command);
					db->ExecCommandOk(command);
					cb.ExecCommandOk("rollback work");
					return 0;
				}
				char curr_dt[11];
				char uc_fdate_for[11];
				strcpy(curr_dt,get_curr_date());
				sprintf(uc_fdate_for,"%04d-%02d-%02d",return_year(uc_fdate),return_month(uc_fdate),return_day(uc_fdate));
				if (!strcmp(curr_dt,uc_fdate_for)){
					run_uc_now = 1;
					do_log("run_uc_now = 1");
				}
			}
		}
		/*
		char *udata = cgi_getentrystr("udata");
		if (!(udata == NULL || udata[0] == 0)){
			udata = strdupa(udata);
			if (!strcmp(udata,"yes")){
				do_log("Unlimited Data opted");
				char *uc_fdate = cgi_getentrystr("uc_fdate");
				char *uc_op = cgi_getentrystr("uc_op");
				if (uc_fdate == NULL || uc_fdate[0] == 0){
					do_log("uc_fdate is empty");
                        		do_log("rollback");
					sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
					do_log(command);
					db->ExecCommandOk(command);
                        		cb.ExecCommandOk("rollback work");
                       			return 0;
				}
				uc_fdate = strdupa(uc_fdate);
				if (uc_op == NULL || uc_op[0] == 0){
					do_log("uc_op is empty");
                        		do_log("rollback");
					sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
					do_log(command);
					db->ExecCommandOk(command);
                        		cb.ExecCommandOk("rollback work");
                       			return 0;
				}
				uc_op = strdupa(uc_op);
				sprintf(command,"udata -%s-, uc_fdate -%s-,uc_op -%s-",udata,uc_fdate,uc_op);
				do_log(command);
				char uc_tdate[21];
				add_days(uc_fdate,atoi(uc_op)-1,uc_tdate);
				sprintf(command,"insert into max_reduction_codes(username,domainname,tripid,std_code,from_date,to_date) VALUES('%s','%s','%s','%s','%s','%s')",username,domainname,tripid,"Data",uc_fdate,uc_tdate);
				do_log(command);
				if (!cb.ExecCommandOk(command)){
					do_log("rollback work");
					sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
					do_log(command);
					db->ExecCommandOk(command);
					cb.ExecCommandOk("rollback work");
					return 0;
				}

			}
		}
		*/
	}
/*	if(!strcmp(isadv,"on")){
		sprintf(command,"select nextval('advance_booking_id')");
		char advid1[1024];
		if(db->ExecTuplesOk(command)>0){
                        sprintf(advid1,"ADV-%s",db->get_field(0,"nextval"));
                }
                else{
                        do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
                        cb.ExecCommandOk("rollback work");
                        return 0;
                }
		do_log(advid1);
		sprintf(command,"select distinct on (type) type,serialno from sim_accessories_stock where active=true and alloted=false;");
                do_log(command);
                count=db->ExecTuplesOk(command);
                for(i=0;i<count;i++){
                        char *type=db->get_field(i,"type");
                        char *serial=db->get_field(i,"serialno");
                        char chktype[1024];
                        sprintf(chktype,"chk_%s",type);
                        do_log(chktype);
                        char *ischeck=cgi_getentrystr(chktype);
                        if(ischeck==NULL||ischeck[0]==0)
                                ischeck="off";
                        else
                                ischeck=strdup(ischeck);
                        do_log(ischeck);
                        if(!strcmp(ischeck,"on")){
                                sprintf(command,"update sim_accessories_stock set alloted=true,tripid='%s' where type='%s' and serialno='%s';",advid1,type,serial);
                                do_log(command);
                                if(cb.ExecCommandOk(command)<=0)
                                {
                                        do_log("rollback");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
                                        cb.ExecCommandOk("rollback work");
                                        return 0;
                                }
                        }
                }

		sprintf(command,"insert into advance_booking (booking_id,username,country,trip_startdate,trip_enddate,simcard_delivery_date,emailid) values('%s','%s','%s','%s','%s','%s','%s');",advid1,username,country,replacestring(fdate,"/","-"),replacestring(tdate,"/","-"),replacestring(ddate,"/","-"),emailid);
		do_log(command);
                if(cb.ExecCommandOk(command)<=0){
                        do_log("rollback work");
			sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
			do_log(command);
			db->ExecCommandOk(command);
                        cb.ExecCommandOk("rollback work");
                        return 0;
                }
		simno=strdup("advance booking");
	}*/
	/*
	sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
	do_log(command);
	db->ExecCommandOk(command);
	*/
	cb.ExecCommandOk("commit work");                           //if(!strcmp(country,"USA WORLDWIDE")){do_log("country success");add_credit_on_booking=1;add_e_credit_entry_on_booking=1;}
	//if (strstr(country,"GLOBAL") || strstr(country,"WorldFree") || strstr(country,"Europe DirectCall") || strstr(country,"USA Euro Data SIM") ){}
	if (add_credit_on_booking){
//ADD E Credit to GLOBAL CARDS
		do_log("add e credit for global card");
		char e_amount[21];
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- GLOBAL_E_CREDIT","-- GLOBAL_E_CREDIT>",country,e_amount)!=1) || (e_amount==NULL || e_amount[0]==0)){
			if (!strcmp(country, "GLOBAL POSTPAID")) strcpy(e_amount,"298.9");
			else if (!strcmp(country,"GLOBAL NEW")) strcpy(e_amount,"98.9");
			else if (!strcmp(country,"UK")) strcpy(e_amount,"298.9");
			else if (!strcmp(country,"USA")) strcpy(e_amount,"298.9");
			//else if (!strcmp(country,"USA Euro Data SIM")) strcpy(e_amount,"48.9");
			//else if (!strcmp(country,"USA WORLDWIDE")) strcpy(e_amount,"58.9");
			else strcpy(e_amount,"58.9");
		}*/

		   char globalcountry[1024];
                sprintf(globalcountry,"GLOBAL_E_CREDIT_%s",country);
                char* data30 = get_simmis_data_from_table(globalcountry,"58.9");
                strcpy(e_amount,data30);





		agentusername = strdupa("trikon");
		psswd = strdupa("K1N065qnoNE");
		gsim GSIM;
		int res = GSIM.init_gsim_with_sim_phone_no(db,simno);
		if (!res){
			do_log("failed to init_gsim_with_sim_phone_no");
			printf("<font color=\"red\">Failed to add emergency credit of %s</font><br>",e_amount);
		}
		sprintf(command,"select * from countrydata  where countryname='%s'",country);
                do_log(command);
		char api_code[20];
                if(db->ExecTuplesOk(command)){
                      strcpy(api_code,db->get_field(0,"api_code"));
                }else
                      do_log("No result");
                do_log(api_code);

		if (add_e_credit_entry_on_booking )
			res = GSIM.add_e_credit(atof(e_amount),0,api_code,simno);				//Added by Neekhil Dedhia
		else
			res = GSIM.debit_from_prepaid(atof(e_amount),api_code);
		if (!res){
			do_log("failed to add_e_credit");
			printf("<font color=\"red\">Failed to add emergency credit of %s</font><br>",e_amount);
			char mailto[1024];
			sprintf(command,"Hello,\nThe SIM Card customer (%s) has been alloted a card %s. Adding a credit of US$%s failed, please do the needful.",username,simno,e_amount);
/*			if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
				do_log("Could not open EMAIL_TO_CUSTOMER file.");
				printf("Error occured while opening the EMAIL_TO_CUSTOMER file.");
			}
			else{
				if (!send_mail(mailto,command,"Adding E Credit to SIM customer failed")){
					do_log("failed to send");
				}
				else	do_log("sent");
			}*/


			    char* data31 = get_simmis_data_from_table("MAILTO_mailto","");
                        if(data31[0]==0 || data31==NULL)
                        {
                                do_log("ERROR OCCURED WHILE OPENING EMAIL_TO_CUSTOMER file data31");
                                printf("ERROR OCCURED WHILE OPENING THE EMAIL_TO_CUSTOMER FILE");
                        }
                        strcpy(mailto,data31);
                        if (!send_mail(mailto,command,"Adding E Credit to SIM customer failed"))
                        {
                                            do_log("failed to send");
                        }
                           else    do_log("sent");










		}
		else{
			if(add_reg_charge){
				char reg_charges[11];
				char reg_str[51];
				sprintf(reg_str,"%s_reg",country);
/*				if((get_tag_value(CONFIG_DATA_FILE,"<!-- GLOBAL_E_CREDIT","-- GLOBAL_E_CREDIT>",reg_str,reg_charges)!=1) || (reg_charges==NULL || reg_charges[0]==0)){
					strcpy(reg_charges,"8");
				}*/


				sprintf(reg_str,"GLOBAL_E_CREDIT_%s_reg",country);
                                char* data32 = get_simmis_data_from_table(reg_str,"8");
                                strcpy(reg_charges,data32);








                		sprintf(command,"UPDATE users set usage=usage+%f,last_update='now' where username='%s' and domainname='%s'",atof(reg_charges),username,domainname);
                		do_log(command);
               			if(!cb.ExecCommandOk(command)){
					printf("<font color=\"red\">Failed to add registration charges for %s customer</font><br>\n",country);
					do_log("failed to update");
					return -1;
				}
				char transid[31];
				GSIM.get_new_transactionid(transid);
				sprintf(command,"insert into faxes(trkn,username,domainname,status,accept_time,receive_time,currency,sender,cost) values('%s','%s','%s','[Pin:%s] [Value = %s]','now','now','%s','%s','{%s}');",transid,username,domainname,"Reg. charge",reg_charges,"USD","Reg. charge",reg_charges);
				do_log(command);
				if(!cb.ExecCommandOk(command)){
					printf("<font color=\"red\">Failed to add registration charges for %s customer</font><br>\n",country);
					do_log("failed to update");
					return -1;
				}
				sprintf(command,"insert into transactions values('%s',%s,0,(select credit-usage from users where username='%s' and domainname='%s'),(select accept_time from faxes where trkn='%s'),'%s','%s','%s');",transid,reg_charges,username,domainname,transid,"Reg. charge",username,domainname);
				do_log(command);
				if(!cb.ExecCommandOk(command)){
					printf("<font color=\"red\">Failed to add registration charges for %s customer</font><br>\n",country);
					do_log("failed to update");
					return -1;
				}
				sprintf(command,"UPDATE e_credit_sim_card set amount_given = %f where tripid='%s'",atof(e_amount)+atof(reg_charges),tripid);
				do_log(command);
				if(!cb.ExecCommandOk(command)){
					printf("<font color=\"red\">Failed to add registration charges for %s customer</font><br>\n",country);
					do_log("failed to update");
				}
				else printf("Reg. charge of %s has been charged to this client<br>\n",reg_charges);
			}
			printf("Successfully added E Credit of %s<br>",e_amount);
		}
	}

//<Added by BINU>

	if (send_sms_flag){
		do_log("sending sms to India number");
		//sprintf(command,"Send VCard to this number from your number to create a Deep Discount entry");
		//simno
		sprintf(command,"Your TSIM Phone No is %s. Please send your frequently called India numbers as business cards to +%s to activate Deep Discount Service",simno,SMS_FROM);
		do_log(command);
		int res=0;
		/*
		res=send_sms(urlencode(command),mobile_number);
		if (res==1) do_log("successfully sent SMS");
		else{
			do_log("SMS sending failed... trying again");
			res=send_sms(urlencode(command),mobile_number);
			if (res==1) do_log("successfully sent SMS");
			else{
				char mailto[1024];
				sprintf(command,"Hello,\nThe SIM Card customer %s (%s) has been alloted a card %s. Sending SMS to the customer's mobile number %s failed. Please do the needful.",clientname_shortened,username,simno,mobile_number);
				if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
					do_log("Could not open EMAIL_TO_CUSTOMER file.");
					printf("Error occured while opening the EMAIL_TO_CUSTOMER file.");
				}
				else{
					if (!send_mail(mailto,command,"Sending mail to SIM customer failed")){
						do_log("failed to send");
					}
					else	do_log("sent");
				}
			}
		}
		*/
		res = create_sms_file(command,mobile_number);
		//Cust sms
		if (res==1) do_log("successfully sent SMS");
		else{
			char mailto[1024];
			sprintf(command,"Hello,\nThe SIM Card customer %s (%s) has been alloted a card %s. Sending SMS to the customer's mobile number %s failed. Please do the needful.",clientname_shortened,username,simno,mobile_number);
/*			if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
				do_log("Could not open EMAIL_TO_CUSTOMER file.");
				printf("Error occured while opening the EMAIL_TO_CUSTOMER file.");
			}
			else{
				if (!send_mail(mailto,command,"Sending mail to SIM customer failed")){
					do_log("failed to send");
				}
				else	do_log("sent");
			}*/

			    char* data33 = get_simmis_data_from_table("MAILTO_mailto","");
                        if(data33[0]==0 || data33==NULL)
                        {
                                do_log("ERROR OCCURED WHILE OPENING EMAIL_TO_CUSTOMER file data33");
                                printf("ERROR OCCURED WHILE OPENING THE EMAIL_TO_CUSTOMER FILE");
                        }
                        strcpy(mailto,data33);
                        if (!send_mail(mailto,command,"Sending mail to SIM customer failed"))
                        {
                                            do_log("failed to send");
                        }
                           else    do_log("sent");









		}
		for (int cc=sales_cnt-1;cc>=0;cc--){
			do_log(sales[cc]);
			char *sp_mobileno = NULL;
			char msg[160];
			sprintf(msg,"The SIM Card customer %s (%s) has been alloted a card %s. The trip start date is %s",clientname_shortened,username,simno,replacestring(fdate,"/","-"));
			do_log(msg);
			sprintf(command,"select * from salesperson where sp_name='%s'",sales[cc]);
			do_log(command);
			if(db->ExecTuplesOk(command)){
				sp_mobileno = db->get_field(0,"mobileno");
				if (sp_mobileno){
					sprintf(command,"sending the following sms to %s number is %s\n%s",sales[cc],sp_mobileno,msg);
					do_log(command);
					//res=send_sms(urlencode(msg),sp_mobileno);
					res = create_sms_file(msg,sp_mobileno);
					if (res==1) do_log("successfully sent SMS");
					else do_log("SMS sending failed");
				}
			}
		}
	}
//</Added by BINU>
	printf("\n<br>	<b>Username: 		%s</b>",username);
	printf("\n<br>	<b>Sim Card Phone Number: %s</b>",simno);
	printf("\n<br><input type=submit name=sbtdetail value=\"Set up DD No.\" onclick=\"return showsetup('frmdone',%s);\">",username);
	if(!(strcmp(isadv,"off"))){	
		do_log("b4-1");
		result=booking_alert(db,username,simno,bdate,country);
		do_log("after");
        	if(result<=0)
                	printf("\n        <b>An error occured while sending mail to the sales person</b>");	
	}


	//send how to use email to customer
	//EMAIL_TO_CUSTOMER
	//emailid
if (is_country_postpaid){
	//Credit Card alert
	int cc=0;
	sprintf(command,"select creditcard_no from sim_user where username='%s'",username);
	do_log(command);
       	cc=db->ExecTuplesOk(command);
       	if(db->get_field(0,"creditcard_no")==NULL||db->get_field(0,"creditcard_no")[0]==0){
		do_log("sending alert of no credit card");
       		cc=creditcard_alert(db,username,simno,bdate);
		if(cc<=0)
               		printf("\n        <b>An error occured in credit card alert.</b>");
		do_log("after");
       	}
	char mailto[1024];
	if (is_dd_enabled){
	char email_msg[9999];
	FILE *fp_email_msg=fopen(EMAIL_TO_CUSTOMER,"r");
	char line[1024];
	if (fp_email_msg){
		sprintf(email_msg,"Hello,\nYour TSIM Phone No is %s. ",simno);
		while (fgets(command,1023,fp_email_msg)){
			if (strstr(command,"XXXX")){
				//char *tmp = strstr(command,"XXXX");
				//*tmp = '\0';
				//strcpy(line,command);
				////line[strlen(line)-1]='\0';
				//strcat(email_msg,line);
				//sprintf(line," +%s\n",mobile_number);
				//strcat(email_msg,line);
				char tmp1[101];
				sprintf(tmp1,"+%s",mobile_number);
				strcpy(line,replacestring(command,"XXXX",tmp1));
				strcat(email_msg,line);
			}
			else if (strstr(command,"The list of substitute numbers is as follows")){
				sprintf(line,"SELECT * from cb_addressbook where username ='%s'",username);
				do_log(line);
				int count=db->ExecTuplesOk(line);
				if(count>0){
					strcat(email_msg,command);
                			sprintf(command,"\nUSERNAME\tNAME\tSUBSTITUTE NO.\tDESTINATION NO.\n");
					strcat(email_msg,command);
                			for(i=0;i<count;i++){
                        			sprintf(command,"%s\t%s\t+%s\t%s\n",db->get_field(i,"username"),db->get_field(i,"name"),db->get_field(i,"acc_num"),db->get_field(i,"destination"));
						strcat(email_msg,command);
                			}
				}
				else strcat(email_msg,"No substitute numbers set\n");
			}
			else strcat(email_msg,command);
		}
		fclose(fp_email_msg);
		do_log(email_msg);
		//Cust mail
		if (!send_mail(emailid,email_msg,"TSIM's Deep Discount Service",1)){
			do_log("failed to send");
		}
		else	do_log("sent");
	}
	else{
		sprintf(command,"Hello,\nThe SIM Card customer %s (%s) has been alloted a card %s. Sending \"How to use\" mail failed. Please do the needful.",clientname_shortened,username,simno);
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
			do_log("Could not open EMAIL_TO_CUSTOMER file.");
			printf("Error occured while opening the EMAIL_TO_CUSTOMER file.");
		}
		else{
			if (!send_mail(mailto,command,"Sending mail to SIM customer failed")){
				do_log("failed to send");
			}
			else	do_log("sent");
		}*/

		 char* data34 = get_simmis_data_from_table("MAILTO_mailto","");
                        if(data34[0]==0 || data34==NULL)
                        {
                                do_log("ERROR OCCURED WHILE OPENING EMAIL_TO_CUSTOMER file data34");
                                printf("ERROR OCCURED WHILE OPENING THE EMAIL_TO_CUSTOMER FILE");
                        }
                        strcpy(mailto,data34);
                        if (!send_mail(mailto,command,"Sending mail to SIM customer failed"))
                        {
                                            do_log("failed to send");
                        }
                           else    do_log("sent");







	}
	}
}
	if (run_uc_now){
		do_log("from_date is today, should enable now");
		//sprintf(command,"/usr/local/ownmail/bin/uc_cloud9_telna.sh enable_packs \"%s\" \"%s\" now 1>/dev/null 2>/dev/null",tripid,uc_fdate);
		sprintf(command,"/usr/local/ownmail/bin/uc_cloud9_telna.sh enable_packs \"%s\" \"%s\" now 1>/dev/null 2>/dev/null",tripid,uc_fdate);
		do_log(command);
		//system("/usr/local/ownmail/bin/uc_cloud9_telna.sh prepaid_uc now 1>/dev/null 2>/dev/null");
		system(command);
	}
	return 1;
}
/*************************************************************My Function**********************************/
char *trimwhitespace(char *str)
{
	  char *end;

	  while(isspace((unsigned char)*str)) str++;
          if(*str == 0)  // All spaces?
	  return str;
          // Trim trailing space
	  end = str + strlen(str) - 1;
	  while(end > str && isspace((unsigned char)*end)) end--;

	  // Write new null terminator
	  *(end+1) = 0;

  return str;
}  
/**********************************************************Ends**********************************************/
int simstatus(fax_database *db){
	char *country=cgi_getentrystr("selcountry");
	char *simno=cgi_getentrystr("selphone");
	char *comment=cgi_getentrystr("txtcomment");
	char *pod=cgi_getentrystr("txtpod");
	char *status=cgi_getentrystr("selstatus");
	char *tdate=cgi_getentrystr("b");
	char *hstatus=cgi_getentrystr("hstatus");
	char *cstatus=cgi_getentrystr("cstatus");
	char *astatus=cgi_getentrystr("astatus");
	int result,i;
	char *t;
	//fax_database db(E2FAXDB,DBSERV);
	fax_database sel(E2FAXDB,DBSERV);
	char command[1024];
	char user[1024];
	char domain[1024];
	do_log(country);
	//if(!strcmp(country,"none")){
                sprintf(command,"select country from sim_stock where sim_phone_no='%s'",simno);
                if ((db->ExecTuplesOk(command))>0)
                        strcpy(country,db->get_field(0,"country"));
        //}
        do_log(country);
	sprintf(command,"select * from clienttrip where sim_phone_no='%s' order by booking_date desc",simno);
	do_log(command);
	if(sel.ExecTuplesOk(command)>0){do_log("inside if");
		char *s=sel.get_field(0,"status");
		do_log(s);
		sprintf(user,"%s",sel.get_field(0,"username"));
		do_log(user);
		t=strdupa(sel.get_field(0,"tripid"));
		do_log(t);
		sprintf(domain,"%s",sel.get_field(0,"domainname"));
		do_log(user);
		do_log(domain);
		if(!strcmp(s,"cardlost")){
			printf("\n<b>&nbsp&nbsp&nbsp&nbsp Status of %s cannot be updated because it has been marked as lost.<b>",simno);
			return 0;
		}
		if (db->ExecCommandOk("begin work") != 1) {
			do_log("102-Could not begin work");
			return 0;
		}
		
		if((!strcmp(status,"Returned"))&&(!strcmp(s,"Delivered"))){
			char act_ret_date[21];
			sprintf(act_ret_date,"%s 23:59:59",tdate);
			sprintf(command,"update clienttrip set status='%s',comment='%s',podno='%s',update_date='now' ,return_date='%s',bill_todate='%s' where sim_phone_no='%s' and tripid='%s'",status,comment,pod,act_ret_date,act_ret_date,simno,t);
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
				printf("\n<b>&nbsp&nbsp&nbsp&nbsp Update in clienttrip table failed</b>");
				return 0;
			}	
			sprintf(command,"update sim_stock set availability=true where sim_phone_no='%s'",simno);
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
				 printf("\n<b>Update in sim_stock table failed</b>");
				 do_log("rollback");
				 db->ExecCommandOk("rollback work"); 
				 return 0;
			}
			sprintf(command,"delete from callerid_user where regd_number='%s'",simno);
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
				 printf("\n<b>Delete in callerid_user failed</b>");
				 do_log("rollback");
				 db->ExecCommandOk("rollback work");
				 return 0;
			}
			printf("\n<b>&nbsp&nbsp&nbsp&nbsp Status of %s has been updated to %s</b><br>&nbsp&nbsp&nbsp&nbsp",simno,status);
		}
		else if((!strcmp(status,"Delivered"))&&(!strcmp(s,"Delivery pending"))){
			sprintf(command,"update clienttrip set status='%s',comment='%s',podno='%s',update_date='now',actual_deliver_date='%s',bill_fromdate='%s' where sim_phone_no='%s' and tripid='%s'",status,comment,pod,tdate,tdate,simno,t);
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
				printf("\n<b>&nbsp&nbsp&nbsp&nbsp Update in clienttrip table failed</b>");
				return 0;
			}
			/* COMMENTED AS THE SAME IS DONE DAILY THRICE
			sprintf(command,"select * from cb_addressbook where username='%s'",user);
			do_log(command);
			do_log(country);
			if((sel.ExecTuplesOk(command)<=0)&&(!strstr(country,"GLOBAL"))){
				status_alert(simno);
			}
			*/
			printf("\n<b>&nbsp&nbsp&nbsp&nbsp Status of %s has been updated to %s</b><br>&nbsp&nbsp&nbsp&nbsp",simno,status);
		}
		else if((!strcmp(status,"Canceled"))&&((!strcmp(s,"Delivery pending"))||(!strcmp(s,"Delivered")))){
			sprintf(command,"update clienttrip set status='%s',comment='%s',podno='%s',update_date='now',return_date='%s' where sim_phone_no='%s' and tripid='%s'",status,comment,pod,tdate,simno,t);
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
				printf("\n<b>&nbsp&nbsp&nbsp&nbsp Update in clienttrip table failed</b>");
				return 0;
			}
			sprintf(command,"update sim_stock set availability=true where sim_phone_no='%s'",simno);
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
				 printf("\n<b>&nbsp&nbsp&nbsp&nbsp Update in sim_stock table failed</b>");
				 do_log("rollback");
				 db->ExecCommandOk("rollback work");
				 return 0;		
			} 
			sprintf(command,"delete from callerid_user where regd_number='%s'",simno);
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
				printf("<b>Delete in callerid_user failed</b>");
				do_log("rollback");
				db->ExecCommandOk("rollback work");
				return 0;
			} 
		       /*	
			sprintf(command,"update sim_accessories_stock set alloted=false,tripid=null where tripid='%s'",t);
			do_log(command);
                        if(db->ExecCommandOk(command)<=0){
                                printf("<b>update in sim_accessories_stock failed</b>");
                                do_log("rollback");
                                db->ExecCommandOk("rollback work");
                                return 0;
                        }
		       */	
			printf("\n<b>&nbsp&nbsp&nbsp&nbsp Status of %s has been updated to %s</b><br>&nbsp&nbsp&nbsp&nbsp",simno,status);
		}
		else if(!strcmp(status,"cardlost")) //Card Lost : @ToDo - Update in backend for gSim, telna 
		{
			do_log(user);
			do_log(domain);
			/*NOTE the variable simno holds sim_phone_no */

		 	if(strcmp(s,"Returned"))		//Not 'Returned'
			{
				sprintf(command,"update clienttrip set status='%s',comment='%s',podno='%s',update_date='now',return_date='%s' where sim_phone_no='%s' and tripid='%s'",status,comment,pod,tdate,simno,t);
				do_log(command);
				if(db->ExecCommandOk(command)<=0)
				{
					printf("\n<b>&nbsp&nbsp&nbsp&nbsp Update in clienttrip table failed</b>");
					return 0;
				}
				sprintf(command,"update sim_stock set active=false where sim_phone_no='%s'",simno);
				do_log(command);
				if(db->ExecCommandOk(command)<=0)
				{
					 printf("\n<b>&nbsp&nbsp&nbsp&nbsp Update in sim_stock table failed</b>");
					 do_log("rollback");
					 db->ExecCommandOk("rollback work");
					 return 0;
				}
				sprintf(command,"delete from callerid_user where regd_number='%s'",simno);
				do_log(command);
				if(db->ExecCommandOk(command)<0)
				{
					printf("\n<b>Delete in callerid_user failed</b>");
					do_log("rollback");
					db->ExecCommandOk("rollback work");
					return 0;
				}
				do_log(user);
                	do_log(domain);
				
				int result=add_charges(db,user,domain,"Other Charges","Sim card lost charges","-30",t);
				if(result<=0)
				{
						db->ExecCommandOk("rollback work");
						printf("<b> &nbsp&nbsp&nbsp&nbsp Card lost charges failed.</b>");
						return 0;
				}
				printf("\n<b>&nbsp&nbsp&nbsp&nbsp Status of %s has been updated to %s</b><br>&nbsp&nbsp&nbsp&nbsp",simno,status);
			}
			else			//Already 'Returned'
			{
				sprintf(command,"update sim_stock set active=false where sim_phone_no='%s'",simno);
				do_log(command);
				if(db->ExecCommandOk(command)<=0)
				{
					printf("\n<b>&nbsp&nbsp&nbsp&nbsp Update in sim_stock table failed</b>");
					return 0;
				}
				sprintf(command,"delete from callerid_user where regd_number='%s'",simno);
                    do_log(command);
                    if(db->ExecCommandOk(command)<0)
				{
                    	printf("\n<b>Delete in callerid_user failed</b>");
                    	do_log("rollback");
                    	db->ExecCommandOk("rollback work");
                   		return 0;
                    }
				printf("\n<b>&nbsp&nbsp&nbsp&nbsp Status of %s has been updated to %s</b><br>&nbsp&nbsp&nbsp&nbsp",simno,status);
			}
			
			//Mark as lost in backend
			//string api_code = getApiCode(simno);
			string backend_output = (backend_update_status(db,simno, false) ? "Deactivated in backend" : "Backend update failed. Needs manual deactivation.");
			cout << backend_output;
		}//Card Lost Ends Here
		else if(!strcmp(status,"Faulty"))
		{
			 sprintf(command,"update sim_stock set active=false where sim_phone_no='%s'",simno);
			 do_log(command);
			 if(db->ExecCommandOk(command)<=0){
				printf("\n<b>&nbsp&nbsp&nbsp&nbsp Update in sim_stock table failed</b>");
				return 0;
			 }
			 printf("\n<b>&nbsp&nbsp&nbsp&nbsp Status of %s has been updated to %s</b><br>&nbsp&nbsp&nbsp&nbsp",simno,status);
		}
		else
		{
			printf("\n<b>&nbsp&nbsp&nbsp&nbsp Status of %s is %s and cannot be marked as %s</b><br>&nbsp&nbsp&nbsp&nbsp",simno,s,status);
			return 0;
		}
		do_log("updating stock");
		/*
		sprintf(command,"select * from sim_accessories_stock where tripid='%s' and active=true;",t);
		do_log(command);
		result=sel.ExecTuplesOk(command);
		for(i=0;i<result;i++){
			char accstat[1024];
		
			sprintf(accstat,"%sstatus",sel.get_field(i,"type"));
			do_log(accstat);
			char *setstat=cgi_getentrystr(accstat);
			do_log(setstat);
			//Add code to Return Handset when card is returned
			if(!strcmp(status,"Returned") && !strcmp(setstat,"none"))
			{
				do_log("Change status of Handset to Returned");
				strcpy(setstat,"Returned");
			}
			if(!strcmp(setstat,"Returned"))
			sprintf(command,"update sim_accessories_stock set alloted=false,tripid=null where tripid='%s' and type='%s';",t,sel.get_field(i,"type"));
			else if(!strcmp(setstat,"lost")){
			sprintf(command,"update sim_accessories_stock set active=false where tripid='%s' and type='%s';",t,sel.get_field(i,"type"));
			char desc[1024];
			sprintf(desc,"%s lost charges",sel.get_field(i,"type"));
			result=add_charges(db,user,domain,"Other Charges",desc,"-10",t);
                        if(result<=0){
                                     db->ExecCommandOk("rollback work");
                                     printf("<b> &nbsp&nbsp&nbsp&nbsp %s lost charges failed.</b>",sel.get_field(i,"type"));
                                     return 0;
                          }	 
			}
			else 
			sprintf(command,"update sim_accessories_stock set alloted=true where tripid='%s' and type='%s';",t,sel.get_field(i,"type"));
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
                                        printf("\n<b>update in sim_accessories_stock failed</b>");
                                        do_log("rollback");
                                        db->ExecCommandOk("rollback work");
                                        return 0;
                                }
		}*/
	}
	else{
		printf("\n<b>&nbsp&nbsp&nbsp&nbsp No trips are registered against %s</b>",simno);
		return 0;
	}
	sprintf(command,"delete from sim_phone_no_lock where sim_phone_no='%s'",simno);
	do_log(command);
	db->ExecCommandOk(command);
	agentusername = strdupa("trikon");
	psswd = strdupa("K1N065qnoNE");
	db->ExecCommandOk("commit work");
	do_log("committing work");
	double bal_tobe_refunded=0;
	char sales_email_combined[9999];
	char tmp_email[1024];
	char email_msg[1024];
	char clientname[250];
	char companyname[250];
	char sales_name[250];
	double e_credit_given = 0;
	char send_mail_to[1024];
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- GLOBAL_E_CREDIT","-- GLOBAL_E_CREDIT>","mail",send_mail_to)!=1) || (send_mail_to==NULL || send_mail_to[0]==0)){
		strcpy(send_mail_to,MAIL_FROM);
	}*/
	
	  char* data35 = get_simmis_data_from_table("GLOBAL_E_CREDIT_mail","");
        if(data35[0]==0 || data35==NULL){
        //Cust mail
	strcpy(send_mail_to,MAIL_FROM);}
        strcpy(send_mail_to,data35);





	sprintf(command,"select * from sim_user where username='%s' and domainname='%s'",user,domain);
	do_log(command);
	int recno = sel.ExecTuplesOk(command);
	strcpy(sales_email_combined,"");
	if (sel.ExecTuplesOk(command)){
		strcpy(sales_name,sel.get_field(0,"sales_person"));
		strcpy(clientname,sel.get_field(0,"clientname"));
		strcpy(companyname,sel.get_field(0,"companyname"));
		char *saleeid=strtok(sales_name,"/");
		tmp_email[0]='\0';
		get_sim_sales_email(saleeid,tmp_email);
		if (strlen(tmp_email)==0)
			sprintf(sales_email_combined,"%s@%s",saleeid,domain);
		else
			sprintf(sales_email_combined,"%s",tmp_email);
		//strcpy(tmp_email,"");
		tmp_email[0]='\0';
		while(saleeid != NULL){
			saleeid = strtok(NULL, "/");
			if((saleeid!=NULL)){
				get_sim_sales_email(saleeid,tmp_email);
				if (strlen(tmp_email)==0)
					sprintf(sales_email_combined,"%s,%s@%s",sales_email_combined,saleeid,domain);
				else{
					sprintf(sales_email_combined,"%s,%s",sales_email_combined,tmp_email);
					strcpy(tmp_email,"");
				}
			}
		}
	}
	else{
		do_log("Could not find this client");
		db->ExecCommandOk("rollback work");
		return 0;
	}
        bool is_country_postpaid = 0;
        bool is_dd_enabled = 0;
        bool add_credit_on_booking = 0;
        bool add_e_credit_entry_on_booking = 0;
        bool add_reg_charge = 0;
	//char ttttt[50];
	//sprintf(ttttt,"postpaid:%B",is_country_postpaid);
	//do_log(ttttt);
        sprintf(command,"SELECT * from countrydata where countryname='%s'",country);
        do_log(command);
        if(!db->ExecTuplesOk(command)){
                do_log("No country from countrydata");
                do_log("rollback");
		db->ExecCommandOk("rollback work");
                return 0;
        }
        if (!strcmp(db->get_field(0,"postpaid_prepaid"),"postpaid")) is_country_postpaid = 1;
        else is_country_postpaid = 0;
        if (!strcmp(db->get_field(0,"is_dd_enabled"),"1")) is_dd_enabled = 1;
        else is_dd_enabled = 0;
        if (!strcmp(db->get_field(0,"add_credit_on_booking"),"1")) add_credit_on_booking = 1;
        else add_credit_on_booking = 0;
        if (!strcmp(db->get_field(0,"add_e_credit_entry_on_booking"),"1")) add_e_credit_entry_on_booking = 1;
        else add_e_credit_entry_on_booking = 0;
        if (!strcmp(db->get_field(0,"add_reg_charge"),"1")) add_reg_charge = 1;
        else add_reg_charge = 0;
	
        //sprintf(ttttt,"postpaid:%d",is_country_postpaid);
        //do_log(ttttt);

	sprintf(sales_email_combined,"%s,%s",sales_email_combined,send_mail_to);
	/*if ((strstr(country,"USA WORLDWIDE") && ((!strcmp(status,"Returned")) || (!strcmp(status,"Canceled"))))){
		//sprintf(email_msg,"Hello,\n\nThe USA WORLDWIDE customer %s (Company Name %s,User ID %s) has just %s the card (%s). Please do the needful.",clientname,companyname,user,status,simno);
		//do_log(email_msg);
	  	//do_log("In USWW return");
		do_log(simno);
		sprintf(command,"USA WORLDWIDE trip %s",status);
		if (!send_mail(sales_email_combined,email_msg,command,1))
			do_log("failed to send");
		else	do_log("sent");

	}*/
	//if(!strcmp(country,"USA WORLDWIDE"))			//Added by neekhil dedhia
	//	add_credit_on_booking=1;
	//if ((strstr(country,"GLOBAL") || strstr(country,"Europe DirectCall") || strstr(country,"WorldFree")) && ((!strcmp(status,"Returned")) || (!strcmp(status,"Canceled")) )){}
	if (add_credit_on_booking && ((!strcmp(status,"Returned")) || (!strcmp(status,"Canceled")))){
	//simno,t
//check the balance, save it in clienttrip and make zero

		if (!strcmp(status,"Returned"))						//Added for Unlimited Calling Plan for Cloud9 and Telna Sims when they are returned.
		{
                	sprintf(command,"select * from unlimited_calling where to_date>='now' and tripid='%s' and enabled=true",t);
                        do_log(command);
                        if(sel.ExecTuplesOk(command))
                        {
                        	sprintf(command,"/usr/local/ownmail/bin/uc_cloud9_telna.sh prepaid_uc returned %s %s 1>/dev/null 2>/dev/null",sel.get_field(0,"to_date"),t);
                        	do_log(command);
                        	system(command);
                        }
		}

                sprintf(command,"select * from countrydata where countryname='%s'",country);
                do_log(command);
                char api_code[20];
                if(db->ExecTuplesOk(command)){
                      strcpy(api_code,db->get_field(0,"api_code"));
                }else
                      do_log("No result");

		gsim GSIM;
                int res = GSIM.init_gsim_with_sim_phone_no(db,simno);
                if (!res){
                        do_log("init_gsim_with_sim_phone_no failed");
                        printf("Could not remove the balance from the global card, please do it manually<br>\n");
                        db->ExecCommandOk("rollback work");
                        return 0;
                }

                if(strcmp(api_code,"telna") && strcmp(api_code,"dummy")){                    //Added by neekhil dedhia
                res = GSIM.check_balance();
                if (!res){
                        do_log("Failed to get balance");
                        printf("<font color=\"red\">Could not remove the balance from the global card, please do it manually</font><br>\n");
                        db->ExecCommandOk("rollback work");
                        return 0;
                }       }

                double bal;                                             //Added by neekhil dedhia

		if(!strcmp(api_code,"telna"))						//Complete method added by Neekhil Dedhia
		{
			do_log(simno);
			//do_log("testing");
                	char file_name[100];
                	strcpy(file_name,simno);
                	strcat(file_name,"_");
                	strcat(file_name,mis_user);
                	do_log(file_name);
                	char php_cmd[100];
                	sprintf(php_cmd,"%s %s %s",PHP_CMD_P,PHP_FILE_WRITE_PATH,file_name);
                	system(php_cmd);char temp_bal[10];
			char read_file[100];
			//strcpy(read_file,"/tmp/");
			sprintf(read_file,"/tmp/");
			do_log(read_file);
			strcat(read_file,file_name);
                	get_tag_value(read_file,"<!-- SIMDETAILS","--SIMDETAILS>","Balance",temp_bal);do_log(temp_bal);
			bal=atof(temp_bal);
			if (bal!=0){
			char call_php[100];   
			sprintf(call_php,"%s %s",PHP_CMD_P,PHP_ADD_CREDIT_PATH);
                	char param1[12];
                	//char param2[10];
                	strcpy(param1,simno);
			if(strstr(temp_bal,"-"))
			{
				std::string str=temp_bal;
				strcat(param1," ");
				strcat(param1,str.substr(1).c_str());
			}
			else
			{
                		strcat(param1," -");
                		strcat(param1,temp_bal);
			}
			char buf[7];sprintf(buf," %d",getpid());
			strcat(param1,buf);
                	strcat(call_php,param1);
			char dev_null[101];
			sprintf(dev_null," 1>/dev/null 2>/dev/null");
			strcat(call_php,dev_null);
                	do_log(call_php);
                	system(call_php);
			char param2[10];sprintf(param2,"%.2f",bal);  do_log(param2);
			do_log(read_file);
			remove(read_file);
			}
		}

		if(strcmp(api_code,"telna"))						//Added by Neekhil Dedhia
			bal = GSIM.get_old_balance();
		double payments_within_one_year = 0;
		sprintf(command,"the balance now is %.2f, reseting the same",bal);
		if (bal == 0) res =1;
		else{ 
			if(!strcmp(api_code,"telna")) 					//Added by Neekhil Dedhia
			{
				char trans_status[20];
				char buf[7];sprintf(buf,"%d",getpid());
                                char transfilename[30]="/tmp/transaction_status_";strcat(transfilename,buf);do_log(transfilename);
				get_tag_value(transfilename,"<!-- TRANSACTION","--TRANSACTION>","Status",trans_status);
				if(!strcmp(trans_status,"OK"))
					res=1;
				else
					res=0;
				remove(transfilename);
			}	
			else 
				res = GSIM.debit_from_prepaid((0-bal),api_code);
		}
		if (!res){
			do_log("Failed to reset balance");
			printf("<font color=\"red\">Could not remove the balance from the global card, please do it manually</font><br>\n");
			db->ExecCommandOk("rollback work");
			return 0;
		}
		else{
			sprintf(command," Balance removed from simcard:%f",bal);
			do_log(command);
			printf("The balance of %.2f has been removed from the sim card",bal);
		//if(!strstr(country,"GLOBAL POSTPAID")){}
		if (!is_country_postpaid){
			do_log("inside is_country_postpaid");
			int send_refund_mail=1;
			if(!strcmp(country,"USA Euro Data SIM"))		//Added this condition since there are no refunds for USA Euro Data SIMs
			{
				send_refund_mail=0;
			}
			sprintf(command,"send_refund_mail=%d",send_refund_mail);
			do_log(command);
			char activation_charge[5];
			char charge_per[5];
			double e_cre_charge=0;
/*			if((get_tag_value(CONFIG_DATA_FILE,"<!-- GLOBAL_E_CREDIT","-- GLOBAL_E_CREDIT>","activation_charge",activation_charge)!=1) || (activation_charge==NULL || activation_charge[0]==0)){
				do_log("inside get config value for activation charge");
				//if(!strstr(country,"WorldFree Standard")) 		//Since Activation charges will be deducted at time of first payment hence no need to deduct it.
				if(!strstr(country,"WorldFree Standard") && (!strstr(country,"USA Euro Data SIM")))	
					strcpy(activation_charge,"10"); 
				else 
					strcpy(activation_charge,"0");
			}*/

			    char query[1024];
                        sprintf(query,"GLOBAL_E_CREDIT_activation_charge_%s",country);
                        char* data36 = get_simmis_data_from_table(query,"10");
                        strcpy(activation_charge,data36);



			sprintf(command,"activation charges:%s",activation_charge);
			do_log(command);
/*			if((get_tag_value(CONFIG_DATA_FILE,"<!-- GLOBAL_E_CREDIT","-- GLOBAL_E_CREDIT>","charge_per",charge_per)!=1) || (charge_per==NULL || charge_per[0]==0)){
				strcpy(charge_per,"0");
			}*/


			     char* data37 = get_simmis_data_from_table("GLOBAL_E_CREDIT_charge_per","0");
                        strcpy(charge_per,data37);




			e_cre_charge = atof(charge_per);
			sprintf(command,"select sum(amount_given) - sum(amount_paid) as e_credit from e_credit_sim_card where tripid ='%s'",t);
			do_log(command);
			if (sel.ExecTuplesOk(command)){
				e_credit_given = atof(sel.get_field(0,"e_credit"));
			}
			char payments_made_invalid_str[501];
			sprintf(command,"SELECT sum(amount) from payment_discount where trip_id ='%s' and accept_time>timestamp_mi_interval('now','31536000')",t);
			do_log(command);
			if (sel.ExecTuplesOk(command)){
				payments_within_one_year = atof(sel.get_field(0,"sum"));
			}

			//sprintf(ttttt,"Bal:%f Payments:%f",bal,payments_within_one_year);
			//do_log(ttttt);
			if (bal <= payments_within_one_year){
				strcpy(payments_made_invalid_str,"");
			}
			else{
				if(payments_within_one_year>0){
				bal = payments_within_one_year;
				sprintf(payments_made_invalid_str,"An amount of %.2f has not been considered as it is made before 1 year",bal - payments_within_one_year);}
			}
			//sprintf(ttttt,"Bal:%f Payments:%f",bal,payments_within_one_year);
                        //do_log(ttttt);
			if (!strcmp(status,"Returned")){
				if (e_credit_given>0)
					bal_tobe_refunded = bal - e_credit_given- atof(activation_charge);
				else{
					if (bal < atof(activation_charge)) bal_tobe_refunded =0;
					else bal_tobe_refunded = bal - atof(activation_charge);
				}
					
				sprintf(command,"%s SIM %s returned",country,simno);
				//if (!strstr(country,"WorldFree Standard")){
					if (bal_tobe_refunded>0 ) 
					{
						if(send_refund_mail)		//send refund mail for all except USA Euro Data SIMs
							sprintf(email_msg,"Hello,\n\nThe %s sim card customer %s (Company Name %s,User ID %s) has just returned the card (%s). The amount USD %.2f has to be refunded to this customer. %s \n\nPlease do the needful.",country,clientname,companyname,user,simno,bal_tobe_refunded,payments_made_invalid_str);
						else
							sprintf(email_msg,"Hello,\n\nThe %s sim card customer %s (Company Name %s,User ID %s) has just returned the card (%s). There is nothing to be refunded to this customer.",country,clientname,companyname,user,simno);
					}
					else if (bal_tobe_refunded <0)
					{
						sprintf(email_msg,"Hello,\n\nThe %s sim card customer %s (Company Name %s,User ID %s) has just returned the card (%s). The amount USD %.2f has to be recovered from this customer. \n\nPlease do the needful.",country,clientname,companyname,user,simno,0-bal_tobe_refunded);
						//sprintf(email_msg,"Hello,\n\nThe %s sim card customer %s (Company Name %s,User ID %s) has just returned the card (%s). The amount USD %.2f has to be recovered from this customer. %s\n\nPlease do the needful.",country,clientname,companyname,user,simno,0-bal_tobe_refunded,payments_made_invalid_str);
					}
					else
						sprintf(email_msg,"Hello,\n\nThe %s sim card customer %s (Company Name %s,User ID %s) has just returned the card (%s). There is nothing to be refunded or recovered from this customer.",country,clientname,companyname,user,simno);
					do_log(email_msg);
					//Cust mail Mark8
					if (!send_mail(sales_email_combined,email_msg,command,1))
						do_log("failed to send");
					else	do_log("sent");
				//}
			}
			else if(!strcmp(status,"Canceled")){
				sprintf(command,"select * from unlimited_calling where tripid='%s' and enabled=true",t);    //Added for Unlimited Calling Plan for Cloud9 and Telna Sims when they are Canceled.
				do_log(command);
				if(sel.ExecTuplesOk(command))
				{
					//sprintf(command,"update unlimited_calling set enabled=false,status='Canceled',last_update='now'  where tripid='%s'",t);
                        		sprintf(command,"/usr/local/ownmail/bin/uc_cloud9_telna.sh prepaid_uc canceled %s %s 1>/dev/null 2>/dev/null",sel.get_field(0,"to_date"),t);
					do_log(command);
                        		system(command);
				}
				sprintf(command,"select sum(amount) as tot from payment_discount where trip_id ='%s'",t);
				do_log(command);
				double amount_paid =0;
				double diff = 0;
				//char tttt[50];
				//sprintf(tttt,"E_cr:%d, Bal:%d",e_credit_given,bal);
				//do_log(tttt);
				if (sel.ExecTuplesOk(command)){
					amount_paid = atof(sel.get_field(0,"tot"));
					char tttt[50];
                                sprintf(tttt,"E_cr:%f, Bal:%f, Amount_Paid:%f",e_credit_given,bal,amount_paid);
                                do_log(tttt);
					if (amount_paid == 0 ){
						if(bal == e_credit_given){
							do_log("No payment made, no usage ");
							bal_tobe_refunded=0;
						}
						else{
							do_log("No payment made, some usage");
							bal_tobe_refunded=bal - e_credit_given - atof(activation_charge);
						}
					}
					else if (abs(bal - (amount_paid + (e_credit_given*(1 - (e_cre_charge/100))))) <0.5){
						do_log("payment made, but there is no usage");
						bal_tobe_refunded= bal -  e_credit_given;
					}
					else{
						sprintf(command,"payment made %.2f bal %.2f, e credit is %.2f in cancel mode",amount_paid,bal,e_credit_given);
						do_log(command);
						bal_tobe_refunded = bal - e_credit_given;
					}
				}
				//if (!strstr(country,"WorldFree Standard")){
					if (bal_tobe_refunded>0)
						sprintf(email_msg,"Hello,\n\nThe %s sim card customer %s (Company Name %s,User ID %s) has just canceled the trip (%s). The amount USD %.2f has to be refunded to this customer.\n\nPlease do the needful.",country,clientname,companyname,user,simno,bal_tobe_refunded);
					else if (bal_tobe_refunded <0)
						sprintf(email_msg,"Hello,\n\nThe %s sim card customer %s (Company Name %s,User ID %s) has just canceled the trip (%s). The amount USD %.2f has to be recovered from this customer.\n\nPlease do the needful.",country,clientname,companyname,user,simno,0-bal_tobe_refunded);
					else
						sprintf(email_msg,"Hello,\n\nThe %s sim card customer %s (Company Name %s,User ID %s) has just canceled the trip (%s). There is nothing to be refunded or recovered from this customer.",country,clientname,companyname,user,simno);
					do_log(email_msg);
					sprintf(command,"%s SIM %s trip canceled",country,simno);
					//Prob tsim mail
					if (!send_mail(sales_email_combined,email_msg,command,1))
						do_log("failed to send");
					else	do_log("sent");
				//}
			}
			sprintf(command,"update clienttrip set last_credit=%.2f where sim_phone_no='%s' and tripid='%s'",bal_tobe_refunded,simno,t);
			do_log(command);
			if(db->ExecCommandOk(command)<=0){
				do_log("Failed to update clienttrip for last_credit");
                        	printf("<font color=\"red\">Updating last_credit for the trip failed. The amount is %.2f<br></font>\n",bal);
				db->ExecCommandOk("rollback work");
				return 0;
			}
			if (e_credit_given > 0 && strcmp(status, "Returned"))	//Emergancy credit marked as paid
			{
				
				sprintf(command,"update e_credit_sim_card set date_paid='now',amount_paid=amount_given where tripid='%s'",t);
				do_log(command);
				if(db->ExecCommandOk(command)<=0){
					do_log("Failed to update e_credit_sim_card");
                        		printf("<font color=\"red\">Updating e_credit_sim_card for the trip failed. The amount is %.2f</font><br>\n",e_credit_given);
					db->ExecCommandOk("rollback work");
					return 0;
				}
			
				do_log("Emergency credit NOT marked as paid.");
			}
			
                        char narration[201];
                        sprintf(narration,"Reset balance");
                        char sender[201];
                        sprintf(sender,"Reset balance");
                        int res = addCredit(user,domain,narration,(0-bal),sender,"USD",1,0);
                        if(!res){
                                do_log("balance is removed");
                        }
                        else{
                                do_log("failed to remove balance");
                                return 0;
                        }
		}
		}
	}

	else if (!strcmp(status,"Returned")){
		int first_cdr_day;
		int last_cdr_day;
		int monthsdiff;
		int cdr_arrival_day;
		char final_bill_date[21];
		char msg_to_cust_on_ret[501];
		char *emm = NULL;
		char *clientname = NULL;
		char *salutation = NULL;
		sprintf(command,"select salutation,clientname,emailadd from sim_user where username='%s'",user);
		if (sel.ExecTuplesOk(command)){
			emm = sel.get_field(0,"emailadd");
			clientname = sel.get_field(0,"clientname");
			if (emm) emm = strdupa(emm);
			else{
				do_log("could not retrieve email address");
				return -1;
			}
			if (clientname) clientname = strdupa(clientname);
			else{
				do_log("could not retrieve clientname");
				return -1;
			}
			salutation = sel.get_field(0,"salutation");
			if (salutation) salutation = strdupa(salutation);
			else{
				do_log("could not retrieve salutation");
				strcpy(salutation,"");
			}
		}
		else{
			do_log("could not retrieve email address");
			return -1;
		}
		sprintf(command,"SELECT *,from_date::date as from_date_short,return_date::date as return_date_short from clienttrip where tripid_min = (select tripid_min from clienttrip where tripid='%s')",t);
		do_log(command);
		int no_of_clubbed_trips = db->ExecTuplesOk(command);
		if(no_of_clubbed_trips<1){
			do_log("Error. No trips found");
			return -1;
		}
		else if (no_of_clubbed_trips == 1){
			sprintf(command,"SELECT * from countrydata where countryname='%s' and postpaid_prepaid ='postpaid' and monthsdiff is not null and first_cdr_day is not null and last_cdr_day is not null and cdr_arrival_day is not null",country);
			do_log(command);
			if (sel.ExecTuplesOk(command)){
				first_cdr_day = atoi(sel.get_field(0,"first_cdr_day"));
				last_cdr_day = atoi(sel.get_field(0,"last_cdr_day"));
				monthsdiff = atoi(sel.get_field(0,"monthsdiff"));
				cdr_arrival_day = atoi(sel.get_field(0,"cdr_arrival_day"));
				sprintf(command,"select '%s'::date-from_date::date as no_of_days from clienttrip where tripid='%s'",tdate,t);
				do_log(command);
				if (!sel.ExecTuplesOk(command)){
					do_log("Could not find the no_of_days");
					return -1;
				}
				int no_of_days = atoi(sel.get_field(0,"no_of_days"));
				if (no_of_days<10) no_of_days =10;
				strcpy(final_bill_date,return_final_bill_dt(tdate,first_cdr_day,last_cdr_day,monthsdiff,cdr_arrival_day));
				sprintf(msg_to_cust_on_ret,"Dear %s %s,\n\nWe have received the sim card %s (User ID: %s) on %d %s %04d. You can expect the final bill for this trip around %02d %s %04d. The minimum bill amount would be US$ %d.\n\nTSIM Team",salutation,clientname,simno,user,return_day(tdate),return_monthname(tdate),return_year(tdate),return_day(final_bill_date),return_monthname(final_bill_date),return_year(final_bill_date),no_of_days);
			}
		}
		else{
			bool other_trips_pending = 0;
			fax_database db_status(E2FAXDB,DBSERV);
			char tmp_date[21];
			char all_sims_in_clubbed_trip[201];
			strcpy(all_sims_in_clubbed_trip,"");
			strcpy(final_bill_date,"");
			char earliest_from_date[21];
			char latest_return_date[21];
			strcpy(earliest_from_date,"");
			strcpy(latest_return_date,"");
			for (int i=0;i<no_of_clubbed_trips;i++){
				char *str_ret_date_tmp = db->get_field(i,"return_date");
				if (!(str_ret_date_tmp==NULL || str_ret_date_tmp[0]==0)){
					str_ret_date_tmp = strdupa(str_ret_date_tmp);
					sprintf(command,"SELECT * from countrydata where countryname = (select country from sim_stock where sim_phone_no =(select sim_phone_no from clienttrip where tripid='%s')) and postpaid_prepaid ='postpaid' and monthsdiff is not null and first_cdr_day is not null and last_cdr_day is not null and cdr_arrival_day is not null",db->get_field(i,"tripid"));
					do_log(command);
					if (db_status.ExecTuplesOk(command)){
						first_cdr_day = atoi(db_status.get_field(0,"first_cdr_day"));
						last_cdr_day = atoi(db_status.get_field(0,"last_cdr_day"));
						monthsdiff = atoi(db_status.get_field(0,"monthsdiff"));
						cdr_arrival_day = atoi(db_status.get_field(0,"cdr_arrival_day"));
						strcpy(tmp_date,db->get_field(i,"from_date_short"));
						if (strlen(earliest_from_date)>0){
							if (strcmp(tmp_date,earliest_from_date)<0) strcpy(earliest_from_date,tmp_date);
						}
						else strcpy(earliest_from_date,tmp_date);
						sprintf(command,"earliest_from_date %s",earliest_from_date);
						do_log(command);
						strcpy(tmp_date,db->get_field(i,"return_date_short"));
						if (strlen(latest_return_date)>0){
							if (strcmp(tmp_date,latest_return_date)>0) strcpy(latest_return_date,tmp_date);
						}
						else strcpy(latest_return_date,tmp_date);
						sprintf(command,"latest_return_date %s",latest_return_date);
						do_log(command);
						strcpy(tmp_date,return_final_bill_dt(str_ret_date_tmp,first_cdr_day,last_cdr_day,monthsdiff,cdr_arrival_day));
						if (strlen(final_bill_date)>0){
							if (strcmp(tmp_date,final_bill_date)>0) strcpy(final_bill_date,tmp_date);
						}
						else strcpy(final_bill_date,tmp_date);
					}
				}
				else{
					sprintf(command,"The sim_phone_no %s is not yet returned",db->get_field(i,"sim_phone_no"));
					do_log(command);
					other_trips_pending =1;
				}
				if (strlen(all_sims_in_clubbed_trip)==0) strcpy(all_sims_in_clubbed_trip,db->get_field(i,"sim_phone_no"));
				else sprintf(all_sims_in_clubbed_trip,"%s,%s",all_sims_in_clubbed_trip,db->get_field(i,"sim_phone_no"));
			}
			sprintf(command,"tdate %s return_monthname %s",return_monthname(tdate),tdate);
			do_log(command);
			if (other_trips_pending)
				sprintf(msg_to_cust_on_ret,"Dear %s %s,\n\nWe have received the sim card %s (User ID: %s) on %d %s %04d. For this clubbed trip consisting of the numbers %s, all have not been returned. Hence we would not be able to tell you the expected final bill date. You will be notified on email about the expected final date and the minimum bill amount as and when all cards are returned.\n\nTSIM Team",salutation,clientname,simno,user,return_day(tdate),return_monthname(tdate),return_year(tdate),all_sims_in_clubbed_trip);
			else{
				sprintf(command,"select '%s'::date-'%s'::date as no_of_days",latest_return_date,earliest_from_date);
				do_log(command);
				if (!sel.ExecTuplesOk(command)){
					do_log("Could not find the no_of_days");
					return -1;
				}
				int no_of_days = atoi(sel.get_field(0,"no_of_days"));
				if (no_of_days<10) no_of_days =10;
				sprintf(msg_to_cust_on_ret,"Dear %s %s,\n\nWe have received the sim card %s (User ID: %s) on %d %s %04d. You can expect the final bill for this clubbed trip consisting of the numbers %s around %02d %s %04d. The minimum bill amount would be US$ %d.\n\nTSIM Team",salutation,clientname,simno,user,return_day(tdate),return_monthname(tdate),return_year(tdate),all_sims_in_clubbed_trip,return_day(final_bill_date),return_monthname(final_bill_date),return_year(final_bill_date),no_of_days);
			}
		}
	//send mail to cust
		do_log("sending mail to customer");
		do_log(msg_to_cust_on_ret);
		char subj[201];
		sprintf(subj,"TSIM number %s (User ID: %s %s %s) received",simno,user,salutation,clientname);
		//Cust mail Mark9

		 char* escapedMessage = escapeForSQL(msg_to_cust_on_ret);
		 do_log("inserting in tsim_order_notification for final bill");
		 sprintf(command,"insert into tsim_order_notifications "
				 "(username,email,mobno,notification_type "
				 "subject_content,message_content,status,priority,from_email) "
				 "values ('%s','%s','%s','{email}', "
				 "'{%s},'{%s}','{email_pending}','normal','services@tsim.in');"
				 ,user,emm,simno,subj,escapedMessage
				 );
		 do_log(command);
		
		if (sel.ExecCommandOk(command)<=0){
			do_log("failed to send");
			send_mail("services@tsim.in",msg_to_cust_on_ret,subj);
		}
		else	do_log("sent");
		delete[] escapedMessage;
	}

	return 0;
}
int savedetails(fax_database *db){
	do_log("Inside savedetails!!!");
	int i;
	//fax_database db(E2FAXDB,DBSERV);
	char command[1024];
	char *user=cgi_getentrystr("b");
	char *salutation=cgi_getentrystr("selsal");
	char *clientname=cgi_getentrystr("txtname");
	char *companyname=cgi_getentrystr("txtcompany");
	char *add_bldg=cgi_getentrystr("txtbldg");
	char *add_street=cgi_getentrystr("txtstreet");
	char *add_pinno=cgi_getentrystr("txtpin");
	char *add_country=cgi_getentrystr("selcountry");
	char *add_city=cgi_getentrystr("selcity");
	char *add_state=cgi_getentrystr("selstate");
	char *deliver_add_bldg=cgi_getentrystr("txtdbldg");
	char *deliver_add_street=cgi_getentrystr("txtdstreet");
	char *deliver_add_pinno=cgi_getentrystr("txtdpin");
	char *deliver_add_country=cgi_getentrystr("dselcountry");
	char *deliver_add_city=cgi_getentrystr("seldcity");
	char *deliver_add_state=cgi_getentrystr("seldstate");
	char *mobno=cgi_getentrystr("txtmobno");
	char *telno=cgi_getentrystr("txtphone");;
	char *expdate=cgi_getentrystr("txtexp");
	char *cc_holder_name=cgi_getentrystr("txtnccno");
	char *creditcard_no=cgi_getentrystr("txtccno");;
	char *emailadd=cgi_getentrystr("txtemail");;
	char *sales_person=cgi_getentrystr("e");
	char *chkaddr=cgi_getentrystr("chkaddr");
	char *passport_no=cgi_getentrystr("pno");
	char *passport_city=cgi_getentrystr("ppcity");
	if(chkaddr==NULL)
                        chkaddr=strdup("off");
	if(!strcmp(chkaddr,"on")){
                        deliver_add_bldg=add_bldg;
                        deliver_add_street=add_street;
                        deliver_add_pinno=add_pinno;
                        deliver_add_country=add_country;
                        deliver_add_city=add_city;
                        deliver_add_state=add_state;
                }
	draw_header(2);
	printf("<div id=mh style=""width:99%;height:95%"">");
	printf("<form name=frmsave action=simmisn.e method=post>");
	printf("<pre>");
	if(strcmp(deliver_add_bldg,"'"))
		deliver_add_bldg=replacestring(deliver_add_bldg,"'","''");
	if(strcmp(deliver_add_street,"'"))
		deliver_add_street=replacestring(deliver_add_street,"'","''");
	if(strcmp(add_bldg,"'"))
		add_bldg=replacestring(add_bldg,"'","''");
	if(strcmp(add_street,"'"))
		add_street=replacestring(add_street,"'","''");

/* <ADDED LINES TO CHECK IF MOBILENUMBER IS REGISTERED WITH ANY OTHER ID> */
	sprintf(command,"SELECT * from sim_user where mobno ='%s' and username !='%s' and username !=(select oldusername from trikon_tsim_map where newusername ='%s')",replacestring(replacestring(mobno,"-",""),"+",""),user,user);
	do_log(command);
	if (db->ExecTuplesOk(command)){
		do_log("The same mobile is regd for another user");
		printf("The mobile number is already registered with %s. The clientname is %s.\nIf you think this ID is no more using the service, please remove the number from this and then proceed",db->get_field(0,"username"),db->get_field(0,"clientname"));
		printf("\n</pre>");
		printf("\n</form>");
		printf("\n</div>");
		draw_end();
		return 0;
	}
	sprintf(command,"SELECT * from callerid_user where regd_number ='%s' and username !='%s' and username !=(select oldusername from trikon_tsim_map where newusername ='%s')",replacestring(replacestring(mobno,"-",""),"+",""),user,user);
	do_log(command);
	if (db->ExecTuplesOk(command)){
		do_log("The same mobile is regd for another user");
		printf("The mobile number is already registered as a callerid for the username %s.\nIf you think this ID is no more using the service, please remove the number from this and then proceed",db->get_field(0,"username"));
		printf("\n</pre>");
		printf("\n</form>");
		printf("\n</div>");
		draw_end();
		return 0;
	}
/* </ADDED LINES TO CHECK IF MOBILENUMBER IS REGISTERED WITH ANY OTHER ID> */

/* <ADDED LINES TO ADD THE MOBILENUMBER TO CALLERID TABLE> */
	sprintf(command,"SELECT * from callerid_user where regd_number = (select mobno as regd_number from sim_user where username ='%s')",user);
	do_log(command);
	if (db->ExecTuplesOk(command))
		sprintf(command,"update callerid_user set regd_number ='%s',username='%s' where regd_number= (select mobno as regd_number from sim_user where username ='%s')",replacestring(replacestring(mobno,"-",""),"+",""),user,user);
	else
		sprintf(command,"insert into callerid_user values('%s','%s')",replacestring(replacestring(mobno,"-",""),"+",""),user);
	do_log(command);
	if (db->ExecCommandOk(command))
		do_log("updated callerid_user");
	else{
		do_log("failed to update callerid_user");
		sprintf(command,"Hello,\n\nFor the SIM Card customer %s (username %s), system could not register the India number %s.\n\nPlease do the needful.",clientname,user,replacestring(replacestring(mobno,"-",""),"+",""));
		char mailto[1024];
/*		if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
			do_log("Could not open config file.");
			printf("Error occured while opening config file.");
		}
		else{
			if (!send_mail(mailto,command,"Registering India number failed")){
				do_log("failed to send");
			}
			else	do_log("sent");
		}*/


		     char* data38 = get_simmis_data_from_table("MAILTO_mailto","");
                if(data38[0]==0 || data38==NULL)
                {
                     do_log("ERROR OCCURED WHILE OPENING EMAIL_TO_CUSTOMER file data38");
                     printf("ERROR OCCURED WHILE OPENING THE EMAIL_TO_CUSTOMER FILE");
                }
                //TSIM mail
		strcpy(mailto,data38);
                if (!send_mail(mailto,command,"Registering India number failed"))
                {
                       do_log("failed to send");
                }
                else    do_log("sent");






	}
/* </ADDED LINES TO ADD THE MOBILENUMBER TO CALLERID TABLE> */

	/*
	if(sales_person==NULL||sales_person==0||(!strcmp(sales_person,""))) //ALSO UPDATE CSREFERRALS 
		sprintf(command,"update sim_user set sales_person=sales_person where  username='%s'",user);
	else
		sprintf(command,"update sim_user set sales_person='%s' where  username='%s'",replacestring(sales_person,",","/"),user);
	do_log(command);
	*/
	char sp_sp[1024];
	if(sales_person==NULL||sales_person==0||(!strcmp(sales_person,"")))
		strcpy(sp_sp,"");
	else{
		strcpy(sp_sp,replacestring(sales_person,",","/"));
		if (canchange_sales){
//change csreferrals
			char sales_arr[MAX_FIELDS_STR][MAX_LEN_STR];
			int nsales=str_split('/',sp_sp,sales_arr);
			time_t t;
			struct tm *tmst;
			char seq[41];
			if (nsales <1){
				do_log("failed to split");
			}
			else{
				sprintf(command,"delete from csreferrals where username='%s@%s'",user,"sim.trikon.in");
				do_log(command);
				if(db->ExecCommandOk(command)>0){
					for (int j=0;j<nsales;j++){
						if (sales_arr[j]){
							time(&t);
							tmst = localtime(&t);
							sprintf(seq, "%04d%02d%02d%02d%02d%02d%02d", 1900+tmst->tm_year, tmst->tm_mon+1, tmst->tm_mday, tmst->tm_hour,tmst->tm_min,tmst->tm_sec,j);
							sprintf(command,"insert into csreferrals(seq,username,referredby,rootid) values('%s','%s@tsim.in','%s@tsim.in','%s@tsim.in')",seq,user,sales_arr[j],sales_arr[j]);
							do_log(command);
							if(db->ExecCommandOk(command)>0){
								do_log("added");
							}
							else{
								do_log("failed to add");
								printf("<br>Failed to add sales %s<br>\n",sales_arr[j]);
							}
						}
					}
				}
				else{
					do_log("could not delete from csreferrals");
					printf("<br>Could not update sales<br>\n");
				}
			}
		}
	}
	sprintf(command,"expdate -%s-",expdate);
	do_log(command);

	if(expdate==NULL||expdate[0]==0)
		 //sprintf(command,"update sim_user set salutation='%s',clientname='%s',companyname='%s',add_building='%s',add_street='%s',add_pinno='%s',add_country='%s',add_city='%s',add_state='%s',deliver_add_building='%s',deliver_add_street='%s',deliver_add_pinno='%s',deliver_add_country='%s',deliver_add_city='%s',deliver_add_state='%s',mobno='%s',telno='%s',emailadd='%s',sales_person='%s' where username='%s'",salutation,clientname,companyname,add_bldg,add_street,add_pinno,add_country,add_city,add_state,deliver_add_bldg,deliver_add_street,deliver_add_pinno,deliver_add_country,deliver_add_city,deliver_add_state,replacestring(replacestring(mobno,"-",""),"+",""),replacestring(replacestring(telno,"-",""),"+",""),emailadd,sp_sp,user);
		if (canchange_sales)
		 	sprintf(command,"update sim_user set salutation='%s',clientname='%s',companyname='%s',add_building='%s',add_street='%s',add_pinno='%s',add_country='%s',add_city='%s',add_state='%s',deliver_add_building='%s',deliver_add_street='%s',deliver_add_pinno='%s',deliver_add_country='%s',deliver_add_city='%s',deliver_add_state='%s',mobno='%s',telno='%s',emailadd='%s',passport_no='%s',passport_city='%s',sales_person='%s' where username='%s'",salutation,clientname,companyname,add_bldg,add_street,add_pinno,add_country,add_city,add_state,deliver_add_bldg,deliver_add_street,deliver_add_pinno,deliver_add_country,deliver_add_city,deliver_add_state,replacestring(replacestring(mobno,"-",""),"+",""),replacestring(replacestring(telno,"-",""),"+",""),emailadd,passport_no,passport_city,sp_sp,user);
		else
		 	sprintf(command,"update sim_user set salutation='%s',clientname='%s',companyname='%s',add_building='%s',add_street='%s',add_pinno='%s',add_country='%s',add_city='%s',add_state='%s',deliver_add_building='%s',deliver_add_street='%s',deliver_add_pinno='%s',deliver_add_country='%s',deliver_add_city='%s',deliver_add_state='%s',mobno='%s',telno='%s',emailadd='%s',passport_no='%s',passport_city='%s' where username='%s'",salutation,clientname,companyname,add_bldg,add_street,add_pinno,add_country,add_city,add_state,deliver_add_bldg,deliver_add_street,deliver_add_pinno,deliver_add_country,deliver_add_city,deliver_add_state,replacestring(replacestring(mobno,"-",""),"+",""),replacestring(replacestring(telno,"-",""),"+",""),emailadd,passport_no,passport_city,user);
	else
		//sprintf(command,"update sim_user set salutation='%s',clientname='%s',companyname='%s',add_building='%s',add_street='%s',add_pinno='%s',add_country='%s',add_city='%s',add_state='%s',deliver_add_building='%s',deliver_add_street='%s',deliver_add_pinno='%s',deliver_add_country='%s',deliver_add_city='%s',deliver_add_state='%s',mobno='%s',telno='%s',expdate='%s',cc_holder_name='%s',creditcard_no='%s',emailadd='%s',sales_person='%s' where username='%s'",salutation,clientname,companyname,add_bldg,add_street,add_pinno,add_country,add_city,add_state,deliver_add_bldg,deliver_add_street,deliver_add_pinno,deliver_add_country,deliver_add_city,deliver_add_state,replacestring(replacestring(mobno,"-",""),"+",""),replacestring(replacestring(telno,"-",""),"+",""),expdate,cc_holder_name,creditcard_no,emailadd,sp_sp,user);
		if (canchange_sales)
			sprintf(command,"update sim_user set salutation='%s',clientname='%s',companyname='%s',add_building='%s',add_street='%s',add_pinno='%s',add_country='%s',add_city='%s',add_state='%s',deliver_add_building='%s',deliver_add_street='%s',deliver_add_pinno='%s',deliver_add_country='%s',deliver_add_city='%s',deliver_add_state='%s',mobno='%s',telno='%s',expdate='%s',cc_holder_name='%s',creditcard_no='%s',emailadd='%s',passport_no='%s',passport_city='%s',sales_person='%s' where username='%s'",salutation,clientname,companyname,add_bldg,add_street,add_pinno,add_country,add_city,add_state,deliver_add_bldg,deliver_add_street,deliver_add_pinno,deliver_add_country,deliver_add_city,deliver_add_state,replacestring(replacestring(mobno,"-",""),"+",""),replacestring(replacestring(telno,"-",""),"+",""),expdate,cc_holder_name,creditcard_no,emailadd,passport_no,passport_city,sp_sp,user);
		else
			sprintf(command,"update sim_user set salutation='%s',clientname='%s',companyname='%s',add_building='%s',add_street='%s',add_pinno='%s',add_country='%s',add_city='%s',add_state='%s',deliver_add_building='%s',deliver_add_street='%s',deliver_add_pinno='%s',deliver_add_country='%s',deliver_add_city='%s',deliver_add_state='%s',mobno='%s',telno='%s',expdate='%s',cc_holder_name='%s',creditcard_no='%s',emailadd='%s',passport_no='%s',passport_city='%s' where username='%s'",salutation,clientname,companyname,add_bldg,add_street,add_pinno,add_country,add_city,add_state,deliver_add_bldg,deliver_add_street,deliver_add_pinno,deliver_add_country,deliver_add_city,deliver_add_state,replacestring(replacestring(mobno,"-",""),"+",""),replacestring(replacestring(telno,"-",""),"+",""),expdate,cc_holder_name,creditcard_no,emailadd,passport_no,passport_city,user);
	do_log(command);
	printf("\n<input type=hidden name=a>");
	envelope *envlp;
	envlp=(envelope *)malloc(sizeof(envelope));
	char *domain_name;
	domain_name = getenv("ALTERNATE_DOMAIN");
            if (domain_name == NULL || domain_name[0] == 0) {
                        do_log("d2");
                    domain_name = getenv("HTTP_HOST");
            }
	if(db->ExecCommandOk(command)>0){
		printf("\n<b>     Details of user %s updated successfully.</b>",user);
		printf("\n<b>     Do you want to search more user details?</b>");
		if(!strcmp(domain_name,"sim.trikon.in"))
		{
			printf("\n        <br><input type=button name=btnyes value=Yes onclick=\"refpage('http://sim.trikon.in/simmisn.e?a=searchclient');\">");
			printf("<input type=button name=btnno value=No onclick=\"refpage('http://sim.trikon.in/simmisn.e');\">");
		}
		else
		{
			printf("\n        <br><input type=button name=btnyes value=Yes onclick=\"refpage('http://mail.tsim.in/simmisn.e?a=searchclient');\">");
			printf("<input type=button name=btnno value=No onclick=\"refpage('http://mail.tsim.in/simmisn.e');\">");
		}
	}
	else{
		printf("\n<b>     Details of user %s was not modified.</b>",user);
	}
	printf("\n</pre>");
	printf("\n</form>");
	printf("\n</div>");
	draw_end();
	return 0;
}
/*int editstock()
{
	printf("\n Coming soon!");
	char *simno=cgi_getentrystr("b");
	printf("\n%s",simno);
}*/
int edituser(fax_database *db){
	//fax_database db(E2FAXDB,DBSERV_R);
	char command[1024];
	int i;	
	char *user=cgi_getentrystr("b");
	sprintf(command,"select * from sim_user where username='%s'",user);
	do_log(command);
	if(db->ExecTuplesOk(command)){
	char *salutation=db->get_field(0,"salutation");
		salutation=strdup(salutation);
	char *clientname=db->get_field(0,"clientname");
		clientname=strdup(clientname);
	char *companyname=db->get_field(0,"companyname");
	if((companyname==NULL)||companyname[0]==0)
		companyname="";
	else
		companyname=strdup(companyname);
	char *add_bldg=db->get_field(0,"add_building");
	if((add_bldg==NULL)||add_bldg[0]==0)
		add_bldg="";
	else
		add_bldg=strdup(add_bldg);
	char *add_street=db->get_field(0,"add_street");
	if((add_street==NULL)||add_street[0]==0)
		add_street="";
	else
		add_street=strdup(add_street);
	char *add_pinno=db->get_field(0,"add_pinno");
	if((add_pinno==NULL)||add_pinno[0]==0)
		add_pinno="";
	else
		add_pinno=strdup(add_pinno);
	char *add_country=db->get_field(0,"add_country");
	if((add_country==NULL)||add_country[0]==0)
		add_country="";
	else
		add_country=strdup(add_country);
	char *add_city=db->get_field(0,"add_city");
	if((add_city==NULL)||add_city[0]==0)
		add_city="";
	else
		add_city=strdup(add_city);
	char *add_state=db->get_field(0,"add_state");
	if((add_state==NULL)||add_state[0]==0)
		add_state="";
	else
		add_state=strdup(add_state);
	char *deliver_add_bldg=db->get_field(0,"deliver_add_building");
	if((deliver_add_bldg==NULL)||deliver_add_bldg[0]==0)
		deliver_add_bldg="";
	else	
		deliver_add_bldg=strdup(deliver_add_bldg);
	char *deliver_add_street=db->get_field(0,"deliver_add_street");
	if((deliver_add_street==NULL)||deliver_add_street[0]==0)
		deliver_add_street="";
	else
		deliver_add_street=strdup(deliver_add_street);
	char *deliver_add_pinno=db->get_field(0,"deliver_add_pinno");
	if((deliver_add_pinno==NULL)||deliver_add_pinno[0]==0)
		deliver_add_pinno="";
	else
		deliver_add_pinno=strdup(deliver_add_pinno);
	char *deliver_add_country=db->get_field(0,"deliver_add_country");
	if((deliver_add_country==NULL)||deliver_add_country[0]==0)
		deliver_add_country="";
	else
		deliver_add_country=strdup(deliver_add_country);
	char *deliver_add_city=db->get_field(0,"deliver_add_city");
	if((deliver_add_city==NULL)||deliver_add_city[0]==0)
		deliver_add_city="";
	else
		deliver_add_city=strdup(deliver_add_city);
	char *deliver_add_state=db->get_field(0,"deliver_add_state");
	if((deliver_add_state==NULL)||deliver_add_state[0]==0)
		deliver_add_state="";
	else
		deliver_add_state=strdup(deliver_add_state);
	char *mobno=db->get_field(0,"mobno");
	if((mobno==NULL)||mobno[0]==0)
		mobno="";
	else
		mobno=strdup(mobno);
	char *telno=db->get_field(0,"telno");
	if((telno==NULL)||telno[0]==0)
		telno="";
	else	
		telno=strdup(telno);
	char *expdate=db->get_field(0,"expdate");
	if((expdate==NULL)||expdate[0]==0)
		expdate="";
	else
		expdate=strdup(expdate);
	char *cc_holder_name=db->get_field(0,"cc_holder_name");
	if((cc_holder_name==NULL)||cc_holder_name[0]==0)
		cc_holder_name="";
	else
		cc_holder_name=strdup(cc_holder_name);
	char *creditcard_no=db->get_field(0,"creditcard_no");
	if((creditcard_no==NULL)||creditcard_no[0]==0)
		creditcard_no="";
	else
		creditcard_no=strdup(creditcard_no);
	char *emailadd=db->get_field(0,"emailadd");
	if((emailadd==NULL)||emailadd[0]==0)
		emailadd="";
	else
		emailadd=strdup(emailadd);
	char *sales_person=db->get_field(0,"sales_person");
	if((sales_person==NULL)||sales_person[0]==0)
		sales_person="";
	else
		sales_person=strdup(sales_person);
	char *passport_no=db->get_field(0,"passport_no");
	if((passport_no==NULL)||passport_no[0]==0)
		passport_no="";
	else
		passport_no=strdup(passport_no);
	char *passport_city=db->get_field(0,"passport_city");
	if((passport_city==NULL)||passport_city[0]==0)
		passport_city="";
	else
		passport_city=strdup(passport_city);
	printf("<form name=frmshowclient action=simmisn.e method=post>");	
	printf("\n<br><label>Contact Person Name:</label>");
	printf("\n<select name=selsal>");
	if(strcmp(salutation,""))
		printf("<option value=\"%s\">%s</option>",salutation,salutation);
	else
		printf("<option value=none>Select</option>");
	if(strcmp(salutation,"Dr."))
		printf("<option value=Dr.>Dr.</option>");
	if(strcmp(salutation,"Ms."))
		printf("<option value=Ms.>Ms.</option>");
	if(strcmp(salutation,"Mrs.")) 
		printf("<option value=Mrs.>Mrs.</option>");
	if(strcmp(salutation,"Mr."))
		printf("<option value=Mr.>Mr.</option>");
	printf("</select>");
	printf("\n<input type=hidden name=e>");
	printf("\n<input type=hidden name=b>");
	printf("\n<input type=hidden name=a>");
	printf("\n<input type=text name=txtname onblur=\"return validname(this.value);\" value=\"%s\">",clientname);
	printf("\n<br><label>Company Name:</label><input type=text  name=txtcompany  value=\"%s\">\n",companyname);
	printf("\n<br><label>Building Name:</label><input type=text name=txtbldg onblur=\"return validaddress(this.value);\" value=\"%s\">\n",add_bldg);
	printf("\n<br><label>Street:</label><input type=text name=txtstreet onblur=\"return validaddress(this.value);\" value=\"%s\" >\n ",add_street);
	printf("\n<br><label>Pin Code:</label><input type=text name=txtpin onblur=\"return validpinno(this.value);\" value=\"%s\" >\n",add_pinno);
	printf("\n<br><label>Country:</label>");
	printf("\n<select name=selcountry onchange=\"setcountry(this.value,'dcity','dstate','selstate');\">\n");
	if(strcmp(add_country,""))
		printf("<option value=\"%s\">%s</option>",add_country,add_country);		
	else
		printf("<option value=none>Select</option>");
	sprintf(command,"select distinct country from country_state_city");
	do_log(command);
	int rec_cnt = db->ExecTuplesOk(command);
	for(i=0;i<rec_cnt;i++)
	{
		if(strcmp(add_country,db->get_field(i,"country")))
			printf("<option value=\"%s\">%s</option>",db->get_field(i,"country"),db->get_field(i,"country"));
	}
	printf("</select>");
	printf("\n<div id=\"dstate\">");
	printf("\n<label>State:</label>");
	printf("\n<select name=selstate onchange=\"new net.ContentLoader('simmisn.e?a=fillcity&b='+this.value+'&c=selcity',parseMyData,'dcity',null,'Get');\">");
	if(strcmp(add_state,""))
		printf("<option value=\"%s\">%s</option>",add_state,add_state);
	else
		printf("<option value=none>Select</option>");
	sprintf(command,"select distinct state from country_state_city where country='%s' order by state",add_country);
	do_log(command);
	rec_cnt = db->ExecTuplesOk(command);
	for(i=0;i<rec_cnt;i++)
	{
		if(strcmp(add_state,db->get_field(i,"state")))
			printf("<option value=\"%s\">%s</option>",db->get_field(i,"state"),db->get_field(i,"state"));
	}
	printf("</select>");
	printf("</div>");
	printf("\n<div id=\"dcity\">");
	printf("\n<label>City:</label>");
	printf("\n<select name=selcity>");
	if(strcmp(add_city,""))
		printf("<option value=\"%s\">%s</option>",add_city,add_city);
	else
		printf("<option value=none>Select</option>");
	sprintf(command,"select distinct city from country_state_city where state='%s' order by city",add_state);
	do_log(command);
	rec_cnt = db->ExecTuplesOk(command);
	for(i=0;i<rec_cnt;i++)
	{
		if(strcmp(add_city,db->get_field(i,"city")))
			printf("<option value=\"%s\">%s</option>",db->get_field(i,"city"),db->get_field(i,"city"));
	}
	
	printf("</select>");
	printf("\n</div>");
	printf("\n<input type=checkbox name=chkaddr onclick=\"toggledeladdr('frmshowclient');\" onchange=\"toggledeladdr(this.value);\">Click here if delivery address is same as billing address.");
	printf("<div id=daddress>");
	printf("\n<br><br><b><u>Delivery address:</u></b><br>");
	printf("\n<br><label>Building Name:</label><input type=text name=txtdbldg onblur=\"return validaddress(this.value);\" value=\"%s\">",deliver_add_bldg);
	printf("\n<br><label>Street:</label><input type=text name=txtdstreet onblur=\"return validaddress(this.value);\" value=\"%s\">",deliver_add_street);
	printf("<br><label>Pin Code:</label><input type=text name=txtdpin onblur=\"return validpinno(this.value);\" value=\"%s\">",deliver_add_pinno);
	printf("\n<br><label>Country:</label>");
	printf("\n<select name=dselcountry onchange=\"setcountry(this.value,'ddcity','ddstate','seldstate');\">");
	printf("<option value=\"%s\">%s</option>",deliver_add_country,deliver_add_country);
	sprintf(command,"select distinct country from country_state_city order by country");
	do_log(command);
	rec_cnt = db->ExecTuplesOk(command);
	for(i=0;i<rec_cnt;i++)
	{
		if(strcmp(deliver_add_country,db->get_field(i,"country")))
			printf("<option value=\"%s\">%s</option>",db->get_field(i,"country"),db->get_field(i,"country"));
	}
	printf("</select>");
	printf("\n<div id=\"ddstate\">");
	printf("\n<label>State:</label>");
	printf("\n<select name=seldstate id=\"seldstate\"onchange=\"new net.ContentLoader('simmisn.e?a=fillcity&b='+this.value+'&c=seldcity',parseMyData,'ddcity',null,'Get');\">");
	printf("<option value=\"%s\">%s</option>",deliver_add_state,deliver_add_state);
	sprintf(command,"select distinct state from country_state_city where country='%s' order by state",deliver_add_country);
	 do_log(command);
	 rec_cnt = db->ExecTuplesOk(command);
	for(i=0;i<rec_cnt;i++)
	{
		if(strcmp(deliver_add_state,db->get_field(i,"state")))
		       printf("<option value=\"%s\">%s</option>",db->get_field(i,"state"),db->get_field(i,"state"));
	}
	printf("</select>");
	printf("\n</div>");
	printf("\n<div id=\"ddcity\">");
	printf("\n<label>City:</label>");
	printf("\n<select name=seldcity>");
	printf("<option value=\"%s\">%s</option>",deliver_add_city,deliver_add_city);
	sprintf(command,"select distinct city from country_state_city where state='%s' order by city",deliver_add_state);
	do_log(command);
	rec_cnt = db->ExecTuplesOk(command);
	for(i=0;i<rec_cnt;i++)
	{
		if(strcmp(deliver_add_city,db->get_field(i,"city")))
		       printf("<option value=\"%s\">%s</option>",db->get_field(i,"city"),db->get_field(i,"city"));
	}
	printf("</select>");
	printf("</div>");
	printf("</div>");
	printf("\n<br><b><u>Contact details:</u></b><br>");
	printf("\n<br><label>Phone no:</label><input type=text name=txtphone onblur=\"return validphone(this.value,'Phone number');\" value=\"%s\">",telno);
	printf("<br><label>Mobile no:</label><input type=text name=txtmobno onblur=\"return validphone(this.value,'Mobile number');\" value=\"%s\">",mobno);
	printf("\n<br><label>Email Id:</label><input type=text name=txtemail onblur=\"valid_emailid(this.value);\" value=\"%s\">",emailadd);
	printf("\n<br><br><b><u>Credit Card Details:</u></b><br>");
	printf("\n<br><label>Credit Card No:</label><input type=text name=txtccno onblur=\"return checkccno(this.value);\" value=\"%s\">",creditcard_no);
	printf("\n<br><label>Name On Credit Card:</label><input type=text name=txtnccno value=\"%s\">",cc_holder_name);
	printf("\n<input type='hidden' id='txtexp' name='txtexp' >");
	printf("\n<br><label>Expiry Date:</label>");
	sprintf(command,"select to_char(expdate,'yyyy') as year,to_char(expdate,'MONTH')as month,to_char(expdate,'FMMM')as m from sim_user where username='%s';",user);
	int res=db->ExecTuplesOk(command);
	char *month=NULL,*m=NULL,*year=NULL;
	do_log(command);
	do_log(month);
	do_log(m);
	if(res>0)
	{
		if(db->get_field(0,"month")==NULL||db->get_field(0,"month")[0]==0)
			month="";
		else
			month=strdup(db->get_field(0,"month"));
		if(db->get_field(0,"year")==NULL||db->get_field(0,"year")[0]==0)
			year="";
		else
		year=strdup(db->get_field(0,"year"));
		if(db->get_field(0,"m")==NULL||db->get_field(0,"m")[0]==0)	
			m="";
		else
			m=strdup(db->get_field(0,"m"));	
	}
	do_log(month);
	do_log(m);
 
	//cout << "\n<input type='hidden' id='txtexp' name='txtexp' value='"  << string() << "' >");
	/* Credit card expiry date */
	//char *expmonth[12]={"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};
	string expmonth[12]={"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};
	printf("\n<select name='expmonth' id='expMonth' onchange='calc_cc_expdate()' >");
	
	/*
	if(month!=NULL && month[0]!=0)	{ printf("\n\t<option value=\"%s\" selectedI>%s</option>",month,month); }
	else				{ printf("\n\t<option value='none' selected>Month</option>");}
	*/

	string m_str = string(m);
	for(i=0; i<12; i++)
	{
		std:stringstream strm;
		strm << ( i + 1 ) ;
		string i_str = strm.str();
		cout << "\n\t<option value='"
		     << ( i+1 < 10 ? "0" : "" ) << ( i+1 )
		     << "' " << ((i_str == m_str ) ? " selected " : "")  << ">"
		     << expmonth[i] <<"</option>";

	}
	printf("</select>");
	
	int expyear[25]={2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2029,2030};
	printf("\n<select name='expyear' id='expYear' onchange='calc_cc_expdate()' >");
	
	if(year!=NULL && year[0]!=0)	{ printf("\n\t<option value=\"%s\" selected>%s</option>",year,year); }
        else				{ printf("\n\t<option value=none selected>Year</option>"); }
        
	for(i=0;i<25;i++)
        {
		printf("<option value='%d'>%d</option>",expyear[i],expyear[i]);
        }
        printf("</select>");
	
	printf("\n<br><br><b><u>Passport Details:</u></b><br>");
	printf("\n<br><label>Passport No:</label><input type=text name=pno value=\"%s\" onblur=\"this.value=removeSpaces(this.value);return chk_pp(this.value);\">",passport_no);
	printf("\n<br><label>City of isssue:</label><input type=text name=ppcity value=\"%s\">",passport_city);
	if (canchange_sales){
		printf("\n<br><label>Sales Person:</label>");
		printf("\n<select name=selsalep id=\"selsalep\"multiple size=4>");
		printf("<option value=\"%s\" selected>%s</option>",sales_person,sales_person);
		sprintf(command,"select sp_name from salesperson where domainname='sim.trikon.in'");
		do_log(command);
		int count=db->ExecTuplesOk(command);
		for(i=0;i<count;i++){
			if(strcmp(sales_person,db->get_field(i,"sp_name")))
				printf("<option value=\"%s\">%s</option>",db->get_field(i,"sp_name"),db->get_field(i,"sp_name"));
		}
		printf("</select>");
	}
	printf("\n<br><input type=submit name=sbtsave value='Save' onclick=\"return savedetails(%s);\">",user);
	printf("<input type=hidden name=txtcriteria value=\"%s\">",user);
	printf("<input type=hidden name=selsearch value='username'>");	
	printf("<input type=submit name=sbtcancel value='Cancel' onclick=\"document.frmshowclient.a.value='showclient';\">");
	}
	printf("\n</form>");
	return 0;
}
int booking_alert(fax_database *db,const char *userid,const char *simno,const char *bdate, char *country){
	char message[1024];
	char subject[1024];
	char emailid[1024];
	char command[1024];
	char mailto[1024];
	sprintf(mailto,"");
	if(!strcmp(simno,"fail")){
		sprintf(message,"An error occurred while booking a card for userid:%s",userid);
		sprintf(subject,"Advance booking Failed");
		return 0;
	}
	//fax_database db(E2FAXDB,DBSERV_R);
	char *bal_due;
	//sprintf(command,"SELECT coalesce((SELECT abs((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT closingbalance as bal from billdetails where username ='%s' order by generatedon desc limit 1)X where X.bal < 0)) as numeric), 2)))),0) as bal",userid);
	//sprintf(command,"SELECT coalesce((SELECT abs((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT (SELECT sum(amount)  from payment_discount p where p.username ='%s' and sim_card_type='country' and accept_time >(select max(generatedon) from billdetails where username='%s')) - (SELECT abs((SELECT closingbalance from billdetails where username='%s' order by generatedon desc limit 1))) as bal)X where X.bal < 0)) as numeric), 2)))),0) as bal",userid,userid,userid);
	//SELECT coalesce(((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT (SELECT sum(amount)  from payment_discount p where p.username ='155556543' and sim_card_type='country' and accept_time >(select max(generatedon) from billdetails where username='155556543')) + (SELECT abs((SELECT closingbalance from billdetails where username='155556543' order by generatedon desc limit 1))) as bal)X where X.bal < 0)) as numeric), 2))),0) as bal
	sprintf(command,"SELECT coalesce(((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT (SELECT sum(amount)  from payment_discount p where p.username ='%s' and sim_card_type='country' and accept_time >(select max(generatedon) from billdetails where username='%s')) + (SELECT (SELECT closingbalance from billdetails where username='%s' order by generatedon desc limit 1)) as bal)X where X.bal < 0)) as numeric), 2))),0) as bal",userid,userid,userid);
	do_log(command);
	if(db->ExecTuplesOk(command)>0)
        {
		bal_due=db->get_field(0,"bal");
		do_log(bal_due);
		if(bal_due==NULL || bal_due[0]==0)
			bal_due="";
		else
			bal_due=strdup(bal_due);
        }
	sprintf(command,"select s.*,password from sim_user s,gkusers g where s.username=g.username and g.username='%s'",userid);
	do_log(command);
	if(db->ExecTuplesOk(command)<=0){
		do_log("Could not find the user.");
		return 0;
	}
	char *cname=db->get_field(0,"clientname");
	if(cname==NULL||cname[0]==0)
		cname="";
	else 
		cname=strdup(cname);
	char *paswd=db->get_field(0,"password");
	if(paswd==NULL||paswd[0]==0)
		paswd="";
	else
		paswd=strdup(paswd);
	char *telno=db->get_field(0,"telno");
	if(telno==NULL||telno[0]==0)
		telno="";
	else
		telno=strdup(telno);
	char *mobno=db->get_field(0,"mobno");
	if((mobno==NULL)||(mobno[0]==0))
		mobno="";
	else
		mobno=strdup(mobno);
	char *sales=db->get_field(0,"sales_person");
	if(sales==NULL||sales[0]==0)
		sales="";
	else
		sales=strdup(sales);
	char *email=db->get_field(0,"emailadd");
	if(email==NULL||email[0]==0)
		email="";
	else
		email=strdup(email);	
	do_log((char *)bdate);

	char *unlimited_calling=cgi_getentrystr("uc");
        char *uc_days=cgi_getentrystr("uc_op");
        do_log(uc_days);
        if (!(unlimited_calling == NULL || unlimited_calling[0] == 0))
        {
                if(!strcmp(unlimited_calling,"uc"))
                        strcpy(unlimited_calling,"Yes");
        }
        //do_log(unlimited_calling);
        //do_log(uc_days);
	sprintf(message,"Following is the details of sim card which was booked on %s",bdate);
	sprintf(message,"%s\nUser ID:%s",message,userid);
	sprintf(message,"%s\nClient Name:%s",message,cname);
	sprintf(message,"%s \nPassword:%s",message,paswd);
	sprintf(message,"%s \nPhone No.:%s",message,telno);
	sprintf(message,"%s \nMobile No.:%s",message,mobno);
	sprintf(message,"%s \nRegistered Phone No.:%s",message,simno);
	sprintf(message,"%s \nCountry:%s",message,country);
	sprintf(message,"%s \nEmail ID:%s",message,email);
	sprintf(message,"%s \nSales Person:%s",message,sales);
	//if(strcmp(bal_due,"0")) //if(bal_due!=NULL || bal_due!="")
	if (atof(bal_due)<0)
		sprintf(message,"%s \nPayment Overdue:%s",message,bal_due);
	if (!(unlimited_calling == NULL || unlimited_calling[0] == 0)){
                sprintf(message,"%s\nUnlimited Calling:%s",message,unlimited_calling);
                sprintf(message,"%s\nUnlimited Calling Duration:%s Days",message,uc_days);
        }
        else
                sprintf(message,"%s\nUnlimited Calling:No",message);
	sprintf(subject,"Details of sim card booked for user id:%s(%s) on %s",userid,cname,bdate);	
	char *saleeid=strtok(sales,"/");
	char semail[1024];
	char tmp_semail[1024];
	strcpy(tmp_semail,"");
	get_sim_sales_email(saleeid,tmp_semail);
	if (strlen(tmp_semail)==0)
		sprintf(semail,"%s@tsim.in",saleeid);
	else
		sprintf(semail,"%s",tmp_semail);
	do_log((char *)semail);
	strcpy(tmp_semail,"");
	int mailforsale=0;
	while(saleeid != NULL)
	{
     		saleeid = strtok(NULL, "/");
     		if((saleeid!=NULL)){
     			get_sim_sales_email(saleeid,tmp_semail);
     			if (strlen(tmp_semail)==0)
					sprintf(semail,"%s,%s@tsim.in",semail,saleeid);
				else{
					sprintf(semail,"%s,%s",semail,tmp_semail);
					strcpy(tmp_semail,"");
				}
	      	 	do_log(semail);
      		}
	}
	//mailforsale=send_dd_mail(semail,message,subject);
	mailforsale=send_mail(semail,message,subject);
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
		do_log("Could not open config file.");
		printf("Error occured while opening config file.");
	}
	else
		mailforsale=send_mail(mailto,message,subject,1);*/

   mailforsale=send_mail(semail,message,subject);
                 char* data39 = get_simmis_data_from_table("MAILTO_mailto","");
                if(data39[0]==0 || data39==NULL)
                //Mark4
		{
                     do_log("ERROR OCCURED WHILE OPENING EMAIL_TO_CUSTOMER file data39");
                     printf("ERROR OCCURED WHILE OPENING THE EMAIL_TO_CUSTOMER FILE");
                }
                strcpy(mailto,data39);
                mailforsale=send_mail(mailto,message,subject,1);
	



	do_log(mailto);
	do_log("out");
	return mailforsale;	
}
int draw_booking(fax_database *db){
	char command[1024];
	//fax_database db(E2FAXDB,DBSERV_R);
	int i;
	char *searchby=cgi_getentrystr("optsearch");
	char *userid=cgi_getentrystr("txtuser");
	char *email=cgi_getentrystr("txtemail");
	char *country=cgi_getentrystr("selcountry");
	char *chkadv=cgi_getentrystr("chkadv");
	if(chkadv==NULL)
                        chkadv=strdup("off");	
	if(!strcmp(searchby,"optuser")){
		sprintf(command,"select * from sim_user where username='%s'",userid);
	}else{
		sprintf(command,"select * from sim_user where emailadd like '%s'",email);
	}
	do_log(command);
	int count=db->ExecTuplesOk(command);
	draw_header(1);
	printf("<div id=mh style=""width:99%"">");
	printf("<form name=frmuserdetails method=post action=simmisn.e>");
	printf("\n<input type=hidden name=a>");
	printf("\n<input type=hidden name=advance value='%s'>",chkadv);
	printf("\n<input type=hidden name=b value='%s'>",country);
	char *username;
	if(count>0){
		username=db->get_field(0,"username");
		if(username==NULL||username[0]==0)
			username=" ";
		else
			username=strdup(username);
	}
	else
		username=" ";
	char *emailadd;
	if(count>0)
		emailadd=db->get_field(0,"emailadd");
	else
		emailadd=email;
	if(emailadd==NULL||emailadd[0]==0)
		emailadd=" ";
	else
		emailadd=strdup(emailadd);
	printf("\n<input type=hidden name=c value='%s'>",username);
	printf("\n<input type=hidden name=d value='%s'>",emailadd);
	printf("\n<input type=hidden name=e>");
	printf("\n<input type=hidden name=f>");
	printf("\n<input type=hidden name=g>");
	if(count>0){
		printf("<center><b><u>Existing client</u></b></center>");
		char *categ=NULL;
		categ = db->get_field(0,"category");
		if (!(categ==NULL || categ[0]==0)) categ = strdupa(categ);
		else categ = strdupa("D");
		//get_clientinfo(db->get_field(0,"salutation"),db->get_field(0,"clientname"),db->get_field(0,"companyname"),db->get_field(0,"add_building"),db->get_field(0,"add_street"),db->get_field(0,"add_city"),db->get_field(0,"add_pinno"),db->get_field(0,"add_state"),db->get_field(0,"add_country"),db->get_field(0,"deliver_add_building"),db->get_field(0,"deliver_add_street"),db->get_field(0,"deliver_add_city"),db->get_field(0,"deliver_add_pinno"),db->get_field(0,"deliver_add_state"),db->get_field(0,"deliver_add_country"),db->get_field(0,"telno"),db->get_field(0,"mobno"),db->get_field(0,"emailadd"),db->get_field(0,"creditcard_no"),db->get_field(0,"cc_holder_name"),db->get_field(0,"expdate"),db->get_field(0,"sales_person"),db->get_field(0,"passport_no"),db->get_field(0,"passport_city"),db->get_field(0,"domainname"),categ);
		get_clientinfo(db->get_field(0,"salutation"),db->get_field(0,"clientname"),db->get_field(0,"companyname"),db->get_field(0,"add_building"),db->get_field(0,"add_street"),db->get_field(0,"add_city"),db->get_field(0,"add_pinno"),db->get_field(0,"add_state"),db->get_field(0,"add_country"),db->get_field(0,"deliver_add_building"),db->get_field(0,"deliver_add_street"),db->get_field(0,"deliver_add_city"),db->get_field(0,"deliver_add_pinno"),db->get_field(0,"deliver_add_state"),db->get_field(0,"deliver_add_country"),db->get_field(0,"telno"),db->get_field(0,"mobno"),db->get_field(0,"emailadd"),db->get_field(0,"creditcard_no"),db->get_field(0,"cc_holder_name"),db->get_field(0,"expdate"),db->get_field(0,"sales_person"),db->get_field(0,"passport_no"),db->get_field(0,"passport_city"),db->get_field(0,"domainname"),categ,db->get_field(0,"reseller_sp"));
		//printf("<br><input type=submit name=subcntd value=Continue onclick=\"document.frmuserdetails.a.value='newtrip';\"></a>");
		//printf("<br><input type=submit name=subcntd value=Continue onclick=\"return book_for_existing();\"></a>");
		printf("<br><input type=submit name=subcntd value=Continue onclick=\"return book_for_existing();\">");
	}
	else{	
		if(!strcmp(searchby,"optuser")){
			printf("\n<br><br><b><center>User ID cannot be found</center></b>");
			printf("\n<center><input type=button name=btnback value=\"Back\" onclick=\"moveback();\"></center>");
			return 0;
		}	
		printf("<center><b><u>New Client</u></b></center>");
		printf("\n<br><label>Contact Person Name:</label>");
		printf("\n<select name=selsal>");
		printf("<option value=none>Select</option>");
		printf("<option value=Dr.>Dr.</option>");
		printf("<option value=Ms.>Ms.</option>");		
		printf("<option value=Mrs.>Mrs.</option>");
		printf("<option value=Mr.>Mr.</option>");
		printf("</select>");
		printf("\n<input type=text name=txtname onblur=\"return validname(this.value);\">\n");
		printf("\n<br><label>Company Name:</label><input type=text name=txtcompany>(optional)");
		printf("\n<br><br><b><u>Billing Address:</u></b><br>");
		printf("\n<br><label>Building Name:</label><input type=text name=txtbldg onblur=\"return validaddress(this.value);\" >");
		printf("\n<br><label>Street:</label><input type=text name=txtstreet onblur=\"return validaddress(this.value);\" >");
		printf("\n<br><label>Pin Code:</label><input type=text name=txtpin onblur=\"return validpinno(this.value);\">");
		printf("\n<br><label>Country:</label><select name=selcountry onchange=\"new net.ContentLoader('simmisn.e?a=fillstate&c=dcity&b='+this.value+'&d=selstate',parseMyData,'dstate',null,'Get');\">");	
		printf("<option value=none>Select</option>");
		sprintf(command,"select distinct country from country_state_city");
		do_log(command);
		int rec_cnt = db->ExecTuplesOk(command);
		for(i=0;i<rec_cnt;i++)
		{
			printf("<option value=%s>%s</option>",db->get_field(i,"country"),db->get_field(i,"country"));
		}
		printf("</select>");
		printf("\n<div id=\"dstate\">");
		printf("\n<label>State:</label>");
		printf("<select name=selstate>");
		printf("<option value=none>Select</option>");
		printf("</select>");
		printf("\n</div>");
		printf("\n<div id=\"dcity\">");
		printf("\n<label>City:</label>");
		printf("<select name=selcity>");
		printf("<option value=none>Select</option>");
		printf("</select>");
		printf("\n</div>");
		printf("\n<input type=checkbox name=chkaddr onclick=\"toggledeladdr('frmuserdetails');\" onchange=\"toggledeladdr(this.value);\">Click here if delivery address is same as billing address.");
		printf("\n<div id=daddress>");
		printf("<br><br><b><u>Delivery address:</u></b><br>");
		printf("\n<br><label>Building Name:</label><input type=text name=txtdbldg onblur=\"return validaddress(this.value);\">");
		printf("\n<br><label>Street:</label><input type=text name=txtdstreet onblur=\"return validaddress(this.value);\">");
		printf("\n<br><label>Pin Code:</label><input type=text name=txtdpin onblur=\"return validpinno(this.value);\">");
		printf("\n<br><label>Country:</label><select name=dselcountry onchange=\"new net.ContentLoader('simmisn.e?a=fillstate&c=ddcity&b='+this.value+'&d=seldstate',parseMyData,'ddstate',null,'Get');\">");
		printf("<option value=none>Select</option>");
		sprintf(command,"select distinct country from country_state_city");
		do_log(command);
		rec_cnt = db->ExecTuplesOk(command);
		for(i=0;i<rec_cnt;i++)
		{
			printf("<option value=%s>%s</option>",db->get_field(i,"country"),db->get_field(i,"country"));
		}
		printf("</select>");
		printf("\n<div id=\"ddstate\">");
		printf("\n<label>State:</label><select name=seldstate onchange=\"new net.ContentLoader('simmisn.e?a=fillcity&b='+this.value,parseMyData,'ddcity',null,'Get');\">");
		printf("<option value=none>Select</option>");
		printf("\n</select>");
		printf("\n</div>");
		printf("\n<div id=\"ddcity\">");
		printf("\n<label>City:</label><select name=seldcity onchange=\"if (this.value != 'none') new net.ContentLoader('simmisn.e?a=fillisdcode&b='+this.value,parseMyData,'ddphonenums',null,'Get');else {document.frmuserdetails.stdcode.value='';}\">");
		printf("<option value=none>Select</option>");
		printf("\n</select>");
		printf("\n</div>");
		printf("\n</div>");
		printf("\n<br><b><u>Contact details:</u></b><br>");
		printf("\n<div id=\"ddphonenums\">");
		printf("\n<br><label>Phone no:</label><input type=text name=stdcode size=5 readonly>&nbsp;&nbsp;<input type=text name=txtphone size=15 onblur=\"return validphone(this.value,'Phone number',document.frmuserdetails.isdcode.value+document.frmuserdetails.stdcode.value);\" >");
		printf("\n<br><label>Mobile no:</label><input type=text name=isdcode size=5 readonly>&nbsp;&nbsp;<input type=text name=txtmobno size=15 onblur=\"return validphone(this.value,'Mobile number',document.frmuserdetails.isdcode.value);\">");
		printf("\n</div>");

		printf("\n<br><b><u>Email Address:</u></b><br>");
		printf("\n<br><label>Primary Email:</label><input type=text name=primary_email value=\"%s\" onblur=\"return valid_emailid(this.value);\">",emailadd);
		printf("\n<br><input type=checkbox name=chkemail onclick=\"togglecommemail('frmuserdetails');\" onchange=\"togglecommemail(this.value);\">Click here if Communication email is same as Primary email.");
                printf("\n<div id=\"dcommemail\">");
                printf("\n<br><label>Communication Email:</label><input type=text name=comm_email onblur=\"return valid_emailid(this.value);\">");
                printf("\n</div>");
		printf("<br><br>");
		draw_tripdetails("frmuserdetails",country);
		printf("<br><b><u>Sales Person Details:</u></b><br>");
		printf("\n<br><label>Sales Person Name:</label>\n");
		printf("\n<select name=selsalep id=\"selsalp\" multiple size=4>");
		printf("<option value=none selected>Select</option>");
		//sprintf(command,"select sp_name from salesperson where domainname='sim.trikon.in' order by sp_name");
		sprintf(command,"select sp_name from salesperson where domainname='sim.trikon.in' and sp_type != 'Reseller' order by sp_name");
		do_log(command);
		int count=db->ExecTuplesOk(command);
		for(i=0;i<count;i++){
			printf("<option value='%s'>%s</option>",db->get_field(i,"sp_name"),db->get_field(i,"sp_name"));
		}
		printf("\n</select>");
		printf("\n<br><label>Sales by Reseller:</label>\n");
		printf("\n<input type=checkbox name=chk_reseller value=\"yes\" onclick=\"toggle_chk_reseller('frmuserdetails');\">");
		printf("\n<select name=selreseller id=\"selreseller\" style=\"visibility:hidden\" >");
		printf("<option value=none selected>Select</option>");
		sprintf(command,"select sp_name from salesperson where domainname='sim.trikon.in' and sp_type = 'Reseller' order by sp_name");
		do_log(command);
		count = db->ExecTuplesOk(command);
		for(i=0;i<count;i++)
		{
			printf("<option value=\"%s\">%s</option>",db->get_field(i,"sp_name"),db->get_field(i,"sp_name"));
		}
		printf("\n<select>");
		printf("\n<br><br><b><u>Passport Details:</u></b><br>\n");	
		printf("\n<br><label>Passport Number:</label><input type=text name=pno onblur=\"this.value=removeSpaces(this.value);return chk_pp(this.value);\" >\n");
		printf("\n<br><label>City of Issue:</label><input type=text name=ppcity ><br>\n");
	//	printf("\n<br><input type=submit name=sbtdone id=\"sbtdone\" value=Done disabled=true onclick=\"return booktrip('%s','%s','%s','frmuserdetails');\">",country,username,emailadd);
		printf("\n<br><input type=submit name=sbtdone id=\"sbtdone\" value=Done disabled=true onclick=\"set_final_fdate();return booktrip('%s','%s','%s','frmuserdetails');\">",country,username,emailadd);
	}
	printf("\n</form>");
	printf("\n</div>");
	draw_end();
}
int mail_ddno(fax_database *db){
	char command[1024];
	//fax_database db(E2FAXDB,DBSERV_R);
	int i;
	char *user=cgi_getentrystr("b");
	user=strdup(user);
	char *sendtosale=cgi_getentrystr("chkmail");
	char message[9999];
	char *emailid,*salesid;
	char *subemail=cgi_getentrystr("txtemail");
	if(sendtosale==NULL)
		sendtosale=strdup("off");
	draw_header(2);
	printf("\n<pre>");
	sprintf(command,"select * from callerid_user where username ='%s' and regd_number in ("
				"select sim_phone_no as regd_number from sim_stock where country in ("
					"select countryname from countrydata where is_dd_enabled=1"
					")"
				") and regd_number not like '91%%'",user);
	do_log(command);
	char cid_list[1024];
	string cidList;
	int cid_count = db->ExecTuplesOk(command);
	if (cid_count>0){
		cidList= "( ";
		for (int i=0;i < cid_count; i++)
		{
			cidList += ((i==0?"":", ") + string(db->get_field(i, "regd_number")));
			/*
				if (i == 0){	sprintf(cid_list,"(%s",db->get_field(i,"regd_number")); //)	}
				else{	strcat(cid_list,",");	strcat(cid_list,db->get_field(i,"regd_number"));	}
				strcat(cid_list,")");
			*/
		}
		cidList += " )";
		strcpy(cid_list, to_c_string(cidList));
	}
	else{
		do_log("\t_Query returned no tuples.");
		strcpy(cid_list,"-");
		do_log("cid_count zero");
	}
	sprintf(command,"select * from callerid_user where username ='%s' and regd_number like '91%%'",user);
	do_log(command);
	int count=db->ExecTuplesOk(command);
	char mobile_number[251];
	if (count>0) strcpy(mobile_number,db->get_field(0,"regd_number"));
	else strcpy(mobile_number,"Not present");


	sprintf(command,"select * from cb_addressbook where username='%s' order by acc_num",user);
	do_log(command);
	count=db->ExecTuplesOk(command);
	if (count>0){
        	FILE *fp_email_msg=fopen(EMAIL_TO_CUSTOMER,"r");
        	char mailto[1024];
        	char line[1024];
        	if (fp_email_msg){
			if (cid_count==0) sprintf(message,"Hello,\n");
			else if ( cid_count==1)  sprintf(message,"Your TSIM Phone No is %s. ",cid_list);
			else sprintf(message,"Your TSIM Phone Nos are %s. ",cid_list);
                	while (fgets(command,1023,fp_email_msg)){
                        	if (strstr(command,"XXXX")){
					char *tmp = strstr(command,"XXXX");
					*tmp = '\0';
                                	strcpy(line,command);
                                	//line[strlen(line)-1]='\0';
                                	strcat(message,line);
                                	sprintf(line," +%s\n",mobile_number);
                                	strcat(message,line);
                        	}
                        	else if (strstr(command,"The list of substitute numbers is as follows")){
					strcat(message,command);
					sprintf(command,"\nNAME\tSUBSTITUTE NO.\tDESTINATION NO.\n");
					strcat(message,command);
					for(i=0;i<count;i++){
						sprintf(command,"%s\t+%s\t%s\n",db->get_field(i,"name"),db->get_field(i,"acc_num"),db->get_field(i,"destination"));
						strcat(message,command);
					}
                        	}
                        	else strcat(message,command);
                	}
                	fclose(fp_email_msg);
                	do_log(message);
			/*
                	if (!send_mail(emailid,message,"TSIM's Deep Discount Service")){
                        	do_log("failed to send");
                	}
                	else    do_log("sent");
			*/
        	}



do_log("user");
do_log(user);
		sprintf(command,"select * from sim_user where username='%s';",user);
		do_log(command);
		if(db->ExecTuplesOk(command)>0){do_log("mail");
			emailid=db->get_field(0,"emailadd");
			salesid=db->get_field(0,"sales_person");
			do_log(salesid);
			char subject[1024];
			sprintf(subject,"TSIM's Deep Discount Service");
			int specific=0,result=0,mailforsale=0;
			if(subemail==NULL||subemail[0]==0)
				specific=1;
			else
			{
				//specific=send_dd_mail(subemail,(char *)message,(char *)subject);
				specific=send_mail(subemail,message,"TSIM's Deep Discount Service",1);
			}

			if(emailid==NULL||emailid[0]==0)
				result=1;
			else
				//result=send_dd_mail(emailid,(char *)message,(char *)subject);
				result=send_mail(emailid,message,"TSIM's Deep Discount Service",1);
			if((!strcmp(sendtosale,"on"))&&(result>0)){
				if((salesid!=NULL)&&(salesid[0]!=0)){
					char *saleeid=strtok(salesid,"/");
					char semail[1024];
					char tmp_email[1024];
					strcpy(tmp_email,"");
					get_sim_sales_email(saleeid,tmp_email);
					if (strlen(tmp_email)==0)
						sprintf(semail,"%s@tsim.in",saleeid);
					else{
						sprintf(semail,"%s",tmp_email);
						strcpy(tmp_email,"");
					}
					do_log(semail);
					while(saleeid != NULL)
					{
						saleeid = strtok(NULL, "/");
						if((saleeid!=NULL)){
							get_sim_sales_email(saleeid,tmp_email);
							if (strlen(tmp_email)==0)
								sprintf(semail,"%s,%s@tsim.in",semail,saleeid);
							else{
								sprintf(semail,"%s,%s",semail,tmp_email);
								strcpy(tmp_email,"");
							}
							do_log(semail);
						}
					}
					do_log(semail);
					//mailforsale=send_dd_mail((char *)semail,(char *)message,(char *)subject);
					mailforsale=send_mail(semail,message,"TSIM's Deep Discount Service");
				}
			}
			else
				mailforsale=1;
			if((result>0)&&(mailforsale>0)&&(specific>0)){
				printf("\n<center><b>Mail sent successfully.</b></center>");
				do_log("Mail sent successfully");
			}
			else if(result==0)
				printf("\n<center><b>An error occured while sending mail to the user.</b></center>");
			else if (mailforsale==0){
				printf("\n<center><b>An error occured while sending mail to the respective salesperson.</b></center>");
				do_log("Mail sent successfully");
			}
			else if(specific==0)
				printf("\n<center><b>An error occured while sending mail to the specified email Id.</b></center>");
			else{
				printf("\n<center><b>Mail sending failed.</b></center>");
				do_log("Mail sending failed");
			}
		}
		else
			printf("\n<center><b>Email Id not available.</b></center>");
		printf("\n<center><input type=button name=btnback value=HOME onclick=\"refpage('http://mail.tsim.in/simmisn.e');\"></center>");
	}
	else{
		printf("\n<center><b>There are no deep discount numbers set for %s.</b></center>",user);
		printf("\n<center><input type=button name=btnback value=BACK onclick=\"refpage('http://mail.tsim.in/simmisn.e?a=setupdd&b=%s');\"></center>",user);
   	}
	printf("\n</pre>");
	draw_end();
	return 0;
}
int show_stock(fax_database *db){
	char command[1024];
	//fax_database db(E2FAXDB,DBSERV_R);
	char *sim=cgi_getentrystr("txtcriteria");
	char *search=cgi_getentrystr("selsearch");
	draw_header(5);
	printf("\n<div id=mh style=""width:99%"">");
        printf("\n<br>");
	if(!strcmp(search,"simno"))
		sprintf(command,"select * from sim_stock where simno='%s'",sim);
	else if(!strcmp(search,"sim_phone_no"))
		sprintf(command,"select * from sim_stock where sim_phone_no='%s'",sim);
	else if(!strcmp(search,"srno"))
		sprintf(command,"select * from sim_stock where srno=upper('%s')",sim);
	do_log(command);
	int count=db->ExecTuplesOk(command);
	printf("\n<div id=\"deditstock\" style=\"float:left\" width=99%>");
	printf("\n<form name=frmshowstock action=simmisn.e method=post>");
	printf("\n<input type=hidden name=a id=\"a\" >");
        printf("\n<input type=hidden name=b value=%s>",db->get_field(0,"simno"));
	if(count>0)
	{
		sim=db->get_field(0,"simno");
		printf("\n<br><label>Sim Number:</label><input type=text  name=txtsimno  value='%s' readonly>",db->get_field(0,"simno"));
	 	//printf("<br> %s",db->get_field(0,"simno"));
		printf("\n<br><label>Sim Phone Number:</label><input type=text  name=txtsimphoneno  value='%s' readonly>",db->get_field(0,"sim_phone_no"));
		//printf("<br> %s",db->get_field(0,"sim_phone_no"));
		printf("\n<br><label>SNumber:</label><input type=text  name=txtsrno  value='%s' readonly>",db->get_field(0,"srno"));
		//printf("<br> %s",db->get_field(0,"srno"));
		if(!strcmp(db->get_field(0,"availability"),"t"))
			printf("\n<br><label>Availability:</label><input type=text  name=txtavail  value=\"Available\" readonly>");
		else
			printf("\n<br><label>Availability:</label><input type=text  name=txtavail  value=\"Not Available\" readonly>");
                //printf("<br> %s",db->get_field(0,"availability"));
		printf("\n<br><label>Country:</label><input type=text  name=txtcountry  value='%s' readonly>",db->get_field(0,"country"));
                //printf("<br> %s",db->get_field(0,"country"));
		printf("\n<br>");
		//if(isAdmin)
			//printf("\n<input type=button name=btnedit value=Edit onclick=\"call_editstock(%s);\">",sim);
		printf("\n<input type=button name=btnback value=Back onclick=\"moveback();\">");
	}
	else
	{
                printf("\n<center><b>Sim not found.</b></center>");
                printf("\n<center><input type=button name=btnback value=Back onclick=\"moveback();\"></center>");
        }
        printf("\n</form>");
        printf("\n</div>");
	draw_end();
	return 0;
}
int show_client(fax_database *db){
	char command[1024],query[1024];
	//fax_database db(E2FAXDB,DBSERV_R);
	int i;
	char *status;
	fax_database del(E2FAXDB,DBSERV_R);
	char *user=cgi_getentrystr("txtcriteria");
	char *search=cgi_getentrystr("selsearch");
	draw_header(2);
	printf("\n<div id=mh style=""width:99%"">");
	printf("\n<br>");
	do_log(user);
	do_log(search);	
	if(!strcmp(search,"username"))
		sprintf(command,"select * from sim_user where username='%s'",user);
	else if(!strcmp(search,"emailadd"))
		sprintf(command,"select * from sim_user where emailadd like '%s'",user);
	else if(!strcmp(search,"clientname"))
		sprintf(command,"select * from sim_user where lower(clientname) like lower('%%%s%%')",user);
	else if(!strcmp(search,"companyname"))
		sprintf(command,"select * from sim_user where lower(companyname) like lower('%%%s%%')",user);
	else if(!strcmp(search,"sim_phone_no"))
		sprintf(command,"select s.*,status from sim_user s,clienttrip c where s.username=c.username and sim_phone_no='%s' order by booking_date desc",user);
	else if(!strcmp(search,"mobno"))
		sprintf(command,"select * from sim_user where mobno ='%s'",user);
	do_log(command);
	int count=db->ExecTuplesOk(command);
	printf("\n<div id=\"dedituser\" style=\"float:left\" width=99%>");
	printf("\n<form name=frmshowclient action=simmisn.e method=post>");
	printf("\n<input type=hidden name=a id=\"a\" >");
	printf("\n<input type=hidden name=b value=%s>",db->get_field(0,"username"));
	if(count==1){
		user=db->get_field(0,"username");
		char *categ=NULL;
		categ = db->get_field(0,"category");
		if (!(categ==NULL || categ[0]==0)) categ = strdupa(categ);
		else categ = strdupa("D");
		sprintf(command,"company -%s-",db->get_field(0,"companyname"));
		do_log(command);
		//get_clientinfo(db->get_field(0,"salutation"),db->get_field(0,"clientname"),db->get_field(0,"companyname"),db->get_field(0,"add_building"),db->get_field(0,"add_street"),db->get_field(0,"add_city"),db->get_field(0,"add_pinno"),db->get_field(0,"add_state"),db->get_field(0,"add_country"),db->get_field(0,"deliver_add_building"),db->get_field(0,"deliver_add_street"),db->get_field(0,"deliver_add_city"),db->get_field(0,"deliver_add_pinno"),db->get_field(0,"deliver_add_state"),db->get_field(0,"deliver_add_country"),db->get_field(0,"telno"),db->get_field(0,"mobno"),db->get_field(0,"emailadd"),db->get_field(0,"creditcard_no"),db->get_field(0,"cc_holder_name"),db->get_field(0,"expdate"),db->get_field(0,"sales_person"),db->get_field(0,"passport_no"),db->get_field(0,"passport_city"),db->get_field(0,"domainname"),categ);
		get_clientinfo(db->get_field(0,"salutation"),db->get_field(0,"clientname"),db->get_field(0,"companyname"),db->get_field(0,"add_building"),db->get_field(0,"add_street"),db->get_field(0,"add_city"),db->get_field(0,"add_pinno"),db->get_field(0,"add_state"),db->get_field(0,"add_country"),db->get_field(0,"deliver_add_building"),db->get_field(0,"deliver_add_street"),db->get_field(0,"deliver_add_city"),db->get_field(0,"deliver_add_pinno"),db->get_field(0,"deliver_add_state"),db->get_field(0,"deliver_add_country"),db->get_field(0,"telno"),db->get_field(0,"mobno"),db->get_field(0,"emailadd"),db->get_field(0,"creditcard_no"),db->get_field(0,"cc_holder_name"),db->get_field(0,"expdate"),db->get_field(0,"sales_person"),db->get_field(0,"passport_no"),db->get_field(0,"passport_city"),db->get_field(0,"domainname"),categ,db->get_field(0,"reseller_sp"));
	if (isAdmin){
		printf("\n<br><br><div></div><input type=button name=sbtedit value=\"Edit User Details\" onclick=\"call_edituser('%s');\">",user);
		printf("\n<input type=submit name=sbtallot value=\"Allot Accessories\" onclick=\"return show_allot();\">");
		printf("\n<input type=submit name=sbtdetail value=\"Set up DD No.\" onclick=\"return showsetup('frmshowclient',%s);\">",user);
	}
	}
	else if(count>1){
		printf("\n<br><table align=center width=100%>");
		printf("\n<b><tr class=mh><th>Username</th><th>Domainname</th><th>Clientname</th><th>Company Name</th><th>Emailaddress</th><th>Sim Card Status</th></tr></b>");
		printf("\n<tbody>");
		for(i=0;i<count;i++){
		if(!strcmp(search,"sim_phone_no")){
			status=db->get_field(i,"status");
		}
		else{
		sprintf(query,"select status from clienttrip where username='%s' order by booking_date desc",db->get_field(i,"username"));
		do_log(query);
		if(del.ExecTuplesOk(query)>0)
			status=del.get_field(0,"status");
		}
		printf("\n<tr class=Ml_nob><td align=center><a href=""simmisn.e?a=showclient&txtcriteria=%s&selsearch=username"">%s</a></td><td align=center>%s</td><td align=center>%s</td><td align=center>%s</td><td align=center>%s</td><td align=center>%s</td></tr>",db->get_field(i,"username"),db->get_field(i,"username"),db->get_field(i,"domainname"),db->get_field(i,"clientname"),db->get_field(i,"companyname"),db->get_field(i,"emailadd"),status);
		}
		printf("\n</tbody>");
		printf("\n</table>");
	}
	else{
		printf("\n<center><b>User not found.</b></center>");
		printf("\n<center><input type=button name=btnback value=\"Back\" onclick=\"moveback();\"></center>");
	}
	 printf("\n</form>");
         printf("\n</div>");
	printf("\n<form name=right_form>");
	 if(count==1){
		std::string user(db->get_field(0,"username"));
		if(user.length()<12) //if((user.length()<12) && !strcmp(db->get_field(0,"domainname"),"tsim.in"))	
	 	{
			printf("\n<div id=\"dtrip\" style=\"float:right;margin:10px\">");
                	printf("\n<label>Show Trip Details:</label>");
                	//printf("\n &nbsp &nbsp <select name=seltripids id=\"seltripids\" onchange=\" return calltrip(this.value);\">");
                	printf("\n &nbsp &nbsp <select name=seltripids id=\"seltripids\" onchange=\" return calltrip(encodeURIComponent(this.value));\">");
                	printf("<option value='none'>Select Trip Id</option>");
                	sprintf(command,"select tripid from clienttrip where username='%s' order by booking_date desc",db->get_field(0,"username"));
                	do_log(command);
                	int reccnt=db->ExecTuplesOk(command);
                	for(i=0;i<reccnt;i++)
                	       printf("<option value=\"%s\">%s</option>",db->get_field(i,"tripid"),db->get_field(i,"tripid"));
                	printf("</select>");
                	printf("\n<div id=\"dshowdet\">");
                	printf("</div>");
			printf("\n</div>");
		}
		else
		{
			printf("\n<div id=\"dtrip\" style=\"float:right;margin:10px\">");
			printf("\n<label>Show INUM Details:</label><br>");
			sprintf(command,"SELECT u.username,u.domainname,number,credit,usage,currency,pstn_number from incoming_number_suscribers i,users u,gkusers gk where i.username=u.username and u.username=gk.username and i.username='%s' and i.domainname='tsim.in'",user.c_str());
			do_log(command);
			printf("<br><br><b>Username : </b>%s",db->get_field(0,"username"));
			int inum_count=db->ExecTuplesOk(command);
			if(inum_count>0)
			{
				sprintf(command,"SELECT  amount,accept_time::date from e_credit where username ='%s' and domainname='%s'",db->get_field(0,"username"),db->get_field(0,"domainname"));
				do_log(command);
				if(del.ExecTuplesOk(command)>0)
					printf("<br><br><b>Balance : </b><span TITLE=\"Emergency credit of %s given on %s\"><font color=\"red\">%.2f</font></span>",del.get_field(0,"amount"),del.get_field(0,"accept_time"),atof(db->get_field(0,"credit"))-atof(db->get_field(0,"usage")));
				else
					printf("<br><br><b>Balance : </b>%.2f",atof(db->get_field(0,"credit"))-atof(db->get_field(0,"usage")));
			for(i=0;i<inum_count;i++)
			{
				printf("<br><br><b><u>Number %d : </u></b>",i+1);
				printf("<br><br><b>DID Number : </b>%s",db->get_field(i,"number"));
				printf("<br><br><b>India Number : </b>%s",db->get_field(i,"pstn_number"));
				printf("<br>");
			}
			}
			printf("\n</div>");
		}
	}
	 printf("\n</form>");
         //printf("\n</div>");
	printf("\n</div>");
	draw_end();
	return 0;
}
int draw_summary(fax_database *db, bool download_mode){
	char command[1024];
        //fax_database db(E2FAXDB,DBSERV_R);
	fax_database db1(E2FAXDB,DBSERV_R);
        int i;
	int no_booking_this_yr; 							//Added by Neekhil Dedhia
	char *fromdt=cgi_getentrystr("b");
		char ddmmyyyy[30];char yyyymmdd[30];
	//	strcpy(ddmmyyyy,fromdt);
	//	strcpy(fromdt,change_date_format(fromdt,yyyymmdd));
	char *todt=cgi_getentrystr("c");
	//	strcpy(ddmmyyyy,todt);
	//	strcpy(todt,change_date_format(todt,yyyymmdd));
	char *stats=cgi_getentrystr("stats");
	if (stats == NULL|| stats[0]==0) stats=strdupa("no");
	else stats = strdupa(stats);
sprintf(command,"stats-%s-",stats);
do_log(command);
	string status_filter = cgi_getentrystr("status_filter");
	status_filter = (status_filter == "all_but_canceled" || string(stats) == "yes" ? "not like 'Canceled'" : "like '" + status_filter + "'");
	char fname[1024];
	sprintf(fname,"sim_detail%s%s.csv",replacestring(fromdt,"-",""),replacestring(todt,"-",""));
	do_log(fname);

	//sprintf(command,"select c.username,clientname,companyname,sales_person,status,sim_phone_no,booking_date,from_date,to_date from clienttrip c,sim_user s where c.username=s.username and status not like 'Canceled' and booking_date>='%s' and booking_date<'%s' order by booking_date",fromdt,todt);
	//sprintf(command,"select c.username,clientname,companyname,sales_person,status,sim_phone_no,booking_date,from_date,to_date,creditcard_no,expdate,REPLACE(TRANSLATE(tripid,REPLACE(TRANSLATE(tripid, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ ',RPAD('#',LENGTH(tripid),'#')),'#',''),RPAD('#',LENGTH(tripid),'#')),'#','') AS country from clienttrip c,sim_user s where c.username=s.username and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') order by booking_date",fromdt,todt);
	//sprintf(command,"select c.username,clientname,companyname,sales_person,reseller_sp,case_source,mis_user,status,c.sim_phone_no,booking_date,from_date,to_date,payment_mode,creditcard_no,expdate,country,c.handset_model,s.add_city from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') order by booking_date",fromdt,todt);
	sprintf(command,"select c.username,clientname,companyname,sales_person,reseller_sp,case_source,mis_user,status,c.sim_phone_no,booking_date,from_date,to_date,payment_mode,creditcard_no,expdate,country,c.handset_model,s.add_city,to_char(booking_date,'dd-mm-yyyy') as booking_date_fmt,to_char(from_date,'dd-mm-yyyy') as from_date_fmt,to_char(to_date,'dd-mm-yyyy') as to_date_fmt from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and c.status %s and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') order by booking_date", to_c_string(status_filter), fromdt, todt);
	do_log(command);
	int count=db->ExecTuplesOk(command);
	if (count>0){
		if (download_mode){
			printf("Content-disposition: attachment; filename=%s\n\n", fname);
			printf("Sl. No.,User Id,Client Name,Company Name,Sales Person / Reseller,Reseller Sales Person,Case Source,MIS User,Sim Card Status,Sim Phone No.,Booking Date,Trip Start Date,Trip End Date,Payment Mode,Creditcard No.,Creditcard ExpDate,Country,Handset,City,Category\n");
		}
		char tmp_clname[200];
		char tmp_cmpname[150];
		if (!download_mode){
			printf("\n<br><b><u>Trip Summary</u><b><br>");
			printf("\n<br>");
			printf("\n<table width=100%% border=0>");
			printf("\n<tr class=Mh><th>Sl. No.</th><th>User Id</th><th>Client Name</th><th>Company Name</th><th>Sales Person / Reseller</th><th>Reseller Sales Person</th><th>Case Source</th><th>MIS User</th><th>Sim Card Status</th><th>Sim Phone No.</th><th>Booking Date</th><th>Trip Start Date</th><th>Trip End Date</th><th>Payment Mode</th><th>Creditcard No.</th><th>Creditcard ExpDate</th><th>Country</th><th>Handset</th><th>City</th><th>Category</th></tr>");
		}
		for(i=0;i<count;i++){
			strcpy(tmp_clname,db->get_field(i,"clientname"));
			strcpy(tmp_cmpname,db->get_field(i,"companyname"));
		if (!download_mode){
			printf("\n<tr class=Ml>");
			printf("<td>%d</td>",i+1);
		}
		else printf("%d,",i+1);
			char *c=db->get_field(i,"username");
			if((c==NULL)||c[0]==0)
				if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
			else{ 
				if (!download_mode) printf("<td>%s</td>",db->get_field(i,"username")); else printf("%s,",db->get_field(i,"username"));
			}
			c=db->get_field(i,"clientname");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                        else{
                                if (!download_mode) printf("<td>%s</td>",db->get_field(i,"clientname")); else printf("%s,",db->get_field(i,"clientname"));
			}
			c=db->get_field(i,"companyname");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                        else{
                                if (!download_mode) printf("<td>%s</td>",db->get_field(i,"companyname")); else printf("%s,",db->get_field(i,"companyname"));
			}
			c=db->get_field(i,"sales_person");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                        else{
                                if (!download_mode) printf("<td>%s</td>",db->get_field(i,"sales_person")); else printf("%s,",db->get_field(i,"sales_person"));
			}
			c=db->get_field(i,"reseller_sp");
			if((c==NULL)|| c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
			else{
				if (!download_mode) printf("<td>%s</td>",db->get_field(i,"reseller_sp")); else printf("%s,",db->get_field(i,"reseller_sp"));
			}
			c=db->get_field(i,"case_source");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                        else{
                                if (!download_mode) printf("<td>%s</td>",db->get_field(i,"case_source")); else printf("%s,",db->get_field(i,"case_source"));
			}
			c=db->get_field(i,"mis_user");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
			else{
				if (!download_mode) printf("<td>%s</td>",db->get_field(i,"mis_user")); else printf("%s,",db->get_field(i,"mis_user"));
			}
			c=db->get_field(i,"status");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                        else{
                                if (!download_mode) printf("<td>%s</td>",db->get_field(i,"status")); else printf("%s,",db->get_field(i,"status"));
			}
			c=db->get_field(i,"sim_phone_no");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                        else{
                                if (!download_mode) printf("<td>%s</td>",db->get_field(i,"sim_phone_no")); else printf("%s,",db->get_field(i,"sim_phone_no"));
			}
			c=db->get_field(i,"booking_date");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                        else{
				//char yyyymmdd[30];
				//char ddmmyyyy[30];
				//printf("<td>%s</td>",db->get_field(i,"booking_date"));
				strcpy(yyyymmdd,db->get_field(i,"booking_date"));
				if (!download_mode) printf("<td>%s</td>",change_date_format(yyyymmdd,ddmmyyyy)); else printf("%s,",change_date_format(yyyymmdd,ddmmyyyy));
                                //printf("<td>%s</td>",db->get_field(i,"booking_date"));
			}
			c=db->get_field(i,"from_date");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                        else{
				//char yyyymmdd[30];
                                //char ddmmyyyy[30];
                                strcpy(yyyymmdd,db->get_field(i,"from_date"));
				if (!download_mode) printf("<td>%s</td>",change_date_format(yyyymmdd,ddmmyyyy)); else printf("%s,",change_date_format(yyyymmdd,ddmmyyyy));
                                //printf("<td>%s</td>",db->get_field(i,"from_date"));
			 }
			c=db->get_field(i,"to_date");
			 if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                         else{
				//char yyyymmdd[30];
                                //char ddmmyyyy[30];
                                strcpy(yyyymmdd,db->get_field(i,"to_date"));
				if (!download_mode) printf("<td>%s</td>",change_date_format(yyyymmdd,ddmmyyyy)); else printf("%s,",change_date_format(yyyymmdd,ddmmyyyy));
                                // printf("<td>%s</td>",db->get_field(i,"to_date"));
			 }
			c=db->get_field(i,"payment_mode");
			if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
			else
			{
				if(!strcmp(db->get_field(i,"payment_mode"),"Advanced Payment"))
				{
					//sprintf(command,"select amount from payment_discount where trip_id=(select tripid from clienttrip where username ='%s' and booking_date>='%s' and booking_date<=timestamp_pl_interval('%s','86400') order by booking_date desc limit 1) and sim_card_type='country'",db->get_field(i,"username"),fromdt,todt);
					sprintf(command,"select coalesce((select sum(amount) from payment_discount where trip_id=(select tripid from clienttrip where username ='%s' and booking_date>='%s' and booking_date<=timestamp_pl_interval('%s','86400') and status != 'Canceled' order by booking_date desc limit 1) and sim_card_type='country'),0) as amount",db->get_field(i,"username"),fromdt,todt);
					//do_log(command);
					db1.ExecTuplesOk(command);
					//if(db1.ExecTuplesOk(command))
					if(strcmp(db1.get_field(0,"amount"),"0"))
						if (!download_mode) printf("<td>Advance Payment: %s $</td>",db1.get_field(0,"amount")); else printf("Advance Payment: %s $,",db1.get_field(0,"amount"));
					else
						if (!download_mode) printf("<td>Advance Payment</td>"); else printf("Advance Payment,");
				}
				else{
					if (!download_mode) printf("<td>%s</td>",db->get_field(i,"payment_mode")); else printf("%s,",db->get_field(i,"payment_mode"));
				}
			}
			c=db->get_field(i,"creditcard_no");
			 if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                         else{
				if(can_view_cc)//if (isAdmin)
                                 	if (!download_mode) printf("<td>%s</td>",db->get_field(i,"creditcard_no")); else printf("%s,",db->get_field(i,"creditcard_no"));
				else{
					char cc_num[31];
					char cc_show_str[31];
					strcpy(cc_num,db->get_field(i,"creditcard_no"));
					if (!download_mode) printf("<td>%s</td>",hide_cc_number(cc_num,cc_show_str)); else printf("%s,",hide_cc_number(cc_num,cc_show_str));
				}
			}
			c=db->get_field(i,"expdate");
			 if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                         else{
				//char yyyymmdd[30];
                                //char ddmmyyyy[30];
                                strcpy(yyyymmdd,db->get_field(i,"expdate"));
				if (!download_mode) printf("<td>%s</td>",change_date_format(yyyymmdd,ddmmyyyy)); else printf("%s,",change_date_format(yyyymmdd,ddmmyyyy));
                                //printf("<td>%s</td>",db->get_field(i,"expdate"));
			}
			c=db->get_field(i,"country");
			 if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                         else{
                                 if (!download_mode) printf("<td>%s</td>",db->get_field(i,"country")); else printf("%s,",db->get_field(i,"country"));
			 }
			c=db->get_field(i,"handset_model");
			 if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                         else{
                                 if (!download_mode) printf("<td>%s</td>",db->get_field(i,"handset_model")); else printf("%s,",db->get_field(i,"handset_model"));
			 }
			c=db->get_field(i,"add_city");
			 if((c==NULL)||c[0]==0)
                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                         else{
                                 if (!download_mode) printf("<td>%s</td>",db->get_field(i,"add_city")); else printf("%s,",db->get_field(i,"add_city"));
			 }
			if(strstr(tmp_cmpname,"Trikon") || strstr(tmp_cmpname,"TSIM") || !strcasecmp(tmp_cmpname,"tsim") || strstr(tmp_cmpname,"Tsim") ){
				if (!download_mode) printf("<td>In House</td>"); else printf("In House,");
			}
			else{
				if (!download_mode) printf("<td></td>"); else printf(",");
			}
			
			if (!download_mode) printf("</tr>"); else printf("\n");
		}
		if (!download_mode) printf("\n</table>"); else printf("\n\n\n");
		if (!strcmp(stats,"yes"))
		{
			//float in_house_sales_this_year,in_house_sales_last_year;
			if (!download_mode) printf("\n<table width=100%%>");
			
			char no_of_cs_this_yr[5];
			char no_of_cs_last_yr[5];
			do_log("printing the stats");
			//sprintf(command,"select count(*) from clienttrip c , sim_stock st,countrydata d where st.sim_phone_no=c.sim_phone_no and d.countryname=st.country and d.card_type='country_specific' and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400')",fromdt,todt);
			sprintf(command,"select count(*) from clienttrip c , sim_stock st,countrydata d where st.sim_phone_no=c.sim_phone_no and d.countryname=st.country and (d.card_type='country_specific' or d.countryname in ('Europe DirectCall Postpaid','GLOBAL POSTPAID','GLUNLIM POSTPAID')) and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400')",fromdt,todt);
			do_log(command);
			int total_cnt_this_yr,total_cnt_last_yr,cnt_without_cs_this_yr,cnt_without_cs_last_yr;
			if(db->ExecTuplesOk(command))
				total_cnt_this_yr=atoi(db->get_field(0,"count"));
			//sprintf(command,"SELECT count(*) from clienttrip c , sim_stock st,countrydata d,sim_user s where st.sim_phone_no=c.sim_phone_no and d.countryname=st.country and d.card_type='country_specific' and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname!~* 'trikon'",fromdt,todt);


			sprintf(command,"SELECT count(*) from clienttrip c , sim_stock st,countrydata d,sim_user s where st.sim_phone_no=c.sim_phone_no and d.countryname=st.country and (d.card_type='country_specific' or d.countryname in ('Europe DirectCall Postpaid','GLOBAL POSTPAID','GLUNLIM POSTPAID')) and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname!~* 'trikon|tsim'",fromdt,todt);
			do_log(command);
			if (db->ExecTuplesOk(command)){
				strcpy(no_of_cs_this_yr,db->get_field(0,"count"));
			}
			char last_yr_fromdt[16];
			char last_yr_todt[16];
			char yyyymmddfrom[30];
			char yyyymmddto[30];
			char ddmmyyyyfrom[30];
			char ddmmyyyyto[30];
			 add_years(fromdt,0,yyyymmddfrom);
			 add_years(todt,0,yyyymmddto);
			add_years(fromdt,-1,last_yr_fromdt);
			add_years(todt,-1,last_yr_todt);
			//change_date_format
			//strcpy(yyyymmddfrom,fromdt);
			//strcpy(yyyymmddto,todt);
			if (!download_mode){
				printf("<tr><td colspan=4>");
				//printf("<br><center>This Year Duration : %s To %s",fromdt,todt);
				printf("<br><center>This Year Duration : %s To %s",change_date_format(yyyymmddfrom,ddmmyyyyfrom),change_date_format(yyyymmddto,ddmmyyyyto));
				//printf("&nbsp &nbsp &nbsp Last Year Duration : %s To %s</center><br>",last_yr_fromdt,last_yr_todt);
				strcpy(yyyymmddfrom,last_yr_fromdt);
				strcpy(yyyymmddto,last_yr_todt);
				printf("&nbsp &nbsp &nbsp Last Year Duration : %s To %s</center><br>",change_date_format(yyyymmddfrom,ddmmyyyyfrom),change_date_format(yyyymmddto,ddmmyyyyto));
				printf("</td></tr>");
				printf("\n<tr><td>");
                        	printf("\n<br><b><u><font color=\"blue\" size=\"2\" >Trip Summary Stats</font></u></b><br>");
			}
			else{
				printf("This Year Duration : %s To %s",change_date_format(yyyymmddfrom,ddmmyyyyfrom),change_date_format(yyyymmddto,ddmmyyyyto));
				strcpy(yyyymmddfrom,last_yr_fromdt);
				strcpy(yyyymmddto,last_yr_todt);
				printf(", Last Year Duration : %s To %s\n\n\n",change_date_format(yyyymmddfrom,ddmmyyyyfrom),change_date_format(yyyymmddto,ddmmyyyyto));
				printf("\nTrip Summary Stats\n\n\n");
			}
			//sprintf(command,"select count(*) from clienttrip c , sim_stock st,countrydata d where st.sim_phone_no=c.sim_phone_no and d.countryname=st.country and d.card_type='country_specific' and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400')", last_yr_fromdt,last_yr_todt);
			sprintf(command,"select count(*) from clienttrip c , sim_stock st,countrydata d where st.sim_phone_no=c.sim_phone_no and d.countryname=st.country and (d.card_type='country_specific' or d.countryname in ('Europe DirectCall Postpaid','GLOBAL POSTPAID','GLUNLIM POSTPAID')) and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400')",last_yr_fromdt,last_yr_todt);
			do_log(command);
			if(db->ExecTuplesOk(command))
				total_cnt_last_yr=atoi(db->get_field(0,"count"));
			//sprintf(command,"SELECT count(*) from clienttrip c , sim_stock st,countrydata d,sim_user s where st.sim_phone_no=c.sim_phone_no and d.countryname=st.country and d.card_type='country_specific' and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname!~* 'trikon'",last_yr_fromdt,last_yr_todt);

			sprintf(command,"SELECT count(*) from clienttrip c , sim_stock st,countrydata d,sim_user s where st.sim_phone_no=c.sim_phone_no and d.countryname=st.country and (d.card_type='country_specific' or d.countryname in ('Europe DirectCall Postpaid','GLOBAL POSTPAID','GLUNLIM POSTPAID')) and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname!~* 'trikon|tsim'",last_yr_fromdt,last_yr_todt);
			do_log(command);
			if (db->ExecTuplesOk(command)){
				strcpy(no_of_cs_last_yr,db->get_field(0,"count"));
			}
			sprintf(command,"select count(*) from clienttrip c,sim_stock st,countrydata d, sim_user s where d.card_type='non_country_specific' and c.sim_phone_no=st.sim_phone_no and st.country = d.countryname and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname ~* 'trikon|tsim'",fromdt,todt);
			do_log(command);
			if(db->ExecTuplesOk(command))
				cnt_without_cs_this_yr=atoi(db->get_field(0,"count"));
			sprintf(command,"select count(*) from clienttrip c,sim_stock st,countrydata d, sim_user s where d.card_type='non_country_specific' and c.sim_phone_no=st.sim_phone_no and st.country = d.countryname and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname ~* 'trikon|tsim'",last_yr_fromdt,last_yr_todt);
			do_log(command);
			if(db->ExecTuplesOk(command))
				cnt_without_cs_last_yr=atoi(db->get_field(0,"count"));
			//sprintf(command,"SELECT countrydata.countryname,coalesce(count,0) as count from (SELECT st.country countryname,count(*) from clienttrip c, sim_stock st,countrydata d,sim_user s where d.card_type='non_country_specific' and c.sim_phone_no=st.sim_phone_no and st.country = d.countryname and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname !~* 'trikon' group by st.country) X right join countrydata on countrydata.countryname=X.countryname where countrydata.card_type='non_country_specific' order by countrydata.countryname",fromdt,todt);
			sprintf(command,"SELECT countrydata.countryname,coalesce(count,0) as count from (SELECT st.country countryname,count(*) from clienttrip c, sim_stock st,countrydata d,sim_user s where (d.card_type='non_country_specific' and d.countryname not in('Europe DirectCall Postpaid','GLOBAL POSTPAID','GLUNLIM POSTPAID')) and c.sim_phone_no=st.sim_phone_no and st.country = d.countryname and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname !~* 'trikon|tsim'group by st.country) X right join countrydata on countrydata.countryname=X.countryname where countrydata.card_type='non_country_specific' and countrydata.countryname not in('Europe DirectCall Postpaid','GLOBAL POSTPAID','GLUNLIM POSTPAID') order by countrydata.countryname",fromdt,todt);
			do_log(command);
			int rec = db->ExecTuplesOk(command);
			fax_database db1(E2FAXDB,DBSERV_R);
			//sprintf(command,"SELECT countrydata.countryname,coalesce(count,0) as count from (SELECT st.country countryname,count(*) from clienttrip c, sim_stock st,countrydata d, sim_user s where d.card_type='non_country_specific' and c.sim_phone_no=st.sim_phone_no and st.country = d.countryname and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname !~* 'trikon' group by st.country) X right join countrydata on countrydata.countryname=X.countryname where countrydata.card_type='non_country_specific' order by countrydata.countryname",last_yr_fromdt,last_yr_todt);
			sprintf(command,"SELECT countrydata.countryname,coalesce(count,0) as count from (SELECT st.country countryname,count(*) from clienttrip c, sim_stock st,countrydata d,sim_user s where (d.card_type='non_country_specific' and d.countryname not in('Europe DirectCall Postpaid','GLOBAL POSTPAID','GLUNLIM POSTPAID')) and c.sim_phone_no=st.sim_phone_no and st.country = d.countryname and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.username=c.username and s.companyname !~* 'trikon|tsim' group by st.country) X right join countrydata on countrydata.countryname=X.countryname where countrydata.card_type='non_country_specific' and countrydata.countryname not in('Europe DirectCall Postpaid','GLOBAL POSTPAID','GLUNLIM POSTPAID') order by countrydata.countryname",last_yr_fromdt,last_yr_todt);
			do_log(command);
			int rec1 = db1.ExecTuplesOk(command);
			if (!download_mode){
				printf("\n<br>");
				printf("\n<table width=95%% border=0 style=\"float:left;\">");
				//printf("\n<tr class=Mh><th>%s To %s</th><th>Card Type</th><th>%s To %s</th></tr>\n",fromdt,todt,last_yr_fromdt,last_yr_todt);
				printf("\n<tr class=Mh><th>This Year<th>Card Type</th><th>Last Year</th></tr>\n");
				printf("\n<tr class=Ml><td>%s</td><td>Country Specific</td><td>%s</td></tr>\n",no_of_cs_this_yr,no_of_cs_last_yr);
			}
			else{
				printf("\n");
				printf("\nThis Year,Card Type,Last Year\n");
				printf("%s,Country Specific,%s\n",no_of_cs_this_yr,no_of_cs_last_yr);
			}
			no_booking_this_yr = atoi(no_of_cs_this_yr);
			int no_booking_last_yr = atoi(no_of_cs_last_yr);
                        char tmp_countryname[101];
                        char tmp_countryname1[101];
                        int tmp_count = 0;
                        if (rec1>rec){
                                for (int i=0; i<rec1;i++){
                                        strcpy(tmp_countryname1,db1.get_field(i,"countryname"));
                                        strcpy(tmp_countryname,"");
                                        tmp_count = 0;
                                        for (int j=0;j<rec;j++){
                                                if (!strcmp(tmp_countryname1,db->get_field(j,"countryname"))){
                                                        strcpy(tmp_countryname,db->get_field(j,"countryname"));
                                                        tmp_count = atoi(db->get_field(j,"count"));
                                                        break;
                                                }
                                        }
                                        if (!download_mode) printf("\n<tr class=Ml><td>%d</td><td>%s</td><td>%s</td></tr>\n",tmp_count,db1.get_field(i,"countryname"),db1.get_field(i,"count"));
					else printf("%d,%s,%s\n",tmp_count,db1.get_field(i,"countryname"),db1.get_field(i,"count"));
                                        no_booking_this_yr += tmp_count;
                                        no_booking_last_yr += atoi(db1.get_field(i,"count"));
                                }
                        }
                        else{
                                for (int i=0; i<rec;i++){
                                        strcpy(tmp_countryname,db->get_field(i,"countryname"));
                                        strcpy(tmp_countryname1,"");
                                        tmp_count = 0;
                                        for (int j=0;j<rec1;j++){
                                                if (!strcmp(tmp_countryname,db1.get_field(j,"countryname"))){
                                                        strcpy(tmp_countryname1,db1.get_field(j,"countryname"));
                                                        tmp_count = atoi(db1.get_field(j,"count"));
                                                        break;
                                                }
                                        }
                                        if (!download_mode) printf("\n<tr class=Ml><td>%s</td><td>%s</td><td>%d</td></tr>\n",db->get_field(i,"count"),db->get_field(i,"countryname"),tmp_count);
					else printf("%s,%s,%d\n",db->get_field(i,"count"),db->get_field(i,"countryname"),tmp_count);
                                        no_booking_this_yr += atoi(db->get_field(i,"count"));
                                        no_booking_last_yr += tmp_count;
                                }
                        }
			if (!download_mode) printf("<tr class=Ml><td>%d</td><td>In House</td><td>%d</td></tr>\n",((total_cnt_this_yr-atoi(no_of_cs_this_yr))+cnt_without_cs_this_yr),(total_cnt_last_yr-atoi(no_of_cs_last_yr))+cnt_without_cs_last_yr);
			else printf("%d,In House,%d\n",((total_cnt_this_yr-atoi(no_of_cs_this_yr))+cnt_without_cs_this_yr),(total_cnt_last_yr-atoi(no_of_cs_last_yr))+cnt_without_cs_last_yr);
			no_booking_this_yr+=(total_cnt_this_yr-atoi(no_of_cs_this_yr)+cnt_without_cs_this_yr);
			no_booking_last_yr+=(total_cnt_last_yr-atoi(no_of_cs_last_yr)+cnt_without_cs_last_yr);
			if (!download_mode) printf("<tr class=Mh><td>%d</td><td>Total</td><td>%d</td></tr>\n",no_booking_this_yr,no_booking_last_yr);
			else printf("%d,Total,%d\n",no_booking_this_yr,no_booking_last_yr);
			if (!download_mode) printf("</table>\n");
			/*
			printf("\n<table width=49%% border=0>");
			printf("<tr class=Mh><th>Sales Done By</th><th>This Year</th><th>Last Year</th>\n");
			printf("</table>\n");
			*/


			//******************************   Start of Sim Sales Summary ****************************// Added by Neekhil Dedhia
			if (!download_mode) printf("\n<td>");
			else printf("\n\n\n");
			char sales_list[256];
/*			if((get_tag_value(CONFIG_DATA_FILE,"<!-- EXCEPTION_LIST","-- EXCEPTION_LIST>","sales_list",sales_list)!=1)){
                                do_log("Could not open config file.");
                                printf("Error occured while opening config file.");
                        }*/
			
			 char* data40 = get_simmis_data_from_table("EXCEPTION_LIST_sales_list","");
                        if(data40[0]==0 || data40==NULL){
                        do_log("Could not get config data40");
                        printf("Error occured while getting config data.");}
                        strcpy(sales_list,data40);




			sprintf(command,"select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') group by sales_person order by sales_person",fromdt,todt); 
	//sprintf(command,"select sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') as countryname,count(*) from clienttrip c,sim_user s, sim_stock st  where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' group by sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') order by sales_person",fromdt,todt);
		        do_log(command);
		        count=db->ExecTuplesOk(command);
			
			//sprintf(command,"select salesperson.sp_name,coalesce(count,0) as count from (select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') group by sales_person) X right join salesperson on salesperson.sp_name=X.sales_person where sp_name !~* '%s' order by sp_name",fromdt,todt,sales_list);
			sprintf(command,"select salesperson.sp_name,coalesce(count,0) as count from (select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and s.companyname !~* 'trikon|tsim' and booking_date<timestamp_pl_interval('%s','86400') group by sales_person) X right join salesperson on salesperson.sp_name=X.sales_person where sp_name !~* '%s' order by sp_name",fromdt,todt,sales_list);

	//sprintf(command,"select salesperson.sp_name,X.countryname,coalesce(count,0) as count from (select sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') as countryname,count(*) from clienttrip c,sim_user s, sim_stock st  where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' group by sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') order by sales_person) X right join salesperson on salesperson.sp_name=X.sales_person where sp_name ~* 'alisha|ashwini|archana|kiranp|bhargavi|mansi|sapna'",fromdt,todt);

	//sprintf(command,"select salesperson.sp_name,X.countryname,coalesce(count,0) as count from (select sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') as countryname,count(*) from clienttrip c,sim_user s,countrydata cn, sim_stock st  where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400')and cn.countryname=regexp_replace(tripid,'[^a-zA-Z ]','','g') and cn.postpaid_prepaid='prepaid' and s.companyname !~* 'trikon|tsim' group by sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') order by sales_person) X right join salesperson on salesperson.sp_name=X.sales_person where sp_name ~* 'alisha|ashwini|archana|kiranp|bhargavi|mansi|sapna'",fromdt,todt);
			do_log(command);
                        rec=db1.ExecTuplesOk(command);
			fax_database db2(E2FAXDB,DBSERV_R);
			float totalcount=0.0f,totalcountly=0.0f;
			char piece1[10],piece2[10];
                        char *pch;
                        char sname[30];
                        int ctr=0,ctrly=0;
                        for(i=0;i<count;i++)                                         //******* Loop to count the number of joint sales e.g. abc/xyz thus creating only that number of 2D array
                        {
                                strcpy(sname,db->get_field(i,"sales_person"));
                                if(strchr(sname,'/'))
                                {
                                        ctr++;
                                }
                        }
                        char twod[ctr][4][40];int row=0;
                        for(i=0;i<count;i++)                                            //******** Loop to insert and divide the joint sales details in 2D array
                        {
                                strcpy(sname,db->get_field(i,"sales_person"));
                                if(strchr(sname,'/'))
                                {
                                        pch=strchr(sname,'/');
                                        int pos=pch-sname;
                                        sprintf(piece1,"%.*s",pos,&sname[0]);
                                        sprintf(piece2,"%.*s",(strlen(sname)-pos+1),&sname[pos+1]);
                                        strcpy(twod[row][0],piece1);
                                        strcpy(twod[row][1],piece2);
                                        strcpy(twod[row][2],db->get_field(i,"count"));
					//strcpy(twod[row][3],db->get_field(i,"countryname"));
                                        row++;
                                        //printf("\n<br>%s   %s %s %s %d %s",db->get_field(i,"sales_person"),db->get_field(i,"count"),piece1,piece2,row,db->get_field(i,"countryname"));
                                }
                        }
			sprintf(command,"select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') group by sales_person order by sales_person",last_yr_fromdt,last_yr_todt);

		//sprintf(command,"select sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') as countryname,count(*) from clienttrip c,sim_user s, sim_stock st  where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' group by sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') order by sales_person",last_yr_fromdt,last_yr_todt);
                        do_log(command);
                        count=db->ExecTuplesOk(command);
			for(i=0;i<count;i++)                                         //******* Loop to count the number of joint sales e.g. abc/xyz thus creating only that number of 2D array
                        {
                                strcpy(sname,db->get_field(i,"sales_person"));
                                if(strchr(sname,'/'))
                                {
                                        ctrly++;
                                }
                        }
                        char twodly[ctrly][4][40];row=0;
                        for(i=0;i<count;i++)                                            //******** Loop to insert and divide the joint sales details in 2D array
                        {
                                strcpy(sname,db->get_field(i,"sales_person"));
                                if(strchr(sname,'/'))
                                {
                                        pch=strchr(sname,'/');
                                        int pos=pch-sname;
                                        sprintf(piece1,"%.*s",pos,&sname[0]);
                                        sprintf(piece2,"%.*s",(strlen(sname)-pos+1),&sname[pos+1]);
                                        strcpy(twodly[row][0],piece1);
                                        strcpy(twodly[row][1],piece2);
                                        strcpy(twodly[row][2],db->get_field(i,"count"));
					//strcpy(twodly[row][3],db->get_field(i,"countryname"));
                                        row++;
                                        //printf("\n<br>%s   %s %s %s %d %s",db->get_field(i,"sales_person"),db->get_field(i,"count"),piece1,piece2,row,db->get_field(i,"countryname"));
                                }
                        }
			
			float in_house_sales_this_year,in_house_sales_last_year;
			sprintf(command,"select count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname ~* 'trikon|tsim'",fromdt,todt);
			do_log(command);
			if(db2.ExecTuplesOk(command))
				in_house_sales_this_year=atof(db2.get_field(0,"count"));

			sprintf(command,"select count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname ~* 'trikon|tsim'",last_yr_fromdt,last_yr_todt);
			do_log(command);
                        if(db2.ExecTuplesOk(command))
				in_house_sales_last_year=atof(db2.get_field(0,"count"));
			//sprintf(command,"select salesperson.sp_name,coalesce(count,0) as count from (select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') group by sales_person) X right join salesperson on salesperson.sp_name=X.sales_person where sp_name !~* '%s' order by sp_name",last_yr_fromdt,last_yr_todt,sales_list);
			sprintf(command,"select salesperson.sp_name,coalesce(count,0) as count from (select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and s.companyname !~* 'trikon|tsim' and booking_date<timestamp_pl_interval('%s','86400') group by sales_person) X right join salesperson on salesperson.sp_name=X.sales_person where sp_name !~* '%s' order by sp_name;",last_yr_fromdt,last_yr_todt,sales_list);
	//sprintf(command,"select salesperson.sp_name,X.countryname,coalesce(count,0) as count from (select sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') as countryname,count(*) from clienttrip c,sim_user s, sim_stock st  where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' group by sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') order by sales_person) X right join salesperson on salesperson.sp_name=X.sales_person where sp_name ~* 'alisha|ashwini|archana|kiranp|bhargavi|mansi|sapna'",last_yr_fromdt,last_yr_todt);

	//sprintf(command,"select salesperson.sp_name,X.countryname,coalesce(count,0) as count from (select sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') as countryname,count(*) from clienttrip c,sim_user s,countrydata cn, sim_stock st  where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400')and cn.countryname=regexp_replace(tripid,'[^a-zA-Z ]','','g') and cn.postpaid_prepaid='prepaid' and s.companyname !~* 'trikon|tsim' group by sales_person,regexp_replace(tripid,'[^a-zA-Z ]','','g') order by sales_person) X right join salesperson on salesperson.sp_name=X.sales_person where sp_name ~* 'alisha|ashwini|archana|kiranp|bhargavi|mansi|sapna'",last_yr_fromdt,last_yr_todt);
                        do_log(command);
                        rec1=db2.ExecTuplesOk(command);

			if (!download_mode){
			printf("\n<b><u><font color=\"blue\" size=\"2\" >Sim Sales Summary</font></u></b><br>");
                        printf("\n<br>");
                        printf("\n<table width=95%% border=0>");
                        //printf("\n<tr class=Mh><th>%s To %s</th><th>Sales Person</th><th>%s To %s</th></tr>",fromdt,todt,last_yr_fromdt,last_yr_todt);
			printf("\n<tr class=Mh><th>This Year</th><th>Sales Person</th><th>Last Year</th>");
			//printf("\n<tr class=Mh><th>This Year</th><th>Country</th><th>Last Year</th></tr>");
			//printf("<b><u>This Year</b></u>");
			}
			else{
				printf("\nSim Sales Summary\n");
				printf("This Year,Sales Person,Last Year\n");
			}
			char country11[30];
			for(i=0;i<rec;i++)
			{
				//printf("<b><u>This Year</b></u>");
				//char country11[30];
				//printf("<table border=1><tr><td>");
				//printf("%s</td><td>%s</td>",db1.get_field(i,"sp_name"),db1.get_field(i,"countryname"));
				//strcpy(sname,db1.get_field(i,"sp_name"));
				//strcpy(country11,db1.get_field(i,"countryname"));
				//if(strstr(country11,"BLACKBERRY") || !strcmp(country11,"WorldFree Smart") || !strcmp(country11,"Europe DirectCall") || !strcmp(country11,"USA Euro Data SIM") || !strcmp(country11,"GLOBAL NEW") || !strcmp(country11,"GLOBAL") || !strcmp(country11,"WorldFree Standard") || !strcmp(country11,"USA WORLDWIDE") )
				//printf("%s</td><td> %s</td>",db1.get_field(i,"sp_name"),db1.get_field(i,"countryname"));
				//else
				//printf("%s</td><td>CS %s</td>",db1.get_field(i,"sp_name"),db1.get_field(i,"countryname"));
				float halfcount=0.0f;
				char *c;
				c=db1.get_field(i,"count");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                {
                                        for(row=0;row<ctr;row++)
                                        {
                                                if( (!(strcasecmp(sname,twod[row][0])) || !(strcasecmp(sname,twod[row][1]))) && (!strcmp(country11,twod[row][3])) )
                                                {
                                                        halfcount=halfcount+(atof(twod[row][2])/2);
                                                }
						/*else
						{
							printf("<br><i>%s %s %s %s</i>",twod[row][0],twod[row][1],twod[row][2],twod[row][3]);
						}*/
                                        }
                                        //printf("<td>%.2f</td></tr></table>",atof(db1.get_field(i,"count"))+halfcount);
					//totalcount+=(atof(db1.get_field(i,"count"))+halfcount);
				}
			}
			//printf("<b>Total:%.2f</b>",totalcount);
			//printf("<br><br><b><u>Last Year</b></u>");
			for(i=0;i<rec1;i++)
			{
				//printf("<table border=1><tr><td>");
				//printf("%s</td><td>%s</td>",db2.get_field(i,"sp_name"),db2.get_field(i,"countryname"));
				//strcpy(sname,db2.get_field(i,"sp_name"));
                                //strcpy(country11,db2.get_field(i,"countryname"));
					//if(strstr(country11,"BLACKBERRY") || !strcmp(country11,"WorldFree Smart") || !strcmp(country11,"Europe DirectCall") || !strcmp(country11,"USA Euro Data SIM") || !strcmp(country11,"GLOBAL NEW") || !strcmp(country11,"GLOBAL") || !strcmp(country11,"WorldFree Standard") || !strcmp(country11,"USA WORLDWIDE") )
                                //printf("%s</td><td> %s</td>",db2.get_field(i,"sp_name"),db2.get_field(i,"countryname"));
                                //else
                                //printf("%s</td><td>CS %s</td>",db2.get_field(i,"sp_name"),db2.get_field(i,"countryname"));
				float halfcountly=0.0f;
				char *c;
				c=db2.get_field(i,"count");
				if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                {
                                        for(row=0;row<ctrly;row++)
                                        {
                                                if( (!(strcasecmp(sname,twodly[row][0])) || !(strcasecmp(sname,twodly[row][1]))) && (!strcmp(country11,twodly[row][3])) )
                                                {
                                                        halfcountly=halfcountly+(atof(twodly[row][2])/2);
                                                }
                                        }
                                        //printf("<td>%.2f</td></tr></table>",atof(db2.get_field(i,"count"))+halfcountly);
                                        //totalcountly+=(atof(db2.get_field(i,"count"))+halfcountly);
                                }
			}
			//printf("<b>Total:%.2f</b>",totalcountly);	


			if(rec==rec1)
                        for(i=0;i<rec;i++)
                        {
				strcpy(sname,db1.get_field(i,"sp_name"));
				float halfcount=0.0f,halfcountly=0.0f;
                                if (!download_mode) printf("<tr class=Ml>"); else printf("\n");
				char *c;
				c=db1.get_field(i,"count");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                {
                                        for(row=0;row<ctr;row++)
                                        {
                                                if( !(strcasecmp(sname,twod[row][0])) || !(strcasecmp(sname,twod[row][1])) )
                                                {
                                                        halfcount=halfcount+(atof(twod[row][2])/2);
                                                }
                                        }
                                        if (!download_mode) printf("<td>%.2f</td>",atof(db1.get_field(i,"count"))+halfcount);
					else printf("%.2f,",atof(db1.get_field(i,"count"))+halfcount);
                                        totalcount+=(atof(db1.get_field(i,"count"))+halfcount);
                                }
				c=db1.get_field(i,"sp_name");
				if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
					if (!download_mode) printf("<td>%s</td>",db1.get_field(i,"sp_name")); else printf("%s,",db1.get_field(i,"sp_name"));
				c=db2.get_field(i,"count");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                {
                                        for(row=0;row<ctrly;row++)
                                        {
                                                if( !(strcasecmp(sname,twodly[row][0])) || !(strcasecmp(sname,twodly[row][1])) )
                                                {
                                                        halfcountly=halfcountly+(atof(twodly[row][2])/2);
                                                }
                                        }
                                        if (!download_mode) printf("<td>%.2f</td>",atof(db2.get_field(i,"count"))+halfcountly); else printf("%.2f,",atof(db2.get_field(i,"count"))+halfcountly);
                                        totalcountly+=(atof(db2.get_field(i,"count"))+halfcountly);
                                }
				if (!download_mode) printf("</tr>"); //else printf("\n");
                        }
			if (!download_mode) printf("<tr class=Ml><td>%.2f</td><td>In house</td><td>%.2f</td>",in_house_sales_this_year,in_house_sales_last_year);
			else printf("\n%.2f,In house,%.2f",in_house_sales_this_year,in_house_sales_last_year);
			totalcount+=in_house_sales_this_year;
			totalcountly+=in_house_sales_last_year;
			if (!download_mode) printf("<tr class=Mh><td>%.2f</td><td>Total</td><td>%.2f</td>",totalcount,totalcountly); else printf("\n%.2f,Total,%.2f",totalcount,totalcountly);
			if (!download_mode) printf("\n</table>"); else printf("\n");
		        //******************************   End  of Sim Sales Summary ****************************// Added by Neekhil Dedhia


			//******************************   Start  of Sim Sales Summary by Type ****************************// Added by Neekhil Dedhia
			if (!download_mode) printf("\n<td>");
		
			sprintf(command,"select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim'  group by sales_person order by sales_person",fromdt,todt);
			do_log(command);
		        count=db->ExecTuplesOk(command);
			 float field=0.0f,tele=0.0f,reseller=0.0f,others=0.0f,fieldly=0.0f,telely=0.0f,resellerly=0.0f,othersly=0.0f; 
		        if (count>0){
//binu				if (!download_mode) printf("\n<table width=95%% border=0>");
		                char piece1[10],piece2[10];
		                char *pch;
		                char sname[30];
		                int ctr=0;
		
		                for(i=0;i<count;i++)                                         //******* Loop to count the number of joint sales e.g. abc/xyz thus creating only that number of 2D array
		                {
		                        strcpy(sname,db->get_field(i,"sales_person"));
		                        if(strchr(sname,'/'))
		                        {
		                                ctr++;
		                        }
		                }
		                char twod[ctr][3][20];int row=0;
		                for(i=0;i<count;i++)                                            //******** Loop to insert and divide the joint sales details in 2D array
		                {
		                        strcpy(sname,db->get_field(i,"sales_person"));
		                        if(strchr(sname,'/'))
		                        {
		                                pch=strchr(sname,'/');
		                                int pos=pch-sname;
		                                sprintf(piece1,"%.*s",pos,&sname[0]);
		                                sprintf(piece2,"%.*s",(strlen(sname)-pos+1),&sname[pos+1]);
		                                strcpy(twod[row][0],piece1);
		                                strcpy(twod[row][1],piece2);
		                                strcpy(twod[row][2],db->get_field(i,"count"));
		                                row++;
		                                //printf("\n<br>%s   %s %s %s %d",db->get_field(i,"sales_person"),db->get_field(i,"count"),piece1,piece2,row);
		                        }
		                }
				//float field=0.0f,tele=0.0f,reseller=0.0f,others=0.0f;
				// printf("%f %f %f %f %d",field,others,tele,reseller,row);
				for(i=0;i<(row);i++)
				{
					for(int j=0;j<2;j++)
					{	
						sprintf(command,"select sp_type from salesperson where sp_name='%s'",twod[i][j]);
						if(db->ExecTuplesOk(command))
						{
							if(!strcmp(db->get_field(0,"sp_type"),"Field Sales"))
								field+=atof(twod[i][2])/2;
							else if(!strcmp(db->get_field(0,"sp_type"),"Others"))
								others+=atof(twod[i][2])/2;
							else if(!strcmp(db->get_field(0,"sp_type"),"Telesales"))
								tele+=atof(twod[i][2])/2;
							else
								reseller+=atof(twod[i][2])/2;
						}
					}
				}
			}
			sprintf(command,"select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' group by sales_person order by sales_person",last_yr_fromdt,last_yr_todt);
                        do_log(command);
                        count=db->ExecTuplesOk(command);
                        if (count>0){
     //binu                           if (!download_mode) printf("\n<table width=95%% border=0>");
                                char piece1[10],piece2[10];
                                char *pch;
                                char sname[30];
                                int ctr=0;

                                for(i=0;i<count;i++)                                         //******* Loop to count the number of joint sales e.g. abc/xyz thus creating only that number of 2D array
                                {
                                        strcpy(sname,db->get_field(i,"sales_person"));
                                        if(strchr(sname,'/'))
                                        {
                                                ctr++;
                                        }
                                }
                                char twod[ctr][3][20];int row=0;
                                for(i=0;i<count;i++)                                            //******** Loop to insert and divide the joint sales details in 2D array
                                {
                                        strcpy(sname,db->get_field(i,"sales_person"));
                                        if(strchr(sname,'/'))
                                        {
                                                pch=strchr(sname,'/');
                                                int pos=pch-sname;
                                                sprintf(piece1,"%.*s",pos,&sname[0]);
                                                sprintf(piece2,"%.*s",(strlen(sname)-pos+1),&sname[pos+1]);
                                                strcpy(twod[row][0],piece1);
                                                strcpy(twod[row][1],piece2);
                                                strcpy(twod[row][2],db->get_field(i,"count"));
                                                row++;
                                                //printf("\n<br>%s   %s %s %s %d",db->get_field(i,"sales_person"),db->get_field(i,"count"),piece1,piece2,row);
                                        }
                                }
                                //float field=0.0f,tele=0.0f,reseller=0.0f,others=0.0f;
                                // printf("%f %f %f %f %d",field,others,tele,reseller,row);
                                for(i=0;i<(row);i++)
                                {
                                        for(int j=0;j<2;j++)
                                        {
                                                sprintf(command,"select sp_type from salesperson where sp_name='%s'",twod[i][j]);
                                                if(db->ExecTuplesOk(command))
                                                {
                                                        if(!strcmp(db->get_field(0,"sp_type"),"Field Sales"))
                                                                fieldly+=atof(twod[i][2])/2;
                                                        else if(!strcmp(db->get_field(0,"sp_type"),"Others"))
                                                                othersly+=atof(twod[i][2])/2;
                                                        else if(!strcmp(db->get_field(0,"sp_type"),"Telesales"))
                                                                telely+=atof(twod[i][2])/2;
                                                        else
                                                                resellerly+=atof(twod[i][2])/2;
                                                }
                                        }
                                }
				//printf("%f %f %f %f",field,others,tele,reseller);	
				// printf("\n</table>");
			}
			//sprintf(command,"select distinct sp_type from salesperson order by sp_type");
			//db2.ExecTuplesOk(command);
			sprintf(command,"select distinct salesperson.sp_type,coalesce(count,0) as count from (SELECT sp_type,count(*) from clienttrip c,sim_user s, sim_stock st,salesperson sp where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and sp.sp_name=s.sales_person and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim'  group by sp_type) X right join salesperson on salesperson.sp_type=X.sp_type order by sp_type",fromdt,todt);
			do_log(command);
		        count=db->ExecTuplesOk(command);

			sprintf(command,"select distinct salesperson.sp_type,coalesce(count,0) as count from (SELECT sp_type,count(*) from clienttrip c,sim_user s, sim_stock st,salesperson sp where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and sp.sp_name=s.sales_person and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim'  group by sp_type) X right join salesperson on salesperson.sp_type=X.sp_type order by sp_type",last_yr_fromdt,last_yr_todt);
                        do_log(command);
			db1.ExecTuplesOk(command);

		        if (count>0){
				float totalcnt=(field+others+tele+reseller);
				float totalcntly=(fieldly+othersly+telely+resellerly);
		                if (!download_mode){
					printf("\n<b><u><font color=\"blue\" size=\"2\" >Sim Sales Summary by Type</font></u></b><br>");
		                	printf("\n<br>");
		                	printf("\n<table width=95%% border=0>");
		                //printf("\n<tr class=Mh><th>%s To %s<th>Sales Person Type</th><th>%s To %s</th></tr>",fromdt,todt,last_yr_fromdt,last_yr_todt);
					printf("\n<tr class=Mh><th>This Year</th><th>Sales Person Type</th><th>Last Year</th></tr>");
				}
				else{
					printf("Sim Sales Summary by Type\n");
					printf("\nThis Year,Sales Person Type,Last Year\n");
				}
				for(i=0;i<count;i++){                                           //******** Loop to print details of sales in table
		                        if (!download_mode) printf("\n<tr class=Ml>");
					else printf("\n");
					char *c;
					c=db->get_field(i,"count");
                                        if((c==NULL)||c[0]==0)
                                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                        else
                                        {
                                                if(!strcmp(db->get_field(i,"sp_type"),"Field Sales"))
                                                {
                                                        field+=atof(db->get_field(i,"count"));
                                                        if (!download_mode) printf("<td>%.2f</td>",field); else printf("%.2f,",field);
                                                }
                                                else if(!strcmp(db->get_field(i,"sp_type"),"Others"))
                                                {
                                                        others+=atof(db->get_field(i,"count"));
                                                        if (!download_mode) printf("<td>%.2f</td>",others); else printf("%.2f,",others);
                                                }
                                                else if(!strcmp(db->get_field(i,"sp_type"),"Reseller"))
                                                {
                                                        reseller+=atof(db->get_field(i,"count"));
                                                        if (!download_mode) printf("<td>%.2f</td>",reseller); else printf("%.2f,",reseller);
                                                }
                                                else if(!strcmp(db->get_field(i,"sp_type"),"Telesales"))
                                                {
                                                        tele+=atof(db->get_field(i,"count"));
                                                        if (!download_mode) printf("<td>%.2f</td>",tele); else printf("%.2f,",tele);
                                                }
                                                totalcnt+=atof(db->get_field(i,"count"));
                                        }
		                        c=db->get_field(i,"sp_type");
		                        if((c==NULL)||c[0]==0)
		                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
		                        else
		                        {      
		                                if (!download_mode) printf("<td>%s</td>",db->get_field(i,"sp_type")); else printf("%s,",db->get_field(i,"sp_type"));
		                        }
		                        c=db1.get_field(i,"count");
		                        if((c==NULL)||c[0]==0)
		                                if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
		                        else
		                        {
						if(!strcmp(db1.get_field(i,"sp_type"),"Field Sales"))
						{
							fieldly+=atof(db1.get_field(i,"count"));
							if (!download_mode) printf("<td>%.2f</td>",fieldly); else printf("%.2f,",fieldly);
						}
						else if(!strcmp(db1.get_field(i,"sp_type"),"Others"))
						{
							othersly+=atof(db1.get_field(i,"count"));
		                                        if (!download_mode) printf("<td>%.2f</td>",othersly); else printf("%.2f,",othersly);
						}
						else if(!strcmp(db1.get_field(i,"sp_type"),"Reseller"))
						{
							resellerly+=atof(db1.get_field(i,"count"));
		                                        if (!download_mode) printf("<td>%.2f</td>",resellerly); else printf("%.2f,",resellerly);
						}
                                                else if(!strcmp(db1.get_field(i,"sp_type"),"Telesales"))
                                                {
                                                        telely+=atof(db1.get_field(i,"count"));
                                                        if (!download_mode) printf("<td>%.2f</td>",telely); else printf("%.2f,",telely);
                                                }
						totalcntly+=atof(db1.get_field(i,"count"));
					}
				}
				if (!download_mode) printf("<tr class=Ml><td>%.2f</td><td>In House</td><td>%.2f</td></tr>\n",in_house_sales_this_year,in_house_sales_last_year);
				else printf("\n%.2f,In House,%.2f\n",in_house_sales_this_year,in_house_sales_last_year);
				totalcnt+=in_house_sales_this_year;
				totalcntly+=in_house_sales_last_year;
				if (!download_mode) printf("<tr class=Mh><td>%.2f</td><td>Total</td><td>%.2f</td></tr>\n",totalcnt,totalcntly);
				else  printf("\n%.2f,Total,%.2f\n",totalcnt,totalcntly);
		                if (!download_mode) printf("\n</table>");
			}

			//******************************   End  of Sim Sales Summary by Type **************************// Added by Neekhil Dedhia

			//******************************   Start  of City Summary ****************************// Added by Neekhil Dedhia
                        if (!download_mode) printf("\n<td>");
			char city_list[120];
/*			if((get_tag_value(CONFIG_DATA_FILE,"<!-- EXCEPTION_LIST","-- EXCEPTION_LIST>","city_list",city_list)!=1)){
		                do_log("Could not open config file.");
		                printf("Error occured while opening config file.");
		        }*/

			 char* data41 = get_simmis_data_from_table("EXCEPTION_LIST_sales_list","");
                        if(data41[0]==0 || data41==NULL){
                        do_log("Could not get config data41");
                        printf("Error occured while getting config data.");}
                        strcpy(sales_list,data41);


			
			sprintf(command,"select count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim'",fromdt,todt);
			do_log(command);
			if (db->ExecTuplesOk(command)){
                                strcpy(no_of_cs_this_yr,db->get_field(0,"count"));
                        }
			int roi_cnt=atoi(no_of_cs_this_yr);
			sprintf(command,"select add_city,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' and add_city ~* '%s' group by add_city order by add_city",fromdt,todt,city_list);
			do_log(command);
			count=db->ExecTuplesOk(command);
			sprintf(command,"select count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim'",last_yr_fromdt,last_yr_todt);
			do_log(command);
			if(db1.ExecTuplesOk(command)){
                                strcpy(no_of_cs_last_yr,db1.get_field(0,"count"));
                        }

                        int roi_cnt_last_yr=atoi(no_of_cs_last_yr);
                        sprintf(command," select add_city,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' and add_city ~* '%s' group by add_city order by add_city",last_yr_fromdt,last_yr_todt,city_list);
			do_log(command);
			int count_last_yr=db1.ExecTuplesOk(command);
			int totalcnt=0;int totalcnt_lastyr=0;

			char tmp_cityname[30];
			char tmp_cityname1[30];
			tmp_count=0;
			if (!download_mode){
			printf("\n<b><u><font color=\"blue\" size=\"2\" >City Summary</font></u></b><br>");
                        printf("\n<br>");
                        printf("\n<table width=95%% border=0>");
                        //printf("\n<tr class=Mh><th>%s To %s</th><th>City</th><th>%s To %s</th></tr>",fromdt,todt,last_yr_fromdt,last_yr_todt);
			printf("\n<tr class=Mh><th>This Year</th><th>City</th><th>Last Year</th>");
			}
			else{
				printf("\nCity Summary\n");
				printf("\nThis Year,City,Last Year\n");
			}
			if(count_last_yr>count)
			{
				for(int i=0;i<count_last_yr;i++)
				{
					strcpy(tmp_cityname1,db1.get_field(i,"add_city"));
					strcpy(tmp_cityname,"");
					tmp_count = 0;
					for(int j=0;j<count;j++)
					{
						if(!strcmp(tmp_cityname1,db->get_field(j,"add_city")))
						{
							strcpy(tmp_cityname,db->get_field(j,"add_city"));
							tmp_count=atoi(db->get_field(j,"count"));
							break;
						}
					}
					if (!download_mode) printf("\n<tr class=Ml><td>%d</td><td>%s</td><td>%s</td></tr>\n",tmp_count,db1.get_field(i,"add_city"),db1.get_field(i,"count"));
					else printf("%d,%s,%s\n",tmp_count,db1.get_field(i,"add_city"),db1.get_field(i,"count"));
					totalcnt+=tmp_count;
					totalcnt_lastyr+=atoi(db1.get_field(i,"count"));
				}
			}
			else
			{
				for(int i=0;i<count;i++)
				{
					strcpy(tmp_cityname,db->get_field(i,"add_city"));
					strcpy(tmp_cityname1,"");
					tmp_count = 0;
                                        for (int j=0;j<count_last_yr;j++)
					{
						if(!strcmp(tmp_cityname,db1.get_field(j,"add_city")))
						{
							strcpy(tmp_cityname1,db1.get_field(j,"add_city"));
							tmp_count = atoi(db1.get_field(j,"count"));
                                                        break;
						}
					}
					if (!download_mode) printf("\n<tr class=Ml><td>%s</td><td>%s</td><td>%d</td></tr>\n",db->get_field(i,"count"),db->get_field(i,"add_city"),tmp_count);
					else printf("%s,%s,%d\n",db->get_field(i,"count"),db->get_field(i,"add_city"),tmp_count);
					totalcnt+=atoi(db->get_field(i,"count"));
					totalcnt_lastyr+=tmp_count;
				}
			}
			if (!download_mode) printf("<tr class=Ml><td>%d</td><td>Rest of India</td><td>%d</td>\n",roi_cnt-totalcnt,roi_cnt_last_yr-totalcnt_lastyr);
			else printf("%d,Rest of India,%d\n",roi_cnt-totalcnt,roi_cnt_last_yr-totalcnt_lastyr);
                        if (!download_mode) printf("<tr class=Ml><td>%d</td><td>In house</td><td>%d</td>\n",(int)in_house_sales_this_year,(int)in_house_sales_last_year);
			else printf("%d,In house,%d\n",(int)in_house_sales_this_year,(int)in_house_sales_last_year);
	                totalcnt+=(roi_cnt-totalcnt+(int)in_house_sales_this_year);
                        totalcnt_lastyr+=(roi_cnt_last_yr-totalcnt_lastyr+(int)in_house_sales_last_year);
			if (!download_mode) printf("<tr class=Mh><td>%d</d><td>Total</td><td>%d</td></tr>\n",totalcnt,totalcnt_lastyr);
			else printf("%d,Total,%d\n",totalcnt,totalcnt_lastyr);
                        if (!download_mode) printf("\n</table>");
			//******************************   End  of City Summary ************************* //

			//******************************   Start of Case source Summary ************************* //
                        if (!download_mode) printf("\n<td>");
			sprintf(command,"select Y.case_source,coalesce(count,0) as count from (select case_source,coalesce(count(*),0) as count from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' and c.case_source is not null and c.case_source ~* 'direct sales|crm|incoming call|reference|existing client|reseller|services|online booking|callrcvd_sales@tsim.in|zopim' group by case_source ) X right join (select distinct case_source from clienttrip where case_source is not null) Y on Y.case_source=X.case_source  order by Y.case_source",fromdt,todt);
			do_log(command);
		        count=db->ExecTuplesOk(command);
			sprintf(command,"select Y.case_source,coalesce(count,0) as count from (select case_source,coalesce(count(*),0) as count from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status not like 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' and c.case_source is not null and c.case_source ~* 'direct sales|crm|incoming call|reference|existing client|reseller|services|online booking|callrcvd_sales@tsim.in|zopim' group by case_source ) X right join (select distinct case_source from clienttrip where case_source is not null) Y on Y.case_source=X.case_source  order by Y.case_source",last_yr_fromdt,last_yr_todt);
			do_log(command);
		        int cnt_lastyr=db1.ExecTuplesOk(command);
			if (count>0){
		                if (!download_mode){
					printf("\n<b><u><font color=\"blue\" size=\"2\" >Sim Sales Summary by Case Source</font></u></b><br>");
		                	printf("\n<br>");
		                	printf("\n<table width=95%% border=0>");
		                //printf("\n<tr class=Mh><th>%s To %s<th>Sales Person Type</th><th>%s To %s</th></tr>",fromdt,todt,last_yr_fromdt,last_yr_todt);
					printf("\n<tr class=Mh><th>This Year</th><th>Sim Sales Summary by Case Source</th><th>Last Year</th></tr>");
				}
				else{
					printf("Sim Sales Summary by Case Source\n");
					printf("\nThis Year,Sales Person Type,Last Year\n");
				}
				int trips = 0;
				int trips_last_yr = 0;
				for(i=0;i<count;i++){                                           //******** Loop to print details of sales in table
		                        if (!download_mode) printf("\n<tr class=Ml>");
					else printf("\n");
					if (!download_mode) {
						printf("<tr class=Ml><td>%s</td><td>%s</td><td>%s</td></tr>\n",db->get_field(i,"count"),db->get_field(i,"case_source"),db1.get_field(i,"count"));
						trips+=atoi(db->get_field(i,"count"));
						trips_last_yr+=atoi(db1.get_field(i,"count"));
					}
					else {
						printf("\n%.2f,%s,%.2f\n",db->get_field(i,"count"),db->get_field(i,"case_source"),db1.get_field(i,"count"));
						trips+=atoi(db->get_field(i,"count"));
						trips_last_yr+=atoi(db1.get_field(i,"count"));
					}
				}
                        	if (!download_mode) printf("<tr class=Ml><td>%d</td><td>In house</td><td>%d</td>\n",(int)in_house_sales_this_year,(int)in_house_sales_last_year);
				else printf("%d,In house,%d\n",(int)in_house_sales_this_year,(int)in_house_sales_last_year);
				trips+=in_house_sales_this_year;
				trips_last_yr+=in_house_sales_last_year;
				if (!download_mode) printf("<tr class=Mh><td>%d</d><td>Total</td><td>%d</td></tr>\n",trips,trips_last_yr);
				else printf("%d,Total,%d\n",count,cnt_lastyr);
			}

			//******************************   End of Case source Summary ************************* //

			//******************************   Start of Handset Inventory stats ************************* //
			if (!download_mode){
			printf("\n</td></tr>");
			printf("\n<tr>");
			printf("\n<br>");
			printf("\n<td colspan=3><b><u><font color=\"blue\" size=\"2\" >Alloted Handset Inventory Details</font></u></b><br>");
			}
			else{
				printf("\n\n\n");
				printf("Alloted Handset Inventory Details\n");
			}
			//sprintf(command,"SELECT substring(handset_model from 18) as handset,'ISSUED' as status,from_date::date,to_date::date,c.username,clientname,companyname,sales_person,country from clienttrip c,sim_user s,sim_stock st where c.username=s.username and lower(handset_model) like lower('%Office Handset%') and status in ('Delivered','Delivery pending') and booking_date>='%s' and booking_date<='%s' and c.sim_phone_no=st.sim_phone_no order by from_date",fromdt,todt);
			sprintf(command,"SELECT substring(handset_model from 18) as handset,'ISSUED' as status,from_date::date,to_date::date,c.username,clientname,companyname,sales_person,country,app_provided from clienttrip c,sim_user s,sim_stock st where c.username=s.username and lower(handset_model) like lower('%Office%') and status in ('Delivered','Delivery pending') and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and c.sim_phone_no=st.sim_phone_no order by from_date",fromdt,todt);
			do_log(command);
			if (!download_mode){
				printf("<table width=95%%>");
				printf("<br><tr class=Mh>");
				printf("<th>Sr</th><th>Handset Code</th><th>Status</th><th>Trip Start Date</th><th>Trip End Date</th><th>Username</th><th>Client Name</th><th>Company Name</th><th>Sales Person</th><th>Card Type</th><th>App Provided</th><tr>");
			}
			else{
				printf("\n\nSr,Handset Code,Status,Trip Start Date,Trip End Date,Username,Client Name,Company Name,Sales Person,Card Type,App Provided\n\n");
			}
			int rec_cnt = db->ExecTuplesOk(command);
			for(i=0;i<rec_cnt;i++)
			{
				//printf("<tr class=Ml><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",i+1,db->get_field(i,"handset"),db->get_field(i,"status"),db->get_field(i,"from_date"),db->get_field(i,"to_date"),db->get_field(i,"username"),db->get_field(i,"clientname"),db->get_field(i,"companyname"),db->get_field(i,"sales_person"),db->get_field(i,"country"));
				if (!download_mode) printf("<tr class=Ml><td>%d</td>",i+1); else printf("%d,",i+1);
				char *c;
                                c=db->get_field(i,"handset");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,"); 
                                else
					if (!download_mode) printf("<td>%s</td>",db->get_field(i,"handset")); else printf("%s,",db->get_field(i,"handset"));
				c=db->get_field(i,"status");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"status")); else printf("%s,",db->get_field(i,"status"));
				c=db->get_field(i,"from_date");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"from_date")); else printf("%s,",db->get_field(i,"from_date"));
				c=db->get_field(i,"to_date");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"to_date")); else printf("%s,",db->get_field(i,"to_date"));
				c=db->get_field(i,"username");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"username")); else printf("%s,",db->get_field(i,"username"));
				c=db->get_field(i,"clientname");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"clientname")); else printf("%s,",db->get_field(i,"clientname"));
				c=db->get_field(i,"companyname");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"companyname")); else printf("%s,",db->get_field(i,"companyname"));
				c=db->get_field(i,"sales_person");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"sales_person")); else printf("%s,",db->get_field(i,"sales_person"));
				c=db->get_field(i,"country");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                        		if (!download_mode) printf("<td>%s</td>",db->get_field(i,"country")); else printf("%s,",db->get_field(i,"country"));
				c=db->get_field(i,"app_provided");
				if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"app_provided")); else printf("%s,",db->get_field(i,"app_provided"));
			}
			if (!download_mode) printf("</table>"); else printf("\n");
			if (!download_mode) printf("\n</td><td><b><u><font color=\"blue\" size=\"2\" >Instock Handset Inventory Details</font></u></b><br>");
			else printf("\nInstock Handset Inventory Details\n");
			/*
			sprintf(command,"SELECT * from sim_accessories_stock where alloted =false and active=true");
			do_log(command);
			if (!download_mode){
			printf("<table width=90%%>");
                        printf("<br><tr class=Mh>");
                        printf("<th>Sr</th><th>Handset Code</th>");
			}
			else{
				printf("\n\n\nHandset Code\n");
			}
			rec_cnt = db->ExecTuplesOk(command);
                        for(i=0;i<rec_cnt;i++)
                        {
				if (!download_mode) printf("<tr class=Ml><td>%d</td>",i+1); else printf("\n%d,",i+1);
                                char *c;
                                c=db->get_field(i,"display_name");
                                if((c==NULL)||c[0]==0)
                                        if (!download_mode) printf("<td>NA</td>"); else printf("NA,");
                                else
                                        if (!download_mode) printf("<td>%s</td>",db->get_field(i,"display_name")); else printf("%s,",db->get_field(i,"display_name"));
			}
			*/
			if (!download_mode) printf("\n</tr></table></td></tr></table>"); else printf("\n");
			

			
			//****************************** End of Handset Inventory Stats ************************* //


			// ***************************** Half yearly Telesales Stats ***************************** //
			/*
			sprintf(command,"select sales_person,country,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' group by sales_person,country order by sales_person",fromdt,todt);
			do_log(command);
			count=db->ExecTuplesOk(command);
			totalcount=0.0f;
			char piece3[30];
                        ctr=0,ctrly=0;
                        for(i=0;i<count;i++)                                         //******* Loop to count the number of joint sales e.g. abc/xyz thus creating only that number of 2D array
                        {
                                strcpy(sname,db->get_field(i,"sales_person"));
                                if(strchr(sname,'/'))
                                {
                                        ctr++;
                                }
                        }
			twod[ctr][4][40];row=0;
                        for(i=0;i<count;i++)                                            //******** Loop to insert and divide the joint sales details in 2D array
                        {
                                strcpy(sname,db->get_field(i,"sales_person"));
                                if(strchr(sname,'/'))
                                {
                                        pch=strchr(sname,'/');
                                        int pos=pch-sname;
                                        sprintf(piece1,"%.*s",pos,&sname[0]);
                                        sprintf(piece2,"%.*s",(strlen(sname)-pos+1),&sname[pos+1]);
                                        strcpy(twod[row][0],piece1);
                                        strcpy(twod[row][1],piece2);
                                        strcpy(twod[row][2],db->get_field(i,"count"));
					strcpy(twod[row][3],db->get_field(i,"country"));
                                        //strcpy(twod[row][3],db->get_field(i,"countryname"));
                                        row++;
                                        //printf("\n<br>%s   %s %s %s %d %s",db->get_field(i,"sales_person"),db->get_field(i,"count"),piece1,piece2,row,db->get_field(i,"country"));
                                }
                        }*/
			/*
			printf("<br><br><br><b><u>Test</b></u><br><br><br>");
			//sprintf(command,"select sales_person ,country,count(*) from clienttrip c,sim_user s, sim_stock st,salesperson sp where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' and sp.sp_name = s.sales_person and sp_type='Telesales' group by sales_person,country order by sales_person",fromdt,todt);
			sprintf(command,"select sp_name as sales_person,countryname as country,coalesce(count,0) as count from (select sales_person ,country,count(*) from clienttrip c,sim_user s, sim_stock st,salesperson sp where c.username=s.username and status!='Canceled' and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and s.companyname !~* 'trikon|tsim' and sp.sp_name = s.sales_person and sp_type='Telesales' group by sales_person,country order by sales_person) X right join (SELECT sp_name,countryname from salesperson cross join countrydata where sp_type='Telesales') Y on X.sales_person=Y.sp_name and X.country=Y.countryname order by sales_person,country",fromdt,todt);
			do_log(command);
			rec=0;
			rec=db1.ExecTuplesOk(command);
			printf("<table border=1><tr><th>Sales Person</th><th>Country</th><th>Count</th><tr>");
			for(i=0;i<rec;i++)
                        {
				printf("<tr><td>%s</td><td>%s</td>",db1.get_field(i,"sales_person"),db1.get_field(i,"country"));
                                char country11[30];
                                strcpy(sname,db1.get_field(i,"sales_person"));
                                strcpy(country11,db1.get_field(i,"country"));
                                float halfcount=0.0f;
                                char *c;
                                c=db1.get_field(i,"count");
                                if((c==NULL)||c[0]==0)
                                        printf("NA");
                                else
                                {
                                        for(row=0;row<ctr;row++)
                                        {
                                                if( (!(strcasecmp(sname,twod[row][0])) || !(strcasecmp(sname,twod[row][1]))) && (!strcmp(country11,twod[row][3])) )
                                                {
                                                        halfcount=halfcount+(atof(twod[row][2])/2);
                                                }
                                        }
                                        printf("<td>%.2f</td></tr>",atof(db1.get_field(i,"count"))+halfcount);
                                        totalcount+=(atof(db1.get_field(i,"count"))+halfcount);
                                }
                        }
			printf("<tr><td></td><td>Total</td><td>%.2f</td></tr></table>",totalcount);
			*/
			// ***************************** Half yearly Telesales Stats ***************************** //

		} //End of include stats
        }
        else
                if (!download_mode) printf("\n<br><center><b>No records present in the specified duration.</b></center><br>"); else printf("\nNo records present in the specified duration.\n");



	char exception[1024];
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- EXCEPTION_LIST","-- EXCEPTION_LIST>","exception_list",exception)!=1)){
                do_log("Could not open config file.");
                printf("Error occured while opening config file.");
        }*/
	
	 char* data42 = get_simmis_data_from_table("EXCEPTION_LIST_exception_list","");
                        if(data42[0]==0 || data42==NULL){
                        do_log("Could not get config data42");
                        printf("Error occured while getting config data.");}
                        strcpy(exception,data42);





	//do_log(exception);
	
	// Canceled trips : BEGIN
	/*
	sprintf(command,"select c.username,clientname,companyname,sales_person,reseller_sp,case_source,mis_user,status,c.sim_phone_no,booking_date,from_date,to_date,payment_mode,creditcard_no,expdate,country,c.handset_model,s.add_city from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and status = 'Canceled' and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') and clientname !~* '%s' order by booking_date",fromdt,todt,exception);
	do_log(command);
	count=db->ExecTuplesOk(command);
	if (count>0)
	{
		printf("\n<br><b><u>Canceled Trips</u><b><br>");
		printf("\n<br>");
		printf("\n<table width=100%% border=0>");
		printf("\n<tr class=Mh><th>Sl. No.</th><th>User Id</th><th>Client Name</th><th>Company Name</th><th>Sales Person / Reseller</th><th>Reseller Sales Person</th><th>Case Source</th><th>MIS User</th><th>Sim Card Status</th><th>Sim Phone No.</th><th>Booking Date</th><th>Trip Start Date</th><th>Trip End Date</th><th>Payment Mode</th><th>Creditcard No.</th><th>Creditcard ExpDate</th><th>Country</th><th>Handset</th><th>City</th></tr>");
		for(i=0;i<count;i++){
			printf("\n<tr class=Ml>");
			printf("<td>%d</td>",i+1);
			char *c=db->get_field(i,"username");
			if((c==NULL)||c[0]==0)
				printf("<td>NA</td>");
			else 
				printf("<td>%s</td>",db->get_field(i,"username"));
			c=db->get_field(i,"clientname");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%s</td>",db->get_field(i,"clientname"));
			c=db->get_field(i,"companyname");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%s</td>",db->get_field(i,"companyname"));
			c=db->get_field(i,"sales_person");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%s</td>",db->get_field(i,"sales_person"));
			c=db->get_field(i,"reseller_sp");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%s</td>",db->get_field(i,"reseller_sp"));
			c=db->get_field(i,"case_source");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%s</td>",db->get_field(i,"case_source"));
			c=db->get_field(i,"mis_user");
			if((c==NULL)||c[0]==0)
				printf("<td>NA</td>");
			else
				printf("<td>%s</td>",db->get_field(i,"mis_user"));
			c=db->get_field(i,"status");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%s</td>",db->get_field(i,"status"));
			c=db->get_field(i,"sim_phone_no");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%s</td>",db->get_field(i,"sim_phone_no"));
			c=db->get_field(i,"booking_date");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
			{
				char yyyymmdd[30];
                                char ddmmyyyy[30];
                                strcpy(yyyymmdd,db->get_field(i,"booking_date"));
                                printf("<td>%s</td>",change_date_format(yyyymmdd,ddmmyyyy));
                                //printf("<td>%s</td>",db->get_field(i,"booking_date"));
			}
			c=db->get_field(i,"from_date");
			if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
			{
				char yyyymmdd[30];
                                char ddmmyyyy[30];
                                strcpy(yyyymmdd,db->get_field(i,"from_date"));
                                printf("<td>%s</td>",change_date_format(yyyymmdd,ddmmyyyy));
                                //printf("<td>%s</td>",db->get_field(i,"from_date"));
			}
			c=db->get_field(i,"to_date");
			 if((c==NULL)||c[0]==0)
                                 printf("<td>NA</td>");
                         else
			 {
				char yyyymmdd[30];
                                char ddmmyyyy[30];
                                strcpy(yyyymmdd,db->get_field(i,"to_date"));
                                printf("<td>%s</td>",change_date_format(yyyymmdd,ddmmyyyy));	
                                //printf("<td>%s</td>",db->get_field(i,"to_date"));
			 }
			c=db->get_field(i,"payment_mode");
			if((c==NULL)||c[0]==0)
				printf("<td>NA</td>");
			else
			{
				if(!strcmp(db->get_field(i,"payment_mode"),"Advanced Payment"))
                                {
                                        sprintf(command,"select amount from payment_discount where trip_id=(select tripid from clienttrip where username ='%s' and booking_date>='%s' and booking_date<=timestamp_pl_interval('%s','86400') order by booking_date desc limit 1) and sim_card_type='country'",db->get_field(i,"username"),fromdt,todt);
                                        if(db1.ExecTuplesOk(command))
                                                printf("<td>Advance Payment: %s $</td>",db1.get_field(0,"amount"));
                                        else
                                                printf("<td>Advance Payment</td>");
                                }
                                else
                                        printf("<td>%s</td>",db->get_field(i,"payment_mode"));
			}
			c=db->get_field(i,"creditcard_no");
			 if((c==NULL)||c[0]==0)
                                 printf("<td>NA</td>");
                         else{
				if(can_view_cc)//if (isAdmin)
                                 	printf("<td>%s</td>",db->get_field(i,"creditcard_no"));
				else{
					char cc_num[31];
					char cc_show_str[31];
					strcpy(cc_num,db->get_field(i,"creditcard_no"));
					printf("<td>%s</td>",hide_cc_number(cc_num,cc_show_str));
				}
			}
			c=db->get_field(i,"expdate");
			 if((c==NULL)||c[0]==0)
                                 printf("<td>NA</td>");
                         else
			{
                                char yyyymmdd[30];
                                char ddmmyyyy[30];
                                strcpy(yyyymmdd,db->get_field(i,"expdate"));
                                printf("<td>%s</td>",change_date_format(yyyymmdd,ddmmyyyy));
                                //printf("<td>%s</td>",db->get_field(i,"expdate"));
			}
			c=db->get_field(i,"country");
			 if((c==NULL)||c[0]==0)
                                 printf("<td>NA</td>");
                         else
                                 printf("<td>%s</td>",db->get_field(i,"country"));
			c=db->get_field(i,"handset_model");
			 if((c==NULL)||c[0]==0)
                                 printf("<td>NA</td>");
                         else
                                 printf("<td>%s</td>",db->get_field(i,"handset_model"));
			c=db->get_field(i,"add_city");
			 if((c==NULL)||c[0]==0)
                                 printf("<td>NA</td>");
                         else
                                 printf("<td>%s</td>",db->get_field(i,"add_city"));
			
			printf("</tr>");
		}
		printf("\n</table>"); 
	}
	*/
	// Canceled trips : END


	/*sprintf(command,"select sales_person,count(*) from clienttrip c,sim_user s, sim_stock st where c.username=s.username and st.sim_phone_no=c.sim_phone_no and booking_date>='%s' and booking_date<timestamp_pl_interval('%s','86400') group by sales_person",fromdt,todt);

	do_log(command);
        count=db->ExecTuplesOk(command);
        if (count>0){
                printf("\n<br><b><u>Sim Sales Summary</u><b><br>");
                printf("\n<br>");
                printf("\n<table width=30%% border=0>");
		printf("\n<tr class=Mh><th>Sl. No.</th><th>Sales Person</th><th>Total Sales</th></tr>");
		for(i=0;i<count;i++){
                        printf("\n<tr class=Ml>");
                        printf("<td>%d</td>",i+1);
                        char *c=db->get_field(i,"sales_person");
                        if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%s</td>",db->get_field(i,"sales_person"));
                        c=db->get_field(i,"count");
                        if((c==NULL)||c[0]==0)
                                printf("<td>NA</td>");
                        else
                                printf("<td>%f</td>",atof(db->get_field(i,"count")));
			printf("</tr>");
		}
		printf("\n</table>");
	}*/
	return 0;
}

int draw_global_upg_rep(){
	char command[1024];
        fax_database db(E2FAXDB,DBSERV_R);
        int i;
	char *fromdt=cgi_getentrystr("fdt");
	char *todt=cgi_getentrystr("tdt");
	sprintf(command,"select c.username,clientname,companyname,sales_person,status,c.sim_phone_no,upgrade_done_on from clienttrip c,sim_user s,globalsim_upgrade_details g where c.username=s.username and g.sim_phone_number=c.sim_phone_no and status not like 'Canceled' and status not like 'Returned' and upgrade_done_on>='%s' and upgrade_done_on<timestamp_pl_interval('%s','86400') order by upgrade_done_on",fromdt,todt);
	do_log(command);
	int count=db.ExecTuplesOk(command);
	if (count>0){
	   char *dwnldupg = cgi_getentrystr("dwnldupg");
	   if (dwnldupg == NULL || dwnldupg[0]==0){
		printf("Content-type:text/html\n\n");
		printf("\n<br><b><u>Upgrade Summary</u><b><br>");
		printf("\n<br>");
		printf("\n<table width=100% border=0>");
		//printf("\n<tr class=Mh><th>User Id</th><th>Client Name</th><th>Company Name</th><th>Sales Person</th><th>Sim Card Status</th><th>Sim Phone No.</th><th>Booking Date</th><th>Trip Start Date</th><th>Trip End Date</th><th>Creditcard No.</th><th>Creditcard ExpDate</th><th>Country</th></tr>");
		printf("\n<tr class=Mh><th>User Id</th><th>Client Name</th><th>Company Name</th><th>Sales Person</th><th>Sim Phone No.</th><th>Upgrade done on</th></tr>");
		for(i=0;i<count;i++){
			printf("\n<tr class=Ml>");
			char *c=db.get_field(i,"username");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else 
				printf("<td>%s</td>",db.get_field(i,"username"));
			c = db.get_field(i,"clientname");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else 
				printf("<td>%s</td>",db.get_field(i,"clientname"));
			c = db.get_field(i,"companyname");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"companyname"));
			c = db.get_field(i,"sales_person");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"sales_person"));
			c = db.get_field(i,"sim_phone_no");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"sim_phone_no"));
			c = db.get_field(i,"upgrade_done_on");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"upgrade_done_on"));
			printf("</tr>");	
		}
		printf("\n</table>");
	   }
	   else{
		dwnldupg = strdupa(dwnldupg);
		if(!strcmp(dwnldupg,"dwnld")){
			char fname[1024];
			sprintf(fname,"sim_upgrade_report%s%s.csv",replacestring(fromdt,"-",""),replacestring(todt,"-",""));
			printf("Content-disposition: attachment; filename=%s\n\n", fname);
			printf("User Id,Client Name,Company Name,Sales Person,Sim Phone No.,Upgrade done on\n");
			for(i=0;i<count;i++){
				printf("%s,%s,%s,%s,%s,%s\n",db.get_field(i,"username"),db.get_field(i,"clientname"),db.get_field(i,"companyname"),db.get_field(i,"sales_person"),db.get_field(i,"sim_phone_no"),db.get_field(i,"upgrade_done_on"));
			
			}
		}

	   } 
	}
	else
		printf("\n<br><center><b>No records present in the specified duration.</b></center><br>");
	return 0;


}
/*
int draw_global_ref_rep(){
	char command[9999];
        fax_database db(E2FAXDB,DBSERV_R);
	fax_database db2(E2FAXDB,DBSERV_R);
        int i;
	char *fromdt=cgi_getentrystr("fdt");
	char *todt=cgi_getentrystr("tdt");
	sprintf(command,"SELECT p.username,s.clientname,s.companyname,s.sales_person,sum(p.amount) as paid,max(p.accept_time) as date,(select sum(amount) from global_refill where username=p.username and accept_time>='%s' and accept_time <'%s' ) as refill from payment_discount p,sim_user s where p.username=s.username and p.username in (SELECT distinct username from global_refill where accept_time >='%s' and accept_time <'%s') and accept_time >='%s' and accept_time <'%s' and p.sim_card_type='global' and p.type !='Credit Card' group by p.username,s.clientname,s.companyname,s.sales_person order by date",fromdt,todt,fromdt,todt,fromdt,todt);
	do_log(command);
	int count_refill = db.ExecTuplesOk(command);

	fax_database db1(E2FAXDB,DBSERV_R);
	//sprintf(command," select c.username,clientname,companyname,sales_person,c.sim_phone_no,g.amount,g.accept_time from clienttrip c,sim_user s,payment_discount g  where c.username=s.username and g.trip_id=c.tripid and c.status not like 'Canceled' and c.status not like 'Returned' and c.tripid like 'GLOBAL%%' and g.accept_time>='%s' and g.accept_time < timestamp_pl_interval('%s','86400') and g.type='Credit Card' order by g.accept_time",fromdt,todt);
	sprintf(command," select c.username,clientname,companyname,sales_person,c.sim_phone_no,g.amount,g.accept_time from clienttrip c,sim_user s,payment_discount g  where c.username=s.username and g.trip_id=c.tripid and c.status not like 'Canceled' and c.status not like 'Returned' and g.sim_card_type='global' and g.accept_time>='%s' and g.accept_time < timestamp_pl_interval('%s','86400') and g.type='Credit Card' order by g.accept_time",fromdt,todt);
	do_log(command);
	int count_cc = db1.ExecTuplesOk(command);
	char *dwnldref = cgi_getentrystr("dwnldref");
	if (dwnldref == NULL || dwnldref[0]==0){
		printf("Content-type:text/html\n\n");
		printf("\n<br><b><u>Refill Summary</u><b><br>");
		printf("\n<br>");
		printf("\n<table width=100% border=0>");
		printf("\n<tr class=Mh><th>User Id</th><th>Client Name</th><th>Company Name</th><th>Sales Person</th><th>Sim Phone No.</th><th>Amount</th><th>Refill type</th><th>Date</th><th>Paid status</th></tr>");
		printf("<div id=refill_det style=\"width=350px;height:50px;display:none;background-color:yellow;position:absolute;top:35%%;left:35%%\"></div>\n");
	   if (count_refill >0){
		for(i=0;i<count_refill;i++){
			printf("\n<tr class=Ml>");
			char *c=db.get_field(i,"username");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else 
				//printf("<td>%s</td>",db.get_field(i,"username"));
				printf("<td><a href=\"javascript:show_refill('%s','%s','%s');\">%s</a></td>",db.get_field(i,"username"),fromdt,todt,db.get_field(i,"username"));
			c = db.get_field(i,"clientname");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else 
				printf("<td>%s</td>",db.get_field(i,"clientname"));
			c = db.get_field(i,"companyname");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"companyname"));
			c = db.get_field(i,"sales_person");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"sales_person"));*/
			/*
			c = db.get_field(i,"sim_phone_no");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"sim_phone_no"));
			*/
/*			printf("<td>---</td>");
			c = db.get_field(i,"refill");
			if((c==NULL)||c[0]==0)
				printf("<td>0</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"refill"));
			printf("<td>Refill</td>");
			c = db.get_field(i,"date");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"date"));
			c = db.get_field(i,"paid");
			if((c==NULL)||c[0]==0)
				printf("<td>0</td>");
			else
				printf("<td>%s</td>",db.get_field(i,"paid"));
			printf("</tr>");	
		}
	   }
	   if (count_cc >0){
		for(i=0;i<count_cc;i++){
			printf("\n<tr class=Ml>");
			char *c=db1.get_field(i,"username");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else 
				printf("<td>%s</td>",db1.get_field(i,"username"));
			c = db1.get_field(i,"clientname");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else 
				printf("<td>%s</td>",db1.get_field(i,"clientname"));
			c = db1.get_field(i,"companyname");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db1.get_field(i,"companyname"));
			c = db1.get_field(i,"sales_person");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db1.get_field(i,"sales_person"));
			c = db1.get_field(i,"sim_phone_no");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db1.get_field(i,"sim_phone_no"));
			c = db1.get_field(i,"amount");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db1.get_field(i,"amount"));
			printf("<td>Credit Card</td>");
			c = db1.get_field(i,"accept_time");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db1.get_field(i,"accept_time"));
			c = db1.get_field(i,"amount");
			if((c==NULL)||c[0]==0)
				printf("<td>No Data</td>");
			else
				printf("<td>%s</td>",db1.get_field(i,"amount"));
			printf("</tr>");	
		}
	   }
	   printf("\n</table>");
	}
	else{
		dwnldref = strdupa(dwnldref);
		if(!strcmp(dwnldref,"dwnld")){
			if (count_cc >0 || count_refill >0){
				char fname[1024];
				sprintf(fname,"globalsim_refil_report%s%s.csv",replacestring(fromdt,"-",""),replacestring(todt,"-",""));
				printf("Content-disposition: attachment; filename=%s\n\n", fname);
				printf("User Id,Client Name,Company Name,Sales Person,Sim Phone No.,Amount,Refill type,Date,Paid status\n");
				for(i=0;i<count_refill;i++){
					//sprintf(command,"SELECT (select sum(amount) from global_refill where username='%s' and accept_time >= '%s' and accept_time < timestamp_pl_interval('%s','86400')) as amount , (select sum(amount) from payment_discount where username='%s' and accept_time >= '%s' and accept_time < timestamp_pl_interval('%s','86400') and type !='Credit Card') as pamount ,(select max(accept_time) from global_refill where username='%s' and accept_time >= '%s' and accept_time < timestamp_pl_interval('%s','86400') ) as ldate",db.get_field(i,"username"),fromdt,todt,db.get_field(i,"username"),fromdt,todt,db.get_field(i,"username"),fromdt,todt);
					sprintf(command,"SELECT (select sum(amount) from global_refill where username='%s' and accept_time >= '%s' and accept_time < timestamp_pl_interval('%s','86400') and sim_phone_no='%s') as amount , (select sum(amount) from payment_discount where username='%s' and accept_time >= '%s' and accept_time < timestamp_pl_interval('%s','86400') and type !='Credit Card' and trip_id in (select tripid from clienttrip where username='%s' and sim_phone_no='%s' and status !='Canceled')) as pamount ,(select max(accept_time) from global_refill where username='%s' and accept_time >= '%s' and accept_time < timestamp_pl_interval('%s','86400') and sim_phone_no='%s' ) as ldate",db.get_field(i,"username"),fromdt,todt,db.get_field(i,"sim_phone_no"),db.get_field(i,"username"),fromdt,todt,db.get_field(i,"username"),db.get_field(i,"sim_phone_no"),db.get_field(i,"username"),fromdt,todt,db.get_field(i,"sim_phone_no"));
					db2.ExecTuplesOk(command);
					char *c=db2.get_field(0,"amount");
					if (!(c == NULL || c[0] == 0))
						printf("%s,%s,%s,%s,%s,%s,%s,%s,%s\n",db.get_field(i,"username"),db.get_field(i,"clientname"),db.get_field(i,"companyname"),db.get_field(i,"sales_person"),db.get_field(i,"sim_phone_no"),db2.get_field(0,"amount"),"Refill",db2.get_field(0,"ldate"),db2.get_field(0,"pamount"));
				}
				for(i=0;i<count_cc;i++){
					printf("%s,%s,%s,%s,%s,%s,%s,%s,%s\n",db1.get_field(i,"username"),db1.get_field(i,"clientname"),db1.get_field(i,"companyname"),db1.get_field(i,"sales_person"),db1.get_field(i,"sim_phone_no"),db1.get_field(i,"amount"),"Credit Card",db1.get_field(i,"accept_time"),db1.get_field(i,"amount"));
				}
			}
		}
	}
	return 0;
}*/


int draw_simstatus_page(fax_database *db){
	char command[1024];
        //fax_database db(E2FAXDB,DBSERV_R);
        int i;		
	time_t t = time(0);
	struct tm *tdate = localtime(&t);
	draw_header(3);
	printf("\n<div id=mh style=""width:99%;height:95%"">");
	printf("\n<form name=frmsimstatus action=simmisn.e method=post>");
	printf("\n<br><br><label>Select Country:</label>");
	sprintf(command,"select distinct country from sim_stock order by country;");
	do_log(command);
        int count=db->ExecTuplesOk(command);
     	printf("\n &nbsp &nbsp <select name=selcountry id=selcountry onchange=\"new net.ContentLoader('simmisn.e?a=getsimno&b='+this.value,parseMyData,'dsimno',null,'GET');\">");
               	printf("<option value=none>Select</option>");
	if (count>0){
               	for (i=0;i<count;i++)
               	{
                       	printf("<option value='%s'>%s</option>",db->get_field(i,"country"),db->get_field(i,"country"));
               	}
        }
	printf("</select>");
	printf("\n<br><label>Sim Phone No:</label>");
	printf("\n<div id=\"dsimno\">");
	printf(" &nbsp &nbsp <select name=selphone id=selphone></select><br>");
	printf("</div>");
	printf(" OR <br><label>Enter Sim Phone No:</label> &nbsp &nbsp <input type=text name=txtselphone id=txtselphone onfocus=\"locksel('selphone','txtselphone');\">");
	printf("\n<input type=hidden name=a>");
        printf("\n<input type=hidden name=b >");
	printf("\n<br><input type=submit name=sbtstatus value=\"Get Status\" onclick=\"return getMultiple(document.frmsimstatus.selphone);\"><br>");
	printf("\n</form>");
	printf("\n</div>");
	draw_end();
	return 0;
}
int add_ddno(fax_database *db){
	char command[1024];
        //fax_database db(E2FAXDB,DBSERV);
	char *user=cgi_getentrystr("b");
	char *subno=cgi_getentrystr("c");
	char *destno=cgi_getentrystr("d");
	char *name=cgi_getentrystr("e");
	char *criteria=cgi_getentrystr("f");
	criteria=strdupa(criteria);
	char *domain;
	if(name==NULL||name[0]==0)
	name="";
	else
	name=strdup(name);
	sprintf(command,"select * from sim_user where username='%s'",user);
	do_log(command);
	if(db->ExecTuplesOk(command)>0)
		domain=strdupa(db->get_field(0,"domainname"));
	if(!strcmp(criteria,"add")){
		sprintf(command,"select * from cb_addressbook where destination='%s' and username='%s'",destno,user);
		if(db->ExecTuplesOk(command)){
			do_log("destination already present");
			printf("Error!!! destination already present");
			return 0;
		}
		sprintf(command,"select * from cb_addressbook where acc_num='%s' and username='%s'",subno,user);
		if(db->ExecTuplesOk(command)){
			printf("Error!! Substitue number already present");
			do_log("Substitue number already present");
			return 0;
		}
		//sprintf(command,"insert into cb_addressbook(acc_num,destination,username,domainname,name)values('%s','%s','%s','%s','%s');",subno,destno,user,domain,name);
		sprintf(command,"insert into cb_addressbook(acc_num,destination,username,domainname,name,dateandtime)values('%s','%s','%s','%s','%s','now');",subno,destno,user,domain,name);
		do_log(command);
		if(db->ExecCommandOk(command)<=0)
			printf("Error");
		else
			 printf("done");
	}
	else if(!strcmp(criteria,"delete")){
		sprintf(command,"delete from cb_addressbook where username='%s' and acc_num='%s' and domainname='%s' and destination='%s'",user,subno,domain,destno);
               	do_log(command);
               	if(db->ExecCommandOk(command)<=0)
                       	printf("<b>     An error occured while deleting deep discount number.</b>");
               	else
			printf("done");
	}
	else if(!strcmp(criteria,"edit")){
		printf("<br><hr>");
		printf("<br>    <b><u>Edit Deep Discount Numbers</u></b><br>");
               	printf("<br>    Name:<input type=text name=txtename id=\"txtename\" value='%s'>",name);
               	printf("    Destination Number:<input type=text name=txtedestination id=\"txtedestination\" value=%s>",destno);
               	printf("<input type=button name=btnsave value=Save onclick=\"saveddno(%s,%s,document.getElementById('txtedestination').value,document.getElementById('txtename').value);\">  <input type=button name=btncancel value=Cancel onclick=\"Javascript:canceledit(%s);\">",user,subno,user);
	}
	else if(!strcmp(criteria,"save")){
		sprintf(command,"select * from cb_addressbook where destination='%s' and username='%s'",destno,user);
		do_log(command);
		if(db->ExecTuplesOk(command)<=0){
	 		sprintf(command,"update cb_addressbook set name='%s',destination='%s',dateandtime='now' where username='%s' and acc_num='%s' and domainname='%s'",name,destno,user,subno,domain);
                	do_log(command);
                	if(db->ExecCommandOk(command)<=0){
                           	printf("<b>     An error occured while updating deep discount number.</b>");
                           	do_log("deep discount number updated successfully");
                	}
                	else
                 	          do_log("deep discount number updated successfully");
		}
		//else printf("<b>The destination %s already present",destno); 
		else{ //check if name needs to be updated
			char *tmpname=db->get_field(0,"name");
			if (!(tmpname == NULL || tmpname[0] == 0)) tmpname = strdupa(tmpname);
			else tmpname ="";
			if (strcmp(tmpname,name)){
	 		sprintf(command,"update cb_addressbook set name='%s',dateandtime='now' where username='%s' and acc_num='%s' and domainname='%s'",name,user,subno,domain);
                	do_log(command);
                	if(db->ExecCommandOk(command)<=0){
                           	printf("<b>     An error occured while updating deep discount number.</b>");
                           	do_log("deep discount number updated successfully");
                	}
                	else
                 	          do_log("deep discount number updated successfully");
				
			}
		}
		//else printf("<b>The destination %s already present",destno); 
        }
	return 0;
}
void status_alert(char *simno){
        char semail[1024];
	char message[1024],subject[1024];
	char mailto[1024];
        sprintf(mailto,"");
	sprintf(subject,"Sim Card Status Alert");
	sprintf(message,"Hello,");
	sprintf(message,"%s\n         Status of sim phone no: %s has been updated to Delivered without setting any deep discount numbers.",message,simno);
	int mailforsale=0;
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
                do_log("Could not open config file.");
                printf("Error occured while opening config file.");
        }
        else
                //mailforsale=send_dd_mail(mailto,message,subject);
                mailforsale=send_mail(mailto,message,subject);*/

	     char* data43 = get_simmis_data_from_table("MAILTO_mailto","");
                if(data43[0]==0 || data43==NULL)
                {
                     do_log("ERROR OCCURED WHILE OPENING EMAIL_TO_CUSTOMER file data43");
                     printf("ERROR OCCURED WHILE OPENING THE EMAIL_TO_CUSTOMER FILE");
                }
                strcpy(mailto,data43);
                mailforsale=send_mail(mailto,message,subject);





	
}
int creditcard_alert(fax_database *db,const char *userid,const char *simno,const char *bdate){
        char message[1024];
        char subject[1024];
        char emailid[1024];
        char command[1024];
	char mailto[1024];
	sprintf(mailto,"");
        //fax_database db(E2FAXDB,DBSERV_R);
	char *bal_due;
	//sprintf(command,"SELECT coalesce((SELECT abs((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT closingbalance as bal from billdetails where username ='%s' order by generatedon desc limit 1)X where X.bal < 0)) as numeric), 2)))),0) as bal",userid);
	//sprintf(command,"SELECT coalesce((SELECT abs((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT (SELECT sum(amount)  from payment_discount p where p.username ='%s' and sim_card_type='country' and accept_time >(select max(generatedon) from billdetails where username='%s')) - (SELECT abs((SELECT closingbalance from billdetails where username='%s' order by generatedon desc limit 1))) as bal)X where X.bal < 0)) as numeric), 2)))),0) as bal",userid,userid,userid);
	sprintf(command,"SELECT coalesce(((SELECT round( CAST(float8 ((SELECT X.bal from (SELECT (SELECT sum(amount)  from payment_discount p where p.username ='%s' and sim_card_type='country' and accept_time >(select max(generatedon) from billdetails where username='%s')) + (SELECT (SELECT closingbalance from billdetails where username='%s' order by generatedon desc limit 1)) as bal)X where X.bal < 0)) as numeric), 2))),0) as bal",userid,userid,userid);
        do_log(command);
        if(db->ExecTuplesOk(command)>0)
        {
                bal_due=db->get_field(0,"bal");
                do_log(bal_due);
                if(bal_due==NULL || bal_due[0]==0)
                        bal_due="";
                else
                        bal_due=strdup(bal_due);
        }
        sprintf(command,"select * from sim_user where username='%s'",userid);
        do_log(command);
        if(db->ExecTuplesOk(command)<=0){
                do_log("Could not find the user.");
                return 0;
        }
        char *cname=db->get_field(0,"clientname");
        if(cname==NULL||cname[0]==0)
                cname="";
        else
                cname=strdup(cname);
        char *telno=db->get_field(0,"telno");
        if(telno==NULL||telno[0]==0)
                telno="";
        else
                telno=strdup(telno);
        char *mobno=db->get_field(0,"mobno");
        if((mobno==NULL)||(mobno[0]==0))
                mobno="";
        else
                mobno=strdup(mobno);
        char *sales=db->get_field(0,"sales_person");
        if(sales==NULL||sales[0]==0)
                sales="";
        else
                sales=strdup(sales);
        char *email=db->get_field(0,"emailadd");
        if(email==NULL||email[0]==0)
                email="";
        else
                email=strdup(email);
	do_log("<mail>");
	char *unlimited_calling=cgi_getentrystr("uc");
	char *uc_days=cgi_getentrystr("uc_op");
	do_log(uc_days);
	if (!(unlimited_calling == NULL || unlimited_calling[0] == 0))
	{
		if(!strcmp(unlimited_calling,"uc"))
			strcpy(unlimited_calling,"Yes");
	}
	do_log(unlimited_calling);
	do_log(uc_days);
        sprintf(message,"Following is the details of sim card which was booked without credit card details.\n");
        sprintf(message,"%s\nUser ID:%s",message,userid);
        sprintf(message,"%s\nClient Name:%s",message,cname);
        sprintf(message,"%s\nPhone No.:%s",message,telno);
        sprintf(message,"%s\nMobile No.:%s",message,mobno);
	sprintf(message,"%s\nDate Of Booking:%s",message,bdate);
        sprintf(message,"%s\nRegistered Phone No.:%s",message,simno);
        sprintf(message,"%s\nEmail ID:%s",message,email);
        sprintf(message,"%s\nSales Person:%s",message,sales);
	//if(strcmp(bal_due,"0"))//if(bal_due!=NULL)
	if (atof(bal_due)<0)
                sprintf(message,"%s \nPayment Overdue:%s",message,bal_due);
	 if (!(unlimited_calling == NULL || unlimited_calling[0] == 0)){
		sprintf(message,"%s\nUnlimited Calling:%s",message,unlimited_calling);
		//sprintf(message,"%s\nUnlimited Calling Duration:%s Days",message,uc_days);
	}
	else
		sprintf(message,"%s\nUnlimited Calling:No",message);
	//do_log(message);
        sprintf(subject,"Sim card booked without credit card details.");
        char *saleeid=strtok(sales,"/");
        char semail[1024];
        char tmp_email[1024];
        strcpy(tmp_email,"");
        get_sim_sales_email(saleeid,tmp_email);
        if (strlen(tmp_email)==0)
		sprintf(semail,"%s@tsim.in",saleeid);
	   else{
	   	sprintf(semail,"%s",tmp_email);
	   	strcpy(tmp_email,"");
	   }
        do_log((char *)semail);
	int mailforsale;
        while(saleeid != NULL)
        {
                saleeid = strtok(NULL, "/");
                if((saleeid!=NULL)){
                	get_sim_sales_email(saleeid,tmp_email);
                	if (strlen(tmp_email)==0)
					sprintf(semail,"%s,%s@tsim.in",semail,saleeid);
				else{
					sprintf(semail,"%s,%s",semail,tmp_email);
					strcpy(tmp_email,"");
				}
                    do_log((char *)semail);
                }
        }
	//mailforsale=send_dd_mail(semail,message,subject);
	mailforsale=send_mail(semail,message,subject);
/*	if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
                do_log("Could not open config file.");
                printf("Error occured while opening config file.");
        }
        else
		//mailforsale=send_dd_mail(mailto,message,subject);
		mailforsale=send_mail(mailto,message,subject);*/
	
   mailforsale=send_mail(semail,message,subject);
                char* data44 = get_simmis_data_from_table("MAILTO_mailto","");
                if(data44[0]==0 || data44==NULL)
                {
                     do_log("ERROR OCCURED WHILE OPENING EMAIL_TO_CUSTOMER file data44");
                     printf("ERROR OCCURED WHILE OPENING THE EMAIL_TO_CUSTOMER FILE");
                }
                strcpy(mailto,data44);
                mailforsale=send_mail(mailto,message,subject);
	





	do_log("</mail>");
        return mailforsale;
}
int advancebooking_alert(fax_database *db,const char *userid,const char *country,const char *tsdate,const char *tedate,const char *tddate){
	char message[1024];
        char subject[1024];
        char emailid[1024];
        char command[1024];
        char mailto[1024];
        sprintf(mailto,"");
        //fax_database db(E2FAXDB,DBSERV_R);
        sprintf(command,"select * from sim_user where username='%s'",userid);
        do_log(command);
        if(db->ExecTuplesOk(command)<=0){
                do_log("Could not find the user.");
                return 0;
        }
        char *cname=db->get_field(0,"clientname");
        if(cname==NULL||cname[0]==0)
                cname="";
        else
                cname=strdup(cname);
        char *telno=db->get_field(0,"telno");
        if(telno==NULL||telno[0]==0)
                telno="";
        else
                telno=strdup(telno);
        char *mobno=db->get_field(0,"mobno");
        if((mobno==NULL)||(mobno[0]==0))
                mobno="";
        else
                mobno=strdup(mobno);
        char *sales=db->get_field(0,"sales_person");
        if(sales==NULL||sales[0]==0)
                sales="";
        else
                sales=strdup(sales);
        char *email=db->get_field(0,"emailadd");
        if(email==NULL||email[0]==0)
                email="";
        else
                email=strdup(email);
        do_log("<mail>");
	sprintf(message,"Hello,\n    There was a request for a %s card from userid id:%s,but sim card for %s is not available.\n",country,userid,country);
	sprintf(message,"%s\nPlease make arrangement for the specified card and book the trip manualy.",message);
        sprintf(message,"%s\nDetails of the trip is as follows:.\n",message);
        sprintf(message,"%s\nUser ID:%s",message,userid);
        sprintf(message,"%s\nClient Name:%s",message,cname);
        sprintf(message,"%s\nPhone No.:%s",message,telno);
	sprintf(message,"%s\nMobile No.:%s",message,mobno);
	sprintf(message,"%s\nCountry:%s",message,country);
	sprintf(message,"%s\nTrip start date:%s",message,tsdate);
        sprintf(message,"%s\nTrip End Date:%s",message,tedate);
	sprintf(message,"%s\nDelivery Date:%s",message,tddate);
        sprintf(message,"%s\nEmail ID:%s",message,email);
        sprintf(message,"%s\nSales Person:%s",message,sales);
        sprintf(subject,"Sim Card Not Available For Booking.");
	int mailforsale;
/*        if((get_tag_value(CONFIG_DATA_FILE,"<!-- MAILTO","--MAILTO>","mailto",mailto)!=1) || (mailto==NULL || mailto[0]==0)){
                do_log("Could not open config file.");
                printf("Error occured while opening config file.");
        }
        else
                //mailforsale=send_dd_mail(mailto,message,subject);
                mailforsale=send_mail(mailto,message,subject);*/

	  char* data45 = get_simmis_data_from_table("MAILTO_mailto","");
                if(data45[0]==0 || data45==NULL)
                {
                     do_log("ERROR OCCURED WHILE OPENING EMAIL_TO_CUSTOMER file data45");
                     printf("ERROR OCCURED WHILE OPENING THE EMAIL_TO_CUSTOMER FILE");
                }
                strcpy(mailto,data45);
                mailforsale=send_mail(mailto,message,subject);






        do_log("</mail>");
        return mailforsale;
}

/*
int advance_booking(fax_database *db){
	printf("Advance_booking");
	char command[1024];
    	int i;
	//fax_database db(E2FAXDB,DBSERV);
    	fax_database del(E2FAXDB,DBSERV);			
	sprintf(command,"select current_date+1 as ptime;");
	do_log(command);
        char *bdate;
        if(db->ExecTuplesOk(command)>0)
               bdate=db->get_field(0,"ptime");
        if(bdate==NULL||bdate[0]==0)
                bdate=" ";
        else
                bdate=strdup(bdate);
        sprintf(command,"select * from advance_booking where simcard_delivery_date='%s'",bdate);
        do_log(command);
        int count=db->ExecTuplesOk(command);
        if(count<=0)
              return 0;
        for(i=0;i<count;i++){
              do_log("start");
              char *username=db->get_field(i,"username");
              char *country=db->get_field(i,"country");
              char *email=db->get_field(i,"emailid");
              char *tsdate=db->get_field(i,"trip_startdate");
              char *tedate=db->get_field(i,"trip_enddate");
              char *tddate=db->get_field(i,"simcard_delivery_date");
              char *bid=db->get_field(i,"booking_id");
              sprintf(command,"select * from sim_stock where country='%s' and active=true and availability=true;",country);
              if(del.ExecTuplesOk(command)<=0){
                      do_log("Card not available");
                      advancebooking_alert(db,username,country,tsdate,tedate,tddate);
		      sprintf(command,"delete from advance_booking where booking_id='%s'",bid);
                      do_log(command);
                      if((del.ExecCommandOk(command))>0){
                               do_log("Entry deleted from advance booking table");
                      }

               }
               else{
                      sprintf(command,"/usr/bin/lynx -source -connect_timeout=60 'http://mail.tsim.in/simmisn.e?l=admin&p=chair&a=done&b=%s&c=%s&d=%s&selsimno=none&advance=off&tsdate=%s&tedate=%s&tddate=%s&advid=%s'",replacestring(country," ","%20"),username,email,replacestring(tsdate," ","%20"),replacestring(tedate," ","%20"),replacestring(tddate," ","%20"),bid);
                       do_log(command);
                       FILE *fp=popen(command,"r");
                       if(fp==NULL) {
                                   do_log("Error 101: Connection with main ownpages server failed.");
                                    break;
                       }
                       char line[1024];
                       if(!fgets(line,sizeof(line),fp)) {
                               pclose(fp);
                               do_log("Error 102: Main ownpages server response contains error");
                               break;
                	 }
                        if(line==NULL || line[0]==0) {
                                do_log("Error 103: Main ownpages server response contains error");
                                pclose(fp);
                                break;
                         }
			
                         while(fgets(line,sizeof(line),fp)){
                                 if(strstr(line,"Sim card booking failed")) {
                                         do_log("Sim card booking failed");
                                         booking_alert(db,username,"fail",bdate,country);
                                         do_log(line);
                                         pclose(fp);
                                         break;
                                  }
                                  else if(strstr(line,"Sim card booking done")){
                                          do_log("Sim card booking done");
                                          do_log(line);
                                          sprintf(command,"delete from advance_booking where booking_id='%s'",bid);
                                          do_log(command);
                                          if((del.ExecCommandOk(command))>0){
                                                    do_log("Entry deleted from advance booking table");
                                           }
                                           pclose(fp);
                                           break;
                                     }
                            }
                            do_log("move next");
                     }
          }    
}
*/

int send_mail(char *toaddr,char *msg,char *subj,int savetosentfolder){
    FILE *pp;
    char prog_files[9999];
    strcpy (prog_files, SENDMAIL_PROG_ARGS);
    strcat (prog_files, " ");
    strcat (prog_files, "-f");
    strcat (prog_files, MAIL_FROM); //check this
    strcat (prog_files, " ");
    strcat (prog_files, toaddr);
    if ((pp = popen(prog_files, "w")) == NULL) {
        do_log("Sending email failed");
        return 0;
    }
    fprintf (pp, "From: TSIM Team<%s>\n", MAIL_FROM);
    fprintf (pp, "To: %s\n", toaddr);
    fprintf (pp, "Subject: %s\n",subj);
    //if (sendashtml)     fprintf (pp, "Content-type: text/html");
    fprintf (pp,"%s\n\n",msg);
    pclose(pp);
//Sending to Sent Folder
    if (savetosentfolder){
          strcpy (prog_files, SENDMAIL_PROG_ARGS);
          strcat (prog_files, " ");
          strcat (prog_files, "-f");
          strcat (prog_files, MAIL_FROM); //check this
          strcat (prog_files, " ");
          strcat (prog_files, MAIL_FROM);
          if ((pp = popen(prog_files, "w")) == NULL) {
              do_log("Sending email failed");
              return 0;
          }
          fprintf (pp, "%s",SAVE_TO_SENT_FLDR);
          fprintf (pp, "From: TSIM Team<%s>\n", MAIL_FROM);
          fprintf (pp, "To: %s\n", toaddr);
          fprintf (pp, "Subject: %s\n",subj); 
          fprintf (pp,"%s\n\n",msg);
          pclose(pp);
     }
     return 1;
}
/*
char *urlencode(char *msg){
int c;
        char *hex = "0123456789abcdef";
        char output[1024];
        char *ptr;
        ptr = msg;
        int ctr=0;
        while( *ptr !=0){
                c = *ptr;
                if( ('a' <= c && c <= 'z')
                || ('A' <= c && c <= 'Z')
                || ('0' <= c && c <= '9') ){
                        //putchar(c);
                        output[ctr++]=c;
                } else {
                        //putchar('%');
                        //putchar(hex[c >> 4]);
                        //putchar(hex[c & 15]);
                        output[ctr++]='%';
                        output[ctr++]=hex[c >> 4];
                        output[ctr++]=hex[c & 15];
                }
                ptr++;
        }
        output[ctr]='\0';
        return output;
}
*/
int send_sms(char *msg, char *dest){
//https://www.siptraffic.com/myaccount/sendsms.php?username=trikonelec&password=blhebrgu&from=+919819156389&to=+919870145601&text=BEGIN
        char curlurl[4024];
        //sprintf(curlurl,"%s -dump --connect-timeout 10 \"%s?cmd=%s&at=%s&tx=%s\"",CURL_PATH,SMS_URL,"_notify-synch",PP_TOKEN,tx);
        sprintf(curlurl,"%s --connect-timeout 10 \"%s?username=%s&password=%s&from=%s&to=%s&text=%s\"",CURL_PATH,SMS_URL,SMS_UNAME,SMS_PWD,SMS_FROM,dest,msg);
        do_log(curlurl);
        FILE *fpp=NULL;
        char response[1024];
        char response_all[9999];
        if (!(fpp=popen(curlurl,"r"))){
                do_log("Could not start curl");
        }
        else{
                while (fgets(response,1024,fpp) ){
                        if(response[strlen(response)-1]=='\n') response[strlen(response)-1]=' ';
                         strcat(response_all,response);
                }
                pclose(fpp);
        }
        do_log(response_all);
        char *first=strstr(response_all,"<result>");
        char *second=NULL;
        if (first) second=strstr(response_all,"</result>");
        if (second){
                first+=8;
                first=strdup(first);
                second=strdup(second);
                int len=strlen(first)-strlen(second);
                char res[101];
                strncpy(res,first,len);
                res[len]='\0';
                if (!strcmp(res,"1")) return 1;
                else return 0;
        }
        else return 0;

}

int create_sms_file(char *msg, char *dest){
	char sms_spool_tmp_path[1024];
	char sms_spool_path[1024];
	char cmd[9999];
	sprintf(sms_spool_tmp_path,"%s%s/%s.%d.vcard",SMS_POOL,SMS_SPOOL_TMP,dest,getpid());
	sprintf(sms_spool_path,"%s%s.%d.vcard",SMS_POOL,dest,getpid());
	FILE *fpout = fopen(sms_spool_tmp_path,"w");
	if (fpout){
		fprintf(fpout,"%s",msg);
		fclose(fpout);
		sprintf(cmd,"/bin/mv %s %s",sms_spool_tmp_path,sms_spool_path);
		if (system(cmd) == 0){
			do_log("done");
			return 1;
		}
		else{
 //Added this condition so that ccno are visible to vinanti			do_log("not done");
			return 0;
		}
	}
	else{
		do_log("creating sms file failed");
		return 0;
	}
}

char *hide_cc_number(char *cc, char *op){
	//if(strlen(cc)>4)
	//{
        	char cc_num[5];
        	strncpy(cc_num,cc,4);
        	cc_num[4]='\0';
        	char *p=cc;
        	p+=strlen(cc)-4;
        	sprintf(op,"%sXXXXXXXX%s",cc_num,p);
        	return op;
	/*}
	else 
	{
		strcpy(op,""); 
		return op;
	}*/
}
char *change_date_format(char *old_frmt,char *new_frmt)
{
        char *ch;
        char charr[3][5]; //array to store date
        int i=0;
        //char da[15]="";
        ch = strtok(old_frmt, "-");
        strcpy(charr[i],ch);
        while (ch != NULL)
        {
                if(i<3)
                        strcpy(charr[i],ch);
                //else //if(strstr(old_frmt," "))
                //        strcpy(da,ch);
                ch = strtok(NULL, "- ");
		//do_log(ch);
                i++;
        }
	//if(strlen(da)>0)
        //        sprintf(new_frmt,"%s-%s-%s %s",charr[2],charr[1],charr[0],da);
        //else
                sprintf(new_frmt,"%s-%s-%s",charr[2],charr[1],charr[0]);
	//do_log(new_frmt);
        return new_frmt;
}

void draw_page(string path, int section_no)
{
	draw_header(section_no);
	cgi_dump_no_abort(to_c_string(path));
	draw_end();

}

inline void draw_warning_page(int section_no, string message /* = "Not authorised to access this section"*/)
{
	draw_header(section_no);
	cout<<  warning(message);
	draw_end();
}

inline string warning(string message)
{
	return "<div class='feedback_msg error'><p>" + message + "<p></div>";

}

void send_countries(fax_database *db)
{
	int count;
	string *countries = get_countries(db,count);
	cout << "<option>Select</option>"<< endl;
	for(int i=0; i<count; i++)
	{
		 cout << "<option value=\'" << countries[i] << "\' >" << countries[i] << "</option>"<< endl;
	}
}

//Returns an array of column_info objects containiong column names and data types
//count is set to number of columns in a table
column_info* get_column_info(fax_database *db,string table_name, int &count)
{
	//fax_database db(E2FAXDB, DBSERV_R);
	count = 0;
	string query;
	column_info *response;

	query = "SELECT column_name, data_type FROM information_schema.columns WHERE table_name = \'" + table_name + "\' ORDER BY ordinal_position;";
	count = db->ExecTuplesOk(to_c_string(query));	//Execute the query
	if(count>0)
	{
		   response = new column_info[count];
		   for(int i=0; i<count; i++)
		   {
				 response[i].name = db->get_field(i, "column_name");
				 response[i].data_type = db->get_field(i, "data_type");

		   }
	}
	return response;
}

//get c style string from a string object
char* to_c_string(string cpp_string)
{
	char *c_string = (char*)malloc(sizeof(char) * (cpp_string.length()+1));
	strcpy(c_string, cpp_string.data());
	return c_string;
}

//Prints sim information
void sim_search(fax_database *db)
{
	if(!valid_search_data())
	{
		cout << "\n<div id=\'s_error_msg\'>" << "Invalid Search Request" << "</div>";
		return;
	}
	char * field = cgi_getentrystr("search_cat");
	char * term = cgi_getentrystr("search_term");

	//fax_database db(E2FAXDB, DBSERV_R);
	string query;

	int column_count;
	column_info *cols = get_column_info(db,"sim_stock", column_count);

	query = "SELECT * FROM sim_stock WHERE "+ string(field) +" = \'"+ string(term) +"\';";
	int tuple_count = db->ExecTuplesOk(to_c_string(query));

	cout << ((tuple_count>0) ? tuple_to_html(db, 0, cols, column_count) : string("\n<div id=\'s_found\'>") + "0" + "</div>");

}

//Update existing sim_stock tuple
void sim_save_changes(fax_database *db)
{
	string error_msg;
	if(!valid_sim_data(db,error_msg))
	{
		cout << feedback("Update Failed.<br>" + error_msg, "error");
		return;
	}
	update_sim_stock(db);
}

void update_sim_stock(fax_database *sdb)
{
	//fax_database sdb(E2FAXDB, DBSERV);
	int total_fields;
	struct column_info *cols = get_column_info(sdb,"sim_stock", total_fields);	//Retrieve all column names
	string primary_key = "sim_phone_no";								//Primary key for the table
	string query = "UPDATE sim_stock SET";								//'update table' query

	for(int i = 0; i<total_fields; i++)
	{
		 //If a column name is present in HTTP request; excluding the primary key column
		 //Then append "column_name = 'value'" to the query
		 if(cgi_getentrystr(to_c_string(cols[i].name.data())) && cols[i].name != primary_key )
		 {
			query += " " + cols[i].name + " = \'" + string(cgi_getentrystr(to_c_string(cols[i].name.data()))) + "\',";
		 }
	}
	query += " entry_by_user = \'" + string(mis_user) + "\', dateandtime = now()";
	query += " WHERE "+ primary_key +" = \'" + string(cgi_getentrystr(to_c_string(primary_key))) + "\';";

	do_log(to_c_string((query)));
	if(sdb->ExecCommandOk(to_c_string(query)))		//Execute the query
	{
		cout << feedback("Update Successful", "noerror");
	}
	else
	{
		do_log("Update query failed");
		cout << feedback("Update Failed. Database Error", "error");
	}

}

string feedback(string mesasge, string status)
{
	return "<div id='s_feedback_status'>" + status + "</div>"
		  + "<div id='s_feedback_msg'>" + mesasge + "</div>";

}
//Validation of data received by search function
bool valid_search_data(void)
{
	//Retrieve data from request
	char* field = cgi_getentrystr("search_cat");
	char* term = cgi_getentrystr("search_term");

	//No empty values
	if(!field || !term)
	{
		 return false;
	}
	//term should be numeric if field != 'srno'
	string field_s = string(field);
	if((field_s != "srno") && (!is_number(term)))
	{
		return false;
	}
	return true;
}

//Validation of a SIM tuple.
//Returns - true / false
//Assigns error message to the error_msg argument
bool valid_sim_data(fax_database *db,string &error_msg)
{
	//Required fields
	string required[] = {"simno", "sim_phone_no", "country", "active", "pinno", "puk1"};
	const int total_required = 6;
	char *data;
	string data_s;
	error_msg = " Invalid Data: ";

	for(int i = 0; i < total_required; i++)
	{
		data = cgi_getentrystr(to_c_string(required[i]));
		data_s = string(data);
		if(data_s.empty())		//Empty data
		{
			error_msg += required[i] + "' was empty. ";
			do_log(to_c_string(error_msg.data()));
			return false;
		}
		else if((required[i] != "active" && required[i] != "country") && (!is_number(data))) //Non numeric
		{
			error_msg += required[i] + "' was non numeric. ";
			do_log(to_c_string(error_msg.data())); 
			return false;
		}
	}
	
	/* Field wise validation check. Validatin for optional fields can be added here */
	
	//'active' field
	string active = cgi_getentrystr("active");
	string comment = cgi_getentrystr("comment");
	if(!(active == "t" || active  == "f"))		//Neither t nor f
	{
		error_msg += "'active' was neither 't' nor 'f'. ";
		do_log(to_c_string(error_msg.data())); 
		return false;
	}
	else if(active == "f" && comment.empty())
	{
		error_msg += "'comment' must be provided when inactivating. ";
		do_log(to_c_string(error_msg.data()));
		return false;
	}
	
	//'country' field
	data_s = cgi_getentrystr("country");
	if(!valid_country(db,data_s))
	{	
		error_msg += "'country' was not found in the list.";
		do_log(to_c_string(error_msg.data())); 
		return false;
	}

	/* @ToDo: Devise html encoding to prevent XSS */

	return true;
}

//Returns true if the parameter is in sim_stock.countries
bool valid_country(fax_database *db,string country)
{
	bool response = false;
	int count, i=0;
	string *countries = get_countries(db,count);
	for(int i = 0; i<count; i++)
	{
		if(countries[i] == country)
		{
			return true;
		}
	}
	return false;
}

//Return the list of all values of sim_stock.country as an array of string objects
string* get_countries(fax_database *db,int &count)
{
	//fax_database db(E2FAXDB, DBSERV_R);
	char *query = "SELECT DISTINCT country FROM sim_stock ORDER BY country;";
	string *response;
	count = db->ExecTuplesOk(query);
	if(count)
	{
		response = new string[count];
		for(int i = 0; i< count; i++)
		{
			response[i] = db->get_field(i, "country");
		}
		return response;
	}
	return response;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
inline const string boolToString(bool b)
{
	return b ? "true" : "false";
}

//Adding 'Current week / Last week' option in case CRM
void draw_crm_case_source_options(void)
{
     //Last week / This week option
     char case_of_array[CRM_CASE_OF_CNT][251];         //str_splits expects such an array
     char case_of[1024];
/*     if((get_tag_value(CONFIG_DATA_FILE,"<!-- CASE_SOURCE","-- CASE_SOURCE>","crm_case_of",case_of)!=1) || (case_of==NULL || case_of[0]==0))
     {
          do_log("Could not open config file.");
          cout << "Error occured while opening config file.";
     }*/

	   char* data46 = get_simmis_data_from_table("CASE_SOURCE_crm_case_of","");
        if(data46[0]==0 || data46==NULL ){
        do_log("Could not get config data46.");
        cout << "Error occured while getting config data.";}
        strcpy(case_of,data46);





     if(str_split(',',case_of,case_of_array)<1)
     {
          do_log("failed to split");
          cout << "Error occured while processing data from config file.";
     }

     cout << "\n<div id=\"case_of_div\" style=\"visibility:hidden;\">";
     cout << "\n\t<span style=\"font-weight:bold;\">Case Of</span>";
     cout << "\n\t<select name=\"case_of\" id=\"case_of\">";
     cout << "\n\t\t<option value=\"none\" >Select case of</option>";
     for(int i = 0; i < CRM_CASE_OF_CNT ; i++)
     {
          cout << "\n\t\t<option value=\"" << case_of_array[i] << "\">" << case_of_array[i] << "</option>";
     }
     cout << "\n\t</select>";
     cout << "\n</div>";
}

//Filters sim_stock tuples accepts pointer to a function for printing a tuple
void query_sim_stock(fax_database *db,tuple_export print)
{
	//fax_database db(E2FAXDB, DBSERV);
	string query, country, active;
	string where_clause, country_clause, active_clause;
	country = cgi_getentrystr("country");
	active = cgi_getentrystr("active");

	//Construct the query
	country_clause = "country " + ((country == "all") ? "IS NOT null" : "in ('" + country + "')") ;
	active_clause = "active " + ((active == "both") ? "IS NOT null" : "in ('" + active + "')") ;
	where_clause = "WHERE " + country_clause + " AND " + active_clause;
	query = "SELECT date(dateandtime) as dateandtime, * FROM sim_stock " + where_clause;
	int col_count;
	column_info *cols = get_column_info(db,"sim_stock", col_count);
	do_log(to_c_string(query));
	int cRecords = db->ExecTuplesOk(to_c_string(query));
	if(cRecords)
	{
		if(cgi_getentryint("format") == 1)		//Send http headers for file download
		{
			do_log("stock.csv download");
			cout << "Content-Description: File Transfer\n"
				<< "Content-Type: application/octet-stream\n"
				<< "Content-Disposition: attachment; filename=stock.csv\n"
				<< "Expires: 0\n"
				<< "Cache-Control: must-revalidate\n"
				<< "Pragma: public\n";
		}
		for(int j=0; j<cRecords; j++)
		{
			cout << print(db, j, cols, col_count);
		}
	}
	else		//No records found
	{
		cout << feedback("Database error", "error");
	}
}

//Accepts an open db connection as fax_database object
string tuple_to_html(fax_database *db, int tuple_index, column_info *cols, int column_count)
{
	string response;
	response += "\n<div class='sim'>";
	for(int i=0; i<column_count; i++)
	{
		response += "\n\t<div class='" + cols[i].name + "'>" + db->get_field(tuple_index,to_c_string(cols[i].name)) + "</div>";
	}
	response += "\n</div>";
	return response;
}

//Accepts an open db connection as fax_database object
string tuple_to_csv(fax_database *db, int tuple_index, column_info *cols, int column_count)
{
	string response;
	response += "\n";
	for(int i=0; i<column_count; i++)
	{
		response += "\"" + string(db->get_field(tuple_index,to_c_string(cols[i].name))) + "\",";
	}
	response.erase(response.length()-1);
	return response;
}

//Runs a shell command and returns output as a string object
string runcmd(string command)
{
	FILE *fp;		//pointer to stdin
	string output;
	int status;
	fp = popen(to_c_string(command), "r");
	if (fp == NULL)
	{
		output = "Error opening pipe";
	}
	else
	{
		unsigned char c; 
		while(1)
		{
			c = fgetc(fp);
			if(feof(fp))
				break;
			output += (char) c;
		}
		status = pclose(fp);
		if (status == -1) 
		{
			output += "Error closing pipe";
		}
	}
	return output;
}

//Get api_code from sim_phone_no
string getApiCode(fax_database *db,string phone_no)
{
	 //fax_database db(E2FAXDB, DBSERV_R);
	 //string query = "SELECT api_code FROM countrydata, sim_stock WHERE sim_phone_no = '" + phone_no + "' AND countrydata.countryname = sim_stock.country;";
	 string query = "SELECT vendor from sim_stock WHERE sim_phone_no = '" + phone_no + "'";
	 int count = db->ExecTuplesOk(to_c_string(query));
	 //return (count > 0)? db->get_field(0, "api_code") : "Error";
	 return (count > 0)? db->get_field(0, "vendor") : "Error";
}

//Get simno (ICCID) from sim_phone_no
string getSimNo(fax_database *db,string phone_no)
{
	//fax_database db(E2FAXDB, DBSERV_R);
	string query = "SELECT simno FROM sim_stock WHERE sim_phone_no = '" + phone_no + "';";
	int count = db->ExecTuplesOk(to_c_string(query));
	return (count > 0)? db->get_field(0, "simno") : "Error";
}

//Retrieve sim_phone_no from simno
string getPhoneNo(fax_database *db,string simno)
{
	//Returns ICCID from phone_no
	//fax_database db(E2FAXDB, DBSERV_R);
	string query = "SELECT sim_phone_no FROM sim_stock WHERE simno = '" + simno + "';";
	int count = db->ExecTuplesOk(to_c_string(query));
	return (count > 0)? db->get_field(0, "sim_phone_no") : "Error";
}

//Calls backend API to activate / deactivate a SIM
bool backend_update_status(fax_database *db,string phone_no, bool active)
{
	string api_code = getApiCode(db,phone_no);
	//fax_database db(E2FAXDB, DBSERV_R);
	string query = "SELECT * from sim_service_providers WHERE api_code = '" + api_code + "';";
	bool has_api, result;

	if(db->ExecTuplesOk(to_c_string(query)))
	{
		has_api = ((string(db->get_field(0, "has_api")) == "t") ? true : false );
	}
	else
	{
		do_log("Database Error");
		return false;
	}
	if(!has_api)	//Other than telna, cloud9
	{
		//Send email to vendor
		string mailto = string(db->get_field(0, "email"));
		string cmd = string("echo -e \"Mail body - ") + (active? "A" : "Dea")+"ctivate "+ phone_no + "\" | mail -s \"(SIM MIS Test)Deactivation\" \""+mailto+"\"";
		string res = runcmd(cmd);
		do_log(to_c_string("Executing command: " + cmd));
		result = true;
	}
	else
	{
		int i = (api_code == "telna") ? 1 : 0;
		string script = string("/usr/local/ownmail/bin/sim_api/") + ((i==0)? "gSim_call.php SetSimStatus" : "telna_call.php setAccountInFormation");
		string active_status = (i==0)? "Enabled" : "true";
		string inactive_status = (i==0)? "Disabled" : "false";
		string input_status = (active)? active_status : inactive_status;
		string input_number = (i == 0) ? phone_no : getSimNo(db,phone_no);
		string success = (i==0)? "success" : "ok" ;
	
		string cmd = script + " " + input_number + " " + input_status;
		do_log(to_c_string("Calling script : " + cmd)); 
		string output = runcmd(cmd);
		result = (output == success) ? true : false;

		if(!result)
		{
			//API call failed. Sending mail.
			string mailto = "binu@staff.ownmail.com";
			string mailsub = api_code + ": update failed for " + phone_no;
			string mailbody = string("The following script returned a negative result while ") + ((active)? "" : "de") + "activating " + phone_no + ". Kindly do it manually.\n\n" + cmd;
			string cmd = "echo -e \"" + mailbody + "\" | " + "mail -s \"" + mailsub + "\" \"" + mailto + "\"";
			string op = runcmd(cmd);
		}
	}
	return result;
}

//Calls web service consumer script to replace sim 
bool backend_replace_sim(fax_database *db,string lost_iccid, string new_iccid)
{
	string phone_no = getPhoneNo(db,lost_iccid);
	string api_code = getApiCode(db,phone_no);
	do_log(to_c_string(api_code));
	string script_path = "/usr/local/ownmail/bin/sim_api/";
	string cmd, success;
	if(api_code == "telna")
	{
		cmd = script_path + "telna_call.php replaceSim " + phone_no + " " + lost_iccid + "  " + new_iccid;
		success = "ok";
	}
	else if(api_code == "cloud9")
	{
		cmd = "gSim_call.php SimSwap " + lost_iccid + "  " + new_iccid;
		success = "success";
	}
	else
	{
		return false;
	}
	do_log(to_c_string(string("before running ") + cmd ));
	return ((runcmd(cmd)) == success ) ? true : false;
}

//Sim Replace
void replace_sim(fax_database *db)
{
	string simno = cgi_getentrystr("simno"), simno_old = cgi_getentrystr("simno_old");
	if(!is_number(simno) || simno == simno_old)
	{
		cout << feedback("Replace Failed. Invalid SIM Number", "error");
		return;
	}
	if(backend_replace_sim(db,simno_old, simno))
	{
		do_log("backend update successful");
		update_sim_stock(db);
	}
	else
	{
		cout << feedback("Replace Failed. Backend Updade Failed.", "error");
	}
}
//Splits a string based on a delimiting character and returns a string vector
vector<string> splitString(string sequence, char delimiter)
{
	stringstream ss(sequence);
	vector<string> result;
	while(ss.good())
	{
		string token;
		getline(ss, token, delimiter);
		result.push_back(token);
	}
	return result;
}

//For accessing data from CONFIG file
string read_tag_attribute(string tag_name, string attribute_name)
{
	//Read the tag
	char *sequence = new char[201];
	if((get_tag_value(CONFIG_DATA_FILE,to_c_string("<!--" + tag_name), to_c_string("--" + tag_name + ">"), to_c_string(attribute_name), sequence)!=1) || (sequence==NULL || sequence[0]==0))
	{
		do_log("Could not open config file.");
		return string("");
	}
	else
	{
		return sequence;
	}
}

//Reports
void client_report(unsigned short int format)
{
	string from_date = cgi_getentrystr("from_date"), to_date = cgi_getentrystr("to_date");
	ostringstream query_stream;
	
	query_stream << "SELECT "
				<< "c.username UserID,"
				<< "clientname AS Client_Name, "
				<< "companyname AS Company_Name, "
				<< "sales_person AS Sales_Person, "
				<< "reseller_sp, "
				<< "status AS SimCard_Status, "
				<< "c.sim_phone_no, "
				<< "telno, "
				<< "mobno, "
				<< "booking_date, "
				<< "from_date AS Trip_Start_Date, "
				<< "to_date AS Trip_End_Date, "
				<< "country AS Country_Of_Travel, "
				<< "s.add_city AS Address_City, "
				<< "s.category "
			<< "FROM "
				<< "clienttrip c,sim_user s, sim_stock st "
			<< "WHERE "
				<< "c.username = s.username and "
				<< "st.sim_phone_no = c.sim_phone_no and "
				<< "booking_date >= '" + from_date + "' and "
				<< "booking_date < timestamp_pl_interval('" + to_date + "','86400') "
			<<"ORDER BY booking_date;";
	string query = query_stream.str();
		log_s(query);
	queryResult q(query);
	switch(format)
	{
		case 1:
			do_log("sending JSON");
			cout << "Content-Type: application/json\n\n";
			cout << q.toJSON();	
			do_log("JSON sent");
			break;
		case 2:
			do_log("sending CSV");
			cout << file_download_headers("clients.csv");
			cout << q.toCSV();	
			do_log("CSV sent");
			break;
		default:
			break;
	}
}

queryResult::queryResult(string queryString)
{
	query = queryString;
	dbr = new sim_database(E2FAXDB, DBSERV_R);
	totalRows = dbr->ExecTuplesOk(to_c_string(query));
	if( totalRows > 0 )
	{
		totalCols = dbr->getPgDb()->Fields();
	}
}

string queryResult::toJSON(void)
{
	const string dq = "\"";
	ostringstream r;
	r << "{\n";
	if( totalRows > 0 )
	{
		//Print cols
		r << dq << "cols" << dq << " : " << "[\n";	
		for(int c = 0; c < totalCols; c++)
		{
			r << endl << "\t{ "
			  << dq << "id" << dq << ":" << dq << dbr->getPgDb()->FieldName(c) << dq << ", "
			  << dq << "label" << dq << ":" << dq << dbr->getPgDb()->FieldName(c) << dq << ", "
			  << dq << "type" << dq << ":" << dq << "string" << dq
			  << " }" << ( ( c == totalCols - 1 ) ? " " : ", " );
		}
		r << "\n],\n";
		//Print rows
		r << dq << "rows" << dq << " : " << "[\n";	
		for( int i = 0; i < totalRows; i++ )
		{
			r << endl << "\t[" << "\n";
			for( int j = 0; j < totalCols; j++ )
			{
				r << "\t\t" 
				     //<< dq << dbr->getPgDb()->FieldName(j) << dq 
				     //<< ":" 
				     << dq << dbr->getPgDb()->GetValue(i,j) << dq 
				     << ( (j == ( totalCols - 1 ) ) ? "\n" : ",\n" );
			}
			r << "\t]" << ( ( i == totalRows - 1 ) ? "\n" : ",\n" )<< endl;
		}
		r << "]";
	}
	else
	{
		/* Error response */
		r << dq << "result" << dq << " : " << dq << "error" << dq;
	} r << "\n}";
	return r.str();
}
string queryResult::toCSV()
{
	const string cm = ",";
	ostringstream r;

	for(int j = 0; j < totalCols; j++)
	{
		r << dbr->getPgDb()->FieldName(j) << (j == (totalCols - 1)? "" : ",");
	}
	for(int i = 0; i < totalRows; i++)
	{
		r << "\n";
		for(int j = 0; j < totalCols; j++)
		{
			r << dbr->getPgDb()->GetValue(i,j) << (j == (totalCols - 1)? "" : ",");
		}
	}
	return r.str();
}

string file_download_headers(string filename)
{
	ostringstream r;
	r << "Content-Description: File Transfer\n"
	  << "Content-Type: application/octet-stream\n"
	  << "Content-Disposition: attachment; filename=" << filename << "\n"
	  << "Expires: 0\n"
	  << "Cache-Control: must-revalidate\n"
	  << "Pragma: public\n\n";
	return r.str();
}
