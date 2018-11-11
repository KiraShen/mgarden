//是否开启debug，串口调试
#define DEBUG  1

//协调器->节点 数据发送周期(秒)
#define CYCLE 30

//WIFI
#define SSID "gogogo"
#define PASSWORD "11223344"
//wifi重连状态默认关闭
bool wifiReconnectStatus = false;
//wifi重连时间
#define WIFIRECONNECTTIME 3600000

//oled
#define INTERVALLCD 2000

//数据是否带有校验字符，默认有4个字符
#define LEN 4

//节点最大个数
#define MAXNUM 10

//心跳周期
//每个节点心跳循播时间
//总周期 = NUM * HEARTBEATCYCLE
#define HEARTBEATCYCLE 1000
//心跳状态默认关闭
bool heartBeatStatus = false;

//6:00-20:00光线小于阈值
#define LUXMIN 200
#define LUXTIME 5 

String WIFINODE="F9D1";
