#include <Wire.h>
#include <SoftwareSerial.h>
#include <bluefruit.h>
#include <math.h>

//Pin define
#define bg96_W_DISABLE 29
#define bg96_RESET 28
#define bg96_PWRKEY 2
#define bg96_GPS_EN 39

//global variable define
TwoWire *wi = &Wire;
const int baudrate = 115200;
String bg96_rsp = "";
#define ADV_TIMEOUT   60 // seconds
#define lis3dh_addr   0x19
#define opt3001_addr  0x44
#define shtc3_addr  0x70
#define lps22hb_addr  0x5c


//bg96 power up
void bg96_init()
{
     pinMode(bg96_RESET, OUTPUT);
     pinMode(bg96_PWRKEY, OUTPUT);
     pinMode(bg96_GPS_EN, OUTPUT);
     pinMode(bg96_W_DISABLE, OUTPUT);

     digitalWrite(bg96_RESET,0);
     digitalWrite(bg96_PWRKEY,1);
     digitalWrite(bg96_W_DISABLE,1);
     delay(2000);
     digitalWrite(bg96_PWRKEY,0);
     digitalWrite(bg96_GPS_EN,1);
     delay(2000);
}
//this function is suitable for most AT commands of bg96. e.g. bg96_at("ATI")
void bg96_at(char *at)
{
  char tmp[256] = {0};
  int len = strlen(at);
  strncpy(tmp,at,len);
  tmp[len]='\r';
  Serial1.write(tmp);
  delay(10);
  while(Serial1.available()){
      bg96_rsp += char(Serial1.read());
      delay(2);
  }
  Serial.println(bg96_rsp);
  bg96_rsp="";
}

//sensor init
void sensor_init()
{
  wi->beginTransmission(lis3dh_addr);
  wi->write(byte(0x20));
  wi->write(byte(0x57));  
  wi->endTransmission();
  delay(5);
  wi->beginTransmission(lis3dh_addr);
  wi->write(byte(0x23));
  wi->write(byte(0x08));  
  wi->endTransmission();
  delay(5);  

  wi->beginTransmission(shtc3_addr);
  wi->write(byte(0x35));
  wi->write(byte(0x17));  
  wi->endTransmission();
  delay(5);  
  
  wi->beginTransmission(shtc3_addr);
  wi->write(byte(0xEF));
  wi->write(byte(0xC8));  
  wi->endTransmission();
  delay(5);


  wi->beginTransmission(opt3001_addr);
  wi->write(byte(0x01));
  wi->write(byte(0xCC));
  wi->write(byte(0x10));  
  wi->endTransmission();
  delay(5);
  
  wi->beginTransmission(lps22hb_addr);
  wi->write(byte(0x10));
  wi->write(byte(0x50)); 
  wi->endTransmission();
}

//acc data
void acc_data_show()
{
  byte acc_h;
  byte acc_l;
  int x = 0;
  int y = 0;
  int z = 0;
  float accx;
  float accy;
  float accz;
  wi->beginTransmission(lis3dh_addr);
  wi->write(byte(0x28));
  wi->endTransmission();
  wi->requestFrom(lis3dh_addr, 1);
  while(wi->available()){
    acc_l = wi->read();
  } 
  wi->beginTransmission(lis3dh_addr);
  wi->write(byte(0x29));
  wi->endTransmission();
  wi->requestFrom(lis3dh_addr, 1);
  while(wi->available()){
    acc_h = wi->read();
  }
  x = (acc_h << 8) | acc_l;
  if(x<0x8000){x=x;}else{x=x-0x10000;}
  accx = x*4000/65536.0;
  
  wi->beginTransmission(lis3dh_addr);
  wi->write(byte(0x2a));
  wi->endTransmission();
  wi->requestFrom(lis3dh_addr, 1);
  while(wi->available()){
    acc_l = wi->read();
  } 
  wi->beginTransmission(lis3dh_addr);
  wi->write(byte(0x2b));
  wi->endTransmission();
  wi->requestFrom(lis3dh_addr, 1);
  while(wi->available()){
    acc_h = wi->read();
  }
  y = (acc_h << 8) | acc_l;
  if(y<0x8000){y=y;}else{y=y-0x10000;}
  accy = y*4000/65536.0;
  
  wi->beginTransmission(lis3dh_addr);
  wi->write(byte(0x2c));
  wi->endTransmission();
  wi->requestFrom(lis3dh_addr, 1);
  while(wi->available()){
    acc_l = wi->read();
  } 
  wi->beginTransmission(lis3dh_addr);
  wi->write(byte(0x2d));
  wi->endTransmission();
  wi->requestFrom(lis3dh_addr, 1);
  while(wi->available()){
    acc_h = wi->read();
  }
  z = (acc_h << 8) | acc_l;
  if(z<0x8000){z=z;}else{z=z-0x10000;}
  accz = z*4000/65536.0;
  Serial.print("ACC(mg): x=");
  Serial.print(accx);
  Serial.print(" y=");
  Serial.print(accy);
  Serial.print(" z=");
  Serial.println(accz);  
}

