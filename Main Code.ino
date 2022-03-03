#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display
SoftwareSerial mySerial(8, 7); //SIM800L Tx & Rx is connected to Arduino #8 & #7

const int trigPin = 9;
const int echoPin = 10;
int dist1,sms=0,rec=0,alert=0,distance,counter=0,x,clean;
long duration;
String msg="";

void setup() {
  // put your setup code here, to run once:
  //setup for the ultrasonic sensor
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input

//setup for the lcd display
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  lcd.print("Hello world");

//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
  delay(1000);
  Serial.println("Setup complete !"); 
  mySerial.println("AT+CSMP=17,167,0,0\r\n");
  updateSerial();
  //mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
 // updateSerial();
 }

void loop() {
  // put your main code here, to run repeatedly:
  sense_level();
  if(sms==1)
  {
   lcd.setCursor(0,0);
   lcd.print("alert sent!");
  recv_sms();
  delay(10000);
  while(rec==1)
  {
    if(mySerial.available())
  {
    msg = mySerial.readString();
    Serial.println(msg);
    int len=msg.length();
    Serial.println(len);
   String msg1;
   int n=51;
   char inChar;
   counter+=1;
   Serial.println("counter:");
   Serial.println(counter);
   //Serial.println("mesage and length printed");
   //Serial.println(n);
   while(n<(len-2))
   {
    inChar=msg.charAt(n);
    msg1+=inChar;
    n++;
   }
   if(counter==2)
   {
     x=msg1.toInt();
     Serial.println(x);
    countdown(x);
    clean=check_cleaned();
    if(clean==0)
    {
      send_alert();
      delay(30000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("complain filled");
      delay(30000);
      Reset();
     }
     else
     {
      lcd.setCursor(0,0);
      lcd.print("bins are cleaned");
      lcd.setCursor(0,1);
      lcd.print("Thank you!!");
      delay(50000);
      Reset();
     }
   }
   else
   {
   Serial.println(msg1);
    }
   }
  }
 }
}

int ultrasonic()
{
  // Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
//Serial.print("Distance: ");
Serial.println(distance);
return distance;
}
void send_sms()
{
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"8104066652\"\r\n");  //change ZZ with country code and xxxxxxxxxxx with phone number to sms
 updateSerial();
  mySerial.println("HELLO bins are full at ||  https://maps.app.goo.gl/sWHZG3zcFERV5Nn99");   //text content
  delay(1000);
  mySerial.println((char)26);
  delay(1000);
  Serial.println("sent");
  sms=1;
  Serial.println(sms);
  Serial.println("exiting send sms");
 }
void recv_sms()
{
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  delay(1000);
  rec=1;
  }

void send_alert()
 {
  //mySerial.println("ATD+918104066652;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
  //updateSerial();
  //delay(20000); // wait for 20 seconds...
  //mySerial.println("ATH"); //hang up
  //updateSerial();
   mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"8104066652\"\r\n");  //change ZZ with country code and xxxxxxxxxxx with phone number to sms
 updateSerial();
  mySerial.println("The bins are not yet cleaned at || https://maps.app.goo.gl/sWHZG3zcFERV5Nn99");   //text content
  delay(1000);
  mySerial.println((char)26);
  delay(1000);
  alert=1;
 }

int check_cleaned()
{
int dist2 = ultrasonic();
int cleaned;
  if(dist2>=18)
  {
  cleaned=1;
  }
  else
  {
  cleaned=0;
  }
 return cleaned;
}

void sense_level()
{
 dist1 = ultrasonic();
   if(dist1 <= 2)
  {
    delay(15000);
  if(dist1 <= 2)
  {
    if(sms==0)
    {
    send_sms();
    delay(10000);
    //recv_sms();
    // delay(80000);
    }
   }
 }
}

void countdown(int mins)
{
  int i,j;
  for(i=mins;i>=0;i--)
  {
    for(j=59;j>=0;j--)
    {
      lcd.setCursor(0,1);
      lcd.print(i);
      lcd.print(":");
      lcd.print(j);
      delay(1000);
    }  
  }
}

void Reset()
{
  sms=0;
  rec=0;
  alert=0;
  clean=0; 
  counter=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("hello world");
}


void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
    
  }
}
