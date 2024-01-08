/*
 * USART - Demo Print.cpp
 *
 * Created: 1/8/2024 9:27:20 AM
 * Author : rotes
 */ 

/* Dependencies */
#include "USART\USART.h"

int main(void)
{
    USART.begin(115200);
    USART.print("USART - Demo Print\n");

    /* Characters */
    USART.print('A');
    USART.println();
    USART.println('B');
    
    /* Char arrays */
    /* These arrays are from RAM */
    USART.print("This is a char array\n");
    USART.println("This is another char array");

    /* Char arrays from FLASH */
    /* PSTR is required and is a macro provided by pgmspace.h */
    USART.printP(PSTR("This is a char array from FLASH\n"));

    /* Numbers */
    /* Typecasting is required because compiler 'thinks' any number inserted as a constant is an 'int' data type */
    /* So that way we forcefully tell the compiler to call the correct function for the number */
    USART.print((uint8_t)255);
    USART.println();
    USART.println((uint8_t)254);
    USART.print((uint16_t)65535);
    USART.println();
    USART.println((uint16_t)65534);
    USART.print((uint32_t)4294967295);
    USART.println();
    USART.println((uint32_t)4294967294);
    while (1) 
    {
    }
}
