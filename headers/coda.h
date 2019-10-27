/*
 * coda.h
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

#ifndef CODA_H
#define CODA_H

#include "pcb.h"
#include "const.h"
#include "alberi.h"
//array di PCB con dimensione MAXPROC.
pcb_t pcbfree_table[MAXPROC];

//testa della lista pcbFree
pcb_t *pcbfree_h;

//Inizializza la pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
void initPcbs();

pcb_t process;
 /*Restituisce NULL se la
pcbFree è vuota. Altrimenti rimuove un
elemento dalla pcbFree, inizializza tutti i
campi (NULL/0) e restituisce l’elemento
rimosso.*/
pcb_t *allocPcb();

//Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree)
void freePcb(pcb_t *p);

//inserisce l’elemento puntato da p nella coda dei processi puntata da head tenendo conto della priorita`
void insertProcQ(pcb_t **head, pcb_t *p);

//Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO.
// Ritorna NULL se la coda non ha elementi.
pcb_t *headProcQ(pcb_t *head);

//rimuove il primo elemento dalla coda dei processi puntata da head. Ritorna NULL se la coda è vuota.
//Altrimenti ritorna il puntatore all’elemento rimosso dalla lista.
pcb_t *removeProcQ(pcb_t **head);

//Rimuove il PCB puntato da p dalla coda dei processi puntata da head.
pcb_t *outProcQ(pcb_t **head, pcb_t *p);

//richiama la funzione fun per ogni elemento della lista puntata da head.
void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg);

int countElements(pcb_t *head);
#endif
