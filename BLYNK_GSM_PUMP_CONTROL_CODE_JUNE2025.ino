/************************************************************************************
 *  My Channel: https://www.youtube.com/c/TeachMeSomething  Visit for More Project Videos
 *  
 *  *********************************************************************************
 *  Preferences--> Aditional boards Manager URLs : 
 *  For ESP32:
 *  https://dl.espressif.com/dl/package_esp32_index.json
 *  
 *  *********************************************************************************/

 
#define BLYNK_PRINT Serial
#include <HardwareSerial.h>
HardwareSerial Sim800L(2); //

/* Fill-in your Template ID (only if using Blynk.Cloud) */

#define BLYNK_TEMPLATE_ID "TMPL3LBL5g6Ft"
#define BLYNK_TEMPLATE_NAME "WIFI MOTOR STARTER"
#define BLYNK_AUTH_TOKEN "1qdqld7CwpW9Tqbyiw9tchsdc5qbTMqi"



// Select your modem:
#define TINY_GSM_MODEM_SIM800


#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>


char auth[] = BLYNK_AUTH_TOKEN;

// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "internet";  // Change your nework apn, user and Pass
char user[] = "";
char pass[] = "";
BlynkTimer timer;



#define relay1_pin 19
#define relay2_pin 21
#define relay3_pin 22
#define relay4_pin 23

int relay1_state = 0;
int relay2_state = 0;
int relay3_state = 0;
int relay4_state = 0;


//Change the virtual pins according the your Wish
#define button1_vpin    V1
#define button2_vpin    V2
#define button3_vpin    V3 
#define button4_vpin    V4


BLYNK_CONNECTED() {
  Blynk.syncVirtual(button1_vpin);
  Blynk.syncVirtual(button2_vpin);
  Blynk.syncVirtual(button3_vpin);
  Blynk.syncVirtual(button4_vpin);
}


BLYNK_WRITE(button1_vpin) {
  relay1_state = param.asInt();
  digitalWrite(relay1_pin, ! relay1_state);
   Serial.print("Relay 1 ");
   if(relay1_state==0)
   Serial.println("OFF");
   else
   Serial.println("ON");
}
//--------------------------------------------------------------------------
BLYNK_WRITE(button2_vpin) {
  relay2_state = param.asInt();
  digitalWrite(relay2_pin,  ! relay2_state);
   Serial.print("Relay 2 ");
   if(relay2_state==0)
   Serial.println("OFF");
   else
   Serial.println("ON");
}
//--------------------------------------------------------------------------
BLYNK_WRITE(button3_vpin) {
  relay3_state = param.asInt();
  digitalWrite(relay3_pin, ! relay3_state);
   Serial.print("Relay 3 ");
   if(relay3_state==0)
   Serial.println("OFF");
   else
   Serial.println("ON");
}
//--------------------------------------------------------------------------
BLYNK_WRITE(button4_vpin) {
  relay4_state = param.asInt();
  digitalWrite(relay4_pin, ! relay4_state);
   Serial.print("Relay 4 ");
   if(relay4_state==0)
   Serial.println("OFF");
   else
   Serial.println("ON");
}
//--------------------------------------------------------------------------

TinyGsm modem(Sim800L);

void setup()
{
  Serial.begin(115200);
    //--------------------------------------------------------------------
  pinMode(relay1_pin, OUTPUT);
  pinMode(relay2_pin, OUTPUT);
  pinMode(relay3_pin, OUTPUT);
  pinMode(relay4_pin, OUTPUT);
  //--------------------------------------------------------------------
  //During Starting all Relays should TURN OFF
  digitalWrite(relay1_pin, HIGH);
  digitalWrite(relay2_pin, HIGH);
  digitalWrite(relay3_pin, HIGH);
  digitalWrite(relay4_pin, HIGH);
  //--------------------------------------------------------------------
  delay(2000);
  Sim800L.begin(115200);
  Sim800L.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  delay(1000);
  Sim800L.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  delay(1000);
  Sim800L.println("AT+CBC"); // Battery % , Voltage level in mV
  updateSerial();
  delay(1000);
  updateSerial();
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  Blynk.begin(auth, modem, apn, user, pass);
  Blynk.virtualWrite(button1_vpin, relay1_state);
  Blynk.virtualWrite(button2_vpin, relay2_state);
  Blynk.virtualWrite(button3_vpin, relay3_state);
  Blynk.virtualWrite(button4_vpin, relay4_state);
}

void loop()
{
  Blynk.run();
  timer.run();
}


void updateSerial()
{
  
  while (Serial.available()) 
  {
    Sim800L.write(Serial.read());//Forward what Serial received to Software Serial Port
    
  }
  while(Sim800L.available()) 
{
 Serial.write(Sim800L.read());
}

}
