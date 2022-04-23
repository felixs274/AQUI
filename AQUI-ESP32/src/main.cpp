// - - - = = = # # # Includes # # # = = = - - -
#include <Arduino.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESP32Time.h>
#include "FS.h"
#include <SPI.h>
#include <SD.h>
#include <Tone32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <AsyncTimer.h>

// - - - = = = # # # Variables # # # = = = - - -
const char* HARDWARE_VERSION = "V2.0";
const char* SOFTWARE_VERSION = "V2.1.2";

const char* hostname = "AQUI";

const char* ssid = "ssid";
const char* password = "password";

const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 3600;

#define P2_RUNVALUE 10


AsyncTimer t;

AsyncWebServer server(80);
ESP32Time rtc;

Adafruit_SSD1306 display(128, 64, &Wire, -1);


#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DS18B20 sensor(&oneWire);


// JSON Buffers
char buffer[512];
StaticJsonDocument<512> doc;

char settbuffer[512];
StaticJsonDocument<512> sett;

char sysbuffer[512];
StaticJsonDocument<512> sys;

char statsbuffer[512];
StaticJsonDocument<512> statistics;


// IO Pins
#define s1 34
#define s2 35
#define s3 32
#define s4 33
#define s5 25
#define s6 26
#define s7 27
#define s8 14
#define s9 13
#define s10 15

#define resetbtn 17

#define p1 2

#define p2 16

#define t2 39 // 79.6 kOhm
#define t3 36 // 47.9 kOhm

#define buzzer 12


// Various timing variables
unsigned int currentday;
unsigned long cur_timer;
unsigned long pre_timer = 0;
unsigned int interval = 1000;
unsigned int timeshift;
unsigned long timer = 1;
unsigned long looptimer_1 = 0;
unsigned long looptimer_5 = 0;

int activation_p2 = 3;
bool activation_p2_pre = false;
int counter_p2 = 0;
bool active_p2 = false;

// Runtime Variables & Arrays
int p1_list[7];
unsigned long p1_nr;
unsigned long p1_max_time;

int p2_list[7];

float t1_temp;
float t2_temp;
float t3_temp;


// States for IO Pins
bool s1_status;
bool s2_status;
bool s3_status;
bool s4_status;
bool s5_status;
bool s6_status;
bool s7_status;
bool s8_status;
bool s9_status;
bool s10_status;

bool p1_status;
bool p2_status;

bool resetbtn_status;
bool resetbtn_status_pre;

bool s1_status_pre;
bool s2_status_pre;
bool s3_status_pre;
bool s4_status_pre;
bool s5_status_pre;
bool s6_status_pre;
bool s7_status_pre;
bool s8_status_pre;
bool s9_status_pre;
bool s10_status_pre;

bool p1_status_pre;

bool t1_status_pre;


int i2c_devs = 0;

bool display_available = false;

bool booted = false;

// - - - = = = # # # Main Functions # # # = = = - - -
// Write to Log on SD card
void write_log(String msg){
  Serial.println(msg);
  File dataFile = SD.open("/log/log.txt", FILE_APPEND);
  String x = (String)rtc.getDay() + "." + (String)(rtc.getMonth()+1) + "." + (String)rtc.getYear() + " " + rtc.getTime() + " - " + msg + "\n";
  dataFile.print(x);
  dataFile.close();
}


// Writes P1 runtime Array and NoReset Variable into Runtime File on SD Card
bool P1_write_PumpTime(){
  write_log("Writing P1 List to SD");
  //Time normal pump time
  String x;
  File dataFile = SD.open("/data/p1_0", FILE_WRITE);
  for(int i=0; i<7; i++){
    x += (String)p1_list[i] + "\n";
  }
  dataFile.print(x);
  dataFile.close();
  //Time no reset
  dataFile = SD.open("/data/p1_nr", FILE_WRITE);
  dataFile.print(p1_nr);
  dataFile.close();
  return true;
}

// Read P1 runtime file & NoReset runtime file on SD card into Array
bool P1_read_PumpTime(){
  //Time normal pump time
  File dataFile = SD.open("/data/p1_0");
  for(int i=0; i<7; i++){
    p1_list[i] = dataFile.parseInt();
  }
  dataFile.close();
  //Time no reset
  dataFile = SD.open("/data/p1_nr");
  p1_nr = dataFile.parseInt();
  dataFile.close();
  return true;
}

