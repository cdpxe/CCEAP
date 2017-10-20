/* A dirty, undocumented hack to encode data as IAT values searated by commas.
 * This script can be used as a parameter for the CCEAP client.
 * Use the following form:
 * $ ./client -t `./iat_encode [input file] [low time value] [high time value]`
 * e.g.:
 * $ ./client -t `./iat_encode message.txt 10 20`
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PRINT_TIME_CODE	printf("%s", low); else printf("%s", high);
#define PRINT_COMMA	putchar(',');

int main(int argc, char *argv[])
{
	FILE *fp;
	char byte[1];
	char *low = NULL;
	char *high = NULL;
	int first = 1;

	if (argc <= 3) {
		fprintf(stderr, "usage: ./tool [filename] [low_time] [high_time]\n");
		exit(1);
	}
	if ((fp = fopen(argv[1], "r"))==NULL) {
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
		
		if (byte[0] & 0x01) PRINT_TIME_CODE PRINT_COMMA
		if (byte[0] & 0x02) PRINT_TIME_CODE PRINT_COMMA
		if (byte[0] & 0x04) PRINT_TIME_CODE PRINT_COMMA
		if (byte[0] & 0x08) PRINT_TIME_CODE PRINT_COMMA
		if (byte[0] & 0x10) PRINT_TIME_CODE PRINT_COMMA
		if (byte[0] & 0x20) PRINT_TIME_CODE PRINT_COMMA
		if (byte[0] & 0x40) PRINT_TIME_CODE PRINT_COMMA
		if (byte[0] & 0x80) PRINT_TIME_CODE
	}
	fclose(fp);

	return 0;
}

