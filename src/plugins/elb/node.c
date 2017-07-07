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

#include <elb/elb.h>

#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/pg/pg.h>
#include <vppinfra/error.h>

typedef struct {
  u32 next_index;
} elb_trace_t;

u8 *
format_elb_trace (u8 * s, va_list * args)
{
  return s;
}

//vlib_node_registration_t elb_node;

#define foreach_elb_error \
  _(NONE, "no error") \
  _(PROTO_NOT_SUPPORTED, "protocol not supported")

typedef enum {
#define _(sym,str) ELB_ERROR_##sym,
  foreach_elb_error
#undef _
    ELB_N_ERROR,
} elb_error_t;

static char * elb_error_strings[] = {
#define _(sym,string) string,
  foreach_elb_error
#undef _
};

static uword
elb_node_fn (vlib_main_t * vm,
		  vlib_node_runtime_t * node,
		  vlib_frame_t * frame)
{
  return frame->n_vectors;
}

VLIB_REGISTER_NODE (elb_node) = {
  .function = elb_node_fn,
  .name = "elb",
  .vector_size = sizeof (u32),
  .format_trace = format_elb_trace,
  .type = VLIB_NODE_TYPE_INTERNAL,
  
  .n_errors = ELB_N_ERROR,
  .error_strings = elb_error_strings,

  .n_next_nodes = ELB_N_NEXT,

  /* edit / add dispositions here */
  .next_nodes = {
        [ELB_NEXT_DROP] = "error-drop",
  },
};
