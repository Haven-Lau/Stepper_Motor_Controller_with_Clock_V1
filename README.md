# Stepper Motor Controller with Clock

This project uses a DS1307 RTC module, a module that is great for keeping track of the time and date without an external power supply.
The module is accessed via I2C with an Arduino.
This code allows a set of stepper motor instructions to be triggered exactly at the time and date preset in the program.
```
For example: Spins the motor 6 times clockwise to open the blinds at 7 AM, and closes the blind at 9 PM.
```
It includes a set of serial debugging instruction set to get date, set date and test the preset motor cycle..

The Wire and Stepper libraries are accessed.
