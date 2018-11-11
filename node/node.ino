#include "user.h"
#include "node.h"
#include "oled.h"
#include <aJSON.h>
#include <ArduinoJson.h>
//Json

aJsonStream serial_stream(&Serial);
unsigned long nowTime = millis();
unsigned long lcdTime = millis();
unsigned long lastTime = millis();
//环境参数
float temperature = 99.99;
float humidity = 99.99;
int light = 1000;
//时间、日期
int month = 0;
int day = 0;
int hour = 0;
int minute = 0;
int time = 0;
//新的一天
int prevDay = 0;
int todayWaterNum[3]={0,0,0};
//关于湿度、浇水
int firstValue[3]={0,0,0};
int sensorValue[3]={0,0,0};
bool needWater[3]={false,false,false};
bool waterState=false;
bool waterPotState[3]={false,false,false};
int lastWaterTime[3]={-99,-99,-99};
unsigned long waterStartTime[3]={0,0,0};

void setup(void) {  
  InitLED();
  delay(100);
  setYel();

  Serial.begin(9600);
  #if DEBUG
    Serial.println(F("DEBUG MODE ON..."));
    delay(100);
  #endif

  ZIGBEESERIALPORT.begin(BAUD); 
  #if DEBUG
    Serial.println("ZIGBEE INIT...");
    delay(100);
  #endif

  InitWater();
  delay(100);
  InitSensor();
  delay(100);

  //取第一次湿度,优化：增加算法
  for(int _i=0;_i<3;_i++){
    if(firstValue[_i]==0) firstValue[_i] = ReadSensor(SENSORPIN[_i]);
    #if DEBUG
      Serial.print(_i);
      Serial.print("-FIRSTVALUE:");
      Serial.println(firstValue[_i]);
    #endif
  }

  setGre();
}


void loop(void) { 
  zigbeeReceive(); 
  if (lastTime > millis())
        lastTime = millis();
  if (millis() - lastTime > INTERVALTIME){
     for (int i = 0; i < 3; ++i){
      if(lastWaterTime[i]>0){
        int _h = lastWaterTime[i]/60;
        int _m = lastWaterTime[i]%60;
        #if DEBUG
          Serial.print(i);
          Serial.print("-POT,LAST WATER AT:");
          Serial.print(_h);Serial.print(":");
          Serial.println(_m);
        #endif
      }
     }
     String _data = createDataJson();
     zigbeeSend(_data);
     lastTime = millis();
  }
  if (lcdTime > millis()) lcdTime = millis();
  if (millis() - lcdTime > INTERVALLCD) {   
    for (int i = 0; i < 3; ++i){
      sensorValue[i] = ReadSensor(SENSORPIN[i]);        
      #if DEBUG
        Serial.print(i);
        Serial.print("-HUMIDITY:");
        Serial.println(sensorValue[i]);
      #endif
    }
    volcd(time,firstValue[0], firstValue[1], firstValue[2], sensorValue[0],sensorValue[1],sensorValue[2],todayWaterNum[0],todayWaterNum[1],todayWaterNum[2]);   
    lcdTime = millis();
  }

  if(day != prevDay){
    prevDay = day;
    #if DEBUG
      Serial.println("NEW DAY.");
      Serial.println("LAST DAY,WATER COUNT:");
    #endif
    for (int i = 0; i < 3; ++i){
      #if DEBUG
        Serial.print(i);
        Serial.print("-POT:");
        Serial.println(todayWaterNum[i]);
      #endif
      todayWaterNum[i] = 0;
      lastWaterTime[i]=-99;
    }
    
  }

  //条件
  if(temperature<TEMPERATUREMAX && light<LIGHTMAX){
    for (int i = 0; i < 3; ++i){
      if(!waterPotState[i] && (time - lastWaterTime[i] > WATERGAP) && todayWaterNum[i] < WATERMAX && (sensorValue[i] < firstValue[i] * THRESHOLD)){
        needWater[i] = true;
        #if DEBUG
          Serial.print(i);
          Serial.println("-POT NEED WATER.");
        #endif
      }
    }
  }

  for (int i = 0; i < 3; ++i){
    if(needWater[i] == true){
      WaterPotOpen(WATERPOTPIN[i]);
      WATEROPEN;
      #if DEBUG
        Serial.println("OPEN WATER.");
      #endif
      waterState=true;
      waterPotState[i]=true;
      waterStartTime[i]=millis();
      todayWaterNum[i]++;
      lastWaterTime[i] = time;
      needWater[i] = false;
      #if DEBUG
        Serial.print(i);
        Serial.println("-POT OPEN WATER.");
      #endif
    }
    if (waterState && millis() - waterStartTime[i] > WATERTIME) {
      WATERCLOSE;
      waterState=false;
      #if DEBUG
        Serial.println("CLOSE WATER.");
      #endif
    }
    if (waterPotState[i] && millis() - waterStartTime[i] > WATERPOTTIME) {
      WaterPotClose(WATERPOTPIN[i]);
      waterPotState[i]=false;
      #if DEBUG
        Serial.print(i);
        Serial.println("-POT CLOSE WATER.");
      #endif
    }
  }
  /*
  //test json
  String temp;
  while(Serial.available() > 0){
    temp += char(Serial.read());
    delay(4);
  }
  if(temp!=""){
    Serial.print(temp);
    const char *json = temp.c_str();
    Serial.println(json);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json); 
    day = root["day"]; 
    Serial.print("DATE,DAY:");Serial.println(day);
    hour = root["hour"]; 
    Serial.print("TIME,HOUR:");Serial.println(hour);
    minute = root["minute"]; 
    Serial.print("TIME,MINUTE:");Serial.println(minute);
    temperature = root["temperature"];
    Serial.print("SENSOR,TEMPERATURE:");Serial.println(temperature);
    light = root["light"];
    Serial.print("SENSOR,LIGHTNESS:");Serial.println(light);
    time = hour*60 + minute;
  }
  */
}


