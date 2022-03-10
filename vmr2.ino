#include <SPI.h>
#include <SD.h>
#define INTERVAL_1 10
#define INTERVAL_2 1000
#define INTERVAL_3 60000
unsigned long time_1=0, time_2=0, time_3=0;
File myFile;
int analogInput = 0;
String Measurement = "Measurement", Space="  ";
float Vout = 0.000, Iout = 0.00, Wvyst=0.00 , WH=0.00, Vin, nap, R1 = 3, R2 = 22,Vref=1.1;
bool FirstMeasurement = true;

void setup()
{
  pinMode(analogInput, INPUT);
  Serial.begin(57600); 
  if (!SD.begin(4)) //If SD card is damaged or not inserted, do not continue
  {
    Serial.println("initialization failed!");
    while (1);
  }
  myFile = SD.open(Measurement, FILE_WRITE);
  Serial.println("Card inserted and it is working!");
  analogReference(INTERNAL);
}

void print_time(unsigned long time_millis); //only showed in serial monitoring

void loop()
{     
      if(millis() >= time_1 + INTERVAL_1)
      {
        time_1 +=INTERVAL_1;
        measurement();
      } 
      if(millis() >= time_2 + INTERVAL_2)
      {
        time_2 +=INTERVAL_2;
        print_time(time_2);
        //Serial.println("prebieha vypocet");
        calculate();
      }
      if(millis() >= time_3 + INTERVAL_3)
      {
        time_3 +=INTERVAL_3;
        print_time(time_3);
        Serial.println("Saving data on SD card");
        sdcard();
      }
          
}

void measurement()
{   
    if ((analogRead(analogInput) <40)&(analogRead(analogInput) >35))Vin = ((analogRead(analogInput)+5) * Vref) / 1024.00;
    else if ((analogRead(analogInput) <35)&(analogRead(analogInput) >20))Vin = ((analogRead(analogInput)+1) * Vref) / 1024.00;
    else if ((analogRead(analogInput) <20)&(analogRead(analogInput) >3))Vin = ((analogRead(analogInput)+2) * Vref) / 1024.00;
    else if (analogRead(analogInput) > 0)Vin = ((analogRead(analogInput)) * Vref) / 1024.00;
    else Vin = ((analogRead(analogInput)) * Vref) / 1024.00;
    Vin /= (R2/(R1+R2));
    nap+= Vin;
}

void calculate()
{
  nap = nap/100; //Median of voltage
  Serial.print("Median of voltage ");
  Serial.println(nap*1000); // display in mV in serial
  Vout=nap*1000; Iout =(nap)/(R1+R2); Wvyst = (nap) * (Iout); Iout*=1000; //V -> mV, A , W, A -> mA, 
  
  WH += (Wvyst*1000); // W -> mW
  WH /= 3600;      // mW + time = mWH
  WH *= 1000000;    // mW -> nWH (E-9)
  if (FirstMeasurement == true) 
    {
      myFile.println("##########Measurement##########");
      FirstMeasurement=false;
    }
  myFile.println(Vout+Space+Iout+Space+WH); //mV, mA, nWH
}

void sdcard()
{
  myFile.close(); //Save data
  Serial.println("Súbor uložený: " + Measurement); 
  myFile = SD.open(Measurement, FILE_WRITE); //Open file and continue where ended
}

void print_time(unsigned long time_millis)
{
    Serial.print("Time: ");
    Serial.print(time_millis/1000);
    Serial.print("s - ");
}
