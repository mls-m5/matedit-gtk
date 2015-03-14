/*
 * completeonpopupwindow.cpp
 *
 *  Created on: 24 jan 2015
 *      Author: Mattias Larsson Sköld
 */

#include "completionwindow.h"
#include <iostream>
#include "completer/rootsourcetree.h"

using std::cout; using std::endl;
using Glib::RefPtr;

namespace MatEdit {

CompletionWindow::CompletionWindow() {
	set_title("completeon");
	set_decorated(false);
	add(_scrolledWindow);
	_scrolledWindow.add(_treeView);
	_refTreeModel = Gtk::ListStore::create(_columns);
	_treeView.set_model(_refTreeModel);
	_treeView.append_column("ID", _columns._colId);
	_treeView.append_column("name", _columns._colName);
	_treeView.append_column("desciption", _columns._colDescription);
	set_size_request(500, 200);
	show_all_children();
}

bool CompletionWindow::on_key_press_event(GdkEventKey* event)
{
	cout << "haha keypress" << (char)(event->keyval) << endl;
	if (event->state & GDK_CONTROL_MASK) {
		cout << "conttrool är nedtryckt" << endl;
	}
	if (event->keyval == GDK_KEY_Escape) {
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
	Glib::RefPtr<Gtk::TreeSelection> selection = _treeView.get_selection();
	Gtk::TreeModel::iterator selectedRow = selection->get_selected();
	Gtk::TreeModel::Row row = *selectedRow;
	Glib::ustring completeonName = row.get_value(_columns._colName);
	return completeonName;
}

void CompletionWindow::populate() {
	_refTreeModel->clear();
	_size = 0;

	addRow("main", "main function");
	for (int i = 0; i < 5; ++i) {
		addRow("apa", "hej");
	}

}

void CompletionWindow::completeSymbol(Glib::ustring name, Glib::RefPtr<Gsv::Buffer>& buffer) {
	_refTreeModel->clear();

	auto sourceTree = RootSourceTree::CreateFromString(buffer->get_text());

	sourceTree.printSymbols(&cout);

	auto res = sourceTree.completeSymbol(name, "", 0);

	for (auto it: res) {
		addRow(it->getLocalName(), it->getFullName());
	}


}

void CompletionWindow::addRow(Glib::ustring name, Glib::ustring description) {
	++_size;

	Gtk::TreeModel::Row row = *(_refTreeModel->append());
	row[_columns._colId] = _size;
	row[_columns._colName] = name;
	row[_columns._colDescription] = description;

	show_all_children();
}

} /* namespace MatEdit */
