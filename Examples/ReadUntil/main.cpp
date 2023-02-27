/*
 * ReadUntil.cpp
 *
 * Created: 2/14/2023 9:09:08 PM
 * Author : rotes
 */ 

#include "USART/USART.h"
#include "Time/Time.h"
#include <string.h>

char buffer[32];

int main(void)
{
    USART.begin(9600);
    Time.begin();
    USART.printP(PSTR("Read Until USART Demo\n"));
    while (1) 
    {
        static time_t timestamp;
        if (Time.millis() - timestamp >= 1000)
        {
            USART.printP(PSTR("Ping...\n"));
            timestamp = Time.millis();
        }

        uint8_t status = USART.readUntil(buffer, '$'); /* Read an array of bytes until specified character */
        if (status)
        {
            USART.printP(PSTR("Got data: "));
            USART.println(buffer);
            memset(buffer, 0, (sizeof(buffer) / sizeof(buffer[0])));
        }
    }
}

