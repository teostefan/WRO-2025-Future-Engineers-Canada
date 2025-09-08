# WRO-2025-Future-Engineers-Canada

The following repository contains the code and technical documentation regarding
the robot created by the CarletonU-uOttawa joint team for the 2025 WRO Future
Engineers competition.

![Robot Image](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/robot_iso.jpg)

# Contents

- [About our team](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/wiki/Mobility-Management)
- [Software setup](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/wiki/Obstacle-Management)
- [Power and Sensor Management](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/wiki/Power-and-Sensor-Management)
- [Obstacle Management]()
- [Mobility Management]()

# About our team

Oscar Barbieri is a student at the University of Ottawa and is studying software engineering, and Stefan Teodorescu is a student a Carleton University studying aerospace avionics. Last year, Oscar and Stefan participated in the WRO Future Innovators challenge and made it into the international finals in İzmir. This year, both students have decided to participate in the WRO Future Engineers competition as a means to further their knowledge regarding mechatronics and systems programming.

# Software setup

## Compiling and running the robot's firmware (Linux)

To get started, clone the repository and build the firmware using `make`:

```sh
git clone https://github.com/teostefan/WRO-2025-Future-Engineers-Beta.git
cd WRO-2025-Future-Engineers-Beta
make
```

> **Note**: Depending on how your Linux system is configured, an "insufficient
> privileges" error may be thrown on compilation regarding the file
> `kf_matrix_gen.sh`. To solve this, run
> `sudo chmod 777 ./software/kf/kf_matrix_gen.sh` in the repository's root. If
> this error still persists, use `sudo` in front of `make` as your user might
> not have enough privileges to execute the above bash file.

Then, simply run the `main` executable in the generated `builds` folder:

```sh
builds/main
```

## Compiling and running individual tests

To generate the test executables, run:

```sh
make all-tests
```

Afterwards, all tests will be in the `builds` folder and have a name in the
format `test_xx`. Each test can then be individually executed as in the
following example:

```sh
builds/test_cv
```

## Software dependencies

If running on Linux, all of the below dependencies should be pre-installed.

- `ffmpeg` - Abstraction layer for the driver, also used for applying filters to
  the video feed.
- `v4l2` - Video4Linux2 multimedia drivers.
- `ffplay` - Media player GUI for debugging.
- `unistd.h` - The UNIX standard header.

# Power and Sensor Management

|![Annotated Diagram](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_sensors.svg)|
|:--:|
|*Figure 1: Annotated diagram of the vehicle. Individual components are discussed in the subsequent sections.*|

This section pertains to the electrical engineering aspects of our vehicle. Design choices such as computing platforms, power supplies, sensors and motor controllers are discussed. The majority of peripherals have been purchased as breakout boards, and a custom Raspberry-P "Hat" acts as a distribution box and motor controller. A general block diagram of the robot's electrical systems is provided below.

|![Drivetrain Diagram](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_wiring.svg)|
|:--:|
|*Figure 2: Diagram of the vehicle's electrical systems. Schematics regarding the custom Raspberry Pi Hat PCB and 5V converter for the servo are provided in the sections below.*|

## Computing Platform

The main controller for the robot is a Raspberry Pi 4B Single Board Computer (SBC). All processing is done on this platform, and no additional controllers have been used, except for an ATTINY45 that generates Pulse-Width Modulation (PWM) to control the speed on the driving motor. This ensures that the system remains simple and allows for a smaller vehicle, which implies better manoeuvrability. The Raspberry-Pi model 4B was specifically chosen as it has a good amount of processing capabilities for the computer-vision algorithms and does not excessively consume power like the newer model 5 (~2.5A@5V versus 5A@5V).

## Power Supply

The vehicle is powered by a 2Ah lithium-ion battery pack that feeds 7.4V nominal into a 3.4A fixed 5V buck regulator and a variable 2A buck-boost regulator set to approximately 9V. The 5V regulator supplies the Raspberry Pi, while the 9V regulator supplies the driving motor. As the steering servo requires 5V only, the 9V from the motor power supply is stepped down to 5V via a simple LM7805 linear regulator circuit (see Figure 3). Although the 5V could have been supplied from the main 5V regulator, this would have been undesirable as any electrical noise caused by the servomotor could permeate to the Raspberry Pi and cause instabilities. For supplying the sensors which work at 3.3V, the internal regulator on the Raspberry Pi is used.

|![Servo Regulator Schematic](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/schematic_servo_regulator.png)|
|:--:|
|*Figure 3: Schematic of the LM7805 5V regulator for the steering servo. Production files for this board can be found in the [`pcb/servo_regulator`](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/tree/main/pcb/servo_regulator) directory.*|

