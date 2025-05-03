#include "mbed.h"
#include "arm_book_lib.h"

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

AnalogIn potentiometer(A0);
AnalogIn lm35(A1); // 10 mV/\xB0 C
DigitalInOut sirenPin(D9);
DigitalIn mq2(A3);

bool quit = false;

char receivedChar = '\0';

float potentiometerReading = 0.0;   // Raw ADC input A1 value
float potentiometerScaledToC = 0.0; // Potentiometer value scaled to Celsius degrees [\xB0 C]
float potentiometerScaledToF = 0.0; // Potentiometer value scaled to Fahrenheit degrees [\xB0 F]

void availableCommands();
void uartTask();
void pcSerialComStringWrite( const char* str );
char pcSerialComCharRead();

float celsiusToFahrenheit( float tempInCelsiusDegrees );
float analogValueToFahrenheit( float analogValue );
float potentiometerScaledToCelsius( float analogValue );
float potentiometerScaledToFahrenheit( float analogValue );

int main()
{
    sirenPin.mode(OpenDrain);
    sirenPin.input();
    availableCommands();
    while( true ) {
        uartTask();
    }
}

void availableCommands()
{
    pcSerialComStringWrite( "Print the readings until 'q' is pressed:\r\n" );

    pcSerialComStringWrite("Press 'a' the reading at the analog pin A0 ");
    pcSerialComStringWrite("(connected to the potentiometer)\r\n");
}

void uartTask()
{
    char receivedChar = '\0';
    char str[100] = "";
    receivedChar = pcSerialComCharRead();
    if( receivedChar !=  '\0') {
        switch (receivedChar) {

        // Print potentiometer ADC input (A1) raw value until pulse 'q' or 'Q'
        case 'a':
        case 'A':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {
                potentiometerReading = potentiometer.read();
                str[0] = '\0';
                sprintf ( str, "Potentiometer reading: %.2f\r\n", potentiometerReading);
                pcSerialComStringWrite( str );
                delay( 200 );
                receivedChar = pcSerialComCharRead();
            }
            break;

        default:
            availableCommands();
            break;
        }
    }
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