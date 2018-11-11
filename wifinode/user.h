//软串口，根据ZigBee硬件选用
//默认为D4，D5
//波特率9600
#include <SoftwareSerial.h>
SoftwareSerial Serial2(4, 5);
#define ZIGBEESERIALPORT Serial2
#define BAUD 9600

//是否开启debug，串口调试
#define DEBUG  1

#define LEN 4

//#define MCOTTON_DEBUG
