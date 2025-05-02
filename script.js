let name_; //declaring variables
let email;
let username;
let password;
let firstName;
let lastName;
let confirmPassword;
let otp;

function sendOTP(callback) {
  //using calllback
  // Generate OTP
  otp = Math.floor(100000 + Math.random() * 900000);
  localStorage.setItem("otp", otp);

  // Get user input by DOM
  email = document.getElementById("input-email").value;
  username = document.getElementById("input-username").value;
  password = document.getElementById("input-password").value;
  firstName = document.getElementById("first-name").value;
  lastName = document.getElementById("last-name").value;
  confirmPassword = document.getElementById("confirm-password").value;
  name_ = firstName + " " + lastName;

  if (password !== confirmPassword) {
    document.getElementById("confirm-password").value = "Wrong Password";
    document.getElementById("confirm-password").style.color = "red";
  } else {
    const data = {
      // JavaScript Object
      name: name_,
      email: email,
      username: username,
      password: password,
    };

    // Save user data to localStorage
    localStorage.setItem("userData", JSON.stringify(data));
    console.log(email);

    // Initialize emailjs with public key
    emailjs.init("UBgttDNUV7h4AJf-_");

    // Send OTP via email using emailJS

    emailjs
      .send(
        "service_lmnxqrh" /*service Id */,
        "template_qm9270q" /*template Id */,
        {
          to_email: email,
          to_name: name_,
          otp: otp,
        }
      )
      .then(
        function (response) {
          //create button dynamically if otp sent successfully
          console.log("OTP sent successfully to " + email, response);

          // Create a 'Verify OTP' button dynamically
          let button = document.createElement("button");
          document.body.appendChild(button);
          button.innerHTML = "Verify OTP";
          button.style.height = "40px";
          button.style.width = "300px";
          button.style.display = "block";
          button.style.position = "absolute";
          button.style.marginTop = "661px";
          button.style.borderRadius = "5px";
          button.style.backgroundColor = "black";
          button.style.marginRight = "788px";
          button.style.color = "white";
          button.style.border = "none";
          button.style.fontFamily = "poppins";

          // Redirect to OTP verification page when button is clicked
          button.addEventListener("click", function () {
            window.location.href = "verify_otp.html";
          });
        },
        function (error) {
          console.log("Error sending OTP: ", error);
        }
      );
    callback();
  }
} //end of sendOTP function

// Function to handle OTP verification logic
function verifyOTP() {
  const enteredOTP = document.getElementById("input-otp").value;
  const storedOTP = localStorage.getItem("otp");

  if (enteredOTP == storedOTP) {
    alert("OTP Verified Successfully!");
    sendToMongo(); // Send user data to MongoDB
  } else {
    alert("Invalid OTP! Please try again.");
  }
}

// Function to send user data to MongoDB
function sendToMongo() {
  const storedData = localStorage.getItem("userData");

  if (!storedData) {
    alert("⚠️ No data found in localStorage");
    return;
  }

  const parsedData = JSON.parse(storedData); // Parse the data

  fetch("http://localhost:5000/save-data", {
    method: "POST", //post to send data
    headers: {
      "Content-Type": "application/json", //specify the content type
    },
    body: JSON.stringify(parsedData),
  })
    .then((res) => res.json())
    .then((response) => {
      alert(response.message); // ✅ or ❌ message
    })
    .catch((err) => {
      console.error("Error sending data:", err);
      alert("❌ Failed to send data");
    });
}

// Function to show data (for debugging purposes) not used in the original code
function show() {
  firstName = document.getElementById("first-name").value;
  lastName = document.getElementById("last-name").value;
  email = document.getElementById("input-email").value;
  username = document.getElementById("input-username").value;
  password = document.getElementById("input-password").value;
  name_ = firstName + " " + lastName;
  console.log(name_);
  console.log(email);
  console.log(username);
  console.log(password);
  console.log(otp);
}
