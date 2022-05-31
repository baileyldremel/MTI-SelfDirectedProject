// This project features code from the following (which has been altered to fit the project)

// Resources related to the PWM module
// Code from the examples related to the Adafruit PWM Servo Driver have been used and adapted for this project
// https://learn.adafruit.com/16-channel-pwm-servo-driver/hooking-it-up
// https://learn.adafruit.com/16-channel-pwm-servo-driver/library-reference

// Resources related to the RTC module
// Code from the DS1307RTC library examples have been used and adapted for this project.

// Resources related to the RFID module
// Code from the examples from the MFRC522 library example have been used and adapted for this project
// https://www.hackster.io/Aritro/security-access-using-rfid-reader-f7c746

// Resources related to other bits of code
// Writing to a specific I2C address
// https://arduino.stackexchange.com/questions/68449/how-do-you-write-to-a-specific-register-using-i2c-communcation


// EVERYTHING RELATED TO THE PWM SERVO DRIVER
// The library I am using for the driver
#include <Adafruit_PWMServoDriver.h>

// This makes it easier to call for the pwm driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Since I am using 2 modules that require I2C for communication, I need to point to a specific address
// Thse are the addresses for the PWM module
#define PWM_CHIP 0x40
#define PWM_REG 0x70

/* These are the PWM pins for each of the LEDs
/* Although it might be quicker for me to call the number instead, using variables makes it easier
/* to understand what it is controlling. */

// Top Left LED
int tlLEDR = 0;
int tlLEDG = 1;
int tlLEDB = 2;

// Top Right LED
int trLEDR = 3;
int trLEDG = 4;
int trLEDB = 5;

// Bottom Left LED
int blLEDR = 6;
int blLEDG = 7;
int blLEDB = 8;

// Bottom Right LED
int brLEDR = 9;
int brLEDG = 10;
int brLEDB = 11;

// EVERYTHING RELATED TO THE RTC MODULE
// The two libraries for the RTC module
#include <DS1307RTC.h>
#include <TimeLib.h>

// These are the addresses to point to for the module, since I'm using two modules that need I2C
#define RTC_CHIP 0x50
#define RTC_REG 0x68

// These variables are all for assigning time values to

// These are constantly called for assigning the current times to it
int currentSec;
int currentMin;
int currentHour;

// These are called when a timer needs to be set. It stores the current time plus the timer to it
int timerSec;
int timerMin;
int timerHour;

// This stores what the past time and used to see a change in time
int prevSec;
int prevMin;

// EVERYTHING RELATED TO THE RFID/NFC MODULE
// A lot of the code used for this project is related to this, but has been adapted for this project (eg variable names and such)
// https://www.hackster.io/Aritro/security-access-using-rfid-reader-f7c746
// These are the two libraries I am using for the module
#include <SPI.h>
#include <MFRC522.h>

// These are the two digital pins used for most of the communication for the RFID/NFC module

// The SS pin acts as a signal to the Arduino
#define SS_PIN 10
// The RST pin turns of the input pins
#define RST_PIN 9

// Using the defined pins above, this makes it easier to call the module in the code.
MFRC522 rfid(SS_PIN, RST_PIN);

// OTHER VARIABLES

// These are the Serial numbers/ID for the tags I am using.
// Each of these corresponds to a timer in the code.
String rfid1min = "53 82 45 0A 50 00 01";
String rfid3min = "53 27 96 0B 50 00 01";
String rfid5min = "53 80 45 0A 50 00 01";
String rfid15min = "53 81 45 0A 50 00 01";
String rfid30min = "53 78 45 0A 50 00 01";

String rfidStopAlarm = "53 8B 45 0A 50 00 01";

// These arrays store the colours that are used for the program
// Storing it in an array makes it easier to change colours when needed and to know what's been called.
int oneMinTimer[] = {255, 84, 181};
int threeMinTimer[] = {255, 150, 0};
int fiveMinTimer[] = {0, 255, 0};
int fifteenMinTimer[] = {0, 255, 221};
int thirtyMinTimer[] = {200, 0, 255};
int sixtyMinTimer[] = {123, 255, 0};


//  These are for assigning colour values.
int ledColR;
int ledColG;
int ledColB;

