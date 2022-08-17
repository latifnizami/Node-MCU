#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>
#include "MQ135.h"
String apiKey = "DH5E3Y7X0X1NB877";     //  Enter your Write API key from ThingSpeak
const char *ssid =  "innovation";     // replace with your wifi ssid and wpa2 key
const char *pass =  "latifwifi";
const char* server = "api.thingspeak.com";
#define DHTPIN 0          //pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);
WiFiClient client;

#define rly 14
unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

int measurePin = A0;
int ledPower = 4;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup() 
{
        Serial.begin(9600); // Baudrate to display data on serial monitor
        dht.begin();
        pinMode(ledPower,OUTPUT);
        pinMode(rly,OUTPUT);
       Serial.println("Connecting to ");  
       Serial.println(ssid);
       WiFi.begin(ssid, pass);

      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");

}

void loop() 
{
  voMeasured = analogRead(measurePin);
  delayMicroseconds(40);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(9680);

  calcVoltage = voMeasured*(5.0/1024);
  dustDensity = 0.17*calcVoltage-0.1;

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }

//  Serial.println("Raw Signal Value (0-1023):");
//  Serial.println(voMeasured);
//
//  Serial.println("Voltage:");
//  Serial.println(calcVoltage);

  Serial.print("Dust Density:");
  Serial.print(dustDensity);
  Serial.print("    Ug/m3");
  delay(1000);
  Serial.println(); 
  Serial.println();   
  delay(12000);                       // wait for a second
  digitalWrite(rly, LOW);    
  MQ135 gasSensor = MQ135(A0);
      float CO2 = gasSensor.getPPM();
      Serial.print("CO2: ");  
      Serial.print(CO2);
      Serial.print("  PPM");   
      Serial.println();
      Serial.println(); 
  delay(2000); 
  digitalWrite(rly,HIGH);
  digitalWrite(ledPower,LOW);
  delayMicroseconds(280);


      float h = dht.readHumidity();
      float t = dht.readTemperature();


       if (isnan(h) || isnan(t)) 
                 {
                 Serial.println("Failed to read from DHT sensor!");
                 return;
                 }
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print(" degrees Celcius ");
        Serial.println();
        Serial.println();
        Serial.print("Humidity : ");
        Serial.print(h);
        Serial.print(" %");
        Serial.println();
        Serial.println();
        delay(2000);
                   if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  

                            String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(CO2);
                             postStr +="&field4=";
                             postStr += String(dustDensity);
                             postStr += "\r\n\r\n";

                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);

                        }
         client.stop();

        Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates
  delay(15000);
}
