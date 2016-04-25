#ifndef __LCDBUFFER_H
#define __LCDBUFFER_H
#include <stdint.h>
#include <ucos_ii.h>
#include <status.h>

#define BUFF_SIZE 6

 static OS_EVENT *lcdMutex; 
 static OS_EVENT *lcdEmptySlot; 
 static OS_EVENT *lcdHasNext;  
 
 typedef struct message {
  char *alarmStatus;
  uint32_t interval;
  uint32_t time;
  char *caseLockMsg;
  char *caseMoveMsg;
  char *code;
  char *cursor;
 } message_t;

void lcdBufferInit(void);
void lcdBufferPut(message_t *);
void lcdBufferGet(message_t *);
#endif