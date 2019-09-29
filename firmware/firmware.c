/*
 * FreeModbus Libary: AVR Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- AVR includes -------------------------------------*/
#include "avr/io.h"
#include "avr/interrupt.h"
#include "avr/eeprom.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* IO port implementation */
void init_output() {
  // PC0-PC3 are connected to switching transistors, so set them to output
  DDRC = 0x0F;
  PORTC = 0x00;
}



/* ----------------------- Start implementation -----------------------------*/
uint8_t usRegInputBuf;
int
main( void )
{
  init_output();
  //  eMBErrorCode    eStatus;
  uint8_t addr = eeprom_read_byte (0);
  
  eMBInit( MB_RTU, addr, 0, 9600, MB_PAR_EVEN );
  
  sei(  );
  
  /* Enable the Modbus Protocol Stack. */
  eMBEnable(  );
  
  for( ;; )
    {
      ( void )eMBPoll(  );
      usRegInputBuf++;
    }
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  uint16_t i;
  for (i=usAddress;i < usAddress+usNRegs;i++) {
    switch(i) {
      // relay outputs
    case 1000:
      if (eMode == MB_REG_READ) {
	*pucRegBuffer++ = PORTC;
	*pucRegBuffer=0;
      } else {
	PORTC = *pucRegBuffer++;
      }
      break;
    case 1001:
      if (eMode == MB_REG_READ) {
	*pucRegBuffer++ = usRegInputBuf;
	*pucRegBuffer=0;
      }
      break;

    
    default:
      return MB_ENOREG;
    }
  }  
  return MB_ENOREG;
}


