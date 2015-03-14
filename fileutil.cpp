/*
 * FileUtil.cpp
 *
 *  Created on: 14 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#include "fileutil.h"
#include "dirent.h"
#include <string>

using Glib::ustring;
using std::string;

namespace MatEdit {

FileUtil::FileUtil() {
}

FileUtil::~FileUtil() {
}

std::list<ustring> FileUtil::getAllFilesRecursive(ustring path) {
	DIR *directory;
	std::list<ustring> result;
	struct dirent *ent;
	if ((directory = opendir (path.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (directory)) != NULL) {
//			printf ("%s\n", ent->d_name);
			if (isFilteredFile(ent->d_name)) {
				result.push_back(path + ent->d_name);
			}
		}
		closedir (directory);
		return result;
	} else {
		/* could not open directory */
		return {};
	}
}

bool FileUtil::isFilteredFile(Glib::ustring path) {
	if (path == ".." or path == ".") {
		return false;
	}
	if (!_filters.empty()) {
		for (auto &it: _filters) {
			if (path.find(it) != string::npos) {
				return true; //Match a filter
			}
		}
		return false;
	}

	return true;
}

} /* namespace MatEdit */
