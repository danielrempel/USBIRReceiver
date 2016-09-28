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

PROJECT=control

SOURCES=control.c usbhelper.c ../inih/ini.c
OBJECTS=control.o usbhelper.o ../inih/ini.o

all: ${PROJECT}
%.o: %.c
	gcc `pkg-config --cflags libusb-1.0` -Wall -Wpedantic -c $< -o $@
${PROJECT}: ${OBJECTS}
	gcc $^ `pkg-config --libs libusb-1.0` -o $@
.PHONY: all clean
clean:
	-rm -rf *.o ${PROJECT}
