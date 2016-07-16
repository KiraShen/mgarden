1.首先我们实现底层节点的代码，使用的硬件是microduino，可以浏览www.microduino.cn查看更多信息
2.选用的硬件：microduino－core＋，mcroduino－ESP8266
3.节点的IDE：arduino for microduino，可以从microduino网站下载安装
4.另外需要的库文件：arduinojson，pubsubclient；添加到节点IDE
5.core＋和esp8266使用串口进行通信，core＋是一个单片机模块，获取传感器到数据或者接受命令；esp8266是一个wifi模组，将云端命令发送给core＋，或者将core＋获取到数据上传到云端
6.mic_esp_firmware.ino是esp8266点固件，开始之前需要为软件IDE添加esp8266点board信息，刷写esp8266可参考microduino官网的

