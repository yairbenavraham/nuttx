/****************************************************************************
 * net/devif/ip_forward.c
 *
 *   Copyright (C) 2017 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <string.h>
#include <debug.h>
#include <errno.h>

#include "devif/ip_forward.h"

#if defined(CONFIG_NET_IPFORWARD) &&  defined(CONFIG_NETDEV_MULTINIC)

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* This is an array of pre-allocating forwarding structures */

static struct forward_s g_fwdpool[CONFIG_NET_IPFORWARD_NSTRUCT];

/* This is a list of free forwarding structures */

static FAR struct forward_s *g_fwdfree;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: ip_forward_initialize
 *
 * Description:
 *   Initialize the struct forward_s allocator.
 *
 * Assumptions:
 *   Called early in system initialization.
 *
 ****************************************************************************/

void ip_forward_initialize(void)
{
  FAR struct forward_s *fwd;
  int i;

  /* Add all pre-allocated forwarding structures to the free list */

  g_fwdfree = NULL;

  for (i = 0; i < CONFIG_NET_IPFORWARD_NSTRUCT; i++)
    {
      fwd          = &g_fwdpool[i];
      fwd->f_flink = g_fwdfree;
      g_fwdfree    = fwd;
    }
}

/****************************************************************************
 * Name: ip_forward_alloc
 *
 * Description:
 *   Allocate a forwarding structure by removing a pre-allocated entry from
 *   a free list.
 *
 * Assumptions:
 *   Caller holds the network lock.  Mutually excluvive access to the free
 *   list is assured by this lock.
 *
 ****************************************************************************/

FAR struct forward_s *ip_forward_alloc(void)
{
  FAR struct forward_s *fwd;

  fwd = g_fwdfree;
  if (fwd != NULL)
    {
      g_fwdfree = fwd->f_flink;
      memset (fwd, 0, sizeof(struct forward_s));
    }

  return fwd;
}

/****************************************************************************
 * Name: ip_forward_free
 *
 * Description:
 *   Free a forwarding structure by adding it to a free list.
 *
 * Assumptions:
 *   Caller holds the network lock.  Mutually excluvive access to the free
 *   list is assured by this lock.
 *
 ****************************************************************************/

void ip_forward_free(FAR struct forward_s *fwd)
{
  fwd->f_flink = g_fwdfree;
  g_fwdfree    = fwd;
}

#endif /* CONFIG_NET_IPFORWARD && CONFIG_NETDEV_MULTINIC */
