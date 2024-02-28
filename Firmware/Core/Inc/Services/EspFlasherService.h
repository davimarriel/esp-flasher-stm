/**
 * @file EspFlasherService.h
 * @author Davi Marriel (davi.marriel@42we.tech)
 * @brief
 * @version 0.1
 * @date 27-02-2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "ReturnStatus.h"
#include "stdbool.h"
#include "stdint.h"

typedef enum ESP_BINARY_TYPE_E {
    ESP_BINARY_TYPE_BOOTLOADER = 0,
    ESP_BINARY_TYPE_APPLICATION,
    ESP_BINARY_TYPE_PARTITION_TABLE,
} ESP_BINARY_TYPE_E;

bool ESPFlasher_SRV_ConnectTarget();
void ESPFlasher_SRV_ResetTarget();
ReturnStatus_E ESPFlasher_SRV_SetupWrite(uint32_t regionAddress,
                                         uint32_t totalBinarySize,
                                         uint32_t chunkSize);
ReturnStatus_E ESPFlasher_SRV_Write(const uint8_t* data, uint32_t length);