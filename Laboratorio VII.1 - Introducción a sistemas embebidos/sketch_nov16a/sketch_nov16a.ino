//URL DEL REPOSITORIO EN GIT: https://github.com/Vecchio51/Laboratorio-VII.1---Introducci-n-a-sistemas-embebidos
//INTEGRANTES: WALTER ARIEL PAZ ESTRADA - FRANCISCO VECCHIONE
//LABORATORIO VII.1 - Introducción a sistemas embebidos

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "html.h"
#include "sensores.h"
#include "pantalla.h"

const char* DeviceName = "motor-ca"; // Acceso vía http://motor-ca.local
const char* ssid       = "Tu_WiFi";
const char* password   = "Tu_Password";

const int PIN_RELE = 26;

// La mayoría de los módulos relé comerciales se activan con LOW.
// Si el tuyo activa con HIGH, invertí estos dos valores.
const int RELE_ENCENDIDO = LOW;
const int RELE_APAGADO   = HIGH;

bool motorEncendido = false;
bool interbloqueoOk = false;

WebServer server(80);

void controlarMotor(bool encender) {
  if (encender && interbloqueoOk) {
    motorEncendido = true;
    digitalWrite(PIN_RELE, RELE_ENCENDIDO);
  } else {
    motorEncendido = false;
    digitalWrite(PIN_RELE, RELE_APAGADO);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_RELE, OUTPUT);
  digitalWrite(PIN_RELE, RELE_APAGADO); // Seguridad: arranca apagado

  iniciarSensores();
  iniciarPantalla();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
  }
  Serial.print("[OK] IP asignada: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(DeviceName)) {
    Serial.printf("[OK] Servidor web en: http://%s.local\n", DeviceName);
  }

  // Rutas del servidor HTTP
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", html_page);
  });

  server.on("/encender", HTTP_POST, []() {
    controlarMotor(true);
    server.send(200, "application/json", "{\"ok\":true}");
  });

  server.on("/apagar", HTTP_POST, []() {
    controlarMotor(false);
    server.send(200, "application/json", "{\"ok\":true}");
  });

  server.on("/datos", HTTP_GET, []() {
    String json = "{";
    json += "\"motor\":" + String(motorEncendido ? "true" : "false") + ",";
    json += "\"interbloqueo\":" + String(interbloqueoOk ? "true" : "false") + ",";
    json += "\"rpm\":" + String(rpmActual, 1) + ",";
    json += "\"temp\":" + String(temperaturaActual, 1);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  // 1. Verificación prioritaria del interbloqueo de seguridad
  interbloqueoOk = leerInterbloqueo();
  if (!interbloqueoOk && motorEncendido) {
    controlarMotor(false);
    Serial.println("[ALERTA] Imán de interbloqueo retirado: motor detenido.");
  }

  // 2. Actualizar lectura de RPM y DS18B20
  actualizarSensores();

  // 3. Atender peticiones de la página web
  server.handleClient();
  actualizarPantalla(WiFi.localIP().toString(), rpmActual, temperaturaActual, motorEncendido, interbloqueoOk);
}