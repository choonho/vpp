/*
 * Copyright (c) 2015 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __included_elb_h__
#define __included_elb_h__

#include <elb/util.h>

#include <vnet/vnet.h>
#include <vnet/ip/ip.h>
#include <vnet/ethernet/ethernet.h>
#include <vnet/fib/fib_table.h>

#include <vppinfra/hash.h>
#include <vppinfra/error.h>
#include <vppinfra/elog.h>

#include <elb/lbhash.h>

typedef enum {
  ELB_NEXT_DROP,
  ELB_N_NEXT,
} elb_next_t;

typedef enum {
  MODE_TCP,
} elb_mode_t;

/**
 * Frontend
 */
typedef struct {
  CLIB_CACHE_LINE_ALIGN_MARK (cacheline0);
  /**
   * Binding address:port
   */
  ip46_address_t address;
  u16 port;

  /**
   * Mode (TCP|HTTP|HTTPS)
   * Supported: TCP
   */
  elb_mode_t mode;

  /**
   * Frontend name
   */
  u8 * name;

  /**
   * Flags
   */
  u8 flags;
#define ELB_FE_FLAGS_USED 0x1

} elb_frontend_t;


typedef struct {
    /**
     * Each CPU has its own sticky flow hash table.
     * One single table is used for all VIPs.
     */
    lb_hash_t *sticky_ht;
} elb_per_cpu_t;


typedef struct {
  /**
   * Pool of all frontends
   */
  elb_frontend_t *fes;

  /**
   * Some global data is per-cpu
   */
  elb_per_cpu_t *per_cpu;

  /**
   * API dynamically registered base ID.
   */
  u16 msg_id_base;

  volatile u32 *writer_lock;
} elb_main_t;

extern elb_main_t elb_main;
extern vlib_node_registration_t elb_node;

int elb_frontend_add(u8 *name, ip46_address_t *host, u16 port, u8 mode, u32 *fe_index);

format_function_t format_elb_main;

#endif /* __included_elb_h__ */
