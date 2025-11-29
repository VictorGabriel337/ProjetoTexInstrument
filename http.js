document.addEventListener("DOMContentLoaded", function() {
  const espIP = "172.20.10.3"; // IP do ESP8266

  async function updateData() {
    try {
      const response = await fetch(`http://${espIP}/data`, { cache: "no-store" });
      const text = await response.text();

      let data;
      try {
        data = JSON.parse(text);
      } catch (e) {
        console.error("Resposta /data não é JSON válido:", text);
        return;
      }

      // Atualiza valores do sensor
      const pressureDiv = document.getElementById("pressureValue");
      if (pressureDiv && data.smpPressure !== undefined) {
        pressureDiv.innerText = Number(data.smpPressure).toFixed(2) + " kPa";
      }

      const tempDiv = document.getElementById("Tempvalue");
      if (tempDiv && data.smpTemperature !== undefined) {
        tempDiv.innerText = Number(data.smpTemperature).toFixed(2) + " °C";
      }

      // Atualiza o velocímetro sempre com a velocidade atual do ESP
      if (data.speed !== undefined && typeof window.setSpeed === "function") {
        window.setSpeed(data.speed);
      }

    } catch (err) {
      console.error("Erro ao buscar dados:", err);
    }
  }

  // Atualiza a cada 300ms para pegar mudanças do botão físico rapidamente
  setInterval(updateData, 300);
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
    if (timeDiv) timeDiv.innerText = timeString;
  }

  updateTime();
  setInterval(updateTime, 1000);
});
