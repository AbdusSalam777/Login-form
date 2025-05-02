 function send(){
    const name_=document.getElementById("input").value;
    
    fetch('http://localhost:5000/ronny',{
        method:'POST',
        headers:{
            'Content-Type':'application/json'
        },
        body:JSON.stringify({name:name_})
    }).then(res => res.json())
    .then(data => {
        document.getElementById("reply").innerText = data.message;
    });
}