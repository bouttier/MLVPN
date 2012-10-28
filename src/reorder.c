#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "debug.h"
#include "pkt.h"
#include "tool.h"
#include "mlvpn.h"
#include "reorder.h"

reorder_buffer_t *
mlvpn_reorder_init(int size)
{
    int i;
    reorder_buffer_t *buf;

    if (size > REORDER_MAX_SEQ)
    {
        _ERROR("[reorder] buffer size can't be more than %d. "
               "Shrinking to maximum value.\n", REORDER_MAX_SEQ);
        size = REORDER_MAX_SEQ;
    }

    /* Basic circular buffer allocation */
    buf = malloc(sizeof(reorder_buffer_t));
    buf->size = size;
    buf->elems = 0;
    buf->timeout = 0;
    buf->next_seq = 0;
    buf->pkts = malloc(buf->size * sizeof(pktdata_t *));
    for(i = 0; i < buf->size; i++)
        buf->pkts[i] = calloc(1, sizeof(pktdata_t));
    buf->used = calloc(sizeof(int), buf->size);
    return buf;
}

void
mlvpn_reorder_free(reorder_buffer_t *buf)
{
    free(buf->used);
    free(buf->pkts);
    free(buf);
}

int
mlvpn_reorder_is_full(reorder_buffer_t *buf)
{
    return (buf->elems == buf->size) ? 1 : 0;
}

int
mlvpn_reorder_is_empty(reorder_buffer_t *buf)
{
    return (buf->elems == 0) ? 1 : 0;
}

void
mlvpn_reorder_flush(reorder_buffer_t *buf)
{
    _INFO("[reorder] buffer flush.\n");
    buf->elems = 0;
    buf->timeout = 0;
    memset(buf->used, 0, sizeof(int) * buf->size);
}

/* returns 1 if timeout condition is reached. 0 otherwise. */
int
mlvpn_reorder_timeout(reorder_buffer_t *buf)
{
    return (buf->timeout >= mlvpn_millis()) ? 1 : 0;
}

/* Returns 0 if everything is fine or 1 if buffer was full. */
int
mlvpn_reorder_put(reorder_buffer_t *buf, pktdata_t *pktdata)
{
    int ret = 0;
    int i;
    _DEBUG("[reorder] new packet received. Seq: %u\n", pktdata->seq);
    if (mlvpn_reorder_is_full(buf))
    {
        _WARNING("[reorder] buffer is full! Flushing the whole buffer!\n");
        mlvpn_reorder_flush(buf);
        ret = 1;
    }
    for(i = 0; i < buf->size && buf->used[i] == 1; i++);

    memcpy(buf->pkts[i], pktdata, sizeof(pktdata_t));
    buf->used[i] = 1;
    buf->elems++;
    return ret;
}

/* Try to get the packet with matching next_seq.
 * TODO: optimizing, and do not flush the whole queue in case of packet loss!
 * Flush the queue in case of timeout.
 */
pktdata_t *
mlvpn_reorder_get_next(reorder_buffer_t *buf)
{
    int i;
    pktdata_t *pktdata = NULL;

    /* Initialization condition, find the "lower sequence packet" in queue. */
    if (buf->next_seq == 0)
    {
        int min = REORDER_MAX_SEQ + 1;
        for(i = 0; i < buf->size; i++)
        {
            if (buf->used[i] == 0)
                continue;
            pktdata = buf->pkts[i];
            if (pktdata->seq < min)
                min = pktdata->seq;
        }

        /* Weird! (safety check) */
        if (min == REORDER_MAX_SEQ + 1)
            min = 0;
        buf->next_seq = min;
    }

    /* Find the next packet */
    for(i = 0; i < buf->size; i++)
    {
        if (buf->used[i] == 0)
            continue;

        pktdata = buf->pkts[i];
        if (buf->next_seq == pktdata->seq)
        {
            buf->used[i] = 0;
            buf->elems--;
            break;
        }
    }

    if (pktdata)
    {
        /* Set next waiting packet to current value +1 */
        if (buf->next_seq == REORDER_MAX_SEQ)
            buf->next_seq = 1;
        else
            buf->next_seq++;
        buf->timeout = mlvpn_millis() + REORDER_TIMEOUT;
    } else {
        if (mlvpn_reorder_timeout(buf))
        {
            /* DROP all the queue. Must find a better way! */
            _INFO("[reorder] timeout condition.\n");
            mlvpn_reorder_flush(buf);
            buf->next_seq = 0;
        }
    }
    return pktdata;
}
