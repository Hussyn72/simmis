//jQuery time
var current_fs, next_fs, previous_fs; //fieldsets
var left, opacity, scale; //fieldset properties which we will animate
var animating; //flag to prevent quick multi-click glitches

var _PDF_DOC,
    //   _CANVAS = document.querySelector('.pdf-preview'),
    _CANVAS,
    _OBJECT_URL, _TOTAL_PAGES, _CURRENT_PAGE;
var image_c_pass1, image_c_pass2, image_c_visa;
var file_arr = [];



function getFormData($form) {
  var unindexed_array = $form.serializeArray();
  var indexed_array = {};

  $.map(unindexed_array, function (n, i) {
    indexed_array[n["name"]] = n["value"].trim();
  });

  return indexed_array;
}


// ISSUE 1896 & 2204
$(document).ready(function () {
  // Get the current date
  var today = new Date();

  // Check if today is Sunday
  if (today.getDay() === 0) {
    // If today is Sunday, set the default date to Monday
    today.setDate(today.getDate() + 1);
  }

  $("#date,#date_mobi").datepicker({
    minDate: today, // Disable dates before today
    beforeShowDay: function (date) {
      var day = date.getDay();
      return [day !== 0, ""]; // Disable Sundays
    },
  });

  $("#date,#date_mobi").datepicker("setDate", today);

  $("#date,#date_mobi").bind("keypress", function (e) {
    e.preventDefault();
  });
});

function validateInput(input) {
    var inputValue = input.value;
    if (/[^0-9-]/.test(inputValue)) {
        input.value = inputValue.replace(/[^0-9-]/g, '');
    }
}

function validateField(input) {
    var inputValue = input.value;
    if (/[^0-9+\-]/.test(inputValue)) {
        input.value = inputValue.replace(/[^0-9+\-]/g, '');
    }
}

$(document).ready(function() {
    // Function to check if a date is a Sunday
    function isSunday(date) {
        return date.getDay() === 0;
    }
    // Event listener for the input field
    $('#date').on('change', function() {
        var inputDate = $(this).val();

        // Check if the input date matches the expected format (MM/DD/YYYY)
        var datePattern = /^\d{2}\/\d{2}\/\d{4}$/;
        if (!datePattern.test(inputDate)) {
            alert("Invalid date format. Please use MM/DD/YYYY.");
            $(this).val(""); // Clear the input field
            return;
        }

        var selectedDate = new Date(inputDate);

        // Check if the selected date is before today
        if (selectedDate < new Date() + 1) {
            alert("Activation date should not be before today.");
            $(this).val(""); // Clear the input field
            return;
        }

        // Check if the selected date is a Sunday
        if (isSunday(selectedDate)) {
            alert("You cannot select a date on Sundays.");
            $(this).val(""); // Clear the input field
        }
    });
    // Initialize the datepicker
    $("#date").datepicker({
        minDate: 0,
        beforeShowDay: function(date) {
            return [(date.getDay() !== 0), '']; // Disable Sundays
        }
    });
    // Set the default date to today
    $("#date").datepicker("setDate", new Date());

    $('#date').bind('keypress', function(e) {
        e.preventDefault();
    });
	$("#num").val("");
	$("#serialNo").val("");
	$("#ordNo").val("");
});

$(document).on('keyup', '#num', function() {
    //console.log("Keyup event triggered");
    if ($(this).val().length ==0) {
       // console.log("Value is empty");
		$("#name_status").html('');
        $("#fnext_ind,#fnext_mobi").addClass("hiding");
        $("#num").val("");
        $("#serialNo").val("");
        $("#ordNo").val("");
		$("#personal_li").addClass("hiding"); 
		$("#documentation_li").addClass("hiding"); 
		$("#activation_li").addClass("hiding");
		$("#details").addClass("hiding");
        $("#file_upload_notice").addClass("hiding");
		//document.getElementById('sim
    }
});

//***********************ISSUE NO 1334 & 2520 *******************/
$(function() {
    // Initialize the date picker on the 'expdate' input field
    $('#expdate').datepicker({
        dateFormat: 'yy-mm-dd', // Set the date format to 'yyyy-mm-dd'
        changeMonth: true, // Show a dropdown for selecting the month
        changeYear: true, // Show a dropdown for selecting the year
        yearRange: '0:+10', // Allow selecting years from 100 years ago to 10 years in the future
        minDate: 1 // Restrict the minimum date to today
    });
});

function clearVal(i) {
    document.getElementById(i).value = null;
    var canvas = document.getElementById('image_c_' + i);
    var image_holder = $("#image_" + i);
    image_holder.empty();
    canvas.style.display = 'none';

  file_arr = [];
}

var timeout;

function showQRCode(response, client_email) {
  var qrCodeContainers = [];
  response.msg.forEach((item, index) => {
    var qrContainerWrapper = document.createElement("div");
    qrContainerWrapper.style.marginBottom = "20px";

    var title = document.createElement("p");
    title.textContent = `${index + 1}: ${item["title"]}`;
    title.style.textAlign = "center";
    qrContainerWrapper.appendChild(title);

    if (item["qrtext"]) {
      var qrcodeContainer = document.createElement("div");
      qrcodeContainer.style.display = "flex";
      qrcodeContainer.style.justifyContent = "center";
      qrcodeContainer.style.alignItems = "center";

      var qrcode = new QRCode(qrcodeContainer, {
        text: item["qrtext"],
        width: 128,
        height: 128,
      });
      qrcodeContainer.removeAttribute("title");

      qrContainerWrapper.appendChild(qrcodeContainer);
      qrCodeContainers.push(qrContainerWrapper);
      // if(send_mail){  // uncomment to send mail to client with each qrcode as of now not required so commented
      //     var canvas = qrcode._el.firstChild;
      //     $.ajax({
      //         type: 'POST',
      //         url: 'checkvalue.php',
      //         data: {
      //             qrCodeImage: canvas.toDataURL("image/png"),
      //             phoneNumber: item['sim_phone_no'],
      //             email: client_email,
      //             qrtext: item['qrtext']
      //         },
      //         success: function(response) {
      //             // console.log('Email sent successfully:', response);
      //         },
      //         error: function(error) {
      //             // console.log('Error sending email:', error);
      //         }
      //     });
      // }
    } else {
      var noQrText = document.createElement("p");
      if (client_email) {
        noQrText.textContent =
          "QR code not available. An email will be sent to " + client_email;
      } else {
        noQrText.textContent = "QR code will be emailed to you.";
      }
      noQrText.style.textAlign = "center";
      noQrText.style.fontWeight = "bold";
      qrContainerWrapper.appendChild(noQrText);
      qrCodeContainers.push(qrContainerWrapper);
    }
  });

    // Combine all QR code containers for display
    var combinedQRCodeContainer = document.createElement("div");
    qrCodeContainers.forEach(container => combinedQRCodeContainer.appendChild(container));

  Swal.fire({
    title: "Your Esim QR Codes",
    html: combinedQRCodeContainer,
    showCloseButton: true,
    showCancelButton: true,
    focusConfirm: false,
    confirmButtonText: "Download",
    preConfirm: () => {
      qrCodeContainers.forEach((container, index) => {
        if (response.msg[index]["qrtext"]) {
          var canvas = container.querySelector("canvas");
          var downloadLink = document.createElement("a");
          downloadLink.href = canvas.toDataURL("image/png");
          downloadLink.download =
            response.msg[index]["sim_phone_no"] + "_esim_qr_code.png";
          downloadLink.click();
        }
      });
    },
    didClose: () => {
      $("#msform")[0].reset();
      location.reload();
    },
  });
}

function redirect_popup() {
  Swal.fire({
    title: "View Your QR Codes",
    html: `<p>please click the button below. You will be redirected to a page where you will need to enter your Amazon Order ID to access the QR codes.</p>`,
    showCloseButton: true,
    showCancelButton: true,
    focusConfirm: false,
    confirmButtonText: "Click here to view your QR codes",
    confirmButtonColor: "#3085d6", // Optional: Customize the button color
    preConfirm: () => {
      window.open(`https://mail.tsim.in/tsim/amazon/index.html`, "_blank");
    },
    didClose: () => {
      $("#msform")[0].reset();
      location.reload();
    },
  });
}

function detectScrollEnd() {
    const iframeBody = document.body;

    iframeBody.addEventListener("scroll", function () {
        const scrollTop = iframeBody.scrollTop;
        const scrollHeight = iframeBody.scrollHeight;
        const clientHeight = iframeBody.clientHeight;

        if (scrollTop + clientHeight >= scrollHeight) {
            sendMessageToParent();
        }

        if (scrollTop <= 0) {
            sendMessageToParent();
        }
    });
}

// Separate function to send message to parent page
function sendMessageToParent() {
    //console.log("Sending message to parent to enable scrolling");
    window.parent.postMessage({ action: "enableParentScroll" }, "*");
}

function smoothScrollPolyfill(element, duration = 300) {
    const targetPosition = element.getBoundingClientRect().top + window.scrollY;
    const startPosition = window.scrollY;
    const distance = targetPosition - startPosition;
    let startTime = null;

    function animation(currentTime) {
        if (startTime === null) startTime = currentTime;
        const timeElapsed = currentTime - startTime;
        const run = ease(timeElapsed, startPosition, distance, duration);
        
        window.scrollTo(0, run);

        if (timeElapsed < duration) {
            requestAnimationFrame(animation);
        }
    }

    function ease(t, b, c, d) {
        t /= d / 2;
        if (t < 1) return c / 2 * t * t + b;
        t--;
        return -c / 2 * (t * (t - 2) - 1) + b;
    }

    requestAnimationFrame(animation);
}


