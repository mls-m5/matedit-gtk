/*
 * completeonpopupwindow.cpp
 *
 *  Created on: 24 jan 2015
 *      Author: Mattias Larsson Sköld
 */

#include "completionwindow.h"
#include <iostream>

#include "clangindex.h"
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
	_scrolledWindow.add(_listLayout);

	set_size_request(800, -1);

	show_all_children();
}

bool CompletionWindow::on_key_press_event(GdkEventKey* event)
{
//	cout << "haha keypress" << (char)(event->keyval) << endl;
	auto keyval = event->keyval;
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
	if (ispunct(keyval) or isspace(keyval) or keyval == ':') { //Terminating characters
		auto text = _textEntry.get_text();
		text.push_back(keyval);

		_completedSignal.emit(text);
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
	for (auto it: _listContent) {
		if (it->get_visible()) {
			return it->name();
		}
	}
	return _textEntry.get_text();
}



void CompletionWindow::completeSymbol(ustring currentWord, Document *document, Gsv::Buffer::iterator location) {
	_size = 0;
	_listContent.clear();
	ClangIndex index; //Todo: Cache this in the future

	auto res = index.getCompletion(currentWord, document, location);


	_textEntry.set_text(currentWord);
	_textEntry.select_region(currentWord.size(), currentWord.size());

	for (auto &it: res) {
		addRow(it.completion, it.description);
	}
	textChanged(); //Updating selection
}

void CompletionWindow::addRow(Glib::ustring name, Glib::ustring description) {
	if (_size > 100) {
		return;
	}
	++_size;

	std::shared_ptr<CompletionRow> row(new CompletionRow(name, description));

	_listContent.push_back(row);
	_listLayout.add(*row);
	row->show_all();

}

void CompletionWindow::textChanged() {
	auto text = _textEntry.get_buffer()->get_text();
	for (auto &it: _listContent) {
		auto name = it->name();
		if (name.find(text) == 0) {
//			cout << "comparing " << name << " and " << text << endl;
			it->show();
//			break;
		}
		else {
			it->hide();
		}
	}
}

} /* namespace MatEdit */