// P1 Switch Day
// [Deletes first value and shift everything back by 1]
bool P1_switch_PumpTime(){
  write_log("Switching P1 List");
  for(int i=6; i>0; i--){
    p1_list[i] = p1_list[i-1];
  }
  p1_list[0] = 0;
  P1_write_PumpTime();
  return true;
}

// Resets only current P1 runtime
bool P1_reset_PumpTime(){
  write_log("Reset soft");
  p1_list[0] = 0;
  P1_write_PumpTime();
  return true;
}

// Resets current P1 runtime & NoReset runtime
bool P1_reset_All(){
  write_log("Reset all");
  p1_list[0] = 0;
  p1_nr = 0;
  P1_write_PumpTime();
  return true;
}

// Prints P1 array
void P1_display(){
  for(int i=0; i<7; i++){
    Serial.print(i+1); Serial.print(": "); Serial.println(p1_list[i]);
  }
  Serial.print("nr: "); Serial.println(p1_nr);
}


// Write P2 runtime array into file on SD card
bool P2_write_PumpTime(){
  write_log("Writing P2 List to SD");
  Serial.print("P2[0] = "); Serial.println(p2_list[0]);
  String x;
  File dataFile = SD.open("/data/p2_0", FILE_WRITE);
  for(int i=0; i<7; i++){
    x += (String)p2_list[i] + "\n";
  }
  dataFile.print(x);
  dataFile.close();
  return true;
}

// Read P2 runtime file on SD card into Array
bool P2_read_PumpTime(){
  File dataFile = SD.open("/data/p2_0");
  for(int i=0; i<7; i++){
    p2_list[i] = dataFile.parseInt();
  }
  dataFile.close();
  return true;
}

// P2 Switch Day
// [Deletes first value and shift everything back by 1]
bool P2_switch_PumpTime(){
  write_log("Switching P2 List");
  for(int i=6; i>0; i--){
    p2_list[i] = p2_list[i-1];
  }
  p2_list[0] = 0;
  P2_write_PumpTime();
  return true;
}

// Prints P2 array
void P2_display(){
  for(int i=0; i<7; i++){
    Serial.print(i+1); Serial.print(": "); Serial.println(p2_list[i]);
  }
}


// New Day Function
// [Resets Pump time and switches Days in Arrays]
void newDay(){
  if(P1_switch_PumpTime()){
    if(P2_switch_PumpTime()){
      P1_read_PumpTime();
      P2_read_PumpTime();
    }
  }
}


// Write MaxPumpTime file on SD card
bool write_MaxPumpTime(int tmn){
  char itos[10];
  write_log("Changed MaxPumpTime to " + (String)itoa(tmn, itos, 10));
  File dataFile = SD.open("/settings/p1_max_runtime", FILE_WRITE);
  p1_max_time = tmn;
  dataFile.print(tmn);
  dataFile.close();
  return true;
}

// Read MaxPumpTime file on SD card
bool read_MaxPumpTime(){
  File dataFile = SD.open("/settings/p1_max_runtime");
  p1_max_time = dataFile.parseInt();
  dataFile.close();
  return true;
}

// Write timeshift file on SD card
bool write_Timeshift(int val){
  char itos[10];
  write_log("Changed Timeshift to " + (String)itoa(val, itos, 10));
  File dataFile = SD.open("/settings/timeshift", FILE_WRITE);
  if(val == 0) timeshift = 0;
  if(val == 1) timeshift = 3600;
  dataFile.print(timeshift);
  dataFile.close();
  return true;
}

// Read timeshift file on SD card
bool read_Timeshift(){
  File dataFile = SD.open("/settings/timeshift");
  timeshift = dataFile.parseInt();
  dataFile.close();
  return true;
}



// - - - = = = # # # Get Time # # # = = = - - -
void recTime(){
  write_log("Receive time from NTP Server");
  configTime(gmtOffset_sec, timeshift, ntpServer);
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)){
      rtc.setTimeStruct(timeinfo);
  }
}



// - - - = = = # # # Get Temp # # # = = = - - -
void getTempT1(){
  sensor.requestTemperatures();

  unsigned long curtime = millis();
  while (!sensor.isConversionComplete()){
    if(millis() - curtime >+ 3000){
      Serial.println("DS18B20 Timeout");
      break;
    }
  };
  float tmp = sensor.getTempC();
  if (tmp != -127.00 && tmp != -128.00 && tmp != -0.0625 && tmp != -0.00 && tmp != 85.00){ // Known Error codes
    t1_temp = tmp;
  } else {
    Serial.println("DS18B20 Error");
  }

}