function checking_num(e) {
  $("#serialNo").val("");
  let checking_value;
  if (typeof e === "string") {
    checking_value = e; // Direct value passed
  } else {
    checking_value = e.target.value; // Event object passed
  }
  if (checking_value.length >= 5) {
    $(".loader").removeClass("hiding");
    const number = checking_value;
    let check_activation = "";
if (sessionStorage.getItem("count_Query") !== null) {
    sessionStorage.removeItem("count_Query");
    //console.log("count_Query removed from sessionStorage.");
document.getElementById("sim-card-selection-frame").style.display = "none";
} else {
    //console.log("count_Query was not found in sessionStorage.");
}

    $.ajax({
      type: "post",
      url: "checkvalue.php",
      dataType: "json",
      data: {
        checking_num: number,
        timestamp: new Date().getTime(),
        //  count_query: countQuery,
      },
      success: function (response) {
        $(".loader").addClass("hiding");
        //                console.log("in checking_num function :----------> " + response.res);
        //		localStorage.setItem('count_query', response.count_query);
        //$("#count_query").val(response.count_query);
        switch (response.code) {
          case 200:
            $("#name_status").html(response.dmsg);
            document.getElementById("count_query").value =
              response.res["count_query"];
            let countQuery = response.count_query; // Get the count
            document.getElementById("count_query").value = countQuery;
            //console.log(response);

            if (response.res["is_kyc_required"] == "t") {
              //			 document.getElementById("count_query").value = response.res['count_query'];
              $("#fnext_mobi").addClass("hiding");
              $("#activation_li").addClass("hiding");
              $("#personal_li").removeClass("hiding");
              $("#documentation_li").removeClass("hiding");
              if (response.res["order_no"]) {
                document.getElementById("ordNo").value =
                  response.res["order_no"];
              } else {
                document.getElementById("ordNo").value =
                  response["new_order_no"];
              }
              //	 document.getElementById("count_query").value = response.res['count_query'];
              document.getElementById("serialNo").value = response.res["simno"];
              const ordno = document.getElementById("ordNo").value;
              check_activation = "india";
              if (response.res["activated"] == "f") {
                $("#fnext_ind").removeClass("hiding");
              }
            } else {
              $("#fieldset2, #fieldset3").addClass("hiding");
              $("#fieldset4").removeClass("hiding");
              $("#personal_li").addClass("hiding");
              $("#documentation_li").addClass("hiding");
              $("#activation_li").removeClass("hiding");
              $("#file_upload_notice").addClass("hiding");
              $("#progressbar").css({
                display: "flex",
                "justify-content": "center",
              });
              document.getElementById("serialNo").value = response.res["simno"];
              //console.log("in else condi");
              $("#fnext_mobi").removeClass("hiding");
              check_activation = "mobi";
            }
            break;
          case 202:
            // showQRCode(response,'');
            // console.log(1112);
            redirect_popup();
            check_activation = "amazon";
            break;
          case 203:
            if (response.res["qrtext"] && response.res["qrtext"] != "") {
              console.log(1111);
              showQRCode(response, "");
            } else {
              $("#name_status").html(
                "<font color=green>QR code will be emailed to you.</font>"
              );
            }
            break;
          case 210:
            $("#name_status").html(response.dmsg);
            document.getElementById("count_query").value =
              response.res["count_query"];
            let countQuery1 = response.count_query; // Get the count
            document.getElementById("count_query").value = countQuery1;
            console.log(response);
            if (countQuery1 > 1) {
              console.log("Displaying permanent frame for SIM selection");

              // Store the flag in sessionStorage to keep the frame visible
              sessionStorage.setItem("isFrameVisible", "true");
		sessionStorage.setItem("count_Query",countQuery1);
var inputField = document.getElementById("num");
/*if (inputField) {
        // Use a slight delay to give time for DOM changes to apply
        setTimeout(() => {
            inputField.blur();  // Remove focus from the input field
        }, 100); // 100 milliseconds delay, you can adjust this if needed
    }
  */
            // Populate the frame with options
              const simCardList = document.getElementById("sim-card-list");
              simCardList.innerHTML = ""; // Clear previous content
	const frameTitle = document.createElement("div");
    frameTitle.textContent = "Select eSIM to Proceed";
    frameTitle.classList.add("frame-title");      
    
	// Check for screen size and add the frame title to the appropriate position
        // If mobile, append it to the bottom
        simCardList.insertBefore(frameTitle, simCardList.firstChild);
        // If not mobile, prepend it to the top
              //console.log("count Query records : ", countQuery1);
              //initializeProgressBar(countQuery1);

              // Loop through the records to create the list items
              response.res.forEach((record, index) => {
                const listItem = document.createElement("li");
	if (window.innerWidth > 768){
                listItem.style.marginBottom = "12px";
                listItem.style.fontSize = "12px";
		}
	else{
		listItem.style.marginBottom = "1rem";
                listItem.style.fontSize = "1rem";

	}
                listItem.style.cursor = "pointer"; // Make the list item clickable

                const date = new Date(record.from_date);

                // Format the date to 'YYYY-MM-DD' format
                const formattedDate = date.toLocaleDateString("en-CA");

                listItem.innerHTML = `		
                		<div>  ${record.title}</div>
				<div> Activation Date :${formattedDate}</div>
            		`;

                // Add click event listener to each list item
                      listItem.addEventListener("click", () => {
                  event.preventDefault(); // Prevent any default behavior like page reload or navigation

                  // Highlight the selected list item
                  /*document.querySelectorAll("#sim-card-list li").forEach((item) => {
        	        item.classList.remove("selected-sim-card"); // Remove the class from all items
		            });
	            listItem.classList.add("selected-sim-card"); // Add the class to the selected item*/

                  // Remove the class from all items
                  document
                    .querySelectorAll("#sim-card-list li")
                    .forEach((item) => {
                      item.classList.remove("selected-sim-card");
                    });

                  // Add the class to the clicked item
                  listItem.classList.add("selected-sim-card");

                  console.log("Selected Record:", record);

                  // Handle case 200 logic (passing selected record details)
                  handleCase200({
                    dmsg: response.dmsg, // Pass `dmsg` for case 200 logic
                    count_query: response.count_query, // Pass count_query
                    simno: record.simno, // Map the SIM number
                  });

                  /*
			  // Update progress based on fieldset and card logic
		        if (activationProgress.currentFieldset < activationProgress.totalFieldsets) {
		            // Move to the next fieldset
		            activationProgress.currentFieldset++;
		        } else {
		            // If all fieldsets for the current card are done, move to the next card
		            activationProgress.currentFieldset = 1;
		            activationProgress.currentCard++;
		        }
		        updateProgressBar(); // Reflect the changes in the progress bar
			*/

                  // Simulate the click of the button to proceed
                  $("#fnext_mobi").click();
				var viewportWidth = $(window).width();
				if ( viewportWidth<768 ){
                  // Hide the frame after selection
                  document.getElementById("sim-card-selection-frame").style.display = "none";
}
                  // Remove the session storage flag after selection
                  sessionStorage.removeItem("isFrameVisible");
		//document.getElementById("sim-card-selection-frame").style.display = "none";
                });

                // Append the list item to the frame
                simCardList.appendChild(listItem);
              });

if (window.innerWidth <= 768) {
/*
    const inputField = document.getElementById("num");
    if (inputField) {
        inputField.blur();  // Prevents keyboard from opening
        console.log("Input field blurred");
    }

    const simFrame = document.getElementById("sim-card-selection-frame");

    if (simFrame) {
        setTimeout(() => {
            console.log("Attempting to scroll into view");

            // Temporarily disable focus to prevent keyboard popup
            simFrame.setAttribute("readonly", true);

            if ("scrollBehavior" in document.documentElement.style) {
                //simFrame.scrollIntoView({ behavior: "smooth", block: "center" });
            //{/} else {/}
				//alert("does not support scroll");
                smoothScrollPolyfill(simFrame, 300); // Use polyfill for unsupported devices
            }

            // Re-enable focus after scrolling completes
            setTimeout(() => {
                simFrame.removeAttribute("readonly");
            }, 500);
        }, 300);
    }

    simCardList.appendChild(frameTitle);

    const scrollToBottom = () => {
        const element = document.getElementById("sim-card-selection-frame");
        if (element) {
            element.scrollTop = element.scrollHeight;  // Scroll to the bottom
            console.log("Inside function scroll");
        }
    };

    setTimeout(scrollToBottom, 0);
*/

        const inputField = document.getElementById("num");
        if (inputField) {
            inputField.blur();  // Prevents keyboard from opening
            console.log("Input field blurred");
        }

        const simFrames = document.getElementById("sim-card-selection-frame");
        if (simFrames) {
            setTimeout(() => {
                console.log("Scrolling inside iFrame with GSAP");

                gsap.to(window, { 
                    duration: 1, 
                    scrollTo: { y: simFrames, offsetY: 100 }, 
                    ease: "power2.out" 
                });

            }, 300);
        }

}


// Show the frame (Ensure it's visible before scrolling)
const simFrame = document.getElementById("sim-card-selection-frame");
if (simFrame) {
    simFrame.style.display = "block";

    setTimeout(() => {
        // Temporarily disable focus to prevent keyboard popup
        simFrame.setAttribute("readonly", true);

        if ("scrollBehavior" in document.documentElement.style) {
            simFrame.scrollIntoView({ behavior: "smooth", block: "center" });
        } else {
            smoothScrollPolyfill(simFrame, 300); // Use polyfill for unsupported devices
        }

        // Re-enable focus after scrolling completes
        setTimeout(() => {
            simFrame.removeAttribute("readonly");
        }, 500);
    }, 300);
}


            }
            check_activation = "get-qr-code";
            break;
          case 404:
            $("#details").addClass("hiding");
            $("#serialNo").val("");
            $("#name_status").html(response.dmsg);
            $("#fnext_ind,#fnext_mobi").addClass("hiding");
            $("#file_upload_notice").addClass("hiding");
            break;
          default:
            break;
        }
      },
      complete: function () {
        function_selector(check_activation);
      },
    });
  }
}

function debounce(func, timeout = 400) {
  let timer;
  return (...args) => {
    clearTimeout(timer);
    timer = setTimeout(() => {
      func.apply(this, args);
    }, timeout);
  };
}

const processChange = debounce((e) => checking_num(e));

function function_selector(check_activation) {
  //	console.log(check_activation);
  switch (check_activation) {
    case "india":
      checkSerialno();
      break;
    case "mobi":
      check_num();
      break;
    case "amazon":
      break;
    /*	case "get-qr-code";
			HandleQrcodeRedirectedPage();
			break;*/
    default:
      return;
  }
}

