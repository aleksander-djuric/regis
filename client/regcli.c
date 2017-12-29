/*
 * regcli.c
 *
 * Description: Simple SSL based registration system (RegiS).
 *              Client application.
 *
 * Copyright (c) 2017 Aleksander Djuric. All rights reserved.
 * Distributed under the GNU Lesser General Public License (LGPL).
 * The complete text of the license can be found in the LICENSE
 * file included in the distribution.
 *
 */

#include "regcli.h"
#include "http.h"
#include "cert.h"
#include "config.h"

/* Static variables for config defaults, etc. */
static char *reg_server = NULL;
static char *serial = NULL;
static char *certificate = NULL;

#define MAXLINE 1024

#ifdef __GNUC__
#define getCPUId(func,ax,bx,cx,dx) \
	__asm__ __volatile__("cpuid": \
	"=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));
#else // GNUC
#define getCPUId(func,ax,bx,cx,dx) { \
	int cpuid[4]; \
	__cpuid(cpuid, func); ax = cpuid[0]; bx = cpuid[1]; cx = cpuid[2]; dx = cpuid[3]; }
#endif // MSVC

void cert_error(int e)
{
	switch (e) {
	case LICENSE_NOT_VALID:
		printf("Failed, license not valid\n");
		break;
	case LICENSE_EXPIRED:
		printf("Failed, license expired\n");
		break;
	default: // VERIFY_ERROR
		printf("License verification failed\n");
		break;
	}
}

void read_config(void)
{
	FILE *fh = fopen(CONFIG_FILE, "r");
	char buffer[MAXLINE], value[MAXLINE];

	if (!fh) return;

	while (fgets(buffer, MAXLINE, fh))
		if (*buffer == '#') {
			continue;
		} else if (strncasecmp("server", buffer, 6) == 0) {
			sscanf(buffer, "%*s %s", value);
			if (!reg_server) reg_server = strdup(value);
		} else if (strncasecmp("serial", buffer, 6) == 0) {
			sscanf(buffer, "%*s %s", value);
			if (!serial) serial = strdup(value);
		} else if (strncasecmp("certificate", buffer, 11) == 0) {
			sscanf(buffer, "%*s %s", value);
			if (!certificate) certificate = strdup(value);
		}

	if (!reg_server) reg_server = strdup(DEFAULT_SERVER);
	if (!certificate) certificate = strdup(DEFAULT_CERT);

	fclose(fh);
}

unsigned int GetDeviceHash()
{
	struct ifreq ifr;
	struct ifreq *it;
	struct ifconf ifc;
	char buf[1024];
	char *env;
	unsigned int i, j;
	int s, hash = 0;

	/* get current user name and hostname */
	env = getenv("USER");
	if (!env) env = getenv("USERNAME");
	else for (i = 0; i < strlen(env); i++)
		hash += (env[i] << (i * 5));

	gethostname(buf, 1024);
	for (i = 0; i < strlen(buf); i++)
		hash += (buf[i] << (i * 5));

	/* get adapters info */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1) return hash;

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	ioctl(s, SIOCGIFCONF, &ifc);

	it = ifc.ifc_req;
	for (i = 0; i < ifc.ifc_len / sizeof(struct ifreq); i++, it++) {
		strcpy(ifr.ifr_name, it->ifr_name);
		if (!ioctl(s, SIOCGIFFLAGS, &ifr)) {
			if (!(ifr.ifr_flags & IFF_LOOPBACK)) {
				if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
					for (j = 0; j < 6; j++)
						hash += (ifr.ifr_hwaddr.sa_data[j] << ((j & 1) * 8));
				}
			}
		}
	}

	close(s);

	return hash;
}

void uidtokey(unsigned char *in, char *out, int len)
{
	unsigned char *p = in;
	unsigned char *e = in + len;
	char *o = out;

	for (; p < e; p++, o += 2)
		snprintf(o, 32, "%02X", *p);

	*o = '\0';
}

int get_license(char *uid)
{
	char cmd[MAX_RSA];
	char *ecmd = NULL;
	char *result = NULL;
	FILE *lic;
	int ret;

	/* check for file exists */
	if (!access(certificate, F_OK)) return 1;

	/* prepare http request */
	ret = snprintf(cmd, MAX_RSA, "serial=%s&uid=%s", serial, uid);
	ecmd = rsa_encrypt(cmd, ret);
	if (!ecmd) return -1;

	snprintf(cmd, MAX_RSA, "/reg.php?%s", ecmd);
	free(ecmd);

	/* process http request */
	ret = http_get(reg_server, cmd, &result);
	if (ret < 0 || !result) return -1;

	/* save license to file */
	lic = fopen(certificate, "w");
	if (!lic || !fwrite(result, ret, 1, lic)) return -1;

	fclose(lic);
	free(result);

	return 0;
}

int main(int argc, char **argv)
{
	char uid[17];
	char key[33];
	char *result = NULL;
	unsigned char *buffer = NULL;
	unsigned int hash = 0;
	int ax, bx, cx, dx;
	int ret = 0;

	read_config();
	cert_init();

	/* processor info and feature bits */
	getCPUId(1, ax, bx, cx, dx);

	/* get device hash */
	hash = GetDeviceHash();

	/* make UID and KEY */
	snprintf(uid, 17, "%08X%08X", ax, hash);
	uidtokey((unsigned char *) uid, key, 16);

	ret = get_license(uid);
	if (ret < 0) {
		printf("Could not get license, error: %s\n", strerror(errno));
		return -1;
	} else if (ret > 0) {
		printf("License file found: %s\n", certificate);
	} else {
		printf("Server response received\n");
	}

	/* verify license file */
	ret = read_cert(certificate, &result);
	ret = aes_decrypt((unsigned char *) result, ret, &buffer, key, uid);

	if (ret <= 0) {
		printf("No License data found\n");
		ret = -1;
	} else {
		ret = verify_cert(buffer);
		printf("License verification: ");
		if (ret == LICENSE_VALID) {
			printf("Success\n");
			print_cert_info(buffer);
		} else {
			cert_error(ret);
		}
	}

	if (ret < 0 && argc == 1)
		(void) unlink(certificate);

	free(buffer);
	free(result);

	return ret;
}

/* end */
