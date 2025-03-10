/*
Escuela Colombiana de Ingenieria Julio Garavito
Luis Francisco Leal Baute
Edgar Daniel Ruiz Patiño
Tomás Felipe Montañez Piñeros

Descripción: Codigo para la ESP32 Receptora.
*/

#include <LoRa.h>
#include <SPI.h>
#include <WiFi.h>
#include <Arduino.h>
#include <ThingerESP32.h>
#include <driver/i2s.h>
#include <math.h>

#define THINGER_SERIAL_DEBUG
#define RXD2 16
#define TXD2 17
#define USERNAME "McTomSs"
#define DEVICE_ID "ESP32_METX"
#define DEVICE_CREDENTIAL "tqmSLAKcR+9kq5vl"
#define SSID "moto g84 5G_6199"
#define SSID_PASSWORD "tomasito2003"

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

#define ss 5
#define rst 14
#define dio0 2

// Variables globales
float h1, t1, h2, t2, w, v, db;
int c, contador, prx;
float voltage_ref = 1.0; // Referencia de 1V

// Función para imprimir el estado del WiFi
void printWiFiStatus() {
  switch (WiFi.status()) {
    case WL_IDLE_STATUS:
      Serial.println("WiFi está inactivo, esperando conexión.");
      break;
    case WL_NO_SSID_AVAIL:
      Serial.println("SSID no disponible. Verifica el nombre de la red.");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("Error de conexión. Verifica la contraseña.");
      break;
    case WL_DISCONNECTED:
      Serial.println("WiFi desconectado. Intentando reconectar...");
      break;
    case WL_CONNECTED:
      Serial.println("WiFi conectado correctamente.");
      Serial.print("Dirección IP: ");
      Serial.println(WiFi.localIP());
      break;
    default:
      Serial.println("Estado desconocido.");
  }
}

// Función para iniciar la conexión WiFi y configurar Thinger.io
void Iniciar() {
  pinMode(2, OUTPUT);

  Serial.println("Conectando a WiFi...");
  WiFi.begin(SSID, SSID_PASSWORD);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 10000;  // Tiempo límite: 10 segundos

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConexión WiFi establecida!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nError: No se pudo conectar al WiFi.");
    printWiFiStatus();
  }

  // Configuración de Thinger.io
  thing.add_wifi(SSID, SSID_PASSWORD);
  thing["GPIO_2"] << digitalPin(2);
}

// Función para configurar el módulo LoRa
void setupLoRa() {
  Serial.println("Configurando LoRa...");
  LoRa.setPins(ss, rst, dio0);  // Configurar módulo LoRa

  while (!LoRa.begin(433E6)) {  // Configuración según la región
    Serial.println("Intentando configurar LoRa...");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa configurado correctamente!");
}

// Función de configuración inicial
void setup() {
  Serial.begin(115200);  // Velocidad de comunicación
  Iniciar();
  setupLoRa();

  // Configuración de recursos en Thinger.io
  thing["Humedad i"] >> [](pson &out) {
    out = h1;
    Serial.println("Se envió humedad i: " + String(h1));
  };
  thing["Temperatura i"] >> [](pson &out) {
    out = t1;
    Serial.println("Se envió temperatura i: " + String(t1));
  };
  thing["Humedad e"] >> [](pson &out) {
    out = h2;
    Serial.println("Se envió humedad e: " + String(h2));
  };
  thing["Temperatura e"] >> [](pson &out) {
    out = t2;
    Serial.println("Se envió temperatura e: " + String(t2));
  };
  thing["Peso"] >> [](pson &out) {
    out = w;
    Serial.println("Se envió peso: " + String(w));
  };
  thing["Potencia_Sonido"] >> [](pson &out) {
    out = db;
    Serial.println("Se envió potencia: " + String(db));
  };
  thing["Cantidad"] >> [](pson &out) {
    out = c;
    Serial.println("Se envió cantidad: " + String(c));
  };
  thing["RRSI"] >> [](pson &out) {
    out = prx;
    Serial.println("Se envió cantidad: " + String(prx));
  };
  thing["Datos"] >> [](pson &out) {
    out = contador;
    Serial.println("Se envió Total Datos Enviados: " + String(contador));
  };

  // Variables para la interfaz de usuario
  // Porcentaje de miel
  thing["miel_porcentaje"] >> [](pson &out) {
    out = (w / 4.0) * 100;  // Convertir de 0-4 kg a 0-100%
    Serial.println("Se envió miel_porcentaje: " + String((w / 4.0) * 100) + "%");
  };

  // Estado de la temperatura
  thing["estado_temperatura"] >> [](pson &out) {
    if (t1 < 10) {
      out = "Muy Baja";
    } else if (t1 >= 10 && t1 <= 25) {
      out = "Baja";
    } else if (t1 >= 26 && t1 <= 33) {
      out = "Regular";
    } else if (t1 >= 34 && t1 <= 36) {
      out = "Ideal";
    } else if (t1 >= 37 && t1 <= 40) {
      out = "Alta";
    } else {
      out = "Muy Alta";
    }
    Serial.println("Se envió estado_temperatura: " + String(out));
  };

  // Estado de la humedad
  thing["estado_humedad"] >> [](pson &out) {
    if (h1 < 30) {
      out = "Muy Baja";
    } else if (h1 >= 30 && h1 <= 50) {
      out = "Baja";
    } else if (h1 >= 51 && h1 <= 60) {
      out = "Regular";
    } else if (h1 >= 61 && h1 <= 70) {
      out = "Ideal";
    } else if (h1 >= 71 && h1 <= 80) {
      out = "Alta";
    } else {
      out = "Muy Alta";
    }
    Serial.println("Se envió estado_humedad: " + String(out));
  };

  // Enviar todos los datos a Thinger.io
  thing["DATA_METX"] >> [](pson &out) {
    out["Humedad_i"] = h1;
    out["Humedad_e"] = h2;
    out["Temperatura_i"] = t1;
    out["Temperatura_e"] = t2;
    out["Peso"] = w;
    out["Voltaje"] = v;
    out["Potencia_Sonido"] = db;
    out["Cantidad"] = c;
    out["RRSI"] = prx;
    out["Datos"] = contador;
  };
}

// Bucle principal
void loop() {
  // Lógica de Thinger.io
  thing.handle();

  // Recepción de datos LoRa
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("Información recibida:");
    String DatosRecibidos = "";
    while (LoRa.available()) {
      DatosRecibidos += (char)LoRa.read();
    }

    sscanf(DatosRecibidos.c_str(), "%f/%f/%f/%f/%d/%d/%f/%f", &h1, &t1, &w, &v, &c, &contador, &h2, &t2);
    if (w < 0) {
      w = 0;
    }
    db = 20 * log10(v / voltage_ref);
    prx = LoRa.packetRssi();
    // Mostrar datos recibidos
    Serial.println("Humedad i: " + String(h1));
    Serial.println("Humedad e: " + String(h2));
    Serial.println("Temperatura i: " + String(t1));
    Serial.println("Temperatura e: " + String(t2));
    Serial.println("Peso: " + String(w));
    Serial.println("Voltaje: " + String(v));
    Serial.println("Abejas: " + String(c));
    Serial.println("Contador: " + String(contador));
    Serial.print("La potencia de recepción es ");
    Serial.print(LoRa.packetRssi());
    Serial.println(" dB.");
  }

  // Mostrar estado WiFi cada 10 segundos
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck >= 10000) {
    lastCheck = millis();
    printWiFiStatus();
  }
}