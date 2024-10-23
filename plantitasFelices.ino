#include <WiFi.h>
#include <WebServer.h>

// Configuración de la red WiFi
const char* ssid = "iPhone de Melina";
const char* password = "12345678";

// Inicializar el servidor web en el puerto 80
WebServer server(80);

// Pines
int sensorH = 34;
int relay = 17;

void setup() {
  Serial.begin(115200);
  pinMode(sensorH, INPUT);       // Configurar el pin del sensor como entrada
  pinMode(relay, OUTPUT);        // Configurar el pin del relé como salida
  digitalWrite(relay, LOW);      // Inicializar el relé en estado apagado (bajo)

  // Conectar a la red WiFi
  WiFi.begin(ssid, password);
  Serial.println("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado a WiFi");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configurar rutas del servidor
  server.on("/", handleRoot);
  server.on("/humedad", handleHumedad);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  // Manejar solicitudes del cliente
  server.handleClient();

  // Leer el valor de humedad
  int humedad = analogRead(sensorH);

  // Controlar la bomba sin usar delay
  if (humedad > 3000) {
    digitalWrite(relay, HIGH);
    delay(3000);
    digitalWrite(relay, LOW);
  }
}

// Manejar la ruta raíz
void handleRoot() {
  String message = "<html><head>";
  message += "<title>Humedad de la Planta</title>";
  message += "<style>";
  message += "body { font-family: Arial, sans-serif; background-color: #ccffcc; margin: 0; padding: 0; }";
  message += ".container { max-width: 600px; margin: 50px auto; padding: 20px; border: 1px solid #00796b; border-radius: 10px; background-color: #ffffff; box-shadow: 0 0 10px rgba(0,0,0,0.1); text-align: center; }";
  message += "h1 { color: #004d40; }";
  message += ".data { font-size: 1.5em; margin: 10px 0; color: #00796b; }";
  message += ".footer { margin-top: 20px; font-size: 0.8em; color: #004d40; text-align: center; }";
  message += "img { max-width: 100%; height: auto; border-radius: 10px; margin-top: 20px; width: 200px; height: 200px; }"; // Ajusta la imagen a un tamaño específico
  message += "</style>";
  message += "</head><body>";
  message += "<div class='container'>";
  message += "<h1>Monitor de Humedad de la Planta 🌱</h1>";
  message += "<p class='data'>Humedad Actual: <span id='humedad'>Cargando...</span>%</p>";
  message += "<img src='https://img.freepik.com/vector-premium/planta-feliz-dando-pulgar-simple-fondo-blanco-ilustracion-vectorial-kawaii_969863-300469.jpg' alt='Imagen de Planta'>"; // URL de la imagen
  message += "<script>";
  message += "function obtenerHumedad() {";
  message += "  var xhr = new XMLHttpRequest();";
  message += "  xhr.onreadystatechange = function() {";
  message += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  message += "      document.getElementById('humedad').innerHTML = xhr.responseText;";
  message += "    }";
  message += "  };";
  message += "  xhr.open('GET', '/humedad', true);";
  message += "  xhr.send();";
  message += "}";
  message += "setInterval(obtenerHumedad, 2000);"; // Actualizar cada 2 segundos
  message += "</script>";
  message += "<div class='footer'>Proyecto Final Electronica I - Sofia Ravenna y Melina Requena</div>";
  message += "</div>";
  message += "</body></html>";
  server.send(200, "text/html", message);
}

// Manejar la ruta /humedad
void handleHumedad() {
  int humedad = analogRead(sensorH);
  float humedadPorcentaje = ((4095.0 - humedad) / (4095.0 - 1021.0)) * 100.0;
  server.send(200, "text/plain", String(humedadPorcentaje, 2));
}
