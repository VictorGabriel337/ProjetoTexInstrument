const espIP = "192.168.15.8"; // mesmo IP do ESP

var speed = 0;
var prevSpeed = 0;
var currentScale = 1;



function setSpeed(newSpeed) {
  // converte e limita
  newSpeed = Math.max(0, Math.min(180, Number(newSpeed)));

  prevSpeed = speed;
  speed = newSpeed;

  addClass();
  changeActive();
  changeText();
}

function increaseSpeed() {
console.log("Botão Acelerar clicado");
  if (speed < 180) {
    prevSpeed = speed;
    speed += 10;
    addClass();
    currentScale++;
    changeActive();
    changeText();

    // 🚀 Envia comando real para o ESP
    fetch(`http://${espIP}/acelerar`)
      .then(res => res.text())
      .then(data => console.log("ESP respondeu:", data))
      .catch(err => console.error("Erro ao acelerar:", err));
  }
}

function decreaseSpeed() {
console.log("Botão Desacelerar clicado");
  if (speed > 0) {
    prevSpeed = speed;
    speed -= 10;
    addClass();
    currentScale--;
    changeActive();
    changeText();

    // 🚀 Envia comando real para o ESP
    fetch(`http://${espIP}/desacelerar`)
      .then(res => res.text())
      .then(data => console.log("ESP respondeu:", data))
      .catch(err => console.error("Erro ao desacelerar:", err));
      
  }
}

function addClass() {
  let newClass = "speed-" + speed;
  let prevClass = "speed-" + prevSpeed;
  let el = document.getElementsByClassName("arrow-wrapper")[0];
  if (el) {
    if (el.classList.contains(prevClass)) {
      el.classList.remove(prevClass);
    }
    el.classList.add(newClass);
  }
}

function changeActive() {
  let allScales = document.querySelectorAll(".speedometer-scale");
  let scaleIndex = speed / 10;

  if (speed === 0) {
    allScales.forEach(el => el.classList.remove("active"));
  } else {
    allScales.forEach((el, idx) => {
      if (idx < scaleIndex + 1 && speed > 0) {
        el.classList.add("active");
      } else {
        el.classList.remove("active");
      }
    });
  }
}

function changeText() {
  let el = document.getElementsByClassName("km")[0];
  el.innerText = speed;
}
