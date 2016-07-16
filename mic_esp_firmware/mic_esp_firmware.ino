#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//#define jsonLengthInit 300
#define jsonLength 300

WiFiClient espClient;
PubSubClient client(espClient);

String APMqttJson;
String sensorJson;
String ssid, APPass, server, port, deviceID, token;
String mqttPublish, mqttSubscribe;
bool getAPInfo = false;

void setup_wifi() {

	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.println("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid.c_str(), APPass.c_str());

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.println("...");
	}

	Serial.println("WiFi connected");
	Serial.println("Local IP: ");
	Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
	for (int i = 0; i < length; i++) {
		Serial.print((char) payload[i]);
	}
	Serial.println();
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.println("MQTT connection...");
		// Attempt to connect
		if (client.connect(deviceID.c_str(), deviceID.c_str(), token.c_str())) {
			//if (client.connect(deviceID.c_str())) {
			client.subscribe(mqttPublish.c_str());
			Serial.println("[con]");
		} else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			Serial.println("[dis]");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}


void listeningWiFiConfigJson() {
  if (Serial.available() > 0) {
    APMqttJson = Serial.readStringUntil('\n');
    StaticJsonBuffer<jsonLength> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(APMqttJson);
    if (!root.success()) {
      Serial.println("parse wifiAP json failed");
      return;
    }
    const char* ssidChar = root["ssid"];
    ssid = ssidChar;
    if (ssid.length()) {
      Serial.print(ssid);
      Serial.println("->get OK!");
    } else {
      Serial.println("ssid->get Fail!");
    }

    const char* APPassChar = root["APPass"];
    APPass = APPassChar;

    if (APPass.length()) {
      Serial.print(APPass);
      Serial.println("->get OK!");
    } else {
      Serial.println("APPass->get Fail!");
    }

    const char* serverChar = root["server"];
    server = serverChar;

    if (server.length()) {
      Serial.print(server);
      Serial.println("->get OK!");
    } else {
      Serial.println("server->get Fail!");
    }

    const char* portChar = root["port"];
    port = portChar;

    if (port.length()) {
      Serial.print(port);
      Serial.println("->get OK!");
    } else {
      Serial.println("port->get Fail!");
    }

    const char* deviceIDChar = root["deviceID"];
    deviceID = deviceIDChar;

    if (deviceID.length()) {
      Serial.print(deviceID);
      Serial.println("->get OK!");
    } else {
      Serial.println("deviceID->get Fail!");
    }

    const char* tokenChar = root["token"];
    token = tokenChar;
    if (token.length()) {
      Serial.print(token);
      Serial.println("->get OK!");
    } else {
      Serial.println("token->get Fail!");
    }

    mqttPublish = "v1.0\/c\/" + deviceID;
    mqttSubscribe = "v1.0\/d\/" + deviceID;

    getAPInfo = true;
  }
}

void listeningSensorJson() {
  if (Serial.available() > 0) {
    sensorJson = Serial.readStringUntil('\n');
    client.publish(mqttSubscribe.c_str(), sensorJson.c_str());
//    Serial.print("sended:");
//    Serial.println(sensorJson);
    sensorJson = "";
  }
}

void smartConfiger() {

  delay(10);
  WiFi.mode(WIFI_AP_STA);
  delay(500);

  WiFi.beginSmartConfig();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
    Serial.println(WiFi.smartConfigDone());
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup(void) {
	Serial.begin(115200);
	while (!getAPInfo) {
		listeningWiFiConfigJson();
	}
	setup_wifi();
	client.setServer(server.c_str(), port.toInt());
	client.setCallback(callback);
}

void loop(void) {
	if (!client.connected()) {
		Serial.println("[dis]");
		reconnect();
	}
	client.loop();
	listeningSensorJson();
}

