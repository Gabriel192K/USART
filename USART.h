#ifndef __USART_H__
#define __USART_H__

/* Dependecies */
#include <stdio.h>
#include <stdint.h>
#include <math.h>          /* Round */
#include <avr/io.h>        /* Memory */
#include <avr/pgmspace.h>  /* Flash Program Space */
#include <avr/interrupt.h> /* Interrupts */
#include <util/atomic.h>   /* Atomic Operaions */
#include "..\Time\Time.h"

/* Macros */
#define USART_DEFAULT_RX_BUFFER_SIZE (const uint8_t)64
#define USART_DEFAULT_TX_BUFFER_SIZE (const uint8_t)64

class __USART__
{
    public:
        __USART__(volatile uint8_t* ubrrh, volatile uint8_t* ubrrl, volatile uint8_t* ucsra, volatile uint8_t* ucsrb, volatile uint8_t* ucsrc, volatile uint8_t* udr);
        ~__USART__();
        void          begin    (uint32_t baudrate);
        const uint8_t available(void);
        void          flush    (void);
        const uint8_t read     (void);
        const int8_t  read     (uint8_t* buffer, uint8_t size, const uint32_t timeout);
        const int8_t  read     (uint8_t* buffer, uint8_t size);
        void          write    (const uint8_t byte);
        void          write    (const uint8_t* s, const uint8_t size);
        void          write    (const void* s, const uint8_t size);
        void          println  (void);
        void          print    (const char c);
        void          println  (const char c);
        void          print    (const char* s);
        void          println  (const char* s);
        void          printP   (const char* s);
        void          print    (uint8_t byte);
        void          println  (uint8_t byte);
        void          print    (uint16_t word);
        void          println  (uint16_t word);
        void          print    (uint32_t dword);
        void          println  (uint32_t dword);
        void          end      (void);
        void          rxIRQ    (void);
        void          txIRQ    (void);
    private:
        uint8_t beginFunctionCalled;
        volatile uint8_t* ubrrh;
        volatile uint8_t* ubrrl;
        volatile uint8_t* ucsra;
        volatile uint8_t* ucsrb;
        volatile uint8_t* ucsrc;
        volatile uint8_t* udr;
        volatile uint8_t rxBuffer[USART_DEFAULT_RX_BUFFER_SIZE];
        volatile uint8_t txBuffer[USART_DEFAULT_TX_BUFFER_SIZE];
        volatile uint8_t rxHead, rxTail, txHead, txTail;
};

#if defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
    #define HAS_USART
    extern __USART__ USART;
#endif

#if defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
    #define HAS_USART1
    extern __USART__ USART1;
#endif

#if defined(__AVR_ATmega2560__)
    #define HAS_USART2
    extern __USART__ USART2;
#endif

#endif
