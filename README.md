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

## Firmware
The current firmware reads data from all sensors, callibration and further upgrades to be done when PCB is manufactured.
The current firmware is also untested, this will also be done after manufacturing.

![Final Design](</screenshots/Final Design V2/Final3DModelV2_View2.png>)

## Schematic and Design
![Final Design](</screenshots/Final Design V2/SchematicFinalV2.png>)

![Final Design](</screenshots/Final Design V2/FinalPCBDesignV2.png>)
