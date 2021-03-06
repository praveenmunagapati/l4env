/****************************************************************************
** $Id$
**
** Definition of QKbdDriverPlugin
**
** Created : 20020218
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Qt/Embedded may use this file in accordance with the
** Qt Embedded Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QKBDDRIVERPLUGIN_QWS_H
#define QKBDDRIVERPLUGIN_QWS_H

#ifndef QT_H
#include "qgplugin.h"
#include "qstringlist.h"
#endif // QT_H

#ifndef QT_NO_COMPONENT

class QWSKeyboardHandler;
class QKbdDriverPluginPrivate;

class Q_EXPORT QKbdDriverPlugin : public QGPlugin
{
    Q_OBJECT
public:
    QKbdDriverPlugin();
    ~QKbdDriverPlugin();

#ifndef QT_NO_STRINGLIST
    virtual QStringList keys() const = 0;
#endif
    virtual QWSKeyboardHandler* create( const QString& driver, const QString &device ) = 0;

private:
    QKbdDriverPluginPrivate *d;
};

#endif // QT_NO_COMPONENT

#endif // QKBDDRIVERPLUGIN_QWS_H
