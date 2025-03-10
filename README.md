# METX
Implementación de un Sistema de Apicultura de Precisión con Monitoreo Remoto Empleando Modulos LoRa. 
# Apicultura de Precisión con Monitoreo Remoto usando ESP32

## Descripción
Este proyecto implementa un sistema de monitoreo en tiempo real para apicultores, permitiendo la visualización de variables clave en la producción de miel mediante el uso de sensores y comunicación LoRa. El sistema garantiza un monitoreo eficiente de las colmenas, optimizando la producción y bienestar de las abejas.

## Características
- Monitoreo de temperatura y humedad interna y externa con sensores **DHT22** y **DHT11**.
- Medición del peso de los panales con una **galga extensiométrica** y módulo **HX711**.
- Análisis del sonido ambiental para estimar la cantidad de abejas con **INMP441**.
- Comunicación inalámbrica de datos mediante **módulos LoRa**.
- Interfaz gráfica accesible a través de **Thinger.io** para visualización en tiempo real.

## Componentes
### Hardware
- 2x **ESP32 WROOM-32**
- 1x **DHT22** (Sensor de temperatura y humedad de alta precisión)
- 1x **DHT11** (Sensor de temperatura y humedad para ambiente externo)
- 1x **Galga extensiométrica 5Kg** + **Módulo HX711** (Medición de peso)
- 1x **Micrófono MEMS INMP441** (Monitoreo de actividad de abejas)
- 2x **Módulos LoRa Ra-01** (Transmisión y recepción de datos)
- Materiales para la estructura de la colmena

### Software y Plataforma
- **Arduino IDE** (Programación de ESP32)
- **Librerías**: `DHT.h`, `HX711.h`, `LoRa.h`, `ThingerESP32.h`
- **Thinger.io** (Plataforma IoT para visualización de datos)

## Instalación y Configuración
1. Clonar el repositorio:
   ```sh
   git clone https://github.com/tuusuario/tu-repositorio.git
   cd tu-repositorio
   ```
2. Instalar las librerías necesarias en Arduino IDE.
3. Configurar las credenciales de **Thinger.io** en `config.h`:
   ```cpp
   #define USERNAME "McTomSs"
   #define DEVICE_ID "ESP32_METX"
   #define DEVICE_CREDENTIAL "tqmSLAKcR+9kq5vl"
   ```
4. Cargar el código en los **ESP32** y conectar los sensores.

## Funcionamiento
- Los sensores recolectan datos de la colmena.
- El ESP32 envía los datos a través de LoRa a otro ESP32 receptor.
- Los datos se suben a **Thinger.io**, donde pueden ser visualizados en tiempo real.
- La interfaz permite a los apicultores monitorear las condiciones de la colmena.

## Resultados y Beneficios
- **Monitoreo en tiempo real** de temperatura, humedad, peso y actividad de abejas.
- **Optimización de producción** de miel mediante datos precisos.
- **Conectividad en zonas rurales** sin necesidad de WiFi.
- **Plataforma accesible** para ingenieros y apicultores.

## Autores
- **Luis Francisco Leal Baute**
- **Tomás Felipe Montañez Piñeros**
- **Edgar Daniel Ruiz Patiño**

## Licencia
Este proyecto está bajo la Licencia MIT.