// This is for assigning the RFID value to it aka the timer that is being used.
int rfidVal;

// This is a third of what the RFID value is, which is used to switch between LEDS
int divideVal;

// This controls the brightness of the LEDS.
float brightness;

// This is used for the minute timer
int i = 0;
int j = 0;

// This is used for when the timer is inactive
int k = 0;

// This is used for the seconds timer
int l =0;
int m =0;

// Used to assign the RFID ID to it
String currentRFID;

// Booleans for use throughout the code
// To check and see if the alarm LED is on or off.
bool alarmLEDOn = false;

// To set the alarm when the timer has gone off
bool alarm = false;

// To check if the time is being activated or not
bool timerActive = false;

// To check if the timer is On or Off. It also determines what timer is on.
bool minTimerOn = false;
bool secTimerOn = false;

/* A lot of the setup is to start all of the modules that are connected to the Arduino */
void setup() {

  // Serial monitor (not really needed for the program but is good for bug testing and monitoring things
  Serial.begin(9600);

  // RFID module
  SPI.begin();
  rfid.PCD_Init();

  // PWM module
  
  // Since I am using two modules needing I2C, this code begins a communication to that address specifically
  Wire.beginTransmission(PWM_CHIP);
  // These two will write to the specific address
  Wire.write(PWM_REG);
  Wire.write(0x70);

  // Begins the module
  pwm.begin();
  // Sets the frequency to 1000, which is for the LEDS
  pwm.setPWMFreq(1000);

  // This stops communication to that address.
  Wire.endTransmission();

  // Setting the pin mode for the buzzer
  pinMode(4,OUTPUT);

}

