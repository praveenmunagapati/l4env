/****************************************************************************
** $Id$
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <unistd.h>

#include <qapplication.h>
#include "application.h"

int main( int argc, char ** argv ) {

    // wait a bit for simple_file_server to get ready
    sleep(2);

    QApplication a( argc, argv );
    ApplicationWindow *mw = new ApplicationWindow();
    mw->setCaption( "Qt Example - Application" );
    mw->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    return a.exec();
}