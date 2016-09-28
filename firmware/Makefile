# *********************************************************************
# Makefile from USBIRReceiver project
# Copyright (C) 2016 Daniel Rempel <danil.rempel@gmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# For full license text see License.txt
# ********************************************************************* */

PROJECT=$(notdir $(PWD))

SOURCES=main.c usbdrv/usbdrv.c usbdrv/usbdrvasm.c
OBJECTS=main.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o

all: ${PROJECT}.hex
%.o: %.c
	avr-gcc -Iusbdrv -Wall -Wpedantic -mmcu=atmega8 -Os -c $< -o $@
%.o: %.S
	avr-gcc -Iusbdrv -Wall -mmcu=atmega8 -Os -x assembler-with-cpp -c $< -o $@
${PROJECT}.bin: ${OBJECTS}
	avr-gcc -mmcu=atmega8 -Os $^ -o $@
${PROJECT}.hex: ${PROJECT}.bin
	avr-objcopy -j .text -j .data -O ihex $< $@
${OBJECTS}: main.h usbdrv/usbconfig.h
.PHONY: all flash
flash: ${PROJECT}.hex
	avrdude -c usbasp -p m8 -U flash:w:${PROJECT}.hex:i
fuse: ${PROJECT}.hex
	avrdude -c usbasp -p m8 -U lfuse:w:0xee:m -U hfuse:w:0xd9:m
.PHONY: clean
clean:
	-rm -rf *.o *.bin *.hex usbdrv/*.o
