#ifndef _MLVPN_PKT_H
#define _MLVPN_PKT_H

#include <stdint.h>

/* TCP overhead = 66 Bytes on the wire */
#define DEFAULT_MTU 1500
#define MAX_PKT_LEN 1500

typedef struct
{
    uint8_t magic;
    uint16_t len;
    /* I don't include "HAVE_MLVPN_REORDER" ifdef there because
     * I just want MLVPN to be compatible with one side reordering and the
     * other not re-ordering.
     */
    uint16_t seq;
    char data[DEFAULT_MTU];
} pktdata_t;

#define PKTHDRSIZ(pktdata) (sizeof(pktdata)-sizeof(pktdata.data))

typedef struct
{
    pktdata_t pktdata;
    /* This variable permits to "sleep" some time before
     * sending a new packet.
     * This is used to permit trafic shaping
     * on the "bulk" queue (not on hpsbuf)
     */
    uint64_t next_packet_send;
} mlvpn_pkt_t;

#endif
