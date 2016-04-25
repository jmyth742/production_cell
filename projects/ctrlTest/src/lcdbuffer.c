#include <lcdbuffer.h>

  static message_t buffer[BUFF_SIZE];
  static uint8_t start = 0;
  static uint8_t end = 0;
  static uint8_t error;
  
  void lcdBufferInit(){
    lcdMutex = OSSemCreate(1);
    lcdEmptySlot = OSSemCreate(BUFF_SIZE);
    lcdHasNext = OSSemCreate(0);
  }
  
  void lcdBufferPut(message_t * msg){
    OSSemPend(lcdEmptySlot, 0, &error); // Check for an empty slot
    OSSemPend(lcdMutex, 0, &error); // get mutex access to buffer
    
      buffer[start] = *msg;
      start = (start + 1) % BUFF_SIZE;
      
    error = OSSemPost(lcdMutex); // release buffer
    error = OSSemPost(lcdHasNext); //update hasNext
  }

  void lcdBufferGet(message_t *msg){
    OSSemPend(lcdHasNext, 0, &error);  // check the buffer has an item
    OSSemPend(lcdMutex, 0, &error); // get mutex access to buffer
    
      *msg = buffer[end];
      end = (end + 1) % BUFF_SIZE;
      
    error = OSSemPost(lcdMutex); // release buffer
    error = OSSemPost(lcdEmptySlot); // update number of empty slots
  }
