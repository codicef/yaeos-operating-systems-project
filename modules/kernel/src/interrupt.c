
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

#include <interrupt.h>

// gestore interrupt
void interruptHandler() {
  // save state
  state_t *old = INT_OLDAREA;
  unsigned int cause;
  if (currentPcb) {
    memcpy(&currentPcb->p_s, (state_t *)INT_OLDAREA, sizeof(state_t));
    currentPcb->p_s.pc -= 4; // torna a prima dell'interrupt
    cause = currentPcb->p_s.CP15_Cause;
  }
  {
    cause = old->CP15_Cause;
  } // controllo causa dell'interrupt e lo gestisco tenendo conto priorita (da 2
    // a 7)
  // CAUSE_IP_GET (definito in uarmConst) returna 1 se c'e' un pending interrupt
  // sulla linea passata come secondo parametro
  if (CAUSE_IP_GET(cause, 2)) {
    timerHandler();
  } // controllo su line 2 (BUS - TIMER)
  else {
    unsigned int i;
    for (i = 3; i <= 7; i++) {
      if (CAUSE_IP_GET(cause, i)) {
        deviceHandler(i); // passo all'handler device la linea (3-7)
        break;
      }
    }
  }
}

// gestore timer interrupt
void timerHandler() {
  if (getTODLO() - tickPerWait >=
      (100 * 1000)) { // sblocco i processi fermi su waitclock
    tickPerWait += (100 * 1000);
    while (headBlocked(&timerSem)) {
      semv(&timerSem);
    }
  }

  if (currentPcb) { // se processo attivo lo tolgo e lo reinserisco in coda
                    // ready
                    // update valori di tempo del processo
    currentPcb->total_time += (cpu_t)(getTODLO() - currentPcb->start);
    currentPcb->user_time = currentPcb->total_time - currentPcb->kernel_time;
    insertProcQ(&readyQueue, currentPcb); // metto in coda ready
    currentPcb = NULL;
  }
  setTIMER(TIMESLICE);
  scheduler();
}

// funzione che restituisce numero device da 0-7
int getDeviceN(int line) {
  unsigned int *bitmap =
      (memaddr *)(0x00006FE0 +
                  ((line - 3) * 4)); // ottengo indirizzo bitmap del device
  // pag 21 manual uarm melletti 3.1.6
  // scorro la bitmap puntata da *bitmap per trovare device attivo d[i] = 1
  unsigned int devbit = 0x00000001;
  int i;
  for (i = 0; i <= 7; i++) { // parto da zero e se trovo corrispondenza ritorno
                             // il numero di device
    if ((*bitmap & devbit) == devbit)
      return i;
    else
      devbit = devbit << 1;
  }
  return i;
}

// gestore interrupt causati da devices
void deviceHandler(int line) {
  int deviceNumber = getDeviceN(line);
  devreg_t *deviceAddress =
      (devreg_t *)(0x00000040 + ((line - 3) * 0x00000080) +
                   (deviceNumber * 0x00000010)); // ottengo indirizzo device
  // (manual uarm melletti pag 37)
  int deviceIdx = 8 * (line - 3) + deviceNumber; // da 0 a 40
  if (headBlocked(&semaphores[deviceIdx])) {
    if (line <= 6) { // gestione di tutti i devices tranne terminal
      pcb_t *p = headBlocked(&semaphores[deviceIdx]);
      /* struttura dati dtpreg_t
      typedef struct {
              unsigned int status;
              unsigned int command;
              unsigned int data0;
              unsigned int data1;
      } dtpreg_t;
      */
      p->p_s.a1 = deviceAddress->dtp.status; // val di ritorno
      deviceAddress->dtp.command = 1; // ack device  (pg manual uarm39 41 44)
    } else {                          // gestione terminal devices
      pcb_t *p = headBlocked(&semaphores[deviceIdx]);
      /* def struttura dati reg terminale
              typedef struct {
              unsigned int recv_status;
              unsigned int recv_command;
              unsigned int transm_status;
              unsigned int transm_command;
              } termreg_t;
       */
      // guardo in transm_status.TRANS'D-CHAR (bit 15) se e' in character
      // received (5) pag 91 tesi melletti
      if ((deviceAddress->term.transm_status & 0x0000000F) == 5) {
        p->p_s.a1 = deviceAddress->term.transm_status; // val di ritorno
        deviceAddress->term.transm_command = 1;        // ack device (pg 46)
      } else if ((deviceAddress->term.recv_status & 0x0000000F) == 5) {
        // guardo in recv_status.RECV'D-CHAR (bit 15) se e' in character
        // received (5) pag 91 tesi melletti
        p->p_s.a1 = deviceAddress->term.recv_status; // val di ritorno
        deviceAddress->term.recv_command = 1;        // ack device
      }
    }
    // semv gestira' anche i tempi del processo
    semv(&semaphores[deviceIdx]);
  } else {
    // PANIC();
  }
  scheduler();
}
