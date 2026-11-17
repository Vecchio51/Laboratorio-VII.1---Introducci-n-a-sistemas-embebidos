#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Definición de pines de sensores
const int PIN_DS18B20     = 4;
const int PIN_HALL_RPM    = 27;
const int PIN_INTERLOCK   = 25;

// Ajustar a 1 si tenés 1 imán (unipolar) o 2 polos alternados (bipolar) por vuelta
const int PULSOS_POR_VUELTA = 1;

OneWire oneWire(PIN_DS18B20);
DallasTemperature sensorTemp(&oneWire);

volatile unsigned long contadorPulsos = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

float temperaturaActual = 0.0;
float rpmActual = 0.0;
unsigned long ultimoCalculoMs = 0;
const unsigned long INTERVALO_MUESTREO = 1000; // 1 segundo

// Rutina de interrupción en RAM para contar pulsos sin perder ninguno
void IRAM_ATTR isrContarPulso() {
  portENTER_CRITICAL_ISR(&mux);
  contadorPulsos++;
  portEXIT_CRITICAL_ISR(&mux);
}

void iniciarSensores() {
  pinMode(PIN_HALL_RPM, INPUT_PULLUP);
  pinMode(PIN_INTERLOCK, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_HALL_RPM), isrContarPulso, FALLING);

  sensorTemp.begin();
  // Modo no bloqueante: evita congelar el servidor HTTP los 750ms que tarda la conversión
  sensorTemp.setWaitForConversion(false);
  sensorTemp.requestTemperatures();
}

// Devuelve true si el imán de seguridad está colocado
bool leerInterbloqueo() {
  return (digitalRead(PIN_INTERLOCK) == LOW);
}

// Actualiza RPM y Temperatura cada 1 segundo sin usar delay()
void actualizarSensores() {
  unsigned long ahora = millis();
  if (ahora - ultimoCalculoMs >= INTERVALO_MUESTREO) {
    ultimoCalculoMs = ahora;

    // 1. Cálculo de RPM
    portENTER_CRITICAL(&mux);
    unsigned long pulsos = contadorPulsos;
    contadorPulsos = 0;
    portEXIT_CRITICAL(&mux);

    float vueltas = (float)pulsos / (float)PULSOS_POR_VUELTA;
    rpmActual = vueltas * 60.0;

    // 2. Lectura de Temperatura DS18B20
    float tempLeida = sensorTemp.getTempCByIndex(0);
    if (tempLeida != DEVICE_DISCONNECTED_C && tempLeida > -55.0) {
      temperaturaActual = tempLeida;
    }
    // Solicita la próxima conversión para el siguiente ciclo
    sensorTemp.requestTemperatures();
  }
}

#endif