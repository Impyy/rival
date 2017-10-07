# rival

__rival__ is a graphical configuration tool for Linux that allows you to change
the settings of SteelSeries Rival mice.

The following mice are currently supported:

* SteelSeries Rival 100

## Screenshot

![](https://u.impy.me/dl?l=GaBqORVaEflkiB9g.png)

## Installation

There are no pre-compiled binaries available at this time.

### Prerequisites

__rival__ requires support for hidraw to be enabled in the kernel. It also
depends on udev.

### Compiling

You'll need CMake to run the build script.

Dependencies:
* libudev
* [libconfig](http://www.hyperrealm.com/libconfig)
* [libui](https://github.com/andlabs/libui)

```sh
mkdir build && cd $_
cmake ..
make
```

The binary can be found in _build/bin_.

### Setup

Make sure your user has permission to read/write to the hidraw device node of
your mouse. 

Example: To fix the permissions for your Rival 100, add yourself to the plugdev
group and add the following rule to udev:

```
SUBSYSTEM=="hidraw", ATTRS{idVendor}=="1038", ATTRS{idProduct}=="1702", MODE="0660", GROUP="plugdev"
```

## The wishlist

* An option to configure the mouse directly from the command line
* Support for the Rival 300
* Support for the Rival 700
