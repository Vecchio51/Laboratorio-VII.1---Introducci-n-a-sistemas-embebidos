//CODIGO DE PRUEBA SIN SERVIDOR HTTP

#include "sensores.h"
#include "pantalla.h"

const int PIN_RELE = 26;

// Recordá invertir estos dos si tu módulo relé activa por HIGH
const int RELE_ENCENDIDO = LOW;
const int RELE_APAGADO   = HIGH;

bool motorEncendido = false;
bool interbloqueoOk = false;

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
  digitalWrite(PIN_RELE, RELE_APAGADO);

  iniciarSensores();
  iniciarPantalla();

  Serial.println("=== MODO TESTEO DE HARDWARE ===");
  Serial.println("Envia '1' por el Monitor Serie para encender el rele o '0' para apagarlo.");
}

void loop() {
  // 1. Verificar interbloqueo de seguridad continuamente
  interbloqueoOk = leerInterbloqueo();
  if (!interbloqueoOk && motorEncendido) {
    controlarMotor(false);
    Serial.println("[ALERTA] Iman retirado: rele apagado.");
  }

  // 2. Actualizar lectura de RPM y temperatura DS18B20
  actualizarSensores();

  // 3. Control manual desde el Monitor Serie (reemplaza temporalmente a la web)
  if (Serial.available() > 0) {
    char comando = Serial.read();
    if (comando == '1') {
      if (interbloqueoOk) {
        controlarMotor(true);
        Serial.println("[OK] Rele ENCENDIDO.");
      } else {
        Serial.println("[BLOQUEADO] Coloca el iman de interbloqueo primero.");
      }
    } else if (comando == '0') {
      controlarMotor(false);
      Serial.println("[OK] Rele APAGADO.");
    }
  }

  // 4. Mostrar valores en la pantalla OLED
  actualizarPantalla("MODO TEST", rpmActual, temperaturaActual, motorEncendido, interbloqueoOk);
}