#infndef QUEUE_H
#define  QUEUE_H
#include <stdint.h>

typedef struct node {
    uint32_t val;
    struct node * next;
} node_t;

bool queueInit(void);
void queueAdd(uint32_t val);
#endif