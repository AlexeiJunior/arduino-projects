#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// IC2 -> Esp8266
// GND -> GND
// VCC -> VIN
// SDA -> D2
// SCL -> D1

const char* login = "";
const char* password = "";
const int serverPort = 80;
const int serialBps = 115200;
const int ledEsp = 2;

ESP8266WebServer server(serverPort);
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

void setup() {
	//ARDUINO
	Serial.begin(serialBps);
	pinMode(ledEsp, OUTPUT);

	//LCD
	lcd.begin(16,2);
	cleanLcd();
	cleanLcd2();

	//WIFI
	WiFi.mode(WIFI_STA);
	WiFi.begin(login, password);

	printLcd("Connecting...");
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
	}

	printLcd2(IpAddress2String(WiFi.localIP()));

	if (MDNS.begin("esp8266")) {
		Serial.println("MDNS responder started");
	}

	setRoutes();
	server.begin();
	printLcd("Server started");
}

void loop() {
	server.handleClient();
	// doGetRequest("alexeimlsn.com.br")
}

void cleanLcd() {
	lcd.setCursor(0,0);
	lcd.print("                         ");
	lcd.setCursor(0,0);
}

void cleanLcd2() {
	lcd.setCursor(0,1);
	lcd.print("                         ");
	lcd.setCursor(0,1);
}

void printLcd(String input) {
	cleanLcd();
	lcd.print(input);
	Serial.println(input);
	blink();
}

void printLcd2(String input) {
	cleanLcd2();
	lcd.print(input);
	Serial.println(input);
	blink();
}

void blink() {
	digitalWrite(ledEsp, HIGH);
	delay(1000);
	digitalWrite(ledEsp, LOW);
}

void setRoutes() {
	server.on("/", handleRoot);

	server.on("/inline", [](){
		server.send(200, "text/plain", "this works as well");
	});

	server.onNotFound(handleNotFound);
}

void handleRoot() {
	String msg;

	msg = "<b>ESP8266</b> is running! Port A0: ";
	msg += analogRead(A0);

	server.send(200, "text/html", msg);
}

void handleNotFound(){
	server.send(404, "text/plain", "Not Found\n");
}

void doGetRequest(String requestUrl) {
	if (WiFi.status() == WL_CONNECTED) {
		HTTPClient http;

		http.begin(requestUrl.c_str());
		int httpResponseCode = http.GET();

		if (httpResponseCode == 200) {
			String payload = http.getString();
			Serial.println(payload);
		}

		http.end();
	}
}

String IpAddress2String(const IPAddress& ipAddress) {
	return String(ipAddress[0]) + String(".") +
	String(ipAddress[1]) + String(".") +
	String(ipAddress[2]) + String(".") +
	String(ipAddress[3]);
}