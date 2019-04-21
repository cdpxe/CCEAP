#/*
# * CCEAP: An Implementation of an Educational Protocol for Covert Channel
# *        Analysis Purposes
# *
# * The goal of this tool is to provide a simple, accessible implementation
# * for students. The tool demonstrates several network covert channel
# * vulnerabilities in a single communication protocol.
# *
# * Copyright (C) 2016-2017 Steffen Wendzel, steffen (at) wendzel (dot) de
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

DEFAULTPORT=9999
CC=gcc
HEADERS=main.h
#DEBUGFLAGS=-g -ggdb
CFLAGS=-Wall -W -Wextra -Wshadow -c $(DEBUGFLAGS)
OBJECTS=client.o support.o server.o iat_encode.o seq_encode.o rand_seq.o
BUILDFLAGS=-O -lrt
SERVERBUILDFLAGS=-lm
.SUFFIXES : .c .o

%.o : %.c  $(HEADERS)
	$(CC) $(CFLAGS) $<

all : $(OBJECTS)
	$(CC) $(BUILDFLAGS) -o client client.o support.o
	$(CC) $(BUILDFLAGS) $(SERVERBUILDFLAGS) -o server server.o support.o
	$(CC) $(BUILDFLAGS) -o iat_encode iat_encode.o
	$(CC) $(BUILDFLAGS) -o seq_encode seq_encode.o
	$(CC) $(BUILDFLAGS) -o rand_seq rand_seq.o

clean : 
	rm -vf client server iat_encode seq_encode rand_seq a.out *~ *.core core $(OBJECTS)

runcli :
	./client -P $(DEFAULTPORT) -D 127.0.0.1 -d Asciii -i 123 -o 1,2,3/4,5,6/7,8,9/ -v

runser :
	./server -P $(DEFAULTPORT) -v

runserloop :
	while [ 1 ]; do ./server -P $(DEFAULTPORT) -v; sleep 1; done

count :
	wc -l *.c

docs :
	(cd documentation; make all)

e :
	gedit *.c || pluma *.c


