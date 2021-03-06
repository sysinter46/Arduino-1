/*
https://webusb.github.io/arduino/
The WebUSB library provides all the extra low-level USB code necessary for WebUSB support except for one thing: 
Your device must be upgraded from USB 2.0 to USB 2.1. 
To do this go into the SDK installation directory and open hardware/arduino/avr/cores/arduino/USBCore.h. 
Then find the line #define USB_VERSION 0x200 and change 0x200 to 0x210. That’s it!
WebUSB Library
https://github.com/webusb/arduino
*/

#include <WebUSB.h>
WebUSB WebUSBSerial(1 /* https:// */, "fustyles.github.io/webduino/myBlockly/");
#define Serial WebUSBSerial

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

String ReceiveData="", Command="",cmd="",P1="",P2="",P3="",P4="",P5="",P6="",P7="",P8="",P9="";
boolean debug = true;

void setup()
{
  while (!Serial) {;}
  Serial.begin(9600);
  SendData("Device is Connected.");
}

void loop() 
{
  getCommand();

  if (ReceiveData.indexOf("?")==0)
  {
  if (cmd=="yourcmd")
    {
      //you can do anything
      //if (debug == true) SendData("[{\"data\":\""+Command+"\"}]");
    } 
  else if (cmd=="inputpullup")
    {
      pinMode(P1.toInt(), INPUT_PULLUP);
      if (debug == true) SendData("[{\"data\":\""+Command+"\"}]");
    }  
  else if (cmd=="pinmode")
    {
      pinMode(P1.toInt(), P2.toInt());
      if (debug == true) SendData("[{\"data\":\""+Command+"\"}]");
    }        
  else if (cmd=="digitalwrite")
    {
      pinMode(P1.toInt(), OUTPUT);
      digitalWrite(P1.toInt(),P2.toInt());
      if (debug == true) SendData("[{\"data\":\""+Command+"\"}]");
    }   
  else if (cmd=="digitalread")
    {
      SendData("[{\"data\":\""+String(digitalRead(P1.toInt()))+"\"}]");
    }    
  else if (cmd=="analogwrite")
    {
      pinMode(P1.toInt(), OUTPUT);
      analogWrite(P1.toInt(),P2.toInt());
      if (debug == true) SendData("[{\"data\":\""+Command+"\"}]");
    }       
  else if (cmd=="analogread")
    {
      SendData("[{\"data\":\""+String(analogRead(P1.toInt()))+"\"}]");
    } 
   else if (cmd=="i2cLcd") {
    P1.toLowerCase();
    //You must convert hex value(P1) to decimal value.
    if (P1=="0x27") 
      P1="39";
    else if (P1=="0x3f") 
      P1="63";
     
    LiquidCrystal_I2C lcd(P1.toInt(),16,2);
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(P4);
    lcd.setCursor(0,1);
    lcd.print(P5);
    if (debug == true) SendData("[{\"data\":\""+P4+"\"},{\"data\":\""+P5+"\"}]");
  }
  else if (cmd=="car")    // ?car=pinL1;pinL2;pinR1;pinR2;L_speed;R_speed;Delay;state
    {
      pinMode(P1.toInt(), OUTPUT);
      pinMode(P2.toInt(), OUTPUT);
      pinMode(P3.toInt(), OUTPUT);
      pinMode(P4.toInt(), OUTPUT);
      digitalWrite(P1.toInt(), 0);
      digitalWrite(P2.toInt(), 0);
      digitalWrite(P3.toInt(), 0);
      digitalWrite(P4.toInt(), 0);
      delay(10);
    
      if (P8=="S")
      {
        //
      }
      else if  (P8=="F")
      {
        analogWrite(P1.toInt(),P5.toInt());
        analogWrite(P2.toInt(),0);
        analogWrite(P3.toInt(),0);
        analogWrite(P4.toInt(),P6.toInt());       
        if ((P7!="")&&(P7!="0"))
        {
          delay(P7.toInt());
          analogWrite(P1.toInt(),0);
          analogWrite(P4.toInt(),0);          
        }     
      }
      else if  (P8=="B")
      {
        analogWrite(P1.toInt(),0);
        analogWrite(P2.toInt(),P5.toInt());
        analogWrite(P3.toInt(),P6.toInt());
        analogWrite(P4.toInt(),0);  
        if ((P7!="")&&(P7!="0"))
        {
          delay(P7.toInt());
          analogWrite(P2.toInt(),0);
          analogWrite(P3.toInt(),0);         
        }     
      }
      else if  (P8=="L")
      {
        analogWrite(P1.toInt(),0);
        analogWrite(P2.toInt(),P5.toInt());
        analogWrite(P3.toInt(),0);
        analogWrite(P4.toInt(),P6.toInt());         
        if ((P7!="")&&(P7!="0"))
        {
          delay(P7.toInt());
          analogWrite(P2.toInt(),0);
          analogWrite(P4.toInt(),0);          
        }
      }
      else if  (P8=="R")
      {
        analogWrite(P1.toInt(),P5.toInt());
        analogWrite(P2.toInt(),0);
        analogWrite(P3.toInt(),P6.toInt());
        analogWrite(P4.toInt(),0);
        if ((P7!="")&&(P7!="0"))
        {
          delay(P7.toInt());
          analogWrite(P1.toInt(),0);
          analogWrite(P3.toInt(),0);       
        }        
      }
      if (debug == true) SendData("[{\"data\":\""+Command+"\"}]");
    }    
  else 
    {
      SendData("[{\"data\":\"Command is not defined\"}]");
    }  
  }
}

void SendData(String data)
{
  Serial.println(data);
  Serial.flush();
}

void getCommand()
{
  ReceiveData="";Command="";cmd="";P1="";P2="";P3="";P4="";P5="";P6="";P7="";P8="";P9="";
  byte ReceiveState=0,cmdState=1,PState=1,questionstate=0,equalstate=0,semicolonstate=0;
  
  if (Serial.available())
  {
    while (Serial.available())
    {
      char c=Serial.read();
      ReceiveData=ReceiveData+String(c);
      
      if (c=='?') ReceiveState=1;
      if ((c==' ')||(c=='\r')||(c=='\n')) ReceiveState=0;
      
      if (ReceiveState==1)
      {
        Command=Command+String(c);
        
        if (c=='=') cmdState=0;
        if (c==';') PState++;

        if ((cmdState==1)&&((c!='?')||(questionstate==1))) cmd=cmd+String(c);
        if ((cmdState==0)&&(PState==1)&&((c!='=')||(equalstate==1))) P1=P1+String(c);
        if ((cmdState==0)&&(PState==2)&&(c!=';')) P2=P2+String(c);
        if ((cmdState==0)&&(PState==3)&&(c!=';')) P3=P3+String(c);
        if ((cmdState==0)&&(PState==4)&&(c!=';')) P4=P4+String(c);
        if ((cmdState==0)&&(PState==5)&&(c!=';')) P5=P5+String(c);
        if ((cmdState==0)&&(PState==6)&&(c!=';')) P6=P6+String(c);
        if ((cmdState==0)&&(PState==7)&&(c!=';')) P7=P7+String(c);
        if ((cmdState==0)&&(PState==8)&&(c!=';')) P8=P8+String(c);
        if ((cmdState==0)&&(PState>=9)&&((c!=';')||(semicolonstate==1))) P9=P9+String(c);
        
        if (c=='?') questionstate=1;
        if (c=='=') equalstate=1;
        if ((PState>=9)&&(c==';')) semicolonstate=1;
      }
    }  
  }
}
