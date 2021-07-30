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

static void enqueue_data(struct sev_queue *ring_buf,
			 const void *src, unsigned int len, unsigned int off)
{
	unsigned int size = ring_buf->mask + 1;
	unsigned int esize = ring_buf->esize;
	unsigned int l;
	void *data;

	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	data = (void *)ring_buf->data_align;
	memcpy(data + off, src, l);
	memcpy(data, src + l, len - l);
	/*
	 * make sure that the data in the ring buffer is up to date before
	 * incrementing the ring_buf->tail index counter
	 */
	smp_wmb();
}

static unsigned int queue_avail_size(struct sev_queue *ring_buf)
{
	/* According to the nature of unsigned Numbers,
	 * it always work well even though tail < head
	 * Reserved 1 element to distinguish full and empty
	 */
	return ring_buf->mask - (ring_buf->tail-ring_buf->head);
}

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

unsigned int enqueue_cmd(struct sev_queue *ring_buf,
			 const void *buf, unsigned int len)
{
	unsigned int size;

	size = queue_avail_size(ring_buf);
	if (len > size)
		len = size;

	enqueue_data(ring_buf, buf, len, ring_buf->tail);
	ring_buf->tail += len;
	return len;
}

static void dequeue_data(struct sev_queue *ring_buf, void *dst,
		unsigned int len, unsigned int off)
{
	unsigned int size = ring_buf->mask + 1;
	unsigned int esize = ring_buf->esize;
	unsigned int l;

	off &= ring_buf->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy(dst, (void *)(ring_buf->data + off), l);
	memcpy((void *)((uintptr_t)dst + l), (void *)ring_buf->data, len - l);
	/*
	 * make sure that the data is copied before
	 * incrementing the ringbuf->tail index counter
	 */
	smp_wmb();
}

unsigned int dequeue_stat(struct sev_queue *ring_buf,
			  void *buf, unsigned int len)
{
	unsigned int size;

	size = ring_buf->tail - ring_buf->head;
	if (len > size)
		len = size;

	dequeue_data(ring_buf, buf, len, ring_buf->head);
	ring_buf->head += len;
	return len;
}

