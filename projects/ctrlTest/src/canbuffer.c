#include <canbuffer.h>

  static canMessage_t buffer[BUFF_SIZE];
  static uint8_t start = 0;
  static uint8_t end = 0;
  static uint8_t error;
  
  void canBufferInit(){
    mBufMutex = OSSemCreate(1);
    mBufEmptySlot = OSSemCreate(BUFF_SIZE);
    mBufHasNext = OSSemCreate(0);
  }
  
  void canBufferPut(canMessage_t  msg){
    OSSemPend(mBufEmptySlot, 0, &error); // Check for an empty slot
    OSSemPend(mBufMutex, 0, &error); // get mutex access to buffer
    
      buffer[start] = msg;
      start = (start + 1) % BUFF_SIZE;
      
    error = OSSemPost(mBufMutex); // release buffer
    error = OSSemPost(mBufHasNext); //update hasNext
  }

  void canBufferGet(canMessage_t msg){
    OSSemPend(mBufHasNext, 0, &error);  // check the buffer has an item
    OSSemPend(mBufMutex, 0, &error); // get mutex access to buffer
    
      msg = buffer[end];
      end = (end + 1) % BUFF_SIZE;
      
    error = OSSemPost(mBufMutex); // release buffer
    error = OSSemPost(mBufEmptySlot); // update number of empty slots
  }
