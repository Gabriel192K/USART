# USART
USART implementation for AVR platform

## Key features
- interrupt driven RX & TX
- buffered RX & TX (ring buffers)
- overflow protection if TX data flow overruns TX interrupt speed (it just waits for TX buffer to have more room)

##Tested on
- ATmega328P
