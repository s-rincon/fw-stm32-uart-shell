/**
 * @file ring_buffer.c
 * @brief Generic byte ring buffer for embedded systems.
 *
 * Provides a circular buffer implementation with overwrite-on-full,
 * and utility functions for buffer management and status.
 *
 * @author Santiago Rincon
 * @date 2025
 */

#include "ring_buffer.h"

bool ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, size_t size) {
    if ((rb == NULL) || (buf == NULL) || (size == 0)) {
        return false;
    }
    rb->buffer = buf;
    rb->capacity = size;
    return ring_buffer_reset(rb);
}

bool ring_buffer_push(ring_buffer_t *rb, uint8_t data) {
    if (rb == NULL) {
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->capacity;

    if (rb->full) {
        rb->tail = (rb->tail + 1) % rb->capacity;
    }

    if (rb->head == rb->tail) {
        rb->full = true;
    }
    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *data) {
    if ((rb == NULL) || (data == NULL) || ring_buffer_is_empty(rb)) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->full = false;
    return true;
}

bool ring_buffer_is_empty(ring_buffer_t *rb) {
    if (rb == NULL) {
        return false;
    }
    return ((rb->full == false) && (rb->head == rb->tail));
}

bool ring_buffer_is_full(ring_buffer_t *rb) {
    if (rb == NULL) {
        return false;
    }
    return (rb->full);
}

bool ring_buffer_reset(ring_buffer_t *rb) {
    if (rb == NULL) {
        return false;
    }

    rb->head = 0;
    rb->tail = 0;
    rb->full = false;
    return true;
}

size_t ring_buffer_get_capacity(ring_buffer_t *rb) {
    if (rb == NULL) {
        return 0U;
    }

    return rb->capacity;
}

size_t ring_buffer_get_count(ring_buffer_t *rb) {
    if (rb == NULL) {
        return 0U;
    }

    size_t count = 0;
    if (rb->full) {
        count = rb->capacity;

    } else if (rb->head >= rb->tail) {
        count = rb->head - rb->tail;

    } else {
        count = rb->capacity + rb->head - rb->tail;
    }

    return count;
}
