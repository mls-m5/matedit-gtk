/*
 * workspace.cpp
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#include "workspace.h"
#include <iostream>

using namespace std;

namespace MatEdit {

Workspace::Workspace() {
	projectPaths.push_back("/home/mattias/testma/");
}

Workspace::~Workspace() {
}

void Workspace::makeProject() {
	showConsole();
	auto path = projectPaths.front();
	auto f = path.rfind("/");
	std::string dirName;
	if (f == std::string::npos) {
		system("make");
	}
	else {
		dirName = path.substr(0, f);
		auto cmd = "make -C '" + dirName + "'";

		cout << "Building" << endl;
		cout << " --> " << cmd << endl;

		_console.clear();
		_console.addText("building \n" + cmd + "\n ------- \n");
		_console.executeCommand(cmd);
	}
}

} /* namespace MatEdit */

void MatEdit::Workspace::showConsole() {
	_console.show_all();
}
