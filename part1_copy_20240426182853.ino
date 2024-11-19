#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>


const char *ssid = "Koushik";
const char *pass = "123456789";
const int sensorIn = A0; //analog pin# A0;
int mVperAmp = 100; 
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
int decimalPrecision=3;
DHT dht(D5, DHT11);
#define relay_pin 4
#define relay_pi 0




WiFiClient client;

long myChannelNumber = 2521861;
const char myWriteAPIKey[] = "90B36YO59SRQNCUQ";

float getVPP()
{
float result;
int readValue; //value read from the sensor
int maxValue = 0; // store max value here
int minValue = 1024; // store min value here
uint32_t start_time = millis();
while((millis()-start_time) < 3000) //sample for 3 Sec
{
readValue = analogRead(sensorIn);
// see if you have a new maxValue
if (readValue > maxValue)
{maxValue = readValue;}
if (readValue < minValue)
{minValue = readValue;}
}
result = ((maxValue-minValue)*5.0)/1024.0;
return result;
}

void setup() {
  Serial.begin(115200);
  pinMode(relay_pin , OUTPUT);
  pinMode(relay_pi , OUTPUT);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());

  dht.begin();
  ThingSpeak.begin(client);

  
}

void loop() {
  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707; //root 2 is 0.707
  AmpsRMS = (VRMS * 1000)/mVperAmp;
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.println("Temperature: " + (String)(temperature));
  Serial.println("Humidity: " + (String)humidity);
  Serial.println("Current Sensor Value: " + (String)AmpsRMS);

  ThingSpeak.writeField(myChannelNumber, 1, temperature, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);
  if ((temperature)<25){
     digitalWrite(relay_pin, HIGH);
     digitalWrite(relay_pi, LOW);
     delay(100);
  }
  if ((temperature)>35){
     digitalWrite(relay_pin, LOW);
     digitalWrite(relay_pi, HIGH);
     delay(100);
  }
  
 
  delay(10); // Wait for 5 seconds before next reading
}
