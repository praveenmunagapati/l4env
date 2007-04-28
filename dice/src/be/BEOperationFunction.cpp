/**
 * \file    dice/src/be/BEOperationFunction.cpp
 * \brief   contains the implementation of the class CBEOperationFunction
 *
 * \date    01/14/2002
 * \author  Ronald Aigner <ra3@os.inf.tu-dresden.de>
 */
/*
 * Copyright (C) 2001-2007
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

#include "be/BEOperationFunction.h"
#include "be/BEContext.h"
#include "be/BEAttribute.h"
#include "be/BEType.h"
#include "be/BETypedDeclarator.h"
#include "be/BEDeclarator.h"
#include "be/BERoot.h"
#include "be/BEException.h"
#include "be/BEClass.h"
#include "be/BEMarshaller.h"
#include "Compiler.h"
#include "fe/FEOperation.h"
#include "fe/FETypedDeclarator.h"
#include "Attribute-Type.h"
#include "fe/FEInterface.h"
#include "fe/FEStringAttribute.h"
#include <cassert>

CBEOperationFunction::CBEOperationFunction(FUNCTION_TYPE nFunctionType)
    : CBEFunction(nFunctionType)
{
}

CBEOperationFunction::CBEOperationFunction(CBEOperationFunction & src)
 : CBEFunction(src)
{
}

/** \brief destructor of target class */
CBEOperationFunction::~CBEOperationFunction()
{

}

/** \brief prepares this class for further deployment using the front-end \
 *         operation
 *  \param pFEOperation the respective front.end operation
 *  \return true if the code generation was succesful
 *
 * This implementation adds the attributes, types, parameters, exception, etc.
 * given by the front-end function to this instance of the back-end function.
 */
void 
CBEOperationFunction::CreateBackEnd(CFEOperation * pFEOperation)
{
    assert(pFEOperation);

    string exc = string(__func__);
    // basic init
    CBEFunction::CreateBackEnd(pFEOperation);
    // add return type
    CBENameFactory *pNF = CCompiler::GetNameFactory();
    string sReturn = pNF->GetReturnVariable();
    if (!SetReturnVar(pFEOperation->GetReturnType(), sReturn))
    {
	exc += " failed because return var could not be set.";
        throw new CBECreateException(exc);
    }
    // add attributes
    if (!AddAttributes(pFEOperation))
    {
	exc += " failed because attributes could not be added.";
        throw new CBECreateException(exc);
    }
    // add parameters
    AddParameters(pFEOperation);
    // add exceptions
    if (!AddExceptions(pFEOperation))
    {
	exc += " failed because exceptions could not be added.";
        throw new CBECreateException(exc);
    }
    // set opcode name
    m_sOpcodeConstName = pNF->GetOpcodeConst(pFEOperation);
    // set parent
    CBERoot *pRoot = GetSpecificParent<CBERoot>();
    assert(pRoot);
    assert(pFEOperation->GetSpecificParent<CFEInterface>());
    m_pClass = pRoot->FindClass(
	    pFEOperation->GetSpecificParent<CFEInterface>()->GetName());
    assert(m_pClass);
    // would like to test for class == parent, but this is not the case for
    // switch case: parent = srv-loop function

    // check if interface has error function and add its name if available
    CFEInterface *pFEInterface = 
	pFEOperation->GetSpecificParent<CFEInterface>();
    if (pFEInterface)
    {
        if (pFEInterface->m_Attributes.Find(ATTR_ERROR_FUNCTION))
        {
            CFEStringAttribute *pErrorFunc = dynamic_cast<CFEStringAttribute*>
		(pFEInterface->m_Attributes.Find(ATTR_ERROR_FUNCTION));
            m_sErrorFunction = pErrorFunc->GetString();
        }
        if (pFEInterface->m_Attributes.Find(ATTR_ERROR_FUNCTION_CLIENT) &&
            !IsComponentSide())
        {
            CFEStringAttribute *pErrorFunc = dynamic_cast<CFEStringAttribute*>
		(pFEInterface->m_Attributes.Find(ATTR_ERROR_FUNCTION_CLIENT));
            m_sErrorFunction = pErrorFunc->GetString();
        }
        if (pFEInterface->m_Attributes.Find(ATTR_ERROR_FUNCTION_SERVER) &&
            IsComponentSide())
        {
            CFEStringAttribute *pErrorFunc = dynamic_cast<CFEStringAttribute*>
		(pFEInterface->m_Attributes.Find(ATTR_ERROR_FUNCTION_SERVER));
            m_sErrorFunction = pErrorFunc->GetString();
        }
    }
}

/** \brief adds the parameters of a front-end function to this class
 *  \param pFEOperation the front-end function
 */
