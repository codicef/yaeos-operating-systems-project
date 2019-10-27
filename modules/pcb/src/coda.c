/*
 * coda.c
 *
 * Copyright 2018
 * Francesco Codice' <francesco.codice3@studio.unibo.it>
 * Daniele Ventura daniele.ventura5@studio.unibo.it>
 * Davide Lelli <davide.lelli5@studio.unibo.it>
 * Nicola:Gentilini <nicola.gentilini@studio.unibo.it>
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



#include <coda.h>
#include <stdlib.h>
#include <stdio.h>
#include <alberi.h>
#include <hash.h>

// pcb_t pcbfree_table[MAXPROC];


//inizializza campi pcb
void startPcb(pcb_t *p){
  (p)->p_s.a1 = 0;
  (p)->p_s.a2 = 0;
  (p)->p_s.a3 = 0;
  (p)->p_s.a4 = 0;
  (p)->p_s.v1 = 0;
  (p)->p_s.v2 = 0;
  (p)->p_s.v3 = 0;
  (p)->p_s.v4 = 0;
  (p)->p_s.v5 = 0;
  (p)->p_s.v6 = 0;
  (p)->p_s.sl = 0;
  (p)->p_s.fp = 0;
  (p)->p_s.ip = 0;
  (p)->p_s.sp = 0;
  (p)->p_s.lr = 0;
  (p)->p_s.pc = 0;
  (p)->p_s.cpsr = 0;
  (p)->p_s.CP15_Control = 0;
  (p)->p_s.CP15_EntryHi = 0;
  (p)->p_s.CP15_Cause = 0;
  (p)->p_s.TOD_Hi = 0;
  (p)->p_s.TOD_Low =0;
  (p)->p_next = NULL;
  (p)->p_parent = NULL;
  (p)->p_first_child = NULL;
  (p)->p_sib = NULL;
  (p)->p_priority = 0;
  *(p)->p_semKey = 0;
  (p)->tlb_new = NULL;
  (p)->tlb_old = NULL;
  (p)->pgm_new = NULL;
  (p)->pgm_old = NULL;
  (p)->sys_new = NULL;
  (p)->sys_old = NULL;
	p->total_time = 0;

}
//Inizializza la pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
void initPcbsRec(pcb_t **head, int index)
{
	(*head) = &pcbfree_table[index - 1];
	if(index >= MAXPROC){
		(*head)->p_next = NULL;
		return;
	}
	(*head)->p_next = &pcbfree_table[index];
	initPcbsRec(&(*head)->p_next, index + 1);
}

void initPcbs(){
	initPcbsRec(&pcbfree_h, 1);
}

//ritorna penultimo nodo
pcb_t *lastButOne(pcb_t *head){
  if(head){
    if(head->p_next)// se ci sono almeno 2 nodi
    {
			if(head->p_next->p_next == NULL)
				return head;
			return lastButOne(head->p_next);
    }
    //c`e solo head:
    return NULL;
  }
  return NULL;
}


/*NULL se la pcbFree è vuota. Altrimenti rimuove un el e inizializza tutti i
campi (NULL/0), restituisce l’elemento rimosso.*/
pcb_t *allocPcb(){
	pcb_t *tmp;
  if (!pcbfree_h){
    return NULL;
	}

  pcb_t *pen = lastButOne(pcbfree_h);//puntatore a penultimo pcb
	if (pen){
		tmp = pen->p_next;
		pen->p_next = NULL;//scollego ultimo elemento della lista
		startPcb(tmp);//inizializzo i campi dell`el. rimosso
	}
	else{//c'e` solo un elemento
		tmp = pcbfree_h;
		pcbfree_h = NULL;
    startPcb(tmp);
	}

	return tmp;

}

//return ultimo nodo lista
pcb_t *lastNode(pcb_t *head){
  if(head){
			if(!(head)->p_next)// se c'e` solo un elemento
      return head;
		else{
			return lastNode((head)->p_next);
		}
  }
	else{
		return NULL;
	}
}

//Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree)
void freePcb(pcb_t *p){
	if(!p)
		return;
	pcb_t *last;
	if(pcbfree_h){//se ci sono elementi nella lista
	  last = lastNode(pcbfree_h);
		(last)->p_next = p;
		p->p_next = NULL;
	}
	else{//p diventa unico elemento lista
		pcbfree_h = p;
	  p->p_next = NULL;
	 }
}

pcb_t *priorityPrevious(pcb_t *head, pcb_t *p){
//assumo che lista non sia vuota e che p non sia primo elemento
	if(head){
			if(head->p_next){
				if(head->p_next->p_priority < p->p_priority){
					return head;//puntatore a elemento a cui appendere p
				}
				else {
					return priorityPrevious(head->p_next, p);
        }
      }
			return head;// e` ultimo elemento
		}
		return NULL;
}

int countElementsR(pcb_t *head, int i){
	if(head)
		return countElementsR(head->p_next, i + 1);
	else
		return i;
}

int countElements(pcb_t *head){
	int i = 0;
	i = countElementsR(head, 0);
	return i;
}

//inserisce l’elemento puntato da p nella coda dei processi puntata da head tenendo conto della priorita`
void insertProcQ(pcb_t **head, pcb_t *p){
	if (!p){
		return;
	}
	if(!*head){//se lista vuota metto p
    *head = p;
     p->p_next = NULL;
     return;
    }
  //caso base se p > head, if sempre da saltare in chiamate ricorsive
  if(p->p_priority > (*head)->p_priority){
      p->p_next = *head;
      *head = p;
      return;
    }
    pcb_t* prox = priorityPrevious(*head, p); //elemento a cui appendere p
    if(prox){//se prox != NULL p ha qualche elemento dopo di se
      p->p_next = (prox)->p_next;
      (prox)->p_next = p;
      return;
      }
    else {//se prox == NULL, appendo in fondo alla lista
      prox = lastNode(*head);
      (prox)->p_next = p;
      p->p_next = NULL;
    }
    return;
  }

//Restituisce l’elemento di testa della coda dei processi da head,
// Ritorna NULL se la coda non ha elementi.
pcb_t *headProcQ(pcb_t *head){
  return head;
}


//rimuove il primo elemento dalla coda dei processi puntata da head. Ritorna NULL se la coda è vuota.
//Altrimenti ritorna il puntatore all’elemento rimosso dalla lista.
pcb_t *removeProcQ(pcb_t **head){
  pcb_t *tmp_head = *head;
  if(*head)
  {
    *head = (*head)->p_next;
  }
  return tmp_head;// funzione testata, cosi commentata, worka.
}

pcb_t **previous(pcb_t **head, pcb_t *p){
//assumo che lista non sia vuota e che p non sia primo elemento
	if (!head)
		return NULL;
	if((*head)->p_next){
    if((*head)->p_next == p){
      return head;
     }
		return previous(&(*head)->p_next, p);
  }
  return NULL;
}

//Rimuove il PCB puntato da p dalla coda dei processi puntata da head.
pcb_t *outProcQ(pcb_t **head, pcb_t *p){
  pcb_t **tmp_head = head;
    if(!*head)//lista vuota
      return NULL;
    if(*head == p){//p in testa
			*head = (*head)->p_next;
    return p;
  }
  pcb_t **prev = previous(tmp_head, p);//p in pos arbitraria
  if(prev != NULL){ // se p e` nella lista
    (*prev)->p_next = p->p_next; //rimosso p dalla lista
    return p;
  }
  return NULL;//se p non c`e
  }

//richiama la funzione fun per ogni elemento della lista puntata da head.
void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg){
  if(head){
    fun(head, arg);
    if(head->p_next == NULL)
      return;
    forallProcQ(head->p_next, fun, arg);
  }
}
