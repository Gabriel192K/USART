#include "USART.h"

/* Macros */
#define PRESCALE(baudrate) (ROUND((F_CPU / (16.0 * baudrate)) - 1.0)) /* Calculate baudrate */

/*********************************************
Function: __USART__()
Purpose:  Constructor to __USART__ class
Input:    USART registers
Return:   None
*********************************************/
__USART__::__USART__(volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,\
                     volatile uint8_t *ucsra, volatile uint8_t *ucsrb,\
                     volatile uint8_t *ucsrc, volatile uint8_t *udr)
{
    this->ubrrh = ubrrh; this->ubrrl = ubrrl;
    this->ucsra = ucsra; this->ucsrb = ucsrb;
    this->ucsrc = ucsrc; this->udr = udr;
}

/*********************************************
Function: ~__USART__()
Purpose:  Destructor to ~__USART__ class
Input:    None
Return:   None
*********************************************/
__USART__::~__USART__()
{
    /* Empty */
}

/*****************************************
Function: begin()
Purpose:  Initialize USART bus
Input:    Baudrate
Return:   None
*****************************************/
void __USART__::begin(uint32_t baudrate)
{
    if (this->hasBegin) return; /* If USART implementation already initialized */
    this->hasBegin = 1;

    uint16_t prescale = (uint16_t)PRESCALE(baudrate); /* Calculate <USART> prescale */

    this->USART_RX_BUFFER = (uint8_t*)calloc(USART_RX_BUFFER_SIZE, sizeof(uint8_t)); /* Allocate memory to RX buffer */
    this->USART_TX_BUFFER = (uint8_t*)calloc(USART_TX_BUFFER_SIZE, sizeof(uint8_t)); /* Allocate memory to TX buffer */
    this->USART_RX_BUFFER_MASK = USART_RX_BUFFER_SIZE - 1;                           /* Calculate RX mask  */
    this->USART_TX_BUFFER_MASK = USART_TX_BUFFER_SIZE - 1;                           /* Calculate TX mask  */

    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        #if defined(__AVR_ATmega328P__)
        *this->ubrrh = (uint8_t)(prescale >> 8);                   /* Write <LSB> of the prescale */
        *this->ubrrl = (uint8_t)prescale;                          /* Write <MSB> of the prescale */
        *this->ucsrc |= (1 << UCSZ1) | (1 << UCSZ0);               /* 8 bit data transmission size */
        *this->ucsrb |= (1 << RXEN) | (1 << RXCIE) | (1 << TXEN);  /* Enable <RX>, <RX-IRQ>, <TX> */
        #endif
    }
}

/***************************************************************
Function: availabe()
Purpose:  Get the number of bytes waiting in the receiver buffer
Input:    None
Return:   Number of bytes waiting in the receiver buffer
***************************************************************/
uint8_t __USART__::available(void)
{
    uint8_t bytes;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        bytes = (USART_RX_BUFFER_SIZE + this->USART_RX_HEAD - this->USART_RX_TAIL) & this->USART_RX_BUFFER_MASK;
    }  
    return (bytes);
}

/***************************************************
Function: flush()
Purpose:  Flush bytes waiting in the receiver buffer
Input:    None
Return:   None
***************************************************/
void __USART__::flush(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        this->USART_RX_HEAD = this->USART_RX_TAIL;
    }
}

/*******************************
Function: read()
Purpose:  Read byte from ring buffer
Input:    None
Return:   Byte to be read
********************************/
uint8_t __USART__::read(void)
{
    if (this->USART_RX_HEAD == this->USART_RX_TAIL)                               /* If RX buffer is empty */
        return ('\0');                                                            /* Return null */
	this->USART_RX_TAIL = (this->USART_RX_TAIL + 1) & this->USART_RX_BUFFER_MASK; /* Increase tail */
	return (this->USART_RX_BUFFER[this->USART_RX_TAIL]);                          /* Return data from buffer */
}

/*******************************
Function: readUntil()
Purpose:  Read array of bytes from ring buffer
Input:    Array and terminator of array
Return:   Flag if array is received
********************************/
uint8_t __USART__::readUntil(char* array, const uint8_t terminator)
{
    if (this->available())                          /* Detect available data into the <USART> buffer */
    {
        while (1)                                   /* Poll bytes received forever until terminator */
        {
            if (this->available())                  /* Only if <USART> buffer has received data */
            {
                uint8_t byte = this->read();        /* Read current byte*/
                if (byte == terminator) return (1); /* If current byte is <TERMINATOR*/
                *array++ = byte;                    /* Insert byte into array */
            }
        }
    }
    return (0);
}