void loop() {

  // This is when the program begins
  // It strobes the LED's on
  k++;
  if(k == 100){
    k = 0;
  }
  setTLLED(k,k,k,2);
  setTRLED(k,k,k,0.5);             
  setBLLED(k,k,k,0.5);
  setBRLED(k,k,k,0.5);
  noTone(4);

  // This checks to see if a card is present or not.
  // If nothing is present, it just continues the code
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  // When a card is present, the following happens
  }else{
    // The buzzer's tone is set to a tone that resembles the note C
    tone(4,261);

    // Turns off all the LEDS
    setTLLED(0,0,0,0);
    setTRLED(0,0,0,0);
    setBLLED(0,0,0,0);
    setBRLED(0,0,0,0);
  }

  // If it cannot read the cards serial, it just continues
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // When a card is present, I want to read what the UID is and assign it to something so I can check it.
  // This does that by making a new string and assigning each letter of the UID to it.
  String content = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }

  // To compare it to, I want to make sure all the letters in the string are uppercase.
  content.toUpperCase();
  
  // This bit of codes checks to see if the current RFID stored is not what has been tapped onto the module.
  if (content.substring(1) != currentRFID) {
    // Sets the UID of the tapped tag to a variable
    currentRFID = content.substring(1);

    // For testing purposes, I want it to tell me what the tags ID is.
    Serial.print("RFID CODE: ");
    Serial.print(currentRFID);
    Serial.println();
  }

  // All these bits of code check to see what timer needs to be set. If the UID matches the one for the timer, it executes the code
  if (currentRFID == rfid15min) {

    // Set the RFID val to 15, which is used for setting the timer
    rfidVal = 15;
    
    // Set the divide value, which is a third of the timer
    divideVal = 5;
    
    // Reset the brightness. It's negetive because the first step of the timer is to add 0.2 to the brightness.
    brightness = -0.2;
    
    // The timer is active
    timerActive = true;
    
    // The colour code for that timer
    setTLLED(0,255,251,1);
  }

  // This is the same as above, but with different values
  if (currentRFID == rfid30min) {
    rfidVal = 30;
    divideVal = 10;
    brightness = -0.2;
    timerActive = true;
    setTLLED(200,0,255,1);
  }

  
  if (currentRFID == rfid5min) {
    rfidVal = 5;

    //This is a bit different as the a third of 5 is a decimal number and would lead to using seconds to change the led brightness
    //Since this will be in a different loop, the divide value is greater (it is a third of the total seconds for the timer)
    //In this case, 300 seconds for the total timer, so 100 for each LED.
    divideVal = 100;
    brightness = -0.2;
    timerActive = true;
    l=0;
    setTLLED(0, 255, 0,1);
  }

  if (currentRFID == rfid3min) {
    rfidVal = 3;
    divideVal = 60;
    brightness = -0.2;
    timerActive = true;
    l=0;
    setTLLED(0, 0, 255,1);
  }

  if (currentRFID == rfid1min) {
    rfidVal = 1;
    divideVal = 20;
    brightness = -0.2;
    timerActive = true;
    l=0;
    setTLLED(0, 0, 255,1);
  }

  if (currentRFID == rfidStopAlarm && timerActive){
    rfidVal = 0;
    divideVal = 0;
    brightness = 0;
    timerActive = false;
    alarm = false;
    setTLLED(0,0,0,1);
    noTone(4);
  }

  // This will only run if the timer has been activated
  if (timerActive == true) {

    // This checks the RTC modules to see if it is running properly and is connected.
    if(checkRTC() == false){
      Serial.println("Please check wiring and ensure the RTC module's time is set before continuing and try again");
      // Keep checking it until it's not true.
      while(0){checkRTC();}
    }

    // This is for testing to make sure the timer will start.
    Serial.print(rfidVal);
    Serial.print(" minute timer set");
    Serial.println();

    // Get the current second for the timer
    timerSec = callSec();

    // Get the current minute for the timer but add the amount of time set for the timer
    timerMin = callMin() + rfidVal;

    // Get the current hour for the timer
    timerHour = callHour();

    Serial.print("Timer will go off at ");
    Serial.print(timerHour);
    Serial.print(":");
    Serial.print(timerMin);
    Serial.print(":");
    Serial.print(timerSec);
    Serial.println();
    
    // If the timer minute is over 60, we need to add an hour to the timer and reduce the timer minute by 60 minutes
    if (timerMin >= 60) {
      timerMin = timerMin - 60;
      timerHour = timerHour + 1;

      // But if the timer hour is equal to 24 (the module uses 24 hour time), it sets it to 0;
      if (timerHour == 24) {
        timerHour = 0;
      }
    }
    
    // Setting the colour values depending on what timer is set.
    // It also determines which timer loop we are using
    if(rfidVal == 1){
      ledColR = oneMinTimer[0]/5;
      ledColG = oneMinTimer[1]/5;
      ledColB = oneMinTimer[2]/5;
      secTimerOn = true;
    }
    
    if(rfidVal == 3){
      ledColR = threeMinTimer[0]/5;
      ledColG = threeMinTimer[1]/5;
      ledColB = threeMinTimer[2]/5;
      secTimerOn = true;
    }
    
    if(rfidVal == 5){
      ledColR = fiveMinTimer[0]/5;
      ledColG = fiveMinTimer[1]/5;
      ledColB = fiveMinTimer[2]/5;
      secTimerOn = true;
      
    }
    
    if (rfidVal == 15) {
      ledColR = fifteenMinTimer[0] / 5;
      ledColG = fifteenMinTimer[1] / 5;
      ledColB = fifteenMinTimer[2] / 5;
      minTimerOn = true;
    }

    if (rfidVal == 30) {
      ledColR = thirtyMinTimer[0] / 5;
      ledColG = thirtyMinTimer[1] / 5;
      ledColB = thirtyMinTimer[2] / 5;
      minTimerOn = true;
    }

    // This loop is for the timers that require seconds to be used to change the LEDS
    while(secTimerOn){

      // Turns off the buzzer, just in case it's still on
      noTone(4);

      // Drawing the current second and minute to variables
      currentSec = callSec();
      currentMin = callMin();

      // This will only do something every second
      if(currentSec != prevSec){
        l++;
        m++;
        prevSec = currentSec;

        //When l is higher it will make the LED that it's currently on brighter.
        if(l >= divideVal/5){
          brightness = brightness +0.2;
          l = 0;
          if(brightness == 1){
            brightness = 0;
          }
        }   
      }

        setTLLED(ledColR, ledColG, ledColB, 2);

        // This bit of code changes what LED is lit. For example, for a 15 minute timer, it j = 7, the bottom left LED should be the one lighting up
        // and the top left LED should be fully it
        if (m < (divideVal*1) + 2) {
          setTRLED(ledColR, ledColG, ledColB, brightness);
        }

        if (m >= (divideVal*1)+2 && m < (divideVal*2)+1) {
          setBLLED(ledColR, ledColG, ledColB, brightness);
        }

        if (m >= (divideVal*2)+1) {
          setBRLED(ledColR, ledColG, ledColB, brightness);
        }

      if(currentMin == timerMin && currentSec == timerSec){
        secTimerOn = false;
        alarm = true;
      }
    }
    
    // Since all the values we need have been assigned above, we just need to set the timer.
    while (minTimerOn) {

      // Turn off the buzzer
      noTone(4);

      // Call the current second and minute
      currentSec = callSec();
      currentMin = callMin();

      // This is checking to see if the minute is the same as the timer minute
      if (currentMin != timerMin) {
        // This statement is used for changing the brightness of the LEDs as well as what LED needs to be lit.
        if (currentMin != prevMin && currentSec == timerSec) {
          i++;
          j++;

          // When i is the same as the divide value divided by 5, it brightness the LED by 0.2.
          //  The division is used for defining how many minutes needs to pass before the brightness needs to be changed.
          if (i >= divideVal / 5) {
            brightness = brightness + 0.2;
            i = 0;

            // If brightness is 1 (AKA the max brightness for these LEDS, reset it back to 0
            if (brightness == 1) {
              brightness = 0;
            }
          }
//          Serial.println(brightness);
//          Serial.println(j);

          // Store what the current minute is.
          prevMin = currentMin;
        }

        // So the user knows what timer is activated, this LED will always be on and always be the colour of the timer (but brighter)
        setTLLED(ledColR, ledColG, ledColB, 2);

        // This bit of code changes what LED is lit. For example, for a 15 minute timer, it j = 7, the bottom left LED should be the one lighting up
        // and the top left LED should be fully it
        if (j < (divideVal* 1) + 1) {
          setTRLED(ledColR, ledColG, ledColB, brightness);
        }

        if (j >= (divideVal*1)+1 && j < (divideVal*2)+1) {
          setBLLED(ledColR, ledColG, ledColB, brightness);
        }

        if (j >= (divideVal*2)+1) {
          setBRLED(ledColR, ledColG, ledColB, brightness);
        }
      }

      // This checks to see if the current time is exactly the same as the one we set for the timer. This turns off the timer
      if (currentMin == timerMin && currentSec == timerSec){
        minTimerOn = false;
        alarm = true;
      }
    }
  }

  // The code only reaches this point after the timer has gone off.  
  
  

  // The loop for the alarm
  while(alarm){
    // Reset the values used for the timer
    j = 0;
    i = 0;
    l = 0;
    m = 0;

    // For the alarm, I only need to call the second
    currentSec = callSec();

    // This statement is so the action only happens every second
    if(currentSec != prevSec){

      // This flicks all the LEDs Red on and off every second. It also turns on and off the buzzer
      if(alarmLEDOn){
        setTLLED(0,0,0,0);
        setTRLED(0,0,0,0);
        setBLLED(0,0,0,0);
        setBRLED(0,0,0,0);
        alarmLEDOn = false;
        noTone(4);
        }else{
        setTLLED(255,0,0,1);
        setTRLED(255,0,0,1);
        setBLLED(255,0,0,1);
        setBRLED(255,0,0,1);
        alarmLEDOn = true;
        tone(4,440);
        }
      prevSec = currentSec;
      }

      // Since we assume if there is a card on the RFID module means they want to do something,
      // We turn off the alamr for it.
      if (rfid.PICC_IsNewCardPresent()) {
      alarm = false;
      setTLLED(0,0,0,0);
      setTRLED(0,0,0,0);
      setBLLED(0,0,0,0);
      setBRLED(0,0,0,0);
      k = 0;
      }
   }
}


