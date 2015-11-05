/**********************************************************************
  Filename: UW_GENE_121.cpp
  Date: October 1, 2014
  File Version: 4.1

  A set of functions that will allow students to run the fuel cell
  car without knowing the microprocessor and interfacing details.

  Students need to call initialize() as their first statement.  It turns
  the watchdog timer off.  If they forget this statement, then the
  processor will reset fairly quickly and the program will appear not
  to run at all.

  LEDs on the light bar are driven by a decoder, so only one can be on
  at any given time.

  History
  Ver  Date        Comment
  4.2  Sep  18/15  Disaled pullups for ADCs 
  4.1  Jul  28/15  Modified pin settings to ensure button and switch resistors
                   are configured as pull-up upon startup.  Also enabled pullups
                   for _ADC_0 and _ADC_1.
  4.0  Oct  1/14   Split from UW_Eng.cpp; Fixed incorrect header;
                   Restored functionality of RED/GREEN LEDs, so will NOT
                   work with accelerometer; Added flag for motor controller
                   reset;  Added leading _ to non-student constants and
                   functions; Removed elapsed variable and references to
                   UART used for printf

UW_Eng.cpp
  3.0  Sep 1/14    Added menus and auto-calibrate with some code review
  2.4  Oct 25/13   Turn off ADC and ENC after each conversion; Make sure to
                   toggle ENC according to user guide
  2.3  Oct 16/13   Adjust motor direction to match wiring
  2.2  Sep 26/13   Add support for button on P1.3; code review for release;
  2.1  Jun 21/13   Removed motor controller reset pin; flipped forward and
                   reverse for motor to match wiring
  2.0  Jun  6/13   Bumper moved to P2.5 from P2.0
                   Added support for light bar of LEDs                   
  1.0  Apr 25/13   original release

**********************************************************************/
#ifndef _UW_GENE_121_CPP
#define _UW_GENE_121_CPP
#include <cmath>
#include "io430.h"
#include "UW_GENE_121.h"
//#define _RESET_MOT_CNTL
//the _millis function needs some global variables
unsigned long _timerOverflows; //stores the number of overflows so we can compute the elapsed time "accurately"
void _resetMotorController();
// initializes MSP 430 and the other electronics
void initialize()
{
    // stop watch dog timer
    WDTCTL = WDTPW + WDTHOLD;
  
    // set CPU clock speed
    BCSCTL1 = _BCS_CLK;
    DCOCTL = _DCO_CLK;

    // set launchpad board LED directions
    P1DIR_bit.P0 = _OUTPUT;
    P1DIR_bit.P6 = _OUTPUT;

    //set the button pin as input
    P1DIR_bit.P3 = _INPUT;
    // turn on internal pull-up resistor for launchpad push button
    P1REN_bit.P3 = _HIGH; //this turns on the resistor but it is not yet
    //confifured as pull up or pull down
    P1OUT_bit.P3 = _HIGH; //this ensures it is set as a pullup
    
    // configure for fuel cell car
    _initMotorController();
#ifdef _RESET_MOT_CNTL
    _resetMotorController();
#endif
    _initADC();

    // set switch pin direction and resistor
    P2DIR_bit._P_SWITCH = _INPUT;
    P2REN_bit._P_SWITCH = _HIGH; //enable the pull-up
    P2OUT_bit._P_SWITCH = _HIGH; 
    
    // set LED light bar pin directions to output
    P2DIR |= _LED_MASK;

    //enable the elapsed time timer
    resetTimer();
    __enable_interrupt();  // enable global interrupts
}

// timer isr.  All we do here is increment overflows.
#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void)
{
//  increment the overflows
  _timerOverflows++;
}

void resetTimer()
{
     /*
    NOTE: since the UART also uses timer A, the elapsed time function will produce incorrect results
    if UART is being used with the code.  It is strongly suggested that you do not use UART if you intend to use the
    elapsed time functions.
    */

    // disable the interrupt
    TACCTL0 &= ~CCIE;

    // clear TAR; clock divide and up/down cleared as side effect so need to be set (again)
    TA0CTL_bit.TACLR = 1;
    TA0CTL_bit.TACLR = 0;

    // a period of 62,500 cycles at 1MHz with 1/8 clock division gives 500ms per cycle
    TACCR0 = _TIMER_COUNTMAX - 1;

    //select SMCLK = DCO, divide by 8 (ID_3), count in up mode (MC_1), start counting at 0
    TACTL = TASSEL_2 | ID_3 | MC_1 | TACLR;

    // clear overflow counter used to give longer timing
    _timerOverflows = 0;

    // enable the interrupt
    TACCTL0 |= CCIE; //enable interrupts
}

//returns elapsed time in milliseconds
unsigned long _millis()
{
    double time = double(TAR); //read the current count of the timer
    time = time / _TIMER_COUNTMAX * _INT_PERIOD;   //change it into milliseconds
    return _timerOverflows * _INT_PERIOD + (unsigned long)(time);
}

// elapsed time in ms, cast as int since other sizes of integers may not be covered in detail
int time1()
{
    return int(_millis());
}

// elasped time in tens of ms
int time10()
{
    return int((_millis()+5)/10);
}

// elapsed time in hundreds of ms
int time100()
{
    return int((_millis()+50)/100);
}

