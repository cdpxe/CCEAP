/* A dirty and undocumented hack to encode data as IAT values searated by commas.
 * License: GPLv3, see `LICENSE' file.
 *
 * This script can be used as a parameter for the CCEAP client.
 *
 * Use the following form:
 * $ ./client -t `./iat_encode [input file] [low time value (ms)] [high time value (ms)]`
 * e.g.:
 * $ ./client -t `./iat_encode message.txt 10 20`
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PRINT_COMMA	putchar(',');
#define CHK_BIT(x)	if (x) { printf("%s", low); } else { printf("%s", high); }

int main(int argc, char *argv[])
{
	FILE *fp;
	char byte[1];
	char *low = NULL;
	char *high = NULL;
	int first = 1;
	extern char *__progname;

	if (argc <= 3) {
		fprintf(stderr, "usage: %s [input filename] [low_time (ms)] [high_time (ms)]\n", __progname);
		exit(1);
	}
	if ((fp = fopen(argv[1], "r")) == NULL) {
		perror("fopen");
		exit(1);
	}
	low = argv[2];
	high = argv[3];

	while (fread(byte, 1, 1, fp)) {
		if (first)
			first = 0;
		else
			PRINT_COMMA /* comma behind every new value */
		
		CHK_BIT (byte[0] & 0x01) PRINT_COMMA
		CHK_BIT (byte[0] & 0x02) PRINT_COMMA
		CHK_BIT (byte[0] & 0x04) PRINT_COMMA
		CHK_BIT (byte[0] & 0x08) PRINT_COMMA
		CHK_BIT (byte[0] & 0x10) PRINT_COMMA
		CHK_BIT (byte[0] & 0x20) PRINT_COMMA
		CHK_BIT (byte[0] & 0x40) PRINT_COMMA
		CHK_BIT (byte[0] & 0x80)
	}
	fclose(fp);

	return 0;
}

