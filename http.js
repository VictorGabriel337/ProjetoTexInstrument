document.addEventListener("DOMContentLoaded", function() {
  const espIP = "192.168.15.5"; // IP do ESP8266

  async function updateData() {
    try {
      const response = await fetch(`http://${espIP}/data`);
      const data = await response.json();

     
      const pressureDiv = document.getElementById("pressureValue");
      pressureDiv.innerText = data.smpPressure.toFixed(2) + " kPa";

    
      const tempDiv = document.getElementById("Tempvalue");
      tempDiv.innerText = data.smpTemperature.toFixed(2) + " °C";

    } catch (err) {
      console.error("Erro ao buscar dados:", err);
    }
  }

  
  setInterval(updateData, 1000);
  updateData(); 
});


document.addEventListener("DOMContentLoaded", function() {
  const timeDiv = document.querySelector(".time");

  function updateTime() {
    const now = new Date();
    let hours = now.getHours();
    let minutes = now.getMinutes();
    let ampm = hours >= 12 ? "PM" : "AM";

    
    hours = hours % 12;
    hours = hours ? hours : 12; 

   
    minutes = minutes < 10 ? "0" + minutes : minutes;

    const timeString = `${hours}:${minutes} ${ampm}`;
    timeDiv.innerText = timeString;
  }

  updateTime(); 
  setInterval(updateTime, 1000); 
});

