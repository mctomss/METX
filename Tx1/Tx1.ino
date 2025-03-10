/*
Escuela Colombiana de Ingenieria Julio Garavito
Luis Francisco Leal Baute
Edgar Daniel Ruiz Patiño
Tomás Felipe Montañez Piñeros

Descripción: Codigo para la ESP32 Transmisora.
*/

// Librerías necesarias
#include <DHT.h>
#include <DHT_U.h>
#include <LoRa.h>
#include <SPI.h>
#include "HX711.h"
#include <driver/i2s.h>

// Definición de pines y constantes
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 15;
#define DHTPIN 4 
#define DHTPIN2 21
#define DHTTYPE DHT22 
#define DHTTYPE2 DHT11
#define ss 5
#define rst 14
#define dio0 2
#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32
#define I2S_PORT I2S_NUM_0

// Parámetros para el buffer y el rango de abejas
#define bufferLen 64
int16_t sBuffer[bufferLen];
#define MIN_BEES 0
#define MAX_BEES 1000

// Variables globales
int contador = 0;
DHT dht1(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE2);
HX711 scale;
unsigned long lastTime = 0;
const unsigned long interval = 1000; // Intervalo en milisegundos

// Función para instalar el driver I2S
void i2s_install() {
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 44100,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S),
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = bufferLen,
        .use_apll = false
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

// Función para configurar los pines I2S
void i2s_setpin() {
    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD
    };

    i2s_set_pin(I2S_PORT, &pin_config);
}

// Configuración de LoRa
void setupLoRa() {
    Serial.println("Configurando LoRa...");
    LoRa.setPins(ss, rst, dio0);

    while (!LoRa.begin(433E6)) { // Frecuencia según región
        Serial.println("Intentando conectar a LoRa...");
        delay(500);
    }
    LoRa.setSyncWord(0xA5);
    Serial.println("LoRa configurado correctamente!");
}

// Función para leer los sensores
void LeerSensores() {
    delay(2000); // Espera para el sensor DHT
    float h1 = dht1.readHumidity();
    float t1 = dht1.readTemperature();
    float h2 = dht2.readHumidity();
    float t2 = dht2.readTemperature();

    if (isnan(h1) || isnan(t1)) {
        Serial.println("Error leyendo del sensor DHT22");
        return;
    }
    if (isnan(h2) || isnan(t2)) {
        Serial.println("Error leyendo del sensor DHT11");
        return;
    }

    // Leer peso de la báscula
    float weight = scale.get_units(10);

    // Procesar datos del micrófono
    float voltageRMS = 0;
    int estimatedBees = 0;
    float mayor_dato = 0; // Almacena el dato máximo absoluto

    size_t bytesIn = 0;
    esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen * sizeof(int16_t), &bytesIn, portMAX_DELAY);
    if (result == ESP_OK) {
        int16_t samples_read = bytesIn / sizeof(int16_t);
        if (samples_read > 0) {
            for (int16_t i = 0; i < samples_read; ++i) {
                float sample = abs(sBuffer[i]); // Trabajamos con valores absolutos
                if (sample > mayor_dato) {
                    mayor_dato = sample; // Actualizar el dato máximo
                }
            }

            // Calcular voltaje RMS utilizando el dato máximo
            voltageRMS = (mayor_dato / 32767.0) * 3.3;

            // Mapear el voltaje al rango de abejas
            estimatedBees = map(voltageRMS * 1000, 50, 3300, MIN_BEES, MAX_BEES);
            if (estimatedBees < MIN_BEES) estimatedBees = MIN_BEES;
            if (estimatedBees > MAX_BEES) estimatedBees = MAX_BEES;
        }
    }

    // Enviar datos
    enviarDatos(h1, t1, weight, voltageRMS, estimatedBees, h2, t2);
}

// Función para enviar datos a través de LoRa
void enviarDatos(float h1, float t1, float w, float vRMS, int bees, float h2, float t2) {
    char sensores[100]; // Asegurar suficiente espacio
    snprintf(sensores, sizeof(sensores), "%.2f/%.2f/%.2f/%.3f/%d/%d/%f/%f", h1, t1, w, vRMS, bees, contador, h2, t2);

    LoRa.beginPacket();
    LoRa.print(sensores);
    LoRa.endPacket();
    Serial.println(sensores);
    contador++;
}

// Función de configuración inicial
void setup() {
    Serial.begin(115200);
    Serial.println("Iniciando sistema...");
    dht1.begin();
    dht2.begin();
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(421);
    scale.tare();
    setupLoRa();
    i2s_install();
    i2s_setpin();
    i2s_start(I2S_PORT);
}

// Bucle principal
void loop() {
    if (millis() - lastTime >= interval) {
        lastTime = millis();
        LeerSensores();
    }
}

