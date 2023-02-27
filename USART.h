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
        __USART__();
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
};

extern __USART__ USART;

#endif