function handleResponse201(msg) {
  document.getElementById("serialNo").value = "";
  document.getElementById("serialNo").value = msg["simno"];
  $("#name_status").html("");
  $("#fnext_ind").removeClass("hiding");
  var value = "<h3>Your profile information</h3><br>";
  value += "<h5><b>Name : </b>" + msg["clientname"] + "</h5>";
  value += "<h5><b>Email : </b>" + msg["emailadd"] + "</h5>";
  if (msg["t"] !== "tsim" && msg["t"] !== "amazon") {
    value +=
      "<h5><b>Activation Date : </b>" +
      msg["from_date"].replace(" 00:00:00", "") +
      "</h5>";
  }
  value += "<h5><b>Your Contact Number : </b>" + msg["mobno"] + "</h5>";
  value += "<div class='soit'>";
  if (msg["pass1"]) {
    value +=
      "<h5><b>Passport Front Page : </b> <a href='#' id=" +
      msg["pass1"] +
      ">Show</a></h5>";
  } else {
    value += "<h5><b>Passport Front Page : </b> Not uploaded</h5>";
  }
  if (msg["pass2"]) {
    value +=
      "<h5><b>Passport Back Page : </b> <a href='#' id=" +
      msg["pass2"] +
      ">Show</a></h5>";
  } else {
    value += "<h5><b>Passport Back Page : </b> Not uploaded</h5>";
  }
  if (msg["visa"]) {
    value +=
      "<h5><b>Visa/Ticket : </b> <a href='#' id=" +
      msg["visa"] +
      ">Show</a></h5>";
  } else {
    value += "<h5><b>Visa/Ticket : </b> Not uploaded</h5>";
  }
  value += "</div>";

  $("#details").removeClass("hiding");
  $("#details").html(value);
  $("#file_upload_notice").removeClass("hiding");
  document.getElementById("file_upload_notice").innerHTML =
    "<h6 style='color:red; padding:1%; border-style: solid; border-width: 1px;'>- Your files have been uploaded and sent for verification.<br>- Verification process may take few days, in the mean time you can re-upload your documents, if necessary.<br>- Once the verification is done, an email/sms will be sent and further you won't be allowed to upload any documents.</h6>";
}

function handleResponse203(msg) {
  var countQuery = document.getElementById("count_query").value;
  document.getElementById("serialNo").value = "";
  document.getElementById("file_upload_notice").innerHTML = "";
  document.getElementById("serialNo").value = msg["simno"];
  //document.getElementById("total_esims").value = msg[''];
  //console.log("RESPONSE 203 here too",countQuery);
  if (msg["simno"] != null) {
    $("#name_status").html("");
    if (countQuery == 1 || count_query == "") {
      document.getElementById("fnext_ind").click();
    } else if (countQuery > 1) {
      $("#fnext_ind").removeClass("hiding");
    }
  } else {
    $("#name_status").html("Order is either canceled or invalid");
  }
}

function handleResponse208(msg) {
  var countQuery = document.getElementById("count_query").value;
  document.getElementById("serialNo").value = "";
  document.getElementById("file_upload_notice").innerHTML = "";
  document.getElementById("serialNo").value = msg["simno"];
   // Get the total eSIMs value
   // IMEI container
    // EID container
    // Fieldset to append duplicate fields
	
  if (msg["simno"] != null) {
    $("#name_status").html("");
    if (countQuery == 1 || countQuery == "") {
      document.getElementById("fnext_ind").click();
    } else if (countQuery > 1) {
      $("#fnext_ind").removeClass("hiding");
    }
  } else {
    console.log("invalid");
    $("#name_status").html("Order is either canceled or invalid");
  }
}

function handleResponse209(msg) {
  var countQuery = document.getElementById("count_query").value;
  document.getElementById("serialNo").value = "";
  document.getElementById("file_upload_notice").innerHTML = "";
  document.getElementById("serialNo").value = msg["simno"];
  if (msg["simno"] != null) {
    $("#name_status").html("");
    if (countQuery == 1 || countQuery == "") {
      document.getElementById("fnext_ind").click();
    } else if (countQuery > 1) {
      $("#fnext_ind").removeClass("hiding");
    }
  } else {
    $("#name_status").html("Order is either canceled or invalid");
  }
}

function imei_eid(value) {
  if (value !== null) {
    if (value.includes("usa")) {
      $(".ind_imei").removeClass("hiding");
      //$('.imei_heading').html("IMEI Number (Optional)");
      $(".imei_heading").html(
        "IMEI Number <span class='small'>(Optional)</span>"
      );
    }
    if (
      value.toLowerCase().includes("usa") &&
      value.toLowerCase().includes("esim")
    ) {
      $(".ind_imei").removeClass("hiding");
$(".ind_eid").removeClass("hiding");

      $("#imei_ind").prop("required", true);
		$("#eid_ind").prop("required",true);
      $(".imei_heading").html("IMEI Number");
    }
  }
}
function amazon_order_esim(value) {
  const countries = ["japan", "europe"]; // Add more countries as needed
  if (
    value !== null &&
    countries.some((country) => value.toLowerCase().includes(country)) &&
    value.toLowerCase().includes("esim")
  ) {
    $("#amz_ord_mobi_label").html("Amazon Order ID");
    $("#amz_ord_mobi").prop("required", true);
  }
}

function checkSerialno() {
  $("#details").html("");
  $(".loader").removeClass("hiding");
  var number = document.getElementById("serialNo").value;
  var ordNo = document.getElementById("ordNo").value;
  console.log("number is :" + number);
  console.log("orderNumber: " + ordNo);
  var countQuery = document.getElementById("count_query").value;

  var site;
  if (ordNo) {
    //console.log("Inside ordNO");
    if (ordNo.includes("-")) {
      document.getElementById("Tsim").value = "amazon";
      site = "amazon";
    } else {
      document.getElementById("Tsim").value = "tsim";
      site = "tsim";
    }
    var prtsim = document.getElementById("Tsim").value;
    //console.log("prtsim : "+prtsim);
    $.ajax({
      type: "post",
      url: "checkvalue.php",
      dataType: "json",
      data: { order_no: ordNo, t: site, count_query: countQuery },
      success: function (response) {
        $(".loader").addClass("hiding");

        switch (response.code) {
          case 101:
            Swal.fire({
              icon: "error",
              title: "",
              text: "Order is either canceled or invalid",
            });
            $("#name_status").html("Order is either canceled or invalid");
            $("#fnext_ind").addClass("hiding");
            break;
          case 208:
            imei_eid(response.info);
            //console.log(response.code);
            //console.log(response.msg);
            handleResponse208(response.msg);
            break;
          case 209:
            imei_eid(response.info);
            //console.log(response);
            handleResponse209(response.msg);
            break;
          case 201:
            imei_eid(response.info);
            //console.log(response.code);
            //console.log(response.msg);
            handleResponse201(response.msg);
            break;
          default:
            break;
        }
      },
    });
  } else {
    $("#name_status").html("");
    return false;
  }
}

function check_num() {
  $("#details").html("");
  $(
    "#fname, #email, #pass_no, #pass_city, #expdate,#date, #active_date,#imei_ind,#eid_ind"
  ).removeAttr("required");
  var number = document.getElementById("serialNo").value;
  var countQuery = document.getElementById("count_query").value;

  if (number.length >= 5) {
    $.ajax({
      type: "post",
      url: "checkvalue.php",
      dataType: "json",
      data: { serial_no: number, count_query: countQuery },
      success: function (response) {
        $("#name_status").html(response.dmsg);
        //console.log("Response "+JSON.stringify(response.msg)+JSON.stringify(response.code));
        switch (response.code) {
          case 200:
            //console.log(response.code);
            $("#name_status").html(response.dmsg);
            console.log(response);

            return true;
          case 202:
            //console.log("in check num func  "+response.code);
            $("#name_status").html(response.dmsg);
            $("#fnext_mobi").addClass("hiding");
            return false;
          default:
            // console.log("in default case")
            $("#name_status").html(response.dmsg);
            return false;
        }
      },
    });
  } else {
    $("#name_status").html("");
    return false;
  }
}

