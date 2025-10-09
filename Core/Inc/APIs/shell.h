#ifndef __SHELL_INC_
#define __SHELL_INC_

#include <stdbool.h>

#include "main.h"
#include "uart_driver.h"

#define SHELL_MAX_LENGTH     128U

typedef struct shell_ {
    uart_driver_t driver;

} shell_t;

static inline uart_driver_t *shell_get_driver_instance(shell_t *shell) {
    return &shell->driver;
}

bool shell_init(shell_t *shell, UART_HandleTypeDef *huart);

void shell_printf(shell_t *shell, const char *format, ...);

size_t shell_send_bytes(shell_t *shell, uint8_t *data, size_t len);

void shell_task(shell_t *shell);



#endif /* __SHELL_INC_ */