|![Servo Regulator Top View](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/pcb_servo_regulator_top_view.png)|
|:--:|
|*Figure 4: Rendering of the servo regulator custom PCB in KiCAD.*|

## Sensor and Motor Management

In addition to the camera for obstacle detection, as many sensors as possible have been installed on the robot to simplify the control algorithms and to avoid estimating data. For example, all four sides of the robot have optical Time-of-Flight (ToF) distance sensors, allowing the vehicle not only to determine it's rectilinear position, but also to measure clearances precisely for manoeuvres such as parallel parking. In addition, a gyroscope has been installed which can measure absolute rotation angles of the robot. The gyro sensor is also used in a proportional controller to keep the robot driving straight when required.

All sensors, except for the camera which is connected via USB, are interfaced via a common I²C bus. To prevent conflicts in between sensors which have the same addresses (the 4 ToF sensors), an I²C-controlled multiplexer is used for connecting the main bus to only one sensor at a time.

Also on this main I²C bus is an ATTINY45 which is configured as a slave. It generates PWM signals that are then used to drive the DC driving motor via an H-bridge at varying speeds in forwards or reverse. The steering servomotor is separate and is controlled directly by the Raspberry Pi through PWM.

In order to reduce the number of external components and keep the wiring relatively manageable, a custom Raspberry Pi "Hat" has been designed. It includes connectors for all of the individual peripherals and integrates the driving motor controller as well as the I²C multiplexer. The circuit board was designed in KiCAD and all relevant files can be found in the `pcb/main` directory. See Figure 5 at the end of this page for the main board's schematic, and the green inset in Figure 2 for the main board's block diagram.

### I²C Peripheral Mapping

**Note:** All I²C peripherals are on I²C channel 1 (GPIO2/SDA & GPIO3/SCL).

| Peripheral | Description | I²C Address | Related Drivers |
| ---------- | ----------- | ----------- | --------------- |
| ESC        | Electronic speed controller for main motor. It is just an ATTNY25/45 programmed with the firmware contained in the `software/esc/esc.ino` C++ file. GPIO23 is also used by the driver to put this peripheral to sleep. | `0x08` | `software/esc` |
| BNO055     | Gyroscope/IMU. By default, the address of this sensor will be `0x29`, but this will interfere with the ToF sensors. To select the alternate address `0x28`, pull the `ADC` pin on the breakout board low. | `0x28` | `software/gyro` |
| VL53L0X    | One of four laser ToF sensors. As all sensors have the same address, they must be selected via the I²C switch (see next row). | `0x29` | `software/tof` |
| TCA9544    | 4 Channel I²C switch / multiplexer for selecting 1 of 4 VL53L0X sensors. | `0x70` | `software/i2cmux` |

### Non-I²C Peripheral Mapping

|   Peripheral   | Description | Pin number & GPIO | Related Drivers |
| -------------- | ----------- | ----------- | --------------- |
| SG-5010        | Standard RC PWM-controlled servo for steering. | `GPIO13/PIN33/PWM1` | `software/esc` |
| KANO Webcam    | Gooseneck USB Camera for computer vision.   | `USB` | `software/cv` |

|![Main Board Schematic](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/schematic_main.png)|
|:--:|
|*Figure 5: Schematic of the main board. Production files for this board can be found in the [`pcb/main`](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/tree/main/pcb/main) directory.*|

|![Main Board Top View](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/pcb_top_view.png)|
|:--:|
|*Figure 6: Rendering of the main board custom PCB in KiCAD.*|

## Bills of Materials

A general Bill of Materials that includes the hardware components as well as the off-the shelf parts can be found at the end of this page. If one desires to fabricate the main board and the servo regulator board, the component-level BOMs can be found in [`pcb/main/main.csv`](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/tree/main/pcb/main/main.csv) and [`pcb/servo_regulator/servo_regulator.csv`](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/tree/main/pcb/servo_regulator/servo_regulator.csv).

## Issues faced regarding the sensors