$("#fnext_ind").click(function (e) {
  $("#fname,#email,#pass_no,#pass_city,#expdate,#date").prop("required", true);
  e.preventDefault();
  $(
    "#fname, #email, #mobileno, #pass_no, #pass_city, #expdate, #date, #imei_ind, #eid_ind"
  ).val("");
  var number = document.getElementById("serialNo").value;
  var prtsim = document.getElementById("Tsim").value;
  var number = document.getElementById("serialNo").value;
  var countQuery = document.getElementById("count_query").value;
  if (number.length >= 5) {
    var invalid = false;
    $(".loader").removeClass("hiding");
    $.ajax({
      type: "post",
      url: "checkvalue.php",
      dataType: "json",
      async: false,
      data: {
        serial_no: number,
        t: prtsim,
        count_query: countQuery,
      },
      success: function (response) {
        $(".loader").addClass("hiding");
        var total_esims = response.msg["total_esims"];
        //console.log("inside function", total_esims);

        var esim_sku = response.msg["esim_sku"];
        trip_ids = response.msg?.tripid ?? null;

        document.getElementById("trip_id").value = trip_ids;
        document.getElementById("total_esims").value = total_esims;

        if ($("#num").val()) {
          $("#name_status").html(response.dmsg);
        }
        //console.log("True Response " + response.code);
        // console.log("Message " + response.msg);
        var str = "OK";
        if (
          response.msg["activation_reqd"] == "f" ||
          response.msg["act_flag"] == "f"
        ) {
          $("#activation_info,#date,#active_date").addClass("hiding");
        }
        if (response.code == 200) {
          //console.log(response.dmsg);
          //console.log(response.code);
          //console.log(response.msg);
          return true;
        } else if (response.code == 209) {
          var from_date = response.msg["from_date"].split(" ")[0];
          var arr = from_date.split("-");
          document.getElementById("fname").value = response.msg["clientname"];
          document.getElementById("email").value = response.msg["emailadd"];
          document.getElementById("mobileno").value = response.msg["mobno"];
          document.getElementById("date").value =
            arr[1] + "/" + arr[2] + "/" + arr[0];
          $("#imei_ind").val(response.msg["imei"]);
          $("#eid_ind").val(response.msg["eid"]);
          return true;
        } else if (response.code == 203) {
          //console.log(response.msg);
          //console.log(response.code);
          var from_date = response.msg["from_date"].split(" ")[0];
          var arr = from_date.split("-");
          //var arr = response.msg['from_date'].replace(' 00:00:00', '').split("-");
          document.getElementById("fname").value = response.msg["clientname"];
          var labels;
          let labelsArray;
	//console.log("Here is total esims",total_esims);
	//console.log(total_esims);
          if (total_esims > 1) {
            labels = response.msg["labels"]; // Assign value to labels
            labelsArray = labels.split("_"); // Assign value to labelsArray
            //console.log("labelsArray is ", labelsArray);
            //console.log("labels is ", labels);
            document.getElementById("trip_id").value = response.msg["tripid"];
          }

          //console.log("Outside the if block - labels:", labels);
          //console.log("Outside the if block - labelsArray:", labelsArray);

          document.getElementById("email").value = response.msg["emailadd"];
          document.getElementById("mobileno").value = response.msg["mobno"];
          document.getElementById("expdate").value =
            response.msg["passport_exp_date"];
          //					//console.log("This is ttala:",totalEsims);
          document.getElementById("date").value =
            arr[1] + "/" + arr[2] + "/" + arr[0];
          if (total_esims > 1) {
            //console.log("labelsArray is ", labelsArray);
            duplicateFields(total_esims, labelsArray);
            imeiArray = response.msg["imei"]
              .split("_")
              .map((imei) => imei.trim());
            eidArray = response.msg["eid"].split("_").map((eid) => eid.trim());
            // Populate IMEI fields
            imeiArray.forEach((imei, index) => {
              $(`#imei_ind${index > 0 ? `_${index}` : ""}`).val(imei);
            });

            // Populate EID fields
            eidArray.forEach((eid, index) => {
              $(`#eid_ind${index > 0 ? `_${index}` : ""}`).val(eid);
            });
          } else {
            $("#imei_ind").val(response.msg["imei"]);
            $("#eid_ind").val(response.msg["eid"]);
          }

          //                    $("#imei_ind").val(response.msg['imei']);
          //                    $("#eid_ind").val(response.msg['eid']);
          //document.getElementById("total_esims").value = response.msg['total_esims'];
          //console.log(msg['total_esims']);
          if (response.msg["passport_no"] !== "X1550000") {
            document.getElementById("pass_no").value =
              response.msg["passport_no"];
          } else {
            document.getElementById("pass_no").value = " ";
          }
          if (response.msg["passport_city"] !== "NA") {
            document.getElementById("pass_city").value =
              response.msg["passport_city"];
          } else {
            document.getElementById("pass_city").value = " ";
          }

          // console.log("pass1 = " + response.msg['pass1']);
          //console.log("pass2 = " + response.msg['pass2']);
          //console.log("visa = " + response.msg['visa']);

          if (typeof response.msg.pass1 !== "undefined" && response.msg.pass1) {
            //console.log("pass1 is present");
            document.getElementById("pass1").style.display = "block";
            document.getElementById("image_pass1").style.display = "block";
            document.getElementById("pass1a").style.display = "block";
            document.getElementById("pfp").style.display = "block";
            document.getElementById("uploadedfp").innerHTML =
              "The front page of the passport was uploaded during the previous session. You can also upload it again if needed.";
          } else {
            //console.log("pass1 is not present");
            // Hide elements if pass1 is not present or is falsy (empty string, 0, null, etc.)
            document.getElementById("pass1").style.display = "block";
            document.getElementById("image_pass1").style.display = "block";
            document.getElementById("pass1a").style.display = "block";
            document.getElementById("pfp").style.display = "block";
          }

          if (typeof response.msg.pass2 !== "undefined" && response.msg.pass2) {
            // console.log("pass2 is present");
            document.getElementById("pass2").style.display = "block";
            document.getElementById("image_pass2").style.display = "block";
            document.getElementById("pass2a").style.display = "block";
            document.getElementById("pbp").style.display = "block";
            document.getElementById("uploadedbp").innerHTML =
              "The back page of the passport was uploaded during the previous session. Feel free to upload it again if required.";
          } else {
            // console.log("pass2 is not present");
            // Hide elements if pass2 is not present or is falsy
            document.getElementById("pass2").style.display = "block";
            document.getElementById("image_pass2").style.display = "block";
            document.getElementById("pass2a").style.display = "block";
            document.getElementById("pbp").style.display = "block";
          }

          if (typeof response.msg.visa !== "undefined" && response.msg.visa) {
            // console.log("visa is present");
            document.getElementById("visa").style.display = "block";
            document.getElementById("image_visa").style.display = "block";
            document.getElementById("visaa").style.display = "block";
            document.getElementById("vpo").style.display = "block";
            document.getElementById("uploadedvisa").innerHTML =
              "The Visa/Ticket (if visa is on arrival) was uploaded during the previous session. You can also upload it again if needed.";
          } else {
            //  console.log("visa is not present");
            // Hide elements if visa is not present or is falsy
            document.getElementById("visa").style.display = "block";
            document.getElementById("image_visa").style.display = "block";
            document.getElementById("visaa").style.display = "block";
            document.getElementById("vpo").style.display = "block";
          }

          return true;
        } else if (response.code == 205) {
          //console.log(response.msg);
          //console.log(response.code);
          $("#fnext_ind").addClass("hiding");
          $("#details").removeClass("hiding");
          $("#file_upload_notice").addClass("hiding");
          var value =
            "<h3 style='color:darkgreen;'>Your documents are verified!</h3><br>";
          value += "<b>Your profile information</b>";
          value += "<h5><b>Name : </b>" + response.msg["clientname"] + "</h5>";
          value += "<h5><b>Email : </b>" + response.msg["emailadd"] + "</h5>";
          if (response.msg["t"] !== "tsim") {
            value +=
              "<h5><b>Activation Date : </b>" +
              response.msg["from_date"].split(" ")[0] +
              "</h5>";
          }
          value +=
            "<h5><b>Your Contact Number : </b>" +
            response.msg["mobno"] +
            "</h5>";
          value +=
            "<div class='soit' <h5><b>Passport Front Page : </b> <a href='#' id=" +
            response.msg["pass1"] +
            ">Show</a>" +
            "</h5>";
          value +=
            "<h5><b>Passport Back Page : </b> <a href='#' id=" +
            response.msg["pass2"] +
            ">Show</a>" +
            "</h5>";
          if (typeof response.msg["visa"] === "undefined") {
            value += "<h5><b>Visa/Ticket : </b> " + "</h5>";
            $("#details").html(value);
            document.getElementById("fname").value = response.msg["clientname"];
            document.getElementById("email").value = response.msg["emailadd"];
            document.getElementById("mobileno").value = response.msg["mobno"];
            var from_date = response.msg["from_date"].split(" ")[0];
            document.getElementById("date").value = from_date;
          } else {
            value +=
              "<h5><b>Visa/Ticket : </b> <a href='#' id=" +
              response.msg["visa"] +
              ">Show</a>" +
              "</h5></div>";
            $("#details").html(value);
          }
          invalid = true;
          Swal.fire({
            icon: "success",
            title: "Documents are verified 😊",
            showCloseButton: true,
            showConfirmButton: true,
            showCancelButton: false,
            confirmButtonText: "OK",
            customClass: {
              confirmButton: "swal2-confirm-btn",
            },
          });
          return false;
        } else {
          invalid = true;
          return false;
        }
      },
    });
    if (invalid) {
      return false;
    }
  } else {
    $("#name_status").html("Serial number is not valid");
    return false;
  }
  if (animating) return false;
  animating = true;

  //console.log("Second Next clicked");
  current_fs = $(this).parent();
  next_fs = $(this).parent().next();

  //activate next step on progressbar using the index of next_fs
  $("#progressbar li").eq($("fieldset").index(next_fs)).addClass("active");

  //show the next fieldset
  next_fs.show();
  //hide the current fieldset with style
  current_fs.animate(
    { opacity: 0 },
    {
      step: function (now, mx) {
        //as the opacity of current_fs reduces to 0 - stored in "now"
        //1. scale current_fs down to 80%
        scale = 1 - (1 - now) * 0.2;
        //2. bring next_fs from the right(50%)
        left = now * 50 + "%";
        //3. increase opacity of next_fs to 1 as it moves in
        opacity = 1 - now;
        current_fs.css({
          transform: "scale(" + scale + ")",
          position: "absolute",
        });
        next_fs.css({
          left: left,
          opacity: opacity,
        });
      },
      duration: 300,
      complete: function () {
        current_fs.hide();
        animating = false;
      },
      //this comes from the custom easing plugin
      easing: "easeInOutBack",
    }
  );
});

$("#fnext_mobi").click(function (e) {
  $("#email_mobi,#fname_mobi,#date_mobi").prop("required", true);
  e.preventDefault();
  var number = document.getElementById("serialNo").value;
  var number = document.getElementById("serialNo").value;
  var countQuery = document.getElementById("count_query").value;

  $(
    "#fname_mobi,#email_mobi,#mobileno_mobi,#date_mobi,#amz_ord_mobi,#imei_mobi,#eid_mobi,#amz_ord_mobi"
  ).val("");
  if (number.length >= 5) {
    var invalid = false;
    $.ajax({
      type: "post",
      url: "checkvalue.php",
      dataType: "json",
      async: false,
      data: { serial_no: number, count_query: countQuery },
      success: function (response) {
        $("#name_status").html(response.dmsg);
        if (response.code == 200) {
          $("name_status").html(response.dmsg);
          amazon_order_esim(response.info["tripid"]);
          if (response.info["tripid"].toLowerCase().includes("usa")) {
            $("#zipcodes").removeClass("hiding");
            //$('.imei_heading').html("IMEI Number (Optional)");
            $(".imei_heading").html(
              "IMEI Number <span class='small'>(Optional)</span>"
            );
            $(".mobi_imei").removeClass("hiding");
          }
		if (
            (response.info["tripid"].toLowerCase().includes("usa") || response.info["tripid"].toLowerCase().includes("us_cn_mx")) &&
            response.info["tripid"].toLowerCase().includes("esim")
          )
{
            $(".mobi_imei,.mobi_eid").removeClass("hiding");
            $("#imei_mobi").prop("required", true); // Make IMEI required
            $("#eid_mobi").prop("required", true); // Make EID required
            $(".imei_heading").html("IMEI Number");
            $("#imei_mobi").val(response.info["imei"]);
            $("#eid_mobi").val(response.info["eid"]);
          }
          return true;
        } else if (response.code == 203) {
          $("#fnext_mobi").removeClass("hiding");
          amazon_order_esim(response.msg["tripid"]);
          //console.log(response.code);
          var value_text;
          value_text = "<b>Your profile information</b>";
          value_text +=
            "<h5><b>Name : </b>" + response.msg["clientname"] + "</h5>";
          value_text +=
            "<h5><b>Email : </b>" + response.msg["emailadd"] + "</h5>";
          if (response.msg["t"] !== "tsim") {
            value_text +=
              "<h5><b>Activation Date : </b>" +
              response.msg["from_date"].split(" ")[0] +
              "</h5>";
          }
          value_text +=
            "<h5><b>Your Contact Number : </b>" +
            response.msg["mobno"] +
            "</h5>";
          if (response.msg["sim_phone_no"]) {
            document.getElementById("fname_mobi").value =
              response.msg["clientname"];
            document.getElementById("email_mobi").value =
              response.msg["emailadd"];
            document.getElementById("mobileno_mobi").value =
              response.msg["mobno"];
            if (response.msg["order_no"]) {
              document.getElementById("amz_ord_mobi").value =
                response.msg["order_no"];
            }
            var from_date = response.msg["from_date"].split(" ")[0];
            var from_date_arr = from_date.split("-");
            //let from_date_arr = response.msg['from_date'].replace(' 00:00:00', '').split("-");
            document.getElementById("date_mobi").value =
              from_date_arr[1] +
              "/" +
              from_date_arr[2] +
              "/" +
              from_date_arr[0];
            if (response.msg["tripid"].toLowerCase().includes("usa")) {
              $("#zipcodes").removeClass("hiding");
              $("#zipcode").val(response.msg["add_pinno"]);
              $(".mobi_imei").removeClass("hiding");
              $("#imei_mobi").val(response.msg["imei"]);
              //$('.imei_heading').html("IMEI Number (Optional)");
              $(".imei_heading").html(
                "IMEI Number <span class='small'>(Optional)</span>"
              );
            }
		    if (
            (response.msg["tripid"].toLowerCase().includes("usa") || response.msg["tripid"].toLowerCase().includes("us_cn_mx")) &&
            response.msg["tripid"].toLowerCase().includes("esim")
                ) 		  {
              $(".imei_heading").html("IMEI Number");
              $(".mobi_imei,.mobi_eid").removeClass("hiding");
          //    console.log("before making compulsory");
		$("#imei_mobi").prop("required", true); // Make IMEI required
              $("#eid_mobi").prop("required", true); // Make EID required
              $("#imei_mobi").val(response.msg["imei"]);
              $("#eid_mobi").val(response.msg["eid"]);
            }
            $("#details").removeClass("hiding");
            $("#details").html(value_text);
            return true;
          } else {
            $("#details").html(value_text);
            invalid = true;
            return false;
          }
        } else {
          //console.log("response code  "+response.code);
          $("#name_status").html(response.dmsg);
          $("#fnext_mobi").addClass("hiding");
          invalid = true;
          return false;
        }
      },
    });
    if (invalid) {
      return false;
    }
  } else {
    $("#name_status").html("Serial number is not valid");
    return false;
  }

  if (animating) return false;
  animating = true;

  //console.log("Next clicked");
  current_fs = $(this).parent();
  //next_fs = $(this).parent().next();
  next_fs = $(this).parent().nextAll("fieldset").eq(2);
  //activate next step on progressbar using the index of next_fs
  $("#progressbar li").eq($("fieldset").index(next_fs)).addClass("active");

  //show the next fieldset
  next_fs.show();
  //hide the current fieldset with style
  current_fs.animate(
    { opacity: 0 },
    {
      step: function (now, mx) {
        //as the opacity of current_fs reduces to 0 - stored in "now"
        //1. scale current_fs down to 80%
        scale = 1 - (1 - now) * 0.2;
        //2. bring next_fs from the right(50%)
        left = now * 50 + "%";
        //3. increase opacity of next_fs to 1 as it moves in
        opacity = 1 - now;
        current_fs.css({
          transform: "scale(" + scale + ")",
          position: "absolute",
        });
        next_fs.css({ left: left, opacity: opacity });
      },
      duration: 800,
      complete: function () {
        current_fs.hide();
        animating = false;
      },
      //this comes from the custom easing plugin
      easing: "easeInOutBack",
    }
  );
});

