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
	auto selectionWindow = new SelectionWindow("open resource");
	FileUtil util;
	util.addFilter(".cpp");
	util.addFilter(".h");
	util.addFilter("makefile");
	util.addFilter("Makefile");


	for (auto &it : _workspace.projectPaths) {
		auto result = util.getAllFilesRecursive(it);
		auto filenamesOnly = result;
		for (auto &it: filenamesOnly) {
			it = FileUtil::stripDirectoryNames(it);
		}
		selectionWindow->addContentList(filenamesOnly, result, result);
	}
	selectionWindow->selectedSignal().connect(
			sigc::mem_fun(this, &MainWindow::openFile) );
	selectionWindow->show_all();
}

void MainWindow::showSelectFileDialog() {
	auto selectionWindow = new SelectionWindow("select open file");

	for (auto it: _documentList) {
		ustring path = it->currentFilename();
		ustring shortFilename = FileUtil::stripDirectoryNames(path);
		selectionWindow->addContent(shortFilename, path, path);
	}

	selectionWindow->selectedSignal().connect(
			sigc::mem_fun(this, &MainWindow::openFile) );
	selectionWindow->show_all();

}




void MainWindow::onCompletionFinished(Glib::ustring completeon) {
	if (not _currentDocument) return;
	cout << "mainwindow incomming completion: " << completeon << endl;

	auto currentView = _currentDocument->sourceView();
	auto buffer = currentView->get_buffer();

	auto end = buffer->get_iter_at_mark(buffer->get_insert());

	auto iter = end;
	if (*iter != ' ') {

		while (iter != buffer->begin()) {
			auto c = buffer->get_text(iter, end, false)[0];
//			auto c = iter.get_char(); //does not work... dont know just why
			if (c == ' ' or c == '\t' or c == '\n' or c == '.' or c == ':'){
				++iter;
				break;
			}
			--iter;

		}
	}
	buffer->erase(iter, end);
	buffer->insert_at_cursor(completeon);

	_currentDocument->setMode(Document::Mode::Insert);
}




void MainWindow::tryComplete(Document* document) {
	cout << "completion" << endl;
	_currentDocument = document;
	auto currentView = document->sourceView();
	auto buffer = currentView->get_source_buffer();

	auto end = buffer->get_iter_at_mark(buffer->get_insert());

	auto iter = end;
	if (*iter != ' ') {

		while (iter != buffer->begin()) {
			auto c = buffer->get_text(iter, end, false)[0];
//			auto c = iter.get_char(); //Dont work
			if (c == ' ' or c == '\t' or c == '\n' or c == ':' or c == '.'){
				++iter;
				break;
			}
			--iter;
		}

		Gdk::Rectangle location;
		currentView->get_iter_location(end, location);
		int x, y;
		currentView->buffer_to_window_coords(Gtk::TextWindowType::TEXT_WINDOW_WIDGET, location.get_x(), location.get_y() + location.get_height(), x, y);
		int winX, winY;
		this->get_position(winX, winY);
		x += winX;
		y += winY;

		auto currentWord = buffer->get_text(iter, end);
//		_completeWindow.completeSymbol(text, buffer, end);
		_completeWindow.move(x, y + 40); //This depends on decoration
		_completeWindow.hide(); //Move to front
		_completeWindow.show_all();
		_completeWindow.completeSymbol(currentWord, document, iter);
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
	int pageNum =_notebookLayout.append_page(*document, title);

	document->show_all();

	_notebookLayout.set_current_page(pageNum);
}




void MatEdit::MainWindow::openLocation(ustring fname, int line, int column) {
	int i = 0;
	for (auto it: _notebookLayout.get_children()) {
		auto document = dynamic_cast<Document*>(it);
		if (document->currentFilename() == fname) {
			_notebookLayout.set_current_page(i);
			if (line > -1) {
				document->gotoLocation(line, column);
			}
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
	if (_currentDocument == document) {
		_currentDocument = nullptr;
	}
}
