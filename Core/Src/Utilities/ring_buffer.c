#include "ring_buffer.h"

bool ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, uint16_t size) {

    if ((!rb) || (!buf) || (size == 0)) {
        return false;
    }

    rb->buffer = buf;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;

    return true;
}

bool ring_buffer_push(ring_buffer_t *rb, uint8_t data) {
    if (!rb) {
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->size;

    if (rb->count < rb->size) {
        rb->count++;
    } else {
        // Buffer full: overwrite oldest, move tail forward
        rb->tail = (rb->tail + 1) % rb->size;
    }

    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *data) {
    if ((!rb) || (!data) || ring_buffer_is_empty(rb)) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count--;

    return true;
}

bool ring_buffer_is_empty(ring_buffer_t *rb) {
    return rb->count == 0;
}

bool ring_buffer_is_full(ring_buffer_t *rb) {
    return rb->count == rb->size;
}