// There are four of these (for all the LEDS), but they are all practically the same (other than the pins it talks to)
// The input is the RGB colour values as well as what the brightness is.
void setTLLED(int r, int g, int b, float bright) {
  
  // Assign it to values
  // I multiply the brightness by 16 because the module goes from 0 to 4095, but the colours are written from 0 to 255.
  int ledRed = r * (bright*16);
  int ledGreen = g * (bright*16);
  int ledBlue = b * (bright*16);

  // Begin communication to the PWM module and write to it
  Wire.beginTransmission(PWM_CHIP);
  Wire.write(PWM_REG);
  Wire.write(0x70);

  // Write the PWM pin. The first number is the pin number I am writing to.
  // The second number isn't used
  // The third number is the colour of the specific coloured LED. 0 is off and 4095 is fully bright
  pwm.setPWM(tlLEDR, 0, ledRed);
  pwm.setPWM(tlLEDG, 0, ledGreen);
  pwm.setPWM(tlLEDB, 0, ledBlue);

  // Finish the transmission
  Wire.endTransmission();
}

void setTRLED(int r, int g, int b, float bright) {
  int ledRed = r * (bright*16);
  int ledGreen = g * (bright*16);
  int ledBlue = b * (bright*16);

  Wire.beginTransmission(PWM_CHIP);
  Wire.write(PWM_REG);
  Wire.write(0x70);

  pwm.setPWM(trLEDR, 0, ledRed);
  pwm.setPWM(trLEDG, 0, ledGreen);
  pwm.setPWM(trLEDB, 0, ledBlue);

  Wire.endTransmission();
}

