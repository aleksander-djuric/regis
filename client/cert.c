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

#include "regcli.h"
#include "cert.h"
#include "tools/public.h"

// openssl init
void cert_init()
{
	OpenSSL_add_all_algorithms();
}

// encode base64
char *base64_encode(const unsigned char *input, int length)
{
	BIO *bio, *b64;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, input, length);
	(void) BIO_flush(bio);

	BIO_get_mem_ptr(bio, &bptr);
	(void) BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	return bptr->data;
}

// decode base64
int base64_decode(unsigned char *input, int length)
{
	BIO *bio, *b64;
	int ret;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new_mem_buf(input, length);
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	ret = BIO_read(bio, input, length);
	BIO_free_all(bio);

	input[ret] = '\0';

	return ret;
}

// encrypt uid
char *rsa_encrypt(char *in, int in_len)
{
	BIO *pkey_bio;
	RSA *pkey;
	unsigned char buffer[MAX_RSA];
	int ret = 0;

	pkey_bio = BIO_new(BIO_s_mem());
	if (!pkey_bio) return NULL;

	BIO_write(pkey_bio, pub_key, sizeof(pub_key));
	pkey = d2i_RSA_PUBKEY_bio(pkey_bio, NULL);

	if (!pkey) return NULL;
	if (RSA_size(pkey) > MAX_RSA)
		return NULL;

	ret = RSA_public_encrypt(in_len, (unsigned char *) in, buffer, pkey,
		RSA_PKCS1_PADDING);

	RSA_free(pkey);
	BIO_free(pkey_bio);

	return base64_encode(buffer, ret);
}

// decrypt certificate
int aes_decrypt(unsigned char *in, int in_len, unsigned char **out,
	char *key, char *iv)
{
	EVP_CIPHER_CTX *ctx;
	int ret, len;

	in_len = base64_decode((unsigned char *) in, in_len);

	*out = (unsigned char *) malloc(in_len);
	if (*out == NULL)
		return -1;

	if (!(ctx = EVP_CIPHER_CTX_new()))
		return -1;

	if (!EVP_DecryptInit_ex
		(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *) key,
		(unsigned char *) iv)) {
		return -1;
	}

	EVP_CIPHER_CTX_set_padding(ctx, 0);

	if (!EVP_DecryptUpdate(ctx, (unsigned char *) *out, &len, in, in_len)) {
		return -1;
	}
	ret = len;

	if (!EVP_DecryptFinal_ex(ctx, (unsigned char *) *out + ret, &len)) {
		return -1;
	}
	ret += len;

	EVP_CIPHER_CTX_free(ctx);

	return ret;
}

int print_cert_info(unsigned char *cert_data)
{
	BIO *cert_bio;
	BIO *bio_out;
	X509 *cert;
	X509_NAME *name;
	char buffer[1024];
	int nid;

	cert_bio = BIO_new(BIO_s_mem());
	bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);

	if (!cert_bio || !bio_out)
		return -1;

	BIO_puts(cert_bio, (char *) cert_data);
	cert = PEM_read_bio_X509(cert_bio, NULL, 0, NULL);

	if (!cert) {
		BIO_free(cert_bio);
		BIO_free(bio_out);
		return -1;
	}

	name = X509_get_issuer_name(cert);

	nid = OBJ_txt2nid("O");
	X509_NAME_get_text_by_NID(name, nid, buffer, 1024);
	printf("Issuer Organization: %s\n", buffer);

	nid = OBJ_txt2nid("CN");
	X509_NAME_get_text_by_NID(name, nid, buffer, 1024);
	printf("Issuer Name: %s\n", buffer);

	nid = OBJ_txt2nid("emailAddress");
	X509_NAME_get_text_by_NID(name, nid, buffer, 1024);
	printf("Issuer E-mail: %s\n", buffer);

	BIO_printf(bio_out, "\n");

	name = X509_get_subject_name(cert);

	nid = OBJ_txt2nid("O");
	X509_NAME_get_text_by_NID(name, nid, buffer, 1024);
	printf("Subject Organization: %s\n", buffer);

	nid = OBJ_txt2nid("CN");
	X509_NAME_get_text_by_NID(name, nid, buffer, 1024);
	printf("Subject Name: %s\n", buffer);

	nid = OBJ_txt2nid("emailAddress");
	X509_NAME_get_text_by_NID(name, nid, buffer, 1024);
	printf("Subject E-mail: %s\n", buffer);

	BIO_free(cert_bio);
	BIO_free(bio_out);

	return 0;
}

int read_cert(char *filename, char **buffer)
{
	FILE *lic;
	int ret;

	lic = fopen(filename, "r");
	if (!lic) {
		*buffer = NULL;
		return 0;
	}

	fseek(lic, 0, SEEK_END);
	ret = ftell(lic);
	fseek(lic, 0, SEEK_SET);

	*buffer = (char *) malloc(ret + 1);
	if (!fread(*buffer, ret, 1, lic)) {
		fclose(lic);
		return 0;
	}

	*(*buffer + ret) = '\0';

	fclose(lic);
	return ret;
}

int verify_cert(unsigned char *cert_data)
{
	BIO *pkey_bio;
	BIO *cert_bio;
	X509 *cert;
	EVP_PKEY *pkey;
	ASN1_TIME *nb, *na;
	int ret = VERIFY_ERROR;

	pkey_bio = BIO_new(BIO_s_mem());
	cert_bio = BIO_new(BIO_s_mem());

	if (!pkey_bio || !cert_bio)
		return ret;

	BIO_puts(cert_bio, (char *) cert_data);
	BIO_write(pkey_bio, pub_key, sizeof(pub_key));

	pkey = d2i_PUBKEY_bio(pkey_bio, NULL);
	cert = PEM_read_bio_X509(cert_bio, NULL, 0, NULL);

	if (pkey && cert && X509_verify(cert, pkey) == 1) {
		nb = cert->cert_info->validity->notBefore;
		na = cert->cert_info->validity->notAfter;

		ret = LICENSE_VALID;
		if (!X509_cmp_current_time(nb))
			ret = LICENSE_NOT_VALID; // license is not yet valid
		if (X509_cmp_current_time(na) == -1)
			ret = LICENSE_EXPIRED; // license expired
	}

	EVP_PKEY_free(pkey);
	X509_free(cert);
	BIO_free(cert_bio);
	BIO_free(pkey_bio);

	return ret;
}

/* eof */
