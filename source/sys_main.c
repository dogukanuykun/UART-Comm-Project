/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/** @mainpage TMS570LS20216SZWT Micro Controller Driver Documentation
*   @date 11-Dec-2018
*
*   @section sec1 Introduction
*   This document describes the TMS570 microcontroller peripheral drivers.
*
*   @section sec2 Drivers
*   @subsection sec2sub1 RTI Driver
*   Real Time Interface Module Driver.
*   @subsection sec2sub2 GIO Driver
*   General Purpose Input Output Module Driver.
*   @subsection sec2sub3 SCI Driver
*   Serial Communication Interface Module Driver.
*   @subsection sec2sub4 SPI Driver
*   Serial Peripheral Interface Module Driver.
*   @subsection sec2sub5 CAN Driver
*   Controller Area Network Module Driver.
*   @subsection sec2sub6 ADC Driver
*   Analog to Digital Converter Module Driver.
*   @subsection sec2sub7 LIN Driver
*   Local Interconnect Network Module Driver.
*   @subsection sec2sub8 HET Driver
*   High End Timer Module Driver.
*/

/* USER CODE BEGIN (0) */
#define MSG_SIZE                    30
#define DATA_RESOLUTION             4096 //2^12
#define ADC_SUPPLY_VOLTAGE          3.3
#define R0                          100000

#define NUMBER_OF_ENABLED_CHANNEL   2

//MSG TYPES
#define REQUEST_SIZE                4

#define MSG_REQUEST_TEMPERATURE     0x1208
#define MSG_REQUEST_LIGHT           0x2109
#define MSG_REQUEST_TEMP_LIGHT      0x3566

#define MSG_REPLY_TEMPERATURE       0x0101
#define MSG_REPLY_LIGHT             0x0201
#define MSG_REPLY_TEMP_LIGHT        0x0301
#define MSG_REPLY_INVALID           0x9999


#define ADC_CHANNEL_TEMPERATURE     8
#define ADC_CHANNEL_LIGHT           9

/* USER CODE END */


/* Include Files */

#include "sys_common.h"
#include "system.h"

/* USER CODE BEGIN (1) */
#include "sci.h"
#include "adc.h"
#include "rti.h"
#include "het.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

adcData_t adc_data[NUMBER_OF_ENABLED_CHANNEL];
uint8 rcvdData[REQUEST_SIZE]; // with msgNum, size should be 4
volatile float fTemperature;
volatile float fLight;
volatile uint32 temperature;
volatile uint32 light;
volatile uint16 request;
volatile uint16 msgNum;
volatile uint8 isMessageRcvd;

float calcTemp(float adcValue);
void delayLoop(uint32_t delayInMs);
void updateSensorData();
void integerToArr(uint8 *arr, uint32 data);
void checkForRequest();
/* USER CODE END */


/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */

/**
 * @brief Structure defining the format of a received message.
 *
 * This structure represents the format of a message received over a communication interface.
 * It contains two fields:
 * - `msgId`: Identifies the type or purpose of the message (16 bits).
 * - `msgNo`: Sequential number or identifier associated with the message (16 bits).
 */
typedef struct{
    uint16 msgId;
    uint16 msgNo;
}T_RX_MSG;


/**
 * @brief Structure defining the format of temperature data.
 *
 * This structure specifies the format of temperature data to be transmitted.
 * It consists of a 4-byte array (`tempArr`) to hold temperature data.
 */
typedef struct{
    uint8 tempArr[4];
}T_DATA_TEMP;


/**
 * @brief Structure defining the format of light intensity data.
 *
 * This structure specifies the format of light intensity data to be transmitted.
 * It consists of a 4-byte array (`lightArr`) to hold light intensity data.
 */
typedef struct{
    uint8 lightArr[4];
}T_DATA_LIGHT;


/**
 * @brief Structure defining the format of combined temperature and light intensity data.
 *
 * This structure specifies the format of combined temperature and light intensity data
 * to be transmitted together.
 * It contains two 4-byte arrays:
 * - `tempArr`: Array to hold temperature data.
 * - `lightArr`: Array to hold light intensity data.
 */
typedef struct{
    uint8 tempArr[4];
    uint8 lightArr[4];
}T_DATA_ALL;



typedef struct{
    uint64 invData;
}T_DATA_INVALID;


/**
 * @brief Structure defining the format of a transmitted message.
 *
 * This structure specifies the format of a message to be transmitted over a communication interface.
 * It includes:
 * - `msgId`: Identifies the type of message being transmitted (16 bits).
 * - `msgNo`: Sequential number or identifier associated with the transmitted message (16 bits).
 * - `data`: Union that allows different types of data to be transmitted based on `msgId`.
 *   - `tempData`: Structure holding temperature data (`T_DATA_TEMP`).
 *   - `lightData`: Structure holding light intensity data (`T_DATA_LIGHT`).
 *   - `allData`: Structure holding combined temperature and light intensity data (`T_DATA_ALL`).
 *   - `invalidData`: Structure holding invalid data response (`T_DATA_INVALID`).
 *
 * @note Byte alignment is set to 1 byte (`#pragma pack(push, 1)`) to ensure
 *       minimal padding between structure members for efficient data transmission.
 */
