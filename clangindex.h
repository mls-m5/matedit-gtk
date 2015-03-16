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
#include <memory>

#include <clang-c/Index.h>


namespace MatEdit {

using Glib::ustring;
using std::list;
using Glib::RefPtr;

class ClangIndexData;

class ClangIndex {
public:
	ClangIndex();
	virtual ~ClangIndex();

	struct CompletionResult {

		ustring completion;
		ustring description;
	};

	list<CompletionResult> getCompletion(ustring word, class Document *, Gsv::Buffer::iterator location);

	void gotoDefinition (class Document*, Gsv::Buffer::iterator location);

	std::unique_ptr<ClangIndexData> _data;
};

} /* namespace MatEdit */
