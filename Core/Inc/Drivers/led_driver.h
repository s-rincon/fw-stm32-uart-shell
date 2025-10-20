/**
 * @file led_driver.h
 * @brief LED driver interface for STM32 microcontrollers.
 *
 * Provides simple LED control with basic functions.
 * Supports multiple LED instances with independent control.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#ifndef __LED_DRIVER_INC_
#define __LED_DRIVER_INC_

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/**
 * @struct led_interface_t
 * @brief LED hardware interface structure.
 */
typedef struct led_interface_ {
    GPIO_TypeDef *port;             /**< GPIO port */
    uint16_t pin;                   /**< GPIO pin */

} led_interface_t;

/**
 * @struct led_driver_t
 * @brief LED driver instance structure.
 */
typedef struct led_driver_ {
    led_interface_t interface;      /**< LED hardware interface */

    uint32_t blink_period_ms;       /**< Blink period in milliseconds */
    uint32_t next_toggle_time;      /**< Next toggle timestamp */
    bool is_blinking;               /**< Blink enable flag */
    bool current_state;             /**< Current LED state */
    bool initialized;               /**< Initialization flag */
} led_driver_t;

/**
 * @brief Initialize the LED driver.
 * @param led Pointer to LED driver instance.
 * @param gpio_port GPIO port (e.g., GPIOA).
 * @param gpio_pin GPIO pin (e.g., GPIO_PIN_5).
 * @return true if successful, false otherwise.
 */
bool led_driver_init(led_driver_t *led, GPIO_TypeDef *gpio_port, uint16_t gpio_pin);

/**
 * @brief Turn LED on.
 * @param led Pointer to LED driver instance.
 */
void led_driver_turn_on(led_driver_t *led);

/**
 * @brief Turn LED off.
 * @param led Pointer to LED driver instance.
 */
void led_driver_turn_off(led_driver_t *led);

/**
 * @brief Toggle LED state.
 * @param led Pointer to LED driver instance.
 */
void led_driver_toggle(led_driver_t *led);

/**
 * @brief Start LED blinking.
 * @param led Pointer to LED driver instance.
 * @param period_ms Blink period in milliseconds.
 */
void led_driver_blink(led_driver_t *led, uint32_t period_ms);

/**
 * @brief LED driver task - call periodically to handle blinking.
 * @param led Pointer to LED driver instance.
 */
void led_driver_task(led_driver_t *led);

/**
 * @brief Get current LED state.
 * @param led Pointer to LED driver instance.
 * @return true if LED is on, false if off.
 */
bool led_driver_get_state(const led_driver_t *led);

/**
 * @brief Check if LED is currently blinking.
 * @param led Pointer to LED driver instance.
 * @return true if blinking, false otherwise.
 */
bool led_driver_is_blinking(const led_driver_t *led);

/**
 * @brief Get current blink period.
 * @param led Pointer to LED driver instance.
 * @return Blink period in milliseconds.
 */
uint32_t led_driver_get_blink_period(const led_driver_t *led);

#endif /* __LED_DRIVER_INC_ */
