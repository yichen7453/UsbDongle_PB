#include "stdafx.h"

#include "libusb.h"


// ------+---------+---------+---------+---------+---------+---------+---------+
// define
// ------+---------+---------+---------+---------+---------+---------+---------+
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))
#define CTRL_OUT		(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT)
#define CTRL_IN			(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN)
#define CTRL_OUT		(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT)
#define	EP_DATA			(1 | LIBUSB_ENDPOINT_IN)
#define EP_INTR			(2 | LIBUSB_ENDPOINT_IN)
#define USB_RQ			0x05

#define PKT_SIZE		4162

// ------+---------+---------+---------+---------+---------+---------+---------+
// FPD Command
// ------+---------+---------+---------+---------+---------+---------+---------+
#define FPD_CMD_WRITE_REG		0x01
#define FPD_CMD_READ_REG		0x02
#define FPD_CMD_READ_FRAME		0x03
#define FPD_CMD_SET_INIT_TABLE	0x04
#define FPD_CMD_GET_ARRTIB		0x05
#define FPD_CMD_SET_ARRTIB		0x06
#define FPD_CMD_START_STREAM	0x07
#define FPD_CMD_STOP_STREAM		0x08
#define FPD_CMD_SELECT_MODE		0x09




int usb_open_device(uint16_t vid, uint16_t pid);
int usb_sensor_reset(struct libusb_device_handle *dev);
int usb_sensor_init(struct libusb_device_handle *dev);
int usb_sensor_start_stream(struct libusb_device_handle *dev);
int usb_sensor_stop_stream(struct libusb_device_handle *dev);
int usb_sensor_get_image_data(unsigned char** image_data, uint16_t* data_len);