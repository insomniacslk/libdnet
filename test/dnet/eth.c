/*
 * eth.c
 *
 * Copyright (c) 2001 Dug Song <dugsong@monkey.org>
 *
 * $Id$
 */

#include "config.h"

#include <sys/types.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dnet.h"
#include "dnet-int.h"

void
eth_usage(int die)
{
	fprintf(stderr, "Usage: dnet eth [type|src|dst <value>] ... \n");
	if (die)
		exit(1);
}

int
eth_main(int argc, char *argv[])
{
	struct eth_hdr *eth;
	struct addr addr;
	u_char *p, buf[ETH_LEN_MAX];	/* XXX */
	char *name, *value;
	int c, len;

	eth = (struct eth_hdr *)buf;
	memset(eth, 0, sizeof(*eth));
	eth->eth_type = htons(ETH_TYPE_IP);

	for (c = 0; c + 1 < argc; c += 2) {
		name = argv[c];
		value = argv[c + 1];

		if (strcmp(name, "type") == 0) {
			if (type_aton(value, &eth->eth_type) < 0)
				eth_usage(1);
		} else if (strcmp(name, "src") == 0) {
			if (addr_aton(value, &addr) < 0)
				eth_usage(1);
			memcpy(&eth->eth_src, &addr.addr_eth, ETH_ADDR_LEN);
		} else if (strcmp(name, "dst") == 0) {
			if (addr_aton(value, &addr) < 0)
				eth_usage(1);
			memcpy(&eth->eth_dst, &addr.addr_eth, ETH_ADDR_LEN);
		} else
			eth_usage(1);
	}
	argc -= c;
	argv += c;

	if (argc != 0)
		eth_usage(1);
	
	if (isatty(STDIN_FILENO))
		err(1, "can't read Ethernet payload from tty");
	
	p = buf + ETH_HDR_LEN;
	len = sizeof(buf) - (p - buf);
	
	while ((c = read(STDIN_FILENO, p, len)) > 0) {
		p += c;
		len -= c;
	}
	len = p - buf;
	
	if (write(STDOUT_FILENO, buf, len) != len)
		err(1, "write");
	
	return (0);
}
