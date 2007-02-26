/**
 *    \file    dice/src/be/BECreateException.h
 *  \brief   contains the declaration of the class CBECreateException
 *
 *    \date    06/30/2005
 *    \author  Ronald Aigner <ra3@os.inf.tu-dresden.de>
 */
/*
 * Copyright (C) 2001-2005
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

/** preprocessing symbol to check header file */
#ifndef __DICE_BECREATEEXCEPTION_H__
#define __DICE_BECREATEEXCEPTION_H__

#include "Exception.h"

/** \class CBECreateException
 *  \ingroup backend
 *  \brief base class for all exceptions
 */
class CBECreateException : public CException
{
// Constructor
public:
    /** the constructor for this class */
    CBECreateException();
    /** the constructor with a reason */
    CBECreateException(string sReason);
    virtual ~CBECreateException();

    void Print();

protected:
    /** \brief copy constructor
     *  \param src the source to copy from
     */
    CBECreateException(CBECreateException &src);

};

#endif  // __DICE_BECREATEEXCEPTION_H__