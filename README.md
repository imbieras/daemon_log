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

## Lua scripts

### Usage

Place your Lua scripts in the designated script folder (default: /usr/bin/scripts). Each script should have the following hooks:
    
- `get_data` **(mandatory)**: Data collection function that is called repeatedly by the C program. This hook should collect and return the desired data. The C program will call this hook multiple times during its execution
- `init` **(optional)**: Initialization function that is called once during the program startup. Use this hook to load files into memory, open connections, or perform any setup required by the script. This hook is not mandatory and can be omitted if not needed
- `destroy` **(optional)**: Cleanup function that is called once at the end of the program. Use this hook to close connections, release resources, or perform any necessary cleanup actions. This hook is not mandatory and can be omitted if not needed

The program will load and execute the Lua scripts present in the script folder

### Script management

- Only scripts with valid descriptions (containing the get_data hook) will be loaded into the program
- Each script is loaded once during the lifetime of the program and remains in program memory until the program terminates
- The program can maintain 20 scripts in memory at any given time. If more scripts are present, the excess scripts will be ignored
- If a script is modified or added while the program is running, it will not be automatically loaded. You need to restart the program for the changes to take effect

### Customizing Script Folder

By default, the script folder is set to /usr/bin/scripts. If you want to change the script folder location, follow these steps:

1. Open the header file `lua_helper.h`
1. Locate the `SCRIPTS_DIR` macro definition
1. Modify the path to the desired script folder
1. Recompile the program

```c
#define SCRIPTS_DIR "/usr/bin/scripts"
```

### Notes

- Ensure that the Lua scripts adhere to the specified hook functions and their requirements
- Take care to handle errors and exceptions within the Lua scripts to avoid unexpected program behavior
- Lua scripts can utilize various Lua libraries and functions to perform complex logic and data processing
