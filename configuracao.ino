#include <ESP8266WiFi.h>         

#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>  
int outputpin= A0;

#define TRIGGER_PIN 0

void setup() {
  Serial.begin(115200);
  Serial.println("\n Inicializando");

  pinMode(TRIGGER_PIN, INPUT);

  
}


void loop() {

  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    WiFiManager wifiManager;

    wifiManager.resetSettings();
 
    if (!wifiManager.startConfigPortal("MicrosTeste")) {
      Serial.println("Falha na conexão");
      delay(3000);

      ESP.reset();
      delay(5000);
    }

    Serial.println("Conectado");

  }

  int valorAnalogico = analogRead(outputpin);
  float millivolts = (valorAnalogico/1024.0) * 3300;
  float celsius = millivolts/10;  
  Serial.print("em Celsius=   ");
  Serial.println(celsius);

  delay(1000);

}
