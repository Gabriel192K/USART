/* Dependencies */
#include "USART.h"

/*********************************************
Function: __USART__()
Purpose:  Constructor to __USART__ class
Input:    USART specific registers
Return:   None
*********************************************/
__USART__::__USART__(volatile uint8_t* ubrrh, volatile uint8_t* ubrrl, volatile uint8_t* ucsra, volatile uint8_t* ucsrb, volatile uint8_t* ucsrc, volatile uint8_t* udr)
{
    this->ubrrh = ubrrh;
    this->ubrrl = ubrrl;
    this->ucsra = ucsra;
    this->ucsrb = ucsrb;
    this->ucsrc = ucsrc;
    this->udr = udr;
}

/*********************************************
Function: ~__USART__()
Purpose:  Destructor to __USART__ class
Input:    None
Return:   None
*********************************************/
__USART__::~__USART__()
{
    this->ubrrh = NULL;
    this->ubrrl = NULL;
    this->ucsra = NULL;
    this->ucsrb = NULL;
    this->ucsrc = NULL;
    this->udr = NULL;
}

/*****************************************
Function: begin()
Purpose:  Start USART bus
Input:    Baudrate of USART bus
Return:   None
*****************************************/
void __USART__::begin(uint32_t baudrate)
{
    /* If <USART> implementation already initialized do not bother */
    if (this->beginFunctionCalled)
        return; 
    this->beginFunctionCalled = 1;

    /* Calculate <USART> prescale */
    uint16_t prescale = roundf((F_CPU / ((float)16 * baudrate)) - (float)1); 

    /* Begin time implementation */
    Time.begin();

    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        #if defined(__AVR_ATmega328P__) || \
            defined(__AVR_ATmega328PB__) || \
            defined(__AVR_ATmega2560__)
        /* Write <LSB> of the prescale */
        *(this->ubrrh) = (uint8_t)(prescale >> 8);
        /* Write <MSB> of the prescale */
        *(this->ubrrl) = (uint8_t)prescale;
        /* 8 bit data transmission size */
        *(this->ucsrc) = *(this->ucsrc) | (1 << UCSZ01) | (1 << UCSZ00);
        /* Enable <RX>, <RX-IRQ>, <TX> */
        *(this->ucsrb) = *(this->ucsrb) | (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0);
        #endif
    }
}

/***************************************************************
Function: available()
Purpose:  Get the number of bytes waiting in the RX buffer
Input:    None
Return:   Number of bytes waiting in the RX buffer
***************************************************************/
const uint8_t __USART__::available(void)
{
    uint8_t bytes = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        bytes = (USART_DEFAULT_RX_BUFFER_SIZE + this->rxHead - this->rxTail) % USART_DEFAULT_RX_BUFFER_SIZE;
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
        this->rxHead = this->rxTail;
    }
}

/*******************************
Function: read()
Purpose:  Read byte from buffer
Input:    None
Return:   Byte to be read
********************************/
const uint8_t __USART__::read(void)
{
    /* Check if RX buffer is empty */
    if (this->rxHead == this->rxTail)
        return ('\0');
	
	uint8_t byte = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        /* Read byte */
        byte = this->rxBuffer[this->rxTail];
        /* Increase tail */
        this->rxTail = (this->rxTail + 1) % USART_DEFAULT_RX_BUFFER_SIZE;
    }
    /* Return data from buffer */
    return (byte);
}

/*******************************
Function: read()
Purpose:  Read bytes from buffer
Input:    Array to store bytes, its size and the timeout value
Return:   How many bytes were read
********************************/
const int8_t __USART__::read(uint8_t* buffer, uint8_t size, const uint32_t timeout)
{
    /* If no size do not bother and return */
    if (!size)
        return (0);
    
    /* If no bytes available do not bother and return */
    if (!this->available())
        return (-1);

    /* Store how many bytes were read */
    uint8_t bytesRead = 0;

    /* Store a timestamp for timeout */
    const uint32_t timestamp = Time.milliseconds();

    /* While bytes fit into the array */
    while (size)
    {
        /* if timeout occured */
        if (((Time.milliseconds() - timestamp) >= timeout) && timeout)
            return (-2);

        /* If no bytes available do not bother and continue */
        if (!this->available())
            continue;

        /* Put byte into buffer */
        *buffer++ = (uint8_t)this->read();
        size--;
        bytesRead++;
    }

    return (bytesRead);
}