void 
CBEOperationFunction::AddParameters(CFEOperation * pFEOperation)
{
    CCompiler::VerboseI(PROGRAM_VERBOSE_NORMAL, 
	"CBEOperationFunction::%s called for %s\n", __func__,
	pFEOperation->GetName().c_str());

    AddBeforeParameters();

    vector<CFETypedDeclarator*>::iterator iter;
    for (iter = pFEOperation->m_Parameters.begin();
	 iter != pFEOperation->m_Parameters.end();
	 iter++)
    {
        AddParameter(*iter);
    }

    AddAfterParameters();

    CCompiler::VerboseD(PROGRAM_VERBOSE_NORMAL,
	"CBEOperationFunction::%s returns true\n", __func__);
}

/** \brief adds a single parameter to this class
 *  \param pFEParameter the parameter to add
 */
void 
CBEOperationFunction::AddParameter(CFETypedDeclarator * pFEParameter)
{
    CBETypedDeclarator *pParameter = 
	CCompiler::GetClassFactory()->GetNewTypedDeclarator();
    m_Parameters.Add(pParameter);
    try
    {
	pParameter->CreateBackEnd(pFEParameter);
    }
    catch (CBECreateException *e)
    {
	m_Parameters.Remove(pParameter);
        delete pParameter;
        throw;
    }
}

/** \brief adds exceptions of a front-end function to this class
 *  \param pFEOperation the front-end function
 *  \return true if successful
 */
bool 
CBEOperationFunction::AddExceptions(CFEOperation * pFEOperation)
{
    CCompiler::Verbose(PROGRAM_VERBOSE_NORMAL, "CBEOperationFunction::%s called for %s\n", __func__,
        pFEOperation->GetName().c_str());
    vector<CFEIdentifier*>::iterator iter;
    for (iter = pFEOperation->m_RaisesDeclarators.begin();
	 iter != pFEOperation->m_RaisesDeclarators.end();
	 iter++)
    {
        if (!AddException(*iter))
            return false;
    }
    CCompiler::Verbose(PROGRAM_VERBOSE_NORMAL, "CBEOperationFunction::%s returns true\n", __func__);
    return true;
}

/** \brief adds a single exception to this class
 *  \param pFEException the exception to add
 *  \return true if successful
 */
bool 
CBEOperationFunction::AddException(CFEIdentifier * pFEException)
{
    CBEException *pException = CCompiler::GetClassFactory()->GetNewException();
    m_Exceptions.Add(pException);
    try
    {
	pException->CreateBackEnd(pFEException);
    }
    catch (CBECreateException *e)
    {
	m_Exceptions.Remove(pException);
        delete pException;
        throw;
    }
    return true;
}

/** \brief adds attributes of a front-end function this this class
 *  \param pFEOperation the front-end operation
 *  \return true if successful
 */
bool 
CBEOperationFunction::AddAttributes(CFEOperation * pFEOperation)
{
    CCompiler::Verbose(PROGRAM_VERBOSE_NORMAL,
	"CBEOperationFunction::%s called for %s\n", __func__,
	pFEOperation->GetName().c_str());
    CBETypedDeclarator *pReturn = GetReturnVariable();
    vector<CFEAttribute*>::iterator iter;
    for (iter = pFEOperation->m_Attributes.begin();
	 iter != pFEOperation->m_Attributes.end();
	 iter++)
    {
        // check if attribute is for function or return type
        ATTR_TYPE nType = (*iter)->GetAttrType();
        switch (nType)
        {
        case ATTR_IDEMPOTENT:
        case ATTR_BROADCAST:
        case ATTR_MAYBE:
        case ATTR_REFLECT_DELETIONS:
        case ATTR_UUID:
        case ATTR_NOOPCODE:
        case ATTR_NOEXCEPTIONS:
        case ATTR_ALLOW_REPLY_ONLY:
        case ATTR_IN:
        case ATTR_OUT:
        case ATTR_STRING:
        case ATTR_CONTEXT_HANDLE:
	case ATTR_SCHED_DONATE:
	case ATTR_DEFAULT_TIMEOUT:
            /* keep attribute */
            if (!AddAttribute(*iter))
                return false;
            break;
        default:
	    try
	    {
		pReturn->AddAttribute(*iter);
	    }
	    catch (CBECreateException *e)
	    {
		e->Print();
		delete e;
                return false;
	    }
            break;
        }
    }
    CCompiler::Verbose(PROGRAM_VERBOSE_NORMAL,
	"CBEOperationFunction::%s returns true\n", __func__);
    return true;
}

/** \brief adds a single attribute to this function
 *  \param pFEAttribute the attribute to add
 *  \return true if successful
 */
bool 
CBEOperationFunction::AddAttribute(CFEAttribute * pFEAttribute)
{
    CBEAttribute *pAttribute = CCompiler::GetClassFactory()->GetNewAttribute();
    m_Attributes.Add(pAttribute);
    try
    {
	pAttribute->CreateBackEnd(pFEAttribute);
    }
    catch (CBECreateException *e)
    {
	m_Attributes.Remove(pAttribute);
        delete pAttribute;
	e->Print();
	delete e;

        return false;
    }
    return true;
}

