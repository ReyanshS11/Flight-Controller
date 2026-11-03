---
title: "Flight Controller"
github: ""
description: "A flight controller built for future projects that might need it."
created_at: "2026-10-12"
total_time: "26h 24m"
---

# October 12, 2026: Built Parts List

I added in all of the parts needed for this project including researching each of them. I used the helper file to create all of the footprints from EasyEDA and LCSC. Next, I'm going to build the PCB in KiCad.

![](https://fabricate.hackclub-assets.com/e96ad33aaeab188c2affaf0dd89effbe97aa32ba687e84023f44ae3df7b0b67b/image-png/PartsOrder.png)

**Total time spent: 2h 30m**

# October 15, 2026: Updated parts and started new part wiring

I updated a few of the parts, specifically the microcontroller and barometer, I updated it to a better version of them. I also researched a little bit on drone flight controllers and what they need. I figured out that I need to add a way to communicate to the ESC, a magnetometer or GPS (or both).

I started on the wiring of the batter charger based on the datasheet.

Next, I will finish the wiring of the battery charger.

![](https://fabricate.hackclub-assets.com/71db466b8ee2219b79ccd59a4be813fa629d849e45ef847936c253c6ae2da08f/image-png/StartedWiring.png)

**Total time spent: 2h 30m**

# October 15, 2026: Finished wiring for battery charger.

I finished the full wiring for the battery charger, now it's time to move onto the next parts like the sensors. I also researched a little bit more about ESC's and how to implement them into my current design.

![](https://fabricate.hackclub-assets.com/487586a81e356a435fb5e744180b47326d1b6ca23f267f096768e8bda94ce71b/image-png/FinishedBatteryChargerWiring.png)

**Total time spent: 2h**

# October 16, 2026: Finished wiring for buck-boost

I finished the wiring for the next part, which is the 5V buck-boost. I also added in the correct parts for the inductors because they needed different models.

![](https://fabricate.hackclub-assets.com/62a59eb15e3580f432f81b6bf9e027bd31bd251bd7b14934d5ae8336a2a8e99d/image-png/VoltageRegulator.png)

**Total time spent: 54m**

# October 17, 2026: Added new parts

I added in 3 new parts that I wanted. I added in a GPS, a LoRa module, and a magnetometer. The GPS is the NEO-M8U-06B, the LoRa module is the Ra-02, and the magnetometer is the LIS2MDLTR.

![](https://fabricate.hackclub-assets.com/fafee0831c7dbd88fef71e80fc2daaf1d8a96a7aa19140803893bc1cec0b7cb6/image-png/GPS_LoRa_MGN_Added.png)

**Total time spent: 1h**

# October 17, 2026: Finished wiring more components

I finished wiring the 3.3 buck, barometer, and the IMU. With this, I'm completely finished with the power components and a good bit through the sensors. Next, I will finish wiring all of the sensors and work a little bit on LoRa and GPS.

![](https://fabricate.hackclub-assets.com/acaa99bff72158d58cb9740dc9ee3ad2e9b51eae3a509dd24dc80224715ce96f/image-png/3.3Buck_Barometer_IMU.png)

**Total time spent: 48m**

# October 18, 2026: Wired MicroSD and Magnetometer

I finished wiring two more parts, the MicroSD and the magnetometer. I used SPI communication for the magnetometer instead of I2C like most of the other components. Next, I will wire the microcontroller.

![](https://fabricate.hackclub-assets.com/c3dafb204825a0ef604b6a2357227f9a1d8c3959ad1d8b54a989883334a52848/image-png/MicroSD_Magnetometer.png)

**Total time spent: 1h**

# October 18, 2026: Wired GPS

I wired the GPS component for the flight controller, including adding in the antenna needed for it. It's using the SPI communication. Next, I will either wire the MCU or LoRa module and then I'm almost done. I don't know if I want to add in a way to connect a camera yet, so I'll have to figure that out soon.

![](https://fabricate.hackclub-assets.com/29b6e576c0a49da1b095d8a74c8726d7fffbe84aae5e999b00f5488ff1987377/image-png/GPSWiring.png)

**Total time spent: 1h 12m**

# October 21, 2026: Connected MCU

I added in all of the connections for the MCU to peripherals in STM32CubeMX. I ended up using only one of the DIO pins from LoRa because I think that should be enough for all of the useful info I need. I still need to wire it in KiCad and a few other small components.

![](https://fabricate.hackclub-assets.com/23b51735b6b8736605dbbddba5dbf9105144bb0473789b790825342945a40848/image-png/MCUConnections.png)

**Total time spent: 1h 30m**

# October 23, 2026: Wiring MCU

I started on the MCU wiring, including adding in the buttons and crystals needed for it. I also added in the jst connecters for the ESC and camera (I decided to add it in).

![](https://fabricate.hackclub-assets.com/9198c4fdbfae8e402607a4e1cc4f97686d6d7395ff3e395209f38792ff37e535/image-png/StartedMCUWiring.png)

**Total time spent: 2h**

# October 24, 2026: Wired MCU and finished schematic

I finished wiring up the MCU and made the final touches to the schematic. Now, the only thing left to do is to route the PCB. I decided to use the RF_DIO1 pin as well and I changed to ESC pins from GPIO output to PWM generation channels.

![](https://fabricate.hackclub-assets.com/d9294a1a4d69774ef582f75deecb68472597fc7417a605d8d0ebb3cb96dbd688/image-png/FinishedSchematic.png)

**Total time spent: 2h**

# October 25, 2026: Switched Battery Chargers

I switched the battery charger I'm using from the BQ25883 to the BQ25887 so that I can get the cell balancing feature because it allows me to charge my battery using the USB safely.

![](https://fabricate.hackclub-assets.com/9c0758d8d6dffc4e4c92e5293f81ec1b835d0f647b72513617a87ac0caf62884/image-png/SwitchedBatteryChargers.png)

**Total time spent: 1h**

# October 28, 2026: Started PCB Design

I started designing the PCB. I placed all of the main components along with the parts connected to them. Next, I will organize the PCB and finalize the design.

![](https://fabricate.hackclub-assets.com/c6ae7106b16562d650ab7272e5c33a4c0afc14abe9ed7f1f0f1722c3dec18916/image-png/PCBDesignPart1.png)

**Total time spent: 1h**

# October 31, 2026: Started Routing

I started routing the PCB. I finished routing around half of the total components, and finalized the PCB layout. Next, I'll finish routing and do the ground plane.

![](https://fabricate.hackclub-assets.com/22255bbceeac10d996abfc5fe2ed8998d30ac41b19ee8121c869c0a37a68ae99/image-png/RoutingPart1.png)

**Total time spent: 2h**

# November 2, 2026: Redesign & Routing

I completely redesigned the board because I realized that some of my components were placed so far away from the MCU pins they needed to be connected to that it would be impossible to connect them. I had almost finished the routing when I realized this so I deleted all of it and I changed the board and started routing again.

![](https://fabricate.hackclub-assets.com/6e45c578d90ca92c8ad666bf84805a6cc2e09f57033c4a1b6159fc2a04af7221/image-png/Redesign%26Routing.png)

**Total time spent: 2h 30m**

# November 2, 2026: Finished PCB (minus silkscreen)

I finished the design and routing of the whole PCB so there aren't any more DRC errors that matter. The only ones left are errors complaining about hole width and clearance in the USB and MicroSD and warnings about the silkscreen. Next, I will finish the silkscreen.

![](https://fabricate.hackclub-assets.com/280d1039ab2f8960c6360432f8b2c95c39c4b6587817e492004728136f645039/image-png/PCBFinishedWithoutSilkscreen.png)

**Total time spent: 1h 30m**

# November 3, 2026: Finishing Touches

I finished organizing the silkscreen for the components and I added in the 3d models for all of the components so the 3D viewer now shows how the PCB will look after everything is on there.

![](https://fabricate.hackclub-assets.com/03e82c6b4d7508f2212c89754d39bb284df5fb964c9de40746c54c01649e62fa/image-png/FinishingTouches.png)

**Total time spent: 1h**
