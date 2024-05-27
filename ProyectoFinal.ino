#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h> // Incluye la librería para el manejo de Bluetooth
// Pines
int bombaAgua = 8;
int sensorHumedadSuelo = A0; 
int botonEncender = 10;
int botonApagar = 9;

// Variables    
int humedadSuelo;
bool bombaEncendida = false; // Variable para controlar el estado de la bomba
// Objetos
LiquidCrystal_I2C lcd(0x27, 16, 2); 
void setup() {
  // Inicializa comunicación serial para el módulo Bluetooth
  Serial.begin(9600);
  // Setup Pantalla
  lcd.init();
  lcd.backlight(); // Encender retroiluminación (si es compatible)
  // Setup rele
  pinMode(bombaAgua, OUTPUT);
  // Setup botones
  pinMode(botonEncender, INPUT);
  pinMode(botonApagar, INPUT);
}

void loop() {
  // Inicialización de Variables sensor
  humedadSuelo = analogRead(sensorHumedadSuelo); 
  // Convertir lectura analógica a porcentaje de humedad (0-100)
  humedadSuelo = map(humedadSuelo, 0, 1023, 0, 100);

  // Lectura del estado de los botones
  int estadoBotonEncender = digitalRead(botonEncender);
  int estadoBotonApagar = digitalRead(botonApagar);

  // Pantalla
  lcd.setCursor(0, 0);
  lcd.print("Humedad Suelo: " + String(humedadSuelo) + "% ");
  
  // Manejo del estado de la bomba según la humedad y los botones
  if (humedadSuelo <= 20) { // Si la humedad es baja
    bombaEncendida = true; // Encender la bomba
  } else if (estadoBotonEncender == HIGH) { // Si se presiona el botón de encendido
    bombaEncendida = true; // Encender la bomba
  } else if (estadoBotonApagar == HIGH) { // Si se presiona el botón de apagado
    bombaEncendida = false; // Apagar la bomba
  }

  // Control de la bomba de agua
  if (bombaEncendida) {
    digitalWrite(bombaAgua, LOW); // Encender bomba de agua
    lcd.setCursor(0, 1);
    lcd.print("Regando*"); // Mostrar en la pantalla LCD
    lcd.setCursor(0, 2);
    lcd.print("");
  } else {
    digitalWrite(bombaAgua, HIGH); // Apagar bomba de agua
  }

  // Comunicación Bluetooth
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Lee el comando como un String
    command.trim(); // Elimina espacios en blanco alrededor del comando
    if (command == "ON") { // Si recibe "ON" desde la app (iniciar riego)
      bombaEncendida = true; // Activar riego automático
    } else if (command == "OFF") { // Si recibe "OFF" desde la app (apagar riego)
      bombaEncendida = false; // Desactivar riego automático
    }
  }

  delay(1000); // Espera un segundo antes de la próxima lectura
}