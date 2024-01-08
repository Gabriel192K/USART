/*
 * USART - Demo Read.cpp
 *
 * Created: 1/8/2024 8:50:11 AM
 * Author : rotes
 */ 

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

