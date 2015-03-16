/*
 * Document.cpp
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */



#include "gtksourceviewmm/styleschememanager.h"
#include "gtksourceviewmm/stylescheme.h"
#include <iostream>
#include <fstream>
#include <pangomm-1.4/pangomm.h>

#include "document.h"
#include "mainwindow.h"
#include "clangindex.h"

using Glib::ustring;
using namespace std;


namespace MatEdit {

Document::Document(MainWindow *root, ustring fname):
	_source(this){
	_rootWindow = root;
	_buffer = _source.get_source_buffer();
	if (!_buffer) {
		std::cerr << "Gsv::View::get_source_buffer () failed" << std::endl;
		return;
	}

//	_source.set_wrap_mode(Gtk::WRAP_WORD);
	add(_source);

	setStyle();

	if (not fname.empty()) {
		readFile(fname);
	}

	_buffer->signal_changed().connect(
			sigc::mem_fun(this, &Document::textChanged) );
}



Document::~Document() {
}



void Document::readFile(ustring fname) {
	std::ifstream file(fname);
	if (!file.is_open()) {
		_currentFilename = fname;
		setMode(Mode::Normal);
		_buffer->set_text("");
		_isChanged = false;
		return;
	}

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::string buffer;
	buffer.resize(size);
	if (file.read(&buffer.front(), size))
	{
		_currentFilename = fname;

		_buffer->set_text(buffer);

		setStyle();
	}
	_isChanged = false;
	setMode(Mode::Normal);
}

void Document::writeFile(ustring fname) {
	std::ofstream file(fname);
	file << _buffer->get_text(false);
	_currentFilename = fname;

	setMode(Mode::Normal);
	_isChanged = false;
}

void Document::setMode(Mode m) {
	mode = m;
	if (mode == Mode::Insert) {
		_rootWindow->set_title(_currentFilename + " - matedit - insert mode");
	}
	else {
		_rootWindow->set_title(_currentFilename + " - matedit - normal mode");
	}

}

bool Document::onHandledKeyEvent(GdkEventKey* event)
{

	auto iter = _buffer->get_iter_at_mark(_buffer->get_insert());

	if (event->state & GDK_CONTROL_MASK) {
		if (event->keyval == ' ') {
			_rootWindow->tryComplete(this);

			return true;
		}
		else if (event->keyval == 's') {
			trySave();
			return true;
		}
		else if (event->keyval == 'o') {
			tryOpen();
			return true;
		}
		else if (event->keyval == 'e') {
			_rootWindow->showSelectFileDialog();
			return true;
		}
		else if (event->keyval == 'd') {
			//Delete line

			auto iterBegin = _buffer->get_iter_at_mark(_buffer->get_insert());
			auto iterEnd = _buffer->get_iter_at_mark(_buffer->get_selection_bound());

			iterBegin.set_line_offset(0);
			iterEnd.forward_line();
			_buffer->erase(iterBegin, iterEnd);
			return true;
		}
		else if (event->keyval == 'w') {
			if (trySave()) {
				_rootWindow->closeDocument(this);
				return true;
			}
		}
		else if (event->keyval == GDK_KEY_Tab) {
			toggleHeader();
		}
		else if (event->keyval == 'R') {
			_rootWindow->showResourceDialog();
		}
	}

	if (event->keyval == GDK_KEY_F11) {
		trySave();
		_rootWindow->workspace().makeProject();
	}
	else if(event->keyval == GDK_KEY_F3) {
		_rootWindow->workspace().clangIndex().gotoDefinition(this, iter);
		return true;
	}

	auto getIntendation = [this](Gtk::TextIter iter) {
		string intendation;

		auto iter1 = iter;
		iter1.set_line_offset(0);
		while (isspace(iter1.get_char()) and iter1.get_char() != '\n' and not iter1.is_end()) {
			intendation.push_back(iter1.get_char());
			iter1.forward_char();
		}

		auto iter2 = iter1;
		while (not iter2.is_end() and iter2.get_char() != '\n') {
			if (iter2.get_char() == '{') {
				intendation += "\t";
			}
			iter2.forward_char();
		}

		return intendation;
	};


	auto moveMark = [this](Gtk::TextIter iter) {
		_buffer->move_mark(_buffer->get_selection_bound(), iter);
		_buffer->move_mark(_buffer->get_insert(), iter);
		_source.scroll_to(_buffer->get_insert());
	};


	if (mode == Mode::Insert) {
		if (event->keyval == GDK_KEY_Escape) {
			setMode(Mode::Normal);
			return true;
		}
		else if (event->keyval == GDK_KEY_Return) {
			string intendation = getIntendation(iter);
			_buffer->insert_at_cursor("\n" + intendation);

			return true;
		}
		else if (event->keyval == '}') {
			auto tIter = iter;
			tIter.backward_char();
			if (isspace(tIter.get_char()) and tIter.get_char() != '\n') {
				_buffer->erase(tIter, iter);
			}

		}
		return ScrolledWindow::on_key_press_event(event);
	}
	else {

		switch (event->keyval) {
		case 'j':
		{
			auto col = iter.get_line_offset();
			iter.forward_visible_line();
			iter.set_line_offset(col);
			moveMark(iter);
			break;
		}
		case 'k':
		{
			auto col = iter.get_line_offset();
			iter.backward_visible_line();
			iter.set_line_offset(col);
			moveMark(iter);
			break;
		}
		case 'h':
			iter.backward_char();
			moveMark(iter);
			break;
		case 'l':
			iter.forward_char();
			moveMark(iter);
			break;
		case 'b':
			iter.backward_visible_word_start();
			moveMark(iter);
			break;
		case 'e':
			iter.forward_visible_word_end();
			moveMark(iter);
			break;
		case 'A':
			iter.forward_to_line_end();
			moveMark(iter);
			setMode(Mode::Insert);
			break;
		case 'I':
			iter.set_line_offset(0);
			moveMark(iter);
			setMode(Mode::Insert);
			break;
		case 'a':
			iter.forward_char();
			moveMark(iter);
			setMode(Mode::Insert);
			break;
		case ' ':
			iter.forward_char();
			moveMark(iter);
			break;
		case 'o':
		{
			string intendation = getIntendation(iter);
			while (iter.get_char() != '\n' and not iter.is_end()) {
				iter.forward_char();
			}
			moveMark(iter);
			setMode(Mode::Insert);
			_buffer->insert_at_cursor("\n" + intendation);
			break;
		}

		case 'i':
			setMode(Mode::Insert);
			break;

		}

		return true;
	}
}


void Document::toggleHeader() {
	if (_currentFilename.back() == 'h') {
		auto f = _currentFilename.rfind(".h");
		if (f != string::npos) {
			if (trySave()) {
				_rootWindow->openFile(_currentFilename.substr(0, f) + ".cpp");
			}
		}
	}
	else {
		auto f = _currentFilename.rfind(".cpp");
		if (f != string::npos) {
			if (trySave()) {
				_rootWindow->openFile(_currentFilename.substr(0, f) + ".h");
			}
		}
	}
}


void Document::setStyle() {
	Glib::ustring file_path;
	if (_currentFilename.empty()) {
		file_path = "/home/user/whatever/main.cpp"; //have c++ as test language
	}
	else {
		file_path = _currentFilename;
	}
	auto languageManager = Gsv::LanguageManager::get_default();
	auto language = languageManager->guess_language(file_path, Glib::ustring());

	RefPtr<Gsv::StyleSchemeManager> styleManager;
	RefPtr<Gsv::StyleScheme> styleScheme;

	styleManager = Gsv::StyleSchemeManager::get_default();
	styleScheme = styleManager->get_scheme("oblivion");

	_buffer->set_style_scheme(styleScheme);
	_buffer->set_language(language);
	_buffer->set_highlight_syntax(true);
	_buffer->set_highlight_matching_brackets(true);

	_source.set_tab_width(2);
	_source.set_show_line_numbers(true);

	const string font_description_string("mono, medium, 10");

	_source.override_font(Pango::FontDescription(font_description_string));
	Gdk::RGBA color("red");

	_source.set_auto_indent(true);

}


void Document::tryOpen() {
	Gtk::FileChooserDialog dialog("Please choose a file to open",
			Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*_rootWindow);

	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("Select", Gtk::RESPONSE_OK);

	Glib::RefPtr<Gtk::FileFilter> filter_cpp = Gtk::FileFilter::create();
	filter_cpp->set_name("C/C++ files/Makefiles");
	filter_cpp->add_mime_type("text/x-c");
	filter_cpp->add_mime_type("text/x-c++");
	filter_cpp->add_mime_type("text/x-c-header");
	filter_cpp->add_mime_type("text/x-makefile");
	dialog.add_filter(filter_cpp);

	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	dialog.add_filter(filter_any);

	auto f = _currentFilename.rfind("/");
	if (f != string::npos) {
		dialog.set_current_folder(_currentFilename.substr(0, f));
	}

	int result = dialog.run();

	//Handle the response:
	switch(result)
	{
	case Gtk::RESPONSE_OK:
		std::cout << "Select clicked." << std::endl;
		std::cout << "file selected: " << dialog.get_filename() << std::endl;
		_rootWindow->openFile(dialog.get_filename());
		break;
	case Gtk::RESPONSE_CANCEL:
		std::cout << "Cancel clicked." << std::endl;
		break;
	default:
		std::cout << "Unexpected button clicked." << std::endl;
		break;
	}
}

bool Document::trySave() {
	if (not _currentFilename.empty()) {
		writeFile(_currentFilename);
		return true;
	}
	if (not _isChanged) {
		return true;
	}

	Gtk::FileChooserDialog dialog("Please choose a file",
			Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*_rootWindow);

	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("Select", Gtk::RESPONSE_OK);

	Glib::RefPtr<Gtk::FileFilter> filter_cpp = Gtk::FileFilter::create();
	filter_cpp->set_name("C/C++ files");
	filter_cpp->add_mime_type("text/x-c");
	filter_cpp->add_mime_type("text/x-c++");
	filter_cpp->add_mime_type("text/x-c-header");
	dialog.add_filter(filter_cpp);

	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	dialog.add_filter(filter_any);

	dialog.set_do_overwrite_confirmation(true);

	int result = dialog.run();

	//Handle the response:
	switch(result)
	{
	case Gtk::RESPONSE_OK:
	{
		std::cout << "Select clicked." << std::endl;
		std::cout << "file selected: " << dialog.get_filename() << std::endl;

		std::string fname = dialog.get_filename();
		writeFile(fname);

		break;
	}
	case Gtk::RESPONSE_CANCEL:
		std::cout << "Cancel clicked." << std::endl;
		return true; //Do noth bother about change
	default:
		std::cout << "Unexpected button clicked." << std::endl;
		return false;
	}
	return true;
}

void Document::textChanged() {
	_isChanged = true;
}

} /* namespace MatEdit */

void MatEdit::Document::gotoLocation(int line, int column) {
	auto iter = _buffer->get_iter_at_line(line);
	iter.set_line_offset(column);

	_buffer->move_mark(_buffer->get_selection_bound(), iter);
	_buffer->move_mark(_buffer->get_insert(), iter);
	_source.scroll_to(_buffer->get_insert(), .2);
}
