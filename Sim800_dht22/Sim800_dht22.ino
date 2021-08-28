#include<SoftwareSerial.h>
#include "DHT.h"
 
 
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
 
SoftwareSerial mySerial(8, 7);
 
int index = 0;
float h;
float temp;
 
 
char incomingByte;
String incomingData;
bool atCommand = true;
String message = "";
String number = "";
 
void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  dht.begin();
 
  while (!mySerial.available()) {
    mySerial.println("AT");
    delay(1000);
    Serial.println("connecting....");
  }
 
  mySerial.println("AT+CMGF=1");  //Set SMS Text Mode
  delay(1000);
  mySerial.println("AT+CNMI=1,2,0,0,0");  //procedure, how to receive messages from the network
  delay(1000);
  //mySerial.println("AT+CMGL=\"REC UNREAD\""); // Read unread messages
  mySerial.println("AT+CBAND=DCS_MODE"); 
  Serial.println("Ready to receive Commands..");
}
 
void loop()
{
  
  
  if (mySerial.available()) {
    delay(100);
    // Serial buffer
    while (mySerial.available()) {
      incomingByte = mySerial.read();
      incomingData += incomingByte;
    }
    delay(10);
    if (atCommand == false) {
      receivedMessage(incomingData);
 
    }
    else {
      atCommand = false;
    }
    //delete messages to save memory
    if (incomingData.indexOf("OK") == -1) {
      mySerial.println("AT+CMGDA=\"DEL ALL\"");
      delay(1000);
      atCommand = true;
    }
    incomingData = "";
  }
  if (message.indexOf("SEND") > -1) {
    SendTextMessage();
    Serial.println("send");
  }
}
 
 
void SendTextMessage()
{
  
 
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
 
  mySerial.println("AT+CMGF=1");    //To send SMS in Text Mode
  delay(1000);
  mySerial.println("AT+CMGS=\"0832144571\"\r"); // change to the phone number you using
  delay(1000);
  mySerial.print("Humidity: ");
  mySerial.print(h);
  mySerial.print(" % ");
  mySerial.print("Temperature: ");
  mySerial.print(temp);
  mySerial.println(" *C ");
  delay(200);
  mySerial.println((char)26);//the stopping character
  delay(1000);
 
}
void receivedMessage(String inputString) {
 
  //Get The number of the sender
  index = inputString.indexOf('"') + 1;
  inputString = inputString.substring(index);
  index = inputString.indexOf('"');
  number = inputString.substring(0, index);
  Serial.println("Number: " + number);
 
  //Get The Message of the sender
  index = inputString.indexOf("\n") + 1;
  message = inputString.substring(index);
  message.trim();
  Serial.println("Message: " + message);
  message.toUpperCase(); // uppercase the message received
  Serial.println("receive");
 
  delay(50);
}
