#include <Wire.h>
#include <BMP280.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SMP3011_ADDR 0x78
BMP280 bmp;

int const acelerar = D7;
int const desacelerar = D6;

int velocidade = 0;

// --- Rede WiFi ---
const char* ssid = "Aimee1306";
const char* password = "@fernandacampos1306";

ESP8266WebServer server(80);

float smpPressure = 0;
float smpTemperature = 0;

// --- Temporizadores ---
unsigned long lastSMP = 0;
const unsigned long intervalSMP = 200; // leitura SMP3011 a cada 200ms

unsigned long lastAcelerar = 0;
unsigned long lastDesacelerar = 0;
const unsigned long debounceDelay = 200;

// --- Funções ---
void readSMP3011() {
  uint8_t command = 0xAC;
  Wire.beginTransmission(SMP3011_ADDR);
  Wire.write(command);
  if (Wire.endTransmission() != 0) {
    Serial.println("SMP3011 não responde no I2C!");
    return;
  }

  uint8_t buffer[6];
  unsigned long start = millis();
  bool ready = false;

  while (millis() - start < 100 && !ready) {  // timeout reduzido
    Wire.requestFrom(SMP3011_ADDR, 6);
    int i = 0;
    while (Wire.available() && i < 6) buffer[i++] = Wire.read();

    if ((buffer[0] & 0x20) == 0) ready = true; // bit5 == 0 -> pronto
    else delay(2);
  }

  if (ready) {
    uint32_t rawPress = ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) | buffer[3];
    float pressurePercentage = rawPress / 16777215.0f;
    pressurePercentage -= 0.15f;
    pressurePercentage /= 0.7f;
    pressurePercentage *= 500000.0f;
    smpPressure = pressurePercentage / 1000.0f; // kPa

    uint16_t rawTemp = ((uint16_t)buffer[4] << 8) | buffer[5];
    float temperaturePercentage = rawTemp / 65535.0f;
    smpTemperature = ((150.0 - (-40.0)) * temperaturePercentage) - 40.0;

    // Serial para debug
    Serial.print("SMP3011 - Pressão: "); Serial.print(smpPressure); 
    Serial.print(" kPa, Temperatura: "); Serial.println(smpTemperature);
  }
}

void handleData() {
  float bmpTemp = bmp.getTemperature();
  float bmpPressure = bmp.getPressure() / 100.0;

  String json = "{";
  json += "\"smpPressure\": " + String(smpPressure) + ",";
  json += "\"smpTemperature\": " + String(smpTemperature) + ",";
  json += "\"bmpPressure\": " + String(bmpPressure) + ",";
  json += "\"bmpTemperature\": " + String(bmpTemp) + ",";
  json += "\"speed\": " + String(velocidade);
  json += "}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleAcelerar() {
  velocidade += 10;
  if (velocidade > 180) velocidade = 180;
  Serial.print("Acelerando... velocidade = "); Serial.println(velocidade);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Acelerando");
}

void handleDesacelerar() {
  velocidade -= 10;
  if (velocidade < 0) velocidade = 0;
  Serial.print("Desacelerando... velocidade = "); Serial.println(velocidade);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Desacelerando");
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D0); 

  pinMode(acelerar, INPUT_PULLUP);
  pinMode(desacelerar, INPUT_PULLUP);

  // Inicializa BMP280
  if (!bmp.begin()) {
    Serial.println("Erro ao inicializar BMP280!");
    while (1);
  }

  // Conecta WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP do ESP8266: ");
  Serial.println(WiFi.localIP());

  server.on("/acelerar", handleAcelerar);
  server.on("/desacelerar", handleDesacelerar);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor HTTP iniciado!");
}

// --- Loop otimizado ---
void loop() {
  server.handleClient();

  unsigned long now = millis();

  // Leitura SMP3011 a cada intervalSMP ms
  if (now - lastSMP >= intervalSMP) {
    readSMP3011();
    lastSMP = now;
  }

  // Botão acelerar
  bool estadoAcelerar = digitalRead(acelerar) == LOW;
  if (estadoAcelerar && now - lastAcelerar > debounceDelay) {
    velocidade += 10;
    if (velocidade > 180) velocidade = 180;
    Serial.print("Acelerando botão físico: "); Serial.println(velocidade);
    lastAcelerar = now;
  }

  // Botão desacelerar
  bool estadoDesacelerar = digitalRead(desacelerar) == LOW;
  if (estadoDesacelerar && now - lastDesacelerar > debounceDelay) {
    velocidade -= 10;
    if (velocidade < 0) velocidade = 0;
    Serial.print("Desacelerando botão físico: "); Serial.println(velocidade);
    lastDesacelerar = now;
  }
}