void zigbeeReceive() {
  String temp,header;

  while(ZIGBEESERIALPORT.available() > 0)
  {
    temp += char(ZIGBEESERIALPORT.read());
    delay(5);
  }
  delay(50);

  //排除刚刚上电时可能收到乱码数据
  //+ZBD=XXXX,YYY,**
  //0123456789ABCDEF
  //正常数据必须保证大于2个字符
  if (temp.length() > 10+LEN){
    #if DEBUG
      Serial.print("RECEIVE DATA:");
      Serial.println(temp);
    #endif
    //取数据头部信息，判断为“重启”还是“数据”
    header = temp.substring(0,5);
    temp = temp.substring(10+LEN);
  }

  //该信息为“数据”
  if(header == "+ZBD="){
    //{"t":22.06,"l":21,"d":24,"h":0,"m":13}
    char *json = temp.c_str();
    Serial.println(json);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json); 
    if(root.success()){
      day = root["d"]; 
      Serial.print("DATE,DAY:");Serial.println(day);
      hour = root["h"]; 
      Serial.print("TIME,HOUR:");Serial.println(hour);
      minute = root["m"]; 
      Serial.print("TIME,MINUTE:");Serial.println(minute);
      temperature = root["t"];
      Serial.print("SENSOR,TEMPERATURE:");Serial.println(temperature);
      light = root["l"];
      Serial.print("SENSOR,LIGHTNESS:");Serial.println(light);
      time = hour*60 + minute;
    }
  //该信息为“重启”
  }else if(header == "+SRST"){
    #if DEBUG
      Serial.print("RESTART SUCCESSFUL.");
    #endif
    //接受到重启，执行操作
  }else{

  }
  
  header = "";
  temp = "";
}

void zigbeeSend(String str){
  ZIGBEESERIALPORT.println(str); delay(50);
  #if DEBUG
    Serial.println("SENDDATA-->COOR:"+ str);
  #endif
}

//void zigbeeSendData(int x,String __data){
//  __x = String(x);
//  zigbeeSend("DATAREQUIRE," + __x + "," + __data);
//}

aJsonObject *createJsonData()
{
  aJsonObject *msg = aJson.createObject();
  aJsonObject *node = aJson.createItem(NODE);
  aJsonObject *h0 = aJson.createItem(sensorValue[0]);
  aJsonObject *h1 = aJson.createItem(sensorValue[1]);
  aJsonObject *h2 = aJson.createItem(sensorValue[2]);
  aJson.addItemToObject(msg, "node", node);
  aJson.addItemToObject(msg, "humidity1", h0);
  aJson.addItemToObject(msg, "humidity2", h1);
  aJson.addItemToObject(msg, "humidity3", h2);
  return msg;
}

String createDataJson(){
  String jsonData;
  jsonData = "{\"node\":";
  jsonData += String(NODE);
  jsonData += ",\"h1\":";
  jsonData += String(sensorValue[0]);  
  jsonData += ",\"h2\":";
  jsonData += String(sensorValue[1]);
  jsonData += ",\"h3\":";
  jsonData += String(sensorValue[2]);
  jsonData += "}";
  #if DEBUG
    Serial.print("DATA:");
    Serial.println(jsonData);
  #endif
  return jsonData;
}


