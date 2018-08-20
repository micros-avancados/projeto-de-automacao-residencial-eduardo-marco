
#include <IRrecv.h>
//#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRtimer.h>
#include <IRutils.h>

#include <ESP8266WiFi.h>         

#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>  
#include <PubSubClient.h>
#include <IRremoteESP8266.h> //INCLUSÃO DE BIBLIOTECA
 
IRsend irsend(4); 
 
int tamanho = 67; //TAMANHO DA LINHA RAW(68 BLOCOS)
int frequencia = 38; //FREQUÊNCIA DO SINAL IR(32KHz)

const char* mqttServer = "m12.cloudmqtt.com";
const int mqttPort = 19545;
const char* mqttUser = "yumjjfqc";
const char* mqttPassword = "HhftaPW2LTQB";

double temp;

uint16_t  liga[] = {4600,4150, 800,1350, 800,300, 750,1400, 750,1400, 750,300, 800,300, 750,1400, 750,350, 750,300, 750,1450, 700,350, 700,400, 700,1450, 700,1450, 700,400, 650,1500, 650,400, 700,400, 650,450, 650,1500, 650,1500, 650,1500, 650,1500, 650,1500, 650,1550, 600,1500, 650,1550, 600,450, 600,500, 600,450, 650,450, 650,450, 600,450, 650,450, 600,1550, 600,500, 600,450, 650,1500, 600,500, 600,500, 600,1550, 600,1550, 600,500, 600,1500, 650,1550, 600,450, 600,1550, 600,1550, 600}; //COLE A LINHA RAW CORRESPONDENTE DENTRO DAS CHAVES
uint16_t  desliga[] = {4500,4200, 650,1450, 700,400, 650,1450, 650,1500, 650,400, 650,450, 650,1500, 600,450, 650,450, 600,1500, 650,450, 600,500, 600,1500, 600,1550, 600,450, 600,1550, 600,450, 600,1550, 600,1550, 600,1550, 600,1550, 600,450, 600,1550, 600,1500, 600,1550, 600,450, 600,500, 600,450, 600,500, 550,1550, 600,500, 550,500, 600,1550, 600,1500, 600,1550, 600,500, 550,500, 600,450, 600,500, 600,450, 600,500, 600,450, 600,450, 600,1550, 600,1550, 600,1550, 550,1600, 550,1550, 550}; //COLE A LINHA RAW CORRESPONDENTE DENTRO DAS CHAVES

WiFiClient espClient;
PubSubClient client(espClient);

int outputpin= A0;

#define TRIGGER_PIN 0

void setup() {
  Serial.begin(115200);
  Serial.println("\n Inicializando");
  pinMode(TRIGGER_PIN, INPUT);  
  
  irsend.begin(); //INICIALIZA A FUNÇÃO
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
   temp = atof ((char*)payload);
  
  Serial.println();
  Serial.println("-----------------------");
 
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

  if (WiFi.status() == WL_CONNECTED) {
            Serial.println("conectado a wi-fi");  

    while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
 
      if (client.connect("ESP8266Reader", mqttUser, mqttPassword )) {
 
        Serial.println("connected");  
 
        client.subscribe("home/temp");
      } else {
 
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
 
      }
    }
  }

  delay(1000);
  

  client.loop();
  if (temp < 25)  {
      irsend.sendRaw(desliga, sizeof(desliga) / sizeof(desliga[0]), frequencia);
     // irsend.sendRaw(desliga,67,38);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
      Serial.println("Comando enviado: Desliga");
      delay(2000);
  } else if (temp > 25) {
    irsend.sendRaw(liga, sizeof(liga) / sizeof(liga[0]), frequencia);
    //irsend.sendRaw(liga,tamanho,frequencia);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
    Serial.println("Comando enviado: Liga");
    delay(2000);
  }
  
}