function validateEmail(email) {
  var regex = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
  return regex.test(email);
}
function containsInvalidCharacters(input) {
  const regex = /<[^>'"]*>|[{}?'"]/;
  return regex.test(input);
}

$("#snext_ind").click(function (e) {
  e.preventDefault();
  var passport_no = document.getElementById("pass_no").value;
  var passport_city = document.getElementById("pass_city").value;
  var serialNo = document.getElementById("serialNo").value;
  var name = document.getElementById("fname").value;
  var email = document.getElementById("email").value;
  var mobileno = document.getElementById("mobileno").value;
  var date = document.getElementById("date").value;
  var expdate = document.getElementById("expdate").value;
  let imei = document.getElementById("imei_ind").value;
  let eid = document.getElementById("eid_ind").value;
  var total_esims = document.getElementById("total_esims").value;
  //console.log("finally here:", total_esims);
  var trip_ids = document.getElementById("trip_id").value;
  //console.log("This is here trips", trip_ids);
  let imeiCSV = "";
  let eidCSV = "";

  for (let i = 0; i < total_esims; i++) {
    let imeiField =
      document.getElementById(`imei_ind_${i}`) ||
      document.getElementById("imei_ind");
    let eidField =
      document.getElementById(`eid_ind_${i}`) ||
      document.getElementById("eid_ind");

    if (imeiField) imeiCSV += (imeiCSV ? "_" : "") + imeiField.value;
    if (eidField) eidCSV += (eidCSV ? "_" : "") + eidField.value;
  }

  //console.log("IMEI Values:", imeiCSV);
  //console.log("EID Values:", eidCSV);
  var prtsim = document.getElementById("Tsim").value;
  var today = new Date();
  var day = today.getDate();
  var mon = today.getMonth() + 1;
  var year = today.getFullYear();
  var dt = mon + "/" + day + "/" + year;
  $(".note").empty();
  var errorMessage = "";

  if (name === "" || containsInvalidCharacters(name)) {
    errorMessage += "Please enter a valid Name.\n";
  }
  if (mobileno === "") {
    errorMessage += "Please enter a valid Mobile Number.\n";
  }
  if (date === "") {
    errorMessage += "Please enter a valid Activation Date.\n";
  }

  if (errorMessage !== "") {
    Swal.fire({
      icon: "error",
      title: "Invalid Input",
      text: errorMessage.replace(/\n/g, "<br/>"),
      html: errorMessage.replace(/\n/g, "<br/>"),
    });
    return;
  }

  if (email && !validateEmail(email)) {
    Swal.fire({
      icon: "error",
      title: "Invalid Input",
      text: "Enter Valid Email Address",
    });
    return;
  }
  // Check if IMEI field has required attribute
  /* const imeiField = document.getElementById("imei_ind");
    const isImeiRequired = imeiField.hasAttribute("required");

    if (isImeiRequired && !imei) {
        Swal.fire({
            icon: 'error',
            title: 'Invalid Input',
            text: "Please fill out the IMEI field."
        });
        return;
    }
    if (imei && imei.length!=15) {
        Swal.fire({
            icon: 'error',
            title: 'Invalid Input',
            text: "Please enter a valid IMEI number. The number should be 15 digits long."
        });
        return;
    }
    if (eid && eid.length!=32) {
        Swal.fire({
            icon: 'error',
            title: 'Invalid Input',
            text: "Please enter a valid EID number. The number should be 32 digits long."
        });
        return;
    }
*/
  const imeiFields = document.querySelectorAll("input[name^='imei_ind']");
  for (const imeiField of imeiFields) {
    if (imeiField.hasAttribute("required") && !imeiField.value) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Please fill out all required IMEI fields.",
      });
      return;
    }
    if (imeiField.value && imeiField.value.length !== 15) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Please enter a valid IMEI number. The number should be 15 digits long.",
      });
      return;
    }
  }

  // Validate all EID fields
  const eidFields = document.querySelectorAll("input[name^='eid_ind']");
  for (const eidField of eidFields) {
    if (eidField.hasAttribute("required") && !eidField.value) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Please fill out all required EID fields.",
      });
      return;
    }
    if (eidField.value && eidField.value.length !== 32) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Please enter a valid EID number. The number should be 32 digits long.",
      });
      return;
    }
  }

  var datas = JSON.stringify({
    serialNo: serialNo,
    fname: name,
    email: email,
    mobileno: mobileno,
    date: date,
    ts: prtsim,
    passport_no: passport_no,
    passport_city: passport_city,
    expdate: expdate,
    total_esims: total_esims,
    tripid: trip_ids,
    imei: total_esims > 1 ? imeiCSV : imei,
    eid: total_esims > 1 ? eidCSV : eid,
  });
  if (total_esims > 1) {
    datas.tripid = trip_ids;
  }
  $.ajax({
    url: "checkvalue.php",
    type: "POST",
    data: {
      json: datas,
    },
    dataType: "json",
    success: function (result) {
      //console.log(result["msg"]);
    },
    error: function (jqXHR, textStatus, errorThrown) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "There was an error while submitting the form. Please try again later.",
      });
      // alert("There was an error while submitting the form. Please try again later.");
    },
  });

  if (animating) return false;
  animating = true;
  current_fs = $(this).parent();
  next_fs = $(this).parent().next();
  //activate next step on progressbar using the index of next_fs
  $("#progressbar li").eq($("fieldset").index(next_fs)).addClass("active");
  next_fs.show();
  current_fs.animate(
    {
      opacity: 0,
    },
    {
      step: function (now, mx) {
        scale = 1 - (1 - now) * 0.2;
        left = now * 50 + "%";
        opacity = 1 - now;
        current_fs.css({
          transform: "scale(" + scale + ")",
          position: "absolute",
        });

        next_fs.css({
          left: left,
          opacity: opacity,
        });
      },
      duration: 300,
      complete: function () {
        current_fs.hide();
        animating = false;
      },
      easing: "easeInOutBack",
    }
  );
});

$(".previous").click(function () {
  if (animating) return false;
  animating = true;
  $("#fnext_ind").removeClass("hiding");
  current_fs = $(this).parent();
  previous_fs = $(this).parent().prev();

  //de-activate current step on progressbar
  $("#progressbar li")
    .eq($("fieldset").index(current_fs))
    .removeClass("active");

  //show the previous fieldset
  previous_fs.show();
  //hide the current fieldset with style
  current_fs.animate(
    {
      opacity: 0,
    },
    {
      step: function (now, mx) {
        //as the opacity of current_fs reduces to 0 - stored in "now"
        //1. scale previous_fs from 80% to 100%
        scale = 0.8 + (1 - now) * 0.2;
        //2. take current_fs to the right(50%) - from 0%
        left = (1 - now) * 50 + "%";
        //3. increase opacity of previous_fs to 1 as it moves in
        opacity = 1 - now;
        current_fs.css({
          left: left,
        });
        previous_fs.css({
          transform: "scale(" + scale + ")",
          opacity: opacity,
        });
      },
      duration: 800,
      complete: function () {
        current_fs.hide();
        animating = false;
      },
      //this comes from the custom easing plugin
      easing: "easeInOutBack",
    }
  );
});