//light data
void light_show()
{
  byte tmp[2];
  int i=0;
  int m;
  int e;
  double h;
  float light;
  wi->beginTransmission(opt3001_addr);
  wi->write(byte(0x00));
  wi->endTransmission();
  wi->requestFrom(opt3001_addr, 2);
  while(wi->available()){
    tmp[i++] = wi->read();
  }
  m=((tmp[0]<<8) | tmp[1]) & 0x0FFF;
  e=(((tmp[0]<<8) | tmp[1]) & 0xF000) >> 12;
  h= pow(2,e);
  light = m*(0.01*h);
  Serial.print("Light=");
  Serial.println(light); 
}

//pressure data
void pressure_data_show()
{
  byte xl;
  byte l;
  byte h;
  int pre;
  float p;
  wi->beginTransmission(lps22hb_addr);
  wi->write(byte(0x28));
  wi->endTransmission();
  wi->requestFrom(lps22hb_addr, 1);
  while(wi->available()){
    xl = wi->read();
  } 
  wi->beginTransmission(lps22hb_addr);
  wi->write(byte(0x29));
  wi->endTransmission();
  wi->requestFrom(lps22hb_addr, 1);
  while(wi->available()){
    l = wi->read();
  }
  
  wi->beginTransmission(lps22hb_addr);
  wi->write(byte(0x2a));
  wi->endTransmission();
  wi->requestFrom(lps22hb_addr, 1);
  while(wi->available()){
    h = wi->read();
  } 
  pre = (h<<16) | (l<<8) | xl;
  if(pre & 0x00800000){
    pre |= 0xFF000000;
  }
  p= pre/4096.0;
  Serial.print("Pressure(HPa) =");
  Serial.println(p); 
}

//temperature & humidity
void environment_data_show()
{
  byte t[6];
  int i=0;
  float _temperature;
  float _humidity;
  wi->beginTransmission(shtc3_addr);
  wi->write(byte(0x7C));
  wi->write(byte(0xA2));  
  wi->endTransmission();
  
  wi->beginTransmission(shtc3_addr);
  wi->endTransmission();
  wi->requestFrom(shtc3_addr, 6);
  while(wi->available()){
    t[i++] = wi->read();
  } 
  _temperature= (t[1]|(t[0]<<8))*175/65536.0 -45.0;
  _humidity=(t[4]|(t[3]<<8))*100/65536.0;
  Serial.print("Temperature =");
  Serial.print(_temperature); 
  Serial.print(" humidity =");
  Serial.println(_humidity);  
  Serial.println("");
}
//gps data
void gps_show()
{
 
  bg96_rsp="";
  Serial1.write("AT+QGPSGNMEA=\"GGA\"\r");
  delay(10);
  while(Serial1.available()){
      bg96_rsp += char(Serial1.read());
      delay(2);
  }
  Serial.println(bg96_rsp);
  bg96_rsp="";
}
/**************************************************************************/
/*!
    @brief  The setup function runs once when reset the board
*/
/**************************************************************************/
void setup()
{
  
  Serial.begin (baudrate);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  Serial.println("******************************Arduino on Rak5010******************************");
  Serial.println("bg96 power up!\n");
  bg96_init();
  Serial1.begin(baudrate);
  while ( !Serial1 ) delay(10);   // for bg96 with uart1, softserial is limited in baudrate
  delay(5000);
  bg96_at("ATE0");
  delay(1000);
  bg96_at("AT+QGPSCFG=\"gpsnmeatype\",1");
  delay(1000);
  bg96_at("AT+QGPS=1, 1, 1, 1, 1");
  wi->begin();         // join i2c bus (address optional for master)
  Serial.println("Scanning address from 0 to 127");

  for (int addr = 1; addr < 128; addr++)
  {
    wi->beginTransmission(addr);
    if ( 0 == wi->endTransmission() )
    {
      Serial.print("Found: 0x");
      Serial.print(addr, HEX);
      Serial.println();
    }
  }
  sensor_init();
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Rak5010");
  Serial.println("Start advertising!");
  // Set up and start advertising
  startAdv();
}
void startAdv(void)
{   
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.setStopCallback(adv_stop_callback);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in units of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(ADV_TIMEOUT);      // Stop advertising entirely after ADV_TIMEOUT seconds 
}
void adv_stop_callback(void)
{
  Serial.println("Advertising time passed, advertising will now stop.");
}
/**************************************************************************/
/*!
    @brief  The loop function runs over and over again forever
*/
/**************************************************************************/
void loop()
{
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Test degin<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  delay(3000);
  Serial.println(" ");
  bg96_at("ATI");
  delay(2000);
  acc_data_show();
  delay(1000);  
  light_show();
  delay(1000);
  pressure_data_show();
  delay(1000);
  environment_data_show();
  delay(1000);
  gps_show();
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Test end<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  delay(5000);

}
