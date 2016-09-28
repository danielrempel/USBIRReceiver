/* *********************************************************************
main.h from USBIRReceiver firmware
Copyright (C) 2016 Daniel Rempel <danil.rempel@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

For full license text see License.txt
********************************************************************* */

#ifndef HAVE_MAIN_H
#define HAVE_MAIN_H

#ifndef F_CPU
#define F_CPU 12000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "usbdrv/usbdrv.h"

#define SetBit(PORT, BIT) PORT|=_BV(BIT)
#define ClearBit(PORT, BIT) PORT&=~_BV(BIT)

#define PinHigh(P,B) SetBit(P, B)
#define PinLow(P,B) ClearBit(P, B)

// Use with DDRX
#define PinOut(P,B) SetBit(P, B)
#define PinIn(P,B) ClearBit(P, B)

// Returns true if a pin is high
// USE PINX
#define GetPinHigh(P,B) ((P & _BV(B))!=0)

#endif