void getTempT2(){
  t2_temp = (float(analogRead(t2))*5/(4095))/0.01;
}

void getTempT3(){
  t3_temp = (float(analogRead(t3))*5/(4095))/0.01;
}



// - - - = = = # # # SD Card # # # = = = - - -
void sd_init(){
  Serial.print("Initializing SD card...");
  if (!SD.begin(5)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  //Check if folders exist
  if(!SD.exists("/data")) {
    SD.mkdir("/data");
    Serial.println("/data did not exist! Created!");
  } else {
    Serial.println("/data exists!");
  }
  if(!SD.exists("/settings")) {
    SD.mkdir("/settings");
    Serial.println("/settings did not exist! Created!");
  } else {
    Serial.println("/settings exists!");
  }
  if(!SD.exists("/log")) {
    SD.mkdir("/log");
    Serial.println("/log did not exist! Created!");
  } else {
    Serial.println("/log exists!");
  }
  Serial.println("SD init complete!");
}



// Buzzer
void tone_start(){
  tone(buzzer, NOTE_D, 500, 0);
  noTone(buzzer, 0);
}

void tone_success(){
  tone(buzzer, NOTE_C6, 100, 0);
  noTone(buzzer, 0);
  tone(buzzer, NOTE_C7, 100, 0);
  noTone(buzzer, 0);
}

void tone_error(){
  tone(buzzer, NOTE_C6, 300, 0);
  noTone(buzzer, 0);
  tone(buzzer, NOTE_C6, 300, 0);
  noTone(buzzer, 0);
  tone(buzzer, NOTE_C6, 300, 0);
  noTone(buzzer, 0);
}



// Get Time without seconds
String getTimeNoSec(){
  int hour = rtc.getHour(true);
  int min = rtc.getMinute();
  String strhour;
  String strmin;
  if(hour < 10){
    strhour = "0" + (String)hour;
  } else {
    strhour = (String)hour;
  }
  if(min < 10){
    strmin = "0" + (String)min;
  } else {
    strmin = (String)min;
  }
  String full = strhour + ":" + strmin;
  return(full);
}



// I2C Scanner
void Scanner(){
  Serial.println("Scanning for I2C Devices");
  Wire.begin();
  for (byte i = 8; i < 120; i++){
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0){
      Serial.print("Found address: 0x");
      Serial.println(i, HEX);
      i2c_devs++;
    }
  }
}




// SSD1306 OLED Display
void updateDisplay() {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setFont(NULL);
    display.setCursor(0, 0);
    display.print("S1:"); display.println(s1_status);
    display.print("S2:"); display.println(s2_status);
    display.print("S3:"); display.println(s3_status);
    display.print("S4:"); display.println(s4_status);
    display.print("S5:"); display.println(s5_status);
    display.print("S6:"); display.println(s6_status);
    display.print("S7:"); display.println(s7_status);
    display.print("S8:"); display.println(s8_status);
    display.setCursor(32, 0);
    display.print("S9: "); display.println(s9_status);
    display.setCursor(32, 8);
    display.print("S10:"); display.println(s10_status);
    display.setCursor(76, 0);
    display.print("P1:"); display.println(p1_list[0]);
    display.setCursor(76, 8);
    display.print("P2:"); display.println(p2_list[0]);
    display.drawRect(30, 20, 128, 64, 1);
    display.drawLine(68, 20, 128, 20, 0);
    display.drawLine(68, 0, 68, 20, 1);
    display.setCursor(35, 25);
    display.print("T1:"); display.println(t1_temp);
    display.setCursor(35, 33);
    display.print("T2:"); display.println(t2_temp);
    display.setCursor(35, 41);
    display.print("T3:"); display.println(t3_temp);
    display.setCursor(35, 56);
    display.print(WiFi.localIP());
    display.drawLine(30, 53, 128, 53, 1);
    display.display();
}




void mainPins(){
  s1_status = digitalRead(s1);
  s2_status = digitalRead(s2);
  s3_status = digitalRead(s3);
  s4_status = digitalRead(s4);
  s5_status = digitalRead(s5);
  s6_status = digitalRead(s6);
  s7_status = digitalRead(s7);
  s8_status = digitalRead(s8);
  s9_status = digitalRead(s9);
  s10_status = digitalRead(s10);
  resetbtn_status = digitalRead(resetbtn);
}



