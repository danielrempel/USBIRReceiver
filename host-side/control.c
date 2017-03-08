/* *********************************************************************
control.c from USBIRReceiver host software
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

#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../inih/ini.h"

#include "usbhelper.h"

void execmd(const char* cmdWithArgs)
{
	FILE* proc;
	proc = popen(cmdWithArgs,"r");
	pclose(proc);
}

char*** addressPool;
const uint addressPoolSize = 256;

// handler for inih library
static int iniLineHandler(void* userPtr, const char* section, const char* name,
                   const char* value)
{
	uint sectionInt;
	sscanf(section, "%x", &sectionInt);
	uint nameInt;
	sscanf(name, "%x", &nameInt);
	printf("Config: 0x%X / 0x%x -> '%s'\n", sectionInt, nameInt, value);

	// check presence of a block (section) in address pool
	if(NULL == addressPool[sectionInt]) {
		// create a new cmd pool
		addressPool[sectionInt] = (char**)malloc(256 * sizeof(char*));
		if(NULL == addressPool) {
			printf("Failed to allocate command pool 0x%X\n", sectionInt);
			return 0;
		}
	}

	// if there's already a command, remove it to put the new one
	if(NULL != addressPool[sectionInt][nameInt]) {
		free(addressPool[sectionInt][nameInt]);
	}

	addressPool[sectionInt][nameInt] = strdup(value);

	return 1;
}

// Initialises arrays and reads the configuration file
void initAddressPool()
{
	addressPool = (char***)malloc(256 * sizeof(char**));
	if(NULL == addressPool)
	{
		printf("Failed to allocate address pool\n");
		return 1;
	}
	int i;
	for(i=0; i < addressPoolSize; i+=1)
	{
		addressPool[i] = NULL;
	}

	if (ini_parse("usbirreceiver-config.ini", iniLineHandler, NULL) < 0)
	{
		printf("Can't load 'usbirreceiver-config.ini'\n");
		return 1;
	}
}

// Cleanup routines on exit
int exitCleanup()
{
	int i;
	int j;

	libusb_exit(NULL);

	for(i=0; i < addressPoolSize; i+=1)
	{
		if(NULL != addressPool[i])
		{
			// free all the strings
			for(j = 0; j < addressPoolSize; j+=1)
			{
				if(NULL != addressPool[i][j])
					free(addressPool[i][j]);
			}
			free(addressPool[i]);
		}
	}

	free(addressPool);

	return 0;
}

void devicePoll(libusb_device_handle *deviceHandle)
{
	unsigned char bytes[8] = { 0 };
	int len = 0;
	int ret;
	while (1)
	{
		ret = libusb_interrupt_transfer(deviceHandle,
								1 | LIBUSB_ENDPOINT_IN,
								bytes, 8, &len, 10000);
		if( ret < 0 )
		{
			// TODO: stay silent on timeout
			printf("%s:%d: interrupt transfer failed: %s\n", __FILE__, __LINE__, libusb_strerror(ret));
			if(LIBUSB_ERROR_NO_DEVICE  == ret)
			{
				break;
			}
		}
		else
		{
			if( (NULL!=addressPool[bytes[0]]) && (NULL!=addressPool[bytes[0]][bytes[2]]) )
			{
				printf("Cmd: 0x%X/0x%x -> '%s'\n",
						bytes[0], bytes[2], addressPool[bytes[0]][bytes[2]]);
				execmd(addressPool[bytes[0]][bytes[2]]);
			} else {
				// unknown cmd, display it to user:
				printf("Unk. cmd: 0x%X/0x%X\n", bytes[0],bytes[2]);
			}
		}
	}
}

int main()
{

	initAddressPool();

	atexit(exitCleanup);

	int i;
	int ret = 0;
	ret = libusb_init(NULL);
	if (ret < 0)
	{
		printf("%s:%d: failed to init libusb\n", __FILE__, __LINE__);
		return 1;
	}

	while(1)
	{
		// NOTE: there may be cases when the device is unreferenced by
		// libusb_free_device_list(..) and thus made unavailable unless is
		// opened
		// TODO: check device ID, whether it can be used
		libusb_device* device = findDevice(0x16C0, "danil.rempel@gmail.com", 0x05DC, "USBIRReceiver");

		if( NULL == device )
		{
			printf("%s:%d: Device not found!\n", __FILE__, __LINE__);
			sleep(5);
			continue;
		}

		// connect to the device
		libusb_device_handle *deviceHandle = NULL;
		ret = libusb_open(device, &deviceHandle);
		if ( LIBUSB_SUCCESS != ret )
		{
			printf("%s:%d: failed to open device. Either root access or udev config required\n", __FILE__, __LINE__);
			sleep(5);
			continue;
		}

		printf("%s:%d: Connected to the device\n", __FILE__, __LINE__);

		// start polling for interrupt transfers
		devicePoll(deviceHandle);

		libusb_close(deviceHandle);
		// Maybe the device was unplugged. Wait one second before
		// retrying
		sleep(1);
	}

}
