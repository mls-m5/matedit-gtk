/*
 * completeonpopupwindow.cpp
 *
 *  Created on: 24 jan 2015
 *      Author: Mattias Larsson Sköld
 */

#include "completionwindow.h"
#include <iostream>

#include "clangindex.h"
#include "completer/rootsourcetree.h"
#include "document.h"

using std::cout; using std::endl;
using Glib::RefPtr;

namespace MatEdit {

CompletionWindow::CompletionWindow()
		{
	set_title("completeon");
	set_decorated(false);
	add(_layout);
	_layout.add(_textEntry);

	_textEntry.signal_changed().connect(
			sigc::mem_fun(this, &CompletionWindow::textChanged) );
	_layout.add(_scrolledWindow);
	_scrolledWindow.add(_treeView);
	_treeModel = Gtk::ListStore::create(_columns);
//	_treeModel = Gtk::TreeModelFilter::create(tmpModel);
	_treeView.set_model(_treeModel);
	_treeView.append_column("ID", _columns._colId);
	_treeView.append_column("name", _columns._colName);
	_treeView.append_column("desciption", _columns._colDescription);

	_treeView.

	set_size_request(500, 200);
	show_all_children();
}

bool CompletionWindow::on_key_press_event(GdkEventKey* event)
{
//	cout << "haha keypress" << (char)(event->keyval) << endl;
	if (event->state & GDK_CONTROL_MASK) {
		cout << "conttrool är nedtryckt" << endl;
	}
	if (event->keyval == GDK_KEY_Escape) {
		auto text = _textEntry.get_text();
		if (!text.empty()) {
			_completedSignal.emit(text);
		}
		hide();
	}
	if (event->keyval == GDK_KEY_Return) {
		hide();
		cout << "use completeon" << getCompletionResult() << endl;
		_completedSignal.emit(getCompletionResult());
	}
	return Gtk::Window::on_key_press_event(event);
}

Glib::ustring CompletionWindow::getCompletionResult() {
	auto selection = _treeView.get_selection();
	auto selectedRowIterator = selection->get_selected();
	auto row = *selectedRowIterator;
	Glib::ustring completeonName = row.get_value(_columns._colName);
	return completeonName;
}

void CompletionWindow::populate() {
	_treeModel->clear();
	_size = 0;

//	addRow("main", "main function");
//	for (int i = 0; i < 5; ++i) {
//		addRow("apa", "hej");
//	}

}

void CompletionWindow::completeSymbol(Glib::ustring name, Glib::RefPtr<Gsv::Buffer>& buffer, Gsv::Buffer::iterator location) {
	_treeModel->clear();

	auto sourceTree = RootSourceTree::CreateFromString(buffer->get_text());

	sourceTree.printSymbols(&cout);

	auto res = sourceTree.completeSymbol(name, "", 0);

	for (auto it: res) {
		addRow(it->getLocalName(), it->getFullName());
	}

//	ClangIndex index; //Todo: Cache this in the future
//
//	auto res = index.getCompletion(buffer, location);
//
//	for (auto &it: res) {
//		addRow(it, "beskrivning");
//	}
}


void CompletionWindow::completeSymbol(ustring currentWord, Document *document, Gsv::Buffer::iterator location) {
	_size = 0;
	_treeModel->clear();
	ClangIndex index; //Todo: Cache this in the future

	auto res = index.getCompletion(currentWord, document, location);


	_textEntry.set_text(currentWord);
//	_textEntry.set_position(currentWord.size());
	_textEntry.select_region(currentWord.size(), currentWord.size());
	textChanged(); //Updating selection



	for (auto &it: res) {
		addRow(it.completion, it.description);
	}
}

void CompletionWindow::addRow(Glib::ustring name, Glib::ustring description) {
	++_size;

	Gtk::TreeModel::Row row = *(_treeModel->append());
	row[_columns._colId] = _size;
	row[_columns._colName] = name;
	row[_columns._colDescription] = description;
	row[_columns._visible] = true;


	show_all_children();
}

void CompletionWindow::textChanged() {
	auto children = _treeModel->children();
	auto text = _textEntry.get_buffer()->get_text();
	for (auto &it: children) {
		auto name = it.get_value(_columns._colName);
		if (name.find(text) == 0) {
			cout << "comparing " << name << " and " << text << endl;
			_treeView.get_selection()->select(it);
			break;
		}
	}
}

} /* namespace MatEdit */
