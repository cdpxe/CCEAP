/*
 * CCEAP: An Implementation of an Educational Protocol for Covert Channel
 *        Analysis Purposes
 *
 * The goal of this tool is to provide a simple, accessible implementation
 * for students. The tool demonstrates several network covert channel
 * vulnerabilities in a single communication protocol.
 *
 * Copyright (C) 2016-2019 Steffen Wendzel, steffen (at) wendzel (dot) de
 *                    http://www.wendzel.de
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Cf. `LICENSE' file.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <netinet/tcp.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#define CCEAP_VER	"0.6.1+"
#define CCEAP_AUTHOR	"Steffen Wendzel, www.wendzel.de"
#define ERR_EXIT	2
#define OK_EXIT		0

#define USAGE_CLIENT	1
#define USAGE_SERVER	2

typedef struct {
	u_int8_t opt_identifier;		/* freely selectable ID of option */
	u_int8_t opt_type;			/* type of the optional header */
	u_int8_t opt_value;			/* value of the optional header */
	u_int8_t dummy;				/* dummy value to end up with 32 bytes */
} options_t;
#define MAX_NUM_OPTIONS 32
#define DEFAULT_IAT_VAL	1000000			/* 1sec */

typedef struct {
	#define MAX_NUM_PREDEF_SEQNOS 16384
	#define MAX_NUM_PREDEF_IATS 16384
	#define MAX_NUM_PACKETS 16384
	u_int8_t sequence_number;		/* random value to identify a connection (incremented) */
	u_int8_t number_of_options;		/* num. of optional header elements */
	u_int8_t destination_length;		/* length of destination value */
	u_int8_t dummy;				/* dummy value to end up with 32 bytes */
	#define DSTPADSIZ 8
	char destination_and_padding[DSTPADSIZ];/* variable length ASCII dest. ID, followed by zero padding so that
						 * the header ends up in a whole 32-bit word */
} cceap_header_t;

void *calloc_exit(int size, int elem);
void usage(int type);
void print_gpl(void);

