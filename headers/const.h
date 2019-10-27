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
#ifndef CONST_H
#define CONST_H

#define MAXPROC 20
#define MAXSEMD 60
#define ASHDSIZE 16
#define UPDATETIMESLICE 10000
#define TIMESLICE 3000
#define MAXPRIO 4

#define SPECPGMT 2
#define SPECTLB 1
#define SPECSYSBP 0

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

#define CREATEPROCESS 1
#define TERMINATEPROCESS 2
#define SEMP 3
#define SEMV 4
#define SPECHDL 5
#define GETTIME 6
#define WAITCLOCK 7
#define IODEVOP 8
#define GETPIDS 9
#define WAITCHLD 10

#endif
