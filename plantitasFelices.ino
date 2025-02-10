#include <Wire.h>
#include <BH1750.h>
#include <WiFi.h>
#include <WebServer.h>

// Configuración de la red WiFi
const char* ssid = "iPhone de Melina";
const char* password = "12345678";

// Inicializar el servidor web en el puerto 80
WebServer server(80);

// Pines
int sensorH = 34;  // Sensor de humedad
int relayBomba = 17;  // Relé de la bomba de agua
int relayLuz = 16;  // Relé de la lámpara

BH1750 lightMeter;
const int UMBRAL_LUZ = 50; // Umbral para encender la lámpara (lux)

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);  // SDA = GPIO21, SCL = GPIO22
    lightMeter.begin();
    Serial.println("BH1750 conectado.");
    
    pinMode(sensorH, INPUT);
    pinMode(relayBomba, OUTPUT);
    pinMode(relayLuz, OUTPUT);
    digitalWrite(relayBomba, LOW);
    digitalWrite(relayLuz, LOW);

    // Conectar a WiFi
    WiFi.begin(ssid, password);
    Serial.println("Conectando a WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(3000);
        Serial.print(".");
    }
    Serial.println("\nConectado a WiFi");
    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());

    // Configurar rutas del servidor
    server.on("/", handleRoot);
    server.on("/humedad", handleHumedad);
    server.on("/luz", handleLuz);
    server.begin();
    Serial.println("Servidor HTTP iniciado");
}

void loop() {
    server.handleClient();

    int humedad = analogRead(sensorH);
    if (humedad > 3000) {
        digitalWrite(relayBomba, HIGH);
        delay(3000);
        digitalWrite(relayBomba, LOW);
    }

    float lux = lightMeter.readLightLevel();
    Serial.print("Luz: ");
    Serial.print(lux);
    Serial.println(" lx");
    
    if (lux < UMBRAL_LUZ) {
        digitalWrite(relayLuz, HIGH);
    } else {
        digitalWrite(relayLuz, LOW);
    }
    
    delay(1000);
}

void handleRoot() {
  String message = "<html><head><title>Maceta Inteligente</title>";
  
  // Estilos CSS básicos
  message += "<style>";
  message += "body { font-family: Arial, sans-serif; background-color: #e0f7e9; color: #333; margin: 0; padding: 0; }";
  message += ".container { text-align: center; padding: 20px; background-color: #ffffff; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); max-width: 600px; margin: 20px auto; }";
  message += "h1 { color: #4CAF50; margin-bottom: 20px; }";
  message += "p { font-size: 18px; margin: 10px 0; }";
  message += "footer { text-align: center; font-size: 12px; color: #888; margin-top: 20px; }";
  message += "footer p { margin: 5px; }";
  message += "img { max-width: 200px; margin-top: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); }";
  message += "</style>";

  // Cuerpo de la página HTML
  message += "</head><body>";
  message += "<div class='container'>";
  message += "<h1>Monitor de Maceta 🌱</h1>";
  message += "<p>Humedad: <span id='humedad'>Cargando...</span>%</p>";
  message += "<p>Luz: <span id='luz'>Cargando...</span> lx</p>";
  message += "<img src='https://img.freepik.com/vector-premium/planta-feliz-dando-pulgar-simple-fondo-blanco-ilustracion-vectorial-kawaii_969863-300469.jpg' alt='Imagen de Planta'>";
  message += "</div>";
  
  message += "<footer>";
  message += "<p>Proyecto Final Electrónica - Diciembre 2025</p>";
  message += "<p>Sofía Ravenna y Melina Requena</p>";
  message += "</footer>";

  // Script para actualizar los datos cada 2 segundos
  message += "<script>";
  message += "function actualizarDatos() {";
  message += "fetch('/humedad').then(res => res.text()).then(data => document.getElementById('humedad').innerText = data);";
  message += "fetch('/luz').then(res => res.text()).then(data => document.getElementById('luz').innerText = data);";
  message += "}";
  message += "setInterval(actualizarDatos, 2000);";
  message += "actualizarDatos();";
  message += "</script>";
  
  message += "</body></html>";

  // Enviar la respuesta con el HTML
  server.send(200, "text/html", message);
}


void handleHumedad() {
    int humedad = analogRead(sensorH);
    float humedadPorcentaje = ((4095.0 - humedad) / (4095.0 - 1021.0)) * 100.0;
    server.send(200, "text/plain", String(humedadPorcentaje, 2));
}

void handleLuz() {
    float lux = lightMeter.readLightLevel();
    server.send(200, "text/plain", String(lux, 2));
}
