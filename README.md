# daemon_log

Simple daemon program that logs command output to Tuya IoT core

## Prerequisites

Before compiling and running this program, ensure that the following dependencies are installed on your system:

```sh
$ cmake --version
cmake version 3.22.1
$ make --version
GNU Make 4.3
$ gcc --version
gcc (Ubuntu 11.3.0-1ubuntu1~22.04.1) 11.3.0<
```

- libconfig
- cJSON

You should also have a Tuya IoT account and product set up. For more information, read [here](https://github.com/tuya/tuya-iot-core-sdk/blob/main/README.md)

## Compilation

**BEFORE YOU INSTALL:** please read the [prerequisites](#Prerequisites)

Start with cloning this repo on your local machine:

```sh
$ git clone https://github.com/imbieras/daemon_log.git
$ cd daemon_log
```

Download all the needed dependencies according to your distribution

Clone the **tuya-iot-core-sdk** repo:

```sh
$ git clone https://github.com/tuya/tuya-iot-core-sdk.git
```

Change all `CMakeLists.txt` files to compile `SHARED` libraries

Compile it by doing the following:

```sh
$ cd tuya-iot-core-sdk
$ mkdir build && cd build
$ cmake ..
$ make
```

Now, navigate to the `/src` directory:

```sh
$ cd ../../src
```

Compile the program:

```sh
$ make
```

The binary `daemon_log` should appear in the project root directory

## Usage

To run the program, use the following command:

```sh
$ ./daemon_log --device_id=<device_id> --product_id=<product_id> --device_secret=<device_secret>
```

To run the program in daemon mode, use the following command:

```sh
$ ./daemon_log -a --device_id=<device_id> --product_id=<product_id> --device_secret=<device_secret>
```
