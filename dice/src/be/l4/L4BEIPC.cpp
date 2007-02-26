/**
 *	\file	dice/src/be/l4/L4BEIPC.cpp
 *	\brief	contains the implementation of the class CL4BEIPC
 *
 *	\date	02/25/2003
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

#include "be/l4/L4BEIPC.h"
#include "be/l4/L4BENameFactory.h"
#include "be/l4/L4BEMsgBufferType.h"
#include "be/BEContext.h"
#include "be/BEFile.h"
#include "be/BEFunction.h"
#include "be/BEDeclarator.h"

#include "TypeSpec-Type.h"

IMPLEMENT_DYNAMIC(CL4BEIPC);

CL4BEIPC::CL4BEIPC()
{
    IMPLEMENT_DYNAMIC_BASE(CL4BEIPC, CBECommunication);
}

/**	\brief destructor of target class */
CL4BEIPC::~CL4BEIPC()
{
}

/** \brief write an IPC call
 *  \param pFile the file to write to
 *  \param pFunction the function to write it for
 *  \param pContext the context of the write operation
 */
void CL4BEIPC::WriteCall(CBEFile *pFile, CBEFunction* pFunction, CBEContext *pContext)
{
    String sServerID = pContext->GetNameFactory()->GetComponentIDVariable(pContext);
    String sResult = pContext->GetNameFactory()->GetString(STR_RESULT_VAR, pContext);
    String sTimeout = pContext->GetNameFactory()->GetTimeoutClientVariable(pContext);
    String sMWord = pContext->GetNameFactory()->GetTypeName(TYPE_MWORD, true, pContext);
    String sMsgBuffer = pContext->GetNameFactory()->GetMessageBufferVariable(pContext);
	CL4BEMsgBufferType *pMsgBuffer = (CL4BEMsgBufferType*)pFunction->GetMessageBuffer();

	// XXX FIXME:
	// not implemented, because X0 adaption has no 3 word bindings
	// CL4BESizes *pSizes = (CL4BESizes*)pContext->GetSizes();
	// bool bWord3 = (pSizes->GetMaxShortIPCSize(DIRECTION_IN) / pSizes->GetSizeOfType(TYPE_MWORD)) == 3;
	// if (bWord3)
	//   pFile->PrintIndent("l4_ipc_call_w3(*%s,\n");
	// else
	//   pFile->PrintIndent("l4_ipc_call(*%s,\n");

    pFile->PrintIndent("l4_ipc_call(*%s,\n", (const char *) sServerID);
    pFile->IncIndent();
    pFile->PrintIndent("");
    if (pMsgBuffer->HasSendFlexpages())
        pFile->Print("(%s*)((%s)", (const char*)sMWord, (const char*)sMWord);
    if (pMsgBuffer->IsShortIPC(pFunction->GetSendDirection(), pContext, 2 /* 2 short IPC parameters*/ /* (bWord3)?3:2 */))
        pFile->Print("L4_IPC_SHORT_MSG");
    else
    {
        if (pMsgBuffer->HasReference())
            pFile->Print("%s", (const char *) sMsgBuffer);
        else
            pFile->Print("&%s", (const char *) sMsgBuffer);
    }
    if (pMsgBuffer->HasSendFlexpages())
        pFile->Print("|2)");
    pFile->Print(",\n");

    pFile->PrintIndent("*((%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[0]))),\n");
    pFile->PrintIndent("*((%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[4]))),\n");

//  if (bWord3)
//     pFile->PrintIndent("*((%s*)(&(", (const char*)sMWord);
//     pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
//     pFile->Print("[8]))),\n");

    if (pMsgBuffer->IsShortIPC(pFunction->GetReceiveDirection(), pContext, 2 /* (bWord3)?3:2 */))
        pFile->PrintIndent("L4_IPC_SHORT_MSG,\n");
    else
    {
        if (pMsgBuffer->HasReference())
            pFile->PrintIndent("%s,\n", (const char *) sMsgBuffer);
        else
            pFile->PrintIndent("&%s,\n", (const char *) sMsgBuffer);
    }

    pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[0])),\n");
    pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[4])),\n");

//  if (bWord3)
//     pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
//     pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
//     pFile->Print("[8])),\n");

    pFile->PrintIndent("%s, &%s);\n", (const char *) sTimeout, (const char *) sResult);

    pFile->DecIndent();
}

/** \brief write an IPC receive operation
 *  \param pFile the file to write to
 *  \param pFunction the function to write it for
 *  \param bAllowShortIPC true if short IPC can be used
 *  \param pContext the context of the write operation
 */
