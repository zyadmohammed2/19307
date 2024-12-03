/*
  visit www.ArduinoYard.com for more details
*/
#define vibration_pin 13

volatile int vibration_count = 0;
int currentstate = LOW;
unsigned long lastVibHighMillis = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 2000;

const int timeDelay = 5;
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#include <HTTPClient.h>
#define On_Board_LED_PIN  2
String Web_App_URL = "https://script.google.com/macros/s/AKfycbxIG4c0di0UIQyXixK020vBbcBEaPWAJ7RsI2mJaQnXTMIXYhUIA4HrQGEfVG-eyloO/exec";


/************************* DHT Sensor Setup *********************************/
#define MQ7_ANALOG_PIN 34  // Analog pin connected to the MQ-7

// Calibration variables
float sensor_voltage;   // Voltage read from the sensor
float COppm;           // Calculated CO in ppm
const float R_LOAD = 10.0; // Load resistor value in kΩ
const float R_ZERO = 10.0; // Sensor resistance in clean air (kΩ)
const float CO_CURVE[3] = {2.8, 0.85, -0.45}; // Curve coefficients (calibrated for MQ-7)

#define DHTPIN 26    // Digital pin connected to the DHT sensor
#define DHTTYPE  DHT11  // Type of DHT Sensor, DHT11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

    // Connect LED on pin D3

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "MalakAshraf"   // Replace with your Wi-Fi SSID
#define WLAN_PASS       "eldolphino@Aa"   // Replace with your Wi-Fi Password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                                 // use 8883 for SSL
#define AIO_USERNAME    "mind0shaft"                        // Replace with your Adafruit IO Username
#define AIO_KEY         "aio_Evhv63wCHTY4QBeNIZPSmRcUVZZW"   // Replace with your Adafruit IO Key

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup feeds called 'temp' & 'hum' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
// feedname should be the same as set in dashboard for temp and hum gauges
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/DHT11");  // feedname
Adafruit_MQTT_Publish Coppm = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/coppm");  // feedname
Adafruit_MQTT_Publish vibrate = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/vibration");  // feedname

// Setup a feed called 'onoff' for controlling LED.

void MQTT_connect();

int counter;

void setup() {
  Serial.begin(115200);
  delay(10);
   pinMode(MQ7_ANALOG_PIN, INPUT);
  pinMode(On_Board_LED_PIN, OUTPUT);
  pinMode(vibration_pin, INPUT);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
 int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(On_Board_LED_PIN, HIGH);
    delay(250);
    digitalWrite(On_Board_LED_PIN, LOW);
    delay(250);
    if (connecting_process_timed_out > 0) connecting_process_timed_out--;
    if (connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }
  digitalWrite(On_Board_LED_PIN, LOW);
  Serial.println("WiFi connected");
  // Setup MQTT subscription for onoff feed.
 
  dht.begin();
}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(On_Board_LED_PIN, HIGH);
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
int analog_value = analogRead(MQ7_ANALOG_PIN);
  sensor_voltage = analog_value * (3.3 / 4095.0); // Scale to ESP32 ADC voltage
  float RS = (3.3 - sensor_voltage) / sensor_voltage * R_LOAD;

  // Calculate ppm using the sensor curve
  COppm = pow(10, (log10(RS / R_ZERO) - CO_CURVE[1]) / CO_CURVE[2] + CO_CURVE[0]);

  Serial.print("CO (ppm): ");
  Serial.println(COppm);

  delay(1000);
    unsigned long currentmillis = millis();

  if(currentmillis - previousMillis >= interval)
  previousMillis = currentmillis;

  currentstate = digitalRead(vibration_pin);

  if(currentstate == LOW)
  {
    vibration_count++;
    lastVibHighMillis = currentmillis;
  }

  else if(currentmillis - lastVibHighMillis > 2000)
  {
    vibration_count = 0;
  }

  Serial.print("Vibrations: "); Serial.println(vibration_count);
}

  Adafruit_MQTT_Subscribe *subscription;

  // Wait 2000 milliseconds, while we wait for data from subscription feed. After this wait, next code will be executed
 
float t = dht.readTemperature();
    float T = t;
    float s = COppm;
    float S = s;
    float v = vibration_count;
    float V = v ;
  Serial.print(F("  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
    String Send_Data_URL = Web_App_URL + "?sts=write" + "&temp=" + T + "&vibrate=" + V + "&cot=" + S;
HTTPClient http;
    // HTTP GET Request.
    http.begin(Send_Data_URL.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    // Gets the HTTP status code.
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);
    // Getting response from google sheets.
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload); 
    }
     http.end();
    
    digitalWrite(On_Board_LED_PIN, LOW);
}
auto time = millis();
  while (time % (timeDelay * 1000) >= 50) {
    time = millis();
    delay(2);
  }
  // DHT11 Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

 
  Serial.print(F("  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  // Now we can publish stuff!


  
  Serial.print(F("\nSending Temperature val "));
  if (! temp.publish(t)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }


Serial.print(F("\nSending co concentration "));
  if (! Coppm.publish(COppm)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending co concentration "));
  if (! vibrate.publish(vibration_count)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
    if(! mqtt.ping()) {
    mqtt.disconnect();
    }
  

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
