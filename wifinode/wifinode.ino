#include "user.h"
#include <aJSON.h>
#include <ArduinoJson.h>
#include "mCotton.h"

#define BUFFSIZE 200
char jsonHY[BUFFSIZE] = { 0x00 };
String str;
String jsonData;

float temperature;
int light;
int node;
int soilmoisture[5][3];

boolean change;

void setup(void) {

  Serial.begin(115200);
#if DEBUG
  Serial.println(F("DEBUG MODE ON..."));
  delay(100);
#endif

  ZIGBEESERIALPORT.begin(BAUD);
#if DEBUG
  Serial.println("ZIGBEE INIT...");
  delay(100);
#endif
  if (mCottonbegin("microduino", "microduino", "5bcf53d557fa8e001655799a", "j0ph2W9gRjnQ")) {
    Serial.println("mCotton connected is ok...");;
  }

}


void loop(void) {

  zigbeeReceive();
  String wifiData = wifi.getMqttJson();
  wifiData.trim();
  if (wifiData != "") {
#ifdef WIFI_DEBUG
    Serial.println(wifiData);
#endif
  }
  if (wifiData.equals("WIFI DISCONNECT")) {
    wifi.setWiFiconnected(false);
    wifi.setMqttConnected(false);
  } else if (wifiData.equals("WIFI Connected")) {
    wifi.setWiFiconnected(true);
  } else if (wifiData.equals("MQTT: Disconnected")) {
    wifi.setMqttConnected(false);
  } else if (wifiData.equals("MQTT: Connected")) {
    mCottonagain(_id, _token);
    wifi.setMqttConnected(true);
  }
  /*
    //test json
    String temp;
    while (Serial.available() > 0) {
      temp += char(Serial.read());
      delay(5);
    }
    if (temp != "") {
      Serial.print(temp);
      const char *json = temp.c_str();
      Serial.println(json);
      //    StaticJsonBuffer<200> jsonBuffer;
      //    JsonObject& root = jsonBuffer.parseObject(json);
      //    day = root["day"];
      //    Serial.print("DATE,DAY:");Serial.println(day);
      //    hour = root["hour"];
      //    Serial.print("TIME,HOUR:");Serial.println(hour);
      //    minute = root["minute"];
      //    Serial.print("TIME,MINUTE:");Serial.println(minute);
      //    temperature = root["temperature"];
      //    Serial.print("SENSOR,TEMPERATURE:");Serial.println(temperature);
      //    light = root["light"];
      //    Serial.print("SENSOR,LIGHTNESS:");Serial.println(light);
      //    time = hour*60 + minute;
    }
  */
}


void zigbeeReceive() {
  String temp, header;

  while (ZIGBEESERIALPORT.available() > 0)
  {
    temp += char(ZIGBEESERIALPORT.read());
    delay(2);
  }
  delay(50);
  if (temp.length() > 16) {
#if DEBUG
    Serial.print("RECEIVE DATA:");
    Serial.println(temp);
#endif
    //取数据头部信息，判断为“重启”还是“数据”
    header = temp.substring(0, 5);
    temp = temp.substring(10 + LEN);
    //temp = temp.substring(10 + LEN, str.length() + 1)
  }

  //该信息为“数据”
  if (header == "+ZBD=") {
    //{"t":22.06,"l":21,"d":24,"h":0,"m":13}
    temp.trim();//格式化数据
    Serial.println("&&&&&&&&&");
    Serial.println(temp);//这块数据格式与我下面的一致就没有问题
    Serial.println("&&&&&&&&&");
    //char *json = temp.c_str();
    for ( int i = 0; i < temp.length(); i++)
      jsonHY[i] = temp[i];
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonHY);
    if (root.success()) {
       Serial.println("in succes");
    //  change = !change;
      if (stringContains(temp, "\"t\"")) {
         Serial.println("in Contains..");
        temperature = root["t"];
        light = root["l"];
        Serial.print("SENSOR,TEMPERATURE:"); Serial.println(temperature);
        Serial.print("SENSOR,LIGHTNESS:"); Serial.println(light);
        jsonData = "{\"Temperature\":";
        jsonData += String(temperature);
        jsonData += ",\"Lightness\":";
        jsonData += String(light);
        jsonData += "}";
        Serial.println(jsonData);
        Serial.println("************************");
        if (wifi.mqttPublishM(jsonData)) {
          Serial.print(F("mqtt publishM ok\r\n"));
        } else {
          Serial.print(F("mqtt publishM err\r\n"));
        }
      } else if (stringContains(temp, "node")) {
        node = root["node"];
        soilmoisture[node - 1][0] = root["h1"];
        soilmoisture[node - 1][1] = root["h2"];
        soilmoisture[node - 1][2] = root["h3"];
        jsonData = "{\"PotH";
        jsonData += String(node);
        jsonData += "1\":";
        jsonData += String(soilmoisture[node - 1][0]);
        jsonData += ",\"PotH";
        jsonData += String(node);
        jsonData += "2\":";
        jsonData += String(soilmoisture[node - 1][1]);
        jsonData += ",\"PotH";
        jsonData += String(node);
        jsonData += "3\":";
        jsonData += String(soilmoisture[node - 1][2]);
        jsonData += "}";
        Serial.print("NODE NUM:"); Serial.println(node);
        Serial.print("SENSOR,H1:"); Serial.println(soilmoisture[node - 1][0]);
        Serial.print("SENSOR,H2:"); Serial.println(soilmoisture[node - 1][1]);
        Serial.print("SENSOR,H3:"); Serial.println(soilmoisture[node - 1][2]);
        Serial.println(jsonData);
        Serial.println("--------------------------");
        if (wifi.mqttPublishM(jsonData)) {
          Serial.print(F("mqtt publishM ok\r\n"));
        } else {
          Serial.print(F("mqtt publishM err\r\n"));
        }
      }
    }

    //该信息为“重启”
  } else if (header == "+SRST") {
#if DEBUG
    Serial.print("RESTART SUCCESSFUL.");
#endif
    //接受到重启，执行操作
  } else {
    //不接收Zigbee数据，直接上传指定数据
#ifdef MCOTTON_DEBUG
    testmCotton();
#endif
  }

  header = "";
  temp = "";
  delay(2000);
}

