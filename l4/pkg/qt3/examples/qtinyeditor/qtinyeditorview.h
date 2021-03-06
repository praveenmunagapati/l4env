/***************************************************************************
                          qtinyeditorview.h  -  description
                             -------------------
    begin                : Tue Aug 19 20:33:02 Local time zone must be set--see zic manual page 2003
    copyright            : (C) 2003 by Fran�ois Dupoux
    email                : fdupoux@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QTINYEDITORVIEW_H
#define QTINYEDITORVIEW_H

// include files for Qt
//#include <qwidget.h>
#include <qtextedit.h>

class QTinyEditorDoc;

/**
 * This class provides an incomplete base for your application view.
 * 	
 * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
 * @version KDevelop version 1.1 code generation
 */
class QTinyEditorView : public QTextEdit //QWidget
{
  Q_OBJECT

  friend class QTinyEditorDoc;

  public:
    /** Constructor for the view
    	* @param pDoc  your document instance that the view represents. Create a document before calling the constructor
    	* or connect an already existing document to a new MDI child widget.*/
    QTinyEditorView(QTinyEditorDoc* pDoc, QWidget* parent, const char *name, int wflags);
    /** Destructor for the main view */
    ~QTinyEditorView();
		/** returns a pointer to the document connected to the view*/
    QTinyEditorDoc *getDocument() const;
    /** gets called to redraw the document contents if it has been modified */
		void update(QTinyEditorView* pSender);
    /** contains the implementation for printing functionality and gets called by QTinyEditorApp::slotFilePrint() */
    //void print(QPrinter *pPrinter);
		
  protected:
	  virtual void closeEvent(QCloseEvent*);

	  QTinyEditorDoc *doc;

	public slots:
	   void slotTextChanged();
	
  private:
	
};

#endif
