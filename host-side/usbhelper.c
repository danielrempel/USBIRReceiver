/* *********************************************************************
usbhelper.c from USBIRReceiver host software
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
#include "usbhelper.h"
#include <stdio.h>
#include <string.h>

int checkDeviceStringID(libusb_device *device, int iVendor, const char *vendorName,
							int iProduct, const char *productName) {

	const int STRING_LENGTH = 256;
	unsigned char stringDescription[ STRING_LENGTH ];

	if( (iVendor <= 0) || (iProduct <= 0) )
		return -1; // no string — no comparsion

	libusb_device_handle *deviceHandle = NULL;
	int ret = libusb_open( device, & deviceHandle );
	if ( LIBUSB_SUCCESS != ret ) {
		printf("%s:%d: failed to open device. Root access may be required\n", __FILE__, __LINE__);
		return -1;
	}

	// =================================================================
	// CHECK manufacturer info
	ret = libusb_get_string_descriptor_ascii( deviceHandle, iVendor, stringDescription, STRING_LENGTH );
	if ( ret < 0 ) {
		printf("%s:%d: failed to get string descriptor\n", __FILE__, __LINE__);
		libusb_close(deviceHandle);
		return -1;
	}

	if (0 != strcmp(vendorName, stringDescription)) {
		libusb_close(deviceHandle);
		return 0; // no match
	}

	// =================================================================
	// CHECK product info
	ret = libusb_get_string_descriptor_ascii( deviceHandle, iProduct, stringDescription, STRING_LENGTH );
	if ( ret < 0 ) {
		printf("%s:%d: failed to get string descriptor\n", __FILE__, __LINE__);
		libusb_close(deviceHandle);
		return -1;
	}

	if (0 != strcmp(productName, stringDescription)) {
		libusb_close(deviceHandle);
		return 0; // no match
	}

	libusb_close(deviceHandle);

	// we're still here thus the device has passed the checks
	return 1;
}

int checkDevice(libusb_device *device, int vendor, const char *vendorName,
									int product, const char *productName) {
	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(device, &desc);
	if (r < 0) {
		printf("%s:%d: failed to get device descriptor\n", __FILE__, __LINE__);
		return -1;
	}

	// check numbers
	if( (vendor == desc.idVendor) &&
		(product == desc.idProduct) )
	{
		// check passed, check text lines now:
		return checkDeviceStringID(device, desc.iManufacturer, vendorName,
										desc.iProduct, productName);
	}

	return 0;
}

libusb_device* findDevice(int vendor, const char *vendorName,
									int product, const char *productName) {
	libusb_device **list;
	libusb_device *found = NULL;

	ssize_t cnt = libusb_get_device_list(NULL, &list);
	ssize_t i = 0;
	int ret = 0;
	if (cnt < 0) {
		printf("%s:%d: No devices found\n", __FILE__, __LINE__);
		libusb_free_device_list(list, 1);
		return NULL;
	}

	for (i = 0; i < cnt; i++) {
		libusb_device *device = list[i];

		ret = checkDevice(device, vendor, vendorName, product, productName);

		if(1 == ret) {
			found = device;
			break;
		}
		if(-1 == ret) {
			break; // error
		}
	}

	libusb_free_device_list(list, 1);

	return found;
}

