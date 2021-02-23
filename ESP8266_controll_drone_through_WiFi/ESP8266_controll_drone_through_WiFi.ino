#include <ESP8266WiFi.h>
#include <Servo.h>


const char* ssid = "LoCoMoCo";
const char* password = "patata97";
const int port = 1234;

WiFiServer wifiServer(port);



unsigned long previousMillis = 0;

const long interval = 1500;

//==========================

//Connect with FlightController
#define CH1_Output 2 //ROLL_FROM_PC
#define CH2_Output 12 //PITCH_FROM_PC
#define CH3_Output 13 //THROTTLE_FROM_PC
#define CH4_Output 15 //YAW_FROM_PC
#define CH5_Output 16 //ARM_FROM_PC

//TODO find a pin to set the arm function
//Concect with receiver
#define CH1_Input 5 //ROLL_FROM_RECEIVER
#define CH2_Input 14 //PITCH_FROM_RECEIVER
#define CH3_Input 4 //THROTTLE_FROM_RECEIVER
#define CH4_Input 0 //YAW_FROM_RECEIVER

Servo servo_1;
Servo servo_2;
Servo servo_3;
Servo servo_4;
Servo servo_5;

int static interuptSensitivity = 4;

int CH1_Receiver_Old = 0;
int CH2_Receiver_Old = 0;
int CH3_Receiver_Old = 0;
int CH4_Receiver_Old = 0;

bool CH1_Interrupts_Is = false;
bool CH2_Interrupts_Is = false;
bool CH3_Interrupts_Is = false;
bool CH4_Interrupts_Is = false;

//==========================


void setup()
{

  //==========================
  servo_1.attach(CH1_Output);
  servo_2.attach(CH2_Output);
  servo_3.attach(CH3_Output);
  servo_4.attach(CH4_Output);
  servo_5.attach(CH5_Output);

  pinMode(CH1_Input, INPUT_PULLUP);
  pinMode(CH2_Input, INPUT_PULLUP);
  pinMode(CH3_Input, INPUT_PULLUP);
  pinMode(CH4_Input, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(CH1_Input), CH1_INTERRUPT, RISING);
  attachInterrupt(digitalPinToInterrupt(CH2_Input), CH2_INTERRUPT, RISING);
  attachInterrupt(digitalPinToInterrupt(CH3_Input), CH3_INTERRUPT, RISING);
  attachInterrupt(digitalPinToInterrupt(CH4_Input), CH4_INTERRUPT, RISING);

  //==========================

  Serial.begin(115200);
  Serial.println();
  Serial.println("Wifi connecting to ");
  Serial.println( ssid );

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED )
  {
    delay(500);
    Serial.println(".");
  }

  Serial.println(WiFi.localIP());
  wifiServer.begin();
}


void loop()
{


  WiFiClient client = wifiServer.available();

  if (client)
  {
    if (client.connected()) //Returns true if the client is connected, false if not.
    {
      Serial.println("Client Connected");
    }

    while (client.connected())
    {
      int len = client.available();  //The number of bytes available.
      int servoData = 0;
      int servo3_old = 0;
      int servo3_new = 0;

      while (len > 0)
      {
        int dataFromClient = client.read(); // read data from the connected client

        if (dataFromClient != -1)
        {
          if (servoData == 0)
            servoData = dataFromClient * 256;
          else
          {
            servoData = servoData + dataFromClient;

            //================================================

            //every interval(1.5) seconds gives the computer drone controll
            unsigned long currentMillis = millis();
            if (currentMillis - previousMillis >= interval)
            {
              previousMillis = currentMillis;
              resetFlags();
            }


            switch (servoData / 10000) // seporates the 5 digit integer
            {
              case 1:
                if (!CH1_Interrupts_Is)
                  servo_1.writeMicroseconds(servoData % 10000);
                break;
              case 2:
                if (!CH2_Interrupts_Is)
                  servo_2.writeMicroseconds(servoData % 10000);
                break;
              case 3:
                servo3_new = (servoData % 10000);
                if (!(servo3_new == servo3_old))
                {
                  if (!CH3_Interrupts_Is)
                  {
                    servo_3.writeMicroseconds(servo3_new);
                    servo3_old = servo3_new ;
                  }
                }
                break;
              case 4:
                if (!CH4_Interrupts_Is)
                  servo_4.writeMicroseconds(servoData % 10000);
                break;
              case 5:
                servo_5.writeMicroseconds(servoData % 10000);
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


void resetFlags()
{
  CH1_Interrupts_Is = false;
  CH2_Interrupts_Is = false;
  CH3_Interrupts_Is = false;
  CH4_Interrupts_Is = false;
}


ICACHE_RAM_ATTR void CH1_INTERRUPT()
{
  int channelNew = pulseIn(CH1_Input, HIGH);

  if ( (CH1_Receiver_Old < (channelNew - interuptSensitivity)) || (CH1_Receiver_Old > (channelNew + interuptSensitivity)) )
  {
    CH1_Interrupts_Is = true;
    servo_1.writeMicroseconds(CH1_Receiver_Old);
    CH1_Receiver_Old = channelNew;
  }
}

ICACHE_RAM_ATTR void CH2_INTERRUPT()
{
  int channelNew = pulseIn(CH2_Input, HIGH);

  if ( (CH2_Receiver_Old < (channelNew - interuptSensitivity)) || (CH2_Receiver_Old > (channelNew + interuptSensitivity)) )
  {
    CH2_Interrupts_Is = true;
    servo_2.writeMicroseconds(CH2_Receiver_Old);
    CH2_Receiver_Old = channelNew;
  }
}

ICACHE_RAM_ATTR void CH3_INTERRUPT()
{
  int channelNew = pulseIn(CH3_Input, HIGH);

  if ( (CH3_Receiver_Old < (channelNew - interuptSensitivity)) || (CH3_Receiver_Old > (channelNew + interuptSensitivity)) )
  {
    CH3_Interrupts_Is = true;
    servo_3.writeMicroseconds(CH3_Receiver_Old);
    CH3_Receiver_Old = channelNew;
  }
}

ICACHE_RAM_ATTR void CH4_INTERRUPT()
{
  int channelNew = pulseIn(CH4_Input, HIGH);

  if ( (CH4_Receiver_Old < (channelNew - interuptSensitivity)) || (CH4_Receiver_Old > (channelNew + interuptSensitivity)) )
  {
    CH4_Interrupts_Is = true;
    servo_4.writeMicroseconds(CH4_Receiver_Old);
    CH4_Receiver_Old = channelNew;
  }
}
