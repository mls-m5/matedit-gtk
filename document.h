/*
 * Document.h
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <gtksourceviewmm.h>
#include "completionwindow.h"
#include <memory>

namespace MatEdit {

using Glib::RefPtr;
using Glib::ustring;

class Document: public Gtk::ScrolledWindow, public std::enable_shared_from_this<Document> {
public:
	Document(class MainWindow *root, ustring fname = "");
	virtual ~Document();

	//handle keypress from the window

	bool beforeKeyPress(GdkEventKey* event);

	bool onHandledKeyEvent(GdkEventKey* event);

	bool trySave();
	void tryOpen();

	void readFile(Glib::ustring fname);
	void writeFile(Glib::ustring fname);

	ustring currentFilename() {
		return _currentFilename;
	}

	Gsv::View *sourceView() {
		return &_source;
	}
	RefPtr<Gsv::Buffer> buffer() {

		return _buffer;
	}

protected:


	enum class Mode {
		Normal,
		Insert
	} mode = Mode::Normal;

	void setMode(Mode m);

	void toggleHeader();

	std::string _currentFilename = "";

	void setStyle();

	//Signal callback
	void textChanged();

	struct HandledView: public Gsv::View {
	public:
		HandledView(Document* parent): _parent(parent) {

		}

		bool on_key_press_event(GdkEventKey* event) override {
			if (_parent->onHandledKeyEvent(event)) {
				return true;
			}
			else {
				View::on_key_press_event(event);
			}
			return true; //Prevent key-propagation
		}

		Document *_parent;
	}
	 _source;
	Gtk::ScrolledWindow _scrolledWindow;

	RefPtr<Gsv::Buffer> _buffer;

	class MainWindow* _rootWindow;

	bool _isChanged = false;
};

} /* namespace MatEdit */
