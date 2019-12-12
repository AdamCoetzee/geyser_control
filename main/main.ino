#include <SPI.h>
#include <WiFiNINA.h>
#include <string.h>
#include <LiquidCrystal.h>
#include "cred.h"

#define LED_WIFI 6
#define LED_SERVER 7
#define LED_GEYSER 9
//LCD
LiquidCrystal lcd(0, 1, 2, 3, 4, 5);
byte tick[8] = {
  B00000,
  B00000,
  B00001,
  B00010,
  B10100,
  B01000,
  B00000,
};

//For establishing wifi and server connection
char ssid[] = NET_ID;                   //  your network SSID (name) 
char pass[] = NET_PWD;   			    // your network password
int status = WL_IDLE_STATUS;			//status of WiFi connection
IPAddress server(192,168,1,11);  		// server
WiFiClient client;					    // Initialize the client library

#define MAXLENGTH 100
uint8_t commandBuffer[MAXLENGTH];		//buffer to store incoming command.

void runCommand(char command[]);
int checkWifi();
void checkServer();
void setLEDs();
void setLCD();
void setLCDcmd();

void setup() 
{
    setLEDs();	
    setLCD();
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
        lcd.setCursor(4, 0);
        lcd.write(byte(0));
        digitalWrite(LED_WIFI, HIGH);
        return CurrentStatus;
    }
    else	//attempt to connect, if not connected
    {
        lcd.setCursor(4, 0);
        lcd.write('?');
        CurrentStatus = WiFi.begin(ssid, pass);
        if (CurrentStatus == WL_CONNECTED)
        {
            lcd.setCursor(4, 0);
            lcd.write(byte(0));
            digitalWrite(LED_WIFI, HIGH);
        }
        else
        {
            lcd.setCursor(4, 0);
            lcd.write('X');
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
        lcd.setCursor(7, 0);
        lcd.write(byte(0));
        digitalWrite(LED_SERVER, HIGH);
        return;
    }
    else
    {
        lcd.setCursor(7, 0);
        lcd.write('?');
        if(client.connect(server, 4444))
        {
            lcd.setCursor(7, 0);
            lcd.write(byte(0));
            digitalWrite(LED_SERVER, HIGH);
            client.println("Connected");
            return;
        }
		else
        {
            lcd.setCursor(7, 0);
            lcd.write('X');
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
        lcd.setCursor(1, 0);
        lcd.write(byte(0));
        digitalWrite(LED_GEYSER, HIGH);
        client.println("GEYSER ON");
        setLCDcmd();
        lcd.print("GEYSER ON");
        return;
    }

    if (strcmp(command, "g_turnOff") == 0)
    {
        lcd.setCursor(1, 0);
        lcd.write('X');
        digitalWrite(LED_GEYSER, LOW);
        client.println("GEYSER OFF");        
        setLCDcmd();
        lcd.print("GEYSER OFF");
        return;
    }
 
    setLCDcmd();
    lcd.print("unrecognized command");
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

void setLCD()
{
    lcd.createChar(0, tick);
    lcd.begin(16,2);
    lcd.print("GX WX SX 00:00");
}

void setLCDcmd()//clears bottom lcd row and sets cursor for cmd
{
    int k;
    lcd.setCursor(0, 1);
    for (k=0; k<16; k++)
    {
        lcd.write(' ');
    }
    lcd.setCursor(0, 1);
}