A recurring issue faced during the development of the robot was the instability of the VL53L0X laser time-of-flight sensors (ToF sensors). Despite the [datasheet](https://www.st.com/resource/en/datasheet/vl53l0x.pdf) stating that the performance of the sensor was independent from surface properties, we noticed a very large change in behaviour from when the robot was tested on a black mat versus a glossy white mat like the one used in during the competition. When the ground was highly reflective, the robot would randomly detect walls and behave erratically (see videos below). This occurred whether or not the lights were turned on, suggesting an issue with the sensor's own laser rather than with the ambient light. The effect was also partially corroborated by a [forum post](https://community.st.com/t5/imaging-sensors/vl53l0x-distance-reading-varies-with-reflectivity/td-p/226904) on the manufacturer's website.


https://github.com/user-attachments/assets/d4aabfce-8ad7-4a68-8b8d-6be23073b4cf


https://github.com/user-attachments/assets/641f2a84-3ea8-4779-814c-0cd47e98a4df


The solution to this issue was to simply have the ToF sensors tilted upwards by 10 degrees with some special 3D printed washers:

|![Tilt Washers](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/image_tilt_washer_1.jpg)|![Tilt Washers](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/image_tilt_washer_2.jpg)|
|:--:|:--:|
|*Figure 7A: Tilt washers for the front ToF sensor.*|*Figure 7B: Tilt washers for the lateral ToF sensors.*|

This most likely reduced any reflections of the floor coased by the ToF laser's scattering.

# Obstacle Management

For autonomous obstacle avoidance capabilities, the vehicle uses Computer Vision (CV). All software in the vehicle has been written in C, and no premade CV library is used by our code (such as OpenCV). Everything has been written from scratch as a learning experience and to reduce bloat. More details regarding the robot's algorithms is provided below.

## General Algorithm

The following flowchart showcases the robot's behaviour:

|![Algorithm Flowchart](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_algorithm.jpg)|
|:--:|
|*Figure 8: Flowchart of the robot's software.*|

The open challenge uses the lateral ToF sensors to detect when one of the track sides gives way, indicating the direction in which it should turn. For the obstacle challenge, the computer vision algorithm determines the position of the obstacles for the first round, and then stores all positions into an "evidence map". For the next three rounds, the robot can proceed faster as it uses the stored obstacle position instead of recomputing it from camera data every time.

## Computer Vision

In order to detect and avoid obstacles, the robot centres itself at the beginning of each straight section on the mat to determine the obstacles in it's path. It grabs a frame from the camera, and processes it to figure out the positioning and colour of the obstacles/signs.

|![Computer Vision Diagram](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_computer_vision.svg)|
|:--:|
|*Figure 9: Algorithm for the robot's Computer Vision. Relevant code can be found in [```software/cv```](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/tree/main/software/cv). Obstacle image credit: [WRO Association](https://world-robot-olympiad-association.github.io/future-engineers-gs/p04-cv/).*|

FFMpeg is used as a wrapper for the V4L linux webcam driver to interface with the USB camera and to perform the Gaussian blurring of the image for removing optical noise. The result's pixel data is then piped and stored in a C static array for further processing. Contrast is increased by multiplying each RGB value by 1.5 and then brightness is decreased by subtracting 0.2 from all pixels. This allows bright colours to stand out more and mute fainter colours. Afterwards, the image data is converted from reed-green-blue (RGB) to hue-saturation-luminosity (HSL). A [recursive flood-fill algorithm](https://en.wikipedia.org/wiki/Flood_fill) then subdivides the image into coloured regions by hue, whilst rejecting low saturation and high luminosity regions. While the flood fill algorithm is performed, an accumulator stores the minimum and maximum x and y values for each coloured region to form bounding boxes. Bounding boxes with a minimum dimension less than a threshold are rejected, and the centroids of the remaining bounding boxes are computed. Due to perspective, the y-position of these bounding boxes are used to determine which coloured obstacle is nearer to the robot.

|![Flood Fill Demonstration Animation](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/animation_recursive_flood_fill.gif)|
|:--:|
|*Figure 10: A simple animation of the flood fill algorithm. Like coloured connected pixels are traversed to find the minimum and maximum values in x and y of the region. Animation credit: [Wikimedia Commons](https://en.wikipedia.org/wiki/File:Recursive_Flood_Fill_4_(aka).gif)*|

## Obstacle Avoidance

Based on the above Computer Vision algorithm, there are four possible cases returned:

|Case|Obstacle Positioning|
|:--:|:--:|
| 1  | Single RED obstacle |
| 2  | Single GREEN obstacle |
| 3  | RED closer, GREEN further |
| 4  | GREEN closer, RED further |

The exact x positioning of these obstacles is not considered, as the possible x-variation is very small (20cm). Based on these four cases, the robot then chooses one of four paths:

|Case|Diagram of the path taken for the detected obstacle configuration|
|:--:|:--:|
| 1  | ![Case 1](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_trajectory_case_1.svg) |
| 2  | ![Case 2](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_trajectory_case_2.svg) |
| 3  | ![Case 3](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_trajectory_case_3.svg) |
| 4  | ![Case 4](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_trajectory_case_4.svg) |

# Mobility Management

The robot and it's linkages/driveshafts are almost fully 3D printed, with only the differential gear being bought off the shelf. All of it's mechanical components were fully designed in IronCAD. This section provides an overview on the decisions regarding the robot's mobility and construction. To see a more in depth overview of the assembly, skip to the [Assembly Instructions](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/wiki/Mobility-Management#assembly-instructions-and-boms) section on this page.

## Mechanical Design

For mobility, our vehicle uses a rear wheel drive along with front wheel steering. Keeping the steering wheelset separate from the driving wheelset reduces mechanical complexities and removes the need for universal joints in the drive shafts. The powered rear-wheels are connected to the motor via a ring and pinion differential that allows the individual wheel speeds to vary so long as their average is equal to the speed provided by the driving motor. This allows the vehicle to steer efficiently with minimum wheel slippage on the ground. All rotating shafts are connected to the frame through ball-bearings to minimise friction and wear.

|![Drivetrain Diagram](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/diagram_drivetrain.svg)|
|:--:|
|*Figure 11: Diagram of the vehicle's drivetrain, as seen from the bottom. For an exploded view of all the components, see the vehicle construction section below.*|

## Motors

A brushed Mabuchi RK-370CA-15370 direct-current (DC) motor was chosen for the driving motor as it is easier to control than a brushless motor, requiring only a simple H-bridge circuit instead of a three-phase inverter. The motor's power is regulated through pulse-width modulation.

## Vehicle Construction

The chassis is fully 3D-printed, with files available in the [`hardware` directory](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/tree/main/hardware) at the root of this repository. The front steering and rear driving are implemented as two separate assemblies which are built individually and then attached together via two beams, forming the structure. The Raspberry-Pi 4 is then screwed into the back and the custom PCB is attached on the 40-pin header. A plate supporting all of the regulators is screwed into the back, while breakout boards for the ToF sensors are attached into the side of the chassis through 20 mm M3 brass standoffs. The front ToF sensor is attached with a special 3D printed bracket that also holds the IMU/Gyroscope breakout. A rack in-between the Raspberry-Pi and the steering motor holds the 2 Ah lithium-ion batteries.

|![Front View](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/robot_front.jpg)|![Side View](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/robot_side.jpg)|![Rear View](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/robot_rear.jpg)|
|:--:|:--:|:--:|
|*Figure 12A: Front view of the vehicle.*|*Figure 12B: Side view of the vehicle.*|*Figure 12C: Rear view of the vehicle. See the `v-photos` directory for all six views.*|

## Assembly Instructions and BOMs

A detailed PDF assembly manual has been provided for reference alongside with a BOM for all of the hardware components. Some parts in the BOM must be purchased, while others are to be 3D printed. All 3D printed parts have been made in black Bambu Lab PLA. A seperate BOM for the two custom PCBs are provided in [`pcb/main/main.csv`](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/tree/main/pcb/main/main.csv) and [`pcb/servo_regulator/servo_regulator.csv`](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/tree/main/pcb/servo_regulator/servo_regulator.csv). 

A preview of the BOM and assembly manual is provided below. For better viewing and to zoom-in, please download the [pdf](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/hardware/Assembly_Manual_and_BOM.pdf).

![Assembly Manual and BOM Page 1](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/Assembly_Manual_and_BOM_page_previews/Assembly_Manual_and_BOM-1.jpg)

![Assembly Manual and BOM Page 2](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/Assembly_Manual_and_BOM_page_previews/Assembly_Manual_and_BOM-2.jpg)

![Assembly Manual and BOM Page 3](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/Assembly_Manual_and_BOM_page_previews/Assembly_Manual_and_BOM-3.jpg)

![Assembly Manual and BOM Page 4](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/Assembly_Manual_and_BOM_page_previews/Assembly_Manual_and_BOM-4.jpg)

![Assembly Manual and BOM Page 5](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/Assembly_Manual_and_BOM_page_previews/Assembly_Manual_and_BOM-5.jpg)

![Assembly Manual and BOM Page 6](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/Assembly_Manual_and_BOM_page_previews/Assembly_Manual_and_BOM-6.jpg)

![Assembly Manual and BOM Page 7](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/Assembly_Manual_and_BOM_page_previews/Assembly_Manual_and_BOM-7.jpg)

![Assembly Manual and BOM Page 8](https://github.com/teostefan/WRO-2025-Future-Engineers-Canada/blob/main/media/Assembly_Manual_and_BOM_page_previews/Assembly_Manual_and_BOM-8.jpg)
