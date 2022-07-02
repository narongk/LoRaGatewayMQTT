//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
//Libraries for modbus
#define ESP32_CONTROL_10RS
#include <ESP32Control.h>

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


//packet counter
int readingID = 0;

int counter = 0;
String LoRaMessage = "";

float data1 = 0;
float data2 = 0;
float data3 = 0;

ModbusMaster232 myMaster(1,100,3);      //- พารามิเตอร์ (Serial1,Timeout,Retry)  Timeout คือเวลาที่รอการตอบกลับจาก Slave ,Retry คือถ้าอ่านค่าไม่ได้ให้ทดลองอีกกี่ครั้ง
const int RX2   = 12;                   //- RX pin ของ rs485 
const int TX2   = 13;                   //- TX pin ของ rs485 
int lastTime      = 0;
const int peroid  = 1000;

//Initialize LoRa module
void startLoRA(){
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
    readingID++;
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
}

void getReadings(){
//  temperature = bme.readTemperature();
//  humidity = bme.readHumidity();
//  pressure = bme.readPressure() / 100.0F;
/*-----------------------------------------------------------------------------------
  readCoilsB(uint8_t,uint16_t);                             //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการอ่านค่า)                          - คืนค่าเป็น 0,1,FF(เมื่ออ่านค่าไม่ได้)
  readCoilsU(uint8_t,uint16_t,uint16_t);                    //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการอ่านค่า ,จำนวน bit ที่ต้องการอ่านค่า)  - คืนค่าเป็น word
  readDiscreteInputsB(uint8_t,uint16_t,uint8_t);            //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการอ่านค่า)                          - คืนค่าเป็น 0,1,FF(เมื่ออ่านค่าไม่ได้)
  readDiscreteInputsU(uint8_t,uint16_t,uint16_t);           //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการอ่านค่า ,จำนวน bit ที่ต้องการอ่านค่า)  - คืนค่าเป็น word
  writeSingleCoilB(uint8_t,uint16_t, uint8_t);              //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการเขียนค่า)
  readHoldingRegistersI(uint8_t,uint16_t);                  //พารามิเตอร์ (slave address ,address)                                            - คืนค่าเป็น word
  readHoldingRegistersIn(uint8_t,uint16_t,uint16_t,uint16_t*); //พารามิเตอร์ (slave address ,address ,จำนวน word ที่ต้องการ ,ตัวแปรที่ต้องการเก็บค่า)
  readHoldingRegistersF(uint8_t,uint16_t);                  //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง
  readHoldingRegistersFI(uint8_t,uint16_t);                 //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง แบบกลับ word
  readHoldingRegistersFAI(uint8_t,uint16_t);                //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง แบบกลับ ทั้ง 4 byte
  readInputRegistersF(uint8_t,uint16_t);                    //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง
  readInputRegistersFI(uint8_t,uint16_t);                   //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง แบบกลับ word
  writeSingleRegisterF(uint8_t,uint16_t, float);            //พารามิเตอร์ (slave address ,address ,จำนวนที่ต้องการเขียน(float)) 
  writeSingleRegisterFI(uint8_t,uint16_t, float);           //พารามิเตอร์ (slave address ,address ,จำนวนที่ต้องการเขียน(float)) แบบกลับ word
  writeSingleRegisterFAI(uint8_t,uint16_t, float);          //พารามิเตอร์ (slave address ,address ,จำนวนที่ต้องการเขียน(float))  แบบกลับ ทั้ง 4 byte
  writeSingleRegisterI(uint8_t,uint16_t,int16_t);
-----------------------------------------------------------------------------------*/
if(millis() - lastTime > peroid){                     //- อ่านค่าทุกๆ 1 วินาที
    lastTime = millis();
    data1 = myMaster.readHoldingRegistersI(1,8);
    data1 =data1/100;
    //delay(1000); 
    sendReadings("data1",data1);
    data2 = myMaster.readHoldingRegistersI(1,9);
    data2 =data2/100;
    //delay(1000); 
    sendReadings("data2",data2);
    data3 = myMaster.readHoldingRegistersI(1,10);
    data3 =data3/100;
    //delay(1000);  
    sendReadings("data3",data3);    
    //myMaster.writeSingleRegisterI(1,512,2);
  }
  Serial.print("DATA1: ");
  Serial.println(data1);
  Serial.print("DATA2: ");
  Serial.println(data2);
  Serial.print("DATA3: ");
  Serial.println(data3);  
}

void sendReadings(String key,float data) {
  LoRaMessage = String(readingID) + "/" + key + ":" + String(data);
  Serial.println(LoRaMessage);
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  Serial.print("Sending packet: ");
  Serial.println(readingID);
  readingID++;
}

//void sendReadings() {
//  LoRaMessage = String(readingID) + "/" + String(data1) + "&" + String(data2) + "#" + String(data3);
//  Serial.println(LoRaMessage);
//  //Send LoRa packet to receiver
//  LoRa.beginPacket();
//  LoRa.print(LoRaMessage);
//  LoRa.endPacket();
//  Serial.print("Sending packet: ");
//  Serial.println(readingID);
//  readingID++;
//}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial1.begin(9600,SERIAL_8N1,RX2,TX2); //- ตั้งค่า Serial อันนี้สำคัญมาก Serial1 นี้ต้องสั่ง Begin ตรงนี้ไม่งั้นจะใช้ Modbus อ่านค่าไม่ได้
  startLoRA();
}
void loop() {
  getReadings();
  sendReadings();
  delay(1000);
}
