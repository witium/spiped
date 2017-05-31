#include <sys/socket.h>
#include <sys/time.h>

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "parsenum.h"
#include "sock.h"
#include "warnp.h"

int
main(int argc, char ** argv)
{
	/* Command-line parameter. */
	const char * addr = NULL;
	size_t buflen;
	size_t count;

	/* Working variables. */
	struct sock_addr ** sas_t;
	clock_t begin;
	clock_t end;
	long double duration;
	char * buffer;
	int socket;
	size_t to_send;

	WARNP_INIT;

	/* Parse command-line arguments. */
	if (argc != 4) {
		warn0("usage: %s ADDRESS BUFLEN COUNT", argv[0]);
		goto err0;
	}
	addr = argv[1];
	if (PARSENUM(&buflen, argv[2], 1, SSIZE_MAX)) {
		warnp("parsenum");
		goto err0;
	}
	if (PARSENUM(&count, argv[3])) {
		warnp("parsenum");
		goto err0;
	}

	/* Allocate and fill buffer to send. */
	if ((buffer = malloc(buflen)) == NULL) {
		warnp("Out of memory");
		goto err0;
	}
	memset(buffer, 0, buflen);

	/* Initialize count. */
	to_send = count;

	/* Resolve target address. */
	if ((sas_t = sock_resolve(addr)) == NULL) {
		warnp("Error resolving socket address: %s", addr);
		goto err1;
	}
	if (sas_t[0] == NULL) {
		warn0("No addresses found for %s", addr);
		goto err2;
	}

	/* Connect to target. */
	if ((socket = sock_connect(sas_t)) == -1) {
		warnp("sock_connect");
		goto err2;
	}

	/* Make it blocking. */
	if (fcntl(socket, F_SETFL,
	    fcntl(socket, F_GETFL, 0) & (~O_NONBLOCK)) == -1) {
		warnp("Cannot make connection blocking");
		goto err3;
	}

	/* Get beginning CPU time. */
	if ((begin = clock()) == (clock_t)-1) {
		warn0("Failed to get CPU time from clock()");
		goto err3;
	}

	while (to_send > 0) {
		if (write(socket, buffer, buflen) != (ssize_t)buflen) {
			warnp("write failed");
			goto err3;
		}
		to_send--;
	}

	/* Get ending CPU time. */
	if ((end = clock()) == (clock_t)-1) {
		warn0("Failed to get CPU time from clock()");
		goto err3;
	}

	if (shutdown(socket, SHUT_RDWR)) {
		warnp("shutdown");
		goto err3;
	}

	if (close(socket)) {
		warnp("close");
		goto err2;
	}

	/* Print duration and speed. */
	duration = ((long double)(end - begin)) / CLOCKS_PER_SEC;
	printf("%zu\t%zu\t%Lf\t%Lf\n", buflen, count, duration,
		buflen * count / duration / (long double)1e6f);

	/* Clean up. */
	sock_addr_freelist(sas_t);
	free(buffer);

	/* Success! */
	exit(0);

err3:
	close(socket);
err2:
	sock_addr_freelist(sas_t);
err1:
	free(buffer);
err0:
	/* Failure! */
	exit(1);
}
