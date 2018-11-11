#include <Microduino_ColorLED.h> //引用彩灯库
#define PIN            6         //彩灯引脚
#define NUMPIXELS      1        //级联彩灯数量
ColorLED strip = ColorLED(NUMPIXELS, PIN); 


void InitLED(){
  strip.begin();                 //彩灯初始化
  strip.setBrightness(60);       //设置彩灯亮度
}

void setRed(){
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 255, 0, 0); 
    strip.show();
  }
}
void setGre(){
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 0, 255, 0); 
    strip.show();
  }
}
void setYel(){
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 255, 255, 0); 
    strip.show();
  }
}

/****************sensor*****************/
void InitSensor(){
	for(int pin_i=0;pin_i<3;pin_i++){
    	pinMode(SENSORPIN[pin_i],INPUT);
  	}
  	#if DEBUG
	    Serial.print(F("initialite sensor pin.\r\n"));
	    delay(100);
	#endif
	  
}

int ReadSensor(const int _pin){
  int temp = analogRead(_pin);
  #if HUMIDITYDEBUG
     temp = random(99);
  #endif
	return temp;
}


/****************water*****************/

#define ENABLED 1
#define DISABLED 0
#define WATEROPEN  digitalWrite(WATERPIN,ENABLED)
#define WATERCLOSE digitalWrite(WATERPIN,DISABLED)

void InitWater(){
    pinMode(WATERPIN,OUTPUT);
    pinMode(WATERPOTPIN[0],OUTPUT);
    pinMode(WATERPOTPIN[1],OUTPUT);
    pinMode(WATERPOTPIN[2],OUTPUT);

  #if DEBUG
    Serial.print(F("initialite water pin.\r\n"));
    delay(100);
  #endif
}

void WaterPotOpen(const int _pin){
  digitalWrite(_pin,ENABLED);
  delay(10);
}


void WaterPotClose(const int _pin){
  digitalWrite(_pin,DISABLED);
}



