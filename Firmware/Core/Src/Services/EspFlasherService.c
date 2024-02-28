#include "Services/EspFlasherService.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <gpio.h>
#include <usart.h>

#include "esp_loader.h"
#include "esp_loader_io.h"

#define BOOT_HOLD_TIME_MS  50
#define RESET_HOLD_TIME_MS 100

typedef struct LoaderConfig_T {
    UART_HandleTypeDef* uart;

    GPIO_TypeDef* gpioPortBoot;
    GPIO_TypeDef* gpioPortReset;
    uint16_t gpioPinBoot;
    uint16_t gpioPinReset;
} LoaderConfig_T;

static esp_loader_error_t ConnectToTarget(void);

static LoaderConfig_T currentConfiguration = {
    .uart          = &huart3,
    .gpioPortBoot  = ESP_BOOT_GPIO_Port,
    .gpioPortReset = ESP_RESET_GPIO_Port,
    .gpioPinBoot   = ESP_BOOT_Pin,
    .gpioPinReset  = ESP_RESET_Pin,
};

static uint32_t startTimer;
static bool isInitialized;

bool ESPFlasher_SRV_ConnectTarget() {
    return (ConnectToTarget() != ESP_LOADER_SUCCESS) ? false : true;
}

void ESPFlasher_SRV_ResetTarget() {
    loader_port_reset_target();
}

static esp_loader_error_t ConnectToTarget(void) {
    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

    esp_loader_error_t err = esp_loader_connect(&connect_config);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Cannot connect to target. Error: %u\n", err);
        return err;
    }
    printf("Connected to target\n");

    if (esp_loader_get_target() != ESP8266_CHIP) {
        err = esp_loader_change_transmission_rate(
            currentConfiguration.uart->Init.BaudRate);
        if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC) {
            printf(
                "ESP8266 does not support change transmission rate command.");
            return err;
        } else if (err != ESP_LOADER_SUCCESS) {
            printf("Unable to change transmission rate on target.");
            return err;
        } else {
            err = loader_port_change_transmission_rate(
                currentConfiguration.uart->Init.BaudRate);
            if (err != ESP_LOADER_SUCCESS) {
                printf("Unable to change transmission rate.");
                return err;
            }
            printf("Transmission rate changed changed\n");
        }
    }

    return ESP_LOADER_SUCCESS;
}

// Extern Functions
esp_loader_error_t loader_port_write(const uint8_t* data,
                                     uint16_t size,
                                     uint32_t timeout) {
    HAL_StatusTypeDef err = HAL_UART_Transmit(currentConfiguration.uart,
                                              (uint8_t*)data,
                                              size,
                                              timeout);

    if (err == HAL_OK) {
        return ESP_LOADER_SUCCESS;
    } else if (err == HAL_TIMEOUT) {
        return ESP_LOADER_ERROR_TIMEOUT;
    } else {
        return ESP_LOADER_ERROR_FAIL;
    }
}

esp_loader_error_t loader_port_read(uint8_t* data,
                                    uint16_t size,
                                    uint32_t timeout) {
    HAL_StatusTypeDef err =
        HAL_UART_Receive(currentConfiguration.uart, data, size, timeout);

    if (err == HAL_OK) {
        return ESP_LOADER_SUCCESS;
    } else if (err == HAL_TIMEOUT) {
        return ESP_LOADER_ERROR_TIMEOUT;
    } else {
        return ESP_LOADER_ERROR_FAIL;
    }
}

// Set GPIO0 LOW, then
// assert reset pin for 100 milliseconds.
void loader_port_enter_bootloader(void) {
    HAL_GPIO_WritePin(currentConfiguration.gpioPortBoot,
                      currentConfiguration.gpioPinBoot,
                      GPIO_PIN_RESET);
    loader_port_reset_target();
    HAL_Delay(BOOT_HOLD_TIME_MS);
    HAL_GPIO_WritePin(currentConfiguration.gpioPortBoot,
                      currentConfiguration.gpioPinBoot,
                      GPIO_PIN_SET);
}

void loader_port_reset_target(void) {
    HAL_GPIO_WritePin(currentConfiguration.gpioPortReset,
                      currentConfiguration.gpioPinReset,
                      GPIO_PIN_RESET);
    HAL_Delay(RESET_HOLD_TIME_MS);
    HAL_GPIO_WritePin(currentConfiguration.gpioPortReset,
                      currentConfiguration.gpioPinReset,
                      GPIO_PIN_SET);
}

void loader_port_delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}

void loader_port_start_timer(uint32_t ms) {
    startTimer = HAL_GetTick() + ms;
}

uint32_t loader_port_remaining_time(void) {
    int32_t remaining = startTimer - HAL_GetTick();
    return (remaining > 0) ? (uint32_t)remaining : 0;
}

void loader_port_debug_print(const char* str) {
    printf("DEBUG: %s", str);
}

esp_loader_error_t loader_port_change_transmission_rate(uint32_t baudrate) {
    currentConfiguration.uart->Init.BaudRate = baudrate;

    if (HAL_UART_Init(currentConfiguration.uart) != HAL_OK) {
        return ESP_LOADER_ERROR_FAIL;
    }

    return ESP_LOADER_SUCCESS;
}
