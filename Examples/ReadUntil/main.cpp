/*
 * ReadUntil.cpp
 *
 * Created: 2/14/2023 9:09:08 PM
 * Author : rotes
 */ 

#include "USART/USART.h"
#include "Time/Time.h"
#include <string.h>

uint8_t buffer[32];

int main(void)
{
    USART.begin(115200);
    Time.begin();
    USART.writePln(PSTR("Hello World"));
    while (1) 
    {
        uint8_t status = USART.readUntil(buffer, '$');
        if (status)
        {
            USART.writef("[%hhu:%hhu::%hhu] ", 12, 12, 12);
            USART.writeP(PSTR("Got data: "));
            USART.writeln(buffer);
            memset(buffer, 0, (sizeof(buffer) / sizeof(buffer[0])));
        }
    }
}

