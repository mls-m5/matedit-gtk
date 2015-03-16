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
	//Note: the window deletes itself when hidden
	SelectionWindow(ustring title = "selection");
	virtual ~SelectionWindow();


	Gtk::VBox _layout;
	Gtk::Entry _textEntry;
	Gtk::ListBox _listlayout;

	class RowContent: public Gtk::HBox {
	public:
		RowContent(ustring content, ustring description, ustring response) :
			_contentString(content),
			_contentLabel(content),
			_contentDescriptionLabel(description),
			_contentDescription(description),
			_response(response)
			{
			_contentString = content;

			_contentLabel.set_alignment(0,.5); //Text to the left
			_contentLabel.set_margin_right(5);
			_contentDescriptionLabel.set_alignment(1,.5); //Text to the right
			add (_contentLabel);
			add (_contentDescriptionLabel);
			show_all();
		}

		const ustring &contentString() {
			return _contentString;
		}

		const ustring &contentDescription() {
			return _contentDescription;
		}

		const ustring &response() {
			return _response;
		}

	protected:
		ustring _contentString;
		ustring _contentDescription;
		ustring _response;

		Gtk::Label _contentLabel;
		Gtk::Label _contentDescriptionLabel;
	};

	template <class T>
	void setContent(T list) {
		_contentList.clear();
		for (auto &it: list) {
			addContent(it);
		}
	}


	template <class T>
	void addContentList(T list) {
		for (auto &it: list) {
			addContent(it, "", it);
		}
	}

	template <class C, class D, class R>
	void addContentList(C contentList, D descriptionList, R responseList) {
		auto dit = descriptionList.begin();
		auto rit = responseList.begin();
		for (auto &it: contentList) {
			if (dit == descriptionList.end()) {
				break;
			}
			addContent(it, *dit, *rit);
			++dit;
			++rit;
		}
	}

	//Add content to be added
	//content is the main content
	//description is a string that is shown together with the content
	//response is the string that is sent when a row is selected
	void addContent(ustring content, ustring description, ustring response);
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