$(".previous_mobi").click(function () {
  if (animating){ 
//console.log("animating true returning false");
return false;}
//console.log("is false setting true");
  animating = true;
  current_fs = $(this).parent();
  //previous_fs = $(this).parent().prev();
  previous_fs = $(this).parent().siblings("fieldset").first();
  //document.getElementById('sim-card-selection-frame').style.display='block';
let counts = sessionStorage.getItem("count_Query");
var viewportWidth = $(window).width();
if (counts >1) {
            document.getElementById("sim-card-selection-frame").style.display = "block";
    		var viewFrames = document.getElementById("sim-card-selection-frame");    
	} else {
            // Optionally hide the frame if the flag is not set
            document.getElementById("sim-card-selection-frame").style.display = "none";
			var viewFrames =  document.getElementById("num");
        }
/*
if (viewportWidth <= 768) {
    setTimeout(() => {
        var viewFrames = document.getElementById("num");
        viewFrames.focus(); // Ensure the field gets focus
        viewFrames.scrollIntoView({ behavior: "smooth", block: "center" });
    }, 300); // Delay scrolling slightly to handle layout changes
}
*/
//previous is here

    setTimeout(() => {
        
        // Temporarily disable focus to prevent keyboard popup
        viewFrames.setAttribute("readonly", true);

        // Scroll smoothly to the input field
        viewFrames.scrollIntoView({ behavior: "smooth", block: "center" });

        // Re-enable focus after scrolling completes
        setTimeout(() => {
            viewFrames.removeAttribute("readonly");
        }, 500); // Delay re-enabling to prevent keyboard from opening
    }, 300);



//de-activate current step on progressbar
  $("#progressbar li")
    .eq($("fieldset").index(current_fs))
    .removeClass("active");

  //show the previous fieldset
  previous_fs.show();
  //hide the current fieldset with style
  current_fs.animate(
    {
      opacity: 0,
    },
    {
      step: function (now, mx) {
        //as the opacity of current_fs reduces to 0 - stored in "now"
        //1. scale previous_fs from 80% to 100%
        scale = 0.8 + (1 - now) * 0.2;
        //2. take current_fs to the right(50%) - from 0%
        left = (1 - now) * 50 + "%";
        //3. increase opacity of previous_fs to 1 as it moves in
        opacity = 1 - now;
        current_fs.css({
          left: left,
        });
        previous_fs.css({
          transform: "scale(" + scale + ")",
          opacity: opacity,
        });
      },
      duration: 800,
      complete: function () {
        current_fs.hide();
        animating = false;

        // Update progress bar for the previous fieldset
        //updateProgressBar("previous");
      },

      //this comes from the custom easing plugin
      easing: "easeInOutBack",
    }
  );
});
$(document).ready(function () {
  $(document).on("click", ".icon-info", function () {
    window.open(
      "https://www.tsim.mobi/wp-content/uploads/2024/12/IMEI_video.mp4",
      "Imei Video"
    );
  });
});


$("#msform").on("click", "#msform_submit", function (e) {
  e.preventDefault();

  if ($("#fieldset4").hasClass("hiding")) {
    $("#fieldset4 input[required]").removeAttr("required");
  }

  var file1 = document.querySelector("#pass1").files[0];
  var file2 = document.querySelector("#pass2").files[0];
  var file3 = document.querySelector("#visa").files[0];
  $(".overlay").show();
  if (e.isDefaultPrevented()) {
    var file1 = document.querySelector("#pass1").files[0];
    var file2 = document.querySelector("#pass2").files[0];
    var file3 = document.querySelector("#visa").files[0];

    if (file1) {
      file_arr.push(file1.name);
    }
    if (file2) {
      file_arr.push(file2.name);
    }
    if (file3) {
      file_arr.push(file3.name);
    }

    var extensions = ["JPG", "jpg", "jpeg", "png", "pdf", "doc", "docx"];

    for (var i = 0; i < file_arr.length; i++) {
      var ext = file_arr[i].substr(file_arr[i].lastIndexOf(".") + 1);

      if (!extensions.includes(ext)) {
        Swal.fire(
          "Oops!",
          "Please upload [ jpg , jpeg , png , pdf , doc , docx ] types of documents only",
          "error"
        ).then((value) => {
          file_arr = [];
        });

        $(".overlay").hide();
        return false;
      }
    }

    $(".note").empty();
    var form = $("form").get(0);
    $.ajax({
      url: "checkvalue.php",
      type: "POST",
      data: new FormData(form),
      contentType: false,
      cache: false,
      processData: false,
      success: function (result) {
        $(".overlay").hide();
        var formData = new FormData(form);
        result = JSON.parse(result);
        if (
          result.msg ===
          "Your form has been successfully submitted. Thank you! We will reach out to you via email shortly."
        ) {
          Swal.fire("Good job!", result.msg, "success").then((value) => {
            $("#msform")[0].reset();
            location.reload();
          });
        } else if (
          result.msg ===
          "There was an error while submitting the form. Please try again later"
        ) {
          Swal.fire("Oops!", result.msg, "error").then((value) => {
            $("#msform")[0].reset();
            location.reload();
          });
        }
      },
      error: function (error) {
        Swal.fire(
          "Oops!",
          "There was an error while submitting the form. Please try again later",
          "error"
        ).then((value) => {
          $(".overlay").hide();
          $("#msform")[0].reset();
          location.reload();
        });
      },
    });

    return false;
  }
});

$("#msform").on("click", "#mobi_submitform", function (e) {
  e.preventDefault();
  $(".overlay").show();
  if (e.isDefaultPrevented()) {
    var $form = $("#msform");
    var datas = getFormData($form);
    let mobi_data = {
      serialNo: datas.serialNo,
      fname_mobi: datas.fname_mobi,
      email_mobi: datas.email_mobi,
      mobileno_mobi: datas.mobileno_mobi,
      amz_ord_mobi: datas.amz_ord_mobi,
      date_mobi: datas.date_mobi,
      imei_mobi: datas.imei_mobi,
      eid_mobi: datas.eid_mobi,
    };

    if (!datas.fname_mobi) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "First Name is required",
      });
      $(".overlay").hide();
      return false;
    }

    if (!datas.email_mobi) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Email Address is required",
      });
      $(".overlay").hide();
      return false;
    }

    if (datas.email_mobi && !validateEmail(datas.email_mobi)) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Enter Valid Email Address",
      });
      $(".overlay").hide();
      return false;
    }
    const imei_field = document.getElementById("imei_mobi");
    const ImeiRequired = imei_field.hasAttribute("required");
   const eid_field = document.getElementById("eid_mobi");
	const EidRequired = eid_field.hasAttribute("required"); 
	const isAmazonOrderRequired = document
      .getElementById("amz_ord_mobi")
      .hasAttribute("required");
    if (ImeiRequired && !datas.imei_mobi) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Please fill out the IMEI field.",
      });
      $(".overlay").hide();
      return;
    }
 let imei_num = datas.imei_mobi;
    let eid_num = datas.eid_mobi;
	if (EidRequired && !datas.eid_mobi) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Please fill out the EID field.",
      });
      $(".overlay").hide();
      return;
    }

       if (imei_num && imei_num.length != 15) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Please enter a valid IMEI number. The number should be 15 digits long.",
      });
      $(".overlay").hide();
      return;
    }
    if (eid_num && eid_num.length != 32) {
      Swal.fire({
        icon: "error",
        title: "Invalid Input",
        text: "Please enter a valid EID number. The number should be 32 digits long.",
      });
      $(".overlay").hide();
      return;
    }

        if (isAmazonOrderRequired) {
            const amazonOrderId = datas.amz_ord_mobi;
            const amazonOrderIdRegex = /^\d{3}-\d{7}-\d{7}$/;
        
            if (!amazonOrderId) {
                Swal.fire({
                    icon: 'error',
                    title: 'Invalid Input',
                    text: "Please fill out the Amazon Order ID."
                });
                $(".overlay").hide();
                return;
            }
        
            if (!amazonOrderIdRegex.test(amazonOrderId)) {
                Swal.fire({
                    icon: 'error',
                    title: 'Invalid Format',
                    text: "Please enter a valid Amazon Order ID in the format 222-3333333-4444444."
                });
                $(".overlay").hide();
                return;
            }
        }
        


 $.ajax({
        url: 'checkvalue.php',
        type: 'POST',
        data: { mobi_json: JSON.stringify(mobi_data) },
        dataType: 'json',
        success: function (result) {
            $(".overlay").hide();
            switch (result["code"]) {
		case 207:
		//added this case, if 1 card got activated from multiple cards we'll handle that condition here.
		 Swal.fire("Good job!", result["msg"], "success").then(() => {
                            // Reset the form for the next input
                            $("#msform")[0].reset();

              // Update progress bar dynamically
              //activationProgress.currentCard++;
              //updateProgressBar();

                            // Reload or any additional logic
                            //location.reload();
                        });
                        break;
                 case 200:
		 Swal.fire("Good job!", result["msg"], "success").then(() => {
                            // Reset the form for the next input
                            $("#msform")[0].reset();

              // Mark one card as completed
              //markItemAsCompleted();

              // Update progress bar dynamically
              //activationProgress.currentCard++;
              //updateProgressBar();

              // Reload or any additional logic
              location.reload();
            });
            break;

                    default:
                        Swal.fire("Error", result["msg"] || "Unknown error", "error");
                }
            },
            error: function (error) {
                $(".overlay").hide();
                Swal.fire("Oops!", "There was an error while submitting the form. Please try again later", "error");
            }
        });
    }
});





$("#pass1").on('change', function() {

    if (typeof(FileReader) != "undefined") {

        var file = this.files[0];
       // console.log(file);
        var canvas = document.getElementById('image_c_pass1');
        var image_holder = $("#image_pass1");
        image_holder.empty();
        canvas.style.display = 'none';
        var mime_types = ['application/pdf'];
        var img_arr = ['image/jpeg', 'image/jpg', 'image/png'];
       // console.log(file.type);
        if (mime_types.indexOf(file.type) == -1 && img_arr.includes(file.type)) {
        //    console.log("Non pdf")

            var reader = new FileReader();
            reader.onload = function(e) {
                $("<img />", {
                    "src": e.target.result,
                    "class": "thumb-image"
                }).appendTo(image_pass1);

            }
            image_holder.show();
            reader.readAsDataURL($(this)[0].files[0]);
        } else if (mime_types.indexOf(file.type) == 0) {
            if (file.size > 10 * 1024 * 1024) {
                alert('Error : Exceeded size 10MB');
                return;
            }
           // console.log("PDF");

            // object url of PDF 
            _OBJECT_URL = URL.createObjectURL(file)
          //  console.log(_OBJECT_URL);
            // send the object url of the pdf to the PDF preview function
            var id = 'image_c_pass1';
            _CANVAS = document.querySelector('#' + id);
            image_c_pass1 = file;
         //   console.log(image_c_pass1);
            showPDF(_OBJECT_URL, id);

        }

    } else {
        alert("This browser does not support FileReader.");
    }
});

