/* Dependencies */
#include "USART.h"

#if defined(HAS_USART2)

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART RX
Input:    Interrupt vector
Return:   None
************************/
ISR(USART2_RX_vect)
{
    USART2.rxIRQ();
}

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART TX
Input:    Interrupt vector
Return:   None
************************/
ISR(USART2_UDRE_vect)
{
    USART2.txIRQ();
}

__USART__ USART2 = __USART__(&UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UCSR2C, &UDR2);

#endif