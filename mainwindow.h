/*
 * MainWindow.h
 *
 *  Created on: 24 jan 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <gtkmm.h>
#include <gtksourceviewmm.h>
#include "completionwindow.h"
#include "document.h"
#include <memory>

#include "workspace.h"
namespace MatEdit {

using Glib::RefPtr;
using std::shared_ptr;
using Glib::ustring;

class MainWindow: public Gtk::Window {
public:

	MainWindow();

	virtual ~MainWindow() {
	}


	void tryComplete(Document *);

	void newDocument(ustring fname);

	void openFile(ustring fname);
	void closeDocument(Document *document);

	void showResourceDialog();
	
	Workspace &workspace() {
		return _workspace;
	}


protected:

	Gtk::Notebook _notebookLayout;

	void onCompletionFinished(Glib::ustring completeon);
	CompletionWindow _completeWindow;
	Document *_currentDocument;
	std::list<shared_ptr<Document>> _documentList;

	Workspace _workspace;
};



} /* namespace MatEdit */
