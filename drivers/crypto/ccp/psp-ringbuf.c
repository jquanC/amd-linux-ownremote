/*
 * HYGON Platform Security Processor (PSP) interface
 *
 * Copyright (C) 2016-2017 HYGON, Inc.
 *
 * Author: Baoshun Fang <baoshunfang@hygon.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "psp-ringbuf.h"

int sev_queue_init(struct sev_queue *ring_buf,
		   void *buffer, unsigned int size, size_t esize)
{
	size /= esize;

	ring_buf->head = 0;
	ring_buf->tail = 0;
	ring_buf->esize = esize;
	ring_buf->data = (u64)buffer;
	ring_buf->mask = size - 1;
	ring_buf->data_align = ALIGN(ring_buf->data, SEV_RING_BUFFER_ALIGN);

	return 0;
}