$("#pass2").on("change", function () {
  if (typeof FileReader != "undefined") {
    var file = this.files[0];
    //console.log(file);
    var canvas = document.getElementById("image_c_pass2");
    var image_holder = $("#image_pass2");
    image_holder.empty();
    canvas.style.display = "none";
    var mime_types = ["application/pdf"];
    var img_arr = ["image/jpeg", "image/jpg", "image/png"];
    //console.log(file.type);
    if (mime_types.indexOf(file.type) == -1 && img_arr.includes(file.type)) {
      //    console.log("Non pdf")

      var reader = new FileReader();
      reader.onload = function (e) {
        $("<img />", {
          src: e.target.result,
          class: "thumb-image",
        }).appendTo(image_pass2);
      };
      image_holder.show();
      reader.readAsDataURL($(this)[0].files[0]);
    } else if (mime_types.indexOf(file.type) == 0) {
      if (file.size > 10 * 1024 * 1024) {
        alert("Error : Exceeded size 10MB");
        return;
      }
      //   console.log("PDF");

      // object url of PDF
      _OBJECT_URL = URL.createObjectURL(file);
      //console.log(_OBJECT_URL);
      // send the object url of the pdf to the PDF preview function
      var id = "image_c_pass2";
      _CANVAS = document.querySelector("#" + id);
      image_c_pass2 = file;
      showPDF(_OBJECT_URL, id);
    }
  } else {
    alert("This browser does not support FileReader.");
  }
});

$("#visa").on("change", function () {
  if (typeof FileReader != "undefined") {
    var file = this.files[0];
    //console.log(file);
    var canvas = document.getElementById("image_c_visa");
    var image_holder = $("#image_visa");
    image_holder.empty();
    canvas.style.display = "none";
    var mime_types = ["application/pdf"];
    var img_arr = ["image/jpeg", "image/jpg", "image/png"];
    //console.log(file.type);
    if (mime_types.indexOf(file.type) == -1 && img_arr.includes(file.type)) {
      //    console.log("Non pdf")

      var reader = new FileReader();
      reader.onload = function (e) {
        $("<img />", {
          src: e.target.result,
          class: "thumb-image",
        }).appendTo(image_visa);
      };
      image_holder.show();
      reader.readAsDataURL($(this)[0].files[0]);
    } else if (mime_types.indexOf(file.type) == 0) {
      if (file.size > 10 * 1024 * 1024) {
        alert("Error : Exceeded size 10MB");
        return;
      }
      //  console.log("PDF");

      // object url of PDF
      _OBJECT_URL = URL.createObjectURL(file);
      // console.log(_OBJECT_URL);
      // send the object url of the pdf to the PDF preview function
      //
      var id = "image_c_visa";
      _CANVAS = document.querySelector("#" + id);
      image_c_visa = file;
      showPDF(_OBJECT_URL, id);
    }
  } else {
    alert("This browser does not support FileReader.");
  }
});

function showPDF(pdf_url, id) {
  PDFJS.getDocument({
    url: pdf_url,
  })
    .then(function (pdf_doc) {
      _PDF_DOC = pdf_doc;

      //console.log(id);
      showPage(1, id);

      // destroy previous object url
      URL.revokeObjectURL(_OBJECT_URL);
    })
    .catch(function (error) {
      alert(error.message);
    });
}

function getPages(pdf_url, id) {
  PDFJS.getDocument({
    url: pdf_url,
  })
    .then(function (pdf_doc) {
      // console.log("Inside GET PAGES");
      _PDF_DOC = pdf_doc;
      _TOTAL_PAGES = _PDF_DOC.numPages;
      //console.log("Total Pages " + _TOTAL_PAGES);
      // Show the first page
      //console.log(id);
      // showPage(1,id);
      //return _TOTAL_PAGES;
      // destroy previous object url
      URL.revokeObjectURL(_OBJECT_URL);
    })
    .catch(function (error) {
      // trigger Cancel on error
      document.querySelector("#cancel-pdf").click();

      // error reason
      alert(error.message);
    });
  //return Promise.resolve(_TOTAL_PAGES);
  //console.log(ch);
  return _TOTAL_PAGES;
}

function showPage(page_no, id, button) {
  // fetch the page
  _CANVAS = document.querySelector("#" + id);
  _PDF_DOC.getPage(page_no).then(function (page) {
    // set the scale of viewport
    //var scale_required = _CANVAS.width / page.getViewport(1).width;
    var scale_required = 2;
    // get viewport of the page at required scale
    var viewport = page.getViewport(scale_required);
    _CANVAS.height = viewport.height;
    _CANVAS.width = viewport.width;
    // set canvas height
    _CANVAS.height = viewport.height;

    var renderContext = {
      canvasContext: _CANVAS.getContext("2d"),
      viewport: viewport,
    };

    // render the page contents in the canvas
    page.render(renderContext).then(function () {
      if (button) {
        var sourceCanvas = document.getElementById(id);
        // console.log("Insdie Button");
        var canvas = Swal.getContent().querySelector("#cann");
        // console.log(canvas);
        var destCtx = canvas.getContext("2d");
        var scale = Math.min(
          canvas.width / sourceCanvas.width,
          canvas.height / sourceCanvas.height
        );

        var x = canvas.width / 2 - (sourceCanvas.width / 2) * scale;
        var y = canvas.height / 2 - (sourceCanvas.height / 2) * scale;
        //call its drawImage() function passing it the source canvas directly
        //              destCtx.drawImage(sourceCanvas, 0, 0,sourceCanvas.width/2,sourceCanvas.height/2);
        destCtx.drawImage(
          sourceCanvas,
          x,
          y,
          sourceCanvas.width * scale,
          sourceCanvas.height * scale
        );
      } else {
        // console.log("inside else of showpage function")
        document.querySelector("#" + id).style.display = "inline-block";
      }
    });
  });
}

var __G_ID;

$("canvas").click(function (e) {
  //console.log(".canvas click triggered");
  // console.log(this.id);
});

function canvasClick(canvas) {
  __G_ID = canvas;
  var sourceCanvas = document.getElementById(canvas);
  //console.log(sourceCanvas);
  var pages;
  // console.log("FILES/********************");
  // console.log(eval(canvas));
  //console.log("FILES/********************");
  var lastone = eval(canvas);
  //	showPDF(lastone,canvas);
  pages = getPages(URL.createObjectURL(lastone), canvas);

  setTimeout(function () {
    // console.log("PAGES Rendered " + pages);
    Swal.fire({
      width: 700,
      padding: "0em",
      title: "<strong>Total Pages " + _TOTAL_PAGES + "</strong>",
      type: "info",
      html:
        '<button id="pdf-prev">Previous</button> &nbsp ' +
        '<button id="pdf-next">Next</button><br>' +
        '<canvas id="cann" width="800" height="800"></canvas>',
      onBeforeOpen: () => {
        var canvas = Swal.getContent().querySelector("#cann");
        // console.log();
        var destCtx = canvas.getContext("2d");
        //	destCtx.scale(1, 1);
        // get the scale
        var scale = Math.min(
          canvas.width / sourceCanvas.width,
          canvas.height / sourceCanvas.height
        );
        // get the top left position of the image
        var x = canvas.width / 2 - (sourceCanvas.width / 2) * scale;
        var y = canvas.height / 2 - (sourceCanvas.height / 2) * scale;
        //call its drawImage() function passing it the source canvas directly
        destCtx.drawImage(
          sourceCanvas,
          x,
          y,
          sourceCanvas.width * scale,
          sourceCanvas.height * scale
        );
        _CURRENT_PAGE = 1;
      },
    }).then((value) => {
      showPage(1, __G_ID);
      // console.log("close");
    });
  }, 300);
}

$(document).on("click", "#pdf-prev", function () {
  //  alert(this.id);
  if (_CURRENT_PAGE != 1) {
    showPage(--_CURRENT_PAGE, __G_ID, true);
  }
});
$(document).on("click", "#pdf-next", function () {
  //  alert(this.id);
  if (_CURRENT_PAGE != _TOTAL_PAGES) {
    showPage(++_CURRENT_PAGE, __G_ID, true);
  }
});

function imgClick(image_id) {
  var global_id;

  if (image_id === "image_pass1") {
    // console.log("/////////////image_pass1");
    global_id = image_id;
  } else if (image_id === "image_pass2") {
    global_id = image_id;

    //  console.log("/////////////image_pass2");
  } else if (image_id === "image_visa") {
    global_id = image_id;

    // console.log("/////////////image_visa");
  }
  Swal.fire({
    width: 700,
    padding: "0em",
    title: "<strong>Image File</strong>",
    type: "info",
    html: '<div id="img-prev"></div>',
    onBeforeOpen: () => {
      var imgg = $("#" + global_id)
        .children("img")
        .clone()
        .addClass("new-image");
      $("#img-prev").append(imgg);
    },
  }).then((value) => {
    // console.log("close");
  });
}

function check(url) {
  var url = url;
  PDFJS.getDocument(url).then(function (pdf) {
    var container = Swal.getContent().querySelector("#show_container");

    // Loop from 1 to total_number_of_pages in PDF document
    for (var i = 1; i <= pdf.numPages; i++) {
      // Get desired page
      pdf.getPage(i).then(function (page) {
        var scale = 2;
        var viewport = page.getViewport(scale);
        var div = document.createElement("div");

        // Set id attribute with page-#{pdf_page_number} format
        div.setAttribute("id", "page-" + (page.pageIndex + 1));

        // This will keep positions of child elements as per our needs
        div.setAttribute("style", "position: relative");

        // Append div within div#container
        container.appendChild(div);

        // Create a new Canvas element
        var canvas = document.createElement("canvas");

        // Append Canvas within div#page-#{pdf_page_number}
        div.appendChild(canvas);

        var context = canvas.getContext("2d");
        canvas.height = viewport.height;
        canvas.width = viewport.width;

        var renderContext = {
          canvasContext: context,
          viewport: viewport,
        };

        // Render PDF page
        page.render(renderContext);
      });
    }
  });
}

function getCookie(cname) {
  var name = cname + "=";
  var decodedCookie = decodeURIComponent(document.cookie);
  var ca = decodedCookie.split(";");
  for (var i = 0; i < ca.length; i++) {
    var c = ca[i];
    while (c.charAt(0) == " ") {
      c = c.substring(1);
    }
    if (c.indexOf(name) == 0) {
      return c.substring(name.length, c.length);
    }
  }
  return "";
}

