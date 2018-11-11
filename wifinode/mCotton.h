#include <ESP8266.h>

#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
#define EspSerial Serial1
#define UARTSPEED  115200
#endif

#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); /* RX:D2, TX:D3 */

#define EspSerial mySerial
#define UARTSPEED  9600
#endif

ESP8266 wifi(&EspSerial);

#define WIFI_DEBUG
#define TIMEOUT  5000

const char* _id;
const char* _token;
long timer_Push, time_Connect;

void connectTimeout(String _st) {
  if (millis() - time_Connect > TIMEOUT) {
#ifdef WIFI_DEBUG
    Serial.print(_st);
    Serial.print(F("    Connect again...\r\n"));
#endif
    time_Connect = millis();
  }
}

boolean mCottonbegin(const char* ssid, const char* pass, const char* id, const char* token) {
  _id = id;
  _token = token;
  Serial.print(F("Start Connection mCotton...Waiting for 5S...\r\n"));
  WifiInit(EspSerial, UARTSPEED);
  // wifi.restore();
  // delay(2000);
  //--------------------1------------------//
  while (!wifi.setOprToStation()) {
    connectTimeout("To station timeout");
  }
  time_Connect = millis();
#ifdef WIFI_DEBUG
  Serial.print(F("To station ok\r\n"));
#endif
  //--------------------2------------------//
  while (!wifi.joinAP(ssid, pass)) {
    connectTimeout("Join AP timeout");
  }
  time_Connect = millis();
  wifi.setWiFiconnected(true);
#ifdef WIFI_DEBUG
  Serial.print(F("Join AP success\r\n"));
#endif
  //--------------------3------------------//
  while (!wifi.mqttSetServer("mCotton.microduino.cn", (1883))) {
    connectTimeout("mqtt set server timeout");
  }
  time_Connect = millis();
#ifdef WIFI_DEBUG
  Serial.print(F("mqtt set server ok\r\n"));
#endif
  //--------------------4------------------//
  while (!wifi.mqttConnect(id, id, token)) {
    connectTimeout("mqtt connect timeout");
  }
  time_Connect = millis();
  wifi.setMqttConnected(true);
#ifdef WIFI_DEBUG
  Serial.print(F("mqtt connect ok\r\n"));
#endif
  //--------------------5------------------//
  while (!wifi.mqttSetDiveceIDToken(id, token)) {
    connectTimeout("mqtt set device ID Token timeout");
  }
  time_Connect = millis();
#ifdef WIFI_DEBUG
  Serial.print(F("mqtt set device ID Token ok\r\n"));
#endif
  //--------------------6------------------//
  char SUBSCRIBTOPICA[30] = "";
  strcat(SUBSCRIBTOPICA, "ca/");
  strcat(SUBSCRIBTOPICA, id);
  while (!wifi.mqttSetSubscrib(SUBSCRIBTOPICA)) {
    connectTimeout("mqtt set subscrib ca topic timeout");
  }
  time_Connect = millis();
#ifdef WIFI_DEBUG
  Serial.print(F("mqtt set subscrib ca topic ok\r\n"));
#endif
  //--------------------7------------------//
  char SUBSCRIBTOPICP[30] = "";
  strcat(SUBSCRIBTOPICP, "cp/");
  strcat(SUBSCRIBTOPICP, id);
  while (!wifi.mqttSetSubscrib(SUBSCRIBTOPICP)) {
    connectTimeout("mqtt set subscrib cp topic timeout");
  }
  time_Connect = millis();
#ifdef WIFI_DEBUG
  Serial.print(F("mqtt set subscrib cp topic ok\r\n"));
#endif
  return true;
}

boolean mCottonagain(const char* id, const char* token) {
  while (!wifi.mqttSetDiveceIDToken(id, token)) {
    connectTimeout("mqtt set device ID Token timeout");
  }
  time_Connect = millis();
#ifdef WIFI_DEBUG
  Serial.print(F("mqtt set device ID Token ok\r\n"));
#endif
  //--------------------6------------------//
  char SUBSCRIBTOPICA[30] = "";
  strcat(SUBSCRIBTOPICA, "ca/");
  strcat(SUBSCRIBTOPICA, id);
  while (!wifi.mqttSetSubscrib(SUBSCRIBTOPICA)) {
    connectTimeout("mqtt set subscrib ca topic timeout");
  }
  time_Connect = millis();
#ifdef WIFI_DEBUG
  Serial.print(F("mqtt set subscrib ca topic ok\r\n"));
#endif
  //--------------------7------------------//
  char SUBSCRIBTOPICP[30] = "";
  strcat(SUBSCRIBTOPICP, "cp/");
  strcat(SUBSCRIBTOPICP, id);
  while (!wifi.mqttSetSubscrib(SUBSCRIBTOPICP)) {
    connectTimeout("mqtt set subscrib cp topic timeout");
  }
  time_Connect = millis();
#ifdef WIFI_DEBUG
  Serial.print(F("mqtt set subscrib cp topic ok\r\n"));
#endif
  return true;
}
