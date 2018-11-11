//软串口，根据ZigBee硬件选用
//默认为D4，D5
//波特率9600
#include <SoftwareSerial.h>
SoftwareSerial Serial2(4,5);
#define ZIGBEESERIALPORT Serial2
#define BAUD 9600

//是否开启debug，串口调试
#define DEBUG  1
#define HUMIDITYDEBUG  1

//数据是否带有校验字符
//默认有4个字符
#define LEN 4

const int WATERPOTPIN[3]={10,11,12}; //电磁阀-----------
#define WATERPIN 13   //水泵----------------------------
#define WATERTIME 3000  //3s 浇水时间--------------------
#define WATERPOTTIME 15000  //15s 电磁阀时间--------------------
#define WATERMAX 5	//每天最大浇水次数--------------------
#define WATERGAP 30 //30mins----------------------------
#define TEMPERATUREMAX 30
#define LIGHTMAX 700
const int SENSORPIN[3]={A0,A2,A6}; //湿度土壤------------

#define THRESHOLD 0.7 //70%----------------------------0.7

#define INTERVALTIME 25000 
#define INTERVALLCD 2000

#define NODE 1 //节点标记，1，2，3，4，5表示5个节点----------