void wait10Usec(int t)
{
    for ( ; t>0; t--)
        __delay_cycles(_US_10_TICK);
}

void wait1Msec(int t)
{
    for ( ; t>0; t--)
        __delay_cycles(_MS_TICK);
}

void wait10Msec(int t)
{
    for ( ; t>0; t--)
        for (int i=0; i<10; i++)
            __delay_cycles(_MS_TICK);
}

void wait1Sec(int t)
{
    for ( ; t>0; t--)
        wait1Msec(1000);
}

void setMotor(int motorNum, int dirSpeed)
{
    // set direction
    unsigned char direction = _FWD;
    if (dirSpeed < 0)
    {
        direction = _REV;
        dirSpeed *= -1;
    }
    
    // set speed (max for user is 100)
    if (dirSpeed > 100)
        dirSpeed = 100;
    unsigned char speed = dirSpeed * _MOTORMAX / 100;

    // send commands to motor controller
    unsigned char motor = _MOTORB;

    switch (motorNum)
    {
      case MOTOR_A:
          motor = _MOTORA;
      case MOTOR_B:  // also for MOTOR_A
          _sendMotorChar(_START_BYTE);
          _sendMotorChar(_CNTL_DEVICE);
          _sendMotorChar(motor | direction);
          _sendMotorChar(speed & 0x7F);  // ensure MSB is zero
          break;
    }
}

int getSensor(int sensorNum)
{
    int sensorValue = 0;

    if (sensorNum == BUMPER)
        sensorValue = P2IN_bit._P_SWITCH;

    else if (sensorNum == BUTTON)
        sensorValue = !P1IN_bit.P3;   // active low, so invert

    else if (sensorNum == REFLECT_1 || sensorNum == REFLECT_2)
    {
        int pinADC = _ADC_0; //reflect sensor 1
        if (sensorNum == REFLECT_2)
            pinADC = _ADC_1; //reflect sensor 2

        // select pin, binary output, no clock divide, SMCLK
        ADC10CTL1 = (pinADC << 12) | ADC10SSEL1 | ADC10SSEL0;
  
        // ADC is on, enabled, and conversion is started
        ADC10CTL0 = ADC10ON | ENC | ADC10SC;
  
        // poll - wait for conversion complete
        while (ADC10CTL1 & ADC10BUSY);

        // return 10-bit value
        sensorValue = _ADC10MAX - ADC10MEM;
        
        //disable conversion
        ADC10CTL0 &= ~ENC;
    }

    return sensorValue;
}

//controls the red/greens.
void LEDoff(int led)
{
    if (led == RED_LED)
        P1OUT_bit.P0 = _LOW;
    else if (led == GREEN_LED)
        P1OUT_bit.P6 = _LOW;
}

//controls the red/green LEDs.
void LEDon(int led)
{
    if (led == RED_LED)
        P1OUT_bit.P0 = _HIGH;
    else if (led == GREEN_LED)
        P1OUT_bit.P6 = _HIGH;
}

//controls the LED display
void LEDnum(int led)
{
    if (led >= _LED_MIN && led <= _LED_MAX)
    {
        unsigned char led_set = (unsigned char)led & _LED_MASK;
        P2OUT &= ~_LED_MASK;
        P2OUT |= led_set;
    }
}

void _initADC()
{
  //enable pull-up: _ADC_0
  //  P1REN_bit.P4 = _HIGH; //this turns on the resistor but it is not yet
    //confifured as pull up or pull down
  //  P1OUT_bit.P4 = _HIGH; //this ensures it is set as a pullup
    //enable pull-up: _ADC_1
  //  P1REN_bit.P5 = _HIGH;
  //  P1OUT_bit.P5 = _HIGH; //this ensures it is set as a pullip

    // ADC enable channels A4, A5
    ADC10AE0 = (1 << _ADC_0) | (1 << _ADC_1);
  
    // leave ADC on, single conversion
    ADC10CTL0 = ADC10ON;
}

void _initMotorController()
{
    // set motor controller bits to be output
    P2DIR_bit._P_MOTOR = _OUTPUT;

    // set serial communication line high
    P2OUT_bit._P_MOTOR = _HIGH;
    wait1Msec(1);
}

#ifdef _RESET_MOT_CNTL
// this function is needed to reset the motor controller to factory spec in case anything goes wrong.
// this is possible when the controller gets shorted out, but it should never be seen by the students.
void _resetMotorController()
{
   _sendMotorChar(_START_BYTE);
   _sendMotorChar(_SETUP_DEVICE);
   _sendMotorChar(_DEFAULT_SETUP);
}
#endif

void _sendMotorChar(unsigned char b)
{
    // start bit
    P2OUT_bit._P_MOTOR = _LOW;
    wait10Usec(_BIT_PERIOD);

    // data bits
    for (int i=0; i<_EIGHT_BITS; i++)
    {
        P2OUT_bit._P_MOTOR = b & 0x01;
        b >>= 1;  // shift to next bit
        wait10Usec(_BIT_PERIOD);
    }
  
    // stop bit(s)
    for (int i=0; i<_TWO_STOP_BIT; i++)
    {
        P2OUT_bit._P_MOTOR = _HIGH;
        wait10Usec(_BIT_PERIOD);
    }
}

#endif



