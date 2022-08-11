int measurePin = A0;
int ledPower = 4;
 
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
 
void setup(){
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
  }
 
void loop(){
  digitalWrite(ledPower,LOW);
  delayMicroseconds(280);

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
 
  Serial.println("Raw Signal Value (0-1023):");
  Serial.println(voMeasured);

  Serial.println("Voltage:");
  Serial.println(calcVoltage);

  Serial.println("Dust Density:");
  Serial.println(dustDensity);
  delay(1000);
  }
