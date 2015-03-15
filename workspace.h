/*
 * workspace.h
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include <list>
#include <glibmm.h>
#include <memory>

#include "console.h"
namespace MatEdit {

class Workspace {
public:
	Workspace();
	virtual ~Workspace();

	void makeProject();
	void showConsole();

	std::list<Glib::ustring> projectPaths;

	Console _console;


	std::unique_ptr<class ClangIndex> clangIndex;

};

} /* namespace MatEdit */
