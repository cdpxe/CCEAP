/* A dirty and undocumented hack to encode data as sequence number values so that
 * CCEAP can use them signaling a file over a covert channel with the MESSAGE (PDU)
 * ORDER pattern. Incremental sequence numbers are used.
 * License: GPLv3, see `LICENSE' file.
 *
 * This script can be used as a parameter for the CCEAP client.
 *
 * Use the following form:
 * $ ./client -s `./seq_encode [input file] [max_value] 2`
 * e.g.:
 * $ ./client -t `./seq_encode message.txt 256 2`
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define PRINT_COMMA	putchar(',');
#define CHK_BIT(x)	if (x) { printf("%i,%i", (cur_seq + 1) % maxval, cur_seq % maxval); } \
			else { printf("%i,%i", cur_seq % maxval, (cur_seq + 1) % maxval); } \
			cur_seq += 2;

int main(int argc, char *argv[])
{
	FILE *fp;
	int cur_seq = 0;
	char byte[1];
	int maxval = 256; /* A header field has a max. value it can contain,
			   * in CCEAP it is 8 bits for the sequence no. field. */
	int num_symbols = 2; /* A covert channel needs at least 2 symbols. */
	int first = 1;
	extern char *__progname;

	if (argc <= 3) {
		fprintf(stderr, "usage: %s [input filename] [maxval] [symbols=2]\n", __progname);
		exit(1);
	}
	if ((fp = fopen(argv[1], "r")) == NULL) {
		perror("fopen");
		exit(1);
	}
	maxval = atoi(argv[2]);
	assert(maxval > 1);
	num_symbols = atoi(argv[3]);
	assert(num_symbols == 2);

	while (fread(byte, 1, 1, fp)) {
		if (first)
			first = 0;
		else
			PRINT_COMMA /* comma behind every new byte */
		
		if (num_symbols == 2) {
			CHK_BIT (byte[0] & 0x01) PRINT_COMMA
			CHK_BIT (byte[0] & 0x02) PRINT_COMMA
			CHK_BIT (byte[0] & 0x04) PRINT_COMMA
			CHK_BIT (byte[0] & 0x08) PRINT_COMMA
			CHK_BIT (byte[0] & 0x10) PRINT_COMMA
			CHK_BIT (byte[0] & 0x20) PRINT_COMMA
			CHK_BIT (byte[0] & 0x40) PRINT_COMMA
			CHK_BIT (byte[0] & 0x80)
		} else {
			perror("internal error.\n"); exit(99);
		}
	}
	fclose(fp);

	return 0;
}

