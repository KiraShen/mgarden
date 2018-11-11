#include "user.h"
#include "dev.h"
#include "udp.h"
#include "oled.h"
#include <aJSON.h>
#include <ArduinoJson.h>
//Json
//StaticJsonBuffer<200> jsonBuffer;
float h1,h2,h3;
int node;
unsigned long lcdTime = millis();
unsigned long nowTime = millis();
//节点地址列表
String nodeAddress[MAXNUM];
//当前节点个数
int currentNodeNum = 0;
//最新节点再列表的序号
int currentNodeOrder = 0;

//UDP
boolean sta = 0;
boolean sta1 = 0;
unsigned long wifi_reconnect_Time = millis();
int count = CYCLE;
int time = 0;
//Json
//aJsonStream serial_stream(&Serial);
//补光灯状态
bool lightingStatus = false;
int lightOnTime = 0;

void setup(void) {
  Serial.begin(9600);
  #if DEBUG
    Serial.println("DEBUG MODE ON...");
    delay(100);
  #endif
  
  Serial1.begin(WIFIBAUD);
  WifiInit(EspSerial, WIFIBAUD);
  #if DEBUG
    Serial.println("WIFI INIT...");
    Serial.print("FW Version: ");
    Serial.println(wifi.getVersion().c_str());
    delay(100);
  #endif
  while (!wifi.setOprToStation()) ;
  #if DEBUG
    Serial.println("WIFI TO STATION OK");
  #endif
  while (!wifi.joinAP(SSID, PASSWORD)) ;
  #if DEBUG
    Serial.print("Join AP SUCCESS\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  #endif
  wifi.enableMUX();
  updateTimeData();
  
  TEMHUMINIT;
  LUXINIT;
  #if DEBUG
    Serial.println("TEM&HUM&LUX INIT...");
    delay(100);
  #endif

  LIGHTINIT;PIR1INIT;PIR2INIT;
  #if DEBUG
    Serial.println("LIGHTING INIT...");
    delay(100);
  #endif

  ZIGBEESERIALPORT.begin(ZIGBEEBAUD); 
  #if DEBUG
    Serial.println("ZIGBEE INIT...");
    delay(100);
  #endif

  //打印当前连接节点地址信息
  printNodeAddress(nodeAddress,currentNodeNum);
}

void loop(void) {
  /*
  // * TEM HUM LUX 测试代码
  Serial.print("Tem:");
  Serial.println(GETTEM); 
  delay(500);
  Serial.print("Hum:");
  Serial.println(GETHUM); 
  delay(500);
  Serial.print("Lux:");
  Serial.println(GETLUX); 
  delay(500);
  */
  /*
  // * WIFI UPD MODE 测试代码
  if (now() != prevDisplay) {
    prevDisplay = now();
    serialClockDisplay(year(), month(), day(), hour(), minute(), second());
  }
  /*/
  //OLED
  if (lcdTime > millis()) lcdTime = millis();
  if (millis() - lcdTime > INTERVALLCD) {
    volcd(GETTEM, GETHUM, GETLUX, time, lightingStatus);
    lcdTime = millis();
  }

  //每一分钟发数据到节点
  if (now() != prevDisplay) {
    prevDisplay = now();
    //serialClockDisplay(year(), month(), day(), hour(), minute(), second());
    if(++count > CYCLE){
      time = hour()*60 + minute();
      count = 1;
      String _data = createDataJson();
      String _wifiData = createWifiDataJson();
      /*
      aJsonObject *sensorData = createJsonData();
      String _data = String(aJson.print(sensorData));
      aJson.deleteItem(sensorData);
      #if DEBUG
        Serial.print("SENSOR DATA:");
        aJson.print(sensorData, &serial_stream);
        Serial.println();
      #endif
      */
      //发送数据到节点
      for (int _i = 0; _i < currentNodeNum; ++_i){
        if(nodeAddress[_i] == WIFINODE){
          ZIGBEESERIALPORT.println("+ZBD=" + nodeAddress[_i] + "," + _wifiData); delay(50);
          #if DEBUG
            Serial.print("SEND TO:WIFI NODE,DATA:");
            Serial.println(_data);
          #endif
          continue;
        }
        ZIGBEESERIALPORT.println("+ZBD=" + nodeAddress[_i] + "," + _data); delay(50);
        #if DEBUG
          Serial.print("SEND TO:"+ nodeAddress[_i]+" NODE,DATA:");
          Serial.println(_data);
        #endif
      }
      _data="";
    }
  }
  
  //补光灯
  if(time>=360 && time<=1200 && GETLUX<LUXMIN && !lightingStatus){//6:00-20:00
    OPENLIGHT;
    lightingStatus = true;
    lightOnTime = time;
    #if DEBUG
      Serial.println("LIGHTING ON - LIGHTING");
    #endif
  }else if((PIR1STATUS || PIR2STATUS) && !lightingStatus){
    OPENLIGHT;
    lightingStatus = true;
    lightOnTime = time;
    #if DEBUG
      Serial.println("LIGHTING ON - PEOPLE");
    #endif
  }else{

  }
  if(lightingStatus && (time-lightOnTime>=LUXTIME)){
    CLOSLIGHT;
    lightingStatus = false;
    #if DEBUG
      Serial.println("LIGHTING OFF");
    #endif
  }

  zigbeeReceive();
  
  //心跳
  /*
  if (nowTime > millis()) nowTime = millis();
  if (heartBeatStatus && (millis() - nowTime > HEARTBEATCYCLE)) {
    for (int _i = 0; _i < currentNodeNum; ++_i)
    {
      ZIGBEESERIALPORT.println("+ZBD=" + nodeAddress[_i] + "," + "HEARTBEAT"); delay(50);
      #if DEBUG
        Serial.println("HEARTBEAT:"+ nodeAddress[_i]);
      #endif
    }
    nowTime = millis();
  }
  */
  //UDP超时重连
  /*
  if (wifi_reconnect_Time > millis()) wifi_reconnect_Time = millis();
  if (wifiReconnectStatus && (millis() - wifi_reconnect_Time > WIFIRECONNECTTIME) ) {
    wifi_reconnect_Time = millis();
    if (wifi.setOprToStation()) {
      Serial.print("to station ok\r\n");
      sta = 1;
    } else {
      Serial.print("to station err\r\n");
      sta = 0;
    }
    if (wifi.joinAP(SSID, PASSWORD)) {
      Serial.print("Join AP success\r\n");
      Serial.print("IP: ");
      Serial.println(wifi.getLocalIP().c_str());
      sta1 = 1;
    } else {
      Serial.print("Join AP failure\r\n");
      sta1 = 0;
    }
  }
  */
}

void zigbeeReceive() {
  String temp,header,_address;

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
  if (temp.length() > (14+LEN)){
    #if DEBUG
      Serial.print("RECEIVE DATA:");
      Serial.println(temp);
    #endif
    //取数据头部信息，判断为“重启”还是“数据”
    header = temp.substring(0,4);
  }

  //该信息为“数据”
  if(header == "+ZBD"){
    //存储当前数据来源地址
    /*
    _address = temp.substring(5,9);
    char *json = temp.c_str();
    #if DEBUG
      Serial.print("DATA:");
      Serial.println(json);
    #endif
    
    JsonObject& root = jsonBuffer.parseObject(json); 
    h1 = root["h1"]; 
    Serial.print("HUMIDITY1:");Serial.println(h1);
    h2 = root["h2"]; 
    Serial.print("HUMIDITY2:");Serial.println(h2);
    h3 = root["h3"]; 
    Serial.print("HUMIDITY3:");Serial.println(h3);
    node = root["node"];
    Serial.print("NODE No.:");Serial.println(node);
    */
    //将各个节点的数据转发给WIFI
    temp = temp.substring(10+LEN);
    ZIGBEESERIALPORT.println("+ZBD=" + WIFINODE + "," + temp); delay(50);
    #if DEBUG
      Serial.print("SEND TO:"+ WIFINODE +" NODE,DATA:");
      Serial.println(temp);
    #endif
    /*
      for (int _i = 0; _i < currentNodeNum; ++_i){
        ZIGBEESERIALPORT.println("+ZBD=" + nodeAddress[_i] + "," + temp); delay(50);
        #if DEBUG
          Serial.print("SEND TO:"+ nodeAddress[_i]+" NODE,DATA:");
          Serial.println(temp);
          //aJson.print(sensorData, &serial_stream);Serial.println();
        #endif
      }
      */
    /*
    接受到心跳响应，执行响应动作
     if(temp.substring(10+LEN,23+LEN) == "RESPHEARTBEAT" ){
       #if DEBUG
         Serial.print("RECEIVE RESPHEARTBEAT:");
         Serial.println(_address);
       #endif
     }
    接受到其他数据，执行操作 
    node--->coor:DATAREQUIRE,X,DATA
    coor rev:+ZBD=XXXX,DATAREQUIRE,X,DATA
    
    if(temp.substring(10+LEN,21+LEN) == "DATAREQUIRE" ){
      int __i = temp.substring(22+LEN,23+LEN).toInt();
      String __address = nodeAddress[__i];
      String __data = temp.substring(24+LEN,temp.length()-2);
      #if DEBUG
        Serial.println("RECEIVE DATAREQUIRE:" + __data + "-->" + __address);
      #endif
      zigbeeSend(__address,__data);
    }
    */
  //该信息为“重启”or“启动”
  }else if(header == "+ZBC"){
    //存储当前数据来源地址
    _address = temp.substring(8,12);
    #if DEBUG
      Serial.print("NODE START,ADDRESS:");
      Serial.println(_address);
    #endif
    currentNodeOrder = getNodeOrder(nodeAddress,_address,currentNodeNum);    
    if(currentNodeOrder == -1){
      Serial.println("ERROR:TOO MORE NODE,PLEASE RESTART.");
    }
    #if DEBUG
      Serial.print("NODE ADDRESS LISTS:");
      //打印当前连接节点地址信息
      printNodeAddress(nodeAddress,currentNodeNum);
    #endif

  }else{
    
  }
  
  header = "";
  temp = "";
}

//协调器向节点发送命令数据
//coor--->node:+ZBD=39D8,DATACMD,DATA
//node rev:+ZBD=39D8,DATACMD,DATA
void zigbeeSend(String address, String str){
  ZIGBEESERIALPORT.println("+ZBD=" + address + ",DATACMD" + "," + str); delay(50);
  #if DEBUG
    Serial.println("SENDDATA-->"+ address + ":" + str);
  #endif
}

//打印节点地址信息
void printNodeAddress(String a[],int n)
{
  int i;
  for(i=0;i<n;i++){
    Serial.print(i);
    Serial.print(":");
    Serial.println(a[i]);
  }
}

//查找当前节点地址是否存在，否则插入
//返回当前插入的序号，溢出则返回-1
int getNodeOrder(String a[], String value, int n)
{
    int i;
    for(i=0; i<n; i++){
        if(a[i]==value){
            return i;
        }
    }
    if(n>=MAXNUM)
      return -1;
    nodeAddress[i] = value;
    currentNodeNum++;
    return i;
}

aJsonObject *createJsonData()
{
  //char _time[10],_minute[4];
  //itoa(hour(),_time,10);itoa(minute(),_minute,10);
  //strcat(_time,":");strcat(_time,_minute);
  aJsonObject *msg = aJson.createObject();
  aJsonObject *temperature = aJson.createItem(GETTEM);
  aJson.addItemToObject(msg, "temperature", temperature);
  aJsonObject *humidity = aJson.createItem(GETHUM);
  aJson.addItemToObject(msg, "humidity", humidity);
  aJsonObject *light = aJson.createItem(GETLUX);
  aJson.addItemToObject(msg, "light", light);
//  aJsonObject *_month = aJson.createItem(month());
//  aJson.addItemToObject(msg, "month", _month);
  aJsonObject *_day = aJson.createItem(day());
  aJson.addItemToObject(msg, "day", _day);
  aJsonObject *_hour = aJson.createItem(hour());
  aJson.addItemToObject(msg, "hour", _hour);
  aJsonObject *_minute = aJson.createItem(minute());
  aJson.addItemToObject(msg, "minute", _minute);
  //aJsonObject *time = aJson.createItem(_time);
  //aJson.addItemToObject(msg, "time", time);
  return msg;
}

String createDataJson(){
  String jsonData;
  jsonData = "{\"t\":";
  jsonData += String(GETTEM);
  jsonData += ",\"l\":";
  jsonData += String(GETLUX);  
  jsonData += ",\"d\":";
  jsonData += String(day());
  jsonData += ",\"h\":";
  jsonData += String(hour());
  jsonData += ",\"m\":";
  jsonData += String(minute());
  jsonData += "}";
  #if DEBUG
    Serial.print("CREATE SENSOR DATA:");
    Serial.println(jsonData);
  #endif
  //ZIGBEESERIALPORT.println("+ZBD=" + WIFINODE + "," + jsonData); delay(20);
  return jsonData;
}

String createWifiDataJson(){
  String jsonData;
  jsonData = "{\"t\":";
  jsonData += String(GETTEM);
  jsonData += ",\"l\":";
  jsonData += String(GETLUX);  
  jsonData += ",\"h\":";
  jsonData += String(GETHUM);
  jsonData += "}";
  #if DEBUG
    Serial.print("CREATE SENSOR WIFI DATA:");
    Serial.println(jsonData);
  #endif
  //ZIGBEESERIALPORT.println("+ZBD=" + WIFINODE + "," + jsonData); delay(20);
  return jsonData;
}