/*******************************
Function: read()
Purpose:  Read bytes from buffer
Input:    Array to store bytes and its size
Return:   How many bytes were read
********************************/
const int8_t __USART__::read(uint8_t* buffer, uint8_t size)
{
    return (this->read(buffer, size, 0));
}

/***************************************************
Function: write()
Purpose:  Write byte into buffer
Input:    Byte to write
Return:   None
***************************************************/
void __USART__::write(const uint8_t byte)
{
    /* Trucate increase of head of buffer */
    uint8_t bufferHead = (this->txHead + 1) % USART_DEFAULT_TX_BUFFER_SIZE;
    /* Check if next element will overflow and wait if true */
    while (bufferHead == this->txTail);

    /* Load data into buffer */
    this->txBuffer[this->txHead] = byte;
    /* Copy trucated head */
    this->txHead = bufferHead;

    #if defined(__AVR_ATmega328P__) || \
        defined(__AVR_ATmega328PB__) || \
        defined(__AVR_ATmega2560__)
    /* Enable TX interrupt */
    *(this->ucsrb) = *(this->ucsrb) | (1 << UDRIE0);
    #endif
}

/***************************************************
Function: write()
Purpose:  Write data into buffer
Input:    Data to write and size of the data
Return:   None
***************************************************/
void __USART__::write(const uint8_t* data, const uint8_t size)
{
    for (const uint8_t* p = data; p < (data + size); p++)
        this->write(*p);
}

/***************************************************
Function: write()
Purpose:  Write data into buffer
Input:    Data to write and size of the data
Return:   None
***************************************************/
void __USART__::write(const void* data, const uint8_t size)
{
    for (const uint8_t* p = (const uint8_t*)data; p < ((const uint8_t*)data + size); p++)
        this->write(*p);
}

/***************************************************
Function: println()
Purpose:  Print newline into buffer
Input:    None
Return:   None
***************************************************/
void __USART__::println(void)
{
    this->print('\n');
}

/***************************************************
Function: print()
Purpose:  Print character into buffer
Input:    Character to print
Return:   None
***************************************************/
void __USART__::print(const char c)
{
    this->write((const uint8_t)c);
}

/***************************************************
Function: println()
Purpose:  Print character into buffer then a newline
Input:    Character to print
Return:   None
***************************************************/
void __USART__::println(const char c)
{
    this->print(c);
    this->println();
}

/***************************************************
Function: print()
Purpose:  Print array into buffer
Input:    Array to print
Return:   None
***************************************************/
void __USART__::print(const char* s)
{
    while(*s)              /* While data is available */
        this->print(*s++); /* Write data into buffer */
}

/***************************************************
Function: println()
Purpose:  Print array into buffer then a newline
Input:    Array to print
Return:   None
***************************************************/
void __USART__::println(const char* s)
{
    this->print(s);
    this->println();
}

/***************************************************
Function: printP()
Purpose:  Print array into buffer (from FLASH)
Input:    Array to be written
Return:   None
***************************************************/
void __USART__::printP(const char* s)
{
    /* While data is available */
    while (pgm_read_byte(s))
        /* Write byte into buffer */
        this->write((uint8_t)pgm_read_byte(s++));
}

/***************************************************
Function: print()
Purpose:  Print 8-bit number into buffer
Input:    8-bit number
Return:   None
***************************************************/
void __USART__::print(uint8_t byte)
{
    /* Example 255 */
    if (byte > 99)
        this->print((char)(((byte / 100) % 10) + '0')); /* 2 */
    if (byte > 9)
        this->print((char)(((byte / 10) % 10) + '0'));  /* 5 */
    this->print((char)((byte % 10) + '0'));             /* 5 */
}

/***************************************************
Function: println()
Purpose:  Print 8-bit number into buffer with a new line
Input:    8-bit number
Return:   None
***************************************************/
void __USART__::println(uint8_t byte)
{
    this->print(byte);
    this->println();
}

