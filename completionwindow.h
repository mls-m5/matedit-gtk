/*
 * completeonpopupwindow.h
 *
 *  Created on: 24 jan 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <gtksourceviewmm.h>
#include <gtkmm.h>

namespace MatEdit {

using Glib::ustring;


class CompletionWindow: public Gtk::Window {
public:
	CompletionWindow();
	virtual bool on_key_press_event(GdkEventKey* event) override;
	Glib::ustring getCompletionResult();

	//Tree model columns:
	class ModelColumns: public Gtk::TreeModel::ColumnRecord {
	public:
		ModelColumns() {
			add (_colId);
			add (_colName);
			add (_colDescription);
		}

		Gtk::TreeModelColumn<unsigned int> _colId;
		Gtk::TreeModelColumn<Glib::ustring> _colName;
		Gtk::TreeModelColumn<Glib::ustring> _colDescription;
		Gtk::TreeModelColumn<bool> _visible;
	};

	sigc::signal<void, Glib::ustring> completedSignal() {
		return _completedSignal;
	}

	void populate();
	void completeSymbol(Glib::ustring name, Glib::RefPtr<Gsv::Buffer>& buffer, Gsv::Buffer::iterator location);
	void completeSymbol(ustring currentWord, class Document *document, Gsv::Buffer::iterator location);
	void addRow(Glib::ustring name, Glib::ustring description);

	protected:

	void textChanged();

	ModelColumns _columns;

	Gtk::VBox _layout;
	Gtk::Entry _textEntry;
	Gtk::ScrolledWindow _scrolledWindow;
	Gtk::TreeView _treeView;
	Glib::RefPtr<Gtk::ListStore> _treeModel;

	sigc::signal<void, Glib::ustring> _completedSignal;
	int _size = 0;
};

} /* namespace MatEdit */
