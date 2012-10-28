#ifndef MLVPN_TUNTAP_GENERIC_H
#define MLVPN_TUNTAP_GENERIC_H

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "buffer.h"
#include "privsep.h"
#include "debug.h"
#include "mlvpn.h"

enum tuntap_type {
    MLVPN_TUNTAPMODE_TUN,
    MLVPN_TUNTAPMODE_TAP
};

struct tuntap_s
{
    int fd;
    int mtu;
    enum tuntap_type type;
    /* Packet sequence (for re-ordering system) */
    uint16_t seq;
    circular_buffer_t *sbuf;
    char devname[MLVPN_IFNAMSIZ];
};

int mlvpn_tuntap_alloc(struct tuntap_s *tuntap);
int mlvpn_tuntap_read(struct tuntap_s *tuntap);
int mlvpn_tuntap_write(struct tuntap_s *tuntap);

/* runs as root! */
int root_tuntap_open(int tuntapmode, char *devname);

#endif
