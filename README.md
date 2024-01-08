# Terminal
- USART library

## Key features
- Compatible with `Microchip Studio IDE`
- ```begin()``` and ```end()``` functions to easily enable or disable the `USART` bus
- Circular buffers for both `RX` and `TX`
- Interrupt driven for both `RX` and `TX`

## Example code - Demo Read
```C
/* Dependencies */
#include "USART\USART.h"

/* Macros */
#define READ_ARRAY_SIZE          (uint8_t)4
#define READ_ARRAY_ELEMENT_SIZE  (uint8_t)1
#define READ_ARRAY_TIMEOUT_VALUE (uint32_t)2

/* Variables */
uint8_t arrayToStoreBytes[READ_ARRAY_SIZE];

int main(void)
{
    USART.begin(115200);
    USART.print("USART - Demo Read\n");
    while (1) 
    {
        int8_t howManyBytes = USART.read(arrayToStoreBytes, (READ_ARRAY_SIZE / READ_ARRAY_ELEMENT_SIZE), READ_ARRAY_TIMEOUT_VALUE);
        switch (howManyBytes)
        {
            case 0:
                USART.print("Size of array is '0'\n");
                break;
            case -1:
                USART.print("No data available\n");
                break;
            case -2:
                USART.print("Timeout occured\n");
                break;
            default:
                USART.print("Amount of bytes received: ");
                USART.println((uint8_t)howManyBytes);
                USART.print("Bytes received: ");
                USART.write(arrayToStoreBytes, (READ_ARRAY_SIZE / READ_ARRAY_ELEMENT_SIZE));
                USART.println();
                break;
        }

        Time.delay(1);
    }
}
```

## Example code - Demo Print
```C
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
```

## Tested on
- `Microchip Studio IDE` and `PuTTY` with `ATmega328PB`