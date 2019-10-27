
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

#include <scheduler.h>

/*interval timer

zona di memoria 0x0000.02E4 	(variabile orologio_del_processo)

da 0xFFFF.FFFF a 0x0000.0000 	quando arriva a 0x0000.0000 e passa a
0xFFFF.FFFF crea un interupt (line2)
*/

// funzione passata a forallProcQ per update di priority
void updatePriority(pcb_t *pcb, void *arg) {
  if (pcb->p_priority < MAXPRIO)
    pcb->p_priority += 1;
}

// funzione scheduler
void scheduler() {
  if (getTODLO() - lastpriorityupdate >
      UPDATETIMESLICE) { // se passati 10 ms necessito di update priorita'
    forallProcQ(readyQueue, updatePriority, NULL);
    lastpriorityupdate = getTODLO();
  }
  unsigned int mul_remaining = TIMESLICE;
  if (currentPcb) { // gestione processo attivo
    currentPcb->total_time += (cpu_t)(getTODLO() - currentPcb->start);
    currentPcb->user_time = currentPcb->total_time - currentPcb->kernel_time;
    if (TIMESLICE > (getTODLO() - currentPcb->start)) {
      mul_remaining = (cpu_t)(TIMESLICE - (getTODLO() - currentPcb->start));
    } else
      mul_remaining = 1;
    currentPcb->start = getTODLO();
  } else { // nessun processo attivo (da interrupt timer)
    // controllo situazione con coda vuota
    if (!headProcQ(readyQueue)) {
      if (activeProcesses == 0) // shutdown
        HALT();
      else {
        if (softblockProcesses >
            0) { // aspetto interrupt devices (after iodevop)
          setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
          WAIT();
        } else // dealock
          PANIC();
      }
    }
    currentPcb = removeProcQ(&readyQueue); // prendi processo da ready queue
    currentPcb->start = getTODLO();        // tempo locale
    currentPcb->p_priority =
        currentPcb->start_priority; // reimposto priority originale
    if (currentPcb->total_time == 0) {
      currentPcb->wall_start = getTODLO();
    }
    // gestire tempi
    mul_remaining = TIMESLICE;
  }
  if ((100 * 1000) - getTODLO() - tickPerWait <
      mul_remaining) { // gestisce sia processo attivo che nuovo processo
    // in questo caso il timer sara' < di timeslice perche' lo pseudoclock scade
    // prima
    setTIMER((100 * 1000) - getTODLO() - tickPerWait);
  } else
    setTIMER(mul_remaining); // nuovo timer da 3ms
  LDST(&(currentPcb->p_s));  // carico stato
}
