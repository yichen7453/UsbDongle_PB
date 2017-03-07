#include "stdafx.h"

#include "usb_ctl.h"

uint8_t toggle = 0x02;

struct libusb_device_handle *dev = NULL;

static uint8_t regs_table[] = {
	0xE0, 0x02,  // AMP
	0xE1, 0x1A,  // VRT 1.60V
	0xE2, 0x10,  // VRB 0.100V + 0.025V * 0x21
	0xE6, 0x12   // DC offset, VCC=3.0V use 0x10, VCC=3.3V use 0x15
};

static uint8_t reset_table[] = {
	0xD2, 0x10,  // sleep mode
	0xD1, 0x80   // soft reset, delay must put here
};

static uint8_t init_table[] = {
	0xD2, 0x10,
	0x30, 0x0A,
	0x31, 0x0A,
	0x32, 0x07,
	0x35, 0x08,
	0x36, 0x8C,
	0x37, 0x64,
	0x38, 0x07,
	0xE0, 0x02,  // AMP
	0xE1, 0x1A,  // VRT 1.60V
	0xE2, 0x10,  // VRB 0.100V + 0.025V * 0x21
	0xE6, 0x12,  // DC offset, VCC=3.0V use 0x10, VCC=3.3V use 0x15
	0x3A, 0x80,
	0x3B, 0x02,
	0xD2, 0x13,
	0xD3, 0x00   // clear status, disable DVR, VRB calibration
};

int usb_open_device(libusb_device_handle *dev, uint16_t vid, uint16_t pid)
{
	int res = -1;

	res = libusb_init(NULL);
	if (res < 0) {
		printf("usb_ctl -> usb_init fali. (%d)\n", res);
	}
	else {
		printf("usb_ctl -> usb_init success. (%d)\n", res);

		dev = libusb_open_device_with_vid_pid(NULL, vid, pid);

		if (dev != NULL) {
			res = libusb_claim_interface(dev, 0);
			if (res < 0) {
				printf("usb_ctl -> clain_interface fail. (%d)\n", res);
			}
			else {
				printf("usb_ctl -> claim_interface success. (%d)\n", res);
			}
		} else {
			res = -1;
		} 
	}

	return res;
}

int usb_sensor_reset(struct libusb_device_handle *dev)
{
	int res = -1;

	if (dev != NULL) {
		res = libusb_control_transfer(dev, CTRL_OUT, FPD_CMD_WRITE_REG, 0, 0, reset_table, ARRAYSIZE(reset_table), 0);
		if (res < 0) {
			printf("usb_ctl -> usb_sensor_reset fail. (%d)\n", res);
		}
		else {
			printf("usb_ctl -> usb_sensor_reset success. (%d)\n", res);
		}
	}

	return res;
}

int usb_sensor_init(struct libusb_device_handle *dev)
{
	int res = -1;

	if (dev != NULL) {
		res = libusb_control_transfer(dev, CTRL_OUT, FPD_CMD_WRITE_REG, 0, 0, init_table, ARRAYSIZE(init_table), 0);
		if (res < 0) {
			printf("usb_ctl -> usb_sensor_init fail. (%d)\n", res);
		}
		else {
			printf("usb_ctl -> usb_sensor_init. (%d)\n", res);
		}
	}

	return res;
}

int usb_sensor_start_stream(struct libusb_device_handle *dev) {

	int res = -1;
	uint16_t frameSize;

	if (dev != NULL) {
		frameSize = 0;
		res = libusb_control_transfer(dev, CTRL_OUT, FPD_CMD_START_STREAM, 0, 0, (uint8_t *)&frameSize, 2, 0);
		if (res < 0) {
			printf("usb_ctl -> usb_sensor_start_stream fail. (%d)\n", res);
		}
		else {
			printf("usb_ctl -> usb_sensor_start_stream. (%d)\n", res);
		}
	}

	return res;
}

int usb_sensor_stop_stream(struct libusb_device_handle *dev) {

	int res = -1;
	uint16_t frameSize;

	if (dev != NULL) {
		frameSize = 0;
		res = libusb_control_transfer(dev, CTRL_OUT, FPD_CMD_STOP_STREAM, 0, 0, (uint8_t *)&frameSize, 2, 0);
		if (res < 0) {
			printf("usb_ctl -> usb_sensor_stop_stream fail. (%d)\n", res);
		}
		else {
			printf("usb_ctl -> usb_sensor_stop_stream. (%d)\n", res);
		}
	}

	return res;
}