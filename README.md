# daemon_log

Simple daemon program that logs Ubus system output to Tuya IoT core

## Prerequisites

This program is made for usage on OpenWrt systems, such as RutOS

For compilation you need the following packages to be installed:

```sh
$ sudo apt install python3.10 python2 libncurses5-dev zlib1g-dev build-essential git gawk unzip u-boot-tools
```

You should be using gcc 10 version. If you are using a newer or older version install gcc 10. Don't forget to change the default gcc and g++ version using these commands:

```sh
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 10
```

You should also have a Tuya IoT account and product set up. For more information, read [here](https://github.com/tuya/tuya-iot-core-sdk/blob/main/README.md)

## Compilation

**BEFORE YOU INSTALL:** please read the [prerequisites](#Prerequisites)

Start with cloning this repo on your local machine in the package folder:

```sh
$ git clone https://github.com/imbieras/daemon_log.git -C package
```

Next, compile by running the following commands:

```sh
$ make package/tuya-iot-core-sdk/{clean,compile}
$ make package/daemon-log/{clean,compile}
$ make package/luci-app-daemon-log/{clean,compile}
```

The resulting `.ipkg` files should appear in `./bin/packages/.../base/`

Upload these files to your system and install with opkg:

```sh
$ opkg install tuya-iot-core-sdk_0.0.1-1_<...>.ipkg
$ opkg install daemon_log_0.1.0-1_<...>.ipkg
$ opkg install luci-app-daemon-log_<...>.ipk
```

## Usage

You should be able to use the Luci web interface to enable the package

To run the program manually, use the following command in the `~/usr/bin/` directory:

```sh
$ ./daemon_log --device_id=<device_id> --product_id=<product_id> --device_secret=<device_secret>
```

To run the program in daemon mode, use the following command:

```sh
$ ./daemon_log -a --device_id=<device_id> --product_id=<product_id> --device_secret=<device_secret>
```
