/*
 * clangindex.h
 *
 *  Created on: 15 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <list>
#include <glibmm.h>
#include <gtksourceviewmm.h>

namespace MatEdit {

using Glib::ustring;
using std::list;
using Glib::RefPtr;

class ClangIndex {
public:
	ClangIndex();
	virtual ~ClangIndex();

	struct CompletionResult {

		ustring completion;
		ustring description;
	};

	list<CompletionResult> getCompletion(ustring word, class Document *, Gsv::Buffer::iterator location);
};

} /* namespace MatEdit */