void CL4BEIPC::WriteReceive(CBEFile* pFile,  CBEFunction* pFunction, bool bAllowShortIPC, CBEContext* pContext)
{
	String sServerID = pContext->GetNameFactory()->GetComponentIDVariable(pContext);
    String sResult = pContext->GetNameFactory()->GetString(STR_RESULT_VAR, pContext);
    String sTimeout;
    if (pFunction->IsComponentSide())
        sTimeout = pContext->GetNameFactory()->GetTimeoutServerVariable(pContext);
    else
        sTimeout = pContext->GetNameFactory()->GetTimeoutClientVariable(pContext);
    String sMsgBuffer = pContext->GetNameFactory()->GetMessageBufferVariable(pContext);
    String sMWord = pContext->GetNameFactory()->GetTypeName(TYPE_MWORD, true, pContext);
	CL4BEMsgBufferType *pMsgBuffer = (CL4BEMsgBufferType*)pFunction->GetMessageBuffer();

    pFile->PrintIndent("l4_ipc_receive(*%s,\n", (const char *) sServerID);
    pFile->IncIndent();

    if (pMsgBuffer->IsShortIPC(pFunction->GetReceiveDirection(), pContext, 2) && bAllowShortIPC)
        pFile->PrintIndent("L4_IPC_SHORT_MSG,\n ");
    else
    {
        if (pMsgBuffer->HasReference())
            pFile->PrintIndent("%s,\n", (const char *) sMsgBuffer);
        else
            pFile->PrintIndent("&%s,\n", (const char *) sMsgBuffer);
    }

    pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[0])),\n");
    pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[4])),\n");

    pFile->PrintIndent("%s, &%s);\n", (const char *) sTimeout, (const char *) sResult);

    pFile->DecIndent();
}

/** \brief write an IPC reply and receive operation
 *  \param pFile the file to write to
 *  \param pFunction the function to write it for
 *  \param bSendFlexpage true if a flexpage should be send (false, if the message buffer should determine this)
 *  \param bSendShortIPC true if a short IPC should be send (false, if message buffer should determine this)
 *  \param pContext the context of the write operation
 */
void CL4BEIPC::WriteReplyAndWait(CBEFile* pFile, CBEFunction* pFunction, bool bSendFlexpage, bool bSendShortIPC, CBEContext* pContext)
{
    String sResult = pContext->GetNameFactory()->GetString(STR_RESULT_VAR, pContext);
    String sTimeout;
    if (pFunction->IsComponentSide())
        sTimeout = pContext->GetNameFactory()->GetTimeoutServerVariable(pContext);
    else
        sTimeout = pContext->GetNameFactory()->GetTimeoutClientVariable(pContext);
    String sServerID = pContext->GetNameFactory()->GetComponentIDVariable(pContext);
    String sMsgBuffer = pContext->GetNameFactory()->GetMessageBufferVariable(pContext);
    String sMWord = pContext->GetNameFactory()->GetTypeName(TYPE_MWORD, true, pContext);
	CL4BEMsgBufferType *pMsgBuffer = (CL4BEMsgBufferType*)pFunction->GetMessageBuffer();

    pFile->PrintIndent("l4_ipc_reply_and_wait(*%s,\n", (const char *) sServerID);
    pFile->IncIndent();
    pFile->PrintIndent("");
    if (bSendFlexpage)
        pFile->Print("(%s*)((%s)", (const char*)sMWord, (const char*)sMWord);
    if (bSendShortIPC)
        pFile->Print("L4_IPC_SHORT_MSG");
    else
        pFile->Print("%s", (const char *) sMsgBuffer);
    if (bSendFlexpage)
        pFile->Print("|2)");
    pFile->Print(",\n");

    pFile->PrintIndent("*((%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[0]))),\n");
    pFile->PrintIndent("*((%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[4]))),\n");

    pFile->PrintIndent("%s,\n", (const char *) sServerID);

    pFile->PrintIndent("%s,\n", (const char *) sMsgBuffer);
    pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[0])),\n");
    pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[4])),\n");

    pFile->PrintIndent("%s, &%s);\n", (const char *) sTimeout, (const char *) sResult);

    pFile->DecIndent();
}

/** \brief write an IPC wait operation
 *  \param pFile the file to write to
 *  \param pFunction the function to write it for
 *  \param pContext the context of the write operation
 */
