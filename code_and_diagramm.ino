/*
   Author: Fabian Schwab
   Version: 1.0
   Required:
      Hardware:
        - Adruino Uno
        - Sparkfun SD shield
        - Adafruit ADXL377 sensor
      Software:
        - SD Library
        - SPI Library
        - ADXL377 Library
   Description:
        - This devices measures the G values and saves it to a *.csv file

*/

#include <SD.h>
#include <SPI.h>
#include <ADXL377.h>

/*
   Senor unsing analog pin A3 for X-axis, A2 for Y-axis and A1 for Z-axis
   You may have to change this pins if you have choosen a different setup on your board
*/
ADXL377 acc(A3, A2, A1);

// The Sparkfun microSD shield uses pin 8 for CS
const int chipSelect = 8;

/*
   This prefixes are used for the folder and file structure.
   Each time the arduion is turned on the folders get a higher number,
   so that each testing sequence is in its own foder.

   For the *.csv files it's the same,
   except that afer each recording the number at the end is increased.
*/
const String folderPrefix = "squ_";
const String filePrefix = "tst_";

//Contains the whole folder name as seen on the SD card.
//It's just there to buffer the name for all iteratinos of the main loop
String folderName;

File file;
int fileCount = 0;
bool isReady;

//Determine the time for the recording of the forces
const int recTimeInSeconds = 10;

/*
   Recording Mode -- work in process
      1. Record all data for the entire time.
      2. Record only the maximum over time.
*/
const int mode = 1;

void setup()
{
  //Serial.print("Initializing SD card... ");
  /*
      Note that even if it's not used as the CS pin, the hardware SS pin
      (10 on most Arduino boards, 53 on the Mega) must be left as an output
      or the SD library functions will not work.
  */
  pinMode(10, OUTPUT);

  if (!SD.begin(chipSelect)) {
    //Serial.println("initialization failed!");
    exit(1);
  }
  //Serial.println("initialization done.");
  isReady = false;
  folderSetup();
}

void folderSetup() {
  int counter = 0;

  //Serial.println("Fetching folder... in case there are some pre existing ones.");

  do {
    folderName = folderPrefix + String(counter);
    counter++;
  } while (SD.exists(folderName));

  if (SD.mkdir(folderName)) {
    //Serial.println(folderName + " created.");
    return folderName;
  }
  //Serial.println("failed creating folder: " + folderName);
  exit(1);
}

void redLED(){
  digitalWrite(7, HIGH);
  digitalWrite(6, LOW);  
}
void greenLED(){
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);  
}

void loop()
{
  greenLED();
  if (digitalRead(2) == HIGH) { 
    isReady = true;
  }  
  if(isReady){
    String fileName = filePrefix + String(fileCount) + ".csv";
    fileCount++;
  
    file = SD.open(folderName + "/" + fileName, FILE_WRITE);
  
    // if file opened, write to it:
    if (file) {
      //Serial.println("Ready to log data...");
      redLED();
      file.println("Time;X;Y;Z;");
			unsigned long start = millis();
      unsigned long limit = recTimeInSeconds * 1000;
     
      while (millis()-start <= limit) {
          file.print(millis()-start);
          file.print(";");
          file.print(acc.getX());
          file.print(";");
          file.print(acc.getY());
          file.print(";");
          file.print(acc.getZ());
          file.println(";");
      }
   
      file.close();
    } else {
      //Serial.print("Cannot open file to log data...");
    }
    isReady = false;
  }
}