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
	void openLocation(ustring fname, int line, int column = 0);
	void openFile(ustring fname) {
		openLocation(fname, -1);
	}
	void closeDocument(Document *document);

	void showResourceDialog();
	void showSelectFileDialog();
	
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
