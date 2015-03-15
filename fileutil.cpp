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

std::list<ustring> FileUtil::getAllFilesRecursive(ustring path, int depth) {
	if (depth > 10) {
		return {};
	}
	DIR *directory;
	std::list<ustring> result;
	struct dirent *ent;
	if ((directory = opendir (path.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (directory)) != NULL) {
//			printf ("%s\n", ent->d_name);
			if (ent->d_type == DT_DIR) {
				ustring name = ent->d_name;
				if (name == ".." or name == ".") {
					continue;
				}
				auto subResult = getAllFilesRecursive(path + "/" + name + "/", depth + 1);
				result.splice(result.end(), subResult);

			}
			else if (isFilteredFile(ent->d_name)) {
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

ustring FileUtil::stripDirectoryNames(ustring path) {
	auto f = path.rfind('/');
	if (f != string::npos and f < path.size()) {
		return path.substr(f + 1, path.size() - f - 1);
	}
	return path;
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
