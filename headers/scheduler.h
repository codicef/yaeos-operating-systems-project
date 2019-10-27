#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "init.h"
#include "const.h"

void scheduler();
int flag;

extern pcb_t* currentPcb;

#endif
