#include "uart_shell.h"

typedef struct uart_shell_ {
	USART_TypeDef *uart;

	// Implement ring buffers

} uart_shell_t;

uart_shell_t uart_shell;

void uart_shell_reconfigure(uint32_t baud_rate) {
	// Reconfiguration code here
}

void uart_shell_send(const uint8_t *data, uint16_t length) {
	// Sending code here
}

void uart_shell_poll(void) {
	// Polling code here
}

void uart_shell_init(uart_shell_rx_callback_t rx_callback) {
	uart_shell.uart = UART_SHELL_INSTANCE;
}
