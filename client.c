/*
 * CCEAP: An Implementation of an Educational Protocol for Covert Channel
 *        Analysis Purposes
 *
 * The goal of this tool is to provide a simple, accessible implementation
 * for students. The tool demonstrates several network covert channel
 * vulnerabilities in a single communication protocol.
 *
 * Copyright (C) 2016-2019 Steffen Wendzel, steffen (at) wendzel (dot) de
 *                    https://www.wendzel.de
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

/* simply iterate through IAT array and use the next element to call usleep() */
void
wait_IAT_before_send(u_int32_t *IAT_array, int IAT_array_elements)
{
	static int next_element = 0;

	/*printf("sleeping %i [usec]\n", IAT_array[next_element]);*/
	usleep(IAT_array[next_element]);
	next_element++;
	if (next_element == IAT_array_elements)
		next_element = 0;
}

int
main(int argc, char *argv[])
{
	int ch;
	int sockfd;
	struct sockaddr_in sa;
	cceap_header_t hdr;
	cceap_header_t *pkt;
	int dst_port = 0;
	char *dst_addr = NULL;
	u_int8_t sequence_number = 0 /* default ISN is 0 */;
	u_int8_t exclude = 0, duplicate = 0; /* seq. no. to exclude/duplicate */
	u_int8_t dummy = 0;
	char destination[8];
	u_int8_t destination_length = 0;
	int ret_val;
	options_t options_array[MAX_NUM_OPTIONS];
	u_int8_t num_options = 0;
	int verbose = 0;
	int cnt;
	int num_of_pkts_to_send = 10;
	int sequence_number_set = 0, sequence_set = 0;
	u_int8_t sequence_number_array[MAX_NUM_PREDEF_SEQNOS] = { '\0' };
	int sequence_number_array_elements = 0;
	u_int32_t IAT_array[MAX_NUM_PREDEF_IATS] = { '\0' };
	int IAT_array_elements = 0;
	u_int32_t default_iat = DEFAULT_IAT_VAL;
	int IAT_set = 0;
	int arg_c_set = 0;
	int quiet = 0;
		
	while ((ch = getopt(argc, argv, "vhP:D:d:i:o:u:p:x:c:s:t:T:q")) != -1) {
		switch (ch) {
		case 'v':
			verbose = 1;
			break;
	/* options for TCP/IP */
		case 'P':
			/* TCP destination port */
			dst_port = atoi(optarg);
			break;
		case 'D':
			/* IP destination address */
			if (!(dst_addr = (char *) calloc_exit(strlen(optarg) + 1, sizeof(char))))
				err(1, "calloc");
			strncpy(dst_addr, optarg, strlen(optarg));
			break;
	/* options for CCEAP */
		case 'd':
			/* destination address for the header */
			if (strlen(optarg) > 8) {
				fprintf(stderr, "Destination address too long.\n");
				exit(ERR_EXIT);
			}
			strncpy(destination, optarg, strlen(optarg));
			destination_length = strlen(destination);
			break;
		case 'c':
			/* number of packets to send */
			if (atoi(optarg) >= 0 && atoi(optarg) < MAX_NUM_PACKETS) {
				num_of_pkts_to_send = atoi(optarg);
				arg_c_set = 1;
			} else {
				fprintf(stderr, "Number of packets to send is out of range (MAX=%i).\n",
					MAX_NUM_PACKETS);
				exit(ERR_EXIT);
			}
			break;
		case 'i':
			/* Initial Sequence Number */
			if (sequence_set) {
				fprintf(stderr, "-i cannot be combined with -s\n");
				exit(ERR_EXIT);
			}
			if (atoi(optarg) >= 0 && atoi(optarg) < 0xff) {
				sequence_number = atoi(optarg);
			} else {
				fprintf(stderr, "Initial sequence number out of range.\n");
				exit(ERR_EXIT);
			}
			sequence_number_set = 1;
			break;
		case 's':
			/* define sequence of sequence_numbers for Message Order Manipulation
			 * in the form: id_1,id_2,id_3,... */
			if (sequence_number_set) {
				fprintf(stderr, "-s cannot be combined with -i\n");
				exit(ERR_EXIT);
			}
			
			{
				/* sequence_number_array[MAX_NUM_PREDEF_.....];
				sequence_number_array_elements = 0; */
				char *token;
				char *ptr_optarg;
				char *saveptr;
				
				bzero(sequence_number_array, sizeof(sequence_number_array));
				
				for (ptr_optarg = optarg; ; ptr_optarg = NULL) {
					int digit;
					
					token = strtok_r(ptr_optarg, ",", &saveptr);
					if (token == NULL)
						break;
					
					digit = token[0];
					if (isdigit(digit) == 0) {
						fprintf(stderr, "in parameter -s: '%c' is not a digit\n",
							token[0]);
						exit(ERR_EXIT);
					}
					
					sequence_number_array[sequence_number_array_elements] = atoi(token);
					sequence_number_array_elements++;
					if (sequence_number_array_elements > MAX_NUM_PREDEF_SEQNOS) {
						fprintf(stderr, "Too many sequence numbers "
							"(%i but max is %i) used in -s\n",
							sequence_number_array_elements,
							MAX_NUM_PREDEF_SEQNOS);
						exit(ERR_EXIT);
					} /*else {
						fprintf(stderr, "sequence_number_array_elements=%d <= %d\n",
							sequence_number_array_elements, MAX_NUM_PREDEF_SEQNOS);
					}*/
				}
			}
			if (sequence_number_array_elements > 0) {
				sequence_set = 1;
				if (!arg_c_set) {
					num_of_pkts_to_send = sequence_number_array_elements;
				}
			}
			break;
		case 't':
			/* define IATs */
			if (IAT_set || default_iat != DEFAULT_IAT_VAL) {
				fprintf(stderr, "IAT values were already set by previous parameter(s).\n");
				exit(ERR_EXIT);
			}
			{
				char *token;
				char *ptr_optarg;
				char *saveptr;
				
				bzero(IAT_array, sizeof(IAT_array));
				
				for (ptr_optarg = optarg; ; ptr_optarg = NULL) {
					int digit;
					
					token = strtok_r(ptr_optarg, ",", &saveptr);
					if (token == NULL)
						break;
					
					digit = token[0];
					if (isdigit(digit) == 0) {
						fprintf(stderr, "in parameter -t: '%c' is not a digit\n",
							token[0]);
						exit(ERR_EXIT);
					}
					
					IAT_array[IAT_array_elements] = atoi(token)*1000;
					IAT_array_elements++;
					if (IAT_array_elements > MAX_NUM_PREDEF_IATS) {
						fprintf(stderr, "Too many IAT values given in -t\n");
						exit(ERR_EXIT);
					} /*else {
						fprintf(stderr, "IAT_array_elements=%d <= %d\n",
							IAT_array_elements, MAX_NUM_PREDEF_IATS);
					}*/
				}
			}
			if (IAT_array_elements > 0) {
				IAT_set = 1;
				if (!arg_c_set) {
					num_of_pkts_to_send = IAT_array_elements;
				}
			}
			break;
		case 'T':
			if (IAT_set || default_iat != DEFAULT_IAT_VAL) {
				fprintf(stderr, "IAT values were already set by previous parameter(s).\n");
				exit(ERR_EXIT);
			}
			
			if (atoi(optarg) >= 0 && atoi(optarg) <= 10000) {
				default_iat = atoi(optarg)*1000;
			} else {
				fprintf(stderr, "Value for -T must be within 1 and 10000.\n");
				exit(ERR_EXIT);
			}
			break;
		case 'p':
			/* duplicate packet with sequence_number provided via optarg */
			if (atoi(optarg) >= 0 && atoi(optarg) < 0xff) {
				duplicate = atoi(optarg);
			} else {
				fprintf(stderr, "Duplicate sequence_number out of range.\n");
				exit(ERR_EXIT);
			}
			break;
		case 'x':
			/* drop packet (do not send it) with provided sequence_number */
			if (atoi(optarg) >= 0 && atoi(optarg) < 0xff) {
				exclude = atoi(optarg);
			} else {
				fprintf(stderr, "Exclusion sequence_number out of range.\n");
				exit(ERR_EXIT);
			}
			break;
		case 'u':
			/* unusued `dummy' field in main header */
			if (atoi(optarg) >= 0 && atoi(optarg) < 0xff) {
				dummy = atoi(optarg);
			} else {
				fprintf(stderr, "Dummy field value out of range.\n");
				exit(ERR_EXIT);
			}
			break;
		case 'o':
			/* optional header components in the form
			 * id_1,ty_1,val_1:id_2,ty_2,val_2:id_3,ty_3,val_3,...
			 */
			{
				char *token_1, *token_2;
				char *ptr_optarg_1, *ptr_optarg_2;
				char *saveptr_1, *saveptr_2;
				
				bzero(options_array, sizeof(options_array));
				
				for (ptr_optarg_1 = optarg; ; ptr_optarg_1 = NULL) {
					int num_elements = 0;
					
					token_1 = strtok_r(ptr_optarg_1, "/", &saveptr_1);
					if (token_1 == NULL)
						break;
					//printf("token_1: %s\n", token_1);
					
					for (num_elements = 0, ptr_optarg_2 = token_1; ; ptr_optarg_2 = NULL) {
						int digit;
						
						if (num_elements == MAX_NUM_OPTIONS) {
							fprintf(stderr, "too many options (max: %d)\n",
								MAX_NUM_OPTIONS);
							exit(ERR_EXIT);
						}
						
						token_2 = strtok_r(ptr_optarg_2, ",", &saveptr_2);
						if (token_2 == NULL)
							break;
						//printf("\ttoken_2: %s\n", token_2);
						digit = token_2[0];
						if (isdigit(digit) == 0) {
							fprintf(stderr, "in parameter -o: '%c' is not a digit\n",
								token_2[0]);
							exit(ERR_EXIT);
						}
						switch (num_elements) {
						case 0: /* option's identifier */
							options_array[num_options].opt_identifier = atoi(token_2);
							break;
						case 1: /* option's type */
							options_array[num_options].opt_type = atoi(token_2);
							break;
						case 2: /* option's value */
							options_array[num_options].opt_value = atoi(token_2);
							num_options++;
							break;
						default:
							fprintf(stderr, "more than three parameters not permitted"
									" per option for parameter -o\n");
							exit(ERR_EXIT);
							break;
						}
						num_elements++;
					}
					if (num_elements < 3) {
						fprintf(stderr, "three parameters are required per option for "
								"parameter -o.\n");
						exit(ERR_EXIT);
					}
				
				}
			}
			break;
		case 'q':
			quiet = 1;
			break;
		case 'h':
		default:
			usage(USAGE_CLIENT);
			/* NOTREACHED */
		}
	}
	
	if (!quiet)
		print_gpl();
	else {
		fprintf(stdout, "CCEAP - Covert Channel Educational Analysis Protocol (Server)\n");
		fprintf(stdout, "   => version: " CCEAP_VER ", written by: " CCEAP_AUTHOR "\n");
	}
	
	/* basic checks of provided parameters */
	if (dst_port >= 0xffff || dst_port < 1) {
		fprintf(stderr, "destination port out of range or not specified.\n");
		exit(ERR_EXIT);
	}
	
	if (dst_addr == NULL) {
		fprintf(stderr, "No destination address specified.\n");
		exit(ERR_EXIT);
	}
	
	/* if no ISN was specified and also no list of seq.no. was defined, then
	 * start with ISN=0 (incremental mode) */
	if (sequence_set == 0 /* no pre-defined sequence was provided */
	 && sequence_number_set == 0 /* no ISN was provided either */) {
	 	if (verbose) {
	 		printf("setting ISN to 1 and will use incremental sequence numbers.\n");
	 	}
		sequence_number = 1;
		sequence_number_set = 1; /* we pretent as if the user provided -i 1 */
	}

	/* summarize information if verbose mode is used */
	if (verbose) {
		/* short welcome notice */
		printf("CCEAP - Covert Channel Educational Analysis Protocol (Client)\n");
		printf("   => version: " CCEAP_VER ", written by: " CCEAP_AUTHOR "\n");
		putchar('\n');
		printf("* Sending %d packets\n"
		       "* Dummy value: %d\n"
		       "* Initial seq. no. to start with (relevant only if *not* in sequence mode): %d\n"
		       "* Seq. no. to duplicate / exclude: %d / %d\n",
		       	num_of_pkts_to_send, dummy, sequence_number,
		       	duplicate, exclude);
		if (sequence_set) {
			int q;
			
			printf("* Sequence of sequence_numbers to use: ");
			for (q = 0; q < num_of_pkts_to_send; q++) {
				printf("%d%c",
				 sequence_number_array[q % sequence_number_array_elements],
				 (((q + 1) % sequence_number_array_elements) ? ',' : '/')
				);
			}
			putchar('\n');
		}
		printf("* Determined %d options...", num_options);
		if (num_options > 0) {
			int i;
			putchar(' ');
			for (i = 0; i < num_options; i++) {
				printf("(%d,%d,%d),",
					options_array[i].opt_identifier,
					options_array[i].opt_type,
					options_array[i].opt_value);
			}
		}
		putchar('\n');
	}

	/* prepare and create the connection to the peer */
	bzero(&sa, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(dst_port);
	ret_val = inet_pton(AF_INET, dst_addr, &sa.sin_addr);
	if (!ret_val) {
		if (ret_val == 0) {
			fprintf(stderr, "Desination address not a valid character string.\n");
			exit(ERR_EXIT);
		} else {
			fprintf(stderr, "Invalid address family for inet_pton().\n");
			exit(ERR_EXIT);
		}
	}
	
	/* socket for client connection */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err(ERR_EXIT, "socket");
	
	printf("connecting ... ");
	if (connect(sockfd, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) != 0)
		err(ERR_EXIT, "connect");
	printf("connected.\n");
	
	/* create protocol header */
	bzero(&hdr, sizeof(cceap_header_t));
	if (sequence_number_set)
		hdr.sequence_number = sequence_number;
	else /* sequence pre-defined */
		hdr.sequence_number = sequence_number_array[0];
	
	/* place padding bytes ('X'es) */
	strncpy(hdr.destination_and_padding, "XXXXXXXX", 8);
	/* now override 1-8 bytes of the padding (so that padding remains, if necessasry) */
	if (destination_length > 0) {
		strncpy(hdr.destination_and_padding, destination, destination_length);
		hdr.destination_length = destination_length;
	}
	
	/* overwrite dummy field value if necessary */
	if (dummy != 0) {
		hdr.dummy = dummy;
	}
	
	/* add number of options to MAIN header */
	hdr.number_of_options = num_options;
	
	/* make one chunk of data out of HEADER + OPTIONS */
	pkt = (cceap_header_t *) calloc_exit(1, sizeof(cceap_header_t) + (sizeof(options_t) * num_options));
	memcpy(pkt, &hdr, sizeof(hdr));
	memcpy(pkt + 1, &options_array, sizeof(options_t)*num_options);
	
	/* send main header + options in one chunk to destination */
	printf("Sending: ");

	for (cnt = 0; cnt < num_of_pkts_to_send; cnt++) {
	
		if (exclude && pkt->sequence_number == exclude) {
			printf("(Excluded packet)"); fflush(stdout);
			if (IAT_set)
				wait_IAT_before_send(IAT_array, IAT_array_elements);
			else
				usleep(default_iat);
		} else {
			if (send(sockfd, pkt, sizeof(cceap_header_t) + (sizeof(options_t) * num_options), 0) < 0)
				err(ERR_EXIT, "send");
			putchar('.'); fflush(stdout);
			if (IAT_set)
				wait_IAT_before_send(IAT_array, IAT_array_elements);
			else
				usleep(default_iat);
		}
		
		if (duplicate && pkt->sequence_number == duplicate) {
			/* only duplicate if there is a packet left that we are allowed to send */
			if ((cnt+1) < num_of_pkts_to_send) {
				cnt++;
				if (send(sockfd, pkt, sizeof(cceap_header_t) + (sizeof(options_t) * num_options), 0) < 0)
					err(ERR_EXIT, "send");
				printf("(Duplicated packet)"); fflush(stdout);
				if (IAT_set)
					wait_IAT_before_send(IAT_array, IAT_array_elements);
				else
					usleep(default_iat);
			} else {
				printf("(Prevented packet duplication due to limit (%i packets)\n", num_of_pkts_to_send);
				fflush(stdout);
			}
		}
		
		if (sequence_number_set) {
			pkt->sequence_number++;
			/* reset sequence_number in case of overrun */
			if (pkt->sequence_number == 0xff) {
				pkt->sequence_number = 0;
			}
		} else { /* sequence mode */
			pkt->sequence_number = sequence_number_array[(cnt+1) % sequence_number_array_elements];
		}
	}
	printf("\nsending done.\n");
	
	close(sockfd);
	return OK_EXIT;
}


