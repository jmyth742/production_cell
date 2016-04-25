#ifndef __CANBUFFER_H
#define __CANBUFFER_H
#include <stdint.h>
#include <ucos_ii.h>
#include <can.h>


#define BUFF_SIZE 128

 static OS_EVENT *mBufMutex; 
 static OS_EVENT *mBufEmptySlot; 
 static OS_EVENT *mBufHasNext;  

void canBufferInit(void);
void canBufferPut(canMessage_t *);
void canBufferGet(canMessage_t *);
#endif