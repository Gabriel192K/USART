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
	Time.begin();
	/*
    USART.begin(115200);
	USART1.begin(9600);
	USART.print("RS485 Master");
    while (1)
    {
		USART1.write((uint8_t)10);
		Time.delay(1);
    }
	*/
		USART.begin(115200);
		USART1.begin(9600);
		USART.print("RS485 Slave");
		while (1)
		{
			if (USART1.available())
			{
				const uint8_t byte = USART1.read();
				USART.print(byte);
			}
		}
}
