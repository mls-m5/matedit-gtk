/*
 * clangindex.cpp
 *
 *  Created on: 15 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#include "clangindex.h"
#include <clang-c/Index.h>
#include <iostream>
#include "document.h"

using namespace std;
using Glib::ustring;

MatEdit::ClangIndex::ClangIndex() {
}

MatEdit::ClangIndex::~ClangIndex() {
}

list<MatEdit::ClangIndex::CompletionResult> MatEdit::ClangIndex::getCompletion(ustring word, Document *document,
		Gsv::Buffer::iterator location) {
	list<CompletionResult> returnList;

	//Todo: Make sure to cache index between calls

	auto index = clang_createIndex(1,1);
	//Todo: parse unsaved documents
	CXUnsavedFile unsavedFile;
	auto buffer = document->buffer();
	ustring fileContent = buffer->get_text(false);
	ustring documentFilename = document->currentFilename();
	unsavedFile.Filename = documentFilename.data();
	unsavedFile.Contents = fileContent.data();
	unsavedFile.Length = fileContent.size();

	CXUnsavedFile unsavedFiles[] = {unsavedFile}; //In plural :)
	int unsavedFilesCount = 1;

	auto translationUnit = clang_parseTranslationUnit(index, document->currentFilename().c_str(), 0,0, unsavedFiles, unsavedFilesCount, clang_defaultEditingTranslationUnitOptions());

	{
		int line = location.get_line() + 1; //To match clang style
		int column = location.get_line_offset() + 1;
		auto result = clang_codeCompleteAt(translationUnit, document->currentFilename().c_str(), line, column, unsavedFiles, unsavedFilesCount, clang_defaultEditingTranslationUnitOptions());

		if (not result) {
			cout << "failed to parse file" << endl;
			return {};
		}

		cout << "completions: (comparing to " << word << ")" << endl;
		for (int i = 0; i < result->NumResults; ++i) {
			auto completion = result->Results[i].CompletionString;
			ustring description;
			ustring completionString;

			// Seems like the chunk with type 1 is the one that is to be completed
			for (auto j = 0; j < clang_getNumCompletionChunks(completion); ++j) {
				auto text = clang_getCompletionChunkText(result->Results[i].CompletionString, j);
				auto kind = clang_getCompletionChunkKind(completion, j);

				if (text.data) {
					auto cString = clang_getCString(text);
					ustring uText = cString;
//					if (not uText.empty()) {// and uText.find(word) != string::npos) {
						cout << uText << " (" << kind << ") ";
						description += (uText + " ");
						if (kind == CXCompletionChunk_TypedText ) {
							completionString = uText;
//							returnList.push_back(uText);
							cout << " (<-completion text)";
						}
//					}
				}
				clang_disposeString(text);
			}
			cout << endl;
//			if (completionString.find(word) != string::npos) { //depending on desired behaviour
			if (completionString.find(word) == 0) { //Match beginning of string
				returnList.push_back({completionString, description});
			}
		}

		clang_disposeCodeCompleteResults(result);
	}

	cout << "compared to " << word << endl;

	clang_disposeTranslationUnit(translationUnit);

	clang_disposeIndex(index);

	return returnList;
}


