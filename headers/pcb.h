
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
#ifndef PCB_H
#define PCB_H
#include <uARMtypes.h>

typedef struct pcb_t  {
  struct pcb_t *p_next;
  struct pcb_t *p_parent;
  struct pcb_t *p_first_child;
  struct pcb_t *p_sib;
  state_t p_s;
  int p_priority;
	int start_priority;
	int last_priority;
  int *p_semKey;
  state_t *tlb_new;
  state_t *tlb_old;
  state_t *pgm_new;
  state_t *pgm_old;
  state_t *sys_new;
  state_t *sys_old;
	unsigned int user_time;
	unsigned int kernel_time;
	unsigned int total_time;
	unsigned int start;
	unsigned int wall_start;
	unsigned int kernel_start;
} pcb_t;

#endif
