

/*
********************************************
14CORE ULTRASONIC DISTANCE SENSOR CODE TEST
NODE MCU .9 esp 8266 and hc-sr04 distance sensor
********************************************
*/


#include <SD.h>
#include <wire.h>


// need this lib for Secure SSL for ESP 8266 chip
#include <WiFiClientSecure.h>  


// common include file with additional user functions ise 
// To use tabs with a .h extension, you need to #include it (using "double quotes" not <angle brackets>).                    
#include "Common.h"
#include <ESP8266WiFi.h>

// need this lib for Secure SSL for ESP 8266 chip
#include <WiFiClientSecure.h>  


// NodeMCU Pin D1 > TRIGGER | Pin D2 > ECHO
#define TRIGGER 5
#define ECHO    4


String netid, pwd, deviceId, url;
long duration, distance, lastDistance;

String passData[4];


void setup() {

	Serial.begin(9600);

	Serial.println("Setup Distance Sensor");
	pinMode(TRIGGER, OUTPUT);
	pinMode(ECHO, INPUT);

	Wire.pins(0, 2);
	//lcd.begin(16, 2);
	//lcd.println("hello");


	// get data from sd card
	// passing an array to house sd card information
	getSDData(passData);

	// move sd card data to global variables
	netid = passData[0];
	pwd = passData[1];
	deviceId = passData[2];
	url = passData[3];

	// verify variables from sd card got into globals
	Serial.print("NETID:");
	Serial.println(netid);
	Serial.print("PWD:");
	Serial.println(pwd);
	Serial.print("DEVICEID:");
	Serial.println(deviceId);
	Serial.print("URL:");
	Serial.println(url);

	// initialize wifi
	WiFi.disconnect();

	WiFi.begin((const char*)netid.c_str(), (const char*)pwd.c_str());
	
	Serial.print("Connecting to SSID:");
	Serial.println(WiFi.SSID());
	Serial.println(WiFi.macAddress());

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");

		switch (WiFi.status())
		{
		
		case WL_CONNECTION_LOST:
			Serial.println("Connection Lost");
			break;
		case WL_CONNECT_FAILED:
			Serial.println("Connection Failed");
			break;
		case WL_DISCONNECTED:
			Serial.println(" Not Connected");
			break;
		default:
			Serial.print("Status:");
			Serial.println(WiFi.status());
			break;
		}

		
	}

	Serial.print("Connected IP = ");
	Serial.println(WiFi.localIP());

	// start time client - used to get current time.
	timeClient.begin();

}

void loop() {

	long duration, distance;
	digitalWrite(TRIGGER, LOW);
	delayMicroseconds(2);

	digitalWrite(TRIGGER, HIGH);
	delayMicroseconds(10);

	digitalWrite(TRIGGER, LOW);
	duration = pulseIn(ECHO, HIGH);
	distance = (duration / 2) / 29.1;

	Serial.print(distance);
	Serial.println("Centimeter:");

	String distanceJson = createJsonData("dist01", distance);
	Serial.println(distanceJson);

	// send json to Azure
	httpRequest("POST", url, "application/atom+xml;type=entry;charset=utf-8", distanceJson);

	delay(1000);
}