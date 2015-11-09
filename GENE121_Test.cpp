#include "UW_GENE_121.h"

const int BLACK = 70;

void followLine()
{
    do
    {
      setMotor(MOTOR_A,-50);
      setMotor(MOTOR_B,50);
      
      while (getSensor(REFLECT_2)>BLACK && getSensor(REFLECT_1)>BLACK)
      {
      }	
      if (getSensor(REFLECT_2<BLACK))
      {
              setMotor(MOTOR_A,0);
              setMotor(MOTOR_B,50);
              while (getSensor(REFLECT_2<BLACK) < BLACK)
              {}
      }
      else if (getSensor(REFLECT_1<BLACK))
      {
              setMotor(MOTOR_A,-50);
              setMotor(MOTOR_B,0);
              while (getSensor(REFLECT_1<BLACK))
              {}
      }
    }while ( (getSensor(BUMPER) == 0) //Breaks when bumper is touched
            //Breaks when both sensors detect black
            && (getSensor(REFLECT_2)>BLACK || getSensor(REFLECT_1)>BLACK));
    
    setMotor(MOTOR_A,0);
    setMotor(MOTOR_B,0);
}

void runCourse1()
{
    followLine();
}

void runCourse2()
{
    
    followLine();
      
    //When crossing intersection, continue straight
    setMotor(MOTOR_A,-50);
    setMotor(MOTOR_B,50);
    wait1Msec(1000);
    
    followLine();
    
    
    //When intersection detected, rotate until car is stradeling line
    setMotor(MOTOR_A,-50);
    setMotor(MOTOR_B,0);
    while (getSensor(REFLECT_1) < BLACK && getSensor(REFLECT_2) < BLACK)
    {}
    
    
    followLine();
}

void runCourse3()
{   
  //Runs until first intersection, then continues straight
  followLine();
}

void main(void)
{   
    //Init Code - DO NOT TOUCH!
    initialize();
    wait1Msec(200);
    
    
    while (getSensor(BUTTON) == 0)
    {}
    while (getSensor(BUTTON) != 0)
    {}
    
    setMotor(MOTOR_A,0);
    setMotor(MOTOR_B,0);
    
    resetTimer();
    LEDoff(GREEN_LED);
    LEDon(RED_LED);
    
    int pushes = 1;
    LEDnum(pushes-1);
      
    while (time1()<5000) // Initialise time to chose course
    {
      //Counts button pushes and displays appropriate light on bar
      if (getSensor(BUTTON) != 0)
      {
        wait1Msec(200);
        pushes++;
      }
      
      if (pushes > 3)
        pushes -= 3;
      
      LEDnum(pushes-1);
    }
    
    int courseNUM = pushes;
    LEDoff(RED_LED);
    LEDon(GREEN_LED);
    
    
    //Runs appropriate course
    if (courseNUM == 1)
      runCourse1();
    else if (courseNUM == 2)
      runCourse2();
    else
      runCourse3();
    
    setMotor(MOTOR_A,0);
    setMotor(MOTOR_B,0);
}
