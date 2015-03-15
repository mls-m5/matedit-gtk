/*
 * consol.h
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include <gtkmm.h>

namespace MatEdit {

using Glib::RefPtr;

class Console: public Gtk::Window {
public:
	Console();
	virtual ~Console();


	void addText(Glib::ustring text);
	void clear() {
		_text.clear();
		_buffer->set_text(_text);
	}

	void executeCommand(Glib::ustring cmd);

	Gtk::TextView _textView;
	Glib::ustring _text;
	RefPtr<Gtk::TextBuffer> _buffer;
};

} /* namespace MatEdit */
