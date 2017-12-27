/**
 * @file uart0.h
 * @author Denise Ratasich
 * @date 09.11.2012
 *
 * @brief UART 0 driver.
 */

#ifndef __UART0_H__
#define __UART0_H__

#include <avr/io.h>	// e.g. uint8_t, uint16_t

void uart0_init();
void uart0_putc(char);
void uart0_print(char*);
void uart0_println(char*);
void uart0_printUInt8(uint8_t);
void uart0_printUInt16(uint16_t);
void uart0_printInt16(int16_t);
void uart0_printUInt8B(uint8_t);
uint8_t uart0_getc(char*);
// receive-ISR UART0

#endif
