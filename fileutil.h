/*
 * FileUtil.h
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include <list>
#include <glibmm.h>

namespace MatEdit {
using Glib::ustring;

class FileUtil {

public:
	FileUtil();
	virtual ~FileUtil();

	std::list<ustring> getAllFilesRecursive(ustring path);
	ustring stripDirectoryNames(ustring path);

	bool isFilteredFile(Glib::ustring path);

	void addFilter(Glib::ustring filter) {
		_filters.push_back(filter);
	}

protected:

	std::list<Glib::ustring> _filters;
};

} /* namespace MatEdit */
