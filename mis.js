$(document).ready(function() {
    //Call the coresponding function
    var div_id = $("#section_id").val();
    if (!$("#" + div_id).length) {
        return;
    }

    var random_id = function() {
        var id_num = Math.random().toString(9).substr(2, 3);
        var id_str = Math.random().toString(36).substr(2);

        return id_num + id_str;
    }

    switch (div_id) {
        case "edit_panel":
            edit_sim();
            break;
        case "stock_summary":
            stock_summary();
            break;
        case "sim_replace":
            sim_replace();
            break;
        case "client_report":
            client_report();
            break;
        case "active_summary":
            activation_report();
            break;
        case "reconcile_summary":
            reconcile_report();
            break;
        default:
            return;
    }

});
//pointer2

        // Function to fetch the latest record from the database
        function fetchLatestRecord(table_row, rowId, orderNumber, tripID) {
            $.ajax({
                //url: 'https://mail004.ownmail.com/html_parts/fetch-latest-record.php',
                url: 'https://mail004.ownmail.com/html_parts/fetch-latest-record.php',
                type: 'POST',
                data: {
                    rowId: rowId,
                    orderNumber: orderNumber,
		    tripid: tripID
                }, // Pass any necessary parameters
                dataType: 'json',
                success: function(response) {
                    // Check if the request was successful
                    if (response.error) {
                        console.error('Error fetching latest record   from here 1:', response.error);
                        return;
                    }

                    if (response.length > 0) {
                        // Assuming there's only one record in the response array
                        var latestRecord = response[0];
//			var og_comment = table_row.find('.comment').text().trim();//email added here
		var og_email = latestRecord.emaildadd;
		var original_comment = latestRecord.comment; // Original value
		var cleaned_comment = original_comment.replace(/OLD:|NEW:|<br>/g, '');

		table_row.data('original-email', og_email);
		table_row.data('cleaned-comment',cleaned_comment);
		console.log('Original Comment:', original_comment);
		console.log('Cleaned Comment:', cleaned_comment);
		latestRecord.comment = cleaned_comment;
		console.log('Original Comment:', original_comment);
                console.log('Cleaned Comment:', cleaned_comment);

                        updateUI(rowId, latestRecord);
		var og_email = latestRecord.emailadd;
                var og_comment = original_comment;

                table_row.data('original-email', og_email);
                table_row.data('original-comment', og_comment);

                console.log("This is original email 2:", og_email);
                console.log("This is original comment 2:", og_comment);
                        if (table_row.find('.row_active').find('.active').find('select').val() == "YES") {
                            table_row.animate({
                                backgroundColor: 'yellow'
                            }, 1000);
                        }
                    } else {
                        //console.error('No records found.');
                    }

                },
                error: function(xhr, status, error) {
                    console.error('Error fetching latest record from turli:', error);
                }
            });
        }

//ISSUE 2451
function checkForSpecialChars(inputString) {
    // Regular expression pattern to check for spaces or special characters
    var pattern = /[^a-zA-Z0-9-_]/;

    // Check if the input string matches the pattern
    if (pattern.test(inputString)) {
        // Alert message indicating the presence of spaces or special characters
        alert("Phone number contains spaces or special characters. Please correct it.");
        return false;
    }
    return true;
}

var sim = function() {};

var messageBox = function(selector) {
    this.element = $(selector);
    this.defaultClass = 'feedback_msg';
    this.reset = function() {
        //reset and hide
        this.element.hide();
        this.element.removeClass();
        this.element.addClass(this.defaultClass);
        this.element.html('');
    }
    this.show = function(content, className) {
        //set classname, content and show
        this.reset();
        this.element.addClass(className);
        this.element.html(content);
        this.element.show();
    }
    this.element.hide();
};

var get_data = function(selector, action, extractor) {
    // console.log("selector is : "+selector);
    // console.log("action is : "+action);
    // console.log("extractor is : "+extractor);
    ////console.log(action);
    var form_element = $(selector);
    ////console.log(form_element.serialize());
    var posting = $.ajax({
        url: action,
        type: 'GET',
        data: form_element.serialize(),
    });
    posting.done(function(data) {
        // console.log("data is : "+data);
        var temp_id = "temp_" + Math.floor((Math.random() * 10) + 1);
        $("body").append("<div id='" + temp_id + "'></div>");
        var div = $("#" + temp_id);
        div.html(data);
        extractor(div); //'div' is a jquery wrapper
        div.remove();
    });
};

var field = function(ftitle, ftype) {
    this.title = ftitle;
    this.type = typeof ftype === undefined ? '' : ftype;
}
var sim_fields = {
    'simno': new field("Sim Number"),
    'sim_phone_no': new field("Phone Number"),
    'srno': new field("Sr. Number"),
    'availability': new field("Available", 'bool'),
    'country': new field("Country"),
    'vendor': new field("Vendor"),
    'dateandtime': new field("Last Updated"),
    'entry_by_user': new field("Updated By"),
    'comment': new field("Comment"),
    'qrtext': new field("Qr Text"),//Qr Text ADDED HERE
    'active': new field("Active", 'bool'),
    'pinno': new field("PIN"),
    'puk1': new field("PUK 1"),
    'puk2': new field("PUK 2"),
    'sim_phone_no_disp': new field("Display Phone No"),
    'expiry_date': new field("Expiry Date")
};

var sim_replace = function() {
    //messageBox object
    msgbox = new messageBox(".feedback_msg");

    var sim_table = $("#sim_display"); //Sim display table
    var sim_form = $("#sim_replace_form"); //Sim replace form

    //Add dsiplayed fields
    for (var field in sim_fields) {
        sim_table.append("<tr><th>" + sim_fields[field]['title'] + "</th><td class = '" + field + "'></td></tr>");
        if (field == 'simno' || field == 'entry_by_user' || field == 'dateandtime') continue;
        sim_form.append("<input name = '" + field + "' type='hidden' />");
    }

    //Search form change event
    $("#sim_search_form").change(function(event) {
        sim_table.find("td").each(function(index) {
            $(this).text('');
            msgbox.reset();
        });
        $("#sim_replace_form").trigger("reset");
    });

    //Search form submit event
    $("#sim_search_form").submit(function(event) {
        event.preventDefault(); // Stop form from submitting normally
        msgbox.show("Please Wait.");

        //Clear the table
        for (var field in sim_fields) {
            sim_table.find("td." + field).text('');
        }

        get_data("#sim_search_form", "simmis.e?a=sim_edit&b=search", function(data) {
            if (data.find(".sim").length) {
                var s = new sim();
                data.find(".sim > [class]").each(function(j) {
                    /*var v = (sim_fields[$(this).attr('class')]['type'] == 'bool')? (($(this).text() == 't')? 'true' : 'false') : $(this).text();*/
                    s[$(this).attr("class")] = $(this).text();
                });
                for (var field in s) {
                    $("#sim_display tr td." + field).html(((sim_fields[field]['type'] === 'bool') ? ((s[field] == 't') ? 'true' : 'false') : s[field]));
                    if (field == 'entry_by_user' || field == 'dateandtime') continue;
                    $("#sim_replace_form input[name='" + field + "']").val(s[field]);
                }
                msgbox.show("Sim Found", "noerror");
            } else {
                msgbox.show("Sim Not Found", "error");
            }
        });
    });

    //Replace sim form submit event
    sim_form.submit(function(event) {
        event.preventDefault();
        sim_form.append("<input name = 'simno_old' type='hidden' value = '" + sim_form.find("input[name='simno']").val() + "' />");
        get_data("#sim_replace_form", "simmis.e?a=replace_sim&b=commit", function(result) {
            alert(result.html());
        });
    });
};

var edit_sim = function() {
    //Get countries
    $("#edit_country").load("simmis.e?a=sim_edit&b=get_countries");

    //messageBox object
    msgbox = new messageBox(".feedback_msg");

    //Initial form visibility
    $("#sim_search_form").show();
    $("#sim_edit_form").hide();
    $("#recieved_sim_info").hide();

    //Trim text inputs on change
    $("#sim_edit_form input, #sim_search_form input").change(function(event) {
        var c = $(event.target);
        c.val($.trim(c.val()));
    });

    //Radio Button Click
    $("input[name='search_cat'][type='radio']").click(function(event) {
        $("#search_term_label").html(event.target.title);
    });

    //Search form Change
    $("#sim_search_form").change(function(event) {
        msgbox.reset();
    });

    //SIM Search form submit
    $("#sim_search_form").submit(function(event) {
        // Stop form from submitting normally
        event.preventDefault();
        var search_data = $("#sim_search_form").serialize(); //Form data => string
        var posting = $.ajax({
            url: "simmis.e?a=sim_edit&b=search",
            type: 'GET',
            data: search_data,
        });

        posting.done(function(data) {
            var raw_data = $("#recieved_sim_info");
            raw_data.html(data);

            if (raw_data.find(".sim").length != 0) {
                //Parse the raw data in received_sim_info

                //simno
                $("#sim_edit_form #edit_simno").val(raw_data.find(".simno").html());

                //sim_phone_no
                $("#sim_edit_form #edit_sim_phone_no").val(raw_data.find(".sim_phone_no").html());
		
		//QR TEXT ADDED HERE
		var qrtext = raw_data.find(".qrtext").html();
                $("#sim_edit_form #edit_qrtext").val(raw_data.find(".qrtext").html());
                console.log(qrtext);
                console.log(data);
	
                //Country
                var new_country = raw_data.find(".country").html();
                $("#sim_edit_form #edit_country").val('');
                $("#sim_edit_form #edit_country").val(new_country);

                //active
                var isactive = raw_data.find(".active").html();
                if (isactive == 't') {
                    $("input[type='radio'][name='active'][value='t']").prop("checked", true);
                    $("input[type='radio'][name='active'][value='f']").prop("checked", false);
                } else {
                    $("input[type='radio'][name='active'][value='t']").prop("checked", false);
                    $("input[type='radio'][name='active'][value='f']").prop("checked", true);
                }

                //pinno
                $("#sim_edit_form #edit_pinno").val(raw_data.find(".pinno").html());

                //puk1
                $("#sim_edit_form #edit_puk1").val(raw_data.find(".puk1").html());

                //dateandtime
                $("#sim_edit_form #edit_dateandtime").html(raw_data.find(".dateandtime").html());

                //entry_by_user
                $("#sim_edit_form #edit_entry_by_user").html(raw_data.find(".entry_by_user").html());

                //srno
                $("#sim_edit_form #edit_srno").html(raw_data.find(".srno").html());

                //vendor
                $("#sim_edit_form #edit_vendor").html(raw_data.find(".vendor").html());

                //comment
                $("#sim_edit_form #edit_comment").text(raw_data.find(".comment").text());

                //Switch to edit form
                $("#sim_search_form").hide();
                $("#sim_edit_form").show();
            } else {
                msgbox.show("No Records found", "error");
            }

            //Server returned an error msg
            if ($("#s_error_msg").length > 0) {
                msgbox.show($("#s_error_msg").html(), "error");
            }
        });
    });

    //SIM Edit form submit
    $("#sim_edit_form").submit(function(event) {
        event.preventDefault();
        var sim_data = $("#sim_edit_form").serialize();
        var posting = $.ajax({
            url: "simmis.e?a=sim_edit&b=save_changes",
            type: 'GET',
            data: sim_data,
        });
        posting.done(function(data) {
            //Refresh the edit form with latest data
            $("input[name='search_cat'][type='radio'][value = 'sim_phone_no']").trigger('click');
            $("#search_term").val($("#edit_sim_phone_no").val());
            $("#sim_search_form").submit();

            var rnd = Math.floor((Math.random() * 10) + 1);
            $("body").append("<div id = 'temp_" + rnd + "'>" + data + "</div>");
            var raw_data = $("#temp_" + rnd)
            var classname = raw_data.find("#s_feedback_status").html();
            var content = raw_data.find("#s_feedback_msg").html();
            $("#temp_" + rnd).remove();
            msgbox.show(content, classname);

        });
        posting.fail(function() {
            msgbox.show("Error connecting to server", "error");
        })
        posting.always(function() {
            //alert( "complete" );
        });

    });

    //SIM Edit form change
    $("#sim_edit_form").change(function(event) {
        msgbox.reset();
    });

    //Edit form cancel button (form reset)
    $("#editor_cancel_button").click(function(event) {
        msgbox.reset();

        $("#sim_edit_form").hide();
        $("#sim_search_form").show();

        //Clear data received
        $("#sim_received_info").html('');
    });
};

var stock_summary = function() {
    //Get countries
    ////console.log("STOCK SUMMARY");
    $("#filter_country").load("simmis.e?a=sim_edit&b=get_countries");

    //messageBox object
    msgbox = new messageBox(".feedback_msg");
    msgbox.show("Please Select a country.", "");

    //Form change event for filter_form
    $("#stock_filter_form").change(function(event) {

        //Clear Stock List Table
        $("#stock_list_table > tbody").html('');
        $("#stock_download_link").attr("href", "#");

        if ($("#filter_country").val() == 'Select') {
            msgbox.show("Please Select a country/documents/shipment report.", "");
            return;
        }

        msgbox.show("Please Wait");

        var country = $("#filter_country").val();
        ////console.log(country);
        ////console.log("Country");
        var active = $("#filter_active").val();
        ////console.log(active);
        var date = $("#filter_date").val();
        //console.log(date);
        var to_date = $("#filter_to_date").val();
        console.log(to_date);



        get_data("#stock_filter_form", "simmis.e?a=stock_summary&b=query", function(data) {
            if (data.find(".sim").length == 0) {
                msgbox.show("No Records Found", "error");
            } else {
                ////console.log(data);
                ////console.log(data.find(".sim"));
                ////console.log("Hello Check this");
                data.find(".sim").each(function(index) {
                    //Create sim object with 14 properties
                    var s = new sim();
                    ////console.log($(this).find("[class]"));
                    $(this).find("[class]").each(function(j) {
                        ////console.log("Attr "+$(this).attr("class")+" === "+$(this).text());
                        s[$(this).attr("class")] = $(this).text();
                    });
                    //append table row
                    var rwNew = "<tr class='" + ((s['active'] == 't') ? "active_sim" : "inactive_sim") + "'>";
                    rwNew += "<td>" + s['simno'] + "</td>";
                    rwNew += "<td>" + s['sim_phone_no'] + "</td>";
                    rwNew += "<td>" + s['country'] + "</td>";
                    rwNew += "<td>" + s['vendor'] + "</td>";
                    rwNew += "<td>" + ((s['availability'] == 't') ? "Yes" : "No") + "</td>";
                    rwNew += "<td>" + s['pinno'] + "</td>";
                    rwNew += "<td>" + s['puk1'] + "</td>";
                    rwNew += "<td>" + s['srno'] + "</td>";
                    rwNew += "<td>" + s['dateandtime'] + "</td>";
                    rwNew += "<td>" + s['entry_by_user'] + "</td>";
                    rwNew += "<td>" + s['comment'] + "</td>";
                    rwNew += "<td>" + s['qrtext'] + "</td>";//QR TEXT
              	    rwNew += "<td>" + s['expiry_date'] + "</td>";
                    rwNew += "<td>" + s['handset_model'] + "</td>";
                    rwNew += "<td>" + s['asin'] + "</td>";
                    rwNew += "</tr>";
                    $("#stock_list_table > tbody").append(rwNew);
                });
		console.log("HERE");
                msgbox.show(data.find(".sim").length + " Records Found.", "noerror");
                $("#stock_download_link").attr("href", "simmis.e?a=stock_summary&b=query&format=1&" + $("#stock_filter_form").serialize());
            }
        });
    });

};


