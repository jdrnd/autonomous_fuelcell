#include "UW_GENE_121.h"

void main(void)
{   
  //These two lines MUST be the first two lines of main, or else the motors
  //don't work
    initialize();
    wait1Msec(200);
    
    //wait for a button press
    while(getSensor(BUTTON) == 0)
    {
      LEDon(GREEN_LED);
    }
    
    //count up
    for (int i = 0; i < 8; i++)
    {
      LEDnum(i % 8);  // choose LED from 0 to 7
      wait1Sec(1);
    }
    
    //Below is sample software showing how to use various functions.
    //It doesn't actually do much, so don't try to run it as-is :-)
/*   
    //read the button and store the  restul
    int buttonSensor = getSensor(BUTTON);
    //turn on the green and red indicator LEDs
    LEDon(GREEN_LED);
    LEDon(RED_LED);
   
    //this is how you wait for a button press.
    //the button gives a 0 or a 1.  It is 1 when pressed.
    while(buttonSensor != 1)
    {
      buttonSensor = getSensor(BUTTON);
    }
    
    while(getSensor(BUTTON) == 1)
    {
    }
    
    while(getSensor(BUTTON)==0)
    {
      while(getSensor(BUMPER) == 0)
      {
        setMotor(MOTOR_A,-50);
        setMotor(MOTOR_B,50);
      }
      setMotor(MOTOR_A,50);
      setMotor(MOTOR_B,-50);
      wait1Msec(2000);
    }
    
    while(getSensor(BUTTON) == 0)
    {
      setMotor(MOTOR_A,20);
      setMotor(MOTOR_B,-20);
    }
    
      setMotor(MOTOR_A,0);
      setMotor(MOTOR_B,0);
    */
    /*
    LEDoff(GREEN_LED);
    //This is how to read the bumper. It gives 0 or 1, it is 1 when pressed.
    while(getSensor(BUMPER) == 0);
    while(getSensor(BUMPER) == 1); //due to sensor bounce, this may or may not work.
    LEDon(GREEN_LED);
    
    //This is how to set motors. Motor power ranges from -100 to +100
    //setMotor(MOTOR_A,-50);  // use negative sign to reverse direction
    //setMotor(MOTOR_B,50);  // max speed is 100
   
    //this is a wait function
    wait1Msec(1000);
    LEDoff(RED_LED);	
    
    //This is how you read the sensors and use the LED bar to see which one is
    //higher or lower
    while(getSensor(BUTTON) == 0)
    {
      if(getSensor(REFLECT_1) > getSensor(REFLECT_2))
        LEDnum(4);
      else 
        LEDnum(0);
    }
    
    for (int i = 0; i < 8; i++)
    {
      LEDnum(i % 8);  // choose LED from 0 to 7
      wait1Sec(1);  */
  
        
    for (int i = 0; i < 3; i++)
    {
      setMotor(MOTOR_A,75);
      setMotor(MOTOR_B,75);
      while(getSensor(BUMPER)==0) {}
      while(getSensor(BUMPER)==1) {}
      setMotor(MOTOR_A,50);
      setMotor(MOTOR_B,50);
      wait1Msec(4000);
      setMotor(MOTOR_A,50);
      setMotor(MOTOR_B,-50);
      wait1Sec(2);
      setMotor(MOTOR_A,50);
      setMotor(MOTOR_B,50);
      wait1Msec(3);
    }
  }
