/*
 * RX and TX.cpp
 *
 * Created: 2/14/2023 8:26:26 PM
 * Author : rotes
 */ 

#include "USART/USART.h"
#include "Time/Time.h"

int main(void)
{
    USART.begin(115200);
    Time.begin();
    USART.print("Hello World\n");
    while (1) 
    {
        static time_t timestamp;
        if (Time.millis() - timestamp >= 1000UL)
        {
            USART.print("Ping...\n");
            timestamp = Time.millis();
        }

        while (USART.available())
            USART.write(USART.read());
    }
}

