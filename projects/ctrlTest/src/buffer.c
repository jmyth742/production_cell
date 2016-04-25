#include <stdbool.h>
#include <stdint.h>
#include <buffer.h>

static message_t buffer[BUF_SIZE];
static uint8_t start = 0;
static uint8_t end = 0;

void putBuffer(message_t *msg) {
  buffer[start] = *msg;
  start = (start + 1) % BUF_SIZE;
}

void getBuffer(message_t *msg) {
  *msg = buffer[end];
  end = (end + 1) % BUF_SIZE;
}


