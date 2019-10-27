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



#include "init.h"

void initializeVariables() { // Init nucleo
  pcb_t *firstPcb = NULL;
  // Setup aree di memoria
  // PgmTrap definito in trap_handlers.
  state_t *ptrapArea = (state_t *)PGMTRAP_NEWAREA;
  STST(ptrapArea); // stato in old
  ptrapArea->pc = (memaddr)pgmHandler;
  ptrapArea->sp = RAM_TOP; // ram topm
  ptrapArea->cpsr =
      0x0000001F | 0x00000080 |
      0x0000040; // sys mode | interrupt disabled | fast interrupt disabled

  // Syscall handler definito in trap_handlers.h
  state_t *syscallArea = (state_t *)SYSBK_NEWAREA;
  STST(syscallArea); // stato in old
  syscallArea->pc = (memaddr)sysHandler;
  syscallArea->sp = RAM_TOP; // ram top
  syscallArea->cpsr = 0x0000001F | 0x00000080 | 0x0000040;

  // Memory trap handler (TLB)  definito in trap_handlers.h
  state_t *tlbArea = (state_t *)TLB_NEWAREA;
  STST(tlbArea); // stato in old
  tlbArea->pc = (memaddr)tlbHandler;
  tlbArea->sp = RAM_TOP; // ram top
  tlbArea->cpsr =
      0x0000001F | 0x00000080 |
      0x0000040; // sys mode | interrupt disabled | fast interrupt disabled

  // Interrupt handler definito in interrupt.h
  state_t *interruptArea = (state_t *)INT_NEWAREA;
  STST(interruptArea); // stato in old
  interruptArea->pc = (memaddr)interruptHandler;
  interruptArea->sp = RAM_TOP; // ram top
  interruptArea->cpsr =
      0x0000001F | 0x00000080 |
      0x0000040; // sys mode | interrupt disabled | fast interrupt disabled

  // Init values per controllare i deadlock
  softblockProcesses = 0;
  activeProcesses = 0;

  tickPerWait = getTODLO();
  // uint timer_processo_3ms = 3 * 1000 * MHZPROC; // MHZPROC velocità in MHz
  // del processore l ho messo in const

  readyQueue = NULL;
  currentPcb = NULL;

  // Inizializzo strutture dati fase 1
  initPcbs();
  initASL();

  // Inizializzo semafori devices
  int i = 0;
  for (i = 0; i < 51; i++)
    semaphores[i] = 0;
  timerSem = 0; // semaforo per waitclock

  // Inizializzazione primo processo secondo specifiche slides
  firstPcb = allocPcb();
  firstPcb->p_priority = 0;
  firstPcb->p_s.sp = RAM_TOP - FRAME_SIZE;
  firstPcb->p_s.pc = (memaddr)test;
  firstPcb->p_s.CP15_Control = 0;  // pag 11 manuale, bit 1 = 0 -> vm disabled
  firstPcb->p_s.cpsr = 0x0000001F; // Kernel mode

  // Inserisco il processo nella ready queue
  insertProcQ(&readyQueue, firstPcb);
  activeProcesses++;

  // gestire orologio qua
  tickPerWait = getTODLO();

  // fine della fase di boot
  scheduler(); // lauch scheduler manager
}

int main() {
  initializeVariables();

  return 0;
}