#pragma pack(push, 1)
typedef struct{
    uint16 msgId;
    uint16 msgNo;
    union{
        T_DATA_TEMP tempData;
        T_DATA_LIGHT lightData;
        T_DATA_ALL allData;
        T_DATA_INVALID invalidData;
    }data;
}T_TX_MSG;
#pragma pack(pop)
#define D_TX_MSG_SIZE   sizeof(T_TX_MSG)

/* USER CODE END */

int main(void)
{
    /* USER CODE BEGIN (3) */
    _enable_IRQ();  // Enable interrupts
    sciInit();      // Initialize SCI (serial communication interface)
    adcInit();      // Initialize ADC module
    adcStartConversion(adcREG1, adcGROUP1);         // Start ADC conversions
    sciReceive(sciREG1, REQUEST_SIZE, rcvdData);    // Initiate first receive operation

    while(1)
    {
        if (isMessageRcvd) // Check if a message has been received
        {
            checkForRequest(); // Process the received request
            isMessageRcvd = 0; // Reset the message received flag
        }
    }
    /* USER CODE END */

    return 0;
}



/* USER CODE BEGIN (4) */

/******************************************************************************
 * Function: sciNotification
 * --------------------------
 * Description:
 *   This function handles notifications from the Serial Communication Interface (SCI).
 *   It sets flags and extracts message information from received data, then initiates
 *   a new receive operation.
 *
 * Parameters:
 *   sci: Pointer to the SCI module (sciBASE_t *) from which the notification originated.
 *        Type: sciBASE_t *
 *
 *   flags: Flags indicating the type of notification received.
 *          Type: uint32
 *
 * Notes:
 *   - Assumes rcvdData is a globally accessible array where received data is stored.
 *   - Delegates further processing of received data to other parts of the application.
 ******************************************************************************/
void sciNotification(sciBASE_t *sci, uint32 flags)
{
    isMessageRcvd = 1;                              // Set flag indicating a message has been received
    request = (rcvdData[0] << 8) | rcvdData[1];     // Extract request type from received data
    msgNum = (rcvdData[2] << 8) | rcvdData[3];      // Extract message number from received data
    sciReceive(sci, REQUEST_SIZE, rcvdData);        // Initiate another receive operation for subsequent data
}

/**
 * @brief Delays execution for a specified duration in milliseconds.
 *
 * This function implements a busy-wait delay loop. It calculates the number
 * of loop iterations based on the provided delay in milliseconds and a fixed
 * loop execution time.
 *
 * @param delayInMs The delay duration in milliseconds.
 *                  Valid range: 0 to UINT32_MAX (system dependent).
 *
 * @note This function uses a busy-wait loop and consumes CPU cycles during
 *       the delay period. It is suitable for short, precise delays where
 *       timing accuracy is critical.
 */
void delayLoop(uint32_t delayInMs)
{
    volatile uint32_t count = delayInMs * 8000;
    while (count--) {
        __asm(" nop");
    }
}


/**
 * @brief Checks for and processes incoming requests, sending appropriate sensor data responses.
 *
 * This function checks the global variable `request` for specific message request types:
 * - `MSG_REQUEST_TEMP_LIGHT`   : Requests both temperature and light intensity data.
 * - `MSG_REQUEST_TEMPERATURE`  : Requests only temperature data.
 * - `MSG_REQUEST_LIGHT`        : Requests only light intensity data.
 * If none of these requests match, it sends an invalid request response.
 *
 * Upon identifying the request type:
 * - It updates sensor data using `updateSensorData()`.
 * - Prepares a response message (`txBuffer`) with the appropriate message ID (`MSG_REPLY_TEMP_LIGHT`,
 *   `MSG_REPLY_TEMPERATURE`, `MSG_REPLY_LIGHT`, or `MSG_REPLY_INVALID`) and message number (`msgNum`).
 * - Converts sensor data (`temperature` or `light`) into byte arrays using `integerToArr`.
 * - Sends the response message over the SCI interface (`sciREG1`) using `sciSend`.
 *
 * @note This function assumes the availability and correct configuration of global variables
 *       `request`, `temperature`, `light`, and `msgNum`, as well as the proper implementation
 *       of `updateSensorData()`, `integerToArr()`, and `sciSend()`.
 */
