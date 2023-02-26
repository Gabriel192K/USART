#include "USART.h"

/* Macros */
#define PRESCALE(baudrate) (ROUND((F_CPU / (16.0 * baudrate)) - 1.0)) /* Calculate baudrate */
#define USART_RX_BUFFER_MASK (USART_RX_BUFFER_SIZE - 1)
#define USART_TX_BUFFER_MASK (USART_TX_BUFFER_SIZE - 1)

#if (USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK)
    #error "USART RX BUFFER SIZE IS NOT A POWER OF 2!"
#elif (USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK)
    #error "USART TX BUFFER SIZE IS NOT A POWER OF 2!"
#endif

/* Variables */
static volatile uint8_t USART_RX_BUFFER[USART_RX_BUFFER_SIZE];
static volatile uint8_t USART_TX_BUFFER[USART_TX_BUFFER_SIZE];
static volatile uint8_t USART_RX_HEAD, USART_RX_TAIL;
static volatile uint8_t USART_TX_HEAD, USART_TX_TAIL;

/*********************************************
Function: __USART__()
Purpose:  Constructor to __USART__ class
Input:    None
Return:   None
*********************************************/
__USART__::__USART__()
{
    /* Empty */
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
    uint16_t prescale = (uint16_t)PRESCALE(baudrate); /* Calculate <USART> prescale */

    #if defined (__AVR_ATmega328P__)
        UBRRH = (uint8_t)(prescale >> 8);                   /* Write <LSB> of the prescale */
        UBRRL = (uint8_t)prescale;                          /* Write <MSB> of the prescale */
        UCSRC |= (1 << UCSZ1) | (1 << UCSZ0);               /* 8 bit data transmission size */
        UCSRB |= (1 << RXEN) | (1 << RXCIE) | (1 << TXEN);  /* Enable <RX>, <RX-IRQ>, <TX> */
    #endif
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
        bytes = (USART_RX_BUFFER_SIZE + USART_RX_HEAD - USART_RX_TAIL) & USART_RX_BUFFER_MASK;
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
        USART_RX_HEAD = USART_RX_TAIL;
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
    if (USART_RX_HEAD == USART_RX_TAIL)                         /* If RX buffer is empty */
        return ('\0');                                          /* Return null */
	USART_RX_TAIL = (USART_RX_TAIL + 1) & USART_RX_BUFFER_MASK; /* Increase tail */
	return (USART_RX_BUFFER[USART_RX_TAIL]);                    /* Return data from buffer */
}

/*******************************
Function: readUntil()
Purpose:  Read array of bytes from ring buffer
Input:    Arrat and terminator of array
Return:   Flag if array is received
********************************/
uint8_t __USART__::readUntil(uint8_t* array, const uint8_t terminator)
{
    if (__USART__::available())                     /* Detect available data into the <USART> buffer */
    {
        while (1)                                   /* Poll bytes received forever until terminator */
        {
            if (__USART__::available())             /* Only if <USART> buffer has received data */
            {
                uint8_t byte = __USART__::read();   /* Read current byte*/
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
    uint8_t bufferHead = (USART_TX_HEAD + 1) & USART_TX_BUFFER_MASK; /* Trucate increase of head of buffer */
    while (bufferHead == USART_TX_TAIL);                             /* Check if next element will overflow and wait if true */
    USART_TX_HEAD = bufferHead;                                      /* Copy trucated head */
    USART_TX_BUFFER[USART_TX_HEAD] = byte;                           /* Load data into buffer */
    UCSRB |= (1 << UDRIE);                                           /* Enable TX interrupt */
}

/***************************************************
Function: write()
Purpose:  Write array into buffer
Input:    Array to be written
Return:   None
***************************************************/
void __USART__::write(const uint8_t* array)
{
    while(*array)                   /* While data is available */
        __USART__::write(*array++); /* Write data into buffer */
}

void __USART__::printf(const char* array, ...)
{
    char buffer[USART_TX_BUFFER_SIZE] = {0};              /* Create a temporary buffer */
    va_list args;                                         /* Create a list */
    va_start(args, array);                                /* Start variadic implementations */
    vsnprintf(buffer, USART_TX_BUFFER_SIZE, array, args); /* Concatenate array into buffer */
    va_end(args);                                         /* End variadic implementations */
    __USART__::print(buffer);                             /* Write array into buffer */
}

void __USART__::printP(const char* array)
{
    while (pgm_read_byte(array))                  /* While data is available */
        __USART__::write(pgm_read_byte(array++)); /* Write data into buffer */
}

void __USART__::end(void)
{
    #if defined (__AVR_ATmega328P__)
        UCSRB &= ~((1 << RXEN) | (1 << RXCIE) | (1 << TXEN));  /* Disable <RX>, <RX-IRQ>, <TX> */
    #endif
}

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART RX
Input:    Interrupt vector
Return:   None
************************/
ISR(USART_RX_vect)
{
    USART_RX_HEAD = (USART_RX_HEAD + 1) & USART_RX_BUFFER_MASK; /* Increase head */
    USART_RX_BUFFER[USART_RX_HEAD] = UDR;                       /* Read data into buffer */
}

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of USART TX
Input:    Interrupt vector
Return:   None
************************/
ISR(USART_UDRE_vect)
{
    if (USART_TX_HEAD != USART_TX_TAIL)                             /* If data is available into buffer */
    {
        USART_TX_TAIL = (USART_TX_TAIL + 1) & USART_TX_BUFFER_MASK; /* Increase tail */
        UDR = USART_TX_BUFFER[USART_TX_TAIL];                       /* Load data from buffer */
    }
    else                                                            /* Else there's no data into buffer */
        UCSRB &= ~(1 << UDRIE);                                     /* Disable TX interrupt */
}

__USART__ USART = __USART__();
