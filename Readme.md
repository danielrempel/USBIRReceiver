USBIRReceiver — an Atmega8L-based USB device to receive IR NEC commands from remote
control and configurable host software to run commands upon receiving
configured codes.

Device firmware is using Objective Development's V-USB (https://www.obdev.at/vusb/).
Host software uses Ben Hoyt's 'inih' library (https://github.com/benhoyt/inih).

Host software launches commands from its configuration file so it is
important to run the program from ordinary user. The user will need
rights to access the usb device.
I expect that adding a udev rule like this one:
> SUBSYSTEM=="usb", ATTR{idVendor}=="16c0", ATTR{idProduct}=="05dc", GROUP==<group>, MODE="0666"
will be enough.

Building:
* put usbdrv folder from OBdev's V-USB to the firmware folder without
	replacing the usbconfig.h file
* put Ben Hoyt's 'inih' library souces (ini.c,ini.h) to <project-root>/inih/
* firmware: cd firmware; make; make flash [; make fuse -- in case you need to re-fuse]
* host-side: cd host-side; make

NOTE: firmware's Makefile is configured to work with avrdude & USBAsp

NOTE: commands in ini file must not contain quoted spaces, up to 20 args

NOTE: repeat cmd from IR NEC is sent as 0xFA/0xFA by the receiver
		according to the standard it may be sent 104ms after the initial press
		from my experience it's not enough, so I just did code any reaction to it
		(on my remote a single click is constantly treated as a click + repeat)
