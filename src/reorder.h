#ifndef MLVPN_REORDER_H
#define MLVPN_REORDER_H

#include "mlvpn.h"
#include "debug.h"

#define REORDER_MAX_SEQ 65500
/* Re-ordering timeout in milli second.
 * This valus *MUST* be more than time needed for a packet to travel from
 * client to server. (RTT/2 should be good)
 * Increasing this value will be "conservative" on packet loss conditions
 * but will increase latency.
 * Decreasing the timeout means more agressive buffer flush on packet loss
 * (bandwidth drop but latency cut also (which is good))
 */
#define REORDER_TIMEOUT 500

typedef struct
{
    int size;
    int elems;
    /* If now is beyond this value, we send the "next" data packet available NOW. */
    uint16_t next_seq;
    uint64_t timeout;
    int *used;
    pktdata_t **pkts;
} reorder_buffer_t;

reorder_buffer_t *
mlvpn_reorder_init(int size);

void
mlvpn_reorder_free(reorder_buffer_t *buf);
int
mlvpn_reorder_is_full(reorder_buffer_t *buf);

int
mlvpn_reorder_is_empty(reorder_buffer_t *buf);

void
mlvpn_reorder_flush(reorder_buffer_t *buf);

int
mlvpn_reorder_timeout(reorder_buffer_t *buf);

int
mlvpn_reorder_put(reorder_buffer_t *buf, pktdata_t *pktdata);

pktdata_t *
mlvpn_reorder_get_next(reorder_buffer_t *buf);

pktdata_t *
mlvpn_reorder_get_outoforder(reorder_buffer_t *buf);

void
mlvpn_reorder_set_seq(reorder_buffer_t *buf, uint16_t seq);

#endif