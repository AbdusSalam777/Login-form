function verifyOTP() {
  const otp_ = localStorage.getItem("otp");
  const otp_user = document.getElementById("input").value;
  if (otp_user === otp_) {
    const data = localStorage.getItem("userData");
    const userData = JSON.parse(data);
    console.log(userData);
    //code to send data to mongodb


    fetch("http://localhost:5000/save-data", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(userData)
    })

  }//end of if

  
  else {
    alert("Invalid OTP. Please try again.");
  }
}