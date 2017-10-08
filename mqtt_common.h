#pragma once

#include <PubSubClient.h>


// example: <myiothub>.azure-devices.net

const char* iothub_url = "";



// this is the id of the device created in Iot Hub

// example: myCoolDevice
const char* iothub_deviceid = "";

// <myiothub>.azure-devices.net/<myCoolDevice>
const char* iothub_user = "";

// SAS token should look like "SharedAccessSignature sr=<myiothub>.azure-devices.net%2Fdevices%2F<myCoolDevice>&sig=123&se=456"
const char* iothub_sas_token = "";

// default topic feed for subscribing is "devices/<myCoolDevice>/messages/devicebound/#""
const char* iothub_subscribe_endpoint = "";

// default topic feed for publishing is "devices/<myCoolDevice>/messages/events/"
const char* iothub_publish_endpoint = "";

PubSubClient client(espClient);


// function to connect to MQTT server
void connect_mqtt() {

	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");

		// Attempt to connect
		if (client.connect(iothub_deviceid, iothub_user, iothub_sas_token)) {

			Serial.println("connected");

			// ... and resubscribe
			client.subscribe(iothub_subscribe_endpoint);

		}
		else {

			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println("try again in 5 seconds");

			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}