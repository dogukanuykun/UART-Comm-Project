
# UART Communication Project

This project demonstrates a UART communication system using the TMS570LS20216SZWT microcontroller. It includes handling temperature and light sensor data and responding to various types of requests via the Serial Communication Interface (SCI).

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Code Overview](#code-overview)
  - [Main Function](#main-function)
  - [Data Structures](#data-structures)
  - [Functions](#functions)
- [License](#license)

## Introduction

This project contains an application for the TMS570LS20216SZWT microcontroller. It handles UART communication, processes temperature and light sensor data, and responds to incoming requests accordingly. The main file for the application is `sys_main.c`.

## Features

- Handles UART communication using the SCI module.
- Reads temperature and light sensor data using the ADC module.
- Responds to requests for temperature, light, or both sensor data.
- Provides a structure for sending and receiving messages with sensor data.

## Installation

1. Clone this repository to your local machine.
    ```sh
    [git clone https://github.com/your-username/uart-comm-project.git](https://github.com/dogukanuykun/UART-Comm-Project.git)
    ```
2. Open the project in your preferred IDE that supports the TMS570 microcontroller development.
3. Build and upload the project to your TMS570LS20216SZWT microcontroller.

## Usage

Once the project is uploaded to the microcontroller, it will begin to:

1. Initialize the UART and ADC modules.
2. Continuously check for incoming requests.
3. Process the requests and send the appropriate sensor data responses.

## Code Overview

### Main Function

The `main` function initializes the system, enabling interrupts, initializing the SCI and ADC modules, and starting ADC conversions. It continuously checks for received messages and processes them.

### Data Structures

Several data structures are used to handle the different types of messages and sensor data:

- `T_RX_MSG`: Defines the format of a received message.
- `T_DATA_TEMP`: Defines the format of temperature data.
- `T_DATA_LIGHT`: Defines the format of light intensity data.
- `T_DATA_ALL`: Defines the format of combined temperature and light intensity data.
- `T_DATA_INVALID`: Defines the format for an invalid data response.
- `T_TX_MSG`: Defines the format of a transmitted message, including a union for different types of data.

### Functions

- `sciNotification(sciBASE_t *sci, uint32 flags)`: Handles notifications from the SCI module.
- `delayLoop(uint32_t delayInMs)`: Implements a busy-wait delay loop.
- `checkForRequest()`: Checks for and processes incoming requests, sending appropriate sensor data responses.
- `calcTemp(float adcValue)`: Calculates temperature based on an ADC value.
- `updateSensorData()`: Updates sensor data by fetching temperature and light intensity readings.
- `integerToArr(uint8 *arr, uint32 data)`: Converts a 32-bit integer into a byte array (big-endian format).

## License

This project is licensed under the terms of the MIT license. See the LICENSE file for details.

---

For more detailed information about the TMS570 microcontroller peripheral drivers, refer to the [Texas Instruments documentation](https://www.ti.com).

