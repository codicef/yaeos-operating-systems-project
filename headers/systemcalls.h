
/*
 *
 * Copyright 2018
 * Francesco Codice' <francesco.codice3@studio.unibo.it>
 * Nicola Gentilini <nicola.gentilini@studio.unibo.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#include<stdlib.h>
#include <uARMconst.h>
#include <uARMtypes.h>
#include "init.h"



extern pcb_t* currentPcb;

// system calls functions dalle slides

int createprocess (state_t *statep, int priority, void **cpid);

int terminateprocess(void *pid);

//P operation on semaphore *semaddr
void semp(int *semaddr);

//V operation on semaphore *semaddr
void semv(int *semaddr);

// Specifica trap handler
int spechdl(int type, state_t *old, state_t *new);

//Return (on param values) process times
void gettime(unsigned int *user, unsigned int *kernel, unsigned int *wallclock);

//Suspend process until next tick (100ms)
void waitclock();

//IO operation command copiato
int iodevop(unsigned int command, unsigned int *comm_device_register);

//Return pid of the process and parent's pid
void getpids(void **pid, void **ppid);

//Wait child to terminate
void waitchild();
int bbb;
