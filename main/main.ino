#include <SPI.h>
#include <WiFiNINA.h>
#include <string.h>
#include <LiquidCrystal.h>
#include "cred.h"
#include <RTCZero.h>

//LED INDICATORS
#define LED_WIFI 6
#define LED_SERVER 7
#define LED_GEYSER 9

//CUSTOM CHHARACTER DEFINITION
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
char ssid[] = NET_ID;                   //your network SSID (name) 
char pass[] = NET_PWD;   			    //your network password
int status = WL_IDLE_STATUS;			//status of WiFi connection
IPAddress server(192,168,1,11);  		//server
WiFiClient client;					    //Initialize the client library

//Internal Clock and alarms
RTCZero rtc;
const int GMT = 2;                      //timezone
struct alarm_time
{   
    bool set;
    bool triggered;
    int hour;
    int minute;
};
alarm_time On_Time;
alarm_time Off_Time;
enum geyser_state {OFF, ON};
geyser_state STATE = OFF;

#define MAXLENGTH 100
uint8_t commandBuffer[MAXLENGTH];		//buffer to store incoming command.

void runCommand(char command[]);
int checkWifi();
void geyserOn();
void geyserOff();
void checkServer();
void setLEDs();
void setLCD();
void clearLCDInfo();
void printLCDTime();
int compareTimes(int hourA, int minA, int hourB, int minB);
bool isValidTime(int hour, int minute);

