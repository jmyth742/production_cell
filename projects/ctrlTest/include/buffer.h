#ifndef __BUFFER_H
#define __BUFFER_H

#define BUF_SIZE 6


typedef struct message {
  uint32_t id;
  uint32_t dataA;
  uint32_t dataB;
  uint32_t dataC;
} message_t;

void putBuffer(message_t *);
void getBuffer(message_t *);


#endif
