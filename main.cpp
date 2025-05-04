//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

AnalogIn potentiometer(A0);
AnalogIn lm35(A1); // 10 mV/\xB0 C
PwmOut buzzer(D5);
AnalogIn mq2(A2);


//=====[Declaration and initialization of public global variables]=============
 bool quit = false;
 
float lm35Reading = 0.0; // Raw ADC input A0 value
float lm35TempC = 0.0;   // Temperature in Celsius degrees [\xB0 C]
float lm35TempF = 0.0;   // Temperature in Fahrenheit degrees [\xB0 F]

float gasread = 0.0;
float tempread = 0.0;

float potentiometerReading = 0.0;   // Raw ADC input A1 value
float potentiometerScaledToC = 0.0; // Potentiometer value scaled to Celsius degrees [\xB0 C]
float potentiometerScaledToF = 0.0; // Potentiometer value scaled to Fahrenheit degrees [\xB0 F]


//=====[Declarations (prototypes) of public functions]=========================
 void uartTask();
void pcSerialComStringWrite( const char* str );
char pcSerialComCharRead();
void Alarm();

 float analogReadingScaledWithTheLM35Formula( float analogReading );;
 float potentiometerScaledToCelsius( float analogValue );
//=====[Main function, the program entry point after power on or reset]========

int main()
{
    while( true ) { 
char str[100] = "";
                lm35Reading = lm35.read();                
                lm35TempC = analogReadingScaledWithTheLM35Formula(lm35Reading);
                str[0] = '\0';
            sprintf ( str, "LM35: %.2f \xB0 C\r\n", lm35TempC);
                pcSerialComStringWrite( str );
                delay(1000);
                potentiometerReading = potentiometer.read();
               str[0] = '\0';
                sprintf ( str, "Potentiometer reading: %.2f\r\n", potentiometerReading);
                pcSerialComStringWrite( str );
                delay(1000);
                Alarm();
    }
}

//=====[Implementations of public functions]===================================
void Alarm() 
{
    gasread = mq2.read();
    lm35Reading = lm35.read();
    tempread = analogReadingScaledWithTheLM35Formula(lm35Reading);
   if(gasread >=0.2){
    pcSerialComStringWrite( "Gas Alarm\r\n");
   } else if(tempread >= 40.0) {
    pcSerialComStringWrite( "Temperature Alarm\r\n");
   }
    while(gasread >= 0.2 || tempread >= 40.0){
               gasread = mq2.read();
                lm35Reading = lm35.read();
    tempread = analogReadingScaledWithTheLM35Formula(lm35Reading);
                if (mq2 >= 0.2 || tempread >= 40.0) {
                       buzzer.period(1.0/55.0);
                       buzzer=5.0;
                      delay(1); 
                      buzzer.period(1.0/55.0);
                         buzzer=5.0;
                     delay(1);
                          buzzer=0.0;                     
                } else {
                    break;
              }
                delay(10);
  }
                    pcSerialComStringWrite( "Gas is not being detected\r\n");
 }
 
float analogReadingScaledWithTheLM35Formula( float analogReading )
{
    return analogReading * 330.0;
}

float celsiusToFahrenheit( float tempInCelsiusDegrees )
{
    return 9.0/5.0 * tempInCelsiusDegrees + 32.0;
}

float potentiometerScaledToCelsius( float analogValue )
{
    return 148.0 * analogValue + 2.0;
}

float potentiometerScaledToFahrenheit( float analogValue )
{
    return celsiusToFahrenheit( potentiometerScaledToCelsius(analogValue) );
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.write( str, strlen(str) );
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    return receivedChar;
}