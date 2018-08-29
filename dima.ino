#include <ESP8266WiFi.h> 
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>



SoftwareSerial gtSerial(4,3); // Arduino RX, Arduino TX

 
const char* ssid = "kolesnik"; //Подключается к точке доступа OpenWrt
const char* password = "ots.kh.ua";
char buf[30]; 
long sec;
int ss;
String addr_ds="";
String read_text="";
String test_read_text_30s="";
int max_count_send_text=300;
int numberOfDevices;


//IPAddress ip(192,168,88,23);
//IPAddress gateway(192,168,88,1);
//IPAddress subnet(255,255,255,0); 
ESP8266WebServer server(8080);
OneWire ds(13); // Датчик температуры DS18B20 на GPIO - 0
#define ONE_WIRE_BUS 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress  DS18B20;
char out_temp[1000];

const int led1 = D7; // GPIO - 1 LED R
const int led2 = D5; // GPIO - 2 LED W
String inputString = ""; 
void temper() { 
  //if ( test_read_text_30s  == read_text ) { read_text=""; Serial.println("clear because no update "); }
  //else { test_read_text_30s = read_text;
  //Serial.println("update: test_read_text_30s ");}
  
  
  sensors.requestTemperatures();
  sprintf(out_temp, "","");
  for ( int ii=0; ii<numberOfDevices; ii++) {
      //String adr_cur=printAddress(123);
      String adr_cur="";
      if (!sensors.getAddress(DS18B20, ii)) 
      {
        adr_cur="adr_didnt_read";
      }else {
        for (uint8_t i = 0,j=0; i < 8; i++)
        {
          if (DS18B20[ii] < 16) adr_cur+="0";
          adr_cur+=String(DS18B20[i],HEX);
        }
      }
      float celsius=sensors.getTempCByIndex(ii);
      char str_temp[6];
      dtostrf(celsius, 4, 2, str_temp);
      char humidity=10;
      sprintf(out_temp, "\"temperature\":%s,\"humidity\":10",str_temp);
  }
}

 
void tem(){
char send_temp[1000];
  sprintf(send_temp, "{%s}", out_temp); 
  server.send(200, "text/x-json",send_temp);
}
void whi_on() {
  digitalWrite(led2, 1);
  delay(1000);
  digitalWrite(led2, 0);
  server.send(200, "text/x-json","1");
}
void whi_off() {
  digitalWrite(led2, 0); 
  server.send(200, "text/html","0");
}
/*
void red_on() {
  digitalWrite(led1, 0); 
  server.send(200, "text/x-json","1");
}
void red_off() {
  digitalWrite(led1, 1); 
  server.send(200, "text/x-json","0");
}
*/

void w(){  
  Serial.println("send"); 
  server.send(200, "text/x-json",read_text); 
read_text="";   
}

void whi_sta(){
  if (digitalRead(led2)==1) server.send(200, "text/html","1");
  if (digitalRead(led2)==0) server.send(200, "text/html","0");
}
void root(){
  server.send(200, "text/html","<h1>D1.<br><br>Command:</h1><br><a href='/io2on'>/io2on</a> <br>  <a href='/io2off'>/io2off</a><br>  <A href='/temp'>temp</a> <br> <a href='/status'>/status</a><br> <a href='/w'>/w</a><br>"); 
}
void setup(void){
  Serial.begin(1200);
  gtSerial.begin(1200);
  Serial.println("wellcome to wesi :D");
  pinMode(12, OUTPUT);
  digitalWrite(12, 1); 
  pinMode(led2, OUTPUT);
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount(); 
  //WiFi.config(ip,gateway,subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.hostname("esp-wifi-vesi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  temper();
  server.on("/temp", tem);
  server.on("/io2on", whi_on);
  server.on("/w", w);
  server.on("/io2off", whi_off); 
  server.on("/status", whi_sta);
  server.on("/", root); 
  server.begin();
 
//  pinMode(led1, OUTPUT);
//  pinMode(led2, OUTPUT);
//  digitalWrite(led1, 1);//1
//  digitalWrite(led2, 0);//0
}


int incomingByte = 0;   // для хранения полученного байта
int i=0;
void loop(void){ 
  /*
  char buffer[4];
 if(i>30000) {
   Serial.println("Sending 02");
   sprintf(buffer, "%02X ", 0x02);
   gtSerial.write(buffer);
  i=0;
 }
 if (gtSerial.available()) {
   int inByte = gtSerial.read();
   Serial.write(inByte); 
 }
 if (Serial.available()) {
   int inByte = Serial.read();
   gtSerial.write(inByte); 
   Serial.write(inByte);
 }
 i++;*/

  

/* if (Serial.available()) {
    byte in = Serial.read(); 
    Serial.write(in);
    gtSerial.write(in);
  }*/
  int i=0;
  if ( gtSerial.available() ) {
  //while (i <200 ) {
                // read the incoming byte:
                int incomingByte = gtSerial.read();
                char tmp[1];
                tmp[0]=incomingByte;
                tmp[1] = '\0';
                //Serial.print(tmp);
                //String incomingByte = gtSerial.readString();
                read_text = read_text+""+tmp+"";
                if ( read_text.length() >=max_count_send_text ) { 
                  int start = read_text.length()-max_count_send_text;
                  read_text = read_text.substring(start,read_text.length());
                  //Serial.println(read_text.length());
                }
                //Serial.println(read_text);
  }
    /*
  
  if (gtSerial.available() > 0) {
                // read the incoming byte:
                incomingByte = gtSerial.read();

                // say what you got:
                //Serial.print("I received: ");
                //Serial.println(incomingByte, DEC);
                
                //Serial.print("inc: ");
                //Serial.println(incomingByte);
                Serial.write(incomingByte);
  } */

 sec=millis()/1000;
 ss=sec%60; // second
 if(ss==0 || ss==30 ) {
  temper(); 
 }
 server.handleClient(); 
}

