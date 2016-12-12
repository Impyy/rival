#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <libudev.h>

#include "rival.h"

int rival_open_first(struct rival_device **dev)
{
	struct rival_device *temp_dev = malloc(sizeof(struct rival_device));
	temp_dev->fd = -1;

	struct udev_list_entry *dev_list_entry;

	struct udev *udev = udev_new();
	if (!udev) {
		printf("unable to create udev context\n");
		return 1;
	}

	struct udev_enumerate *enumeration = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumeration, "hidraw");
	udev_enumerate_scan_devices(enumeration);

	struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumeration);
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path = udev_list_entry_get_name(dev_list_entry);
		struct udev_device *dev = udev_device_new_from_syspath(udev, path);
		path = udev_device_get_devnode(dev);

		dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
		if (!dev) {
			continue;
		}
	
		const char *id_vendor = udev_device_get_sysattr_value(dev,"idVendor");
		const char *id_product = udev_device_get_sysattr_value(dev, "idProduct");

		if (!strcmp(id_vendor, "1038") && !strcmp(id_product, "1702")) {
			// open the device and break out
			temp_dev->fd = open(path, O_RDWR | O_NONBLOCK);
			break;
		}

		udev_device_unref(dev);
	}

	udev_enumerate_unref(enumeration);
	udev_unref(udev);

	if (temp_dev->fd < 0) {
		printf("could not open device: %d\n", errno);
		return 1;
	}

	*dev = temp_dev;
	return 0;
}

static bool rival_set_feature(struct rival_device *dev, uint8_t *payload, size_t payload_size)
{
	return ioctl(dev->fd, HIDIOCSFEATURE(payload_size), payload) >= 0;
}

bool rival_set_light_color(struct rival_device *dev, uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t payload[] = { 0x05, 0x00, r, g, b };
	return rival_set_feature(dev, payload, sizeof(payload));
}

bool rival_set_light_mode(struct rival_device *dev, RIVAL_LIGHT_MODE mode)
{
	if (mode == RIVAL_LIGHT_MODE_OFF) {
		return rival_set_light_color(dev, 0, 0, 0);
	}

	uint8_t payload[] = { 0x07, 0x00, mode };
	return rival_set_feature(dev, payload, sizeof(payload));
}

bool rival_set_dpi(struct rival_device *dev, RIVAL_DPI_PRESET preset, RIVAL_DPI dpi)
{
	uint8_t payload[] = { 0x03, preset, dpi };
	return rival_set_feature(dev, payload, sizeof(payload));
}

bool rival_set_rate(struct rival_device *dev, RIVAL_RATE rate)
{
	uint8_t payload[] = { 0x04, 0x00, rate };
	return rival_set_feature(dev, payload, sizeof(payload));
}

bool rival_save(struct rival_device *dev)
{
	uint8_t payload[] = { 0x09, 0x00 };
	return rival_set_feature(dev, payload, sizeof(payload));
}

bool rival_get_name(struct rival_device *dev, uint8_t *buffer, size_t buffer_size)
{
	return ioctl(dev->fd, HIDIOCGRAWNAME(buffer_size), buffer) >= 0;
}


void rival_close(struct rival_device *dev)
{
	if (!dev) {
		return;
	}

	if (dev->fd >= 0) {
		close(dev->fd);
	}

	free(dev);
}
