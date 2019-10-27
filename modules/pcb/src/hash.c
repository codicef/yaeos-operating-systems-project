/*
 * hash.c
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


#include "hash.h"
#include "coda.h"
#include "alberi.h"
#include <stdint.h>

//semd_t semd_table[MAXSEMD];

/* UTILS FUNCTIONS */
semd_t* lastNodeSemd(semd_t *head){ // funzione ricorsiva che restituisce ultimo nodo lista semd
	if(head){
		if(!(head)->s_next)
			return head;
		else{
			return lastNodeSemd((head)->s_next);
		}
	}
	else
		return NULL;
}

int countElementsSemd(semd_t *head, int i){ // funzione ricorsiva che restituisce lunghezza lista semd
	if(!head)
		return i;
	else
		return countElementsSemd(head = head->s_next, i + 1);
}

int countSemd(int key, int rec){ // passa tutta la hashtable e conta elementi
	if (key == ASHDSIZE)
		return rec;
	if(semdhash[key])
		rec = rec + countElementsSemd(semdhash[key], 0);
	return countSemd(key + 1, rec);
}

semd_t *previousSemd(semd_t *head, int *key){ // return puntatore a elemento prima di head 
	if(!head)
		return NULL;
	if((head)->s_next){
    if((head)->s_next->s_key == key)// ho trovato nodo prima di nodo key
			{
				return head;
			}
		return previousSemd((head)->s_next, key);
	}
	else
		return NULL; // null se non c'è o è primo elemento (se non c'è search darà null quindi no ulteriori checks)
}

semd_t *lastButOneSemd(semd_t *head){ // ritorna penultimo puntatore  della lista
  if(head){
    if((head)->s_next)
			{
				if((head)->s_next->s_next == NULL)
					return head;
				return lastButOneSemd((head)->s_next);
			}   //c`e solo head:
		else
			return NULL;
  }
  return NULL;
}


/* CODICE HASH TABLE */
intptr_t hash(int* key){ // funzione di hash (add -> int)
	uintptr_t  hash_value;
	hash_value = (uintptr_t) key;
	hash_value = (hash_value ^ 61) ^ (hash_value >> 16); // credits to Thomas Wang for the hash fun
	hash_value = hash_value + (hash_value << 3);
	hash_value = hash_value ^ (hash_value >> 4);
	hash_value = hash_value * 0x27d4eb2d;
	hash_value = hash_value ^ (hash_value >> 15);
	hash_value = hash_value % ASHDSIZE;
	return hash_value;
}


void hashInsert(semd_t* sem){ // inserisce semaforo sem nella hashtable
	int hash_index = hash(sem->s_key);
	semd_t *tmp;
	if (semdhash[hash_index]){
		tmp = lastNodeSemd(semdhash[hash_index]);
		(tmp)->s_next = sem;
	}
	else{
		semdhash[hash_index] = sem;
	}
}


semd_t *is_in(semd_t *head, int *key){ // returna head se è presente sem con s_key == key, altrimenti null
	if(!head)
		return NULL;
	else if (head->s_key == key)
		return head;
	else return is_in(head->s_next, key);
}

semd_t *search(int *key)
{
	//cerca all'interno della lista se c'è key e restituisce il semd_t
	int hash_index = hash(key);
	semd_t *search_sem = semdhash[hash_index];
	return is_in(search_sem, key);

}


semd_t *delete(int *key){ // elimina dalla hashtable il semaforo con chiave *key
	int hash_index = hash(key);
	semd_t *sem = search(key);
	semd_t *prev;

	if(sem){
		prev = previousSemd(semdhash[hash_index], key);
		if(prev)
			(prev)->s_next = sem->s_next;
		else
			semdhash[hash_index] = sem->s_next;
	}
	else
		return NULL;
	return sem;
}

/* CODICE SEMDs */

void startSemd(semd_t **sem){ // inizializza un sem
	(*sem)->s_key = 0;
	(*sem)->s_procQ = NULL;
	(*sem)->s_next = NULL;
}


void initASLRec(semd_t **head, int index) // funzione ricorsiva per inserimento di semdtable in semdfree list
{
	(*head) = &semd_table[index - 1];
	if(index >= MAXSEMD){
		(*head)->s_next = NULL;
		return;
	}
	(*head)->s_next = &semd_table[index];
	initASLRec(&(*head)->s_next, index + 1);
}

