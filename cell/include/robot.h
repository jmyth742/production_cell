#ifndef __ROBOT_H
#define __ROBOT_H

#include <stdint.h>

typedef enum robotJoint {ROBOT_HAND=1, ROBOT_WRIST=2, ROBOT_ELBOW=3, ROBOT_WAIST=4} robotJoint_t;
typedef enum robotJointStep_t {ROBOT_JOINT_POS_INC, ROBOT_JOINT_POS_DEC} robotJointStep_t;

enum {
  DUMMY_MIN = 45000,
  HAND_MIN = 45000,
  WRIST_MIN = 45000,
  ELBOW_MIN = 45000,
  WAIST_MIN = 45000,
  DUMMY_MAX = 100000,
  HAND_MAX  = 100000,
  WRIST_MAX = 100000,
  ELBOW_MAX = 100000,
  WAIST_MAX = 100000,
  DUMMY_NEUTRAL = 72500,
  HAND_NEUTRAL = 68750,
  WRIST_NEUTRAL = 82250,
  ELBOW_NEUTRAL = 87500,
  WAIST_NEUTRAL = 67250,
};

void robotInit(void);
void robotJointSetState(robotJoint_t, robotJointStep_t);
uint32_t robotJointGetState(robotJoint_t);
uint32_t robotJointGetMinPos(robotJoint_t);
uint32_t robotJointGetMaxPos(robotJoint_t);
uint32_t robotJointGetStepValue(void);

#endif
