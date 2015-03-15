/*
 * completeonpopupwindow.h
 *
 *  Created on: 24 jan 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <gtksourceviewmm.h>
#include <gtkmm.h>
#include <memory>

namespace MatEdit {

using Glib::ustring;
using Glib::RefPtr;


class CompletionWindow: public Gtk::Window {
public:
	CompletionWindow();
	virtual bool on_key_press_event(GdkEventKey* event) override;
	Glib::ustring getCompletionResult();

	//Tree model columns:

	sigc::signal<void, Glib::ustring> completedSignal() {
		return _completedSignal;
	}

	class CompletionRow: public Gtk::HBox {
	public:
		CompletionRow(ustring name, ustring description):
			_name(name),
			_description(description),
			_nameLabel(name),
			_descriptionLabel(description){
			add(_nameLabel);
			add(_descriptionLabel);
			_nameLabel.set_alignment(0, .5);
		}

		ustring &name() {
			return _name;
		}

	protected:
		ustring _name;
		ustring _description;

		Gtk::Label _nameLabel;
		Gtk::Label _descriptionLabel;
	};

	void completeSymbol(Glib::ustring name, Glib::RefPtr<Gsv::Buffer>& buffer, Gsv::Buffer::iterator location);
	void completeSymbol(ustring currentWord, class Document *document, Gsv::Buffer::iterator location);
	void addRow(Glib::ustring name, Glib::ustring description);

	protected:

	void textChanged();

	Gtk::VBox _layout;
	Gtk::Entry _textEntry;
	Gtk::ScrolledWindow _scrolledWindow;
	Gtk::VBox _listLayout;
	std::list<std::shared_ptr<CompletionRow>> _listContent;

	sigc::signal<void, Glib::ustring> _completedSignal;
	int _size = 0;
};

} /* namespace MatEdit */
