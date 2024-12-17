/*
  Proyecto para ESP8266 con pantalla OLED y comunicación Bluetooth.
  Este programa permite escanear redes WiFi y dispositivos Bluetooth y
  manejar la comunicación con una placa LuckFox.

  **Librerías necesarias**:
  1. **Adafruit GFX Library** (Instalación desde el Gestor de Librerías de Arduino):
     - GitHub: https://github.com/adafruit/Adafruit-GFX-Library
  2. **Adafruit SSD1306** (Instalación desde el Gestor de Librerías de Arduino):
     - GitHub: https://github.com/adafruit/Adafruit_SSD1306
  3. **ESP8266WiFi** (Instalada con el soporte de ESP8266 en el IDE de Arduino)
  
  **Configuración de puertos**:
  - **RX/TX para comunicación Bluetooth**: Usaremos los puertos por defecto para Bluetooth, sin `SoftwareSerial`.
  - **Dirección I2C para pantalla OLED**: Definimos la dirección como una variable para poder cambiarla si es necesario.
  - **Dirección I2C para LuckFox**: Similar a la pantalla, también debe ser definida por variables.
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BluetoothSerial.h>

// Configuración de pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración Bluetooth
BluetoothSerial ESP_BT;

// Variables globales
uint8_t i2cAddressOLED = 0x3C;  // Dirección OLED por defecto
uint8_t i2cAddressLuckFox = 0;  // Se asignará tras escaneo I2C

bool luckFoxFound = false;
bool bluetoothConnected = false;

// Variables de color y estado
enum SecurityType { WPA, WPA2, WPA3 };
enum SignalStrength { DEBIL, NORMAL, FUERTE };

// Definir colores predeterminados
const int COLOR_RED = 1;     // Para alertas y fallos
const int COLOR_GREEN = 2;   // Sistema funcional y seguro
const int COLOR_ORANGE = 3;  // Información y advertencias
const int COLOR_BLUE = 4;    // Conexiones Bluetooth y reloj

// Mensajes del sistema
String systemMessage = "Sistema esperando ordenes";

void scanI2CDevices() {
  Serial.println("Escaneando dispositivos I2C...");
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      if (address == i2cAddressOLED) {
        Serial.println("Pantalla OLED encontrada.");
      } else {
        i2cAddressLuckFox = address;
        luckFoxFound = true;
        Serial.println("Placa LuckFox encontrada.");
      }
    }
  }
}

void initOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, i2cAddressOLED)) {
    Serial.println(F("No se pudo iniciar la pantalla OLED."));
    while (1);
  }
  display.clearDisplay();
  display.display();
  Serial.println("Pantalla OLED inicializada.");
}

void showStartupScreen() {
  display.clearDisplay();
  if (!luckFoxFound) {
    display.setTextColor(COLOR_RED);
    display.setCursor(0, 10);
    display.print("System Not Found");
  }
  if (!bluetoothConnected) {
    display.setTextColor(COLOR_BLUE);
    display.setCursor(0, 30);
    display.print("Bluetooth Not Connected");
  }
  if (luckFoxFound && bluetoothConnected) {
    display.setTextColor(COLOR_GREEN);
    display.setCursor(0, 20);
    display.print("Sistema Encendido");
    display.setCursor(0, 40);
    display.print("y Funcional");
  }
  display.display();
}

void scanWiFiNetworks() {
  Serial.println("Escaneando redes Wi-Fi...");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    String security = (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_PSK) ? "WPA2" : "WPA";
    Serial.printf("%d: SSID: %s, Seguridad: %s\n", i + 1, WiFi.SSID(i).c_str(), security.c_str());
  }
}

void scanBluetoothDevices() {
  Serial.println("Escaneando dispositivos Bluetooth...");
  ESP_BT.scanDevices();
}

void displayClock() {
  display.clearDisplay();
  for (int minutes = 0; minutes < 60; minutes++) {
    display.setTextColor(COLOR_BLUE);
    display.setCursor(10, 30);
    display.print("Tiempo: ");
    display.print(minutes);
    display.print(" min");
    display.display();
    delay(60000);  // Espera 60 segundos
  }
}


// Librerías necesarias
#include <Wire.h>        // Comunicación I2C
#include <Adafruit_GFX.h> // Biblioteca gráfica de Adafruit
#include <Adafruit_SSD1306.h> // Biblioteca para pantalla OLED SSD1306
#include <ESP8266WiFi.h> // Biblioteca WiFi para ESP8266

// Definición de pines para RX y TX (Bluetooth sin SoftwareSerial)
#define RX_PIN 3
#define TX_PIN 1

// Dirección I2C de la pantalla OLED
#define OLED_I2C_ADDR 0x3C







// Función para mostrar el menú##########################################################################################################
void showMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_GREEN);
  
  display.setCursor(10, 10);
  display.print("Selecciona una opcion:");
  
  display.setCursor(10, 30);
  display.print("1. Escanear WiFi");

  display.setCursor(10, 50);
  display.print("2. Escanear Bluetooth");

  display.setCursor(10, 70);
  display.print("3. Escanear todo");

  display.setCursor(10, 90);
  display.print("4. Salir");

  display.display();
}

// Función para manejar las entradas del menú
void handleMenuSelection(char selection) {
  switch (selection) {
    case '1':
      startWiFiScan();
      break;
    case '2':
      startBluetoothScan();
      break;
    case '3':
      startAllScan();
      break;
    case '4':
      showLogo();  // Volver al logo cuando el usuario salga
      break;
    default:
      display.clearDisplay();
      display.setTextColor(SSD1306_RED);
      display.setCursor(10, 10);
      display.print("Opcion no valida");
      display.display();
      break;
  }
}
// Enumerar redes WiFi
void listWiFiNetworks() {
  int numNetworks = WiFi.scanNetworks();
  
  for (int i = 0; i < numNetworks; i++) {
    display.setTextColor(SSD1306_RED);
    display.setCursor(10, 30 + (i * 10));
    display.print(i + 1);
    display.print(". ");
    display.setTextColor(SSD1306_ORANGE);
    display.print(WiFi.SSID(i));
    display.print(" ");
    display.setTextColor(SSD1306_GREEN);
    display.print(WiFi.encryptionType(i));
  }
  
  display.display();
}

// Enumerar dispositivos Bluetooth
void listBluetoothDevices() {
  // Código para escanear dispositivos Bluetooth
  // Mostrar MAC, nombre y proximidad (verde, naranja, rojo)
}
// Función para enviar paquetes a LuckFox
void sendDataToLuckFox(String data) {
  Wire.beginTransmission(LUCKFOX_I2C_ADDR);
  Wire.write(data.c_str());
  Wire.endTransmission();
}
// Función para mostrar el reloj
void showClock() {
  unsigned long previousMillis = 0;
  int minutes = 0;

  while (true) {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= 60000) {
      previousMillis = currentMillis;
      minutes++;
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(SSD1306_BLUE);
      display.setCursor(10, 10);
      display.print(minutes);
      display.display();
    }
  }
}


  WiFi.scanNetworksAsync([](int networksFound) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_GREEN);
    display.setCursor(10, 10);
    display.print("Redes WiFi encontradas:");
    
    for (int i = 0; i < networksFound; i++) {
      display.setCursor(10, 30 + (i * 10));
      display.print(i + 1);
      display.print(". ");
      display.setTextColor(SSD1306_RED);
      display.print(WiFi.SSID(i));
      display.setTextColor(SSD1306_GREEN);
      display.print(" ");
      display.print(WiFi.encryptionType(i));
    }
    display.display();
  });
}

// Función para escanear dispositivos Bluetooth
void startBluetoothScan() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_GREEN);
  display.setCursor(10, 10);
  display.print("Escaneando Bluetooth...");
  display.display();

  // Simulación de escaneo Bluetooth (aquí va el código real de escaneo Bluetooth)
  delay(2000);  // Simulación de espera

  // Ejemplo de dispositivos encontrados
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print("Dispositivos Bluetooth:");
  display.setTextColor(SSD1306_BLUE);

  display.setCursor(10, 30);
  display.print("1. 00:14:22:01:23:45");
  display.setTextColor(SSD1306_ORANGE);
  display.print(" Dispositivo1");

  display.setTextColor(SSD1306_BLUE);
  display.setCursor(10, 50);
  display.print("2. 00:14:22:01:23:46");
  display.setTextColor(SSD1306_ORANGE);
  display.print(" Dispositivo2");

  display.display();
}

// Función para escanear tanto WiFi como Bluetooth
void startAllScan() {
  startWiFiScan();
  delay(1000); // Espera entre escaneos
  startBluetoothScan();
}
// Función para mostrar notificaciones de progreso
void showProgress(String message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print(message);
  display.display();
}

// Función para mostrar barra de progreso
void showProgressBar(int progress) {
  int barWidth = 128; // Ancho total de la barra
  int fillWidth = map(progress, 0, 100, 0, barWidth);  // Mapea el progreso en la barra

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("Progreso:");
  display.fillRect(10, 30, fillWidth, 10, SSD1306_GREEN);
  display.display();
}

// Función para mostrar notificación cuando se está inyectando
void showInjecting() {
  showProgress("Inyectando datos...");
  showProgressBar(50); // Simulación de progreso
  delay(1000);
  showProgress("Terminado.");
  showLogo(); // Volver al logo de espera
}
// Función para mostrar el reloj en la pantalla
void showClock() {
  unsigned long previousMillis = 0;
  int minutes = 0;

  while (minutes < 60) {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= 60000) {  // Cada minuto
      previousMillis = currentMillis;
      minutes++;
      
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(SSD1306_BLUE);
      display.setCursor(10, 10);
      display.print(minutes); // Muestra los minutos
      display.display();
    }
  }
}
void setup() {
  // Iniciar la comunicación serial
  Serial.begin(115200);
  
  // Iniciar la pantalla OLED
  Wire.begin();
  if (!display.begin(SSD1306_I2C_ADDRESS, OLED_I2C_ADDR)) {
    Serial.println(F("No se pudo inicializar la pantalla OLED"));
    while (true);
  }
  
  // Mostrar el logo y el estado inicial
  showLogo();
  
  // Inicializar WiFi
  WiFi.begin("SSID", "PASSWORD"); // Reemplazar con tu red WiFi
  
  // Esperar la conexión WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.setCursor(10, 50);
    display.print("Conectando WiFi...");
    display.display();
  }
  
  // Cuando esté conectado, mostrar la IP
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print("WiFi conectado");
  display.setCursor(10, 30);
  display.print("IP: ");
  display.print(WiFi.localIP());
  display.display();
}





// Función para esperar y mostrar la respuesta de LuckFox
void handleLuckFoxResponse() {
  String response = readLuckFoxResponse();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("Respuesta LuckFox:");
  display.setCursor(10, 30);
  display.print(response);
  display.display();
}
// Función para enviar un comando a la placa LuckFox
void sendCommandToLuckFox(String command) {
  Wire.beginTransmission(LUCKFOX_I2C_ADDRESS);  // Iniciar la transmisión I2C
  Wire.write(command.c_str());  // Enviar el comando a LuckFox
  Wire.endTransmission();  // Finalizar la transmisión
  
  // Mostrar que el comando fue enviado correctamente
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("Enviando: ");
  display.print(command);
  display.display();
}
void startWiFiScan() {
  sendCommandToLuckFox("scan_wifi");  // Enviar el comando a LuckFox para escanear WiFi
  
  // Esperar la respuesta de LuckFox
  handleLuckFoxResponse();
}
void startBluetoothScan() {
  sendCommandToLuckFox("scan_bluetooth");  // Enviar el comando a LuckFox para escanear Bluetooth
  
  // Esperar la respuesta de LuckFox
  handleLuckFoxResponse();
}
void startAllScan() {
  sendCommandToLuckFox("scan_all");  // Enviar el comando para escanear WiFi y Bluetooth
  
  // Esperar la respuesta de LuckFox
  handleLuckFoxResponse();
}
void showInjecting() {
  sendCommandToLuckFox("inject_data");  // Enviar el comando para inyectar datos
  
  // Esperar la respuesta de LuckFox
  handleLuckFoxResponse();
}
void showClock() {
  unsigned long previousMillis = 0;
  int minutes = 0;

  while (minutes < 60) {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= 60000) {  // Cada minuto
      previousMillis = currentMillis;
      minutes++;
      
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(SSD1306_BLUE);
      display.setCursor(10, 10);
      display.print(minutes); // Muestra los minutos
      display.display();
    }
  }
}
#include <BluetoothSerial.h> // Biblioteca para manejar Bluetooth

// Declaración de objetos
BluetoothSerial ESP_BT;

// Prototipos de funciones
void startBluetoothScan();
void displayBluetoothDevice(String bluetoothDevice);

void setup() {
  // Configuración inicial del Serial
  Serial.begin(115200);
  Serial.println("Iniciando Bluetooth...");

  // Iniciar Bluetooth
  startBluetoothScan();

  // Configuración inicial de la pantalla
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Dirección I2C de la pantalla
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  Serial.println("Listo para recibir dispositivos Bluetooth.");
}

void loop() {
  // Verificar si hay datos Bluetooth disponibles
  if (ESP_BT.available()) {
    String bluetoothDevice = ESP_BT.readString();
    
    // Mostrar en la pantalla el dispositivo encontrado
    displayBluetoothDevice(bluetoothDevice);
  }
}

// Función para iniciar el escaneo Bluetooth
void startBluetoothScan() {
  ESP_BT.begin("ESP8266"); // Nombre de tu dispositivo Bluetooth
  Serial.println("Escaneando dispositivos Bluetooth...");
  ESP_BT.scanDevices();
}

// Función para mostrar información del dispositivo en la pantalla
void displayBluetoothDevice(String bluetoothDevice) {
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print("Dispositivo encontrado:");
  display.setCursor(10, 30);
  display.print(bluetoothDevice);
  display.display();

  // Imprimir también en el Serial
  Serial.print("Dispositivo encontrado: ");
  Serial.println(bluetoothDevice);
}
