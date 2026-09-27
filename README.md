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
├── Smart_Environmental_Monitor.ino
├── sensors.h
├── sensors.cpp
├── display.h
├── display.cpp
├── README.md
└── images/
```

## Technologies Used

* C++
* Arduino IDE
* ESP32
* I²C
* SPI
* Analog to Digital Conversion (ADC)
* Embedded Systems Programming
