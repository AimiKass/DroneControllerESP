#include <ESP8266WiFi.h>
#include <Servo.h>


const char* ssid="LoCoMoCo";
const char* password="patata97";
const int port = 1234;

WiFiServer wifiServer(port);

//==========================

#define CH1_Input 16 //
#define CH2_Input 5 //
#define CH3_Input 4 //
#define CH4_Input 0 //

#define SWC_PIN 10 // pin 10 works only as an input

#define CH1_Output 14
#define CH2_Output 12
#define CH3_Output 13
#define CH4_Output 15
#define CH5_Output 2


Servo servo_1; //ROLL
Servo servo_2; //PITCH
Servo servo_3; //THROTTLE
Servo servo_4; //YAW
Servo servo_5; //ARM
//==========================


void setup() 
{
  //==========================
  servo_1.attach(CH1_Output);
  servo_2.attach(CH2_Output);
  servo_3.attach(CH3_Output);
  servo_4.attach(CH4_Output);
  servo_5.attach(CH5_Output);

  
  pinMode(CH1_Input,INPUT);
  pinMode(CH2_Input,INPUT);
  pinMode(CH3_Input,INPUT);
  pinMode(CH4_Input,INPUT);

  pinMode(SWC_PIN,INPUT); 
  //==========================
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("Wifi connecting to ");
  Serial.println( ssid );

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);

  Serial.println();
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED )
  {
    digitalWrite(2,HIGH); //del
    delay(500);
    Serial.println(".");
    digitalWrite(2,LOW); //del
    delay(500);
  }

  Serial.println(WiFi.localIP());
  digitalWrite(2,HIGH); //del
  wifiServer.begin();
}



void loop() 
{

        
  WiFiClient client = wifiServer.available();
          
  if (client) 
  {
    if(client.connected()) //Returns true if the client is connected, false if not.
    {
      Serial.println("Client Connected");
    }
    
    while(client.connected())
    {
      int len = client.available();  //The number of bytes available.
      int servoData = 0; 
 
      while(len>0)
      {
        int dataFromClient = client.read(); // read data from the connected client

         if(dataFromClient != -1)
         {
            if (servoData == 0)
              servoData = dataFromClient*256;
            else
            {
              servoData = servoData + dataFromClient;

//================================================

              switch(servoData/10000) // seporates the 5 digit integer
                {
                 case 1:
                    servo_1.writeMicroseconds(servoData%10000);
                  break;
                 case 2:
                    servo_2.writeMicroseconds(servoData%10000);
                 break;
                 case 3:
                    servo_3.writeMicroseconds((servoData%10000)-5);
                 break;
                 case 4:
                    servo_4.writeMicroseconds(servoData%10000);
                 break;
                 case 5:
                    servo_5.writeMicroseconds(servoData%10000);
                 break;
                }

//================================================
              servoData = 0;              
            }
         }
      }
   }
   Serial.println("Client Disconnected");
 }
}
