#/*
# * CCEAP: An Implementation of an Educational Protocol for Covert Channel
# *        Analysis Purposes
# *
# * The goal of this tool is to provide a simple, accessible implementation
# * for students. The tool demonstrates several network covert channel
# * vulnerabilities in a single communication protocol.
# *
# * Copyright (C) 2016 Steffen Wendzel, steffen (at) wendzel (dot) de
# *                    http://www.wendzel.de
# *
# * This program is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# *
# * Cf. `LICENSE' file.
# *
# */

DEFAULTPORT=2222
CC=gcc
HEADERS=main.h
CFLAGS=-Wall -W -Wshadow -c -g -ggdb
OBJECTS=client.o support.o server.o
BUILDFLAGS=-O
.SUFFIXES : .c .o

.c.o : $(HEADERS)
	$(CC) $(CFLAGS) $<

all : $(OBJECTS)
	$(CC) $(BUILDFLAGS) -o client client.o support.o
	$(CC) $(BUILDFLAGS) -o server server.o support.o

clean : 
	rm -vf client server a.out *~ *.core core $(OBJECTS)

runcli :
	./client -P $(DEFAULTPORT) -D 127.0.0.1 -d Asciii -i 123 -o 1,2,3/4,5,6/7,8,9/ -v

runser :
	./server -P $(DEFAULTPORT) -v

count :
	wc -l *.c


