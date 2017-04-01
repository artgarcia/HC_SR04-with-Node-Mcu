

/*
********************************************
14CORE ULTRASONIC DISTANCE SENSOR CODE TEST
NODE MCU .9 esp 8266 and hc-sr04 distance sensor
********************************************
*/



// common include file with additional user functions ise 
// To use tabs with a .h extension, you need to #include it (using "double quotes" not <angle brackets>).                    

#include "Common.h"
#include <ESP8266WiFi.h>

// need this lib for Secure SSL for ESP 8266 chip
#include <WiFiClientSecure.h>  

// http://easycoding.tn/tuniot/demos/code/
// D3 -> SDA
// D4 -> SCL      display( address of display, SDA,SCL)
#include "SSD1306.h"
SSD1306  display(0x3C, 2, 0);

// NodeMCU Pin D1 > TRIGGER | Pin D2 > ECHO
#define TRIGGER 5
#define ECHO    4

String netid, pwd, deviceId, url, host, sas;
long duration, distance, lastDistance;

String passData[6];


void setup() {

	Serial.begin(9600);
	
	// stup for display
	display.init();
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.flipScreenVertically();
	display.setFont(ArialMT_Plain_10);
	
	DisplayText(0, 0, "Screen Init");

	// setup for distance sensor
	Serial.println("Setup Distance Sensor");
	pinMode(TRIGGER, OUTPUT);
	pinMode(ECHO, INPUT);

	display.clear();
	DisplayText(0, 0, "Read data from SD Card");

	// get data from sd card
	// passing an array to house sd card information
	getSDData(passData);

	// move sd card data to global variables
	netid = passData[0];
	pwd = passData[1];
	deviceId = passData[2];
	url = passData[3];

	// endpoint to use to send message /devices/{device name}/messages/events?api-version=2016-02-03
	// host =  address for your Azure IoT Hub
	// sas  =  sas authorization token from below
	//
	// on device monitor generate a sas token on config page.
	//String uri = "/devices/esp8266v2/messages/events?api-version=2016-02-03";
	//
	host = passData[4];
	sas = passData[5];
	
	// replace device id in url 
	url.replace("{0}", deviceId);

	DisplayText(0, 15, "SSID:" + netid);
	DisplayText(0, 30, "pwd :" + pwd);
	DisplayText(0, 45, "Device Id:" + deviceId);
	
	// verify variables from sd card got into globals
	Serial.print("NETID:");
	Serial.println(netid);
	Serial.print("PWD:");
	Serial.println(pwd);
	Serial.print("DEVICEID:");
	Serial.println(deviceId);
	Serial.print("URL:");
	Serial.println(url);

	Serial.print("HOSTNAME:");
	Serial.println(host);

	Serial.print("SAS:");
	Serial.println(sas);

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

	display.clear();
	DisplayText(0, 0, "Connected:" + netid);
	DisplayText(0, 15, "Local IP:");
	DisplayText(0, 30, WiFi.localIP().toString());

	// start time client - used to get current time.
	timeClient.begin();
	// delay 5 sec
	delay(5000);

}

void loop() {

	display.clear();
	DisplayText(0, 0, "Distance Sensor");

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

	DisplayText(0, 15, (String)distance);
	DisplayText(25, 15, "CM");

	String distanceJson = createJsonData("dist01", distance);
	Serial.println(distanceJson);

	// send json to Azure
	httpRequest("POST", url, host,sas,"application/atom+xml;type=entry;charset=utf-8", distanceJson);

	display.drawStringMaxWidth(0, 30,100, distanceJson);
	display.display();
	delay(1000);
}


void DisplayText(int row, int col, String data)
{
	display.drawString(row, col, data);
	display.display();
}
