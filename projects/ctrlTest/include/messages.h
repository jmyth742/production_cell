#ifndef MESSAGES_H
#define MESSAGES_H
#include <stdint.h>
//Might need to change these ID's at some point
#define CONVEYOR_ID 3
#define INPUT_ROBOT_ID 6
#define OUTPUT_ROBOT_ID 9
#define CONTROLLER_ID 12

#define ATTEMPTS = 3 //system fails after

//All CAN messages
typedef enum {
    START,
    ERROR,
    PAUSE,
    STOP,
    INPUT_READY,
    OUTPUT_READY,
    CONVEYOR_READY,
    INPUTPAD_LOADED,
    PICKUP_ATTEMPTED,
    PICKUP_OK,
    PICKUP_FAILED,
    REQ_PLACEBLOCK,
    ACK_PLACEBLOCK,
    REQ_REMOVEBLOCK,
    ACK_REMOVEBLOCK,
    REMOVE_BLOCK,
    REMOVE_ATTEMPTED,
    REMOVE_FAILED,
    REMOVE_OK
} messageType;


#endif // MESSAGES_H_INCLUDED