void main0(){

  if(s6_status && activation_p2 > 0){
    activation_p2--;
    activation_p2_pre = false;
  } else if(!s6_status && activation_p2 < 5 && !active_p2) {
    activation_p2 = 5;
  }

  if (activation_p2 == 0){
    active_p2 = true;
    if(counter_p2 < P2_RUNVALUE){
      if(!activation_p2_pre){
        activation_p2_pre = true;
        digitalWrite(p2, HIGH);
        p2_status = 1;
        write_log("p2 on");
      }

    p2_list[0]++;
    counter_p2++;

    } else {
      digitalWrite(p2, LOW);
      P2_write_PumpTime();
      p2_status = 0;
      write_log("p2 off");
      activation_p2 = 5;
      counter_p2 = 0;
      active_p2 = false;
    }

  }

}




// Main
void main1() {

// ---
  if(s3_status){
    if(s2_status && !s4_status && p1_list[0] < p1_max_time){
      if(!p1_status_pre) {
        write_log("p1 on");
        digitalWrite(p1, HIGH);
        p1_status = 1;
      }
      p1_list[0]++;
      p1_nr++;
      p1_status_pre = p1_status;
      //Serial.println(p1_list[0]); Serial.println(p1_nr);
      p1_status_pre = p1_status;
    } else if(!s2_status || s4_status || p1_list[0] >= p1_max_time) {
      if(p1_status_pre) {
        write_log("p1 off");
        digitalWrite(p1, LOW);
        p1_status = 0;
        P1_write_PumpTime();
      }
      p1_status_pre = p1_status;
    }
  } else {
    Serial.println("S3 ALARM");
    Serial.println("p1 off");
    digitalWrite(p1, LOW);
    p1_status = 0;
  }

}

// Main Reset Button
void main2(){
  if(resetbtn_status && !resetbtn_status_pre){
    resetbtn_status_pre = resetbtn_status;
    write_log("Reset button pressed");
    P1_reset_All();
  } else if(!resetbtn_status && resetbtn_status_pre){
    resetbtn_status_pre = resetbtn_status;
  }
}


// Main Temp !!NOT IN USE!!
void main3(){
  getTempT1();
  getTempT2();
  getTempT3();
  Serial.println("---------");
  Serial.print("T1: "); Serial.println(t1_temp); //write_log("Temperature measured");
  Serial.print("T2: "); Serial.println(t2_temp);
  Serial.print("T3: "); Serial.println(t3_temp);
}


// Main NewDay
void main4(){

  unsigned int newday = rtc.getDayofWeek();
  if(newday != currentday){
    currentday = newday;
    write_log("New Day");
    newDay();
  }

}


