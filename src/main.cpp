#include <Arduino.h> // Initialising Arduino library.
#include <WiFi.h>    // Initialses WiFi connection library.
#include <PubSubClient.h> // Initialises PubSubClient to utilse mqtt communication protocol.
#include <Adafruit_Sensor.h> // Initialising Adafruit sensor library.
#include "DHT.h" // Initialising DHT sensor library.
#include <DHT_U.h>
#include <string.h> // Initialises string libraries for string conversion functions.
#include <stdlib.h> //Initialising standard C/C++ library.

#define DHTTYPE DHT11  // Defining sensor type as DHT11 for temperature and humidity sensor.
#define DHTPIN 32  // Defining ESP32 digital pin for sensor readings.
// Declaring network id and password for wifi network as global constant variables.
const char* ssid = "Industrial-Hub";
const char* password = "Dissertation-IIoT";
// Declaring ip address for mqtt broker aswell as the topics it needs to publish.
const char* mqtt_broker = "REDACTED";
const char* topic_temp = "Temperature";
const char* topic_hum = "Humidity";
// Declaring the username and password to publish topic data to the designated mqtt broker.
const char* broker_username = "Industry";
const char* broker_password = "Four";

// Creating a new classification to utilise the PubSubClient functionality
WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE); // Calling DHT sensor function

/*
 Function which connects the ESP32 to the Wifi router and thus the factory LAN network.
 If connection isn't establsihed by a designated time fraem then the program will
 terminate.
*/
void WiFi_setup()
{
  delay(200);
  Serial.print("Connecting to  ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int time_out = 0;

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("-");
    delay(100);
    time_out += 100;
    if(time_out == 5000)
    {
      Serial.println("Connection Failed");
      time_out = 0;
    }
  }
  Serial.println("Connection Established");
}

/* 
  Function which reconnects to wifi network and mqtt broker in the event of a
  connection failure.
*/
void reconnect()
{
  while(!client.connected())
  {
    Serial.print("Connecting to ");
    Serial.println(mqtt_broker);

    if(client.connect(mqtt_broker))
    {
      Serial.print("Connected to ");
      Serial.println(mqtt_broker);
    }
    else
    {
      Serial.println("Trying to Connect");
      delay(5000);
    }
  }
}

/*
  Function to for intitial setup procedures of ESP32
*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Initialing serial port with a baud rate of 115200 with the ESP32.
  pinMode(32, INPUT);
  WiFi_setup(); // Calls WiFi setup function. 
  client.setServer(mqtt_broker, 1883); // Function call to connect to mqtt broker using the appropriate port number.
  dht.begin(); // Initialising communication between the ESP32 and the DHT11 temperature and humidity sensor.
}

/*
  Function for main program of ESP32.
*/
void loop() {
  // put your main code here, to run repeatedly:
  WiFi_setup(); // Calling WiFi setup function.

  // Connects ESP32 to mqtt broker using username and password.
  client.connect(mqtt_broker, broker_username, broker_password);

  // Calls reconnect function if client fails to connect to mqtt broker.
  if(!client.connected())
  {
    reconnect();
  }

  // Initialising a float variable and storing the temperature reading from the DHT11 sensor.
  float t = dht.readTemperature(); 

  char ts[100]; // character array for storing temperature in string format.

  dtostrf(t, 5, 3, ts); // conversion of float to string for mqtt tranmission. 

  // Initialising a float variable and storing the humidity reading from the DHT11 sensor.
  float h = dht.readHumidity();

  char hs[100]; // character array for storing humidity in string format.

  dtostrf(h, 5, 3, hs); // conversion of float to string for mqtt tranmission.

  client.publish(topic_temp, ts); // Publishes temperature data to mqtt broker.

  client.publish(topic_hum, hs); // Publishes humidity data to mqtt broker.

  Serial.println(t);
}

