#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Ring buffer structure for byte storage.
 *
 * Use ring_buffer_init() to initialize before use.
 */
typedef struct ring_buffer_ {
    uint8_t *buffer;      /**< Pointer to buffer memory */
    uint16_t size;        /**< Size of buffer */
    uint16_t head;        /**< Write index */
    uint16_t tail;        /**< Read index */
    uint16_t count;       /**< Number of bytes in buffer */
} ring_buffer_t;

/**
 * @brief Initializes a ring buffer.
 *
 * @param rb Pointer to ring buffer structure.
 * @param buf Pointer to buffer memory.
 * @param size Size of buffer.
 * @return true if initialization is successful, false otherwise.
 */
bool ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, uint16_t size);

/**
 * @brief Pushes a byte into the ring buffer.
 *
 * If the buffer is full, the oldest data is overwritten.
 *
 * @param rb Pointer to ring buffer structure.
 * @param data Byte to push.
 * @return true if successful, false otherwise.
 */
bool ring_buffer_push(ring_buffer_t *rb, uint8_t data);

/**
 * @brief Pops a byte from the ring buffer.
 *
 * @param rb Pointer to ring buffer structure.
 * @param data Pointer to store popped byte.
 * @return true if successful, false if buffer is empty or arguments are invalid.
 */
bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *data);

/**
 * @brief Checks if the ring buffer is empty.
 *
 * @param rb Pointer to ring buffer structure.
 * @return true if empty, false otherwise.
 */
bool ring_buffer_is_empty(ring_buffer_t *rb);

/**
 * @brief Checks if the ring buffer is full.
 *
 * @param rb Pointer to ring buffer structure.
 * @return true if full, false otherwise.
 */
bool ring_buffer_is_full(ring_buffer_t *rb);

#endif // __RING_BUFFER_H__
