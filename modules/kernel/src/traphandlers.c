
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

#include <stdint.h>
#include <traphandlers.h>

state_t *tlb_area = (state_t *)TLB_OLDAREA;
state_t *pgm_area = (state_t *)PGMTRAP_OLDAREA;

// gestore system calls
void sysHandler() {
  state_t *state = (state_t *)SYSBK_OLDAREA;
  currentPcb->kernel_start = getTODLO();
  currentPcb->p_s.pc += 4;
  if (state->a1 > 0 && state->a1 < 11) { // controllo se sys call e' <= 10
                                         // altrimenti va passata a sopra
                                         // (syshandler)
    // controllo se processo e' eseguito in system o in user mode (pag 11 manual
    // uarm)
    if ((getSTATUS() & 0x0000001F) ==
        0x0000001F) { // gestione processo in system kernel mode
      memcpy(&currentPcb->p_s, state, sizeof(state_t));
      // selezione funzione relativa sys call ricevuta
      switch (state->a1) {
      case 1:
        currentPcb->p_s.a1 = createprocess((state_t *)state->a2, (int)state->a3,
                                           (void **)state->a4);
        break;
      case 2:
        currentPcb->p_s.a1 = terminateprocess((pcb_t *)state->a2);
        break;
      case 3:
        semp((uintptr_t *)state->a2);
        break;
      case 4:
        semv((int *)state->a2);
        break;
      case 5:
        currentPcb->p_s.a1 = spechdl(state->a2, state->a3, state->a4);
        break;
      case 6:
        gettime(state->a2, state->a3, state->a4);
        break;
      case 7:
        waitclock();
        break;
      case 8:
        iodevop(state->a2, state->a3);
        break;
      case 9:
        getpids((void **)state->a2, (void **)state->a3);
        break;
      case 10:
        waitchild();
        break;
      }
      if (currentPcb)
        currentPcb->kernel_time +=
            (cpu_t)(getTODLO() - currentPcb->kernel_start);
      scheduler();
    } else if ((getSTATUS() & 0x00000010) == 0x00000010) { // gestione processo
                                                           // che fa sys call in
                                                           // user mode trap di
      // tipo instruction aeserved
      memcpy(pgm_area, state, sizeof(state_t));
      pgm_area->CP15_Cause =
          0x00000014; // istruzione riservata (pag 20 manual uarm)
      currentPcb->kernel_time += (cpu_t)(getTODLO - currentPcb->kernel_start);
      pgmHandler();
    }
  } else {
    // se presente va al livello superiore
    // passare a sys handler se settato
    if (!currentPcb->sys_new) { // non e' settato l'handler
      terminateprocess(NULL);
      scheduler();
    } else {
      memcpy(currentPcb->sys_old, state, sizeof(state_t));
      currentPcb->kernel_time += (cpu_t)(getTODLO - currentPcb->kernel_start);
      currentPcb->sys_new->a1 = state->CP15_Cause;
      LDST(currentPcb->sys_new);
    }
  }
}

void tlbHandler() { // gestore trap memoria tlb
  currentPcb->kernel_start = getTODLO();
  if (!currentPcb->tlb_new) { // non e' settato l'handler
    terminateprocess(NULL);
    scheduler();
  } else {
    memcpy(currentPcb->tlb_old, tlb_area, sizeof(state_t));
    currentPcb->kernel_time += (cpu_t)(getTODLO - currentPcb->kernel_start);
    currentPcb->tlb_new->a1 = tlb_area->CP15_Cause;
    LDST(currentPcb->tlb_new);
  }
}

void pgmHandler() { // gestore trap pgm
  currentPcb->kernel_start = getTODLO();
  if (!currentPcb->pgm_new) { // non e' settato l'handler
    terminateprocess(NULL);
    scheduler();
  } else {
    memcpy(currentPcb->pgm_old, pgm_area, sizeof(state_t));
    currentPcb->kernel_time += (cpu_t)(getTODLO - currentPcb->kernel_start);
    // se arriva da syscall in user avremo 0x00000014 (instr reserved)
    currentPcb->pgm_new->a1 = pgm_area->CP15_Cause;
    LDST(currentPcb->pgm_new);
  }
}
