/**
 *	\file	dice/src/be/l4/L4BERcvAnyFunction.h
 *	\brief	contains the declaration of the class CL4BERcvAnyFunction
 *
 *	\date	Thu Jul 4 2002
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

/** preprocessing symbol to check header file */

#ifndef L4BERCVANYFUNCTION_H
#define L4BERCVANYFUNCTION_H

#include <be/BERcvAnyFunction.h>

/** \class CL4BERcvAnyFunction
 *  \ingroup backend
 *  \brief implements the L4 specific receive-any function
 *
 * This class implements a function which might receive  any message from a specific
 * sender. Because these messages may also contain flexpages, we have to write the
 * flexpage patch. Since we need an opcode var to determine which message was sent,
 * we need this as well.
 */
class CL4BERcvAnyFunction : public CBERcvAnyFunction
{
DECLARE_DYNAMIC(CL4BERcvAnyFunction);
public:
    /** default constructor for class */
	CL4BERcvAnyFunction();
	~CL4BERcvAnyFunction();
	
protected:
	virtual void WriteIPCErrorCheck(CBEFile *pFile, CBEContext *pContext);
	virtual void WriteVariableDeclaration(CBEFile *pFile, CBEContext *pContext);
	virtual void WriteUnmarshalling(CBEFile *pFile, int nStartOffset, bool& bUseConstOffset, CBEContext *pContext);
	virtual void WriteInvocation(CBEFile *pFile, CBEContext *pContext);
    virtual void WriteFlexpageOpcodePatch(CBEFile *pFile, CBEContext *pContext);
    virtual void WriteVariableInitialization(CBEFile * pFile, CBEContext * pContext);
    virtual void WriteIPC(CBEFile *pFile, CBEContext *pContext);
};

#endif