void zigbeeSend(String str) {
  ZIGBEESERIALPORT.println(str); delay(50);
#if DEBUG
  Serial.println("SENDDATA-->COOR:" + str);
#endif
}

boolean stringContains(String _msg, String _str)
{
  String Containsmsg = _msg;
  int Containsnum = Containsmsg.indexOf(_str);
  if (Containsnum >= 0)
    return true;
  else
    return false;
}

void testmCotton() {
  if (change)
    str = "{\"t\":26.26,\"l\":21,\"d\":24,\"h\":0,\"m\":13}";
  else
    str = "{\"node\":2,\"h1\":11,\"h2\":14,\"h3\":20}";

  for ( int i = 0; i < str.length(); i++)
    jsonHY[i] = str[i];
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonHY);
  if (root.success()) {
 //  serial.println("sdfhjkl");
    change = !change;
    if (stringContains(str, "\"t\"")) {
      temperature = root["t"];
      light = root["l"];
      Serial.print("SENSOR,TEMPERATURE:"); Serial.println(temperature);
      Serial.print("SENSOR,LIGHTNESS:"); Serial.println(light);
      jsonData = "{\"Temperature\":";
      jsonData += String(temperature);
      jsonData += ",\"Lightness\":";
      jsonData += String(light);
      jsonData += "}";
      Serial.println(jsonData);
      Serial.println("************************");
      if (wifi.mqttPublishM(jsonData)) {
        Serial.print(F("mqtt publishM ok\r\n"));
      } else {
        Serial.print(F("mqtt publishM err\r\n"));
      }
    } else if (stringContains(str, "node")) {
      node = root["node"];
      soilmoisture[node - 1][0] = root["h1"];
      soilmoisture[node - 1][1] = root["h2"];
      soilmoisture[node - 1][2] = root["h3"];
      jsonData = "{\"PotH";
      jsonData += String(node);
      jsonData += "1\":";
      jsonData += String(soilmoisture[node - 1][0]);
      jsonData += ",\"PotH";
      jsonData += String(node);
      jsonData += "2\":";
      jsonData += String(soilmoisture[node - 1][1]);
      jsonData += ",\"PotH";
      jsonData += String(node);
      jsonData += "3\":";
      jsonData += String(soilmoisture[node - 1][2]);
      jsonData += "}";
      Serial.print("NODE NUM:"); Serial.println(node);
      Serial.print("SENSOR,H1:"); Serial.println(soilmoisture[node - 1][0]);
      Serial.print("SENSOR,H2:"); Serial.println(soilmoisture[node - 1][1]);
      Serial.print("SENSOR,H3:"); Serial.println(soilmoisture[node - 1][2]);
      Serial.println(jsonData);
      Serial.println("--------------------------");
      if (wifi.mqttPublishM(jsonData)) {
        Serial.print(F("mqtt publishM ok\r\n"));
      } else {
        Serial.print(F("mqtt publishM err\r\n"));
      }
    }
  }
}