/***************************************************
Function: write()
Purpose:  Write byte into buffer
Input:    Byte to be written
Return:   None
***************************************************/
void __USART__::write(const uint8_t byte)
{
    uint8_t bufferHead = (this->USART_TX_HEAD + 1) & this->USART_TX_BUFFER_MASK; /* Trucate increase of head of buffer */
    while (bufferHead == this->USART_TX_TAIL);                                   /* Check if next element will overflow and wait if true */
    this->USART_TX_HEAD = bufferHead;                                            /* Copy trucated head */
    this->USART_TX_BUFFER[this->USART_TX_HEAD] = byte;                           /* Load data into buffer */

    #if defined(__AVR_ATmega328P__)
    *this->ucsrb |= (1 << UDRIE); /* Enable TX interrupt */
    #endif
}

/***************************************************
Function: write()
Purpose:  Write array into buffer
Input:    Array to be written
Return:   None
***************************************************/
void __USART__::write(const uint8_t* array)
{
    while(*array)              /* While data is available */
        this->write(*array++); /* Write data into buffer */
}

/***************************************************
Function: printf()
Purpose:  Write variadic array into buffer
Input:    Variadic array to be written
Return:   None
***************************************************/
void __USART__::printf(const char* array, ...)
{
    char buffer[USART_TX_BUFFER_SIZE] = {0};              /* Create a temporary buffer */
    va_list args;                                         /* Create a list */
    va_start(args, array);                                /* Start variadic implementations */
    vsnprintf(buffer, USART_TX_BUFFER_SIZE, array, args); /* Concatenate array into buffer */
    va_end(args);                                         /* End variadic implementations */
    this->print(buffer);                                  /* Write array into buffer */
}

/***************************************************
Function: printP()
Purpose:  Write array into buffer
Input:    Array to be written
Return:   None
***************************************************/
void __USART__::printP(const char* array)
{
    while (pgm_read_byte(array))             /* While data is available */
        this->write(pgm_read_byte(array++)); /* Write data into buffer */
}

/***************************************************
Function: end()
Purpose:  End USART bus
Input:    None
Return:   None
***************************************************/
void __USART__::end(void)
{
    free((uint8_t*)this->USART_RX_BUFFER); /* Free RX buffer */
    free((uint8_t*)this->USART_TX_BUFFER); /* Free TX buffer */
    this->hasBegin = 0;                    /* Allow reinitialization of USART bus */

    #if defined(__AVR_ATmega328P__)
    *this->ucsrb &= ~((1 << RXEN) | (1 << RXCIE) | (1 << TXEN)); /* Disable <RX>, <RX-IRQ>, <TX> */
    #endif
}

/***************************************************
Function: rxIRQ()
Purpose:  RX interrupt call
Input:    None
Return:   None
***************************************************/
inline void __USART__::rxIRQ(void)
{
    this->USART_RX_HEAD = (this->USART_RX_HEAD + 1) & this->USART_RX_BUFFER_MASK; /* Increase head */
    this->USART_RX_BUFFER[this->USART_RX_HEAD] = *this->udr;                      /* Read data into buffer */
}

/***************************************************
Function: txIRQ()
Purpose:  TX interrupt call
Input:    None
Return:   None
***************************************************/
inline void __USART__::txIRQ(void)
{
    if (this->USART_TX_HEAD != this->USART_TX_TAIL)                                   /* If data is available into buffer */
    {
        this->USART_TX_TAIL = (this->USART_TX_TAIL + 1) & this->USART_TX_BUFFER_MASK; /* Increase tail */
        *this->udr = this->USART_TX_BUFFER[this->USART_TX_TAIL];                      /* Load data from buffer */
    }
    else                                                                              /* Else there's no data into buffer */
        #if defined(__AVR_ATmega328P__)
        *this->ucsrb &= ~(1 << UDRIE);                                                /* Disable TX interrupt */
        #endif
}

#if defined(__AVR_ATmega328P__)
__USART__ USART = __USART__(&UBRRH, &UBRRL, &UCSRA, &UCSRB, &UCSRC, &UDR);
#endif

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART RX
Input:    Interrupt vector
Return:   None
************************/
#if defined(__AVR_ATmega328P__)
ISR(USART_RX_vect)
{
    USART.rxIRQ();
}
#endif

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART TX
Input:    Interrupt vector
Return:   None
************************/
#if defined(__AVR_ATmega328P__)
ISR(USART_UDRE_vect)
{
    USART.txIRQ();
}
#endif
