#include <SPI.h>
#include <SD.h>
#define INTERVAL_1 10           //interval lenght [ms] - 10 means it repeats 100 times per second
#define INTERVAL_2 1000
#define INTERVAL_3 60000
unsigned long time_1=0, time_2=0, time_3=0;                        //do not change
File myFile;                                                       //change for file name
int analogInput = 0;                                               //analog pin (do not use 4 or 5)
String Measurement = "Measurement", Space="  ";                    //saved string for less code writing :)
float Vout = 0.000, Iout = 0.00, Wvyst=0.00 , WH=0.00, 
float Vin, Voltage;               
float R1 = 3, R2 = 22;          //change values of R1 and R2 (MUST BE SAME AS IN CIRCUIT!)
float Vref=1.1;                 //reference voltage 
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
  Serial.println("Card inserted and it is working!"); //measurement can start
  analogReference(INTERNAL);
}

void print_time(unsigned long time_millis); //only showed in serial monitoring

void loop()
{     
      if(millis() >= time_1 + INTERVAL_1) //measure every 10ms
      {
        time_1 +=INTERVAL_1;
        measurement();
      } 
      if(millis() >= time_2 + INTERVAL_2) //calculate every second
      {
        time_2 +=INTERVAL_2;
        print_time(time_2);
        //Serial.println("calculation is in progress");  //remove comment for debugging, leave it for faster processing  
        calculate();
      }
      if(millis() >= time_3 + INTERVAL_3)   //save data every 60 seconds
      {
        time_3 +=INTERVAL_3;
        print_time(time_3);
        //Serial.println("Saving data on SD card");   //remove comment for debugging, leave it for faster processing  
        sdcard();                                     //calling function for saving
      }
          
}

void measurement()
{   //here you can calibrate ADC, you must measure along with multimeter for more accurate measurement
    if ((analogRead(analogInput) <40)&(analogRead(analogInput) >35))Vin = ((analogRead(analogInput)+5) * Vref) / 1024.00;
    else if ((analogRead(analogInput) <35)&(analogRead(analogInput) >20))Vin = ((analogRead(analogInput)+1) * Vref) / 1024.00;
    else if ((analogRead(analogInput) <20)&(analogRead(analogInput) >3))Vin = ((analogRead(analogInput)+2) * Vref) / 1024.00;
    else if (analogRead(analogInput) > 0)Vin = ((analogRead(analogInput)) * Vref) / 1024.00;
    else Vin = ((analogRead(analogInput)) * Vref) / 1024.00;
    Vin /= (R2/(R1+R2));
    Voltage+= Vin;
}

void calculate()    //change if you know what are you doing
{     
  Voltage = Voltage/100; //Median of voltage
  //Serial.print("Median of voltage ");   //remove comment for debugging, leave it for faster processing  
  //Serial.println(Voltage*1000);         // display in mV in serial
  Vout=Voltage*1000; Iout =(Voltage)/(R1+R2); Wvyst = (Voltage) * (Iout); Iout*=1000; //V -> mV, A , W, A -> mA, 
  
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
  myFile.close();                                   //Save data
  //Serial.println("File saved: " + Measurement);   //remove comment for debugging, leave it for faster processing
  myFile = SD.open(Measurement, FILE_WRITE);        //Open file and continue where ended
}

void print_time(unsigned long time_millis)
{
    //Serial.print("Time: ");                       //remove comment for debugging, leave it for faster processing
    //Serial.print(time_millis/1000);
    //Serial.print("s - ");
}
