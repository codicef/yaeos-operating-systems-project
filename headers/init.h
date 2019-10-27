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
#ifndef INIT_H
#define INIT_H
#include <arch.h>
#include "const.h"
#include <libuarm.h>
#include <uARMtypes.h>
#include <uARMconst.h>
#include "pcb.h"
#include "coda.h"
#include "hash.h"
#include "traphandlers.h"
#include "interrupt.h"
#include <string.h>

// Contatori processi (e controllo deadlock)
unsigned int activeProcesses;
unsigned int softblockProcesses;

pcb_t *currentPcb; // processo in esecuzione
pcb_t *readyQueue; // lista processi ready

// Variabili tempo
unsigned int tickPerWait;
unsigned int lastpriorityupdate;
// Semaphores
int semaphores[MAXSEMD];
int timerSem;

void initializeVariables(); // inizializza il sistema
extern void test(); // da test phase2 (p2test.c)
extern void scheduler();
#endif
