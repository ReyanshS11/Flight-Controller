# Express Flight Controller
Express is a feature-packed flight controller for drones.

![Final Design](</screenshots/Final Design V2/Final3DModelV2_View1.png>)

## Key Features
- STM32H743VIT6 microcontroller for speed and performance
- ICM-45686 IMU for precise motion tracking
- BMP581 barometer for accurate altitude data
- NEO-M8U-06B GPS for pinpoint positioning
- Ra-02 RF transmitter for communication
- MicroSD slot for data storage
- USB-C port for program and data transfer
- 2s LiPo battery support and charging with BQ25887RGER
- Connectors for ESC and camera

## Design Features
- The complete flight controller fits within a 70x70mm space.
- Comprised of four layers, front and back copper along with two copper planes for PWR and GND
- Silkscreen for organization and ease of use

![Final Design](</screenshots/Final Design V2/Final3DModelV2_View2.png>)

## Firmware
The current firmware reads data from all sensors, callibration and further upgrades to be done when PCB is manufactured.
The current firmware is also untested, this will also be done after manufacturing.

## Compatibility
Here are the components I built it for:
- [HGLRC 60A V1 ESC](https://www.getfpv.com/hglrc-60a-v1-2-6s-blheli-s-4-in-1-esc-30-5x30-5.html)
- [Camera](https://www.getfpv.com/fpv/cameras/betafpv-p1-air-unit-hd-vtx.html)
- [Battery](https://motionrc.com/products/admiral-1300mah-2s-7-4v-30c-lipo-battery-with-xt60-connector-epr13002x6)

Although I built it for these components, it should work for pretty much anything that can connect to the JST pins in the same order as the components listed above.

## Why I Built It
I built this both as a wy to learn more about electronics, but also to be able to use this in future projects if I ever make a drone.

## Schematic and Design
![Final Design](</screenshots/Final Design V2/SchematicFinalV2.png>)

![Final Design](</screenshots/Final Design V2/FinalPCBDesignV2.png>)
