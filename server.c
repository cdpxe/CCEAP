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

#include "main.h"

#ifdef __MACH__
 #include <mach/clock.h>
 #include <mach/mach.h>
#endif

void print_time_diff(void)
{
	long ms;
	time_t s;
	struct timespec spec_now;
	static struct timespec spec_last;
	static int first_call = 1;
#ifdef __MACH__ /* code from Stackoverflow.com (Mac OS lacks clock_gettime()) */
	#warning "Including experimental code for MacOS. CCEAP runs best on Linux!"
	clock_serv_t cclock;
	mach_timespec_t mts;

	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	spec_now.tv_sec = mts.tv_sec;
	spec_now.tv_nsec = mts.tv_nsec;
#else
	clock_gettime(CLOCK_REALTIME, &spec_now);
#endif
	
	if (first_call) {
		printf("0.000\n");
		first_call = 0;
	} else {
		s  = spec_now.tv_sec - spec_last.tv_sec;
		ms = (spec_now.tv_nsec - spec_last.tv_nsec) / 1.0e6;	
		if (ms < 0) {
			ms = 1000 + ms;
			s -= 1;
		}
		printf("%"PRIdMAX".%03ld\n", (intmax_t)s, ms);
	}
	bcopy(&spec_now, &spec_last, sizeof(struct timespec));
}

void
inform_disconnected()
{
	fprintf(stderr, "The connection was closed by the foreign host.\n");
	fprintf(stderr, "Shutdown ..\n");
	exit(OK_EXIT);
}

int
main(int argc, char *argv[])
{
	int ch;
	int sockfd;
	int connfd;
	socklen_t salen = sizeof(struct sockaddr_in);
	int yup;
	struct sockaddr_in sa;
	cceap_header_t *hdr;
	int lst_port = 0;
	options_t *options_array;
	int verbose = 0;
	int quiet = 0;
	int max = 0;
	int ret = 0;
	int x;
	fd_set rset;
	char buf[1024];
	
	while ((ch = getopt(argc, argv, "vhP:q")) != -1) {
		switch (ch) {
		case 'v':
			verbose = 1;
			break;
		case 'P':
			/* TCP port to listen on */
			lst_port = atoi(optarg);
			break;
		case 'q':
			quiet = 1;
			break;
		case 'h':
		default:
			usage(USAGE_SERVER);
			/* NOTREACHED */
		}
	}

	if (!quiet)
		print_gpl();

	/* basic checks of provided parameters */
	if (lst_port >= 0xffff || lst_port < 1) {
		fprintf(stderr, "TCP listen port out of range or not specified.\n");
		exit(ERR_EXIT);
	}
	
	/* short welcome notice */
	if (quiet) {
		fprintf(stdout, "---\n");
	} else {
		fprintf(stdout, "CCEAP - Covert Channel Educational Analysis Protocol (Server)\n");
		fprintf(stdout, "   => version: " CCEAP_VER ", written by: " CCEAP_AUTHOR "\n");
	}
	
	/* summarize information if verbose mode is used */
	if (verbose) {
		printf("verbose output:\n");
		printf("* Using TCP port %d\n", lst_port);
	}
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err(ERR_EXIT, "socket()");

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yup, sizeof(yup)) != 0)
		err(ERR_EXIT, "setsockopt(..., SO_REUSEADDR, ...)");
	
	/* prepare and create the connection to the peer */
	bzero(&sa, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(lst_port);
	sa.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(sockfd, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0)
		err(ERR_EXIT, "bind()"); /* socket for client connection */
	
	if (listen(sockfd, 1) < 0)
		err(ERR_EXIT, "listen()");
	
	if ((connfd = accept(sockfd, (struct sockaddr *) &sa, &salen)) < 0) {
		err(ERR_EXIT, "accept()");
	}
	
	max = connfd;
	for (;;) {
		FD_ZERO(&rset);
		FD_SET(connfd, &rset);
		
		ret = select(max + 1, &rset, NULL, NULL, NULL);
		if (ret) {
			int size_header = sizeof(cceap_header_t);
		
			bzero(buf, sizeof(buf));

			if (FD_ISSET(connfd, &rset)) {
				if ((x = recv(connfd, buf, sizeof(buf) - 1, 0)) == -1)
					err(ERR_EXIT, "recv()");
				if (x == 0)
					inform_disconnected();
				
				if (x >= size_header) {
					char dst_n_pad[DSTPADSIZ + 1] = { '\0' };
					
					hdr = (cceap_header_t *) buf;
					
					printf("received data (%d bytes):\n", x);
					printf(" > time diff to prev pkt: ");
						print_time_diff();
					printf(" > sequence number:       %d\n", hdr->sequence_number);
					printf(" > destination length:    %d\n", hdr->destination_length);
					printf(" > dummy value:           %d\n", hdr->dummy);
					strncpy(dst_n_pad, hdr->destination_and_padding, DSTPADSIZ);
					printf(" > destination + padding: %s\n", dst_n_pad);
					printf(" > number of options:     %d\n", hdr->number_of_options);
					if (hdr->number_of_options >= 1) {
						int i;
						
						/* check whether too many options */
						if ((x - sizeof(cceap_header_t)) > (MAX_NUM_OPTIONS * sizeof(options_t))) {
							fprintf(stderr, "received too many options\n");
							exit(ERR_EXIT);
						}
						/* check whether number_of_options is correct */
						if (hdr->number_of_options * sizeof(options_t) > (x - sizeof(cceap_header_t))) {
							fprintf(stderr, "did not receive enough bytes for %d options\n",
								hdr->number_of_options);
							exit(ERR_EXIT);
						} else {
							if (verbose) {
								printf("    > packet structure overview with optional space:\n"
								       "\t\t\t  %ld bytes required for handling the indicated %d options\n"
								       "\t\t\t  %ld bytes attached for options data, of which\n"
								       "\t\t\t  %ld bytes of received trailing data will be ignored\n",
									hdr->number_of_options * sizeof(options_t),
									hdr->number_of_options,
									x - sizeof(cceap_header_t),
									x - sizeof(cceap_header_t) - (hdr->number_of_options * sizeof(options_t))
									);
							}
						}
						
						options_array = (options_t *) (buf + sizeof(cceap_header_t));
						printf("    > options overview:\n");
						for (i = 0; i <  hdr->number_of_options; i++) {
							printf("\t\t\t  #%d: (identifier: %hhu, type: %hhu, value: %hhu)\n",
								i + 1,
								(options_array + i)->opt_identifier,
								(options_array + i)->opt_type,
								(options_array + i)->opt_value);
						}
					}
				} else {
					printf("did not implement caching of segments but received only part of a "
						"buffer -- sry ...\n");
					exit(ERR_EXIT);
				}
				
			} else {
				printf("huh?!\n");
				exit(ERR_EXIT);
			}
		} else if (ret == 0) {
			/* do nothing */
		} else { /* ret = -1 */
			if (errno == EINTR)
				continue;
			else
				err(ERR_EXIT, "select");
		}
	}
	
	close(sockfd);
	return OK_EXIT;
}