void CL4BEIPC::WriteWait(CBEFile* pFile, CBEFunction *pFunction, bool bAllowShortIPC, CBEContext* pContext)
{
    String sServerID = pContext->GetNameFactory()->GetComponentIDVariable(pContext);
    String sResult = pContext->GetNameFactory()->GetString(STR_RESULT_VAR, pContext);
    String sTimeout;
    if (pFunction->IsComponentSide())
        sTimeout = pContext->GetNameFactory()->GetTimeoutServerVariable(pContext);
    else
        sTimeout = pContext->GetNameFactory()->GetTimeoutClientVariable(pContext);
    String sMsgBuffer = pContext->GetNameFactory()->GetMessageBufferVariable(pContext);
    String sMWord = pContext->GetNameFactory()->GetTypeName(TYPE_MWORD, true, pContext);
	CL4BEMsgBufferType *pMsgBuffer = (CL4BEMsgBufferType*)pFunction->GetMessageBuffer();
    int nDirection = pFunction->GetReceiveDirection();
    bool bVarBuffer = pMsgBuffer->IsVariableSized(nDirection) || (pMsgBuffer->GetAlias()->GetStars() > 0);

    pFile->PrintIndent("l4_ipc_wait(%s,\n", (const char *) sServerID);
    pFile->IncIndent();
    if (pMsgBuffer->IsShortIPC(nDirection, pContext, 2) && bAllowShortIPC)
        pFile->PrintIndent("L4_IPC_SHORT_MSG,\n");
    else
    {
        if (bVarBuffer)
            pFile->PrintIndent("%s,\n", (const char *) sMsgBuffer);
        else
            pFile->PrintIndent("&%s,\n", (const char *) sMsgBuffer);
    }
    pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[0])),\n");
    pFile->PrintIndent("(%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[4])),\n");
    pFile->PrintIndent("%s, &%s);\n", (const char *) sTimeout, (const char *) sResult);
    pFile->DecIndent();
}

/** \brief write an IPC send operation
 *  \param pFile the file to write to
 *  \param pFunction the function to write it for
 *  \param pContext the context of the write operation
 */
void CL4BEIPC::WriteSend(CBEFile* pFile, CBEFunction* pFunction, CBEContext* pContext)
{
    int nDirection = pFunction->GetSendDirection();
    String sServerID = pContext->GetNameFactory()->GetComponentIDVariable(pContext);
    String sResult = pContext->GetNameFactory()->GetString(STR_RESULT_VAR, pContext);
    String sTimeout;
    if (pFunction->IsComponentSide())
        sTimeout = pContext->GetNameFactory()->GetTimeoutServerVariable(pContext);
    else
        sTimeout = pContext->GetNameFactory()->GetTimeoutClientVariable(pContext);
    String sMsgBuffer = pContext->GetNameFactory()->GetMessageBufferVariable(pContext);
    String sMWord = pContext->GetNameFactory()->GetTypeName(TYPE_MWORD, true, pContext);
	CL4BEMsgBufferType *pMsgBuffer = (CL4BEMsgBufferType*)pFunction->GetMessageBuffer();

    pFile->PrintIndent("l4_ipc_send(*%s,\n", (const char *) sServerID);
    pFile->IncIndent();
    pFile->PrintIndent("");
    bool bVarBuffer = pMsgBuffer->IsVariableSized(nDirection) || (pMsgBuffer->GetAlias()->GetStars() > 0);
    if (pMsgBuffer->HasSendFlexpages())
        pFile->Print("(%s*)((%s)", (const char*)sMWord, (const char*)sMWord);
    if (pMsgBuffer->IsShortIPC(nDirection, pContext, 2))
        pFile->Print("L4_IPC_SHORT_MSG");
    else
    {
        if (bVarBuffer)
            pFile->Print("%s", (const char *) sMsgBuffer);
        else
            pFile->Print("&%s", (const char *) sMsgBuffer);
    }
    if (pMsgBuffer->HasSendFlexpages())
        pFile->Print(")|2)");
    pFile->Print(",\n");

    pFile->PrintIndent("*((%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[0]))),\n");
    pFile->PrintIndent("*((%s*)(&(", (const char*)sMWord);
    pMsgBuffer->WriteMemberAccess(pFile, TYPE_INTEGER, pContext);
    pFile->Print("[4]))),\n");

    pFile->PrintIndent("%s, &%s);\n", (const char *) sTimeout, (const char *) sResult);

    pFile->DecIndent();
}

/** \brief determine if we should use assembler for the IPCs
 *  \param pFunction the function to write the call for
 *  \param pContext the context of the write operation
 *  \return true if assembler code should be written
 *
 * This implementation currently always returns false, because assembler code
 * is always ABI specific.
 */
bool CL4BEIPC::UseAssembler(CBEFunction *pFunction, CBEContext *pContext)
{
    return false;
}

/** \brief helper function to test for short IPC
 *  \param pFunction the function to test
 *  \param pContext the context of the test
 *  \param nDirection the direction to test
 *  \return true if the function uses short IPC in the specified direction
 *
 * This is a simple helper function, which just delegates the call to
 * the function's message buffer.
 */
bool CL4BEIPC::IsShortIPC(CBEFunction *pFunction, CBEContext *pContext, int nDirection)
{
    CL4BEMsgBufferType *pMsgBuf = (CL4BEMsgBufferType*)(pFunction->GetMessageBuffer());
	if (nDirection == 0)
	    return IsShortIPC(pFunction, pContext, pFunction->GetSendDirection()) &&
		       IsShortIPC(pFunction, pContext, pFunction->GetReceiveDirection());
    return pMsgBuf->IsShortIPC(nDirection, pContext);
}