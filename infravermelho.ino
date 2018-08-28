#include <IRrecv.h>
#include <IRsend.h>
#include <IRtimer.h>
#include <IRutils.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>

IRsend irsend(4); //define o pino GPIO do ESP8266 a ser usado para enviar a mensagem IR, no caso o GPIO4 (D2)

int frequencia = 38; //FREQUÊNCIA DO SINAL IR(38KHz)

//dados do servidor MQTT
const char *mqttServer = "m12.cloudmqtt.com";
const int mqttPort = 19545;
const char *mqttUser = "yumjjfqc";
const char *mqttPassword = "HhftaPW2LTQB";

double temp;
//códigos RAW para emissor infravermelho (validos para o ar-condicionado split Electrolux CI36F)
uint16_t liga[] = {4600, 4150, 800, 1350, 800, 300, 750, 1400, 750, 1400, 750, 300, 800, 300, 750, 1400, 750, 350, 750, 300, 750, 1450, 700, 350, 700, 400, 700, 1450, 700, 1450, 700, 400, 650, 1500, 650, 400, 700, 400, 650, 450, 650, 1500, 650, 1500, 650, 1500, 650, 1500, 650, 1500, 650, 1550, 600, 1500, 650, 1550, 600, 450, 600, 500, 600, 450, 650, 450, 650, 450, 600, 450, 650, 450, 600, 1550, 600, 500, 600, 450, 650, 1500, 600, 500, 600, 500, 600, 1550, 600, 1550, 600, 500, 600, 1500, 650, 1550, 600, 450, 600, 1550, 600, 1550, 600};    //COLE A LINHA RAW CORRESPONDENTE DENTRO DAS CHAVES
uint16_t desliga[] = {4500, 4200, 650, 1450, 700, 400, 650, 1450, 650, 1500, 650, 400, 650, 450, 650, 1500, 600, 450, 650, 450, 600, 1500, 650, 450, 600, 500, 600, 1500, 600, 1550, 600, 450, 600, 1550, 600, 450, 600, 1550, 600, 1550, 600, 1550, 600, 1550, 600, 450, 600, 1550, 600, 1500, 600, 1550, 600, 450, 600, 500, 600, 450, 600, 500, 550, 1550, 600, 500, 550, 500, 600, 1550, 600, 1500, 600, 1550, 600, 500, 550, 500, 600, 450, 600, 500, 600, 450, 600, 500, 600, 450, 600, 450, 600, 1550, 600, 1550, 600, 1550, 550, 1600, 550, 1550, 550}; //COLE A LINHA RAW CORRESPONDENTE DENTRO DAS CHAVES

WiFiClient espClient;
PubSubClient client(espClient);

int outputpin = A0;

#define TRIGGER_PIN 0

void setup()
{
  Serial.begin(115200);
  Serial.println("\n Inicializando");
  pinMode(TRIGGER_PIN, INPUT);

  irsend.begin(); //INICIALIZA A FUNÇÃO

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
  temp = atof((char *)payload); //converte conjunto de chars recebidos para double e salva na variavel temp (temperatura)

  Serial.println();
  Serial.println("-----------------------");
}

void loop()
{
  if (digitalRead(TRIGGER_PIN) == LOW)
  {
    WiFiManager wifiManager;

    wifiManager.resetSettings(); //reseta configs da wi-fi

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
    Serial.println("Conectado a wi-fi");

    while (!client.connected())
    {
      Serial.println("Connectando ao MQTT...");

      if (client.connect("EmissorIR", mqttUser, mqttPassword))
      {

        Serial.println("Conectado ao servidor MQTT");

        client.subscribe("home/temp"); //se inscreve no canal MQTT para o qual o sensor de temperatura publica
      }
      else
      {

        Serial.print("Conexao com o servidor MQTT falhou com o estado  ");
        Serial.print(client.state());
        delay(2000);
      }
    }
  }

  delay(1000);

  if (client.loop()) //testa se esta conectado ao servidor MQTT e entra no loop
  {
    //Serial.print("entrou no loop ");
  }
  else //senao tenta reconectar ao MQTT
  {
    if (client.connect("EmissorIR", mqttUser, mqttPassword))
    {

      Serial.println("Conectado ao servidor MQTT");
      client.subscribe("home/temp");
    }
    else
    {
      Serial.print("Conexao com o servidor MQTT falhou com o estado ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  if (temp < 25) //desliga o ar-condicionado se a temperatura for menor que 25 graus Celsius
  {
    irsend.sendRaw(desliga, sizeof(desliga) / sizeof(desliga[0]), frequencia);
    Serial.println("Comando enviado: Desliga");
    delay(2000);
  }
  else if (temp > 25) //liga o ar-condicionado se a temperatura for maior do que 25 graus Celsius
  {
    irsend.sendRaw(liga, sizeof(liga) / sizeof(liga[0]), frequencia);
    Serial.println("Comando enviado: Liga");
    delay(2000);
  }
}
