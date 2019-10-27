/*
 * alberi.c
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

#include <alberi.h>
#include <stddef.h>
#include <stdio.h>


void insertLastSib(pcb_t *sib, pcb_t *p)
{
    //aggiunge p come ultimo fratello di sib
    if (!sib->p_sib){
        sib->p_sib = p;
    }
    else
    {
        insertLastSib(sib->p_sib, p);
    }
}

void insertChild(pcb_t *parent, pcb_t *p) // inserisce figlio p a parent
{
	  p->p_sib = NULL;
    if (!parent->p_first_child) // se parent non ha figli mette p come primo figlio
    {
        parent->p_first_child = p;
        p->p_parent = parent;
    }
    else
			{ // inserisce p come ultimo figlio usando insertLastSib
        p->p_parent = parent;
        insertLastSib(parent->p_first_child, p);
    }
}


pcb_t *removeChild(pcb_t *p) // rimuove  il figlio di p e lo ritorna
{
	pcb_t *tmp;
    if (!p->p_first_child) //guardo se ha figli se non ne ha ritorno null
    {
      return NULL;
    }
    else // se ne ha assegno il prossimo fratello al padre (se esiste)
    {
			  tmp = p->p_first_child;
        if(!p->p_first_child->p_sib) 
        {
            p->p_first_child = NULL;
        }
        else
        {
            p->p_first_child = p->p_first_child->p_sib;
        }
				tmp->p_parent = NULL;
				return tmp;
    }
}

void searchChild(pcb_t *sib, pcb_t *a)
{
    //cancella dai fratelli di sib a, e se non ha fratelli, mette come fratello di sib NULL, altrimenti mette il fratello di a
    if (sib->p_sib == a)
    {
        a->p_parent = NULL;
        if (sib->p_sib->p_sib == NULL)
        {
            sib->p_sib = NULL;
        }
        else
        {
            sib->p_sib = sib->p_sib->p_sib;
        }
        return;
    }
    else
    {
        searchChild(sib->p_sib, a);
    }
}


pcb_t *outChild(pcb_t* p) // rimuove il figlio p dalla lista dei figli del padre 
{
	if (p->p_parent == NULL) 
    {
        return NULL;
    }
    else
    {
			if (p->p_parent->p_first_child == p) // se è primo figlio lo sostituisce col fratello ( se non ha fratelli con null )
        {
					p->p_parent->p_first_child =  p->p_sib;
				 	p->p_parent = NULL;
				 	return p;
        }
			else{  // altrimenti usa search child per eliminare dalla lista dei fratelli p
        searchChild(p->p_parent->p_first_child, p);
				p->p_parent = NULL;
        return p;
			}
		}
}

