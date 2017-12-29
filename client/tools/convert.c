/*
 * convert.c
 * Description: Command line tool for converting
 *              binary Server Public Key data into C array definition.
 *
 * Extraction of Public key from rootCA.crt:
 * openssl x509 -in rootCA.crt -pubkey -noout > public.key
 * cat public.key | openssl enc -base64 -d > public.der
 *
 * Copyright (c) 2017 Aleksander Djuric. All rights reserved.
 * Distributed under the GNU Lesser General Public License (LGPL).
 * The complete text of the license can be found in the COPYING
 * file included in the distribution.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_key_file(char *filename, __uint8_t **buffer) {
	FILE *lic;
	int ret;

	lic = fopen(filename, "r");
	if (!lic) return 0;

	fseek(lic, 0, SEEK_END);
	ret = ftell(lic);
	fseek(lic, 0, SEEK_SET);

	*buffer = (__uint8_t *) malloc(ret + 1);
	if (!fread(*buffer, ret, 1, lic)) {
		fclose(lic);
		return 0;
	}

	*(*buffer + ret) = '\0';

	fclose(lic);
	return ret;
}

int main(int argc, char **argv) {
	FILE *out;
	__uint8_t *in = NULL;
	int i, len;

	if (argc < 2) {
		printf("usage: %s public.der\n", argv[0]);
		return 0;
	}

	out = fopen("public.h", "w+");
	if (!out) return 0;

	len = read_key_file(argv[1], &in);
	if (!len) return 0;

	fprintf(out, "const __uint8_t pub_key[] = {\n\t");

	for (i = 1; i <= len; i++) {
		fprintf(out, "0x%02X", in[i-1]);
		if (len != i) {
			fprintf(out, ",");
			if (!(i % 16)) fprintf(out, "\n\t");
			else fprintf(out, " ");
		}
	}

	fprintf(out, " };\n");

	if (in) free(in);
	fclose(out);
	return 0;
}
