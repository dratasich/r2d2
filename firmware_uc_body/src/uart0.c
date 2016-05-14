/**
 * @file uart0.h
 * @author Denise Ratasich
 * @date 09.11.2012
 *
 * @brief Header for UART 0 driver.
 */

#include <avr/interrupt.h>
#include "uart0.h"

// baudrate register values from datasheet:
#define UBRRH_9600	0
#define UBRRL_9600	207
#define UBRRH_19200	0
#define UBRRL_19200	103	
#define UBRRH_57600	0
#define UBRRL_57600	34	
#define UBRRH_115200	0
#define UBRRL_115200	16

static volatile uint8_t uart0_receive_flag = 0;
static volatile char uart0_receive_data;

void uart0_init()
{
  cli();

  // pin operations of Tx and Rx are overriden when UART enabled

  // set baud rate
  UBRR0H = UBRRH_115200;
  UBRR0L = UBRRL_115200;

  // double speed, buffer ready to be written
  UCSR0A |= (1<<U2X0) | (1<<UDRE0);

  // already default value of UART register C:
  //UCSR0C &= ~(1<<UMSEL00) & ~(1<<UMSEL01);	// asynchronous mode
  //UCSR0C &= ~(1<<UPM00) & ~(1<<UPM01);	// no parity
  //UCSR0C &= ~(1<<USBS0);			// 1 stopbit
  //UCSR0B &= ~(1<<UCSZ02);
  //UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);	// 8-bit

  // enable transmit/receive, receive-interrupt
  UCSR0B |= (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0);

  sei();
}

void uart0_putc(char myData)
{
  // wait for empty transmit buffer
  while ( !( UCSR0A & (1<<UDRE0)) );
  UDR0 = myData;
}

void uart0_print(char* string)
{
  while(*string != '\0') {
    uart0_putc(*string);
    string++;
  }
}

void uart0_println(char* string)
{
  while(*string != '\0') {
    uart0_putc(*string);
    string++;
  }

  uart0_putc('\n');
  uart0_putc('\r');
}

void uart0_printUInt8(uint8_t value)
{
  uart0_putc(value/100 + '0');
  value %= 100;
  uart0_putc(value/10 + '0');
  uart0_putc(value%10 + '0');
}

void uart0_printUInt8B(uint8_t value)
{
  int8_t i;
  for (i = 7; i >= 0; i--)
    uart0_putc(((value>>i) & 0x01) + '0');
}

void uart0_printUInt16(uint16_t value)
{
  uart0_putc(value/10000 + '0');
  value %= 10000;
  uart0_putc(value/1000 + '0');
  value %= 1000;
  uart0_putc(value/100 + '0');
  value %= 100;
  uart0_putc(value/10 + '0');
  uart0_putc(value%10 + '0');
}

// receive complete
ISR(USART0_RX_vect) {
  uart0_receive_data = UDR0;
  uart0_receive_flag = 1;
}

uint8_t uart0_getc(char* data)
{
  if(uart0_receive_flag) {
    *data = uart0_receive_data;
    uart0_receive_flag = 0;
    return 1;
  }

  return 0;
}