var activation_report = function() {


    var random_id = function() {
        var id_num = Math.random().toString(9).substr(2, 3);
        var id_str = Math.random().toString(36).substr(2);

        return id_num + id_str;
    }

    $("#filter_country").load("simmis.e?a=getcountry_actreport");
    //Get countries
    ////console.log("ACTIVATION");
    $(document).find('.btn_save').hide();
    $(document).find('.btn_cancel').hide();
    var select = document.getElementById("filter_country");
    var to_date = $("#to_date_act_form").val();
    //console.log("#to_date_act_form = " + to_date);
    var dtToday = new Date();

    var monthdt = dtToday.getMonth() + 1;
    var daydt = dtToday.getDate();
    var yeardt = dtToday.getFullYear();
    if (monthdt < 10)
        monthdt = '0' + monthdt.toString();
    if (daydt < 10)
        daydt = '0' + daydt.toString();

    var maxDate = yeardt + '-' + monthdt + '-' + daydt;
    //console.log("Max date = " + maxDate);
    $(document).find('#to_date_act_form').attr('max', maxDate);
    //messageBox object
    msgbox = new messageBox(".feedback_msg");
    datebox = new messageBox(".date_msg");
    msgbox.show("Please Select a country/customer documents.", "");
    //	if(to_date==""){	
    //	datebox.show("Report will show past 10 days records of pending activation", "");
    //	}
    //	else{
    //	datebox.show("");
    //	}
    var d = new Date();
    var month = d.getMonth() + 1;
    var day = d.getDate();
    var output = d.getFullYear() + '-' + (month < 10 ? '0' : '') + month + '-' + (day < 10 ? '0' : '') + day;
    $(document).find('#active_date').val(output);
    $("#activation_report_form").change(function(event) {
        $("#stock_list_table > tbody").html('');
        $("#report_download_link").attr("href", "#");
        if ($("#filter_country").val() == 'Select') {
            msgbox.show("Please Select a country/document report.", "");
            return;
        }
        var country = $("#filter_country").val();
        var dtToday = new Date();

        var monthdt = dtToday.getMonth() + 1;
        var daydt = dtToday.getDate();
        var yeardt = dtToday.getFullYear();
        if (monthdt < 10)
            monthdt = '0' + monthdt.toString();
        if (daydt < 10)
            daydt = '0' + daydt.toString();

        var maxDate = yeardt + '-' + monthdt + '-' + daydt;
        // console.log("Max date = " + maxDate);
        $(document).find('#to_date_act_form').attr('max', maxDate);
        if (country != "none" && country != "customer_documents") {
            msgbox.show("Please Wait");
            var to_date = $("#to_date_act_form").val();
            //console.log("#to_date_act_form = " + to_date);
            var date = $("#active_date").val();
            var date1 = new Date(date);
            var to_date1 = new Date(to_date);
            // console.log("to_date1 = " + to_date1);
            //console.log("date1 = " + date1);
            if (to_date == "") {
                var daysPrior = 10;
                //			datebox.show("Report will show past 10 days records of pending activation", "");
            } else {
                const diffTime = Math.abs(date1 - to_date1);
                const diffDays = Math.ceil(diffTime / (1000 * 60 * 60 * 24));
                //  console.log(diffDays);
                var daysPrior = diffDays;
                //			datebox.show("Report of records from "+to_date1+" to "+date1, "");
            }
            // console.log("daysprior = " + daysPrior);
            date1.setDate(date1.getDate() - daysPrior);
            var month1 = date1.getMonth() + 1;
            //  console.log("month1 = " + month1);
            var day1 = date1.getDate();
            // console.log("day1 = " + day1);
            var output1 = date1.getFullYear() + '-' + (month1 < 10 ? '0' : '') + month1 + '-' + (day1 < 10 ? '0' : '') + day1;
            // console.log("output1 = " + output1);
        } else if (country == "customer_documents") {
            datebox.show("Report will show past 45 days records of document verification", "");
        }
        if (country == "none") {
            console.log("Country in none");
            $('#report_download_link').hide();
            $('#tsim_date_activation').show();
            $('#stock_list_display').hide();
            $('#documents_list_display').hide();
	    $('#check_imei_list_display').hide();
            $("#documents_list_tbody").empty();
            $("#shipping_list_tbody").empty();
	    $("#imei_list_tbody").empty();
            $('#shipping_list_display').hide();
            $('.all_docs').hide();
	    $('.all_imei').hide();
        } else if (country == "customer_documents") {
            console.log("Customer documents selected");
            $('.all_docs').show();
	    $('.all_imei').hide();
            $("#shipping_list_tbody").empty();
            $('#report_download_link').hide();
            $('#tsim_date_activation').hide();
            $('#stock_list_display').hide();
	    $('#check_imei_list_display').hide();
            $('#documents_list_display').show();
            $('#shipping_list_display').hide();
            //			console.log("Inside get_data of customer documents selected");
            //			$('.all_docs').on("click","input[type='checkbox']",function(e){
            if ($('#all_documents').prop("checked") == true) {
                show_all_docs();
            } else {
                some_docs();
            }
            //			});
        } else if (country == "shipping_labels") {
            console.log("Shipping labels selected");
            $('#shipping_list_display').show();
            $('#documents_list_display').hide();
            $('#stock_list_display').hide();
            $('#tsim_date_activation').show();
            $('#report_download_link').hide();
	    $('#check_imei_list_display').hide();
            $('.all_docs').hide();
	    $('.all_imei').hide();
            $("#documents_list_tbody").empty();
            show_shipments();
        } else if (country == "check_imei") {
         	console.log("Check IMEI GOT Selected");
	    $('.all_imei').show();
		$('.all_docs').hide();
            $("#shipping_list_tbody").empty();
            $('#report_download_link').hide();
            $('#tsim_date_activation').hide();
            $('#stock_list_display').hide();
            $('#check_imei_list_display').show();
            $('#shipping_list_display').hide();
            if ($('#all_imei').prop("checked") == true) {
                show_all_imei();
            } else {
         	check_imei();
            }

        } else if (country != "none" && country != "customer_documents" && country != "shipping_labels") {
            $('.all_docs').hide();
	    $('.all_imei').hide();
            $("#documents_list_tbody").empty();
            $("#shipping_list_tbody").empty();
            $('#documents_list_display').hide();
	    $('#check_imei_list_display').hide();
            $('#shipping_list_display').hide();
            $('#stock_list_display').show();
            $('#report_download_link').show();
            $('#tsim_date_activation').show();
            get_data("#activation_report_form", "simmis.e?a=active_summary&b=query&d=" + output1, function(data) {
                if (data.find(".sim").length == 0) {
                    msgbox.show("No Records Found", "error");
                } else {
                    datebox.show("Following Records Found from " + output1 + "  to  " + date, "noerror");
                    data.find(".sim").each(function(index) {
                        //console.log("index is : "+index);
                        var row_id = random_id();

                        //Create sim object with 14 properties
                        var s = new sim();
                        $(this).find("[class]").each(function(j) {
                            s[$(this).attr("class")] = $(this).text();
                            //console.log("-----------------");
                            //console.log($(this).text());
                            // console.log("------------------");
                        });

                        //console.log(s);
                        //append table row
                        var rwNew = "<tr class='" + ((s['activated'] == 't') ? "activated_sim" : "inactivated_sim") + "' row_id='" + row_id + "' >";

                        rwNew += "<td class=\"orderno\" ><a href=\"https://www.tsim.in/wp-admin/post.php?post=" + s['order_no'] + "&action=edit\" target=\"_blank\">" + s['order_no'] + "</a></td>";
                        rwNew += "<td >" + s['clientname'] + "</td>";
                        rwNew += "<td class =\"row_data emailadd\">" + s['emailadd'] + "</td>";
                        //rwNew += "<td  class=\"row_data row_date_data\" > <div style=\"height:0px; overflow:hidden\"> <input class=\"datepicker-input\" type=\"date\"/></div> <span class=\"date\">  " + s['from_date'].split(" ")[0] + "</span> </td><!--";
			rwNew += "<td class=\"from_date\">" + s['from_date'].split(" ")[0] + "</td>";
                        rwNew += "--><td class=\"row_data phone_no\"><span>" + s['sim_phone_no'] + "</span></td><!--";
                        rwNew += "--><td class=\"serialno\"><span>" + s['simno'] + "</span></td><!--";
                        rwNew += "--><td >" + s['booking_date'].split(" ")[0] + "</td>";
                        rwNew += "<td class=\"row_data vendor\">" + s['vendor'] + "</td>";
                        rwNew += "<td class=\"trip\" style='display:none;'>" + s['tripid'] + "</td>";
                        rwNew += "<td class=\"simcountry\"><span>" + s['country'] + "</span></td>";
                        rwNew += "<td class=\"row_data pinno\">" + s['pinno'] + "</td>";
                        rwNew += "<td class=\"row_data puk1\">" + s['puk1'] + "</td>";
                        rwNew += "<td>" + s['status'] + "</td>";
                        rwNew += "<td>" + s['type'] + "</td>";
                        rwNew += "<td>" + s['handset_model'] + "</td>";
                        if (s['documents_uploaded'] == 't') {
                            rwNew += "<td id=\"documents_verified\" class=\"row_docs\" row_docs=\"" + s['order_no'] + "\">" + s['no_of_documents_uploaded'] + "</td>";
                        } else {
                            rwNew += "<td class=\"row_docs\" row_docs=\"" + s['order_no'] + "\">" + s['no_of_documents_uploaded'] + "</td>";
                        }
			
			if (s['imei_verified'] == 't') {
                            rwNew += "<td id=\"imei_verified\" class=\"row_imei\" row_imei=\"" + s['order_no'] + "\"> Yes </td>";
                        } else {
                            rwNew += "<td class=\"row_imei\" row_imei=\"" + s['order_no'] + "\"> No </td>";
                        }			

                        rwNew += "<td class=\"row_data qrtext\">" + s['qrtext'] + "</td>";
                        rwNew += "<td class=\"row_data comment\">" + s['comment'] + "</td>";
                        rwNew += "<td class=\" row_active\"><span class=\"active\">" + ((s['activated'] == 't') ? "YES" : "NO") + "</span></td>";
                        rwNew += "<td>";
                        rwNew += "<span  class=\"btn_edit\"><a href=\"#\" row_id='" + row_id + "'> Edit</a> </span>";
                        rwNew += "<span  class=\"btn_save\"><a href=\"#\" row_id='" + row_id + "'> Save</a> </span>";
			if (s['activated'] == 'f'){
                        	rwNew += "<span  class=\"btn_activate\"><a href=\"#\" row_id='" + row_id + "'> Activate</a> </span>";
				rwNew += "<input type=hidden id=\"activation_scriptname\" value=\"" + s['activation_scriptname'] + "\">";
			}
                        rwNew += "<span  class=\"btn_sendmail\"><a href=\"#\" row_id='" + row_id + "'> Mail</a> </span>";
                        rwNew += "<span  class=\"btn_cancel\"><a href=\"#\" row_id='" + row_id + "'> Cancel</a> </span>";
                        rwNew += "</td>";
			//if (s['activated'] == 'f') rwNew += "<td class=\"activation_scriptname\" style='display:none;'>" + s['activation_scriptname'] + "</td>";
                        rwNew += "</tr>";

                        $("#stock_list_table > tbody").append(rwNew);
                    });
                    msgbox.show(data.find(".sim").length + " Records Found.", "noerror");
                    $("#report_download_link").attr("href", "simmis.e?a=active_summary&b=query&d=" + output1 + "&format=1&" + $("#activation_report_form").serialize());
                    $(document).find('.btn_save').hide();
                    $(document).find('.btn_cancel').hide();
                    $(document).find('.btn_sendmail').hide();
                    $(document).find('.btn_activate').hide();
                }
            });
        } else {
            datebox.show("Report will show past 10 days records of pending activation", "");
            msgbox.show("Please Select a country.", "");
        }
    });

    // $('#documents_list_tbody').on("click", ".swal_order_info", function(e) {

    //     console.log(this);
    //     var order_id = this.innerText;
    //     console.log(this.innerText);
    //     $('#tr_' + order_id + '_iframe_order_info').toggle();
    //     $('.' + order_id).toggleClass("tsim-lavender");
    //     $('#tr_' + order_id + '_iframe_order_info').toggleClass("tsim-lavender");
    //     for (var l = 1; l <= 3; l++) {
    //         $('.' + order_id + '_iframe_order_info_' + l).toggle();
    //         if ($('.' + order_id + '_' + l).length > 0) {
    //             var ex = $('.' + order_id + '_' + l).data("src");
    //             $('.' + order_id + '_' + l).css({
    //                 "background-image": "url('" + ex + "')",
    //                 "background-size": "contain",
    //                 "background-repeat": "no-repeat",
    //                 "width":"100%",
    //                 "height":"auto"
    //             });
    //         } else {
    //             $('.' + order_id + '_iframe_order_info_' + l).attr("src", $('.' + order_id + '_iframe_order_info_' + l).data("src"));
    //         }
    //     }
    // });
    $('#documents_list_tbody').on("click", ".swal_order_info", function(e) {
        //		var x = $(this).attr('class').split(" ");
        //		var order_id = x[0];
        //console.log(this);
        var order_id = this.innerText;
        // console.log(this.innerText);
        $('#tr_' + order_id + '_iframe_order_info').toggle();
        $('.' + order_id).toggleClass("tsim-lavender");
        $('#tr_' + order_id + '_iframe_order_info').toggleClass("tsim-lavender");
        for (var l = 1; l <= 3; l++) {
            $('.' + order_id + '_iframe_order_info_' + l).toggle();
            if ($('.' + order_id + '_' + l).length > 0) {
                var ex = $('.' + order_id + '_' + l).data("src");
                $('.' + order_id + '_' + l).css({
                    "background-image": "url('" + ex + "')",
                    "background-size": "contain",
                    "background-repeat": "no-repeat"
                });
            } else {
                $('.' + order_id + '_iframe_order_info_' + l).attr("src", $('.' + order_id + '_iframe_order_info_' + l).data("src"));
            }
        }
    });

    $('#shipping_list_tbody').on("click", ".swal_order_info", function(e) {
        //  console.log("above this");
        //  console.log(this);
        var order_id = this.innerText;

        //  console.log(this.innerText);
        $('#tr_' + order_id + '_shipment_info').toggle();
        $('.' + order_id).toggleClass("tsim-lavender");
        $('#tr_' + order_id + '_shipment_info').toggleClass("tsim-lavender");
        for (var l = 0; l <= 2; l++) {
            $('.' + order_id + '_shipment_info_' + l).toggle();
            if ($('.' + order_id + '_' + l).length > 0) {
                var ex = $('.' + order_id + '_' + l).data("src");
                $('.' + order_id + '_' + l).css({
                    "background-image": "url('" + ex + "')",
                    "background-size": "contain",
                    "background-repeat": "no-repeat"
                });
            } else {
                $('.' + order_id + '_shipment_info_' + l).attr("src", $('.' + order_id + '_shipment_info_' + l).data("src"));
            }
        }
        $.ajax({
            url: 'simmis.e?a=get_simno_shipping&o=' + order_id,
            type: 'GET',
            success: function(data) {
		 createSimOptions(order_id, data);
                 //console.log(data);
                //document.getElementById("simno_" + order_id).innerHTML = data;
        }
    });
});


    $('.all_docs').on("click", "input[type='checkbox']", function(e) {
        if ($(this).prop("checked") == true) {
            $("#documents_list_tbody").empty();
            show_all_docs();
            datebox.show("Report will show past 10 days records of document verification", "");
        } else if ($(this).prop("checked") == false) {
            $("#documents_list_tbody").empty();
            some_docs();
            datebox.show("Report will show past 45 days records of document verification", "");
        }
    });

	
    $('.all_imei').on("click", "input[type='checkbox']", function(e) {
        if ($(this).prop("checked") == true) {
            $("#imei_list_tbody").empty();
            show_all_imei();
            datebox.show("Report will show past 10 days records of verified/unverified IMEI", "");
        } else if ($(this).prop("checked") == false) {
            $("#imei_list_tbody").empty();
            check_imei();
            datebox.show("Report will show past 10 days records of unverified IMEI", "");
        }
    });

	


    $('#shipping_list_tbody').on("click", "input[type='submit']", function(e) {
        var id = this.id;
        var arr_val = id.split("_");
        var field = arr_val[1];
        var order = arr_val[2];
        var value = $("#comment_" + order).val();
        // console.log(field + order + value);
        $.ajax({
            url: 'simmis.e?a=update_user_info&p=' + value + '&f=' + field + '&o=' + order,
            type: 'GET',
            success: function(data) {
                if (data != "fail" && data == "comment") {
                    alert("Comment updated!");
                }
            }
        });
    });

/*	
    $('#imei_list_tbody').on("click", "input[type='submit']", function(e) {
        var id = this.id;
	//var tripid = document.getElementById('imeinumber').innerHTML;
	var row = $(this).closest('tr');
    	var imeiCell = row.find("td[id^='imeinumber_']");
    	var tripid = imeiCell.text();
	console.log(id);
        var arr_val = id.split("_");
	console.log(arr_val);
        var field = arr_val[1];
        var order = arr_val[2];
	var value = $("#imeicomment_" + order).val();

        console.log(field + order + value + tripid);
        $.ajax({
            url: 'simmis.e?a=update_user_info&p=' + value +  '&f=' + field + '&o=' + order + '&t=' + tripid,
            type: 'GET',
            success: function(data) {
                if (data != "fail" && data == "imeicomment") {
                    alert("Comment updated!");
                }
            }
        });
    });


	
	$('#imei_list_tbody').on("click", "input[type='submit']", function(e) {
        var id = this.id;
        //var tripid = document.getElementById('imeinumber').innerHTML;
        var row = $(this).closest('tr');
        var imeiCell = row.find("td[id^='imeinumber_']");
        var tripid = imeiCell.text();
        console.log(id);
        var arr_val = id.split("_");
        console.log(arr_val);
        var field = arr_val[1];
        var order = arr_val[2];
        var value = $("#imeinumber_" + order).val();

        console.log(field + order + value + tripid);
        $.ajax({
            url: 'simmis.e?a=update_user_info&p=' + value +  '&f=' + field + '&o=' + order + '&t=' + tripid,
            type: 'GET',
            success: function(data) {
                if (data != "fail" && data == "imeinumber") {
                    alert("IMEI Number updated!");
                }
            }
        });
    });
*/


$('#imei_list_tbody').on("click", "input[type='submit']", function(e) {
    var id = this.id;
    var row = $(this).closest('tr');
    var imeiCell = row.find("td[id^='imeinumber_']");
    var tripidElement = row.find('input[type="hidden"][id^="tripid_"]').attr('id');
    // Remove the 'tripid_' prefix
    var tripid = tripidElement.replace('tripid_', '');
    console.log(tripid);		

    console.log("tripid",tripid);
    console.log(id);
    var arr_val = id.split("_");
    console.log(arr_val);
    var field = arr_val[1];
    console.log(field);
    var order = arr_val[2];
    console.log(order);
    var value;
	  
    var row = $(this).closest('tr');
    var imeiCell = row.find("td[id^='imeinumber_']");
    var imei = imeiCell.text();
    console.log("imei");
    console.log(imei);

 
 
    if (field == 'imeicomment') 
    {       
    	console.log("taking comment value");
        value = $("#imeicomment_" + order).val();
	console.log(value);
    } 
    else if (field == 'imeinumber') 
    {       
       	console.log("taking imei number value");
        value = $("#imeinumberedit_" + order).val(); 
	console.log(value);
    }
    	
    console.log(field,order,value,tripid);

    $.ajax({
        url: 'simmis.e?a=update_user_info&p=' + value + '&f=' + field + '&o=' + order + '&t=' + tripid,
        type: 'GET',
        success: function(data) {
            if (data != "fail") {
                if (field == "imeicomment" && data == "imeicomment") {
                    alert("Comment updated!");
                } else if (field == "imeinumber" && data == "imeinumber") {
                    alert("IMEI Number updated!");
                }
            }
        }
    });
});







    $('#documents_list_tbody').on("click", "input[type='submit']", function(e) {
        var id = this.id;
        var arr_val = id.split("_");
        var field = arr_val[1];
        var order = arr_val[2];
        if (field == "pass") {
            var value = $("#passport_" + order).val();
        } else if (field == "city") {
            var value = $("#coi_" + order).val();
        } else if (field == "name") {
            var value = $("#tname_" + order).val();
        } else if (field == "exp") {
            var value = $("#exp_" + order).val();
        } else if (field == "comments") {
            var value = $("#comments_" + order).val();
        }

        $.ajax({
            url: 'simmis.e?a=update_user_info&p=' + value + '&f=' + field + '&o=' + order,
            type: 'GET',
            success: function(data) {
                if (data != "fail" && data == "pass") {
                    alert("Passport number updated in database!");
                }
                if (data != "fail" && data == "city") {
                    alert("City of issue updated in database!");
                }
                if (data != "fail" && data == "name") {
                    alert("Traveller's name updated in database!");
                }
                if (data != "fail" && data == "exp") {
                    alert("Expiry date updated in database!");
                }
                if (data != "fail" && data == "comments") {
                    alert("Comment updated in database!");
                }

            }
        });

    });
    $('#documents_list_tbody').on("click", "input[type='checkbox']", function(e) {
        var order_no = this.id;
        if ($(this).prop("checked") == true) {
            $.ajax({
                url: 'simmis.e?a=document_verified&o=' + order_no + '&f=t',
                type: 'GET',
                success: function(data) {
                    if (data != "fail") {
                        $('.' + data).attr("id", "documents_verified");
                    }
                }
            });
        } else if ($(this).prop("checked") == false) {
            $.ajax({
                url: 'simmis.e?a=document_verified&o=' + order_no + '&f=f',
                type: 'GET',
                success: function(data) {
                    if (data != "fail") {
                        $('.' + data).attr("id", "");
                    }
                }
            });
        }
    });

//IMEI Verification checkbox
$('#imei_list_tbody').on("click", "input[type='checkbox']", function(e) {
    var order_no = this.id;
  // Assuming the IMEI value is in the same row as the checkbox
    var row = $(this).closest('tr');
    var imeiCell = row.find("td[id^='imeinumber_']");
    var imei = imeiCell.text();
    var tripidElement = row.find('input[type="hidden"][id^="tripid_"]').attr('id');
    // Remove the 'tripid_' prefix
    var tripid = tripidElement.replace('tripid_', '');
    console.log(tripid);

	console.log(this);
	
    if ($(this).prop("checked") == true) {
        $.ajax({
            url: 'simmis.e?a=imei_verified&o=' + order_no + '&f=t' + '&t=' + tripid,
            type: 'GET',
            success: function(data) {
                if (data != "fail") {
                    if (data) {
                        $('.' + data).attr("id", "imei_verified");
                    } else {
			console.log(data);
                        console.log("Empty data:", data);
                        // Set background color to greenyellow if data is empty
                        $('#' + order_no).closest('tr').css('background-color', 'greenyellow');
                    }
                }
            }
        });
    } else if ($(this).prop("checked") == false) {
        $.ajax({
            url: 'simmis.e?a=imei_verified&o=' + order_no + '&f=f' + '&t=' + tripid,
            type: 'GET',
            success: function(data) {
                if (data != "fail") {
                    if (data) {
                        $('.' + data).attr("id", "");
                    } else {
			console.log(data);
                        console.log("Empty data:", data);
                        // Set background color to default if data is empty
                        $('#' + order_no).closest('tr').css('background-color', '');
                    }
                }
            }
        });
    }
});










    $('#shipping_list_tbody').on("click", "input[type='checkbox']", function(e) {
        var order_no = this.id;

        var item_type_e = document.getElementById("item_type_e");
        var item_type_p = document.getElementById("item_type_p");
        // Get the row (or parent container) of the clicked checkbox
        var row = $(this).closest('tr');

        // Check if the row has the `data-value` attributes for item types
        var item_type_e = row.find("#item_type_e");
        var item_type_p = row.find("#item_type_p");
        var item_type = "";

        if (item_type_e && (item_type_e.attr("data-value") == 'Esim')) {
            item_type = item_type_e.attr("data-value");
        } else if (item_type_p && (item_type_p.attr("data-value") == 'physical sim')) {
            item_type = item_type_p.attr("data-value");
        } else {
            item_type = "";
        }

        // console.log("ABOVE THE ITEM TYPE");

        // console.log(item_type);
        //var person = document.getElementById("telesales_person_"+order_no).value; 
        var person = getCookie("OMUSER");
        document.getElementById("telesales_person_" + order_no).innerHTML = "";
        document.getElementById("telesales_person_" + order_no).innerHTML = person;
        //console.log(person);
        if ($(this).prop("checked") == true) {
            $.ajax({
                url: 'simmis.e?a=shipment_done&o=' + order_no + '&p=' + person + '&f=t' + '&item=' + item_type,
                type: 'GET',
                success: function(data) {
                    if (data != "fail") {
                        $('.' + data).attr("id", "shipment_done");
                    }
                }
            });
        } else if ($(this).prop("checked") == false) {
            $.ajax({
                url: 'simmis.e?a=shipment_done&o=' + order_no + '&p=' + person + '&f=f' + '&item=' + item_type,
                type: 'GET',
                success: function(data) {
                    //console.log(data);
                    if (data != "fail") {
                        $('.' + data).attr("id", "");
                    }
                }
            });
        }
    });













    function getCookie(cname) {
        var name = cname + "=";
        var decodedCookie = decodeURIComponent(document.cookie);
        var ca = decodedCookie.split(';');
        for (var i = 0; i < ca.length; i++) {
            var c = ca[i];
            while (c.charAt(0) == ' ') {
                c = c.substring(1);
            }
            if (c.indexOf(name) == 0) {
                return c.substring(name.length, c.length);
            }
        }
        return "";
    }

    $('#documents_list_table').on("click", "a", function(e) {
        // var url = "https://mail004.ownmail.com/bc.e?i=" + (this.id.includes('/') ? this.id.split('/')[1] : this.id) + "&d=tsim_doc&a=U&dl=yes&l=services";
        // if (this.id.includes('/')) {
        //     var subd = this.id.split('/')[0];
        //     url = url.replace("d=tsim_doc", "d=tsim_doc/" + subd);
        // }
        // var img_ext = ['jpg', 'jpeg', 'png'];
        // var pdf_ext = ['pdf'];

        // if (img_ext.includes((this.id).split(".").pop())) {

        //     Swal.fire({
        //         type: 'info',
        //         title: 'Image File',
        //         imageUrl: url,
        //         imageHeight: 500,
        //         width: 500,
        //         imageAlt: 'Loading File....'

        //     })



        // } else if (pdf_ext.includes((this.id).split(".").pop())) {
        //     setTimeout(function() {

        //         Swal.fire({
        //             type: 'info',
        //             width: 700,
        //             title: '<strong>PDF File </strong>',
        //             padding: '0em',
        //             html: '<div id="show_container"></div>',

        //             onBeforeOpen: () => {

        //                 check(url)

        //             },


        //         })

        //     }, 300);
        // }

        // //      console.log(url); 
        console.log(this.id);
        var url = "https://mail004.ownmail.com/bc.e?i=" + (this.id.includes('/') ? this.id.split('/')[1] : this.id) + "&d=tsim_doc";
        url += "&a=U&dl=yes&l=services";
        if (this.id.includes('/')) {
            var subdirectory = this.id.split('/')[0];
            url = url.replace("d=tsim_doc", "d=tsim_doc/" + subdirectory);
        }
        var img_ext = ['jpg', 'jpeg', 'png'];

        if (img_ext.includes((this.id).split(".").pop())) {

            Swal.fire({
                icon: 'info',
                title: 'Image File',
                imageUrl: url,
                imageHeight: 500,
                width: 500,
                imageAlt: 'Loading File....'
            })
        } else {
        var iframe = document.createElement('iframe');
            iframe.src = url;
            iframe.style.width = '1000px';
            iframe.style.height = '600px'; 
            Swal.fire({
                html: iframe.outerHTML,
                showCloseButton: true,
                showConfirmButton: true,
                customClass: {
                    popup: 'custom-swal-popup'
                }
            });
        }      

    });
    $('#documents_list_table').on("click", "i", function(e) {
        var row_id = this.id.split("_");
        //console.log(row_id);
        //	if(row_id[0]=="rl"){
        var matrix = $("." + row_id[1] + "_iframe_order_info_" + row_id[2]).css("transform");
        if (matrix !== 'none') {
            //console.log(matrix);
            var values = matrix.split('(')[1].split(')')[0].split(',');
            var a = values[0];
            var b = values[1];
            //console.log(a);
            // console.log(b);
            var angle = Math.round(Math.atan2(b, a) * (180 / Math.PI));
        } else {
            var angle = 0;
        }
        console.log(angle);
        if (row_id[0] == "rl") {
            var la = angle - 90;
            // console.log("New angle = " + la);
            $("." + row_id[1] + "_iframe_order_info_" + row_id[2]).css({
                'transform': 'rotate(' + la + 'deg)'
            });
        } else if (row_id[0] == "rr") {
            var ra = angle + 90;
            //console.log("New angle = " + ra);
            $("." + row_id[1] + "_iframe_order_info_" + row_id[2]).css({
                'transform': 'rotate(' + ra + 'deg)'
            });
        }
    });



    $('#shipping_list_table').on("click", ".shipment_links", function(e) {
        //console.log(this);
        // console.log(this.id);
        var url = this.id;
        url = url.replace(/^http:\/\//i, 'https://');
        //console.log(this.innerHTML);
        //console.log(this.getAttribute('data'));
        var order_no = this.getAttribute('data');
        if (this.innerHTML == "Auto Bluedart Waybill") {
            $('.' + order_no + '_shipment_info_1').attr("src", url);
        }
        if (this.innerHTML == "Auto Shiprocket label") {
            $('.' + order_no + '_shipment_info_2').attr("src", url);
        }
        if (this.innerHTML == "Manual Bluedart") {
            $('.' + order_no + '_shipment_info_1').attr("src", url);
        }
        if (this.innerHTML == "Manual Shiprocket") {
            $('.' + order_no + '_shipment_info_2').attr("src", url);
        }
        $.ajax({
            url: 'simmis.e?a=get_shiptracking_no&o=' + order_no + '',
            type: 'GET',
            async: false,
            success: function(data) {
                // console.log(data);
                const arr = data.split(",");
                if (arr[2] != "") {
                    document.getElementById("" + order_no + "_waybill_no").innerHTML = arr[2] + "<br>(" + arr[3] + ")";
                    $("." + order_no + "").css("background-color", "");
                }
                if (arr[0] != "") {
                    document.getElementById("" + order_no + "_tracking_no").innerHTML = arr[0] + "<br>(" + arr[1] + ")";
                    $("." + order_no + "").css("background-color", "");
                }
            }
        });
    });
    $('#shipping_list_table').on("click", ".email_to_clara", function(e) {
        var id = this.id;
        console.log(this.id);
        const arr = id.split("_");
        var order = arr[0];
        var value = $("#comment_" + order).val();
        var sent_data;
        if (value == "") {
            alert("Cannot send email, comment section is empty!");
        } else {
            $.ajax({
                url: 'simmis.e?a=email_shipment_info&o=' + order + '&info=' + value,
                type: 'GET',
                async: false,
                success: function(data) {
                    //  console.log(data);
                    sent_data = data.split(",");
                    if (sent_data[0] == "Sent") {
                        alert("Information sent to Clara and Vinanti!");
                        document.getElementById("" + order + "_emailsent").innerHTML = "Sent(" + sent_data[1] + ")";
                    } else if (data == "Failed") {
                        alert("Failed to send email.");
                    }
                }
            });
        }
    });



    function show_shipments() {
        var from_dt = document.getElementById("active_date").value;
        var to_dt = document.getElementById("to_date_act_form").value;
        var today = new Date();
        var dd = today.getDate();
        var mm = today.getMonth() + 1;
        var yyyy = today.getFullYear();
        var params, t, f;
        if (dd < 10) {
            dd = '0' + dd;
        }

        if (mm < 10) {
            mm = '0' + mm;
        }
        today = yyyy + '-' + mm + '-' + dd;
        // console.log("Today = " + today);
        // console.log(from_dt);
        //console.log(to_dt);
        if (from_dt == today && to_dt == "") {
            params = "a=shipment_summary&b=query&t=none&f=none";
            // console.log("From date is today, params =" + params);
            datebox.show("Report will show past 5 days records of packed orders and past 45 days of unpacked orders", "");
        } else if (from_dt != today && to_dt == "") {
            f = from_dt + "T24:00:00";
            t = from_dt;
            params = "a=shipment_summary&b=query&t=" + t + "&f=" + f;
            // console.log("From date changed, to date is empty, params =" + params);
            datebox.show("Report will show packed/unpacked orders of " + t + "", "");
        } else if (to_dt != "") {
            t = to_dt;
            f = from_dt + "T24:00:00";
            params = "a=shipment_summary&b=query&t=" + t + "&f=" + f;
            // console.log("From date changed, to date changed, params =" + params);
            datebox.show("Report will show packed/unpacked orders from " + t + " to " + f + "", "");
        }
        $("#shipping_list_tbody").empty();
        //		datebox.show("Report will show past 5 days records of unverified documents", "");
        msgbox.show("Please wait ...");
        $.ajax({
            url: 'simmis.e?' + params + '',
            type: 'GET',
            async: false,
            success: function(data) {
                var ship_flag, usr_data, order_no, booking_date, shipping_methods, tracking_no, waybill_no, comment, no_of_documents_uploaded, label_created_at, bluedart_label_created_at, documents_uploaded, bluedartlink, fedexlink, final_flag, sales_person, d_city, email_sent, item_type;
                const arr = data.split("|");
                var count = 0;
                var sales_staff = ["select", "alisha", "archana", "snehal", "prashantk", "rohits", "muthuraj", "santosh", "anamoy"];
                for (var j = 0; j < arr.length; j++) {
                    console.log(arr[j]);
                    if (arr[j] != "") {
                        count = count + 1;
                        msgbox.show(count + " Records Found.", "noerror");
                        usr_data = arr[j].split("`");
                        order_no = usr_data[0];
                        booking_date = usr_data[1];
                        shipping_methods = usr_data[2];
                        if (shipping_methods.includes("Local Pickup")) {
                            shipping_methods = "Local Pickup";
                            //		console.log(shipping_methods);
                        }
                        tracking_no = usr_data[3];
                        waybill_no = usr_data[4];
                        comment = usr_data[5];
                        comment = comment.replace(/'/g, '&#39;');
                        no_of_documents_uploaded = usr_data[6];
                        label_created_at = usr_data[7];
                        bluedart_label_created_at = usr_data[8];
                        documents_uploaded = usr_data[9];
                        bluedartlink = usr_data[10];
                        fedexlink = usr_data[11];
                        final_flag = usr_data[12];
                        sales_person = usr_data[13];
                        d_city = usr_data[14];
                        email_sent = usr_data[15];
                        item_type = usr_data[16];
                        console.log("Email sent - " + email_sent);
                        //		console.log(sales_person);
                        if (final_flag == 't') {
                            var rw = "<tr id='shipment_done' class='" + order_no + "'>";
                            rw += "<td><input type = 'checkbox' name = '" + order_no + "' id = '" + order_no + "' checked></td>";
                        } else if (final_flag == 'f' && tracking_no == "" && waybill_no == "") {
                            var rw = "<tr class='" + order_no + "' style='background-color : lightskyblue;'>";
                            rw += "<td><input type = 'checkbox' name = '" + order_no + "' id = '" + order_no + "'></td>";
                        } else {
                            var rw = "<tr class='" + order_no + "'>";
                            rw += "<td><input type = 'checkbox' name = '" + order_no + "' id = '" + order_no + "'></td>";
                        }
                        rw += "<td class='swal_order_info'><p id='swal_" + order_no + "_info'>" + order_no + "</p></td>";
                        rw += "<td>" + booking_date + "</td>";
                        rw += "<td>" + shipping_methods + "</td>";
                        rw += "<td>" + d_city + "</td>";
                        if (waybill_no != "") {
                            rw += "<td id='" + order_no + "_waybill_no'>" + waybill_no + "<br>(" + bluedart_label_created_at + ")</td>";
                        } else {
                            rw += "<td id='" + order_no + "_waybill_no'>-</td>";
                        }
                        if (tracking_no != "") {
                            rw += "<td id='" + order_no + "_tracking_no'>" + tracking_no + "<br>(" + label_created_at + ")</td>";
                        } else {
                            rw += "<td id='" + order_no + "_tracking_no'>-</td>";
                        }
                        if (shipping_methods.includes('Airport Pickup') || (shipping_methods.includes('Cash') && d_city != 'Mumbai') || (shipping_methods.includes('Same Business Day (if ordered before 6PM)') && d_city == 'Mumbai')) {
                            /*	rw += "<td><select id='telesales_person_"+order_no+"'>";
                            	for(var z=0;z<sales_staff.length;z++){
                            	if(sales_staff[z] == sales_person){
                            	rw += "<option value='"+sales_staff[z]+"' selected>"+sales_staff[z]+"</option>";
                            	}
                            	else{
                            	rw += "<option value='"+sales_staff[z]+"'>"+sales_staff[z]+"</option>";
                            	}
                            	}
                            	rw += "</select>";*/
                            //<option>Select</option><option value='alisha'>alisha</option><option value='archana'>archana</option><option value='rohits'>rohits</option><option value='muthuraj'>muthuraj</option><option value='snehal'>snehal</option><option value='prashantk'>prashantk</option><option value='santosh'>santosh</option></select>";
                            //					rw +="<input id='comment_"+order_no+"' type='text' value='"+comment+"'><input type = 'submit' style='width:5%;' id = 'update_comment_"+order_no+"' value='&#10003'></td>";
                            rw += "<td><b style='margin-right:5px;' id='telesales_person_" + order_no + "'>" + sales_person + "</b>";
                            rw += "<input id='comment_" + order_no + "' type='text' value='" + comment + "'><input type = 'submit' style='width:5%;' id = 'update_comment_" + order_no + "' value='&#10003'><p><a class='email_to_clara' id='" + order_no + "_email' href='#' style='color: blue;' onclick='return false'>Email to Clara/Vinanti</a></p><p id='" + order_no + "_emailsent'>Sent(" + email_sent + ")</p></td>";
                        } else {
                            /*	rw += "<td><select id='telesales_person_"+order_no+"'>";
                            //<option>Select</option><option value='alisha'>alisha</option><option value='archana'>archana</option><option value='rohits'>rohits</option><option value='muthuraj'>muthuraj</option><option value='snehal'>snehal</option><option value='prashantk'>prashantk</option><option value='santosh'>santosh</option></select>";
                            for(var z=0;z<sales_staff.length;z++){
                            if(sales_staff[z] == sales_person){
                            rw += "<option value='"+sales_staff[z]+"' selected>"+sales_staff[z]+"</option>";
                            }
                            else{
                            rw += "<option value='"+sales_staff[z]+"'>"+sales_staff[z]+"</option>";
                            }
                            }
                            rw += "</select>";*/
                            rw += "<td><b style='margin-right:5px;' id='telesales_person_" + order_no + "'>" + sales_person + "</b>";
                            rw += "<input id='comment_" + order_no + "' type='text' value='" + comment + "'><input type = 'submit' style='width:5%;' id = 'update_comment_" + order_no + "' value='&#10003'></td>";
                            //					rw += "<input id='comment_"+order_no+"' type='text' value='"+comment+"'><input type = 'submit' style='width:5%;' id = 'update_comment_"+order_no+"' value='&#10003'><p><a class='email_to_clara' id='"+order_no+"_email' href='#' style='color: blue;' onclick='return false'>Email to Clara/Vinanti</a></p></td>";
                        }
                        if (documents_uploaded == 't') {
                            rw += "<td id='docs_verified'>" + no_of_documents_uploaded + "</td>";
                        } else {
                            rw += "<td>" + no_of_documents_uploaded + "</td>";
                        }
                        if (item_type == 'Esim') {
                            rw += "<td id='item_type_e' data-value='" + item_type + "'>" + item_type + "</td>";
                        } else {
                            rw += "<td id='item_type_p' data-value='" + item_type + "'>" + item_type + "</td>";
                        }
                        rw += "</tr>";
                        rw += "<tr id='tr_" + order_no + "_shipment_info' style='display:none;'>";
//                        rw += "<td colspan='5'><table style='width: -moz-available;'><tbody><tr><td id='simno_" + order_no + "'></td></tr><tr><td><button onclick='showSimInfoModal()'>Enter Sim Number</button> </td></tr><tr>";
			rw += "<td colspan='5'><table style='width: -moz-available;'><tbody><tr><td id='simno_" + order_no + "'></td></tr>";

                        if (order_no.includes("-")) {
                            rw += "<td><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_shipment_info_0' style='display:none;' src='about:blank' data-src='about:blank'></iframe></div></td>";
                        } else {
                            rw += "<td><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_shipment_info_0' style='display:none;' src='about:blank' data-src='https://www.tsim.in/wp-admin/post.php?post=" + order_no + "&action=edit'></iframe></div></td>";
                        }
                        rw += "</tr></tbody></table></td>";
                        rw += "<td colspan='4'><table style='width: -moz-available;'><tbody>";
                        rw += "<tr><td><a class='shipment_links' href='#' id='https://mail004.ownmail.com/tsim/bluedartAmazon.php' data='" + order_no + "' onclick='return false'>Manual Bluedart</a></td>";
                        if (bluedartlink != "") {
                            rw += "<td><p><a class='shipment_links' href='#' id='" + bluedartlink + "' data='" + order_no + "' onclick='return false'>Auto Bluedart Waybill</a></p></td>";
                        } else {
                            rw += "<td><p>-</p></td>";
                        }
                        rw += "<td><a class='shipment_links' href='#' id='https://mail004.ownmail.com/tsim/amazonFedex.php' data='" + order_no + "' onclick='return false'>Manual Shiprocket</a></td>";
                        if (fedexlink != "") {
                            rw += "<td><p><a class='shipment_links' href='#' id='" + fedexlink + "' data='" + order_no + "' onclick='return false'>Auto Shiprocket label</a></p></td>";
                        } else {
                            rw += "<td><p>-</p></td>";
                        }
                        rw += "</tr><tr>";

                        if (waybill_no != "") {
                            rw += "<td colspan='2'><div class='panel'><iframe id='bluedart_iframe_" + order_no + "' cellspacing='0' frameborder='0' class='" + order_no + "_shipment_info_1' style='display:none;' src='about:blank' data-src='https://mail004.ownmail.com/bc.e?i=Waybill_" + waybill_no + "_" + order_no + ".pdf&f=6377&d=shipments'></iframe></div></td>";
                        } else {
                            rw += "<td colspan='2'><div class='panel'><iframe id='bluedart_iframe_" + order_no + "' cellspacing='0' frameborder='0' class='" + order_no + "_shipment_info_1' style='display:none;' src='about:blank'></iframe></div></td>";
                        }
                        if (tracking_no != "") {
                            rw += "<td colspan='2'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_shipment_info_2' style='display:none;' src='about:blank' data-src='https://mail004.ownmail.com/bc.e?i=Waybill_" + tracking_no + "_" + order_no + ".pdf&f=6377&d=shipments'></iframe></div></td>";
                        } else {
                            rw += "<td colspan='2'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_shipment_info_2' style='display:none;' src='about:blank'></iframe></div></td>";
                        }
                        //		rw += "<td colspan='1'><p><a class='shipment_links' href='#' id='"+bluedartlink+"' data='"+order_no+"' onclick='return false'>Bluedart Waybill</a></p><br><p><a class='shipment_links' href='#' id='"+fedexlink+"' data='"+order_no+"' onclick='return false'>Fedex Ship label</a></p></td>";

                        rw += "</tr></tbody></table></td></tr>";
                        //$("#telesales_person_"+order_no+" option:checked").val(sales_person);
                        //		document.getElementById('#telesales_person_'+order_no+'').value = sales_person;

                        $("#shipping_list_table > tbody").append(rw);
                        //						document.getElementById('#telesales_person_'+order_no+'').value = sales_person;
                    }
                }
            }
        });
    }





function check_imei()
{
        console.log("GOT THE FUNCTION CALL ");
	datebox.show("Report will show past 10 days records of unverified IMEI", "");
        msgbox.show("Please wait ...");
	$.ajax({
        	url: 'simmis.e?a=check_imei&b=query',
            	type: 'GET',
		dataType: 'json',
            	async: false,
            	success: function(data) 
		{
			 // Clear the table body before adding new rows
                        $("#imei_list_table > tbody").empty();				

			console.log("inside ajax request ");
			console.log(data);
			var count = 0;

			data.forEach(function(item) 
			{
				count = count + 1;
                               	msgbox.show(count + " Records Found.", "noerror");

				let imei_flag = item.imei_verified_flag.replace(/["()]/g, '');
            			let order_no = item.order_no.replace(/["()]/g, '');
            			let emailadd = item.emailadd.replace(/["()]/g, '');
            			let activation_date = item.activation_date.replace(/["()]/g, '');
            			let booking_date = item.booking_date.replace(/["()]/g, '');
            			let imei = item.imei.replace(/["()]/g, '');
            			let imeicomment = item.comment.replace(/["'()]/g, '');
				let tripid = item.tripid;
				

            			console.log(imei_flag, order_no, emailadd, activation_date, booking_date, imei, imeicomment ,tripid);
			
			   	if (imei_flag == 't') 
                               	{
					if(order_no)
					{
                                		var rw = "<tr id='imei_verified' class='" + order_no + "' style='background-color: greenyellow;'>";
                                        	rw += "<td><input type = 'checkbox' checked name = '" + order_no + "' id = '" + order_no + "'></td>";
					}
					else
					{
						var rw = "<tr id='imei_verified' class='" + imei + "' style='background-color: greenyellow;'>";
                                                rw += "<td><input type = 'checkbox' checked name = '" + imei + "' id = '" + imei + "'></td>";
					}
                               	} 
                                else 
                               	{	
					if(order_no)
					{
                                       		var rw = "<tr class='" + order_no + "'>";
                                        	rw += "<td><input type = 'checkbox' name = '" + order_no + "' id = '" + order_no + "'></td>";
					}
					else
					{
						var rw = "<tr class='" + imei + "'>";
                                                rw += "<td><input type = 'checkbox' name = '" + imei + "' id = '" + imei + "'></td>";
					}
                                }
                                        
				rw += "<td>" + order_no + "</td>";                      
                                rw += "<td>" + emailadd + "</td>";
                                rw += "<td>" + activation_date + "</td>";
                                rw += "<td>" + booking_date + "</td>";
				
				//imei number
				if(order_no)
				{
                             		rw += "<td id='imeinumber_"+imei+"'><input id='imeinumberedit_" + order_no + "' type='text' value='" + imei + "'><input type = 'submit' style='width:8%;' id ='update_imeinumber_" + order_no +"' value='&#10003'></td>";
				}
				else
				{
					rw += "<td id='imeinumber_"+imei+"'><input id='imeinumberedit_" + imei + "' type='text' value='" + imei + "'><input type = 'submit' style='width:8%;' id ='update_imeinumber_" + imei +"' value='&#10003'></td>";
				}
				
				//imei comment
				if(order_no)
				{
                               		rw += "<td><input id='imeicomment_" + order_no + "' type='text' value='" + imeicomment + "' style='width:90%;'><input type = 'submit' style='width:8%;' id = 'update_imeicomment_" + order_no + "' value='&#10003'></td>";
				}
				else
				{
					 rw += "<td><input id='imeicomment_" + imei + "' type='text' value='" + imeicomment + "' style='width:90%;'><input type = 'submit' style='width:8%;' id = 'update_imeicomment_" + imei + "' value='&#10003'></td>";
				}
			
					
					
				rw += "<td><a href='#' class='send-email' data-url='https://mail004.ownmail.com/tsim/send_imei_notcompatible_email.php' data-order_no='" + encodeURIComponent(order_no) + "' data-emailadd='" + encodeURIComponent(emailadd) + "' data-imei='" + encodeURIComponent(imei) + "'>Not Compatible <br>Email</a></td>";


				rw += "<input type='hidden' id='tripid_" + tripid + "' value='" + tripid + "'>";	
                                rw += "</tr>";
                                $("#imei_list_table > tbody").append(rw);
			});
		}	
		});
}






$(document).ready(function() {
    $('body').on('click', '.send-email', function(e) {
        e.preventDefault();  // Prevent the default link behavior
        var url = $(this).data('url');  // Get the URL from the data-url attribute
	console.log(url);
	var emailadd = $(this).data('emailadd'); // Get the email from the data-email attribute
	console.log(emailadd);
        var imei = $(this).data('imei'); // Get the IMEI number from the data-imei attribute
	console.log(imei);
	var order_no = $(this).data('order_no');
	console.log(order_no);
        // Show confirmation dialog
        if (confirm("Do you want to send IMEI is not compatible email?")) {
            $.ajax({
                url: url,
                type: 'GET',
		 data: {
                    emailadd: emailadd,
                    imei: imei,
			order_no:order_no
                },
                success: function(response) {
                   var jsonResponse;
                    try {
                        jsonResponse = JSON.parse(response);
                    } catch (e) {
                        jsonResponse = { status: 'fail' };  // Default to fail if response is not valid JSON
                    }

                    if (jsonResponse.status === 'success') {
                        alert(jsonResponse.message);
                    } else {
                        alert(jsonResponse.message);
                    }
                },
                error: function(xhr, status, error) {
                    alert("An error occurred: " + error);
                }
            });
        }
    });
});







function show_all_imei()
{
	console.log("INSIDE show_all_imei function");
	msgbox.show("Please wait ...");
        datebox.show("Report will show all verified/unverified IMEI's of past 5 days", "");
	$.ajax({
                url: 'simmis.e?a=check_all_imei&b=query',
                type: 'GET',
		dataType: 'json',
                async: false,
                success: function(data)
                {

		 	$("#imei_list_table > tbody").empty();

                        console.log("inside ajax request ");
                        console.log(data);
                        var count = 0;

                        data.forEach(function(item)
                        {
                                count = count + 1;
                                msgbox.show(count + " Records Found.", "noerror");

                                let imei_flag = item.imei_verified_flag.replace(/["()]/g, '');
                                let order_no = item.order_no.replace(/["()]/g, '');
                                let emailadd = item.emailadd.replace(/["()]/g, '');
                                let activation_date = item.activation_date.replace(/["()]/g, '');
                                let booking_date = item.booking_date.replace(/["()]/g, '');
                                let imei = item.imei.replace(/["()]/g, '');
                                let imeicomment = item.comment.replace(/["'()]/g, '');
				let tripid = item.tripid;


                                console.log(imei_flag, order_no, emailadd, activation_date, booking_date, imei, imeicomment ,tripid);

                                if (imei_flag == 't')
                                {
					if(order_no)
					{
                                        	var rw = "<tr id='imei_verified' class='" + order_no + "' style='background-color: greenyellow;'>";
                                        	rw += "<td><input type = 'checkbox' checked name = '" + order_no + "' id = '" + order_no + "'></td>";
					}
					else
					{
						var rw = "<tr id='imei_verified' class='" + imei + "' style='background-color: greenyellow;'>";
                                                rw += "<td><input type = 'checkbox' checked name = '" + order_no + "' id = '" + order_no + "'></td>";
					}
                                }
                                else
                                {
					if(order_no)
					{
                                        	var rw = "<tr class='" + order_no + "'>";
                                        	rw += "<td><input type = 'checkbox' name = '" + order_no + "' id = '" + order_no + "'></td>";
					}
					else
					{
						var rw = "<tr class='" + imei + "'>";
						rw += "<td><input type = 'checkbox' name = '" + order_no + "' id = '" + order_no + "'></td>";
					}
                                }

                                rw += "<td>" + order_no + "</td>";
                                rw += "<td>" + emailadd + "</td>";
                                rw += "<td>" + activation_date + "</td>";
                                rw += "<td>" + booking_date + "</td>";
	
				//imei number
				if(order_no)
				{
                                	rw += "<td id='imeinumber_"+imei+"'><input id='imeinumberedit_" + order_no + "' type='text' value='" + imei + "'><input type = 'submit' style='width:8%;' id ='update_imeinumber_" + order_no +"' value='&#10003'></td>";
				}
				else
				{
					rw += "<td id='imeinumber_"+imei+"'><input id='imeinumberedit_" + imei + "' type='text' value='" + imei + "'><input type = 'submit' style='width:8%;' id ='update_imeinumber_" + imei +"' value='&#10003'></td>";
				}
				
				//imei comment
				if(order_no)
				{
                                	rw += "<td><input id='imeicomment_" + order_no + "' type='text' value='" + imeicomment + "' style='width:90%;'><input type = 'submit' style='width:8%;' id = 'update_imeicomment_" + order_no + "' value='&#10003'></td>";
				}
				else
				{
					rw += "<td><input id='imeicomment_" + imei + "' type='text' value='" + imeicomment + "' style='width:90%;'><input type = 'submit' style='width:8%;' id = 'update_imeicomment_" + imei + "' value='&#10003'></td>";
				}
				
				rw += "<td><a href='#' class='send-email' data-url='https://mail004.ownmail.com/tsim/send_imei_notcompatible_email.php' data-email='" + encodeURIComponent(emailadd) + "' data-imei='" + encodeURIComponent(imei) + "''>Not Compatible <br>Email</a></td>";
				
				rw += "<input type='hidden' id='tripid_" + tripid + "' value='" + tripid + "'>";
                                rw += "</tr>";
                                $("#imei_list_table > tbody").append(rw);


	                	});
			}
                });	
}





    function extractFileName(filePath) {
        const parts = filePath.split('/');
        if (parts.length > 1) {
            return parts[parts.length - 1];
        } else {
            return filePath;
        }
    }


    function some_docs() {
        datebox.show("Report will show past 45 days records of unverified documents", "");
        msgbox.show("Please wait ...");
        $.ajax({
            url: 'simmis.e?a=document_summary&b=query',
            type: 'GET',
            async: false,
            success: function(data) {
                // console.log(data);
                var jshtml = '';
                var passport, travname, coi, doc_flag, order_no, user_arr, user_name, file1, file2, file3, order_date, expd, order_booking_date, from_date, comments, customerphoneno;
                const arr = data.split("|");
                var count = 0;
                for (var j = 0; j < arr.length; j++) {
                    console.log(arr[j]);
                    if (arr[j] != "Success" && arr[j] != "") {
                        user_arr = arr[j].split(",");
                        order_no = user_arr[0];
                        user_name = user_arr[1];
                        doc_flag = user_arr[2];
                        passport = user_arr[3];
                        travname = user_arr[4];
                        coi = user_arr[5];
                        expd = user_arr[6];
                        order_booking_date = user_arr[7];
                        from_date = user_arr[8];
                        comments = user_arr[9];
                        customerphoneno = user_arr[10];
                        var file_array = [];
                        for (var i = 11; i <= 13; i++) {
                            if (user_arr[i] !== undefined) {
                                file_array.push(user_arr[i]);
                            }
                        }
                        if (file_array.length <= 3) {
                            count = count + 1;
                            msgbox.show(count + " Records Found.", "noerror");
                            if (doc_flag == 't') {
                                var rw = "<tr id='documents_verified' class='" + order_no + "'>";
                            } else {
                                var rw = "<tr class='" + order_no + "'>";
                            }
                            rw += "<td><input type = 'checkbox' name = '" + order_no + "' id = '" + order_no + "'></td>";
                            rw += "<td class='swal_order_info'><p id='swal_" + order_no + "_info'>" + order_no + "</p></td>";
                            rw += "<td>" + user_name + "</td>";
                            rw += "<td><input type = 'text' style='width:70%;' id ='passport_" + order_no + "' value='" + passport + "'><input type = 'submit' style='width:10%;' id = 'update_pass_" + order_no + "' value='&#10003'></td>";
                            rw += "<td><input type = 'text' style='width:70%;' id ='tname_" + order_no + "' value='" + travname + "'><input type = 'submit' style='width:10%;' id = 'update_name_" + order_no + "' value='&#10003'></td>";
                            rw += "<td><input type = 'text' style='width:70%;' id ='coi_" + order_no + "' value='" + coi + "'><input type = 'submit' style='width:10%;' id = 'update_city_" + order_no + "' value='&#10003'></td>";
                            rw += "<td><input type = 'date' style='width:80%;' id ='exp_" + order_no + "' value='" + expd + "'><input type = 'submit' style='width:10%;' id = 'update_exp_" + order_no + "' value='&#10003'></td>";

                            for (var i = 11; i <= 13; i++) {
                                // Check if the file exists
                                if (user_arr[i]) {
                                    var file = user_arr[i];
                                    var extension = file.substr((file.lastIndexOf('.') + 1));

                                    // Construct the link based on the file extension
                                    if (extension === 'docx' || extension === 'doc') {
                                        rw += "<td><h4><a href='https://mail004.ownmail.com/bc.e?i=" + (file.includes('/') ? file.split('/')[1] : file) + "&f=2500&d=a&a=U&dl=yes&l=services' onclick='return false'>Doc " + (i - 10) + "</a> (.doc file)</h4></td>";
                                    } else {
                                        rw += "<td><h4><a href='https://mail004.ownmail.com/bc.e?i=" + (file.includes('/') ? file.split('/')[1] : file) + "&f=2500&d=a&a=U&dl=yes&l=services' id='" + (file.includes('/') ? file.split('/')[1] : file) + "' onclick='return false'>Doc " + (i - 10) + "</a></h4></td>";
                                    }

                                    // Check if the file path contains a subdirectory
                                    if (file.includes('/')) {
                                        var subdirectory = file.split('/')[0];
                                        rw = rw.replace("d=a", "d=tsim_doc/" + subdirectory);
                                    }
                                } else {
                                    rw += "<td><h4>Null</h4></td>";
                                }
                            }
                            rw += "<td>" + order_booking_date + "</td>";
                            rw += "<td>" + from_date + "</td>";
                            rw += "<td><input id='comments_" + order_no + "' type='text' value='" + comments + "'><input type = 'submit' style='width:8%;' id = 'update_comments_" + order_no + "' value='&#10003'></td>";
                            //   console.log("-" + customerphoneno + "-");
                            var mobno = customerphoneno.substring(2);
                            console.log("-" + mobno + "-");
                            rw += "<td><a href='#' onclick=\"openTab('https://trikondomestic.cc.warmconnect.com/vicidial/non_agent_api.php?source=test&user=api&pass=apitasting2pword&function=add_lead&phone_number=" + mobno + "&phone_code=91&list_id=24216344&dnc_check=Y&campaign_dnc_check=Y&campaign_id=BROTSIM&add_to_hopper=Y&hopper_local_call_time_check=Y')\">Add</a><br><br> <a href='#' onclick=\"openTab('https://trikondomestic.cc.warmconnect.com/vicidial/non_agent_api.php?source=test&user=api&pass=apitasting2pword&function=update_lead&search_location=SYSTEM&search_method=PHONE_NUMBER&phone_number=" + mobno + "&delete_lead=Y')\">Delete</a></td>";


                            rw += "</tr>";
                            rw += "<tr id='tr_" + order_no + "_iframe_order_info' style='display:none;'>";
                            //   console.log(user_arr[10] + user_arr[11] + user_arr[12]);
                            if (order_no.includes("-")) {
                                rw += "<td colspan='1'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_iframe_order_info_0' style='display:none;' src='about:blank' data-src='about:blank'></iframe></div></td>";
                            } else {
                                rw += "<td colspan='1'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_iframe_order_info_0' style='display:none;' src='about:blank' data-src='https://www.tsim.in/wp-admin/post.php?post=" + order_no + "&action=edit'></iframe></div></td>";
                            }

                            for (var i = 11; i <= 13; i++) {
                                // Check if the file exists
                                if (user_arr[i]) {
                                    var file = user_arr[i];
                                    var extension = file.substr((file.lastIndexOf('.') + 1));

                                    // Check if the file is an image (JPEG or JPG)
                                    if (extension === 'jpeg' || extension === 'JPEG' || extension === 'jpg' || extension === 'JPG') {
                                        rw += "<td colspan='4'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_iframe_order_info_" + (i - 10) + "' style='display:none;' src='about:blank' data-src='https://mail004.ownmail.com/bc.e?i=" + (file.includes('/') ? file.split('/')[1] : file) + "&f=2500&d=b&a=U&dl=yes&l=services'></iframe></div>";
                                        if (file.includes('/')) {
                                            var sub = file.split('/')[0];
                                            rw = rw.replace("d=b", "d=tsim_doc/" + sub);
                                        }
                                        rw += "<i class='fa fa-rotate-left' id='rl_" + order_no + "_" + (i - 10) + "'></i><i class='fa fa-rotate-right' id='rr_" + order_no + "_" + (i - 10) + "'></i></td>";
                                    } else {
                                        rw += "<td colspan='4'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_iframe_order_info_" + (i - 10) + "' style='display:none;' src='about:blank' data-src='https://mail004.ownmail.com/bc.e?i=" + (file.includes('/') ? file.split('/')[1] : file) + "&f=2500&d=b&a=U&dl=yes&l=services'></iframe></div>";
                                        if (file.includes('/')) {
                                            var sub = file.split('/')[0];
                                            rw = rw.replace("d=b", "d=tsim_doc/" + sub);
                                        }
                                        rw += "<i class='fa fa-rotate-left' id='rl_" + order_no + "_" + (i - 10) + "'></i><i class='fa fa-rotate-right' id='rr_" + order_no + "_" + (i - 10) + "'></i></td>";
                                    }
                                }
                            }
                            rw += "</tr>";
                            $("#documents_list_table > tbody").append(rw);
                        }
                    }
                }
            }
        });
    }

    function show_all_docs() {
        msgbox.show("Please wait ...");
        datebox.show("Report will show all verified/unverified documents of past 10 days", "");
        $.ajax({
            url: 'simmis.e?a=all_document_summary&b=query',
            type: 'GET',
            async: false,
            success: function(data) {
                console.log(data);
                var jshtml = '';
                var passport, travname, coi, doc_flag, order_no, user_arr, user_name, file1, file2, file3, order_date, expd, order_booking_date, from_date, comments, customerphoneno;
                const arr = data.split("|");
                var count = 0;
                for (var j = 0; j < arr.length; j++) {
                    console.log(arr[j]);
                    if (arr[j] != "Success" && arr[j] != "") {
                        user_arr = arr[j].split(",");
                        order_no = user_arr[0];
                        user_name = user_arr[1];
                        doc_flag = user_arr[2];
                        passport = user_arr[3];
                        travname = user_arr[4];
                        coi = user_arr[5];
                        expd = user_arr[6];
                        order_booking_date = user_arr[7];
                        from_date = user_arr[8];
                        comments = user_arr[9];
                        customerphoneno = user_arr[10];
                        var file_array = [];
                        for (var i = 11; i <= 13; i++) {
                            if (user_arr[i] !== undefined) {
                                file_array.push(user_arr[i]);
                            }
                        }
                        if (file_array.length <= 3) {
                            count = count + 1;
                            msgbox.show(count + " Records Found.", "noerror");
                            if (doc_flag == 't') {
                                var rw = "<tr id='documents_verified' class='" + order_no + "'>";
                            } else {
                                var rw = "<tr class='" + order_no + "'>";
                            }
                            rw += "<td><input type = 'checkbox' name = '" + order_no + "' id = '" + order_no + "'></td>";
                            rw += "<td class='swal_order_info'><p id='swal_" + order_no + "_info'>" + order_no + "</p></td>";
                            rw += "<td>" + user_name + "</td>";
                            rw += "<td><input type = 'text' style='width:70%;' id ='passport_" + order_no + "' value='" + passport + "'><input type = 'submit' style='width:10%;' id = 'update_pass_" + order_no + "' value='&#10003'></td>";
                            rw += "<td><input type = 'text' style='width:70%;' id ='tname_" + order_no + "' value='" + travname + "'><input type = 'submit' style='width:10%;' id = 'update_name_" + order_no + "' value='&#10003'></td>";
                            rw += "<td><input type = 'text' style='width:70%;' id ='coi_" + order_no + "' value='" + coi + "'><input type = 'submit' style='width:10%;' id = 'update_city_" + order_no + "' value='&#10003'></td>";
                            rw += "<td><input type = 'date' style='width:80%;' id ='exp_" + order_no + "' value='" + expd + "'><input type = 'submit' style='width:10%;' id = 'update_exp_" + order_no + "' value='&#10003'></td>";

                            for (var i = 11; i <= 13; i++) {
                                // Check if the file exists
                                if (user_arr[i]) {
                                    var file_name1 = user_arr[i];
                                    var extension = file_name1.substr((file_name1.lastIndexOf('.') + 1));

                                    // Construct the link based on the file_name1 extension
                                    if (extension === 'docx' || extension === 'doc') {
                                        rw += "<td><h4><a href='https://mail004.ownmail.com/bc.e?i=" + (file_name1.includes('/') ? file_name1.split('/')[1] : file_name1) + "&f=2500&d=c&a=U&dl=yes&l=services' onclick='return false'>Doc " + (i - 10) + "</a> (.doc file_name1)</h4></td>";
                                    } else {
                                        rw += "<td><h4><a href='https://mail004.ownmail.com/bc.e?i=" + (file_name1.includes('/') ? file_name1.split('/')[1] : file_name1) + "&f=2500&d=c&a=U&dl=yes&l=services' id='" + (file_name1.includes('/') ? file_name1.split('/')[1] : file_name1) + "' onclick='return false'>Doc " + (i - 10) + "</a></h4></td>";
                                    }

                                    // Check if the file_name1 path contains a subdirectory
                                    if (file_name1.includes('/')) {
                                        var subdirectory = file_name1.split('/')[0];
                                        rw = rw.replace("d=c", "d=tsim_doc/" + subdirectory);
                                    }
                                } else {
                                    rw += "<td><h4>Null</h4></td>";
                                }
                            }
                            rw += "<td>" + order_booking_date + "</td>";
                            rw += "<td>" + from_date + "</td>";
                            rw += "<td><input id='comments_" + order_no + "' type='text' value='" + comments + "'><input type = 'submit' style='width:8%;' id = 'update_comments_" + order_no + "' value='&#10003'></td>";
                            //   console.log("-" + customerphoneno + "-");
                            var mobno = customerphoneno.substring(2);
                            //   console.log("-" + mobno + "-");
                            rw += "<td><a href='#' onclick=\"openTab('https://trikondomestic.cc.warmconnect.com/vicidial/non_agent_api.php?source=test&user=api&pass=apitasting2pword&function=add_lead&phone_number=" + mobno + "&phone_code=91&list_id=24216344&dnc_check=Y&campaign_dnc_check=Y&campaign_id=BROTSIM&add_to_hopper=Y&hopper_local_call_time_check=Y')\">Add</a><br><br> <a href='#' onclick=\"openTab('https://trikondomestic.cc.warmconnect.com/vicidial/non_agent_api.php?source=test&user=api&pass=apitasting2pword&function=update_lead&search_location=SYSTEM&search_method=PHONE_NUMBER&phone_number=" + mobno + "&delete_lead=Y')\">Delete</a></td>";


                            rw += "</tr>";
                            rw += "<tr id='tr_" + order_no + "_iframe_order_info' style='display:none;'>";
                            //  console.log(user_arr[10] + user_arr[11] + user_arr[12]);
                            if (order_no.includes("-")) {
                                rw += "<td colspan='1'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_iframe_order_info_0' style='display:none;' src='about:blank' data-src='about:blank'></iframe></div></td>";
                            } else {
                                rw += "<td colspan='1'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_iframe_order_info_0' style='display:none;' src='about:blank' data-src='https://www.tsim.in/wp-admin/post.php?post=" + order_no + "&action=edit'></iframe></div></td>";
                            }

                            for (var i = 11; i <= 13; i++) {
                                // Check if the file exists
                                if (user_arr[i]) {
                                    var filename = user_arr[i];
                                    var extension = filename.substr((filename.lastIndexOf('.') + 1));

                                    // Check if the filename is an image (JPEG or JPG)
                                    if (extension === 'jpeg' || extension === 'JPEG' || extension === 'jpg' || extension === 'JPG') {
                                        rw += "<td colspan='4'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_iframe_order_info_" + (i - 10) + "' style='display:none;' src='about:blank' data-src='https://mail004.ownmail.com/bc.e?i=" + (filename.includes('/') ? filename.split('/')[1] : filename) + "&f=2500&d=d&a=U&dl=yes&l=services'></iframe></div>";
                                        if (filename.includes('/')) {
                                            var sub = filename.split('/')[0];
                                            rw = rw.replace("d=d", "d=tsim_doc/" + sub);
                                        }
                                        rw += "<i class='fa fa-rotate-left' id='rl_" + order_no + "_" + (i - 10) + "'></i><i class='fa fa-rotate-right' id='rr_" + order_no + "_" + (i - 10) + "'></i></td>";
                                    } else {
                                        rw += "<td colspan='4'><div class='panel'><iframe cellspacing='0' frameborder='0' class='" + order_no + "_iframe_order_info_" + (i - 10) + "' style='display:none;' src='about:blank' data-src='https://mail004.ownmail.com/bc.e?i=" + (filename.includes('/') ? filename.split('/')[1] : filename) + "&f=2500&d=d&a=U&dl=yes&l=services'></iframe></div>";
                                        if (filename.includes('/')) {
                                            var sub = filename.split('/')[0];
                                            rw = rw.replace("d=d", "d=tsim_doc/" + sub);
                                        }
                                        rw += "<i class='fa fa-rotate-left' id='rl_" + order_no + "_" + (i - 10) + "'></i><i class='fa fa-rotate-right' id='rr_" + order_no + "_" + (i - 10) + "'></i></td>";
                                    }
                                }
                            }
                            rw += "</tr>";
                            $("#documents_list_table > tbody").append(rw);
                        }
                    }
                }
            }
        });

    }


    function check(url) {
        var url = url;
        PDFJS.getDocument(url)
            .then(function(pdf) {
                var container = Swal.getContent().querySelector('#show_container');
                for (var i = 1; i <= pdf.numPages; i++) {
                    pdf.getPage(i).then(function(page) {
                        var scale = 2;
                        var viewport = page.getViewport(scale);
                        var div = document.createElement("div");
                        div.setAttribute("id", "page-" + (page.pageIndex + 1));
                        div.setAttribute("style", "position: relative");
                        container.appendChild(div);
                        var canvas = document.createElement("canvas");
                        canvas.setAttribute("style", "width:100%");
                        div.appendChild(canvas);
                        var context = canvas.getContext('2d');
                        canvas.height = viewport.height;
                        canvas.width = viewport.width;
                        var renderContext = {
                            canvasContext: context,
                            viewport: viewport
                        };
                        page.render(renderContext);
                    }).catch(function(error) {
                        console.error('Error rendering page:', error);
                    });
                }
            })
            .catch(function(error) {
                console.error('Error loading PDF:', error);
            });
    }


    $(document).on('click', '.row_docs', function(event) {
        event.preventDefault();
        var row_doc = $(this).closest('td').attr('row_docs');
        console.log("TAble Is Done " + row_doc);
        swal.fire({
            type: 'info',
            text: 'Fetching Documents.......',
            showCancelButton: true,
            //cancelButtonText: 'CANCEL',
            reverseButtons: true,
            onBeforeOpen: () => {
                Swal.showLoading()
            }
        });
        $.ajax({
            url: 'simmis.e?a=popupdocs&d=' + row_doc,
            type: 'GET',
            success: function(data) {
                console.log(data)
                var jshtml = '';
                const arr = data.split(',');
                // arr.pop();
                // arr.shift();
                console.log(arr);
                // for (var j = 0; j < arr.length; j++) {
                //     var filenamename = arr[j].trim();

                //     jshtml += '<h4><a href="https://mail004.ownmail.com/bc.e?i=' + (filename.includes('/') ? filename.split('/')[1] : filename) + '&f=9337&d=tsim_doc&z=z">Document ' + j + '</a></h4>';
                //     if (filename.includes('/')) {
                //         var subdirectory = filename.split('/')[0];
                //         jshtml = jshtml.replace("d=tsim_doc", "d=tsim_doc/" + subdirectory);
                //     }
                // }
                for (var j = 0; j < arr.length; j++) {
                    var filename = arr[j].trim();
                    var documentUrl = 'https://mail004.ownmail.com/bc.e?i=' + (filename.includes('/') ? filename.split('/')[1] : filename) + '&f=9337&d=tsim_doc&z=z';

                    if (filename.includes('/')) {
                        var subdirectory = filename.split('/')[0];
                        documentUrl = documentUrl.replace("d=tsim_doc", "d=tsim_doc/" + subdirectory);
                    }

                    jshtml += '<h4><a href="' + documentUrl + '">Document ' + (j + 1) + '</a></h4>';
                }

                swal.fire({
                    type: 'info',
                    html: jshtml,
                    showCancelButton: true,
                    cancelButtonText: 'CANCEL',
                    reverseButtons: true
                });

            }
        });
    });


    var recover = {};

    $(document).on('click', '.btn_edit', function(event) {
        event.preventDefault();
        var table_row = $(this).closest('tr');
        var row_id = table_row.attr('row_id');
        var orderNumber = table_row.find('.orderno a').text();
        console.log(orderNumber);
        var simNumber = table_row.find('.serialno span').text().trim();
        console.log(simNumber);
	var tripId = table_row.find('.trip').text().trim()
	console.log(tripId);
        console.log("Edit button called "+row_id);
	//var og_email = table_row.find('.emailadd').text().trim();	


	console.log("this is table row");
        var row_data = [table_row.find('.row_date_data').find('.date').html(), table_row.find('.phone_no').text(), table_row.find('.row_active').find('.active').html(), table_row.find('.comment').text(), table_row.find('.pinno'), table_row.find('.puk1'), table_row.find('.simno'), table_row.find('.qrtext'),table_row.find('.emailadd')];//QR TEXT FETCH

        //********************ISSUE2047************************
	console.log(table_row);
        fetchLatestRecord(table_row, row_id, orderNumber, tripId);
//pointer

        console.log(row_data);
        recover[row_id] = row_data;
	console.log("table row is:",table_row);
        console.log("recover is ",recover);
        console.log("row data in edit is:",row_id);
	table_row.find('.btn_save').show();
        table_row.find('.btn_cancel').show();
        table_row.find('.btn_sendmail').show();
        table_row.find('.btn_activate').show();
        table_row.find('.btn_edit').hide();
	var og_email = table_row.find('.emailadd').text().trim();//email added here
	table_row.data('original-email', og_email);
	console.log("This is original email"+og_email);
	console.log("Whole row",table_row);
	var og_comment = table_row.find('.comment').text().trim();//email added here
        table_row.data('original-comment', og_comment);
        console.log("This is original comment"+og_comment);

        table_row.find('.row_active').on('click');

        table_row.find('.row_date_data').find('.datepicker-input').datepicker({
            dateFormat: 'yy-mm-dd',
            minDate: 0,
            onSelect: function(dateText, inst) {
                ////console.log(dateText);
                table_row.find('.row_date_data').find('.date').html(dateText);
            }
        });

        table_row.find('.row_data')
            .attr('contenteditable', 'true')
            .css("background-color", "aqua")
            .css('padding', '3px')
        table_row.find('.row_active').css("background-color", "aqua");
        table_row.find('.row_date_data').click(function() {
            ////console.log("Row Data clicked "+row_id);
            table_row.find('.row_date_data').find('.datepicker-input').focus();
        });
//	table_row.find('.emailadd')
//	    .attr('contenteditable', 'true') // Make it editable
//	    .css("background-color", "aqua") // Highlight for visibility
	    

        table_row.find('.row_active').click(function() {
            ////console.log("Heloo");

            selectValues = {
                "NO": "NO",
                "YES": "YES"
            };
            var output = [];
            output.push('<select id="d">');
            $.each(selectValues, function(key, value) {
                output.push('<option value="' + key + '">' + value + '</option>');
            });
            output.push('</select>');
            table_row.find('.row_active').find('.active').html(output.join(''));
            table_row.find('.row_active').off('click');

        });


    });


    $(document).on('click', '.btn_sendmail', function(e) {   
        e.preventDefault();
        var table_row = $(this).closest('tr');    
        var orderNumber = table_row.find('.orderno a').text();
        console.log(orderNumber);
        var simNumber = table_row.find('.serialno span').text().trim();
        console.log(simNumber);
        $.ajax({
            url:"sendmaildocs_details.php",
            type:"POST",
            dataType: "json",
            data:{
                simnumber:simNumber
            },
            success:(response)=>{
		        console.log(response);
                console.log(response.code)
                switch(response.code) {
                    case 200:
                        Swal.fire({
                            html: `
                                SimSerialNumber: ${simNumber}  OrderNumber: ${orderNumber}<br><br>
                                <button id="documents-btn" class="swal2-confirm swal2-styled">Documents</button>
                                <button id="sim-details-btn" class="swal2-confirm swal2-styled" style="margin-left: 10px;">SIM Details</button>
                            `,
                            showCloseButton: true,
                            showConfirmButton: false, // Disable the default confirm button
                            onOpen: () => {
                                // Add click event listeners for the buttons
                                document.getElementById('documents-btn').addEventListener('click', async () => {
                                    try {
                                        // Call sendData and wait for it to complete
                                        const msg = await sendData('documents', simNumber, orderNumber);
                                        console.log(msg);                        
                                        Swal.close(); 
                                        // Check the message and show appropriate alert
                                        if(msg === "all_uploaded") {
                                            Swal.fire({
                                                title: 'Documents uploaded',
                                                text: 'All three documents have been uploaded by the client.',
                                                showConfirmButton: true
                                            });
                                        } else {
                                            Swal.fire({
                                                title: 'Mail Sent',
                                                text: 'The document reminder has been sent to the client.',
                                                showConfirmButton: true
                                            });
                                        }
                                        table_row.find('.btn_save').hide();
                                        table_row.find('.btn_cancel').hide();
                                        table_row.find('.btn_sendmail').hide();
                                        table_row.find('.btn_activate').hide();
                                        table_row.find('.btn_edit').show();
                                    } catch (error) {
                                        console.error('Error:', error);
                                        Swal.fire({
                                                title: 'Error',
                                                text: 'An error occurred while sending the data. Please try again.',
                                                showConfirmButton: true,
                                                icon: 'error'
                                        });
                                    }
                                });
                                document.getElementById('sim-details-btn').addEventListener('click', async () => {
                                    try{
                                        const msg = await sendData('sim_details', simNumber, orderNumber);
                                        Swal.close(); // Close the current SweetAlert popup
                                        Swal.fire({
                                            title: 'Sim Details',
                                            text: 'Sim Details has been sent to client.',
                                            showConfirmButton: true
                                        });
                                        table_row.find('.btn_save').hide();
                                        table_row.find('.btn_cancel').hide();
                                        table_row.find('.btn_sendmail').hide();
                                        table_row.find('.btn_activate').hide();
                                        table_row.find('.btn_edit').show();
                                    }
                                    catch(e){
                                    console.log(e);
                                    }
                                });
                            }
                        });
                        break;
                    case 202:
                        Swal.fire({
                            html: `
                                SimSerialNumber: ${simNumber}<br><br>
                                <button id="sim-details-btn" class="swal2-confirm swal2-styled" style="margin-left: 10px;">SIM Details</button>
                            `,
                            showCloseButton: true,
                            showConfirmButton: false, // Disable the default confirm button
                            onOpen: () => {
                                document.getElementById('sim-details-btn').addEventListener('click',async () => {
                                   
				                try{
                                    const msg = await sendData('sim_details', simNumber, orderNumber);
                                    Swal.close(); // Close the current SweetAlert popup
                                    Swal.fire({
                                        title: 'Sim Details',
                                        text: 'Sim Details has been sent to client.',
                                        showConfirmButton: true
                                    });
                                        table_row.find('.btn_save').hide();
                                        table_row.find('.btn_cancel').hide();
                                        table_row.find('.btn_sendmail').hide();
                                        table_row.find('.btn_activate').hide();
                                        table_row.find('.btn_edit').show();
                                }
                                catch(e){
                                    console.log(e)
                                }
                                });
                            }
                        });
                        break;
                    default:
                        Swal.fire({
                            title: 'Unhandled Response',
                            text: `Received response code: ${response.code}`
                        });
                        break;
                }
            },
            error:(err)=>{
                console.log(err);
            }
        })
    });


   $(document).on('click', '.btn_activate', function(e) {
	event.preventDefault();
/*
	Swal.fire({
	  title: 'Coming soon',
	  text: 'Coming soon'
	});
*/
	var table_row = $(this).closest('tr');
	var row_id = table_row.attr('row_id');
	table_row.find('.btn_activate').hide();
	var orderNumber = table_row.find('.orderno a').text();
	console.log(orderNumber);
	var simNumber = table_row.find('.serialno span').text().trim();
	console.log(simNumber);
	var simCountry = table_row.find('.simcountry span').text().trim();
	console.log(simCountry);
	var act_script = table_row.find('input[type="hidden"][id="activation_scriptname"]').val();
	//var act_script = table_row.find('input[type="hidden"]').val();
	var tripId = table_row.find('.trip').text().trim();
	var sVendor = table_row.find('.vendor').text().trim();
	console.log(tripId);
//find('input[type="hidden"][id^="tripid_"]').attr('id');
	console.log(act_script);
	$.ajax({
            url: "https://mail004.ownmail.com/html_parts/tsim_activate_sim.php",
            type:"POST",
            dataType: "json",
            data:{      
                simnumber:simNumber,
		order_no:orderNumber,
		country:simCountry,
		tripid:tripId,
		a_script:act_script,
		vendor:sVendor
            },          
            success:(response)=>{
		console.log(response);
		fetchLatestRecord(table_row, row_id, orderNumber, tripId);
		switch(response.code){
		    case 200:
			sTitle='Success!';
			sMsg='Activation has been done';
	table_row.find('.btn_save').hide();
        table_row.find('.btn_cancel').hide();
        table_row.find('.btn_sendmail').hide();
        table_row.find('.btn_activate').hide();
        table_row.find('.btn_edit').show();
			break;
		    default:
			sTitle='Failed!';
			sMsg='Activation failed.';
		}
	Swal.fire({
		title: sTitle,
		text: sMsg,
		showConfirmButton: true
	});
/*
	table_row.find('.btn_save').hide();
        table_row.find('.btn_cancel').hide();
        table_row.find('.btn_sendmail').hide();
        table_row.find('.btn_activate').hide();
        table_row.find('.btn_edit').show();

*/	    }

	})
   });


    async function sendData(action, simNumber, orderNumber) {
        console.log(action);
        console.log(simNumber);
        console.log(orderNumber);

        try {
            const response = await $.ajax({
                url: "sendmaildocs_details.php",
                type: "POST",
                dataType: "json",
                data: {
                    action: action,
                    simno: simNumber,
                    order: orderNumber
                }
            });
            switch(response.code) {
                case 200:
                case 204:
                    return response.msg;
                default:
                    Swal.fire({
                        title: 'Something went wrong',
                        text: `Received response code: ${response.code}`
                    });
                    return null;
            }
        } catch (err) {
            console.error('Error:', err);
            return null;
        }
    }
        

    $(document).on('click', '.btn_save', function(event) {
        event.preventDefault();


        var table_row = $(this).closest('tr');
        var row_id = table_row.attr('row_id');
	var og_email = table_row.data('original-email');  // Get the original 
	console.log("this is original email");
	console.log(og_email);
	
	var og_comment = table_row.data('original-comment');  // Get the original 
        console.log("this is original comment");
        console.log(og_comment);

        table_row.find('.btn_save').hide();
        table_row.find('.btn_cancel').hide();
        table_row.find('.btn_sendmail').hide();
        table_row.find('.btn_activate').hide();
        table_row.find('.btn_edit').show();


        table_row.find('.row_data')
            .attr('contenteditable', 'false')
            .css("background-color", "")
            .css('padding', '');

        table_row.find('.row_active').css("background-color", "");
        table_row.find('.row_date_data').off('click');
        table_row.find('.row_active').off('click');

        var active = $("#active_date").val();

        var send = {};
        var activation_field = table_row.find('.row_active').find('.active').html();

        if (typeof table_row.find('.row_active').find('.active').find('select').val() != "undefined") {
            activation_field = table_row.find('.row_active').find('.active').find('select').val();
            //console.log("Inside if");

        }
        var send_data = [$.trim(table_row.find('.row_date_data').find('.date').html()), table_row.find('.phone_no').text(),activation_field];
        send_data.push(recover[row_id][1]);
        send_data.push(table_row.find('.trip').text());
        send_data.push(table_row.find('.comment').text());
        send_data.push(table_row.find('.orderno').text());
        send_data.push(table_row.find('.pinno').text());
        send_data.push(table_row.find('.puk1').text());
        send_data.push(table_row.find('.serialno').text().trim());
        send_data.push(table_row.find('.qrtext').text());
	send_data.push(table_row.find('.from_date').text());
	send_data.push(table_row.find('.emailadd').text());	
	send_data.push(og_email);
        	console.log(`-${send_data[9]}-`);
	send_data.push(og_comment);
	console.log(`-${send_data[10]}-`);
        //	console.log(`-${send_data[9].trim()}-`);


        //console.log("Now what happened "+table_row.find('.trip').text());	
        //console.log(" Existing record"+send_data);
        //var phonenumber = send_data[1];
        // console.log(send_data);
        //	var phonenumber = send_data[1].replace(/[^\x00-\x7F]/g, "");
        //	phonenumber     =        phonenumber.trim();

        var phonenumber = send_data[1];
        var saveflag = checkForSpecialChars(phonenumber);
        if (saveflag) {
            // console.log(phonenumber);
            var array = phonenumber.match(/[^\s]+/g);
            //console.log(array[0]);
            send_data[1] = array[0];


            var out = [];

            for (var key in send_data) {
                if (send_data.hasOwnProperty(key)) {
                    out.push(key + '=' + encodeURIComponent(send_data[key]));
                    //console.log("key" + '=' + encodeURIComponent(send_data[key]));
                }
            }
            var gett = out.join('&');
            console.log(gett);
            //	var phonenumber = send_data[1];
            //	       phonenumber     =	phonenumber.trim();
            //	console.log(phonenumber);
            //	var array = phonenumber.match(/[^\s]+/g); 
            //		console.log(array[0]);	
            //		send_data[1] = array[0];	
            $.ajax({
                url: 'simmis.e?a=save_activationreport',
                type: 'GET',
                data: gett,
                success: function(data) {
                    // codes....
                    // console.log(data);
                    if (data == '532601') {
                        alert("Number is Already Present, Please Refresh page to undo Action");
                        return;
                    }

                    if (data.includes("PIN_NO_NEEDED")) {
                        console.log("Got hit number 1");
                        alert("Enter Pin Number Please");
                        return;
                    }

                    if (table_row.find('.row_active').find('.active').find('select').val() == "YES") {
                        table_row.animate({
                            backgroundColor: 'yellow'
                        }, 1000);
                    } else if ($.trim(table_row.find('.row_date_data').find('.date').html()) != $.trim(recover[row_id][0])) {
                        ////console.log("Select value" +active);
                        ////console.log(table_row.find('.row_date_data').find('.date').html());
                        table_row.animate({
                            backgroundColor: '#ff9900'
                        }, 1000);
                    }
                    if (table_row.find('.row_active').find('.active').find('select').val() == "NO") {
                        table_row.animate({
                            backgroundColor: '#ffffe6'
                        }, 1000);
                    }
                    table_row.find('.row_active').find('.active').html(table_row.find('.row_active').find('.active').find('select').val());
                    table_row.find('.row_active').find('span').children().remove();

                }
            });

        } else {
            console.log("phone number is not proper might be containning space or special character");




            table_row.find('.btn_save').show();
            table_row.find('.btn_cancel').show();
            table_row.find('.btn_edit').show();
            table_row.find('.btn_sendmail').show();
            table_row.find('.btn_activate').show();

        }


    });
//pointer3
    $(document).on('click', '.btn_cancel', function(event) {
        event.preventDefault();
        var table_row = $(this).closest('tr');
        var row_id = table_row.attr('row_id');

        var og_email = table_row.data('original-email');
    	var og_comment = table_row.data('original-comment'); // Use the original comment
	console.log(table_row);
        var retain_value = recover[row_id];
        table_row.find('.row_date_data').find('.date').html(retain_value[0]);
        table_row.find('.phone_no').text(retain_value[1]);
        table_row.find('.row_active').find('.active').html(retain_value[2]);
        //table_row.find('.comment').text(retain_value[3]);
	table_row.find('.emailadd').text(og_email);
	table_row.find('.comment').text(og_comment);
        table_row.find('.btn_save').hide();
console.log("this is og commnet in cancel",og_comment);

        table_row.find('.btn_cancel').hide();
        table_row.find('.btn_sendmail').hide();
        table_row.find('.btn_activate').hide();
        table_row.find('.btn_edit').show();


        table_row.find('.row_data')
            .attr('contenteditable', 'false')
            .css("background-color", "")
            .css('padding', '');
	 table_row.find('.emailadd')
		.attr('contenteditable', 'false')
            .css("background-color", "")

        table_row.find('.row_active').css("background-color", "");
        table_row.find('.row_date_data').off('click');
        table_row.find('.row_active').off('click');
        ////console.log(table_row.find('.row_active').find('.active').find('select').val());
        table_row.find('.row_active').find('.active').html(table_row.find('.row_active').find('.active').find('select').val());
        table_row.find('.row_active').find('span').children().remove();
	console.log(table_row);

    });
};

var client_report = function() {
    selTable = "#report_table";
    selForm = "#stock_filter_form";

    //UI Setup
    selDate = selForm + " input.datepicker";
    $(selDate).each(function() {
        $(this).datepicker({
            dateFormat: "yy-mm-dd",
            showOn: "button",
            buttonImage: "//sim.trikon.in/cal.gif",
            buttonImageOnly: true
        });
    });
    //$( selDate ).addClass( "ui-widget ui-widget-content ui-helper-clearfix ui-corner-all" );


    $(selForm).submit(function(event) {
        event.preventDefault();
        $(selTable + " > thead").empty();
        $(selTable + " > tbody").empty();
        var posting = $.ajax({
            dataType: "json",
            url: "simmis.e",
            type: 'GET',
            data: {
                a: "client_report",
                b: "queryJSON",
                from_date: $('#from_date').val(),
                to_date: $('#to_date').val(),
                format: "1"
            }
        });
        posting.success(function(data) {
            var items = [];
            for (i = 0; i < data.cols.length; i++) {
                items.push("<th>" + data.cols[i].label + "</th>");
            }
            $("<tr/>", {
                html: items.join("")
            }).appendTo(selTable + " > thead");
            for (i = 0; i < data.rows.length; i++) {
                var row = "<tr>"
                for (j = 0; j < data.rows[i].length; j++) {
                    row += "<td>" + data.rows[i][j] + "</td>";
                }
                $(selTable + ' > tbody').append(row);
            }
            $('#report_download_link').attr("href", "simmis.e?a=client_report&b=queryCSV&format=1&" + $('#from_date').serialize() + '&' + $('#to_date').serialize());

        });
        posting.error(function(data) {
            alert("Error" + $(data).serialize());
        });
    });
};

var reconcile_report = function() {


    var random_id = function() {
        var id_num = Math.random().toString(9).substr(2, 3);
        var id_str = Math.random().toString(36).substr(2);

        return id_num + id_str;
    }

    //     $("#filter_country").load("simmis.e?a=getcountry_actreport");
    //Get countries
    //console.log("ACTIVATION");
    var tbody = $("#stock_list_table tbody");

    if (tbody.children().length == 0) {
        //    tbody.html("<tr>message foo</tr>");
        //console.log("Empty Html");
        $("#active_date").change();
    }
    //   $(document).find('.btn_save').hide();
    //   $(document).find('.btn_cancel').hide();
    //   var select = document.getElementById("filter_country");

    //messageBox object
    msgbox = new messageBox(".feedback_msg");
    datebox = new messageBox(".date_msg");
    msgbox.show("Please Select a country.", "");
    datebox.show("Report will show only for selected date", "");
    var d = new Date();

    var month = d.getMonth() + 1;
    var day = d.getDate();

    var output = d.getFullYear() + '-' + (month < 10 ? '0' : '') + month + '-' + (day < 10 ? '0' : '') + day;
    //      //console.log(output);
    $(document).find('#active_date').val(output);
    //console.log(output)
    //Form change event for filter_form
    function recon_execute() {
        //      $("#active_date").change(function(event){
        //console.log("Event Triggered");
        //Clear Stock List Table
        $("#stock_list_table > tbody").html('');
        $("#report_download_link").attr("href", "#");
        //                if($("#filter_country").val() == 'Select'){
        //                      msgbox.show("Please Select a country.", "");
        //                    return;
        //          }

        msgbox.show("Please Wait");

        //        var country = $("#filter_country").val();
        var date = $("#active_date").val();
        var date1 = new Date(date);
        var daysPrior = 1;
        date1.setDate(date1.getDate() + daysPrior);
        var month1 = date1.getMonth() + 1;
        var day1 = date1.getDate();
        var output1 = date1.getFullYear() + '-' + (month1 < 10 ? '0' : '') + month1 + '-' + (day1 < 10 ? '0' : '') + day1;
        //console.log(date);
        //console.log(output1);
        ////console.log("Activation report");   
        ////console.log($(this).find('.btn_save'));
        ////console.log("BTN SAVE");
        if (true) {
            console.log("Inside Data");
            get_data("#reconcile_report_form", "simmis.e?a=reconciliation_summary&b=query&d=" + output1, function(data) {
                if (data.find(".sim").length == 0) {
                    msgbox.show("No Records Found", "error");
                } else {


                    datebox.show("Following Records Found For Selected Date ", "noerror");
                    data.find(".sim").each(function(index) {

                        var row_id = random_id();

                        //Create sim object with 14 properties
                        var s = new sim();
                        $(this).find("[class]").each(function(j) {
                            s[$(this).attr("class")] = $(this).text();
                        });
                        //append table row
                        var rwNew = "<tr class='" + ((s['reco_done'] == 't') ? "recon_done" : "recon_pending") + "' row_id='" + row_id + "' >";

                        rwNew += "<td class=\"orderno\" ><a href=\"https://www.tsim.in/wp-admin/post.php?post=" + s['order_no'] + "&action=edit\" target=\"_blank\">" + s['order_no'] + "</a></td>";
                        rwNew += "<td>" + s['username'] + "</td>";
                        rwNew += "<td >" + s['clientname'] + "</td>";

                        rwNew += "<td class=\"row_data phone_no\"><span>" + s['sim_phone_no'] + "</span></td>";
                        rwNew += "<td >" + s['booking_date'] + "</td>";
                        rwNew += "<td>" + s['status'] + "</td>";
                        rwNew += "<td>" + s['actual_payment_recd'] + "</td>";
                        rwNew += "<td>" + s['accept_time'] + "</td>";
                        rwNew += "<td>" + s['trans_id'] + "</td>";
                        rwNew += "<td>" + s['type'] + "</td>";
                        //$('.datepicker-input').focus(); 
                        rwNew += "<td>";
                        if (s['reco_done'] == 'f') {
                            rwNew += "<span  class=\"btn_recon\" id=\"pending\"><a href=\"#\" row_id='" + row_id + "'>Pending</a> </span>";
                        } else {
                            rwNew += "<span  class=\"btn_recon\" id=\"done\"><a href=\"#\" row_id='" + row_id + "'>Done</a> </span>";
                        }
                        rwNew += "</td>";
                        rwNew += "</tr>";

                        $("#stock_list_table > tbody").append(rwNew);
                    });
                    msgbox.show(data.find(".sim").length + " Records Found.", "noerror");
                    $("#report_download_link").attr("href", "simmis.e?a=active_summary&b=query&d=" + output1 + "&format=1&" + $("#activation_report_form").serialize());
                    //////console.log("Button Check Here ");////console.log($(document).find('.btn_edit'));
                    // $(document).find('.btn_save').hide();
                    // $(document).find('.btn_cancel').hide();
                }
            });
        } else {
            datebox.show("Report will show only for selected date", "");
            msgbox.show("Please Select a country.", "");
        }
        // });

    } //end of function recon_execute;
    var tbody = $("#stock_list_table tbody");

    if (tbody.children().length == 0) {
        //console.log("Empty Html");
        recon_execute();
    }


    //console.log("Change event occured");
    $("#active_date").change(function(event) {
        recon_execute();
    });

    /*	$(document).on("click","#tsim_refund_list_cb",function(e){
    	if($(this).prop("checked") == true){
    	$("#stock_list_display").html('');
    	$("#stock_list_display").html("<input type='text' name='tsim_order_id' id='tsim_order_id' placeholder='Order number'><input type='text' name='tsim_sim_ph_no' id='tsim_sim_ph_no' placeholder='Sim phone number'><input type='submit' name='add_to_refund' id='submit_add_to_refund' value='Add to list'><br><br>");
    	$("#stock_list_display").append("<table id='stock_list_table' class='summary_table'><thead><tr><th>OrderNumber</th><th>Username</th><th>ClientName</th><th>Sim phone number</th><th>Payment mode</th><th>Amount</th><th>Refund status</th></tr></table>");
    //			tsim_refund(); 
    datebox.show("Refund List | Report will show sims to be refunded", "");
    console.log("Refund list");
    }
    else if($(this).prop("checked") == false){
    $("#stock_list_display").html('');
    $("#stock_list_display").append("<table id='stock_list_table' class='summary_table'><thead><tr><th>OrderNumber</th><th>Username</th><th>ClientName</th><th>SimPhoneNumber</th><th>BookingDate</th><th>Status</th><th>Payment Amount</th><th>Payment Time</th><th>Payment TxnID</th><th>Payment Type</th><th>Action</th></tr></thead><tbody></tbody></table>");
    recon_execute();
    datebox.show("Reconcilation List | Following Reconcilation Records Found For Selected Date", "");
    console.log("Not a Refund list");
    }
    });*/


}

$(document).on('click', '.btn_recon', function(event) {
    event.preventDefault();
    var table_row = $(this).closest('tr');
    var row_id = table_row.attr('row_id');
    // console.log(table_row.find('.btn_recon').attr('id'));
    var currid = table_row.find('.btn_recon').attr('id');
    var act;
    var send_status = [];
    if (currid === 'pending') {
        console.log("pending");
        act = 'pending';
        send_status.push("pending");

    } else if (currid === 'done') {
        //console.log("done");
        act = 'done';
        send_status.push("done");
    }
    send_status.push(table_row.find('.phone_no').text());

    var outs = [];

    for (var key in send_status) {
        if (send_status.hasOwnProperty(key)) {
            outs.push(key + '=' + encodeURIComponent(send_status[key]));
        }
    }
    //console.log(outs);
    var bhej = outs.join('&');


    $.ajax({
        url: 'simmis.e?a=reconcile_done',
        type: 'GET',
        data: bhej,
        success: function(data) {
            if (data === "Success200") {
                if (table_row.find('.btn_recon').attr('id') == "pending") {
                    table_row.animate({
                        backgroundColor: '#66ffff'
                    }, 1000);
                    table_row.find('span').find('a').text("Done");
                    table_row.find("#pending").attr("id", "done");
                } else if (table_row.find('.btn_recon').attr('id') == "done") {
                    ////console.log("Select value" +active);
                    ////console.log(table_row.find('.row_date_data').find('.date').html());
                    table_row.animate({
                        backgroundColor: '#e6ffff'
                    }, 1000);
                    table_row.find('span').find('a').text("Pending");
                    table_row.find("#done").attr("id", "pending");
                    //table_row.prev("#done").attr("id","pending");
                }
            }
            // console.log(data);


        }



    });



});

function openTab(url) {
    var width = 800; // Width of the new tab
    var height = 400; // Height of the new tab
    var left = (window.screen.width - width) / 2; // Position the tab in the center horizontally
    var top = (window.screen.height - height) / 2; // Position the tab in the center vertically

    var newWindow = window.open(url, '_blank', 'width=' + width + ', height=' + height + ', left=' + left + ', top=' + top);

    setTimeout(function() {
        newWindow.close(); // Close the tab after 5 seconds
    }, 5000);
}

//************ISSUE2047
function updateUI(rowId, latestRecord) {
    var tableRow = $("tr[row_id='" + rowId + "']");

    var recover = {};
    recover[rowId] = latestRecord;


    // Update each cell with the new data
    if (latestRecord.order_no != null){
    tableRow.find('.orderno a').attr('href', 'https://www.tsim.in/wp-admin/post.php?post=' + latestRecord.order_no + '&action=edit');
    tableRow.find('.orderno a').text(latestRecord.order_no);
    }
    tableRow.find('.clientname').text(latestRecord.clientname);
    tableRow.find('.emailadd').text(latestRecord.emailadd);
    tableRow.find('.row_date_data .date').text(latestRecord.from_date);
    tableRow.find('.phone_no span').text(latestRecord.sim_phone_no);
    tableRow.find('.serialno span').text(latestRecord.simno);
    tableRow.find('.booking_date').text(latestRecord.booking_date);
    if (latestRecord.vendor != null) tableRow.find('.vendor').text(latestRecord.vendor);
    tableRow.find('.trip').text(latestRecord.tripid);
    tableRow.find('.simcountry span').text(latestRecord.country);
    tableRow.find('.pinno').text(latestRecord.pinno);
    tableRow.find('.puk1').text(latestRecord.puk1);
    tableRow.find('.status').text(latestRecord.status);
    tableRow.find('.type').text(latestRecord.type);
    tableRow.find('.handset_model').text(latestRecord.handset_model);
    tableRow.find('.row_docs').text(latestRecord.no_of_documents_uploaded);
    if (latestRecord.qrtext !=null) tableRow.find('.qrtext').text(latestRecord.qrtext);
    if (latestRecord.documents_uploaded === 't') {
        tableRow.find('#documents_verified').attr('class', 'row_docs').text(latestRecord.no_of_documents_uploaded);
    }
    tableRow.find('.comment').text(latestRecord.comment);
    tableRow.find('.row_active .active').text(latestRecord.activated === 't' ? 'YES' : 'NO');
    if (latestRecord.activated === 't') {
        tableRow.animate({
            backgroundColor: 'yellow'
        }, 1000);
    }
    else{
        tableRow.animate({
            backgroundColor: '#ffffe6'
        }, 1000);

    }
    tableRow.find('input[type="hidden"][id="activation_scriptname"]').val(latestRecord.activation_scriptname);
    if (latestRecord.activation_scriptname != "0" && latestRecord.activated =='f'){
	if (tableRow.find('.btn_activate').length){
	  tableRow.find('.btn_activate').attr('href','#');
	  tableRow.find('.btn_activate').attr('row_id',rowId);
	}
	else{
	  tableRow +='<span  class="btn_activate"><a href="#" row_id=' + rowId + '> Activate</a> </span>';  
	}
	tableRow.find('.btn_activate').show();
    }
    else{
	tableRow.find('.btn_activate').hide();
    }
}





/*
function showSimInfoModal() {
  Swal.fire({
    title: "Enter SIM Card Number",
    html: `
      <label for="currentSimNo">Enter Current SIM No:</label>
      <input type="text" id="currentSimNo" class="swal2-input" placeholder="Current SIM No"><br><br>

      <label for="currentSimPhoneNo">Enter Current SIM Phone No:</label>
      <input type="text" id="currentSimPhoneNo" class="swal2-input" placeholder="Current SIM Phone No"><br><br>

      <label for="newSimNo">Enter New SIM No:</label>
      <input type="text" id="newSimNo" class="swal2-input" placeholder="New SIM No"><br><br>

      <label for="newSimPhoneNo">Enter New SIM Phone No:</label>
      <input type="text" id="newSimPhoneNo" class="swal2-input" placeholder="New SIM Phone No">
    `,
    focusConfirm: false,
    showCancelButton: true,
    confirmButtonText: "Submit",
    showClass: {
      popup: `
        animate__animated
        animate__fadeInUp
        animate__faster
      `
    },
    hideClass: {
      popup: `
        animate__animated
        animate__fadeOutDown
        animate__faster
      `
    },
    preConfirm: () => {
      // Retrieve values from inputs
      const currentSimNo = document.getElementById("currentSimNo").value;
      const currentSimPhoneNo = document.getElementById("currentSimPhoneNo").value;
      const newSimNo = document.getElementById("newSimNo").value;
      const newSimPhoneNo = document.getElementById("newSimPhoneNo").value;

      // Check if all fields are filled
      if (!currentSimNo || !currentSimPhoneNo || !newSimNo || !newSimPhoneNo) {
        Swal.showValidationMessage("Please enter all fields.");
        return false;
      }

      return { currentSimNo, currentSimPhoneNo, newSimNo, newSimPhoneNo };
    }
  }).then((result) => {
    if (result.isConfirmed) {
      const { currentSimNo, currentSimPhoneNo, newSimNo, newSimPhoneNo } = result.value;

      // Send data to executesimno.php
      fetch('https://mail004.ownmail.com/html_parts/executesimno.php', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: `currentSimNo=${encodeURIComponent(currentSimNo)}&currentSimPhoneNo=${encodeURIComponent(currentSimPhoneNo)}&newSimNo=${encodeURIComponent(newSimNo)}&newSimPhoneNo=${encodeURIComponent(newSimPhoneNo)}`
      })
          .then(response => {
        // Check if the response status is okay, then parse JSON
        if (!response.ok) {
          throw new Error('Network response was not ok.');
        }
        return response.json();
      })
        .then(data => {
          if (data.status === "success") {
            Swal.fire('Success', data.message, 'success');

         // Log the submitted data
        console.log("Current SIM No:", currentSimNo);
        console.log("Current SIM Phone No:", currentSimPhoneNo);
        console.log("New SIM No:", newSimNo);
        console.log("New SIM Phone No:", newSimPhoneNo);

        // Log the server response
        console.log(data);


          } else {
            Swal.fire('Error', data.message, 'error');
          }
        })
        .catch(error => {
          Swal.fire('Error', 'There was an error processing your request.', 'error');
          console.error('Error:', error);
        });
      }
    });
  }
*/




function createSimOptions(orderId, simDetails) {
  let rw = "<table style='width: -moz-available;'><tbody>";
  simDetails.split("<br>").forEach((detail, index) => {
    if (detail.trim() !== "") {
      // Extract SIM number and SIM phone number from detail
      const match = detail.match(/Sim - (\d+) \((\d+)\)/);
      const simNo = match ? match[1] : "";
      const simPhoneNo = match ? match[2] : "";

      rw += `
        <tr>
          <td>
            <input type="radio" id="sim_${orderId}_${index}" name="sim_selection_${orderId}" value="${simNo}|${simPhoneNo}">
            <label for="sim_${orderId}_${index}" style="margin-left: 8px;">${detail}</label>
          </td>
        </tr>
      `;
    }
  });

  rw += `
    <tr>
      <td>
        <button onclick="handleSimSelection('${orderId}')" style="margin-top: 10px;">Enter SIM Number</button>
      </td>
    </tr>
  `;
  rw += "</tbody></table>";

  document.getElementById("simno_" + orderId).innerHTML = rw;
}

function handleSimSelection(orderId) {
  // Get the selected radio button
  const selectedOption = document.querySelector(`input[name="sim_selection_${orderId}"]:checked`);
  if (!selectedOption) {
    Swal.fire("Error", "Please select a SIM number first.", "error");
    return;
  }

  // Split the selected value into SIM number and SIM phone number
  const [simNo, simPhoneNo] = selectedOption.value.split("|");

  // Call the modal with pre-filled values
  showSimInfoModal(simNo, simPhoneNo);
}

function showSimInfoModal(currentSimNo = "", currentSimPhoneNo = "") {
  Swal.fire({
    title: "Enter SIM Card Number",
    html: `
      <label for="currentSimNo">Enter Current SIM No:</label>
      <input type="text" id="currentSimNo" class="swal2-input" value="${currentSimNo}" placeholder="Current SIM No"><br><br>

      <label for="currentSimPhoneNo">Enter Current SIM Phone No:</label>
      <input type="text" id="currentSimPhoneNo" class="swal2-input" value="${currentSimPhoneNo}" placeholder="Current SIM Phone No"><br><br>

      <label for="newSimNo">Enter New SIM No:</label>
      <input type="text" id="newSimNo" class="swal2-input" placeholder="New SIM No"><br><br>

      <label for="newSimPhoneNo">Enter New SIM Phone No:</label>
      <input type="text" id="newSimPhoneNo" class="swal2-input" placeholder="New SIM Phone No">
    `,
    focusConfirm: false,
    showCancelButton: true,
    confirmButtonText: "Submit",
    preConfirm: () => {
      const currentSimNo = document.getElementById("currentSimNo").value;
      const currentSimPhoneNo = document.getElementById("currentSimPhoneNo").value;
      const newSimNo = document.getElementById("newSimNo").value;
      const newSimPhoneNo = document.getElementById("newSimPhoneNo").value;

      if (!currentSimNo || !currentSimPhoneNo || !newSimNo || !newSimPhoneNo) {
        Swal.showValidationMessage("Please enter all fields.");
        return false;
      }

      return { currentSimNo, currentSimPhoneNo, newSimNo, newSimPhoneNo };
    }
  }).then((result) => {
    if (result.isConfirmed) {
      const { currentSimNo, currentSimPhoneNo, newSimNo, newSimPhoneNo } = result.value;
      // Handle the form submission logic
      console.log("Current SIM No:", currentSimNo);
      console.log("Current SIM Phone No:", currentSimPhoneNo);
      console.log("New SIM No:", newSimNo);
      console.log("New SIM Phone No:", newSimPhoneNo);
    }
  });
}

