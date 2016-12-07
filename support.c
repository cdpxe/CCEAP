/*
 * CCEAP: An Implementation of an Educational Protocol for Covert Channel
 *        Analysis Purposes
 *
 * The goal of this tool is to provide a simple, accessible implementation
 * for students. The tool demonstrates several network covert channel
 * vulnerabilities in a single communication protocol.
 *
 * Copyright (C) 2016 Steffen Wendzel, steffen (at) wendzel (dot) de
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

#include "main.h"

void
*calloc_exit(int size, int elem)
{
	void *q;
	
	if (!(q = calloc(size, elem))) {
		perror("calloc");
		fprintf(stderr, "calloc_exit was called, parameters: (%i, %i)\n", size, elem);
		exit(ERR_EXIT);
	}
	return q;
}

void
print_gpl(void)
{
	printf("CCEAP protocol implementation. Copyright (C) 2016 Steffen Wendzel.\n"
	       "This program comes with ABSOLUTELY NO WARRANTY; for details see "
	       "LICENSE file.\n"
	       "This is free software, and you are welcome to redistribute it "
	       "under certain conditions;\nfor details see LICENSE file.\n\n");
}

void
usage(int type)
{
	switch (type) {
	case USAGE_CLIENT:
		printf( "The following parameters are supported by CCEAP client v." CCEAP_VER ":\n"
			"\n"
			"--------------------------------------------------------------\n"
			"*** General parameters:\n"
			"-D x   Destination IP x to connect to\n"
			"-P x   TCP port x to connect to\n"
			"-h     Provide an overview of supported parameters (this output)\n"
			"-v     Activate verbose mode\n"
			"-t x   Use the inter-arrival times in 'x' between packets (x should\n"
			"       be given in the format 'Time_1,Time_2,...' (in usec/1000)\n"
			"\n"
			"*** Parameters specific for the CCEAP protocol:\n"
			"-c x   Number of packets to send (default: 10)\n"
			"-i x   Sequence number x to use for CCEAP\n"
			"-p x   Duplicate the packet with the sequence number x\n"
			"-x x   Exclude the packet with the sequence number x\n"
			"-s x   Use a pre defined sequence numbers given in x in the form:\n"
			"       'Seq_1,Seq_2,Seq_3', e.g. '1,2,3', where 1 is the ISN\n"
			"-d x   Use CCEAP destination address x\n"
			"-o x   Optional header elements specified via x.\n"
			"       Formatting: 'Option_1/Option_2/Option_3/...' where each\n"
			"       option is formatted as 'Identifier,Type,Value'.\n"
			"       Maximum of allowed options: %d\n"
			"       Example for a string with 3 options: '1,2,3/4,5,6/7,8,9'\n"
			"-u x   Use digit x instead of 0 as `dummy' value in the main header\n"
			"--------------------------------------------------------------\n",
			MAX_NUM_OPTIONS
		);
		break;
	case USAGE_SERVER:
		printf( "The following parameters are supported by the CCEAP server v." CCEAP_VER ":\n"
			"\n"
			"--------------------------------------------------------------\n"
			"*** General parameters:\n"
			"-h     Provide an overview of supported parameters (this output)\n"
			"-v     Activate verbose mode\n"
			"-P x   TCP port x to listen to\n"
			"-q     Suppress welcome messages; useful if server runs in a loop\n"
			"--------------------------------------------------------------\n"
		);
		break;
	default:
		fprintf(stderr, "Unsupported usage() parameter!\n");
		break;
	}
	exit(ERR_EXIT);
}

