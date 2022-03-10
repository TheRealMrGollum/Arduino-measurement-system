#include <SPI.h>
#include <SD.h>

File myFile;
int analogInput = 0;
String Meranie = "Meranie";
int sekundy=-1, minuty=0, hodiny=0;
float Vvyst = 0.000, Ivyst = 0.00, Wvyst=0.00 , WH=0.00, mWspolu;
float Vout, Vin;
float R1 = 3, R2 = 22,Vref=1.1;
int val = 0;
bool prveMeranie = true;
void setup(){
   pinMode(analogInput, INPUT);
   Serial.begin(9600); 
   if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  myFile = SD.open(Meranie, FILE_WRITE);
  Serial.println("Begin");
  analogReference(INTERNAL);
  
}
void loop(){
    //Počítanie
    if (analogRead(analogInput) > 0)
    Vout = ((analogRead(analogInput)+1) * Vref) / 1024.00;
    else 
    Vout = ((analogRead(analogInput)) * Vref) / 1024.00;
    Vin = (Vout) / (R2/(R1+R2)); 
    Vvyst = ((Vin)*1000);
    
    // prúd
    Ivyst = (Vin)/(R1+R2);
    Ivyst = Ivyst * 1000;
    // energia
    Wvyst = (Vvyst/1000) * (Ivyst/1000);
    // cas
    sekundy += 1;
    
    
    if (sekundy == 60) 
      {
        minuty += 1;
        sekundy = 0;
      }
    if (minuty == 60)
      {
        hodiny +=1;
        minuty=0;
      }
    // Veľkosť vytvorenej energie
    mWspolu += (Wvyst*1000);
    WH = mWspolu /3600;
    WH = WH*1000000;
    Serial.print(Vvyst);
    Serial.print("  ");
    Serial.print(Ivyst);
    Serial.print("  ");
    Serial.println(WH);
     
  
//////////////////////////////////
// SD karta
//////////////////////////////////
    if (prveMeranie == true) 
    {
      myFile.println("##########Meranie##########");
      prveMeranie=false;
    }
    myFile.print(Vvyst);
    myFile.print("  ");
    myFile.print(Ivyst);
    myFile.print("  ");
    myFile.println(WH);
    if ((minuty == 9)|(minuty == 19)|(minuty == 29)|(minuty == 39)|(minuty == 49)|(minuty == 59))
    { 
      if (sekundy == 30)
      {
      myFile.close();
      Serial.println("Súbor uložený: " + Meranie);
      myFile = SD.open(Meranie, FILE_WRITE);
      }
    }
    delay(1000);

}
