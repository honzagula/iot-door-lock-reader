/*
  Door lock reader
*/

#include <ESP8266WiFi.h>
#include <Ticker.h> //Ticker Library

#ifndef STASSID
#define STASSID "lundegaard.public"
#define STAPSK  "jerrylundegaard"
//#define STASSID "Abc"
//#define STAPSK "abc12345"

#endif

Ticker blinker;

// constants won't change. They're used here to set pin numbers:
const int buttonPin = D2;       // the number of the pushbutton pin
const int ledPin = LED_BUILTIN; // the number of the LED pin

// variables will change:
volatile bool locked = false;

const char *ssid = STASSID;
const char *password = STAPSK;

void readDoorState()
{
  Serial.print("Door sate: ");
  if (digitalRead(buttonPin) == LOW) {
    locked = true;
    digitalWrite(ledPin, LOW);
    Serial.println("Locked");
  } else {
    locked = false;
    digitalWrite(ledPin, HIGH);
    Serial.println("Unlocked");
  }
}

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  delay(4000);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  blinker.attach(1, readDoorState);
}

void loop()
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  Serial.println("Client is available reading data");

  // Wait until the client sends some data
  Serial.println("new client");
  unsigned long timeout = millis() + 3000;
  while (!client.available() && millis() < timeout)
  {
    delay(1);
  }
  if (millis() > timeout)
  {
    Serial.println("timeout");
    client.flush();
    client.stop();
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request

  if (req.indexOf("/doorState") != -1)
  {
    Serial.println("Returning door sate");
    //String response = "HTTP/1.1 200 OK \r\n Content-Type: text/plain \r\n \r\n {doorStatus: " + doorState;
    //client.print( response + "}");
    //client.flush();

    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html> ";
    s += "dooor status";
    s += locked;
    s += "</html>\n";
    client.print(s);
    client.flush();
    delay(1);
    return;
  }

  delay(1);
  Serial.println("Client disconnected");

}
