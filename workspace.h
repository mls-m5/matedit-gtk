/*
 * workspace.h
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include <list>
#include <glibmm.h>

namespace MatEdit {

class Workspace {
public:
	Workspace();
	virtual ~Workspace();

	void makeProject();

	std::list<Glib::ustring> projectPaths;

};

} /* namespace MatEdit */
