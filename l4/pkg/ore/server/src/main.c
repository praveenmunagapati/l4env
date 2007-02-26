/****************************************************************
 * ORe - Oshkosh Resurrection                                   *
 *                                                              *
 * Bjoern Doebel <doebel@os.inf.tu-dresden.de>                  *
 * 2005-08-10                                                   *
 *                                                              *
 * (c) 2005 - 2007 Technische Universitaet Dresden              *
 * This file is part of DROPS, which is distributed under the   *
 * terms of the GNU General Public License 2. Please see the    *
 * COPYING file for details.                                    *
 ****************************************************************/

#include <linux/netdevice.h>

#include <l4/sys/types.h>
#include <l4/util/l4_macros.h>
#include <l4/util/util.h>
#include <l4/util/parse_cmd.h>
#include <l4/rmgr/librmgr.h>
#include <l4/dde_linux/dde.h>
#include <l4/names/libnames.h>
#include <l4/sys/syscalls.h>
#include <l4/util/getopt.h>
#include <l4/thread/thread.h>

#include <stdlib.h>

#include "ore-local.h"

static char *_oreName = "ORe";
/* MAC addresses are by default 04:EA:xx:xx:xx:xx; where xx is a checksum
 * over real MAC and ORe handle
 * FIXME no global vars, please */
unsigned char global_mac_address_head[4] = { 0x00, 0x00, 0x00, 0x00};
static   int  use_events;
int loopback_only = 0;

ore_connection_t ore_connection_table[ORE_CONFIG_MAX_CONNECTIONS];
l4_threadid_t ore_main_server   = L4_INVALID_ID;

/* Our version of CORBA_alloc().
 *
 * This one is tricky: Dice-generated code will allocate buffers for receiving
 * packets, because ore_recv() is marked "prealloc" within the ORe idl. On the
 * server side this is however unnecessary, because we will never use this buffer
 * to send a packet to a client, but we use the data area of an skb allocated by
 * the NIC driver instead.
 *
 * The Dice code's malloc/free-policy may lead to contention of our whole kernel
 * memory, if a client issues several requests one after another. In this case
 * we will run out of memory. This may be circumvented, if we do not allocate
 * memory here but return a NULL pointer. This is safe as long as we do not use
 * the Dice buffer.
 *
 * A special case is Dice's first call to CORBA_alloc(). This will allocate
 * memory for the real receive buffer and is truly necessary. As we know how
 * large this buffer is, we kmalloc() memory if someone calls CORBA_alloc() with
 * the right size.
 *
 * However, this may result in memory being allocated if later Dice code wants
 * to have a buffer of this size, but this incident will not happen that often,
 * so that we may live with that.
 *
 * FIXME: Dice should be altered to distinguish client-side and server-side
 *        prealloc.
 */
void *CORBA_alloc(unsigned long size)
{
#ifndef SENSOR
  if (size == ORE_CONFIG_MAX_BUF_SIZE)
#endif
    return kmalloc(size, GFP_KERNEL);

#ifndef SENSOR
  return NULL;
#endif
}

/* CORBA_free() - not that tricky. We only free ptr if it is != NULL. */
void CORBA_free(void *ptr)
{
  if (ptr == NULL)
    return;

  kfree(ptr);
}

/** Copy mac address from command line
 */
static void copy_mac(int id, const char *arg, int num)
{
  unsigned long tmp = strtoul(optarg, NULL, 16);

  global_mac_address_head[3] = (unsigned char)(tmp & 0xff);
  global_mac_address_head[2] = (unsigned char)(tmp >> 8 & 0xff);
  global_mac_address_head[1] = (unsigned char)(tmp >> 16 & 0xff);
  global_mac_address_head[0] = (unsigned char)(tmp >> 24 & 0xff);
}

/* Basic initialization. */
int main(int argc, const char **argv)
{
  int ret;

  // setup server environment
  DICE_DECLARE_SERVER_ENV(env);
  env.malloc = (dice_malloc_func)CORBA_alloc;
  env.free   = (dice_free_func)CORBA_free;

  // using this function induces two benefits
  // (1) it is shorter and better to read than getopt_long()
  // (2) it provides a help screen if the binary is called with --help
  if (parse_cmdline(&argc, &argv,
                    'd', "debug", "debug on",
                    PARSE_CMD_SWITCH, 1, &ore_debug,
                    'e', "events", "use events to detect broken connections",
                    PARSE_CMD_SWITCH, 1, &use_events,
                    'l', "loopback-only", "use only a loopback device",
                    PARSE_CMD_SWITCH, 1, &loopback_only,
                    'm', "mac", "mac address",
                    PARSE_CMD_FN_ARG, 0, &copy_mac,
                    'n', "name", "configure server name",
                    PARSE_CMD_STRING, "ORe", &_oreName,
                    0, 0))
    return 1;

  ore_main_server = l4_myself();

  rmgr_init();

  LOGd(ore_debug, "debug is on");

  // initialize linux emulation library
  LOGd(ORE_DEBUG_INIT, "memsize = %d", ORE_LINUXEMUL_MEMSIZE);
  ret = init_emulation(ORE_LINUXEMUL_MEMSIZE);
  LOGd(ORE_DEBUG_INIT, "init_emulation: %d (%s)", ret, l4env_strerror(-ret));

  LOG("loopback: %d", loopback_only);
  // initialize network devices
  ret = open_network_devices();
  LOG("Initialized %d network devices.", ret);

  // always list devices at start time
  list_network_devices();

  init_connection_table();

  // register
  LOG("Registering '%s' at names...", _oreName);
  ret = names_register(_oreName);
  if (!ret)
    LOG_Error("Could not register at names.");

  LOGd(ORE_DEBUG_INIT, "registered at names: %d", ret);

#ifdef CONFIG_ORE_EVENTS
  if (use_events)
    {
      l4thread_create(handle_events, NULL, L4THREAD_CREATE_SYNC);
      LOGd(ORE_DEBUG_INIT, "created event handler thread.");
    }
#endif

#ifdef CONFIG_ORE_DUMPER
  l4thread_create(dump_periodic, NULL, L4THREAD_CREATE_ASYNC);
  LOGd(ORE_DEBUG, "created debug dumper thread");
#endif

  __l4ore_tls_id_key = l4thread_data_allocate_key();
  LOGd(ORE_DEBUG_INIT, "Allocated tls key: %d", __l4ore_tls_id_key);

#ifdef ORE_DSI
  dsi_init();
#endif

  LOG("Ready for service.");
  // The main thread enters the manager server loop and is 
  // now only responsible for managing connections
  ore_manager_server_loop(&env);

  LOG("Loop returned.\n");
  l4_sleep_forever();

  return 0;
}
