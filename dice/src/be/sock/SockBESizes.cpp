/**
 *	\file	dice/src/be/sock/SockBESizes.cpp
 *	\brief	contains the implementation of the class CSockBESizes
 *
 *	\date	11/28/2002
 *	\author	Ronald Aigner <ra3@os.inf.tu-dresden.de>
 *
 * Copyright (C) 2001-2003
 * Dresden University of Technology, Operating Systems Research Group
 *
 * This file contains free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2 as
 * published by the Free Software Foundation (see the file COPYING).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For different licensing schemes please contact
 * <contact@os.inf.tu-dresden.de>.
 */

#include "be/sock/SockBESizes.h"
#include <sys/socket.h>

IMPLEMENT_DYNAMIC(CSockBESizes);

CSockBESizes::CSockBESizes()
{
    IMPLEMENT_DYNAMIC_BASE(CSockBESizes, CBESizes);
}

CSockBESizes::CSockBESizes(CSockBESizes & src)
: CBESizes(src)
{
    IMPLEMENT_DYNAMIC_BASE(CSockBESizes, CBESizes);
}

/**	\brief destructor of target class */
CSockBESizes::~CSockBESizes()
{

}

/** \brief calculate the maximum for a specific type
 *  \param nFEType the type to get the max-size for
 *  \return the maximum size in bytes
 */
int CSockBESizes::GetMaxSizeOfType(int nFEType)
{
    // for sockets, the maximum size is approximately 8KB,
    // so we restrict this to 2KB, assuming we have a max of 4
    // var sized parameters
    // even if we would have more than 4, so what...;)
    return 2048;
}

/** \brief tries to find out the size of possible environment types
 *  \param sName the name of the environment type
 *  \return the size of this type
 */
int CSockBESizes::GetSizeOfEnvType(String sName)
{
    if (sName == "sockaddr_in")
	    return sizeof(struct sockaddr);
    if (sName == "CORBA_Object")
        return 4; // sizeof(CORBA_Object_base*)
    if (sName == "CORBA_Object_base")
	    return sizeof(struct sockaddr);
    if (sName == "CORBA_Environment")
        return 24; // 4(major+repos_id) + 4(param) + 4(srv_port) + 4(cur_socket) + 4(user_data) + 4(malloc ptr)
    return CBESizes::GetSizeOfEnvType(sName);
}
