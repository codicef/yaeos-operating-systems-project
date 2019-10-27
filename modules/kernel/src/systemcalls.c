
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

#include <systemcalls.h>

// funzione di appoggio per ottenere indice del device
int getDeviceNi(unsigned int base) {
  int i = 0;
  unsigned int address = 0x00000040;

  for (i = 0; i < 40; i++) {
    if (base < address) // trovato numero device
      return i - 1;
    address = address + 0x00000010;
  }
  return i;
}

// funzione di appoggio ricorsiva per eliminazione prole processo
int liberaTutti(pcb_t *p) {
  if (!p)
    PANIC();
  while (p->p_first_child) { // elimino tutti i figli
    liberaTutti(removeChild(p));
  }
  outProcQ(&readyQueue, p);
  activeProcesses -= 1;
  outChild(p); // cancello rapporto padre-figlio
  freePcb(p);
  return 0;
}

// system calls functions
// sys1 CREATEPROCESS
int createprocess(state_t *statep, int priority, void **cpid) {
  pcb_t *pcb;

  if (!(pcb = allocPcb()))
    return -1;
  pcb->p_priority = 0;
  if (currentPcb)
    insertChild(currentPcb, pcb);
  // start_priority utilizzato per reimpostare priority originale dopo gli
  // update aging
  pcb->p_priority = priority;
  pcb->start_priority = priority;
  memcpy(&pcb->p_s, statep, sizeof(state_t));
  insertProcQ(&readyQueue, pcb);
  activeProcesses += 1;
  if (cpid) // se non nullo restiuisco riferimento a pcb creato
    *cpid = pcb;
  return 0;
}

// sys2 TERMINATEPROCESS
int terminateprocess(void *pid) {
  pcb_t *p;
  if (!pid) { // se NULL metto currentPcb
    p = currentPcb;
    currentPcb = NULL;
  } else
    p = (pcb_t *)pid;

  if (p->p_parent) { // se padre bloccato su processo da interrompere faccio
                     // semv
    if (headBlocked(p->p_parent->p_semKey)) {
      semv(p->p_parent->p_semKey);
    }
  }
  liberaTutti(p);
  return 0;
}

// sys3 SEMP
void semp(int *semaddr) {
  if (*semaddr <= 0) { // block process
    // last _priority e' utilizzato per salvare la priorita' ed evitare che
    // venga considerata nelle code dei semafori
    currentPcb->last_priority = currentPcb->p_priority;
    currentPcb->p_priority = 0;
    // insertBlocked ha come parametri la key del semaforo e il processo,
    insertBlocked(semaddr, currentPcb);
    // if (!headBlocked(semaddr))
    // PANIC();
    if (semaddr == &timerSem ||
        (semaddr >= semaphores && semaddr <= semaphores + MAXSEMD))
      softblockProcesses++; // incremento counter processi fermi a semaforo
    // gestione tempi
    currentPcb->kernel_time += getTODLO() - currentPcb->kernel_start;
    currentPcb->total_time += getTODLO() - currentPcb->start;
    currentPcb = NULL;
  } else {
    (*semaddr)--;
  }
}

// sys4 SEMV
void semv(int *semaddr) {
  pcb_t *blockedPcb = removeBlocked(semaddr);
  if (blockedPcb) {
    // reimposto priorita' del processo prima di inserire in readyqueue
    // in questo modo il processo bloccato e' inserito in base alla priority
    blockedPcb->p_priority = blockedPcb->last_priority;
    insertProcQ(&readyQueue, blockedPcb);
    if (semaddr == &timerSem ||
        (semaddr >= semaphores && semaddr <= semaphores + MAXSEMD))
      softblockProcesses--;
  } else {
    (*semaddr)++;
  }
}

// sys5 SPECHDL
int spechdl(int type, state_t *old, state_t *new) {
  switch (type) {
  case 0: // sys call
    if (currentPcb->sys_new)
      return -1;
    else {
      currentPcb->sys_old = old;
      currentPcb->sys_new = new;
    }
    break;

  case 1: // tlb
    if (currentPcb->tlb_new)
      return -1;
    else {
      currentPcb->tlb_old = old;
      currentPcb->tlb_new = new;
    }
    break;

  case 2: // pgm
    if (currentPcb->pgm_new) {
      return -1;
    } else {
      currentPcb->pgm_old = old;
      currentPcb->pgm_new = new;
    }
    break;
  }

  return 0;
}

// sys6 GETTIME
void gettime(cpu_t *user, cpu_t *kernel, cpu_t *wallclock) {
  if (currentPcb) {
    // i valori restituiti sono aggiornati all'esatto istante in cui gettime
    // restituisce i valori
    *kernel = (getTODLO() - currentPcb->kernel_start) + currentPcb->kernel_time;

    *user = currentPcb->total_time + (getTODLO() - currentPcb->start) - *kernel;
    *wallclock = (cpu_t)(getTODLO() - currentPcb->wall_start);
  }
}

// sys7 WAITCLOCK
void waitclock() {
  // e' il semaforo utilizzato per il timer
  semp(&timerSem);
}

// sys8 IODEVOP
int iodevop(unsigned int command, unsigned int *comm_device_register) {
  int deviceIdx = getDeviceNi(comm_device_register); // prendi indice semaforo
  semp(&semaphores[deviceIdx]);
  *comm_device_register = command;
  // valore di ritorno verra' gestito dall'interrupt handler prima di sbloccare
  // il processo dal semaforo
  return 0;
}

// sys9 GETPIDS
void getpids(void **pid, void **ppid) {
  if (pid != NULL)
    *pid = currentPcb;
  if (ppid != NULL && currentPcb->p_parent) {
    *ppid = currentPcb->p_parent;
  }
}

// sys10 WAITCHILD
void waitchild() {
  if (!currentPcb->p_first_child) { // se figlio non presente non fa alcuna semp
    return;
  } else {
    semp(currentPcb->p_semKey);
  }
}
