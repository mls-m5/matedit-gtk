/*
 * consol.cpp
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#include "console.h"
#include <cstdio>

using Glib::ustring;

namespace MatEdit {

Console::Console() {
	add(_textView);
	_buffer = _textView.get_buffer();

	set_title("console output");
	_textView.set_size_request(200, 200);
}

Console::~Console() {
}

void Console::addText(Glib::ustring text) {
	_text += text;
	_buffer->set_text(_text);
}

void Console::executeCommand(Glib::ustring cmd) {
	auto command = cmd + " 2>&1"; //Make sure to get the error output as well
	auto pipe = popen(command.c_str(), "r");
	if (!pipe) {
		addText("failed: " + cmd + "\n");
		return;
	}
	char buffer[4];
	std::string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, 4, pipe) != NULL)
		{
			addText(buffer);
		}
	}
	pclose(pipe);
}

} /* namespace MatEdit */
