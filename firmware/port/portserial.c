/*
 * FreeModbus Libary: ATMega168 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *   - Initial version and ATmega168 support
 * Modfications Copyright (C) 2006 Tran Minh Hoang:
 *   - ATmega8, ATmega16, ATmega32 support
 *   - RS485 support for DS75176
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#define UART_BAUD_RATE          9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) \
    ( ( F_OSC ) / ( ( UART_BAUD_RATE ) * 8UL ) - 1 )

//#define UART_UCSRB  UCSR0B
#define UCSRB UCSR0B
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
#ifdef RTS_ENABLE
    UCSRB |= _BV( TXEN0 ) | _BV(TXCIE0);
#else
    UCSRB |= _BV( TXEN0 );
#endif

    if( xRxEnable )
    {
        UCSRB |= _BV( RXEN0 ) | _BV( RXCIE0 );
    }
    else
    {
        UCSRB &= ~( _BV( RXEN0 ) | _BV( RXCIE0 ) );
    }

    if( xTxEnable )
    {
        UCSRB |= _BV( TXEN0 ) | _BV( UDRE0 );
#ifdef RTS_ENABLE
        RTS_HIGH;
#endif
    }
    else
    {
        UCSRB &= ~( _BV( UDRE0 ) );
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    UCHAR ucUCSRC = 0;

    /* prevent compiler warning. */
    (void)ucPORT;
	
    UBRR0 = UART_BAUD_CALC( 9600, F_CPU );
    UCSR0A |= _BV (U2X0);

    ucUCSRC |= _BV( UPM01 );
    ucUCSRC |= _BV( UCSZ00 ) | _BV( UCSZ01 );
    
#if defined (__AVR_ATmega168__)
    UCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega169__)
    UCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega8__)
    UCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega88PA__)
    UCSR0C |=  ucUCSRC;
#elif defined (__AVR_ATmega16__)
    UCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega32__)
    UCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega128__)
    UCSRC |= ucUCSRC;
#endif

    vMBPortSerialEnable( FALSE, FALSE );

#ifdef RTS_ENABLE
    RTS_INIT;
#endif
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    UDR0 = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = UDR0;
    return TRUE;
}

SIGNAL( USART_UDRE_vect )
{
    pxMBFrameCBTransmitterEmpty(  );
}

SIGNAL( USART_RX_vect )
{
    pxMBFrameCBByteReceived(  );
}

#ifdef RTS_ENABLE
SIGNAL( USART_TX_vect )
{
    RTS_LOW;
}
#endif

