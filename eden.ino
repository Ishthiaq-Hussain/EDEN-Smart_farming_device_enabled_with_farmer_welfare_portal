//#include <FirebaseESP32.h>






#include <WiFi.h>                                                    // esp8266 library
#include <SoftwareSerial.h>
#include <IOXhop_FirebaseESP32.h>                                                // firebase library
#include <DHT.h>                                                            // dht11 temperature and humidity sensor library

#define FIREBASE_HOST "eden-database.firebaseio.com"                          // the project name address from firebase id
#define FIREBASE_AUTH "5quXnqe59W3vq7zn4yrENq7MXcN5JphvDcf7vBl0"            // the secret key generated from firebase

const char* WIFI_SSID="Madhu";                                             // input your home or public wifi name 
const char* WIFI_PASSWORD="12345678";                                   //password of wifi ssid
 
#define DHTPIN 27                                                           // what digital pin we're connected to
#define DHTTYPE DHT11                                                       // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE); 
const int sensor_pin = 26;
const int ldr = 13 ;
int val; 
int sensorValue = 0; 
const int pin = 12;  // Analog input pin that the potentiometer is attached to  


  
float sensor_volt; //Define variable for sensor voltage 
float RS_air; //Define variable for sensor resistance
float R0; //Define variable for R0
float sensorValue1;
float sensorValue2;//Define variable for analog readings
float m = -0.379; //Slope 
float b = 0.837; //Y-Intercept
float sensor_volt1;
float RS_gas; //Define variable for sensor resistance  
float ratio;  



//fuction to find the R0 value
 void value() { 
   
  for(int x = 0 ; x < 500 ; x++) //Start for loop 
  {
    sensorValue1 = sensorValue1 + analogRead(pin); //Add analog values of sensor 500 times 
  }
  sensorValue1 = sensorValue1/500.0; //Take average of readings
  sensor_volt = sensorValue1*(5.0/1023.0); //Convert average to voltage 
  RS_air = ((5.0*10.0)/sensor_volt)-10.0; //Calculate RS in fresh air 
  R0 = RS_air/9.5; //Calculate R0 
 
  Serial.print("R0 = "); //Display "R0"
  Serial.println(R0); //Display value of R0 
  
  return ;
}

void setup() {
  Serial.begin(115200);
  delay(1000);                
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                              // connect to firebase
  dht.begin(); 
  value();
   //Start reading dht sensor
}

void loop() { 
  sensorValue2 = analogRead(pin); //Read analog values of sensor  
  sensor_volt1 = sensorValue2*(5.0/1023.0); //Convert analog values to voltage 
  RS_gas = ((5.0*10.0)/sensor_volt1)-10.0; //Get value of RS in a gas
  ratio = RS_gas/R0; 
 // Get ratio RS_gas/RS_air
 
  double ppm_log = (log10(ratio)-b)/m; //Get ppm value in linear scale according to the the ratio value  
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale 
  double percentage = ppm/100; //Convert to percentage 
  
  float moisture_percentage;

  moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
  val=analogRead(ldr);
  
  float h = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
  float t = dht.readTemperature();                                           // Read temperature as Celsius (the default)
    
  if (isnan(h) || isnan(t)) {                                           // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  Serial.print("Humidity =  ");  Serial.print(h);Serial.println("%");
  String fireHumid = String(h) + String("%");                                         //convert integer humidity to string humidity 
  Serial.print("Temperature = ");  
  Serial.print(t);  
  Serial.println("°C ");
  String fireTemp = String(t) + String("Â°C");                                                     //convert integer temperature to string temperature
  delay(4000);
  Serial.print("Soil Moisture(in Percentage) = ");
  Serial.print(moisture_percentage);
  Serial.println("%");
  Serial.print("light intensity = ");
  Serial.println(val);
  String firemoist = String(moisture_percentage) + String("%");
  String fireldr = String(val);
  Serial.println(ratio);
  //Serial.print("percentage:");
  //Serial.print(percentage);
  Serial.print("Co2 concentration in air= ");
  Serial.println(ppm);
  String firegas = String(ppm) + String("ppm");
  
  
  
 Firebase.push("/eden/humidity", fireHumid );
 Firebase.push("/eden/temperature", fireTemp);
 Firebase.push("/eden/soil_moisture", firemoist );
 Firebase.push("/eden/light_intensity", fireldr);
 Firebase.push("/eden/Co2_concentration", firegas);
}
