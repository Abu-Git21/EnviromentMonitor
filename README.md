# EnviroMonitor

## Overview

EnviroMonitor is an ESP32 based environmental monitoring system designed to collect and display real time environmental data.

The system measures carbon dioxide (CO₂) levels, temperature, humidity, and ambient light. Sensor readings are displayed on a 2.8 inch TFT screen and sent to the Serial Monitor for testing and debugging.

The project combines embedded programming, sensor integration, analog input, I²C communication, and SPI communication to create a real time environmental monitoring system.

## Features

* Real time CO₂ monitoring
* Temperature monitoring
* Humidity monitoring
* Ambient light measurement
* Live TFT display dashboard
* Serial Monitor output for debugging
* Sensor initialization and connectivity checks
* Real time environmental data visualization

## Hardware

* ESP32 Development Board
* SCD30 CO₂, Temperature, and Humidity Sensor
* LDR Photoresistor
* 2.8 inch SPI TFT Display
* Breadboard
* Jumper Wires

## System Components

### SCD30 Environmental Sensor

The SCD30 measures three environmental parameters:

* Carbon dioxide concentration
* Temperature
* Relative humidity

The sensor communicates with the ESP32 using the I²C protocol.

### LDR Light Sensor

The light dependent resistor (LDR) measures ambient light levels. It connects to an analog input on the ESP32, allowing the system to read changes in light intensity.

### TFT Display

The 2.8 inch TFT display provides a real time dashboard showing:

* CO₂ concentration
* Temperature
* Humidity
* Ambient light levels

The display communicates with the ESP32 using the SPI protocol.

## Communication Protocols

| Component   | Communication Method |
| :---------- | :------------------- |
| SCD30       | I²C                  |
| LDR         | Analog Input         |
| TFT Display | SPI                  |

## Project Structure

```text
EnviroMonitor/
├── EnviroMonitor.ino
├── EnviroMonitor.h
├── README.md
├── LICENSE
└── .gitignore
```

## Technologies Used

* C++
* Arduino IDE
* ESP32
* I²C
* SPI
* Analog to Digital Conversion (ADC)
* Embedded Systems Programming

## Hardware Requirements

• ESP32 development board
• SCD30 CO₂, temperature, and humidity sensor
• 2.8 inch SPI TFT display (ILI9341)
• Light dependent resistor (LDR)
• 10 kΩ resistor
• Three pushbuttons
• Breadboard and jumper wires

## Pin Assignments

| Component     | ESP32 Pin |
| ------------- | --------- |
| SCD30 SDA     | GPIO 21   |
| SCD30 SCL     | GPIO 22   |
| TFT CS        | GPIO 5    |
| TFT DC        | GPIO 2    |
| TFT RST       | GPIO 4    |
| TFT MOSI      | GPIO 23   |
| TFT SCK       | GPIO 18   |
| LDR           | GPIO 34   |
| Up Button     | GPIO 32   |
| Down Button   | GPIO 33   |
| Select Button | GPIO 25   |

## Software Requirements

• Arduino IDE
• ESP32 board support package
• Adafruit GFX Library
• Adafruit ILI9341 Library
• SparkFun SCD30 Arduino Library

## Installation

1. Install the Arduino IDE and ESP32 board support package.
2. Install the required libraries through the Arduino Library Manager.
3. Download or clone this repository.
4. Open `EnviroMonitor.ino` in the Arduino IDE.
5. Connect the ESP32 and select the appropriate board and port.
6. Upload the code to the ESP32.

## Operation

EnviroMonitor provides three menu sections:

• **Environment:** Displays temperature, humidity, CO₂ concentration, and ambient light readings.
• **Connectivity:** Displays Wi-Fi and sensor connectivity information.
• **System Status:** Displays the operational status of the system components.

The three pushbuttons provide menu navigation and selection.

## Technical Implementation

The system uses I²C communication to retrieve measurements from the SCD30 and SPI communication to control the TFT display. The LDR is connected to an analog input to measure relative ambient light levels.

Sensor readings are displayed on the TFT and printed to the Serial Monitor for debugging.

## Limitations

The LDR provides relative light measurements rather than calibrated lux values. Wi-Fi functionality requires network configuration. Actual sensor readings depend on the connected hardware and operating conditions.
