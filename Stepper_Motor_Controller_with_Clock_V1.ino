#include <Wire.h>
#include <Stepper.h>

// Variables to change
int spd = 60; // Motor speed (MAX RPM IS 60);
int angle = 360; // Angle of rotation
int cyc = 3; // Number of cycles
int delayCycle = 1000; // ms delay between cycle

byte secondZ = 61; // Target Second (0 - 59)
byte minuteZ = 61; // Target Minute (0 - 59)
byte hourZ = 25; // Target Hour (0 - 23)
byte dayOfWeekZ = 8; // Target Day Of Week (1 - 7)
byte dayOfMonthZ = 32; // Target Day Of Month (1 - 31) 
byte monthZ = 13; // Target Month (1 - 12)
byte yearZ = 99; // Target Year (00 - 99) -> 20XX

//------------------------------------------------------------
//**************************SETTINGS**************************
int clockAddress = 0x68;  // This is the I2C address
int command = 0;  // This is the command char, in ascii form, sent from the serial port     
long previousMillis = 0;  // will store last time Temp was updated
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
byte test; 

int enA  = 3;  // Enable pin 1 on Motor Control Shield  
int enB  = 11;  // Enable pin 2 on Motor Control Shield  
int dirA = 12;  // Direction pin dirA on Motor Control Shield
int dirB = 13;  // Direction pin dirB on Motor Control Shield
float rev = (float (angle) / 360.0);
const int stepsPerRevolution = 200; 
Stepper myStepper(stepsPerRevolution, dirA, dirB); // Initialize stepper library on pin12&13

//------------------------------------------------------------

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val) {
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val) {
  return ( (val/16*10) + (val%16) );
}

// Set target time and date
void setDateTarget(){
  secondZ = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48)); 
  minuteZ = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
  hourZ  = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
  dayOfWeekZ = (byte) (Serial.read() - 48);
  dayOfMonthZ = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
  monthZ = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
  yearZ = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
}

void getDateTarget(){
  Serial.print(hourZ, DEC);
  Serial.print(":");
  Serial.print(minuteZ, DEC);
  Serial.print(":");
  Serial.print(secondZ, DEC);
  Serial.print("  ");
  Serial.print(monthZ, DEC);
  Serial.print("/");
  Serial.print(dayOfMonthZ, DEC);
  Serial.print("/");
  Serial.print(yearZ, DEC);
}

void setDateDs1307(){
  
  // Use of (byte) type casting and ascii math to achieve result.  
  second = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48)); 
  minute = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
  hour  = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
  dayOfWeek = (byte) (Serial.read() - 48);
  dayOfMonth = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
  month = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));
  year = (byte) ((Serial.read() - 48) *10 + (Serial.read() - 48));

  Wire.beginTransmission(clockAddress);
  Wire.write(byte(0x00));
  Wire.write(decToBcd(second));  // 0 to bit 7 starts the clock
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));    // If you want 12 hour am/pm you need to set

  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

// Gets the date and time from the ds1307 and prints result
void getDateDs1307() {
  
  // Reset the register pointer
  Wire.beginTransmission(clockAddress);
  Wire.write(byte(0x00));
  Wire.endTransmission();

  Wire.requestFrom(clockAddress, 7);

  // A few of these need masks because certain bits are control bits
  second     = bcdToDec(Wire.read() & 0x7f);
  minute     = bcdToDec(Wire.read());
  
  // Need to change this if 12 hour am/pm
  hour       = bcdToDec(Wire.read() & 0x3f);  
  dayOfWeek  = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month      = bcdToDec(Wire.read());
  year       = bcdToDec(Wire.read());

  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
  Serial.print("  ");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(year, DEC);

}

//-------------------------------------------------------------------

void setup() {
  
  Wire.begin();
  Serial.begin(57600);
  
  // set the speed at 60 rpm:
  myStepper.setSpeed(max(spd, 60));
     
  // Enable power to the motor
  pinMode(enA, OUTPUT);
  // digitalWrite (enA, HIGH); commented to only enable when moving
    
  pinMode(enB, OUTPUT);
  // digitalWrite (enB, HIGH); commented to only enable when moving
}

// Perform specified cycle and revolutions
void doCycle(){
  digitalWrite (enA, HIGH);
  digitalWrite (enB, HIGH);
  for (int i=0; i<cyc; i++){
    myStepper.step(stepsPerRevolution*rev);
    delay(100);
    myStepper.step(-stepsPerRevolution*rev);
    delay(delayCycle);
  }
  digitalWrite (enA, LOW);
  digitalWrite (enB, LOW);
  delay(5000);
}

void refreshClock(){
  // Reset the register pointer
  Wire.beginTransmission(clockAddress);
  Wire.write(byte(0x00));
  Wire.endTransmission();

  Wire.requestFrom(clockAddress, 7);

  second     = bcdToDec(Wire.read() & 0x7f);
  minute     = bcdToDec(Wire.read());
  
  hour       = bcdToDec(Wire.read() & 0x3f);  
  dayOfWeek  = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month      = bcdToDec(Wire.read());
  year       = bcdToDec(Wire.read());
}
void loop() {
  
  //refresh the internal clock 
  refreshClock();
  
  //########################################################################
  // the if statement is the time condition to perform the motor cycle
  // if seconds is not needed, add condition: second == 0
  // e.g. 18:20 => hourZ = 18; minuteZ = 20;
  //            => if (hour == hourZ && minute == minuteZ && second == 0)
  //########################################################################
  
  if (hour == hourZ && minute == minuteZ && second == 0){ 
    doCycle();
    
  }else if (Serial.available()) {  // Serial Monitor Commands
    command = Serial.read();
  
    if (command == 84) {      //If command = "T" Set Date
      setDateDs1307();
      Serial.println("New time: ");
      getDateDs1307();
      Serial.println(" ");
      
    }else if (command == 'G'){  //If command = "G" Get Date
      getDateDs1307();
      Serial.println(" ");
      
    }else if (command == 'M'){  //If command = "M" Test motor cycle
      doCycle();
    
    }else if (command == 'Z'){  
      setDateTarget();
    }
  }

  command = 0;  // reset command                  
}
