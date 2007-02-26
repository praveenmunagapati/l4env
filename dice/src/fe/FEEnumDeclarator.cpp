/**
 *	\file	dice/src/fe/FEEnumDeclarator.cpp
 *	\brief	contains the implementation of the class CFEEnumDeclarator
 *
 *	\date	06/08/2001
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

#include "fe/FEEnumDeclarator.h"
#include "fe/FEExpression.h"
#include "File.h"

IMPLEMENT_DYNAMIC(CFEEnumDeclarator) 

CFEEnumDeclarator::CFEEnumDeclarator(CFEEnumDeclarator & src)
:CFEDeclarator(src)
{
    IMPLEMENT_DYNAMIC_BASE(CFEEnumDeclarator, CFEDeclarator);

    m_pInitialValue = (CFEExpression *) (src.m_pInitialValue->Clone());
}

CFEEnumDeclarator::CFEEnumDeclarator()
:CFEDeclarator(DECL_ENUM, String())
{
    IMPLEMENT_DYNAMIC_BASE(CFEEnumDeclarator, CFEDeclarator);

    m_pInitialValue = 0;
}

CFEEnumDeclarator::CFEEnumDeclarator(String sName, CFEExpression * pInitialValue)
:CFEDeclarator(DECL_ENUM, sName)
{
    IMPLEMENT_DYNAMIC_BASE(CFEEnumDeclarator, CFEDeclarator);

    m_pInitialValue = pInitialValue;
    if (m_pInitialValue)
	m_pInitialValue->SetParent(this);
}

/** cleans up the declarator object */
CFEEnumDeclarator::~CFEEnumDeclarator()
{
    if (m_pInitialValue)
	delete m_pInitialValue;
}

/**	\brief creates a copy of this object
 *	\return a reference to a new object
 */
CObject *CFEEnumDeclarator::Clone()
{
    return new CFEEnumDeclarator(*this);
}

/** serializes this object
 *	\param pFile the file to serialize to/from
 */
void CFEEnumDeclarator::Serialize(CFile * pFile)
{
    if (pFile->IsStoring())
    {
        pFile->PrintIndent("<enum_declarator>\n");
        pFile->IncIndent();
        pFile->PrintIndent("<name>%s</name>\n", (const char *) GetName());
        if (m_pInitialValue)
        {
            pFile->PrintIndent("<value>\n");
            m_pInitialValue->Serialize(pFile);
            pFile->PrintIndent("</value>\n");
        }
        pFile->DecIndent();
        pFile->PrintIndent("</enum_declarator>\n");
    }
}

/** \brief retrieves a reference to the initial value of the enum
 *  \return a reference to the initial value of the enum
 */
CFEExpression* CFEEnumDeclarator::GetInitialValue()
{
    return m_pInitialValue;
}
