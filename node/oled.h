#include <Arduino.h>
#include <U8glib.h>
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);    //设置OLED型号  
//-------字体设置，大、中、小   
#define setFont_L u8g.setFont(u8g_font_7x13)   
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)   
#define setFont_S u8g.setFont(u8g_font_chikitar)   
#define X 0
#define Y -2
//
//void osd_setup(int _osd_setup,char* _osd_text) {
//  u8g.firstPage();  
//  do {
//    setFont_L;
//    u8g.setPrintPos(4, 30); 
//    u8g.print(_osd_text);
//    u8g.drawFrame(0,48,128,14);
//    if(_osd_setup)
//      u8g.drawBox(0+2,48+2,map(_osd_setup,0,5,0,128-4),14-4);
//  } 
//  while( u8g.nextPage() );
//}


//显示函数 
void volcd(int _t,unsigned int _fh1, unsigned int _fh2,unsigned int _fh3,unsigned int _h1,unsigned int _h2,unsigned int _h3,unsigned int _s1,unsigned int _s2,unsigned int _s3) {
  u8g.firstPage();
  do {
    u8g.setDefaultForegroundColor();

    // u8g.drawXBMP( 4, 1, 15, 32, bmp_tem);
    // u8g.drawXBMP( 70, 2, 24, 30, bmp_hum);

    setFont_M;                             //设置字体为大  

    u8g.setPrintPos(6+X, 16+Y);         //设置文字开始坐标  
    u8g.print("FH1:");   
    u8g.setPrintPos(30+X, 16+Y);         //设置文字开始坐标  
    u8g.print(_fh1);             //温度  


    u8g.setPrintPos(6+X, 32+Y);          //设置文字开始坐标  
    u8g.print("FH2:"); 
    u8g.setPrintPos(30+X, 32+Y);         //设置文字开始坐标  
    u8g.print(_fh2);       //光照强度  

    u8g.setPrintPos(6+X, 48+Y);          //设置文字开始坐标  
    u8g.print("FH3:"); 
    u8g.setPrintPos(30+X, 48+Y);         //设置文字开始坐标  
    u8g.print(_fh3);       //光照强度  


    u8g.setPrintPos(68+X, 16+Y);         //设置文字开始坐标  
    u8g.print("H1%:");   
    u8g.setPrintPos(92+X, 16+Y);         //设置文字开始坐标  
    u8g.print(_h1);             //湿度  
    
    u8g.setPrintPos(68+X, 32+Y);          //设置文字开始坐标
    u8g.print("H2%:"); 
    u8g.setPrintPos(92+X, 32+Y);         //设置文字开始坐标  
    u8g.print(_h2);       //光照强度

    u8g.setPrintPos(68+X, 48+Y);          //设置文字开始坐标
    u8g.print("H2%:"); 
    u8g.setPrintPos(92+X, 48+Y);         //设置文字开始坐标  
    u8g.print(_h3);       //光照强度

     int h = _t/60;
     int m = _t%60;  
     u8g.setPrintPos(6+X, 64+Y);          //设置文字开始坐标
     u8g.print("TIME:"); 
     u8g.setPrintPos(36+X, 64+Y);         //设置文字开始坐标  
     u8g.print(h);       //光照强度
     u8g.setPrintPos(48+X, 64+Y);         //设置文字开始坐标
     u8g.print(":");       //光照强度
     u8g.setPrintPos(54+X, 64+Y);         //设置文字开始坐标
     u8g.print(m);       //光照强度

    u8g.setPrintPos(78+X, 64+Y);          //设置文字开始坐标
    u8g.print("N:"); 
    setFont_S;  
    u8g.setPrintPos(92+X, 64+Y);         //设置文字开始坐标  
    u8g.print(_s1); 
    u8g.setPrintPos(100+X, 64+Y);         //设置文字开始坐标  
    u8g.print(_s2);     
    u8g.setPrintPos(108+X, 64+Y);         
    u8g.print(_s3); 

  }
  while( u8g.nextPage() );
} 


//void volcdsetup(char* zi,unsigned int x,unsigned int y) {
//  //#ifdef OLED
//  u8g.firstPage();  
//  do {
//    setFont_L;    
//    u8g.setPrintPos(x, y); 
//    u8g.print(zi);
//  } 
//  while( u8g.nextPage() );
//  //#endif
//}









