/* *********************************************************************
main.c from USBIRReceiver firmware
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

// NOTE: the state machine may become stuck upon receiving non IR NEC signal
// TODO: ^^ reset the state machine's state to NONE 50-80 ms after the
//		initial pulse

#include "main.h"

#define SENSOR_STATE_PULSE					0
#define SENSOR_STATE_SPACE					1

// For state machine:
#define SIGNAL_STATE_NONE					0
#define SIGNAL_STATE_INIT_PULSE				1
#define SIGNAL_STATE_INIT_SPACE				2
#define SIGNAL_STATE_BIT_PULSE				3
#define SIGNAL_STATE_BIT_SPACE				4
#define SIGNAL_STATE_REPEATCMD_END_PULSE	5

// vv for prescaler /8
// vv = 3ms
#define MINIMAL_INIT_SPACE_LEN_IN_TIMER_CYCLES 4500
// vv = 0.7ms
#define MINIMAL_LOGICAL_ONE_SPACE_IN_TIMER_CYCLES 1350
// vv a bit less than 9ms
#define MINIMAL_INIT_PULSE_LEN_IN_TIMER_CYCLES 13500


// no commands are available.
USB_PUBLIC uchar usbFunctionSetup(uint8_t data[8]) {
	return 0;
}

uint8_t databuf[8];
uint8_t signalState = SIGNAL_STATE_NONE;
uint16_t timerValue = 0;
uint8_t bitCount = 0;

inline void stateMachine () {
	switch(signalState) {
	case SIGNAL_STATE_NONE:
		// noise-proofing: start counting the length of initial pulse
		TCNT1 = 0;
		// zero everything
		databuf[0] = 0;
		databuf[1] = 0;
		databuf[2] = 0;
		databuf[3] = 0;
		bitCount = 0;

		signalState = SIGNAL_STATE_INIT_PULSE;
		break;
	case SIGNAL_STATE_INIT_PULSE:
		// noise-proofing: initial pulse must be about 9ms
		if (TCNT1 < MINIMAL_INIT_PULSE_LEN_IN_TIMER_CYCLES) {
			// noise!
			signalState = SIGNAL_STATE_NONE;
		} else {
			TCNT1 = 0; // start measuring initial space length
			signalState = SIGNAL_STATE_INIT_SPACE;
		}
		break;
	case SIGNAL_STATE_INIT_SPACE:
		timerValue = TCNT1;
		// if space_len >= 3ms
		if (timerValue >= MINIMAL_INIT_SPACE_LEN_IN_TIMER_CYCLES) {
			// initial space is ordinary, next pulse is a part of a bit
			signalState = SIGNAL_STATE_BIT_PULSE;
		} else {
			// initial space is short — the commad is 'repeat'
			signalState = SIGNAL_STATE_REPEATCMD_END_PULSE;
		}
		break;
	case SIGNAL_STATE_BIT_PULSE:
		TCNT1=0;
		bitCount += 1;
		signalState = SIGNAL_STATE_BIT_SPACE;
		if(bitCount > 32) {
			signalState = SIGNAL_STATE_NONE;
		}
		break;
	case SIGNAL_STATE_BIT_SPACE:
		timerValue = TCNT1;
		// if space_len >= 0.7ms
		if (timerValue >= MINIMAL_LOGICAL_ONE_SPACE_IN_TIMER_CYCLES) {
		// ^^ we've got a logical 1; else do nothing
			SetBit(
				databuf[(bitCount-1) / 8], // I count bits from 1, processor
				(bitCount-1)%8				// counts them from 0
				);
		};

		// if we've already gathered all 32 bits — send the data
		if ((bitCount >= 32) && (usbInterruptIsReady())) {
			usbSetInterrupt(databuf, 8);
		}
		signalState = SIGNAL_STATE_BIT_PULSE;
		break;
	case SIGNAL_STATE_REPEATCMD_END_PULSE:
		// notify that we received a 'repeat' command from remote
		if( usbInterruptIsReady() ) {
			databuf[0] = 0xfa;
			databuf[1] = 0xaa;
			databuf[2] = 0xfa;
			databuf[3] = 0xaa;
			usbSetInterrupt(databuf, 8);
		}
		signalState = SIGNAL_STATE_NONE;
		break;
	}
}

int main() {

	PinIn(DDRD,7);
	PinLow(PORTD,7);

	uint8_t pulseState = SENSOR_STATE_SPACE;
	uint8_t prevPulseState = pulseState;
	uint8_t i;

	// enable 16-bit timer1
	// CS11 bit in TCCR1B means F_CPU/8 frequency for timer1
	SetBit(TCCR1B,1);
	TCNT1 = 0;

	// enable 1s watchdog timer:
    wdt_enable(WDTO_1S);
    usbInit();

	// enforce re-enumeration:
    usbDeviceDisconnect();
    for(i = 0; i<250; i+=1) {
        wdt_reset();
        _delay_ms(2);
    }
    usbDeviceConnect();

    sei();

    while(1) {
        wdt_reset();
        usbPoll();

        cli(); // the state machine code takes from 1.5 to 6 us
				// hope, that doesn't matter for V-USB :)
        // note: may be changed to pulseState = !GetPinHigh?
        if(GetPinHigh(PIND,7)) {
			pulseState = SENSOR_STATE_SPACE;
		}
		else {
			pulseState = SENSOR_STATE_PULSE;
		}

		if(pulseState != prevPulseState) {
			prevPulseState = pulseState;
			// state machine
			stateMachine();
		}
		sei();
    }

    return 0;
}
