/**
 * @file led_driver.c
 * @brief LED driver implementation for STM32 microcontrollers.
 *
 * Simple LED control with on/off/toggle/blink functions.
 */

#include "led_driver.h"
#include <string.h>

/**
 * @brief Set physical LED state.
 * @param led Pointer to LED driver instance.
 * @param state true for on, false for off.
 */
static void led_driver_write_gpio(led_driver_t *led, bool state) {
    if ((led == NULL) || (!led->initialized)) {
        return;
    }

    HAL_GPIO_WritePin(led->interface.port, led->interface.pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief Get current system tick in milliseconds.
 * @return Current tick in ms.
 */
static uint32_t led_driver_get_tick() {
    return HAL_GetTick();
}

bool led_driver_init(led_driver_t *led, GPIO_TypeDef *gpio_port, uint16_t gpio_pin) {
    if ((led == NULL) || (gpio_port == NULL)) {
        return false;
    }

    /* Clear the structure */
    (void)memset(led, 0, sizeof(led_driver_t));

    /* Initialize parameters */
    led->interface.port = gpio_port;
    led->interface.pin = gpio_pin;
    led->initialized = true;

    led_driver_turn_off(led);

    return true;
}

void led_driver_turn_on(led_driver_t *led) {
    if ((led == NULL) || (!led->initialized)) {
        return;
    }

    led->is_blinking = false;
    led->current_state = true;
    led_driver_write_gpio(led, true);
}

void led_driver_turn_off(led_driver_t *led) {
    if ((led == NULL) || (!led->initialized)) {
        return;
    }

    led->is_blinking = false;
    led->current_state = false;
    led_driver_write_gpio(led, false);
}

void led_driver_toggle(led_driver_t *led) {
    if ((led == NULL) || (!led->initialized)) {
        return;
    }

    led->current_state = !led->current_state;
    led_driver_write_gpio(led, led->current_state);
}

void led_driver_blink(led_driver_t *led, uint32_t period_ms) {
    if ((led == NULL) || (!led->initialized) || (period_ms == 0U)) {
        return;
    }

    led->blink_period_ms = period_ms;
    led->is_blinking = true;
    led->next_toggle_time = led_driver_get_tick() + led->blink_period_ms;
    led->current_state = true;
    led_driver_write_gpio(led, true);
}

void led_driver_task(led_driver_t *led) {
    if ((led == NULL) || (!led->initialized) || (!led->is_blinking)) {
        return;
    }

    if (led_driver_get_tick() >= led->next_toggle_time) {
        led_driver_toggle(led);
        led->next_toggle_time = led_driver_get_tick() + led->blink_period_ms;
    }
}