// Main WiFi
void main5(){
  if(WiFi.status() != WL_CONNECTED){
    write_log("WiFi Connection lost. Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
    while(WiFi.waitForConnectResult() != WL_CONNECTED){
      Serial.print(".");
    }
  }
}



void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetWidth(12);
  delay(100);
  tone_start();
  delay(100);
  sd_init();
  delay(100);

  Scanner();

  if (sensor.begin() == false){
    Serial.println("No DS18B20 Sensor found");
    tone_error();
    while (!sensor.begin()){
      if (millis() >= 5000){
        break;
      }
    }; // wait until device comes available.
  }
  sensor.setResolution(9);
  sensor.setConfig(DS18B20_CRC);  // or 1
  sensor.requestTemperatures();

  delay(100);

  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    display_available = true;
  }

  // Set IO Pin modes
  pinMode(s1, INPUT);
  pinMode(s2, INPUT);
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  pinMode(s5, INPUT);
  pinMode(s6, INPUT);
  pinMode(s7, INPUT);
  pinMode(s8, INPUT);
  pinMode(s9, INPUT);
  pinMode(s10, INPUT);
  pinMode(t2, INPUT);
  pinMode(t3, INPUT);
  pinMode(resetbtn, INPUT);
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(p1, LOW);
  digitalWrite(p2, LOW);

  // Everything WiFi
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname);
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to "); Serial.println(ssid);

   while (WiFi.status() != WL_CONNECTED) {

  }

  Serial.print("Connected to "); Serial.println(ssid); Serial.print("IP address: "); Serial.println(WiFi.localIP());
  delay(100);

  // Everything Time
  while(!read_Timeshift()){
    Serial.println(".");
  }
  Serial.print("Timeshift: "); Serial.println(timeshift);

  recTime(); // Get current time over NTP
  while(rtc.getYear() < 2021){
    // If NTP fails and year is 1970 -> Do again until time is right
    recTime();
  }

  currentday = rtc.getDayofWeek(); // save todays date (0-6)


  // Web funtions
  server.on("/web", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(SD, "/www/index.html", "text/html");
    request->send(response);
  });


  server.on("/api/values", HTTP_GET, [](AsyncWebServerRequest *request){

      doc["s1"] = s1_status;
      doc["s2"] = s2_status;
      doc["s3"] = s3_status;
      doc["s4"] = s4_status;
      doc["s5"] = s5_status;
      doc["s6"] = s6_status;

      doc["s7"] = s7_status;
      doc["s8"] = s8_status;
      doc["s9"] = s9_status;
      doc["s10"] = s10_status;

      doc["p1"] = p1_status;
      doc["p1_time"] = p1_list[0];
      doc["p1_time_reset"] = p1_nr;
      doc["p2"] = p2_status;
      doc["p2_time"] = p2_list[0];

      doc["t1"] = t1_temp;

      serializeJson(doc, buffer);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", buffer);
    request->send(response);
  });


  server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request){

    sett["p1_max_time"] = p1_max_time;
    sett["timeshift"] = timeshift;

    serializeJson(sett, settbuffer);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", settbuffer);
    request->send(response);
  });


  server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest *request){

    sys["localtime"] = getTimeNoSec();
    sys["localepoch"] = rtc.getEpoch();
    sys["uptime"] = (millis()/1000);
    sys["rssi"] = WiFi.RSSI();
    sys["t2"] = t2_temp;
    sys["t3"] = t3_temp;
    sys["i2c"] = i2c_devs;
    sys["pcb"] = HARDWARE_VERSION;
    sys["firmware"] = SOFTWARE_VERSION;


    serializeJson(sys, sysbuffer);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", sysbuffer);
    request->send(response);
  });



  server.on("/api/stats/p1", HTTP_GET, [](AsyncWebServerRequest *request){

    for(int i=0; i<7; i++){
      statistics[i] = p1_list[i];
    }

    serializeJson(statistics, statsbuffer);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", statsbuffer);
    request->send(response);
  });


  server.on("/api/stats/p2", HTTP_GET, [](AsyncWebServerRequest *request){

    for(int i=0; i<7; i++){
      statistics[i] = p2_list[i];
    }

    serializeJson(statistics, statsbuffer);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", statsbuffer);
    request->send(response);
  });


  server.on("/api/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    P1_reset_PumpTime();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    request->send(response);
  });


  server.on("/api/resetall", HTTP_GET, [](AsyncWebServerRequest *request){
    P1_reset_All();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    request->send(response);
  });


  server.on("/api/write/maxpumptime", HTTP_GET, [](AsyncWebServerRequest *request){

    AsyncWebParameter* p = request->getParam(0);
    const String pin = p->name();
    const String tempvalue = p->value();

    if(pin == "val") write_MaxPumpTime(tempvalue.toInt());

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    request->send(response);
  });


  server.on("/api/write/timeshift", HTTP_GET, [](AsyncWebServerRequest *request){

    AsyncWebParameter* p = request->getParam(0);
    const String pin = p->name();
    const String tempvalue = p->value();

    if(pin == "val") write_Timeshift(tempvalue.toInt()); // 1 or 0

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
    request->send(response);
  });


  server.serveStatic("/", SD, "/www/").setDefaultFile("index.html");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Private-Network", "true");
  DefaultHeaders::Instance().addHeader("Access-Control-Request-Private-Network", "true");
  server.begin();                  // Start server
  Serial.println("HTTP server started");



  /* Auth Code

    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();

  const char* http_username = "admin";
  const char* http_password = "admin";

  */



  // Startup
  Serial.print("Time: "); Serial.println(rtc.getTime());
  P1_read_PumpTime();
  P2_read_PumpTime();
  Serial.println("P1 List: "); P1_display();
  Serial.println("P2 List: "); P2_display();
  read_MaxPumpTime();
  Serial.print("P1 Max Time: "); Serial.println(p1_max_time);
  delay(2000);
  write_log("Initialisation successful!");



  t.setInterval(mainPins, 100);
  t.setInterval(main0, 1000);
  t.setInterval(main1, 1000);
  t.setInterval(main2, 1000);

  t.setInterval(getTempT1, 3000);
  t.setInterval(getTempT2, 1000);
  t.setInterval(getTempT3, 1000);

  t.setInterval(main4, 1000);
  t.setInterval(main5, 5000);

  t.setInterval(updateDisplay, 1000);



  tone_success();
  booted = true;

}



void loop() {
  t.handle();
}