void setup() 
{
    setLEDs();	
    setLCD();
    rtc.begin();
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
    checkTime(); 
    clearLCDInfo();
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
        clearLCDInfo();
        lcd.print("connecting wifi");
        lcd.setCursor(4, 0);
        lcd.write('?');
        CurrentStatus = WiFi.begin(ssid, pass);
        if (CurrentStatus == WL_CONNECTED)
        {
            clearLCDInfo();
            lcd.print("connected wifi");
            lcd.setCursor(4, 0);
            lcd.write(byte(0));
            digitalWrite(LED_WIFI, HIGH);
        }
        else
        {
            clearLCDInfo();
            lcd.print("failed wifi");
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
        client.println("gs_conConfirm");//send a message every so often to confirm connection
        lcd.setCursor(7, 0);
        lcd.write(byte(0));
        digitalWrite(LED_SERVER, LOW);
        delay(500);
        digitalWrite(LED_SERVER, HIGH);
        return;
    }
    else
        clearLCDInfo();
        lcd.print("finding server");   {
        lcd.setCursor(7, 0);
        lcd.write('?');
        if(client.connect(server, 4444))
        {
            clearLCDInfo();
            lcd.print("found server");
            lcd.setCursor(7, 0);
            lcd.write(byte(0));
            digitalWrite(LED_SERVER, HIGH);
            client.println("Connected");             
            client.println("gs_fetchTime");
            return;
        }
		else
        {
            clearLCDInfo();
            lcd.print("failed server con");
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
        geyserOn(); 
        return;
    }

    if (strcmp(command, "g_turnOff") == 0)
    {
        geyserOff();
        return;
    }

    if (strncmp (command,"g_setTime",9) == 0)
    {
        clearLCDInfo();
        uint32_t epoch;
        char comm[128];
        sscanf(command, "%s %u", comm, &epoch);
        clearLCDInfo();
        rtc.setEpoch((uint32_t)epoch);
        return;
    }    
 
    if (strncmp (command,"g_setOnTime",11) == 0)
    {
        char comm[128];
        char info[16];
        sscanf(command, "%s %d %d", comm, &On_Time.hour, &On_Time.minute);
        bool validTime = isValidTime(On_Time.hour, On_Time.minute);
        int timeOrder = compareTimes(On_Time.hour, On_Time.minute, Off_Time.hour, Off_Time.minute);       

        if (((Off_Time.set == false) || (timeOrder == 1)) && (validTime == true))
        {
            On_Time.set = true;
            snprintf(info, sizeof(info), "on time: %d:%d", On_Time.hour,On_Time.minute);
            clearLCDInfo();
            lcd.print(info);
            client.println(info);
        }
        else
        {   
            clearLCDInfo();
            lcd.print("invalid time");
            client.println("invalid time");
        }
        delay(2000);
        return;
    }        
    
    if (strncmp (command,"g_setOffTime",12) == 0)
    {
        char comm[128];
        char info[16];
        sscanf(command, "%s %d %d", comm, &Off_Time.hour, &Off_Time.minute);
        bool validTime = isValidTime(Off_Time.hour, Off_Time.minute);
        int timeOrder = compareTimes(On_Time.hour, On_Time.minute, Off_Time.hour, Off_Time.minute);       

        if (((On_Time.set == false) || (timeOrder == 1)) && (validTime == true))
        {
            Off_Time.set = true;
            snprintf(info, sizeof(info), "off time: %d:%d", Off_Time.hour,Off_Time.minute);
            clearLCDInfo();
            lcd.print(info);
            client.println(info);
        }
        else
        {
            clearLCDInfo();
            lcd.print("invalid time");
            client.println("invalid time");
        }
        delay(2000);
        return;
    }   

    return;
}

//update current time and update current state
void checkTime()
{
    printLCDTime();
    int currHour = rtc.getHours()+GMT;
    int currMin = rtc.getMinutes();
    char info[128];
   
    if ((On_Time.set == true) && (Off_Time.set == true))//check that times are set   
    {
        int OnTimeStatus = compareTimes(currHour, currMin, On_Time.hour, On_Time.minute);  
        int OffTimeStatus = compareTimes(currHour, currMin, Off_Time.hour, Off_Time.minute);
        
        if (OnTimeStatus == 1)  //if currentTime BEFORE On_Time
        {
            geyserOff();
            snprintf(info, sizeof(info), "set: %d:%d-%d:%d", On_Time.hour, On_Time.minute, Off_Time.hour, Off_Time.minute);
            clearLCDInfo();
            lcd.print(info);
        }

        if ((OnTimeStatus == 0) && (OffTimeStatus == 1)) //if current time AFTER On_Time AND BEFORE Off_Time
        {
            geyserOn();
            snprintf(info, sizeof(info), "On until  %d:%d", Off_Time.hour, Off_Time.minute);
            clearLCDInfo();
            lcd.print(info);
        }
        
        if (OffTimeStatus == 0) //if AFTER Off_Time
        {
            geyserOff();
            snprintf(info, sizeof(info), "set: %d:%d-%d:%d", On_Time.hour, On_Time.minute, Off_Time.hour, Off_Time.minute);
            clearLCDInfo();
            lcd.print(info);
        }
    }
    else
    {
        clearLCDInfo();
        lcd.print("times not set");       
    }
    delay(3000);
    return;
}

void geyserOn()
{   
    if (STATE == OFF)
    { 
        STATE = ON;
        lcd.setCursor(1, 0);
        lcd.write(byte(0));
        digitalWrite(LED_GEYSER, HIGH);
        client.println("GEYSER ON");
        clearLCDInfo();
        lcd.print("GEYSER ON");
        delay(1000);
    }
}

void geyserOff()
{
    if (STATE == ON)
    {
        STATE = OFF;
        lcd.setCursor(1, 0);
        lcd.write('X');
        digitalWrite(LED_GEYSER, LOW);
        client.println("GEYSER OFF");
        clearLCDInfo();
        lcd.print("GEYSER OFF");
    }
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
    lcd.print("GX WX SX   00:00");
}

void clearLCDInfo()//clears bottom lcd row and sets cursor for info prompt
{
    int k;
    lcd.setCursor(0, 1);
    for (k=0; k<16; k++)
    {
        lcd.write(' ');
    }
    lcd.setCursor(0, 1);
}

void printLCDTime()
{
    char currentTime[8];    //current time hh:mm
    int k;
    //clear current time
    lcd.setCursor(11, 0);
    for(k=0; k<8; k++)
    {
        lcd.write(' ');
    } 
    lcd.setCursor(11, 0);    
    memset(currentTime, 0, sizeof(currentTime));
    int hour = rtc.getHours()+GMT;
    int minute = rtc.getMinutes();

    if (hour < 10)
    {
        lcd.write('0');
    }
    lcd.print(hour);   
    lcd.write(':');
    if (minute < 10)
    {
        lcd.write('0');
    }
    lcd.print(minute);
    return;
}

//compares timeA and timeB to check which is earliest
//returns 1 if time A is before time B
//returns 0 if time A is after time B
//returns -1 if timeA is the same as timeB
int compareTimes(int hourA, int minA, int hourB, int minB)
{
    //check if timeA is before timeB
    if (((hourA == hourB) && (minA < minB)) || (hourA < hourB))
    {
        return 1;
    }
    
    //check if timeA is after timeB
    if (((hourA == hourB) && (minA > minB)) || (hourA > hourB))
    {
        return 0;
    }

    //check if times are equal
    if ((hourA == hourB) && (minA == minB))
    {
        return -1;
    }
    
    return -2;
}

bool isValidTime(int hour, int minute)
{
    if ((hour <= 24) && (minute <=59))
    {
        return true;
    }
    else
    {
        return false;
    }
}
