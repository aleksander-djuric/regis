/*
 * cert.h
 *
 * Description: Simple SSL based registration system (RegiS).
 *              SSL certificate validation functions.
 *
 * Copyright (c) 2017 Aleksander Djuric. All rights reserved.
 * Distributed under the GNU Lesser General Public License (LGPL).
 * The complete text of the license can be found in the LICENSE
 * file included in the distribution.
 *
 */

#ifndef _CERT_H
#define _CERT_H

#define MAX_RSA 2048

enum {
	VERIFY_ERROR = -1, // license verification failed
	LICENSE_VALID, // license is valid
	LICENSE_NOT_VALID, // license not valid
	LICENSE_EXPIRED, // license expired
};

void cert_init();
int read_cert(char *filename, char **buffer);
char *rsa_encrypt(char *in, int in_len);
int aes_decrypt(unsigned char *in, int in_len, unsigned char **out, char *key, char *iv);
int verify_cert(unsigned char *cert_data);
int print_cert_info(unsigned char *cert_data);

#endif /* CERT */
