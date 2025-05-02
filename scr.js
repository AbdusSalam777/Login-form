
const apikey="99e684bd6ab3669ae3062c1cea5b9ece";

async function getWeather(city){

    const weatherData=`https://api.openweathermap.org/data/2.5/weather?q=${city}&appid=${apikey}`;


const data=await fetch(weatherData);
const jsonData=await data.json();
const temp=(jsonData.main.feels_like-273).toFixed(2);
const weatherId=jsonData.weather[0].id;

const show=document.querySelector("#display");
const emoji=getWeatherEmoji(weatherId);
show.innerHTML=`<div class="info">
        <p class="city">${city}</p><br>
        <p class="temp">${temp}°C</p><br>
        <p class="forecast">${jsonData.weather[0].description}</p><br>
        <p class="emoji">${emoji}</p></div>
        </div>`;
        
}

function displaydata(){
   const input= document.querySelector("#input").value;
   getWeather(input);
   
}

function getWeatherEmoji(weatherId) {
    if (weatherId >= 200 && weatherId < 300) return "⛈️"; // Thunderstorm
    if (weatherId >= 300 && weatherId < 500) return "🌧️"; // Drizzle
    if (weatherId >= 500 && weatherId < 600) return "🌧️"; // Rain
    if (weatherId >= 600 && weatherId < 700) return "❄️"; // Snow
    if (weatherId >= 700 && weatherId < 800) return "🌫️"; // Atmosphere
    if (weatherId === 800) return "☀️"; // Clear
    if (weatherId >= 801 && weatherId < 810) return "☁️"; // Clouds
    return "❓"; 
}
