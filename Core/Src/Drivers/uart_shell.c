#include "uart_shell.h"
#include "ring_buffer.h"

typedef struct uart_shell_ {
	USART_TypeDef *interface;

	ring_buffer_t ring_buffer_rx;
	ring_buffer_t ring_buffer_tx;

	uint8_t tx_buffer[UART_SHELL_MAX_TX_BUFFER];
	uint8_t rx_buffer[UART_SHELL_MAX_RX_BUFFER];
	uint8_t rx_byte;

	uart_shell_rx_callback_t rx_callback;

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

void uart_shell_init(UART_HandleTypeDef *huart, uart_shell_rx_callback_t rx_callback) {

	uart_shell.rx_callback = rx_callback;

	ring_buffer_init(&uart_shell.ring_buffer_rx, uart_shell.rx_buffer, UART_SHELL_MAX_RX_BUFFER);
	ring_buffer_init(&uart_shell.ring_buffer_tx, uart_shell.tx_buffer, UART_SHELL_MAX_TX_BUFFER);

}
