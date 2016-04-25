#include "queue.h"

bool queueInit(void){
  node_t *head = NULL;
  head = malloc(sizeof(node_t));
  if (head == NULL) {
      return false;
  }
  head->val = 1;
  head->next = NULL;
  return true;
}