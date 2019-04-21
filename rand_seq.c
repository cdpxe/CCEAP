/* Very simple tool to generate randomized sequence numbers for testing
 * purposes.
 * License: GPLv3, see `LICENSE' file.
 * (C) 2019 Steffen Wendzel <wendzel (at) hs-worms (dot) de>
 *
 * This script can be used as a parameter for the CCEAP client.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

int two_array[2][2] = {
		{ 0, 1 },
		{ 1, 0 }
	};

int three_array[6][3] = {
		{ 0, 1, 2 },
		{ 0, 2, 1 },
		{ 1, 0, 2 },
		{ 1, 2, 0 },
		{ 2, 0, 1 },
		{ 2, 1, 0 }
	};

int four_array[24][4] = {
		{ 0, 1, 2, 3 },
		{ 0, 1, 3, 2 },
		{ 0, 2, 1, 3 },
		{ 0, 2, 3, 1 },
		{ 0, 3, 1, 2 },
		{ 0, 3, 2, 1 },
		
		{ 1, 0, 2, 3 },
		{ 1, 0, 3, 2 },
		{ 1, 2, 0, 3 },
		{ 1, 2, 3, 0 },
		{ 1, 3, 0, 2 },
		{ 1, 3, 2, 0 },

		{ 2, 0, 1, 3 },
		{ 2, 0, 3, 1 },
		{ 2, 1, 0, 3 },
		{ 2, 1, 3, 0 },
		{ 2, 3, 0, 1 },
		{ 2, 3, 1, 0 },

		{ 3, 0, 1, 2 },
		{ 3, 0, 2, 1 },
		{ 3, 1, 0, 2 },
		{ 3, 1, 2, 0 },
		{ 3, 2, 0, 1 },
		{ 3, 2, 1, 0 },
	};


#define PRINT_COMMA	putchar(',');

int main(int argc, char *argv[])
{
	int cur_seq = 0;
	int num_pairs_to_gen = 2; /* how many seq. pairs should be generated? */
	int max_seq_val = 256; /* A header field has a max. value it can contain,
			   * in CCEAP it is 8 bits for the sequence no. field. */
	int num_symbols = 2; /* A covert channel needs at least 2 symbols. */
	int first = 1;
	int counter = 0;
	extern char *__progname;

	if (argc <= 3) {
		fprintf(stderr, "usage: %s [#pairs to generate] [max_seq_val] [symbols (2||3||4)]\n", __progname);
		fprintf(stderr, "Please note that you can use the tool in combination with CCEAP\n"
				"the following way: ./client -D ... -P ... -s `./%s [parameters]`\n", __progname);
		exit(1);
	}
	num_pairs_to_gen = atoi(argv[1]);
	assert(num_pairs_to_gen > 1);
	max_seq_val = atoi(argv[2]);
	assert(max_seq_val > 1);
	num_symbols = atoi(argv[3]);
	assert(num_symbols >=2 && num_symbols <=4);
	
	srand(time(NULL));

	for (counter = 0; counter < num_pairs_to_gen; counter++) {
		if (first)
			first = 0;
		else
			PRINT_COMMA /* comma behind every new pair */

		int rand_val = rand();
		if (num_symbols == 2) {
			rand_val %= 2;
			printf("%i,%i",
				(cur_seq + two_array[rand_val][0]) % max_seq_val,
				(cur_seq + two_array[rand_val][1]) % max_seq_val);
			cur_seq += 2;
		} else if (num_symbols == 3) {
			rand_val %= 6;
			printf("%i,%i,%i",
				(cur_seq + three_array[rand_val][0]) % max_seq_val,
				(cur_seq + three_array[rand_val][1]) % max_seq_val,
				(cur_seq + three_array[rand_val][2]) % max_seq_val);
			cur_seq += 3;
		} else if (num_symbols == 4) {
			rand_val %= 24;
			printf("%i,%i,%i,%i",
				(cur_seq + four_array[rand_val][0]) % max_seq_val,
				(cur_seq + four_array[rand_val][1]) % max_seq_val,
				(cur_seq + four_array[rand_val][2]) % max_seq_val,
				(cur_seq + four_array[rand_val][3]) % max_seq_val);
			cur_seq += 4;
		} else {
			perror("internal error.\n"); exit(99);
		}
	}

	return 0;
}