async function pdf_canvas(url) {
    const { value: swal } = await Swal.fire({
        icon: 'info',
        width: 700,
        title: '<strong>PDF File</strong>',
        html: '<div id="pdf-container"></div>',
        showCloseButton: true, 
        showConfirmButton: false,
        didOpen: async () => {
            try {
                const pdf = await PDFJS.getDocument({ url: url }).promise;
                const container = document.getElementById('pdf-container');
                for (let pageNum = 1; pageNum <= pdf.numPages; pageNum++) {
                    //console.log("1");
                    const page = await pdf.getPage(pageNum);
                    const scale = 1.5;
                    const viewport = page.getViewport({ scale });
                   // console.log("2");
                    const canvas = document.createElement('canvas');
                    const context = canvas.getContext('2d');
                    canvas.height = viewport.height;
                    canvas.width = viewport.width;
                    const renderContext = {
                        canvasContext: context,
                        viewport: viewport
                    };
                    //console.log("3");
                    container.appendChild(canvas);
                    await page.render(renderContext).promise;
                }
            } catch (error) {
                console.error('Error displaying PDF:', error);
                Swal.fire({
                    icon: 'error',
                    title: 'Error',
                    text: 'Failed to display PDF'
                });
            }
        }
    });
}


$('#details').on("click", "a", function(e) {
    // console.log("Helooooo");
    // var x2 = getCookie("OMUSER");
    // console.log(x2);
    var url = "https://mail.tsim.in/bc.e?i=" + (this.id.includes('/') ? this.id.split('/')[1] : this.id) + "&d=tsim_doc";

    // if (x2 == "services") {
    //     url += "&f=2500&z=z";
    // } else {
    url += "&a=U&dl=yes&l=services";
    // }

    if (this.id.includes('/')) {
        var subdirectory = this.id.split('/')[0];
        url = url.replace("d=tsim_doc", "d=tsim_doc/" + subdirectory);
    }

    //console.log("URL is =" + url);

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


function amazon_check_process(e){
    //console.log(e.target.value)
    let num;
    let show_qr_direct=false;
    if (typeof e === 'string') {
        num = e; // Direct value passed
        show_qr_direct=true;
    } else {
        num = e.target.value; // Event object passed
    }
    // num = e.target.value;
    if(num.length>5){
        $.ajax({
            type: 'post',
            url: 'amazon_qr_check.php',
            dataType: "json",
            data: {
                amazon_orderid: num,
                timestamp: new Date().getTime()
            },
          success: function(response) {
            switch(response.code) {
                case 200:
                    // Ensure the container is empty before appending buttons
                    $("#qr-buttons-container").empty();

                    if (response.res.length > 0) {
                        console.log(response);
                        $("#response-message").html(response.msg);
                        $("#response-message").removeClass("hiding");

                        response.res.forEach(function(item, index) {
                            if (item) { // Check if item is not null
                                console.log("Processing item " + index);
                                let btnId = `btn-${index}`;

                                let title = $('<p>')
                                .text(`${index+1}: ${item['title']}`)
                                .css('font-size', 'small'); // Set the font size to small
   // Create a button for each item
                                let btn = $('<button>')
                                    .attr('id', btnId)
                                    .addClass('qr-btn');

                                if (item['qrtext'] && item['qrtext'] != '') {
                                    // If QR code is present
                                    btn.text(`View QR Code ${index+1}`)
                                       .on("click", function() {
                                            console.log("Button " + index + " is clicked to show the QR popup");
                                            showQRCode(item);
                                        });
                                } else {
                                    // If QR code is not present
                                    btn.text(`${index+1} - QR code will be emailed to you.`)
                                       .attr('disabled', true) // Make the button non-clickable
                                       .css({
                                           'background-color': 'darkcyan',
                                           'cursor': 'not-allowed'
                                       });
                                }
                                // Append each button to the QR buttons container
                                $("#qr-buttons-container").append(title,btn);
                            }
                        });
                        let closeButton = $('<button>')
                            .attr('id', 'qrCloseButton')
                            .text('Close')
                            .on("click", function() {
                                console.log("Close button clicked");
                                $("#qr-buttons-container-overlay").removeClass('show');
                            });
                        // Append the Close button to the QR buttons container
                        $("#qr-buttons-container").append(closeButton);
                        $("#btn")
                        .removeClass('hiding')
                        .off("click") // Remove any previous click event to avoid duplicates
                        .on("click", function() {
                            console.log("Main button clicked to reveal individual QR code buttons");
                           $("#qr-buttons-container-overlay").addClass('show'); // Show the overlay
                            $("#qr-buttons-container").addClass('show'); // Scale up the container
                        });

                       // Close the modal when the user clicks outside the buttons container
                        $("#qr-buttons-container-overlay").on("click", function(event) {
                            if (event.target == this) {
                                $(this).removeClass('show');
                            }
                        });
   if (response.res.every(item => !item || !item['qrtext'] || item['qrtext'] == '')) {
                            $("#response-message").html("<font color=green>QR code will be emailed to you.</font>");
                            $("#response-message").removeClass("hiding");
                        }
                    }
                    else {
                        $("#response-message").html("<font color=green>No QR code found.</font>");
                        $("#response-message").removeClass("hiding");
                    }
                    break;
                case 202:
                        if (response.res['sellersku'].toLowerCase().includes('esim')) {
                                $("#response-message").html('<span class="text-green">QR code will be emailed to you.</span>');
                        } else {
                                $("#response-message").html('<span class="text-green">This order ID is valid, but it appears that the ordered item is not an eSIM.</span>');
                        }
                        $("#response-message").removeClass("hiding");
                        $("#btn").addClass('hiding');
                    break;
                case 404:
                    $("#response-message").html(response.msg);
                    $("#response-message").removeClass("hiding");
                    $("#btn").addClass('hiding');
                    break;
                default:
                    return;
            }
            }
        })
    }
}

function duplicateFields(totalEsims, esimPlans) {
  const imeiContainer = document.querySelector(".ind_imei");
  const eidContainer = document.querySelector(".ind_eid");

  // Create a wrapper container for the first group
  let originalGroupContainer = document.createElement("div");
  originalGroupContainer.classList.add("imei-group");
  originalGroupContainer.style.border = "1px solid #007bff";
  originalGroupContainer.style.padding = "10px";
  originalGroupContainer.style.marginBottom = "10px";
  originalGroupContainer.style.borderRadius = "5px";

  // Add a heading for the first plan
  let planHeading = document.createElement("h4");
  planHeading.style.color = "#007bff";
  //console.log("esism plans is ", esimPlans);
  planHeading.textContent = esimPlans[0] || "Plan 1"; // Default to "Plan 1" if no plan details are provided
  originalGroupContainer.appendChild(planHeading);

  // Move the original IMEI and EID containers into the new wrapper
  const parentContainer = imeiContainer.parentNode;
  parentContainer.insertBefore(originalGroupContainer, imeiContainer);
  originalGroupContainer.appendChild(imeiContainer);
  originalGroupContainer.appendChild(eidContainer);

  // Start creating duplicates for the remaining plans
  for (let i = 1; i < totalEsims; i++) {
    // Create a new container for each duplicate
    const groupContainer = document.createElement("div");
    groupContainer.classList.add("imei-group", "duplicated-group");
    groupContainer.style.border = "1px solid #007bff";
    groupContainer.style.padding = "10px";
    groupContainer.style.marginBottom = "10px";
    groupContainer.style.borderRadius = "5px";

    // Add a heading for the plan details
    const planHeading = document.createElement("h4");
    planHeading.style.color = "#007bff";
    planHeading.textContent = esimPlans[i] || `Plan ${i + 1}`; // Use plan details directly
    groupContainer.appendChild(planHeading);

    // Duplicate IMEI field
    const newImei = imeiContainer.cloneNode(true);
    newImei.classList.add("duplicated-imei");
    newImei.querySelector("input").id = `imei_ind_${i}`;
    newImei.querySelector("input").name = `imei_ind_${i}`;
    newImei.querySelector("input").placeholder = `IMEI Number ${i + 1}`;
    newImei.querySelector("input").value = ""; // Reset value
    newImei.querySelector("input").required = true; // Mark as required
    groupContainer.appendChild(newImei);

    // Duplicate EID field
    const newEid = eidContainer.cloneNode(true);
    newEid.classList.add("duplicated-eid");
    newEid.querySelector("input").id = `eid_ind_${i}`;
    newEid.querySelector("input").name = `eid_ind_${i}`;
    newEid.querySelector("input").placeholder = `EID Number ${i + 1}`;
    newEid.querySelector("input").value = ""; // Reset value
    newEid.querySelector("input").required = true; // Mark as required
    groupContainer.appendChild(newEid);

    // Insert the new group after the last group or original fields
    originalGroupContainer.insertAdjacentElement("afterend", groupContainer);
    originalGroupContainer = groupContainer; // Update reference to keep order
  }

  //console.log(`Fields initialized with ${totalEsims} total plans.`);
}

// Example Usage: Change the totalEsims value as needed

function debounce(func, timeout = 400) {
  let timer;
  return (...args) => {
    clearTimeout(timer);
    timer = setTimeout(() => {
      func(...args);
    }, timeout);
  };
}

const amazon_check = debounce((e) => amazon_check_process(e));
//pointer
function getQueryParams() {
  const params = {};
  const queryString = window.location.search.substring(1);
  const paramPairs = queryString.split("&");

  paramPairs.forEach((param) => {
    const [key, value] = param.split("=");
    params[key] = decodeURIComponent(value);
  });

  return params;
}

// Function to check query params and call the appropriate function
function checkAndCallFunction() {
  const queryParams = getQueryParams();

    if (queryParams.amazonorderid!=='' && typeof queryParams.amazonorderid!=='undefined') {
        $("#input-num").val(queryParams.amazonorderid);
        amazon_check_process(queryParams.amazonorderid);
	

	// Check if the frame visibility flag is set in sessionStorage
	if (sessionStorage.getItem('isFrameVisible') === 'true') {
	    document.getElementById("sim-card-selection-frame").style.display = "block";
	} else {
	    // Optionally hide the frame if the flag is not set
	    document.getElementById("sim-card-selection-frame").style.display = "none";
	}
    }
  
}

// Run the check when the page loads
window.onload = checkAndCallFunction;

// Function for handling case 200 logic
function handleCase200(response) {
  $("#name_status").html(response.dmsg);
  document.getElementById("count_query").value = response["count_query"];
  let countQuery1 = response.count_query; // Get the count
  document.getElementById("count_query").value = countQuery1;

  console.log("Executing case 200 logic");
  console.log("Response for case 200:", response);

  // Additional logic for case 200
  $("#fieldset2, #fieldset3").addClass("hiding");
  $("#fieldset4").removeClass("hiding");
  $("#personal_li").addClass("hiding");
  $("#documentation_li").addClass("hiding");
  $("#activation_li").removeClass("hiding");
  $("#file_upload_notice").addClass("hiding");
  $("#progressbar").css({
    display: "flex",
    "justify-content": "center",
  });
  document.getElementById("serialNo").value = response["simno"];
  $("#fnext_mobi").removeClass("hiding");
  check_activation = "mobi";
}

function HandleQrcodeRedirectedPage() {}

