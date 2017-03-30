// Common.h
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SD.h>

// need this lib for Secure SSL for ESP 8266 chip
#include <WiFiClientSecure.h>  

#define OLED_address  0x3c                           // OLED I2C bus address

WiFiUDP ntpUDP;

// setup https client for 8266 SSL client
WiFiClientSecure client;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

// endpoint to use to send message /devices/{device name}/messages/events?api-version=2016-02-03
// host name address for your Azure IoT Hub
// on device monitor generate a sas token on config page.
//String uri = "/devices/esp8266v2/messages/events?api-version=2016-02-03";
char hostnme[] = "ArtTempIOT.azure-devices.net";
char authSAS[] = "SharedAccessSignature sr=ArtTempIOT.azure-devices.net&sig=vmUF6p3IANfHmNWrvk4Zf%2BlpngD365hUX9f%2FB2zNaUM%3D&se=1515799811&skn=iothubowner";

 
String createJsonData(String devId, float distance)
{
	// create json object
	StaticJsonBuffer<200> jsonBuffer;
	char* buff;
	String outdata;

	// get time from 1/1/1970 to use as key
	timeClient.update();
	String key = (String)timeClient.getEpochTime();

	JsonObject& root = jsonBuffer.createObject();
	root["DeviceId"] = devId;
	root["KeyId"] = (String)timeClient.getEpochTime();
	root["distance"] = distance;

	// convert to string
	root.printTo(outdata);
	return outdata;
}

void DisplayText(int row, int col, String data)
{
	display.drawString(row, col, data);
	display.display();
}


void getSDData(String *passData)
{
	String str, netid, pwd, deviceId, url;

	File dataFile;
	Serial.println("In getSDData");

	// initialize sd card 
	// for nodemcu use begin() else use begin(4)
	Serial.print("Initializing SD card...");

	if (!SD.begin()) {
		Serial.println("initialization failed!");
		return;
	}
	Serial.println("initialization done.");

	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
	dataFile = SD.open("wifiFile.txt");
	int index = 0;

	if (dataFile)
	{
		Serial.println("data from sd card");
		while (dataFile.available())
		{
			if (dataFile.find("SSID:"))
			{
				str = dataFile.readStringUntil('|');
				netid = str;
				Serial.println(netid);
				//sendToDisplay(0, 15, netid);
			}
			if (dataFile.find("PASSWORD:"))
			{
				str = dataFile.readStringUntil('|');
				pwd = str;
				Serial.println(pwd);
				//sendToDisplay(30, 15, pwd);
			}
			if (dataFile.find("DEVICEID:"))
			{
				str = dataFile.readStringUntil('|');
				deviceId = str;
				Serial.println(deviceId);
				//sendToDisplay(0, 30, deviceId);
			}
			if (dataFile.find("URL:"))
			{
				str = dataFile.readStringUntil('|');
				url = str;
				Serial.println(url);
				//sendToDisplay(50, 30, url);
			}
		}
		// close the file
		dataFile.close();
	}

	passData[0] = netid;
	passData[1] = pwd;
	passData[2] = deviceId;
	passData[3] = url;
}


void httpRequest(String verb, String uri, String contentType, String content)
{
	Serial.println("--- Start Process --- ");
	if (verb.equals("")) return;
	if (uri.equals("")) return;

	// close any connection before send a new request.
	// This will free the socket on the WiFi shield
	client.stop();

	// if there's a successful connection:
	if (client.connect(hostnme, 443)) {
		Serial.println("--- We are Connected --- ");
		Serial.print("*** Sending Data To:  ");
		Serial.println(hostnme + uri);

		Serial.print("*** Data To Send:  ");
		Serial.println(content);

		client.print(verb); //send POST, GET or DELETE
		client.print(" ");
		client.print(uri);  // any of the URI
		client.println(" HTTP/1.1");
		client.print("Host: ");
		client.println(hostnme);  //with hostname header
		client.print("Authorization: ");
		client.println(authSAS);  //Authorization SAS token obtained from Azure IoT device explorer
								  //client.println("Connection: close");

		if (verb.equals("POST"))
		{
			Serial.println("--- Sending POST ----");
			client.print("Content-Type: ");
			client.println(contentType);
			client.print("Content-Length: ");
			client.println(content.length());
			client.println();
			client.println(content);
		}
		else
		{
			Serial.println("--- client status --- ");
			Serial.println(client.status());

			client.println();

		}
	}

	while (client.available()) {
		char c = client.read();
		Serial.write(c);
	}

	Serial.println("--- Send complete ----");
}