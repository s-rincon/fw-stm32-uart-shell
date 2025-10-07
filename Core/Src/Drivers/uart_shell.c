#include "uart_shell.h"
#include "ring_buffer.h"

typedef struct uart_shell_ {
	UART_HandleTypeDef *huart;

	ring_buffer_t ring_buffer_rx;
	ring_buffer_t ring_buffer_tx;

	uint8_t tx_buffer[UART_SHELL_MAX_TX_BUFFER];
	uint8_t rx_buffer[UART_SHELL_MAX_RX_BUFFER];
	uint8_t rx_byte;

	uart_shell_rx_callback_t rx_callback;

} uart_shell_t;

uart_shell_t uart_shell;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance != uart_shell.huart->Instance) {
		return;
	}

	uint8_t data_char;
	if (ring_buffer_pop(&uart_shell.ring_buffer_tx, &data_char)) {
		HAL_UART_Transmit_IT(uart_shell.huart, &data_char, 1);
	}
}

void uart_shell_reconfigure(uint32_t baud_rate) {
	// Reconfiguration code here
}

void uart_shell_send(const uint8_t *data, uint16_t length) {
	if ((!data) || (length == 0)) {
		return;
	}

	for (uint16_t idx = 0; idx < length; idx++) {
		ring_buffer_push(&uart_shell.ring_buffer_tx, data[idx]);
	}

	if (uart_shell.huart->gState == HAL_UART_STATE_READY) {
		uint8_t data_char;
		if (ring_buffer_pop(&uart_shell.ring_buffer_tx, &data_char)) {
			HAL_UART_Transmit_IT(uart_shell.huart, &data_char, 1);

		}
	}
}

void uart_shell_poll(void) {
	// Polling code here
}

void uart_shell_init(UART_HandleTypeDef *huart, uart_shell_rx_callback_t rx_callback) {
	uart_shell.huart = huart;
	uart_shell.rx_callback = rx_callback;

	ring_buffer_init(&uart_shell.ring_buffer_rx, uart_shell.rx_buffer, UART_SHELL_MAX_RX_BUFFER);
	ring_buffer_init(&uart_shell.ring_buffer_tx, uart_shell.tx_buffer, UART_SHELL_MAX_TX_BUFFER);

	HAL_UART_Receive_IT(uart_shell.huart, &uart_shell.rx_byte, 1);

}
