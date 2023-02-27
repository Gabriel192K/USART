#ifndef __USART_H__
#define __USART_H__

/* Dependecies */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include "Core\Core.h"
#include "Utilities\pgmspace.h"
#include "Utilities\Interrupts.h"
#include "Utilities\Math.h"

/* Macros */
#define USART_RX_BUFFER_SIZE 256
#define USART_TX_BUFFER_SIZE 256

class __USART__
{
    public:
        __USART__(volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,\
                  volatile uint8_t *ucsra, volatile uint8_t *ucsrb,\
                  volatile uint8_t *ucsrc, volatile uint8_t *udr);
        ~__USART__();
        void    begin    (uint32_t baudrate);
        uint8_t available(void);
        void    flush    (void);
        uint8_t read     (void);
        uint8_t readUntil(char* array, const uint8_t terminator);
        void    write    (const uint8_t byte);
        void    write    (const uint8_t* array);
        void    print    (const char byte) {__USART__::write((uint8_t)byte);}
        void    print    (const char* array) {__USART__::write((const uint8_t*)array);}
        void    printf   (const char* array, ...);
        void    printP   (const char* array);
        void    println  (void) {__USART__::write('\n');}
        void    println  (const char* array) {__USART__::write((const uint8_t*)array); __USART__::println();}
        void    end      (void);
        inline void rxIRQ(void);
        inline void txIRQ(void);
    private:
        volatile uint8_t* USART_RX_BUFFER;
        uint16_t USART_RX_BUFFER_MASK;
        volatile uint8_t USART_RX_HEAD, USART_RX_TAIL;
        volatile uint8_t* USART_TX_BUFFER;
        uint16_t USART_TX_BUFFER_MASK;
        volatile uint8_t USART_TX_HEAD, USART_TX_TAIL;
        uint8_t hasBegin;

        volatile uint8_t* ubrrh;
        volatile uint8_t* ubrrl;
        volatile uint8_t* ucsra;
        volatile uint8_t* ucsrb;
        volatile uint8_t* ucsrc;
        volatile uint8_t* udr;
};

#if defined(__AVR_ATmega328P__)
#define HAVE_USART
extern __USART__ USART;
#endif

#endif
