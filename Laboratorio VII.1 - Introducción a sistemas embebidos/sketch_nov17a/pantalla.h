#ifndef PANTALLA_H
#define PANTALLA_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define DIRECCION_I2C 0x3C

Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);
unsigned long ultimaActualizacionOLED = 0;

void iniciarPantalla() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, DIRECCION_I2C)) {
    Serial.println("[ERROR] No se detectó la pantalla OLED");
    return;
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("Sistema Motor CA");
  display.setCursor(0, 28);
  display.println("Conectando WiFi...");
  display.display();
}

void actualizarPantalla(String ip, float rpm, float temp, bool motor, bool interlock) {
  unsigned long ahora = millis();
  if (ahora - ultimaActualizacionOLED < 400) return;
  ultimaActualizacionOLED = ahora;

  display.clearDisplay();
  display.setTextSize(1);

  // Fila 1: Dirección IP asignada
  display.setCursor(0, 0);
  display.print("IP: ");
  display.println(ip);
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  // Fila 2: Velocidad y Temperatura en tamaño destacado
  display.setCursor(0, 16);
  display.print("RPM : ");
  display.println((int)rpm);

  display.setCursor(0, 28);
  display.print("TEMP: ");
  display.print(temp, 1);
  display.println(" C");

  display.drawLine(0, 40, 128, 40, SSD1306_WHITE);

  // Fila 3: Estados de seguridad y accionamiento
  display.setCursor(0, 45);
  display.print("MOTOR: ");
  display.println(motor ? "EN MARCHA" : "DETENIDO");

  display.setCursor(0, 56);
  display.print("SEGUR: ");
  display.println(interlock ? "HABILITADO" : "SIN IMAN!");

  display.display();
}

#endif