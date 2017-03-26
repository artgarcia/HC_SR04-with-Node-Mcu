// Common.h
#include <ArduinoJson.h>


String createJsonData(String devId, float temp, float humidity, String keyid)
{
	// create json object
	StaticJsonBuffer<200> jsonBuffer;
	char* buff;
	String outdata;

	JsonObject& root = jsonBuffer.createObject();
	root["DeviceId"] = devId;
	root["KeyId"] = keyid;
	root["temperature"] = temp;
	root["humidity"] = humidity;

	// convert to string
	root.printTo(outdata);
	return outdata;
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
