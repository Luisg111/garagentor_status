#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "wifi_credentials.h"

const uint8_t LEDPIN = D3;
const uint8_t LEDPIN2 = D2;
const uint8_t REEDPIN = D1;

const uint16_t scanDelay = 100;

const uint16_t CONNECT_DELAY = 20000;
const uint16_t SEND_INTERVAL_NORMAL = 30000;
const uint16_t SEND_INTERVAL_AFTER_FAIL = 2000;

const char* SERVER_URL = "http://melderserver.homeserver.lan/garage_status";
const char* HOSTNAME = "Garage";

WiFiClient client;
HTTPClient http;
uint32_t rpi_millis = 0;
uint32_t connectTimer = 0;

bool garageZu = false;
bool garageZuAlt = false;
bool zustandGesendet = false;
uint8_t ledState = LOW;
uint16_t sendIntervall = SEND_INTERVAL_AFTER_FAIL;

void WiFiBegin()
{
  WiFi.mode(WIFI_STA);
  WiFi.persistent(true);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(true);
  WiFi.hostname(HOSTNAME);

  if(WiFi.SSID() != WIFI_SSID || WiFi.psk() != WIFI_PASS)
  {
    WiFi.begin(WIFI_SSID,WIFI_PASS);
    //Serial.println("Speichere neue WiFi-Zugangsdaten");
  }

  client.setTimeout(500);
}

bool sendMessage()
{
  String message;
  bool zustandGesendet = false;
  if(WiFi.isConnected())
  {
    client.setTimeout(5000);
    if(http.begin(client,SERVER_URL))
    {
      char buffer[48];
      memset(buffer,0,sizeof(buffer));
      sprintf(buffer,"{\"isOpen\":%s}",garageZu?"false":"true");
      http.POST(buffer);
      //Serial.println((uint8_t)garageZu);
      zustandGesendet = true;
      http.end();
    }
    else
      //Serial.println("Fehler beim senden der Nachricht");
    client.flush();
    client.stop();
  }
  else
  {
    //Serial.println("WiFi nicht verbunden");
  }
  return zustandGesendet;
}

void checkGarage()
{
  if(digitalRead(REEDPIN) == HIGH)
    garageZu = true;
  else
    garageZu = false;
}

void setup() {
  //Serial.begin(115200);
  //Serial.println();
  pinMode(REEDPIN, INPUT_PULLUP);
  pinMode(LEDPIN, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  WiFiBegin();
}

void loop() {
  checkGarage();
  digitalWrite(LEDPIN, garageZu ? 0 : 1);
  digitalWrite(LEDPIN2, garageZu ? 0 : 1);

  if((garageZu^garageZuAlt)){
    delay(scanDelay);
    checkGarage();
    if((garageZu^garageZuAlt))
      sendMessage();
  }
    

  garageZuAlt = garageZu;  
  digitalWrite(LED_BUILTIN,!WiFi.isConnected());

  if(millis()-rpi_millis >= sendIntervall)
  {
    rpi_millis = millis();
    if(sendMessage())
      sendIntervall = SEND_INTERVAL_NORMAL;
    else
      sendIntervall = SEND_INTERVAL_AFTER_FAIL;  
  }

  if((!WiFi.isConnected()) && (millis() - connectTimer >= CONNECT_DELAY || connectTimer == 0))
  {
    connectTimer = millis();
    WiFi.begin();
    //Serial.println("Wiederverbindungsversuch");
  }
}