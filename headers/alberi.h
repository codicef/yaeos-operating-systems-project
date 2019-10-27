/*
 * alberi.h
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

#ifndef ALBERI_H
#define ALBERI_H
#include "pcb.h"
#include <stdlib.h>

void insertChild(pcb_t *parent, pcb_t *p);

pcb_t *removeChild(pcb_t *p);

pcb_t *outChild(pcb_t* p);
#endif
