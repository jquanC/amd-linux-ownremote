/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * HYGON Platform Security Processor (PSP) interface driver
 *
 * Copyright (C) 2017-2019 HYGON, Inc.
 *
 * Author: Baoshun Fang <baoshunfang@hygon.cn>
 */

#ifndef __PSP_RINGBUF_H__
#define __PSP_RINGBUF_H__

#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/dmapool.h>
#include <linux/hw_random.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/dmaengine.h>
#include <linux/psp-sev.h>
#include <linux/miscdevice.h>
#include <linux/capability.h>

int sev_queue_init(struct sev_queue *ring_buf,
		   void *buffer, unsigned int size, size_t esize);
unsigned int enqueue_cmd(struct sev_queue *ring_buf,
			 const void *buf, unsigned int len);
unsigned int dequeue_stat(struct sev_queue *ring_buf,
			  void *buf, unsigned int len);

#endif /* __PSP_RINGBUF_H__ */

