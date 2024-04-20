/* Dependencies */
#include "USART.h"

#if defined(HAS_USART)

__USART__ USART = __USART__(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0);

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART RX
Input:    Interrupt vector
Return:   None
************************/
#if defined(USART_TX_vect)
ISR(USART_RX_vect)
#elif defined(USART0_TX_vect)
ISR(USART0_RX_vect)
#endif
{
    USART.rxIRQ();
}

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART TX
Input:    Interrupt vector
Return:   None
************************/
#if defined(USART_UDRE_vect)
ISR(USART_UDRE_vect)
#elif defined(USART0_UDRE_vect)
ISR(USART0_UDRE_vect)
#endif
{
    USART.txIRQ();
}

#endif


