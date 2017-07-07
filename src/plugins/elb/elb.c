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
#include <vnet/vnet.h>
#include <vpp/app/version.h>
#include <vnet/plugin/plugin.h>

#include <vlibapi/api.h>
#include <vlibmemory/api.h>
#include <vlibsocket/api.h>

elb_main_t elb_main;

#define elb_get_writer_lock() do {} while(__sync_lock_test_and_set (elb_main.writer_lock, 1))
#define elb_put_writer_lock() elb_main.writer_lock[0] = 0

static
int elb_frontend_find_index_with_lock(ip46_address_t *host, u16 port, u32 *fe_index)
{
  elb_main_t *em = &elb_main;
  elb_frontend_t *fe;
  ASSERT (em->writer_lock[0]);  //This must be called with the lock owned
  pool_foreach(fe, em->fes, {
    if ((fe->flags && ELB_FE_FLAGS_USED) &&
        fe->port == port &&
        fe->address.as_u64[0] == host->as_u64[0] &&
        fe->address.as_u64[1] == host->as_u64[1]) {
      *fe_index = fe - em->fes;
      return 0;
    }
  });
  return VNET_API_ERROR_NO_SUCH_ENTRY;
}

int elb_frontend_add(u8 *name, ip46_address_t *host, u16 port,  u8 mode, u32 *fe_index)
{
  elb_main_t *em = &elb_main;
  u8 *name_dup = vec_dup(name);
  elb_frontend_t *fe;
  elb_get_writer_lock();

  //Check Already exist or not
  if (!elb_frontend_find_index_with_lock(host, port, fe_index)) {
    elb_put_writer_lock();
    return VNET_API_ERROR_VALUE_EXIST;
  }

  //Allocate
  pool_get(em->fes, fe);

  //Init
  fe->name = name_dup;
  fe->address = *host;
  fe->port = port;
  fe->mode = mode;
  fe->flags = ELB_FE_FLAGS_USED;
 
  //Create a new flow hash table full of the default entry
  //elb_vip_update_new_flow_table(vip);

  //Return result
  *fe_index = fe - em->fes;

  elb_put_writer_lock();
  return 0;
}

u8 *format_elb_main (u8 * s, va_list * args)
{
  elb_frontend_t *fe;
  //vlib_thread_main_t *tm = vlib_get_thread_main();
  elb_main_t *em = &elb_main;
  s = format(s, "elb_main\n");
  s = format(s, " #Frontends: %u\n", pool_elts(em->fes));

  pool_foreach(fe, em->fes, {
    s = format(s, "  #name:    %s\n", fe->name);
    s = format(s, "  #address: %U\n", format_ip46_address, &fe->address, IP46_TYPE_ANY);
    s = format(s, "  #port:    %u\n", fe->port);
    s = format(s, "  #mode:    %u\n", fe->mode);
    }
  );
  return s;
}

/* *INDENT-OFF* */
VLIB_PLUGIN_REGISTER () = {
    .version = VPP_BUILD_VER,
    .description = "Elastic Load Balancer",
};
/* *INDENT-ON* */

/**
 * @brief Initialize the ELB plugin.
 */
static clib_error_t * elb_init (vlib_main_t * vm)
{
  vlib_thread_main_t *tm = vlib_get_thread_main ();

  elb_main_t * em = &elb_main;
  clib_error_t * error = 0;


  em->fes = 0;
  em->per_cpu = 0;
  vec_validate(em->per_cpu, tm->n_vlib_mains - 1);
  em->writer_lock = clib_mem_alloc_aligned (CLIB_CACHE_LINE_BYTES, CLIB_CACHE_LINE_BYTES);
  em->writer_lock[0] = 0;

  //Allocate and init default Backend

  return error;
}

VLIB_INIT_FUNCTION (elb_init);


