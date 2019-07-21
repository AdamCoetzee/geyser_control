#include <SPI.h>
#include <WiFiNINA.h>
#include <string.h>

//For establishing wifi connection
char ssid[] = "reedflute_flat";          //  your network SSID (name) 
char pass[] = "hungary1";   // your network passwor
int status = WL_IDLE_STATUS;
IPAddress server(192,168,1,100);  // server
WiFiClient client;  // Initialize the client library

#define MAXLENGTH 100
uint8_t commandBuffer[MAXLENGTH];//buffer to store incoming message.

void connectWiFi();
void runCommand(char command[]);

void setup() 
{
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  connectWiFi();
}

void loop()
{
  memset(commandBuffer, 0, sizeof(commandBuffer));

  if (client.available()) 
	{
    client.read(commandBuffer, sizeof(commandBuffer));
    client.println((char*)commandBuffer);   
    runCommand((char*)commandBuffer);
  } 
}

void connectWiFi()
{
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    delay(1000); // don't do anything else:
  } 
  else {
    Serial.println("Connected to wifi");
    Serial.println("Starting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 4444)) {
      Serial.println("connected");
      client.println("Connected arduino to TCP socket :)");
    }
  }
}

void runCommand(char command[])
{
  if (strcmp(command, "g_turnOn\n") == 0)
  {
    digitalWrite(9, HIGH);
  }

  if (strcmp(command, "g_turnOff\n") == 0)
  {
    digitalWrite(9, LOW);
  } 
}
