#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>
#include "MQ135.h"
#define DHTPIN 0          //pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);
WiFiClient client;
#include "ThingSpeak.h"
const char* ssid = "latif";   // your network SSID (name) 
const char* password = "innovation";   // your network password

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "U602C80RJTWFN7V8";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Variable to hold temperature readings
#define rly 14
unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

int measurePin = A0;
int ledPower = 4;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
float t;
float h;
String myStatus = "";

void setup() {
  Serial.begin(115200);  //Initialize serial
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  dht.begin();
        pinMode(ledPower,OUTPUT);
        pinMode(rly,OUTPUT);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }

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
  //delay(12000);                       // wait for a second
  delay(2000);
  digitalWrite(rly, HIGH);    
  MQ135 gasSensor = MQ135(A0);
      float CO2 = gasSensor.getPPM();
      delay(1000);
      Serial.print("CO2: ");  
      Serial.print(CO2);
      Serial.print("  PPM");   
      Serial.println();
      Serial.println(); 
  delay(2000); 
  digitalWrite(rly,LOW);
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
//setting thingspeak fields data
      ThingSpeak.setField(1, t);
      ThingSpeak.setField(2, h);
      ThingSpeak.setField(3, CO2);
      ThingSpeak.setField(4, dustDensity);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}
