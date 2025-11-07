#include <Wire.h>
#include <BMP280.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SMP3011_ADDR 0x78
BMP280 bmp;

// --- Rede WiFi ---
const char* ssid = "Aimee1306";
const char* password = "@fernandacampos1306";

ESP8266WebServer server(80);


float smpPressure = 0;
float smpTemperature = 0;


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

  while (millis() - start < 500 && !ready) {  // timeout maior
    Wire.requestFrom(SMP3011_ADDR, 6);
    int i = 0;
    while (Wire.available() && i < 6) buffer[i++] = Wire.read();

    // Debug buffer
    // Serial.print("Buffer SMP3011: ");
    // for (int j = 0; j < 6; j++) {
    //   Serial.print(buffer[j], HEX); Serial.print(" ");
    // }
    // Serial.println();

    if ((buffer[0] & 0x20) == 0) ready = true; // bit5 == 0 -> pronto
    else delay(5);
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

    Serial.print("SMP3011 - Pressão: "); Serial.print(smpPressure); Serial.print(" kPa, ");
    Serial.print("Temperatura: "); Serial.println(smpTemperature);
  } else {
    Serial.println("SMP3011 não está pronto ou timeout!");
  }
}


void handleData() {
  readSMP3011();
  float bmpTemp = bmp.getTemperature();
  float bmpPressure = bmp.getPressure() / 100.0;

  String json = "{";
  json += "\"smpPressure\": " + String(smpPressure) + ",";
  json += "\"smpTemperature\": " + String(smpTemperature) + ",";
  json += "\"bmpPressure\": " + String(bmpPressure) + ",";
  json += "\"bmpTemperature\": " + String(bmpTemp);
  json += "}";

  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS
  server.send(200, "application/json", json);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1); 

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

  // Servidor HTTP
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor HTTP iniciado!");
}

// --- Loop ---
void loop() {
  server.handleClient(); 

  
  readSMP3011();
  delay(1000);
}