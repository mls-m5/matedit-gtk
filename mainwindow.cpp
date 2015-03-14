/*
 * MainWindow.cpp
 *
 *  Created on: 24 jan 2015
 *      Author: Mattias Larsson Sköld
 */

#include "mainwindow.h"
#include <iostream>
#include <gdkmm-3.0/gdkmm/rectangle.h>

#include "selectionwindow.h"
#include "document.h"
#include "fileutil.h"

using namespace std;

namespace MatEdit {


MainWindow::MainWindow(){
	set_size_request(400, 400);

	_completeWindow.completedSignal().connect(
			sigc::mem_fun(this, &MainWindow::onCompletionFinished) );

	add(_notebookLayout);
	newDocument("");

	show_all();
}

void MainWindow::showResourceDialog() {
	auto w = new SelectionWindow();
	FileUtil util;
	util.addFilter(".cpp");
	util.addFilter(".h");
	util.addFilter("makefile");
	util.addFilter("Makefile");


	for (auto &it : _workspace.projectPaths) {
		auto result = util.getAllFilesRecursive(it);
		w->addContent(result);
	}
	w->selectedSignal().connect(
			sigc::mem_fun(this, &MainWindow::openFile) );
	w->show_all();
}




void MainWindow::onCompletionFinished(Glib::ustring completeon) {
	if (not _currentView) return;
	cout << "mainwindow incomming completeon" << completeon << endl;

	auto buffer = _currentView->get_buffer();


	auto end = buffer->get_iter_at_mark(buffer->get_insert());

	auto iter = end;
	if (*iter != ' ') {

		while (iter != buffer->begin()) {
			auto c = buffer->get_text(iter, end, false)[0];
			if (c == ' ' or c == '\t' or c == '\n'){
				++iter;
				break;
			}
			--iter;

		}
	}
	buffer->erase(iter, end);
	buffer->insert_at_cursor(completeon);
}




void MainWindow::tryComplete(Gsv::View* sourceView) {
	cout << "completion" << endl;
	_currentView = sourceView;
	RefPtr<Gsv::Buffer> buffer = sourceView->get_source_buffer();

	auto end = buffer->get_iter_at_mark(buffer->get_insert());

	auto iter = end;
	if (*iter != ' ') {

		while (iter != buffer->begin()) {
			auto c = buffer->get_text(iter, end, false)[0];
			if (c == ' ' or c == '\t' or c == '\n'){
				++iter;
				break;
			}
			--iter;
		}

		Gdk::Rectangle location;
		sourceView->get_iter_location(end, location);
		int x, y;
		sourceView->buffer_to_window_coords(Gtk::TextWindowType::TEXT_WINDOW_WIDGET, location.get_x(), location.get_y() + location.get_height(), x, y);
		int winX, winY;
		this->get_position(winX, winY);
		x += winX;
		y += winY;

		auto text = buffer->get_text(iter, end);
		_completeWindow.completeSymbol(text, buffer);
		_completeWindow.move(x, y + 40); //This depends on decoration
		_completeWindow.hide();
		_completeWindow.show_all();
	}
}

} /* namespace MatEdit */




void MatEdit::MainWindow::newDocument(Glib::ustring fname) {
	shared_ptr<Document> document(new Document(this, fname));
	_documentList.push_back(document);
	ustring title = "unnamed";
	if (!fname.empty()) {
		title = fname;
		auto f = title.rfind('/');
		if (f != string::npos and f < title.size()) {
			title = title.substr(f + 1, title.size() - f + 1);
		}
	}
	_notebookLayout.append_page(*document, title);

	document->show_all();

	_notebookLayout.set_current_page(_notebookLayout.get_n_pages() - 1);
}




void MatEdit::MainWindow::openFile(ustring fname) {
	int i = 0;
	for (auto it: _notebookLayout.get_children()) {
		auto document = dynamic_cast<Document*>(it);
		if (document->currentFilename() == fname) {
			_notebookLayout.set_current_page(i);
			return;
		}
		++i;
	}
	newDocument(fname);
}




void MatEdit::MainWindow::closeDocument(Document* document) {
	for(auto it = _documentList.begin(); it != _documentList.end(); ++it) {
		if (&(**it) == document) {
			_documentList.erase(it);
			return;
		}
	}
	if (_currentView == document->sourceView()) {
		_currentView = nullptr;
	}
}
