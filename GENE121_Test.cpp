#include "UW_GENE_121.h"

const int BLACK = 80;

void followLine(int time100ms)
{
    resetTimer();
    while (getSensor(BUMPER) != 1 && time100ms > time100() )
    {
        if (getSensor(REFLECT_1) > BLACK && getSensor(REFLECT_2) > BLACK)
        {
            setMotor(MOTOR_A,-50);
            setMotor(MOTOR_B, 50);
            while (getSensor(REFLECT_1) > BLACK && getSensor(REFLECT_2) > BLACK)
            {}
        }
        
        else if (getSensor(REFLECT_1) < BLACK && getSensor(REFLECT_2) > BLACK)
        {
            setMotor(MOTOR_A,-50);
            setMotor(MOTOR_B, 20);
            while (getSensor(REFLECT_1) < BLACK && getSensor(REFLECT_2) > BLACK)
            {}
        }
        
        else if (getSensor(REFLECT_1) > BLACK && getSensor(REFLECT_2) < BLACK)
        {
            setMotor(MOTOR_A,-20);
            setMotor(MOTOR_B, 50);
            while (getSensor(REFLECT_1) > BLACK && getSensor(REFLECT_2) < BLACK)
            {}
        }
      
    }
    
    setMotor(MOTOR_A,0);
    setMotor(MOTOR_B,0);
}

void runCourse1()
{
    followLine(220);
    setMotor(MOTOR_A,-50);
    setMotor(MOTOR_B, 50);
    while (getSensor(BUMPER) != 1)
    {}
     
     
}

void runCourse2()
{
    
    followLine(260);
    setMotor(MOTOR_A,-50);
    setMotor(MOTOR_B, 50);
    while (getSensor(BUMPER) != 1)
    {}
    
}

void runCourse3()
{   
  //Runs until first intersection, then continues straight
    followLine(35);
    setMotor(MOTOR_A,-30);
    setMotor(MOTOR_B, 50);
    wait1Sec(5);
    setMotor(MOTOR_A,-50);
    setMotor(MOTOR_B, 50); 
    
    while (getSensor(BUMPER) != 1)
    {}
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
