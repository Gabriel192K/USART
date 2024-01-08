/* Dependencies */
#include "USART.h"

#if defined(HAS_USART1)

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART RX
Input:    Interrupt vector
Return:   None
************************/
ISR(USART1_RX_vect)
{
    USART1.rxIRQ();
}

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART TX
Input:    Interrupt vector
Return:   None
************************/
ISR(USART1_UDRE_vect)
{
    USART1.txIRQ();
}

__USART__ USART1 = __USART__(&UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1);

#endif