void setBLLED(int r, int g, int b, float bright) {
  int ledRed = r * (bright*16);
  int ledGreen = g * (bright*16);
  int ledBlue = b * (bright*16);

  Wire.beginTransmission(PWM_CHIP);
  Wire.write(PWM_REG);
  Wire.write(0x70);

  pwm.setPWM(blLEDR, 0, ledRed);
  pwm.setPWM(blLEDG, 0, ledGreen);
  pwm.setPWM(blLEDB, 0, ledBlue);

  Wire.endTransmission();
}

void setBRLED(int r, int g, int b, float bright) {
  int ledRed = r *(bright*16);
  int ledGreen = g*(bright*16);
  int ledBlue = b*(bright*16);

  Wire.beginTransmission(PWM_CHIP);
  Wire.write(PWM_REG);
  Wire.write(0x70);

  pwm.setPWM(brLEDR, 0, ledRed);
  pwm.setPWM(brLEDG, 0, ledGreen);
  pwm.setPWM(brLEDB, 0, ledBlue);

  Wire.endTransmission();
}

// This checks the RTC module to see if it's running or not.
bool checkRTC(){

  // Time elements
  tmElements_t tm;

  // Start talking to the address for the RTC module
  Wire.beginTransmission(RTC_CHIP);
  Wire.write(RTC_REG);
  Wire.write(0x68);

  // If it can read the time, then return true.
  if(RTC.read(tm)){
      return true;
  // If not, return false.
  }else {
      return false;
  }

  // Stop talking to that address.
  Wire.endTransmission();
}

// There are 3 of these, which function the same but are for a different value (eg seconds, minutes, hours)
int callSec(){

  // Value to assign the time to
  int secResult;

  // Time elements
  tmElements_t tm;

  // Start talking to the RTC modules' address
  Wire.beginTransmission(RTC_CHIP);
  Wire.write(RTC_REG);
  Wire.write(0x68);

  // Read the second
  if(RTC.read(tm)){
    // Assign the second to the secondResult
    secResult = tm.Second;
  }

  // Stop talking to that address
  Wire.endTransmission();

  // Return the value it pulled from the RTC module
  return secResult;
}

int callMin(){
  int minResult;

  tmElements_t tm;

  Wire.beginTransmission(RTC_CHIP);
  Wire.write(RTC_REG);
  Wire.write(0x68);

  if(RTC.read(tm)){
    minResult = tm.Minute;  
  }
  
  Wire.endTransmission();

  return minResult;
}

int callHour(){
  int hourResult;

  tmElements_t tm;

  Wire.beginTransmission(RTC_CHIP);
  Wire.write(RTC_REG);
  Wire.write(0x68);

  if(RTC.read(tm)){
    hourResult = tm.Hour;  
  }
  
  Wire.endTransmission();

  return hourResult;
}
