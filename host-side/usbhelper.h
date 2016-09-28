/* *********************************************************************
usbhelper.h from USBIRReceiver host software
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

#ifndef __HAVE__USB_HELPER_H__
#define __HAVE__USB_HELPER_H__

#include <libusb.h>

// checkDeviceStringID — checks the given device's vendor name and
// 						product name
// @return:
// 		1 — match
//		0 — no match
//		-1 — error
int checkDeviceStringID(libusb_device *device, int iVendor, const char *vendorName,
							int iProduct, const char *productName);

// checkDevice — checks whether the given device has required idVendor,
// 				idProduct and corresponding names
// @return:
// 		1 — match
//		0 — no match
//		-1 — error
int checkDevice(libusb_device *device, int vendor, const char *vendorName,
									int product, const char *productName);

// findDevice — finds device with given parameters
// returns NULL if the device wasn't found or an error occured
libusb_device* findDevice(int vendor, const char *vendorName,
									int product, const char *productName);

#endif
