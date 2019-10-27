/*
 * hash.h
 *
 * Copyright 2018
 * Francesco Codice' <francesco.codice3@studio.unibo.it>
 * Daniele Ventura daniele.ventura5@studio.unibo.it>
 * Davide Lelli <davide.lelli5@studio.unibo.it>
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

#ifndef HASH_H
#define HASH_H

#include "pcb.h"
#include "const.h"
#include <libuarm.h>
#include <stdlib.h>

typedef struct semd_t {
	struct semd_t *s_next;
	int *s_key;
	struct pcb_t *s_procQ;
} semd_t;

semd_t semd_table[MAXSEMD];
//array di SEMD con dimensione massima MAXSEMD.

semd_t *semdFree_h;
//Testa della lista dei SEMD liberi o inutilizzati.

semd_t *semdhash[ASHDSIZE];

int insertBlocked(int *key,pcb_t *p);
//guarda pdf Davoli

pcb_t *headBlocked(int *key);
//restituisce il pun tatore al pcb del primo
//processo bloccato sul semaforo, senza deaccordarlo. Se il
//semaforo non esiste restituisce NULL.

pcb_t* removeBlocked(int *key);
//guarda pdf Davoli

void forallBlocked(int *key, void fun(pcb_t *pcb, void *), void *arg);
//richiama la funzione fun per ogni processo bloccato sul semaforo identificato da key.


void outChildBlocked(pcb_t *p);
//Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato.

void  initASL();
//Inizializza la lista dei semdFree in modo da contenere tutti gli elementi della semdTable.
#endif
