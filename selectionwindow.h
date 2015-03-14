/*
 * SelectionWindow.h
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include <memory>
#include <gtkmm.h>
#include <list>

using Glib::Cond;

namespace MatEdit {

using Glib::ustring;

class SelectionWindow: public Gtk::Window {
public:
	SelectionWindow();
	virtual ~SelectionWindow();


	Gtk::VBox _layout;
	Gtk::Entry _textEntry;
	Gtk::ListBox _listlayout;

	class RowContent: public Gtk::Label {
	public:
		RowContent(ustring content) : Label(content) {
			_contentString = content;
		}

		ustring contentString() {
			return _contentString;
		}

	protected:
		ustring _contentString;
	};

	template <class T>
	void setContent(T list) {
		_contentList.clear();
		for (auto &it: list) {
			addContent(it);
		}
	}


	template <class T>
	void addContent(T list) {
		for (auto &it: list) {
			addContent(it);
		}
	}

	void addContent(ustring content);
	void selectCurrent();

	sigc::signal<void, Glib::ustring> &selectedSignal() {
		return _selectedSignal;
	}

	protected:
	std::list<std::shared_ptr<RowContent>> _contentList;



	bool on_key_press_event(GdkEventKey* event) override;


	//Signal callback
	void textChanged();

	sigc::signal<void, Glib::ustring> _selectedSignal;


};

} /* namespace MatEdit */