/***************************************************
Function: print()
Purpose:  Print 16-bit number into buffer
Input:    16-bit number
Return:   None
***************************************************/
void __USART__::print(uint16_t word)
{
    /* Example 65535 */
    if (word > 9999)
        this->print((char)(((word / 10000) % 10) + '0')); /* 6 */
    if (word > 999)
        this->print((char)(((word / 1000) % 10) + '0'));  /* 5 */
    if (word > 99)
        this->print((char)(((word / 100) % 10) + '0'));   /* 5 */
    if (word > 9)
        this->print((char)(((word / 10) % 10) + '0'));    /* 3 */
    this->print((char)((word % 10) + '0'));               /* 5 */
}

/***************************************************
Function: println()
Purpose:  Print 16-bit number into buffer with new line
Input:    16-bit number
Return:   None
***************************************************/
void __USART__::println(uint16_t word)
{
    this->print(word);
    this->println();
}

/***************************************************
Function: print()
Purpose:  Print 32-bit number into buffer
Input:    32-bit number
Return:   None
***************************************************/
void __USART__::print(uint32_t dword)
{
    /* Example 4294967295 */
    if (dword > 999999999) 
        this->print((char)(((dword / 1000000000) % 10) + '0')); /* 4 */
    if (dword > 99999999) 
        this->print((char)(((dword / 100000000) % 10) + '0'));  /* 2 */
    if (dword > 9999999) 
        this->print((char)(((dword / 10000000) % 10) + '0'));   /* 9 */
    if (dword > 999999) 
        this->print((char)(((dword / 1000000) % 10) + '0'));    /* 4 */
    if (dword > 99999) 
        this->print((char)(((dword / 100000) % 10) + '0'));     /* 9 */
    if (dword > 9999) 
        this->print((char)(((dword / 10000) % 10) + '0'));      /* 6 */
    if (dword > 999) 
        this->print((char)(((dword / 1000) % 10) + '0'));       /* 7 */
    if (dword > 99) 
        this->print((char)(((dword / 100) % 10) + '0'));        /* 2 */
    if (dword > 9)
        this->print((char)(((dword / 10) % 10) + '0'));         /* 9 */
    this->print((char)((dword % 10) + '0'));                    /* 5 */
}

/***************************************************
Function: println()
Purpose:  Print 32-bit number into buffer with new line
Input:    32-bit number
Return:   None
***************************************************/
void __USART__::println(uint32_t dword)
{
    this->print(dword);
    this->println();
}

/***************************************************
Function: end()
Purpose:  End USART bus
Input:    None
Return:   None
***************************************************/
void __USART__::end(void)
{
    if (!this->beginFunctionCalled)
        return;
    /* Allow reinitialization of USART bus */
    this->beginFunctionCalled = 0;

    /* Disable <RX>, <RX-IRQ>, <TX> */
    #if defined(__AVR_ATmega328P__) || \
        defined(__AVR_ATmega328PB__) || \
        defined(__AVR_ATmega2560__)
    /* Enable TX interrupt */
    *(this->ucsrb) = *(this->ucsrb)  & ~((1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0));
    #endif
    
}

/***************************************************
Function: rxIRQ()
Purpose:  RX interrupt call
Input:    None
Return:   None
***************************************************/
void __USART__::rxIRQ(void)
{
    /* Read data into buffer */
    this->rxBuffer[this->rxHead] = *(this->udr);
    /* Increase head */
    this->rxHead = (this->rxHead + 1) % USART_DEFAULT_RX_BUFFER_SIZE;
}

/***************************************************
Function: txIRQ()
Purpose:  TX interrupt call
Input:    None
Return:   None
***************************************************/
void __USART__::txIRQ(void)
{
    /* If data is available into buffer */
    if (this->txHead != this->txTail)
    {
        /* Load data from buffer */
        *(this->udr) = this->txBuffer[this->txTail];
        /* Increase tail */
        this->txTail = (this->txTail + 1) % USART_DEFAULT_TX_BUFFER_SIZE;
    }
    /* Else there's no data into buffer */
    else
        /* Disable TX interrupt */
        *(this->ucsrb) = *(this->ucsrb) & ~(1 << UDRIE0); 
}
