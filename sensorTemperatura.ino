/*
Projeto 01 - NodeMCU ESP8266
Sistemas Microprocessados Avançados 2018/B
Integrantes: Eduardo Poletto e Marco Antônio Arnhold

Módulo Sensor de Temperatura
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

//dados do servidor MQTT
const char *mqttServer = "m12.cloudmqtt.com";
const int mqttPort = 19545;
const char *mqttUser = "yumjjfqc";
const char *mqttPassword = "HhftaPW2LTQB";

WiFiClient espClient;
PubSubClient client(espClient);

int outputpin = A0;

#define TRIGGER_PIN 0

void setup()
{
  Serial.begin(115200);
  Serial.println("\n Inicializando");
  pinMode(TRIGGER_PIN, INPUT);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void callback(char *topic, byte *payload, unsigned int length)
{

  Serial.print("Mensagem recebida no topico: ");
  Serial.println(topic);

  Serial.print("Mensagem: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");
}

void loop()
{
  if (digitalRead(TRIGGER_PIN) == LOW)
  {
    WiFiManager wifiManager;

    wifiManager.resetSettings();

    if (!wifiManager.startConfigPortal("MicrosTeste"))
    {
      Serial.println("Falha na conexão");
      delay(3000);

      ESP.reset();
      delay(5000);
    }

    Serial.println("Conectado");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    while (!client.connected())
    {
      Serial.println("Conectando ao MQTT...");

      if (client.connect("TempSensor", mqttUser, mqttPassword))
      {

        Serial.println("Conectado ao MQTT");
      }
      else
      {

        Serial.print("Conexao com o servidor MQTT falhou com o estado ");
        Serial.print(client.state());
        delay(2000);
      }
    }
  }

  int valorAnalogico = analogRead(outputpin);
  float millivolts = (valorAnalogico / 1024.0) * 3300;
  float celsius = millivolts / 10;
  Serial.print("Temperatura em Celsius =   ");
  Serial.println(celsius);

  delay(1000);

  char buff[10];

  dtostrf(celsius, 3, 2, buff);

  client.publish("home/temp", buff); //publica no canal home/temp do servidor MQTT
  client.subscribe("home/temp");
}
