//软串口，根据ZigBee硬件选用
//默认为D4，D5
//波特率9600
#include <SoftwareSerial.h>
SoftwareSerial Serial2(4,5);
#define ZIGBEESERIALPORT Serial2
#define ZIGBEEBAUD 9600

//温湿度传感器
#include <Microduino_Tem_Hum.h>
Tem_Hum_S2 termo;
#define TEMHUMINIT termo.begin()
#define GETTEM termo.getTemperature()
#define GETHUM termo.getHumidity()

//光敏传感器
#define LIGHTPIN A0
#define LUXINIT pinMode(LIGHTPIN,INPUT)
#define GETLUX analogRead(LIGHTPIN)  //map(analogRead(LIGHTPIN),0,1024,0,255)

//补光灯
#define LIGHTINGPIN 10
#define LIGHTINIT pinMode(LIGHTINGPIN,OUTPUT)
#define OPENLIGHT digitalWrite(LIGHTINGPIN,1)
#define CLOSLIGHT digitalWrite(LIGHTINGPIN,0)

//PIR
#define PIR1 8
#define PIR2 9
#define PIR1INIT pinMode(PIR1,INPUT)
#define PIR2INIT pinMode(PIR2,INPUT)
#define PIR1STATUS digitalRead(PIR1)
#define PIR2STATUS digitalRead(PIR2)
