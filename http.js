document.addEventListener("DOMContentLoaded", function() {
  const espIP = "192.168.15.7"; // IP do ESP8266

  async function updateData() {
    try {
      const response = await fetch(`http://${espIP}/data`);
      const data = await response.json();

      // Atualiza a div de pressão
      const pressureDiv = document.getElementById("pressureValue");
      pressureDiv.innerText = data.smpPressure.toFixed(2) + " kPa";

      // Atualiza a div de temperatura (id correto: Tempvalue)
      const tempDiv = document.getElementById("Tempvalue");
      tempDiv.innerText = data.smpTemperature.toFixed(2) + " °C";

    } catch (err) {
      console.error("Erro ao buscar dados:", err);
    }
  }

  // Atualiza a cada 1 segundo
  setInterval(updateData, 1000);
  updateData(); // chamada imediata
});


document.addEventListener("DOMContentLoaded", function() {
  const timeDiv = document.querySelector(".time");

  function updateTime() {
    const now = new Date();
    let hours = now.getHours();
    let minutes = now.getMinutes();
    let ampm = hours >= 12 ? "PM" : "AM";

    // Converte hora para formato 12h
    hours = hours % 12;
    hours = hours ? hours : 12; // Se for 0, coloca 12

    // Adiciona zero à esquerda nos minutos
    minutes = minutes < 10 ? "0" + minutes : minutes;

    const timeString = `${hours}:${minutes} ${ampm}`;
    timeDiv.innerText = timeString;
  }

  updateTime(); // chamada imediata
  setInterval(updateTime, 1000); // atualiza a cada segundo
});

