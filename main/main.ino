#include <SPI.h>
#include <WiFiNINA.h>
#include <string.h>

#define LED_WIFI 6
#define LED_SERVER 7
#define LED_GEYSER 9

//For establishing wifi and server connection
char ssid[] = "reedflute_flat";         //  your network SSID (name) 
char pass[] = "hungary1";   			// your network passwor
int status = WL_IDLE_STATUS;			//status of WiFi connection
IPAddress server(192,168,1,100);  		// server
WiFiClient client; 	 					// Initialize the client library

#define MAXLENGTH 100
uint8_t commandBuffer[MAXLENGTH];		//buffer to store incoming command.

void runCommand(char command[]);
int checkWifi();
void checkServer();
void setLEDs();

void setup() 
{
	setLEDs();	
}

void loop()
{
  memset(commandBuffer, 0, sizeof(commandBuffer));
  status = checkWifi();
	checkServer();

  if (client.available()) 
	{
    client.read(commandBuffer, sizeof(commandBuffer));
    runCommand((char*)commandBuffer);
  } 
}

int checkWifi()//check wifi connection status, attempt to connect if not connected
{
	int CurrentStatus = WiFi.status();	//retreive current status
	if (CurrentStatus == WL_CONNECTED)	//if currently connected
	{
		digitalWrite(LED_WIFI, HIGH);
		return CurrentStatus;
	}
	else	//attempt to connect, if not connected
	{
		CurrentStatus = WiFi.begin(ssid, pass);
		if (CurrentStatus == WL_CONNECTED)
		{
			digitalWrite(LED_WIFI, HIGH);
		}
		else
		{
			digitalWrite(LED_WIFI, HIGH);
			delay(1000);
			digitalWrite(LED_WIFI, LOW);
		}
		return CurrentStatus;
	}
}

void checkServer()//check server connection status, attempt to connect if not connected
{
	if(client.connected())
	{
		digitalWrite(LED_SERVER, HIGH);
		return;
	}
	else
	{
		if(client.connect(server, 4444))
		{
			digitalWrite(LED_SERVER, HIGH);
			client.println("Connected");
			return;
		}
		else
		{
			digitalWrite(LED_SERVER, HIGH);
			delay(1000);
			digitalWrite(LED_SERVER, LOW);
			return;
		}		
	}
}

void runCommand(char command[])
{
  if (strcmp(command, "g_turnOn") == 0)
  {
    digitalWrite(LED_GEYSER, HIGH);
		client.println("GEYSER ON");
		return;
  }

  if (strcmp(command, "g_turnOff") == 0)
  {
    digitalWrite(LED_GEYSER, LOW);
		client.println("GEYSER OFF");
		return;
  } 

	client.print("unrecognized command");
	return;
}

void setLEDs()
{
  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_SERVER, OUTPUT);
  pinMode(LED_GEYSER, OUTPUT);
 
  digitalWrite(LED_WIFI, HIGH);
  digitalWrite(LED_SERVER, HIGH);
  digitalWrite(LED_GEYSER, HIGH);
	delay(1000);
  digitalWrite(LED_WIFI, LOW);
  digitalWrite(LED_SERVER, LOW);
  digitalWrite(LED_GEYSER, LOW);
}
