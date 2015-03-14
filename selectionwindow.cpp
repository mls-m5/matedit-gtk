/*
 * SelectionWindow.cpp
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#include "selectionwindow.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace MatEdit {


SelectionWindow::SelectionWindow() {

	_layout.add(_textEntry);
	_layout.add(_listlayout);

	_textEntry.signal_changed().connect(
			sigc::mem_fun(this, &SelectionWindow::textChanged) );

	auto delFun = [this] () {
		delete this;
	};
	signal_hide().connect(delFun);

	add(_layout);

	set_modal(true);
    set_keep_above(true);

	show_all();

}

SelectionWindow::~SelectionWindow() {
}

} /* namespace MatEdit */

void MatEdit::SelectionWindow::addContent(ustring content) {
	std::shared_ptr<RowContent> label(new RowContent(content));

	_contentList.push_back(label);
	_listlayout.add(*label);
}

void MatEdit::SelectionWindow::textChanged() {
	auto text = _textEntry.get_text();
	for (auto it: _contentList) {
		if (it) {
			if (it->contentString().find(text) == string::npos) {
				it->hide();
			}
			else {
				it->show();
			}
		}
	}
}

bool MatEdit::SelectionWindow::on_key_press_event(GdkEventKey* event) {
	if (event->keyval == GDK_KEY_Escape) {
		hide();
		return true;
	}
	else if(event->keyval == GDK_KEY_Return) {
		selectCurrent();
		return true;
	}
	return Window::on_key_press_event(event);
}


void MatEdit::SelectionWindow::selectCurrent() {
	for (auto it: _contentList) {
		//select the first
		if (it->is_visible()) {
			cout << "valde " << it->contentString() << endl;
			_selectedSignal.emit(it->contentString());
			hide();
			return;
		}
	}
}
