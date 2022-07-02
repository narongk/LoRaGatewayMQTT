// Import Wi-Fi library
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

#include <SPIFFS.h>

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

// Libraries to get time from NTP Server
#include <NTPClient.h>
#include <WiFiUdp.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 433E6

// Replace with your network credentials
//const char* ssid     = "home2G_8225";
//const char* password = "tao@45231538";

const char *Apssid = "Anything you like";     //Give AccessPoint name whatever you like. (this will be Name of your esp32 HOTSPOT)
const char *Appassword = "123456789";         //Password of your Esp32's hotspot,(minimum length 8 required)


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String day;
String hour;
String timestamp;


// Initialize variables to get and save LoRa data
int rssi;
String loRaMessage;
String data1;
String data2;
String data3;
String key;
String dataa;

String readingID;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with values
String processor(const String& var){
  //Serial.println(var);
  if(var == "DATA1"){
    return data1;
  }
  else if(var == "DATA2"){
    return data2;
  }
  else if(var == "DATA3"){
    return data3;
  }
  else if(var == "TIMESTAMP"){
    return timestamp;
  }
  else if (var == "RRSI"){
    return String(rssi);
  }
  return String();
}

//Initialize LoRa module
void startLoRA(){
  int counter;
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
}

void connectWiFi(){
  // Connect to Wi-Fi network with SSID and password
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);                    // Changing ESP32 wifi mode to AccessPoint

  // You can remove the Appassword parameter if you want the hotspot to be open.
  WiFi.softAP(Apssid, Appassword);      //Starting AccessPoint on given credential
  IPAddress myIP = WiFi.softAPIP();     //IP Address of our Esp8266 accesspoint(where we can host webpages, and see data)
  Serial.print("AP IP address: ");
  Serial.println(myIP);                //Default IP is 192.168.4.1
  Serial.println("Scan For Wifi in your Mobile or laptop, you will see this network");

//  Serial.print("Connecting to ");
//  Serial.println(ssid);
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//  // Print local IP address and start web server
//  Serial.println("");
//  Serial.println("WiFi connected.");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());

}

// Read LoRa packet and get the sensor readings
void getLoRaData() {
  Serial.print("Lora packet received: ");
  // Read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // LoRaData format: readingID/temperature&soilMoisture#batterylevel
    // String example: 1/27.43&654#95.34
    Serial.println(LoRaData); 
    
    // Get readingID, 
    int pos1 = LoRaData.indexOf('/');
    int pos2 = LoRaData.indexOf(':');
    //int pos3 = LoRaData.indexOf('#');
    readingID = LoRaData.substring(0, pos1);
    key = LoRaData.substring(pos1 +1, pos2);
    dataa = LoRaData.substring(pos2+1, LoRaData.length());
    //data3 = LoRaData.substring(pos3+1, LoRaData.length());
    if(key=="data1"){data1=dataa;}
    if(key=="data2"){data2=dataa;}
    if(key=="data3"){data3=dataa;}  
    
    Serial.print("DATA1: ");    
    Serial.println(data1);
    Serial.print("DATA2: ");    
    Serial.println(data2);
    Serial.print("DATA3: ");    
    Serial.println(data3);        
  }
  // Get RSSI
  rssi = LoRa.packetRssi();
  Serial.print(" with RSSI ");    
  Serial.println(rssi);
}

// Function to get date and time from NTPClient
void getTimeStamp() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  day = formattedDate.substring(0, splitT);
  Serial.println(day);
  // Extract time
  hour = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.println(hour);
  timestamp = day + " " + hour;
}

void setup() { 
  // Initialize Serial Monitor
  Serial.begin(115200);
  connectWiFi();
  startLoRA();
  //connectWiFi();
  
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/data1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", data1.c_str());
  });
  server.on("/data2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", data2.c_str());
  });
  server.on("/data3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", data3.c_str());
  });
  server.on("/timestamp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", timestamp.c_str());
  });
  server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(rssi).c_str());
  });
  server.on("/winter", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/winter.jpg", "image/jpg");
  });
  // Start server
  server.begin();
  
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(25200);
}

void loop() {
  // Check if there are LoRa packets available
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    getLoRaData();
    getTimeStamp();
  }

}
