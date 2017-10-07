#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "rival.h"

struct rival *rival_new(rival_handler_opened *opened, rival_handler_closed *closed)
{
	struct rival *rival = calloc(1, sizeof(struct rival));
	rival->fd = -1;
	rival->udev = udev_new();
	rival->on_opened = opened;
	rival->on_closed = closed;
	return rival;
}

static void rival_close(struct rival *rival)
{
	if (rival && rival->fd >= 0) {
		close(rival->fd);
		rival->fd = -1;
	}
}

void rival_free(struct rival *rival)
{
	rival_close(rival);
	udev_unref(rival->udev);
	free(rival);
}

static bool rival_check(struct rival *rival, struct udev_device *dev)
{
	dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
	if (!dev) {
		return false;
	}

	const char *id_vendor = udev_device_get_sysattr_value(dev,"idVendor");
	const char *id_product = udev_device_get_sysattr_value(dev, "idProduct");
	if (strcmp(id_vendor, "1038") != 0 || strcmp(id_product, "1702") != 0) {
		return false;
	}

	return true;
}

static bool rival_open(struct rival *rival, struct udev_device *dev)
{
	const char *path = udev_device_get_devnode(dev);
	printf("path: %s\n", path);
	rival->fd = open(path, O_RDWR | O_NONBLOCK);
	if (rival->fd < 0) {
		return false;
	}
	rival->on_opened();
	return true;
}

static bool rival_find(struct rival *rival)
{
	struct udev_enumerate *enumeration = udev_enumerate_new(rival->udev);
	udev_enumerate_add_match_subsystem(enumeration, "hidraw");
	udev_enumerate_scan_devices(enumeration);

	struct udev_list_entry *dev_list_entry;
	struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumeration);
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path = udev_list_entry_get_name(dev_list_entry);
		struct udev_device *dev = udev_device_new_from_syspath(rival->udev, path);
		if (!rival_check(rival, dev)) {
			continue;
		}
		bool success = rival_open(rival, dev);
		udev_device_unref(dev);
		if (success) {
			break;
		}
	}
	udev_enumerate_unref(enumeration);

	if (rival->fd < 0) {
		return false;
	}

	return true;
}

void rival_monitor(struct rival *rival, bool *end)
{
	// check whether the mouse is already connected
	rival_find(rival);

	// setup a monitor the check for new connections and disconnects
	struct udev_monitor *mon = udev_monitor_new_from_netlink(rival->udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(mon, "hidraw", NULL);
	udev_monitor_enable_receiving(mon);
	int fd = udev_monitor_get_fd(mon);

	for (;;) {
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		struct timeval timeout = {0};
		timeout.tv_sec = 1;

		int res = select(fd + 1, &fds, NULL, NULL, &timeout);
		if (res > 0 && FD_ISSET(fd, &fds)) {
			struct udev_device *dev = udev_monitor_receive_device(mon);
			if (dev) {
				// only check devices we're interested in
				if (rival_check(rival, dev)) {
					const char *action = udev_device_get_action(dev);
					if (strcmp(action, "add") == 0) {
						// a new device was added, ignore it if we already have a device
						if (rival->fd < 0) {
							rival_open(rival, dev);
						}
					} else if (strcmp(action, "remove") == 0) {
						// a device was removed, try to find another
						rival_close(rival);
						rival->on_closed();
						rival_find(rival);
					}
				}

				udev_device_unref(dev);
			}					
		}

		if (*end) {
			return;
		}
	}
}

static bool rival_set_feature(struct rival *rival, uint8_t *payload, size_t payload_size)
{
	return ioctl(rival->fd, HIDIOCSFEATURE(payload_size), payload) >= 0;
}

bool rival_set_light_color(struct rival *rival, uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t payload[] = { 0x05, 0x00, r, g, b };
	return rival_set_feature(rival, payload, sizeof(payload));
}

bool rival_set_light_mode(struct rival *rival, RIVAL_LIGHT_MODE mode)
{
	if (mode == RIVAL_LIGHT_MODE_OFF) {
		return rival_set_light_color(rival, 0, 0, 0);
	}

	// map to the correct RIVAL_LIGHT_MODE by adding 1
	uint8_t payload[] = { 0x07, 0x00, mode + 1 };
	return rival_set_feature(rival, payload, sizeof(payload));
}

bool rival_set_dpi(struct rival *rival, RIVAL_DPI_PRESET preset, RIVAL_DPI dpi)
{
	// map to the correct RIVAL_DPI_PRESET and RIVAL_DPI by adding 1
	uint8_t payload[] = { 0x03, preset + 1, dpi + 1 };
	return rival_set_feature(rival, payload, sizeof(payload));
}

bool rival_set_rate(struct rival *rival, RIVAL_RATE rate)
{
	// map to the correct RIVAL_RATE by adding 1
	uint8_t payload[] = { 0x04, 0x00, rate + 1 };
	return rival_set_feature(rival, payload, sizeof(payload));
}

bool rival_save(struct rival *rival)
{
	uint8_t payload[] = { 0x09, 0x00 };
	return rival_set_feature(rival, payload, sizeof(payload));
}

bool rival_get_name(struct rival *rival, uint8_t *buffer, size_t buffer_size)
{
	return ioctl(rival->fd, HIDIOCGRAWNAME(buffer_size), buffer) >= 0;
}