void  initASL()
{
	initASLRec(&semdFree_h, 1);
}




semd_t *allocSemd(int* key){ // toglie elemento da semdfree list e lo restituisce
	semd_t *tmp;
  if (!semdFree_h){
    return NULL;
	}
  semd_t *pen = lastButOneSemd(semdFree_h);//penultimo pcb
	if (pen){// elimina ultimo elemento lista
		tmp = (pen)->s_next;
		(pen)->s_next = NULL;

	}
	else{// pen == null quindi elimino la testa
		tmp = semdFree_h;
		semdFree_h = NULL;
	}
	tmp->s_key = key;
	return tmp;
}



int insertBlocked(int *key, pcb_t *p) // utilizzando funzioni search, alloc e insert si inserisce  nel semaforo key della hashtable il processo p
{
	semd_t *sem;
	sem = search(key); //ottengo semaforo relativo a key
	p->p_next = NULL;
	// p->p_semKey = key;
	if(sem)
		insertProcQ(&sem->s_procQ, p);  // utilizzo fun di coda.c
	else{
		if (countSemd(0, 0) >= MAXSEMD) // se ci sono piu di maxsemd non inserisce piu
			return -1;
		sem = allocSemd(key);
		if(sem){
			hashInsert(sem);
			sem->s_procQ = p;
		}
		else
			return -1;
	}
	return 0;
}


pcb_t *headBlocked(int *key)	//guarda per ogni semdhash se trova key. se lo trova restituisce pcb_t puntato dal semaforo key
{
	semd_t *trovato = search(key);
	if (trovato)
		return trovato->s_procQ;
	return NULL;
}

void vaInCoda(semd_t **head, semd_t *sem) // mette sem in coda inicizzata da head (solitamente semdfree)
{
	if(!*head){
		(*head) = sem;
		return;
	}
  if ((*head)->s_next == NULL)
  {
    (*head)->s_next = sem;
    return ;
  }
  vaInCoda(&(*head)->s_next, sem);
  return ;
}



pcb_t *removeBlocked(int *key) // rimuove primo processo bloccato in sem 
{
	pcb_t* head = headBlocked(key);
	semd_t *sem = search(key);
	if (sem)
		{
			head = removeProcQ(&sem->s_procQ);

			if (!sem->s_procQ)
				{
					//se oltre a sem non c'è nulla nella lista,
					//rimuove il descrittore corrispondente dalla ASHT
					//e lo inserisce nella coda dei descrittori liberi (semdFree),
					//ritorna il pcb_t.
					sem = delete(key); // questa funzione gestisce l'eliminazione di un sem dalla hash table
					startSemd(&sem);
					vaInCoda(&semdFree_h, sem);
				}
			return head;
		}
	else{
		return NULL;
	}
}


void forallBlocked(int *key, void fun(pcb_t *pcb, void *), void *arg)// esegue fun su tutti i processi di key
{
	pcb_t *head = headBlocked(key);
	if(head) // se non null , si utilizza funzione di coda.c
		forallProcQ(head, fun, arg);
}

void forallHashIdx(void fun(pcb_t *pcb, semd_t **a_sem), pcb_t *p, int index){ // esegue una funzione (non del tutto generica ma rispetto ai problemi che si presentano si) su ogni semaforo testa di una cella dell'hashtable
	semd_t *head = &semd_table[index];
	if (index >= ASHDSIZE)
		return;

	if(head){
    fun(p, &head);
		forallHashIdx(fun, p, index + 1);
  }
}

void outChildRec(pcb_t *p, semd_t **sem){ // viene eliminato un elemento p da una lista di semafori sem
	pcb_t *tmp; 
	if (*sem){
		if((tmp = outProcQ(&(*sem)->s_procQ, p))){// se è non è null vuol dire che lo ha trovato e eliminato
			if (!(*sem)->s_procQ)
				{
					*sem = delete((*sem)->s_key); // funzione di eliminazione del sem dall'hashtable
					startSemd(&(*sem));
					vaInCoda(&semdFree_h, *sem);
				}
		}
		else
		  outChildRec(p, &(*sem)->s_next);
	}
	else
		return;
}

void outChildBlocked(pcb_t *p)// rimuove processo bloccato nella hashtable utilizzando le due funzioni ricorsive di supporto sopra definite
{
	forallHashIdx(outChildRec, p, 0);
}