void checkForRequest(){
    uint64 invData;             // Variable to store invalid data
    T_TX_MSG txBuffer = {0};    // Initialize transmission buffer
    txBuffer.msgNo = msgNum;    // Assign message number
    updateSensorData();         // Update sensor data

    if(request == MSG_REQUEST_TEMP_LIGHT) // Check if request is for Temperature and Light SENSOR
    {
        txBuffer.msgId = MSG_REPLY_TEMP_LIGHT; // Set message ID for Temperature and Light reply
        integerToArr(txBuffer.data.allData.tempArr, temperature); // Convert temperature to array format
        integerToArr(txBuffer.data.allData.lightArr, light); // Convert light intensity to array format
    }
    else if(request == MSG_REQUEST_TEMPERATURE) // Check if request is for Temperature SENSOR
    {
        txBuffer.msgId = MSG_REPLY_TEMPERATURE; // Set message ID for Temperature reply
        integerToArr(txBuffer.data.tempData.tempArr, temperature); // Convert temperature to array format
    }
    else if (request == MSG_REQUEST_LIGHT) // Check if request is for Light SENSOR
    {
        txBuffer.msgId = MSG_REPLY_LIGHT; // Set message ID for Light reply
        integerToArr(txBuffer.data.lightData.lightArr, light); // Convert light intensity to array format
    }
    else // Invalid request case
    {
        txBuffer.msgId = MSG_REPLY_INVALID; // Set message ID for Invalid request reply
        invData = 0; // Initialize invalid data variable
        txBuffer.data.invalidData.invData = (invData | request); // Store invalid request type in data structure
    }
    sciSend(sciREG1, D_TX_MSG_SIZE, (uint8*)&txBuffer); // Send the constructed message via SCI
}

/**
 * @brief Calculates temperature in degrees Celsius based on an ADC value.
 *
 * This function computes the temperature using the Steinhart-Hart equation,
 * which relates the resistance of a temperature-sensitive resistor (thermistor)
 * to temperature. The ADC value is used to determine the resistance, which is
 * then used to compute the temperature.
 *
 * @param adcValue The ADC value read from the temperature sensor.
 *                 Valid range: 0 to DATA_RESOLUTION (system dependent).
 *
 * @return The temperature in degrees Kelvin.
 *         Calculation is based on the Steinhart-Hart equation.
 */
float calcTemp(float adcValue)
{
    float resistance;
    float temperature;
    float T0 = 298; //Kelvin (273 + 25 (room temperature))
    float B = 4250; //coefficient
    resistance = ((float)(DATA_RESOLUTION * R0) / adcValue) - R0;
    temperature = (T0*B)/(T0*log(resistance/R0) + B);
    return temperature;
}


/**
 * @brief Updates sensor data by fetching temperature and light intensity readings.
 *
 * This function calls two separate functions to update sensor data:
 * - `updateTemperatureData()`: Updates temperature data by initiating an ADC conversion,
 *   calculating temperature from the ADC result, and storing it in a global variable.
 * - `updateLightData()`: Updates light intensity data by initiating an ADC conversion,
 *   calculating light intensity from the ADC result, and storing it in a global variable.
 *
 * @note This function assumes the availability and proper implementation of the
 *       `updateTemperatureData()` and `updateLightData()` functions elsewhere in the codebase.
 *       It updates global variables (`temperature` and `light`) with the latest sensor readings.
 *       */
void updateSensorData()
{
    int i;
    adcGetData(adcREG1, adcGROUP1, adc_data); // Retrieve ADC data

    // Process each enabled ADC channel
    for (i = 0; i < NUMBER_OF_ENABLED_CHANNEL; i++)
    {
        if (adc_data[i].id == ADC_CHANNEL_TEMPERATURE)
        {
            fTemperature = calcTemp(adc_data[i].value); // Calculate temperature from ADC value
            temperature = (uint32)(fTemperature * 1000); // Convert temperature to milli-degrees Celsius
        }
        else if (adc_data[i].id == ADC_CHANNEL_LIGHT)
        {
            fLight = ((adc_data[i].value) * 0.9765625); // Convert ADC light value to lux
            light = (uint32)(fLight * 1000); // Convert light intensity to milli-lux
        }
        else
        {
            // Handle other ADC channels if necessary (currently empty)
        }
    }
}



/**
 * @brief Converts a 32-bit integer into a byte array (big-endian format).
 *
 * This function takes a 32-bit integer (data) and splits it into
 * four bytes, storing each byte in a specified array (arr) in
 * big-endian order (most significant byte first).
 *
 * @param arr Pointer to the array where the bytes will be stored.
 *            The array must have at least 4 elements allocated.
 *
 * @param data 32-bit integer data to be converted.
 *             Valid range: 0 to UINT32_MAX (system dependent).
 *
 * @note This function assumes the size and endianness of the integer
 *       and byte array are suitable for the application context. It
 *       uses bitwise operations to extract individual bytes from
 *       the integer data.
 */
void integerToArr(uint8 *arr, uint32 data)
{
    arr[0] = (0xFF000000 & data) >> 24;
    arr[1] = (0x00FF0000 & data) >> 16;
    arr[2] = (0x0000FF00 & data) >> 8;
    arr[3] = (0x000000FF & data) >> 0;
}
/* USER CODE END */
