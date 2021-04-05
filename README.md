# iOS-BLE-maximum-througput-example

A prototype to demonstrate high throughput between a peripheral running with Zephyr RTOS and a central on iOS/OSX using CoreBluetooth framework.

## Setup ##

### Peripheral 

To run the peripheral use the following setups.

1) In the project directory, create a python venv and activate it

```bash
python3 -m venv venv
source venv/bin/activate
```

2) Now setup a Zephyr Workspace

```bash
pip3 install west
west init .
west update
west zephyr-export
pip3 install -r zephyr/scripts/requirements.txt
```
3) Build the peripheral prototype

```bash
west build peripheral
```

4) Connect an NRF board to your computer and flash the prototype on it

```bash
west flash
```

5) To view the NRF Console use minicom to connect to the board

```bash
minicom -b 115200 -D /dev/tty.usbmodem... 
```

### Central

To run the central use the following setups.

1) In the Central Directory, open MaximumThroughput.xcodeproj in Xcode.

2) From the schemes choose either an iOS and OSX target

3) To run the App on your selected target, **Run** from the Product menu or press Command-R. For the iOS target please make sure and iOS Device is connected with USB.


