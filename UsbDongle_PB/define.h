#include "libusb.h"

// ------+---------+---------+---------+---------+---------+---------+---------+
// define
// ------+---------+---------+---------+---------+---------+---------+---------+
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))

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

// ------+---------+---------+---------+---------+---------+---------+---------+
// FPD Command
// ------+---------+---------+---------+---------+---------+---------+---------+
int max_enrollment_samples_301b = 24;
int max_enrollment_samples_301c = 32;
int max_enrollment_samples_501 = 16;

int max_fingerprint_area_301b = 40;
int max_fingerprint_area_301c = 35;
int max_fingerprint_area_501 = 80;

int enrollment_image_quality_301b = 40;
int enrollment_image_quality_301c = 50;
int enrollment_image_quality_501 = 40;

int enrollment_fingerprint_area_301b = 24;
int enrollment_fingerprint_area_301c = 24;
int enrollment_fingerprint_area_501 = 24;

int enrollment_return_coverage = 0;

int verify_image_quality_301b = 30;
int verify_image_quality_301c = 30;

int verify_fingerprint_area_301b = 10;