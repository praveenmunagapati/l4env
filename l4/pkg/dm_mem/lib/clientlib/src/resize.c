/* $Id$ */
/*****************************************************************************/
/**
 * \file   dm_mem/client-lib/src/resize.c
 * \brief  Memory dataspace manager client library, resize dataspace
 *
 * \date   01/30/2002
 * \author Lars Reuther <reuther@os.inf.tu-dresden.de>
 */
/*****************************************************************************/

/* (c) 2003 Technische Universitaet Dresden
 * This file is part of DROPS, which is distributed under the terms of the
 * GNU General Public License 2. Please see the COPYING file for details.
 */

/* L4/L4Env includes */
#include <l4/sys/types.h>
#include <l4/env/errno.h>
#include <l4/util/macros.h>

/* DMmem includes */
#include <l4/dm_mem/dm_mem.h>
#include <l4/dm_mem/dm_mem-client.h>

/*****************************************************************************
 *** libdm_mem API functions
 *****************************************************************************/

/*****************************************************************************/
/**
 * \brief  Resize dataspace, IDL call wrapper
 * 
 * \param  ds            Dataspace id
 * \param  new_size      New dataspace size
 *	
 * \return 0 on success (resized dataspace), error code otherwise:
 *         - \c -L4_EIPC    IPC error calling dataspace manager
 *         - \c -L4_EINVAL  invalid dataspace id
 *         - \c -L4_EPERM   caller is not the owner of the dataspace
 *         - \c -L4_ENOMEM  out of memory
 */
/*****************************************************************************/ 
int
l4dm_mem_resize(l4dm_dataspace_t * ds, 
		l4_size_t new_size)
{
  int ret;
  CORBA_Environment _env = dice_default_environment;
  
  /* call dataspace manager */
  ret = if_l4dm_mem_resize_call(&(ds->manager),ds->id,new_size,&_env);
  if (ret || (_env.major != CORBA_NO_EXCEPTION))
    {
      ERROR("libdm_mem: resize dataspace %u at %x.%x failed (ret %d, exc %d)!",
	    ds->id,ds->manager.id.task,ds->manager.id.lthread,ret,_env.major);
      if (ret)
        return ret;
      else
	return -L4_EIPC;      
    }
  
  /* done */
  return 0;    
}
