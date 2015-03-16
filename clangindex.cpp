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
#include "mainwindow.h"

using namespace std;
using Glib::ustring;

namespace MatEdit {
struct ClangIndexData {

	CXIndex index;

};

}

#define dcout if(0) cout

MatEdit::ClangIndex::ClangIndex():
	_data(new ClangIndexData){
	_data->index = clang_createIndex(1,1);

}

MatEdit::ClangIndex::~ClangIndex() {
	clang_disposeIndex(_data->index);
}

list<MatEdit::ClangIndex::CompletionResult> MatEdit::ClangIndex::getCompletion(ustring word, Document *document,
		Gsv::Buffer::iterator location) {
	list<CompletionResult> returnList;

	auto index = _data->index;

	CXUnsavedFile unsavedFile;
	auto buffer = document->buffer();
	ustring fileContent = buffer->get_text(false);
	ustring documentFilename = document->currentFilename();
	unsavedFile.Filename = documentFilename.data();
	unsavedFile.Contents = fileContent.data();
	unsavedFile.Length = fileContent.size();

	auto standardArg = "-std=c++11";
	const char* args[] = {standardArg};
	int argCount = 1;

	CXUnsavedFile unsavedFiles[] = {unsavedFile}; //In plural :)
	int unsavedFilesCount = 1;

	auto translationUnit = clang_parseTranslationUnit(index, document->currentFilename().c_str(), args,1, unsavedFiles, unsavedFilesCount, clang_defaultEditingTranslationUnitOptions());

	{
		int line = location.get_line() + 1; //To match clang style
		int column = location.get_line_offset() + 1;
		auto result = clang_codeCompleteAt(translationUnit, document->currentFilename().c_str(), line, column, unsavedFiles, unsavedFilesCount, clang_defaultEditingTranslationUnitOptions());

		if (not result) {
			cout << "failed to parse file" << endl;
			return {};
		}

		dcout << "completions: (comparing to " << word << ")" << endl;
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
						dcout << uText << " (" << kind << ") ";
						description += (uText + " ");
						if (kind == CXCompletionChunk_TypedText ) {
							completionString = uText;
//							returnList.push_back(uText);
							dcout << " (<-completion text)";
						}
//					}
				}
				clang_disposeString(text);
			}
			dcout << endl;
//			if (completionString.find(word) != string::npos) { //depending on desired behaviour
			if (completionString.find(word) == 0) { //Match beginning of string
				returnList.push_back({completionString, description});
			}
		}

		clang_disposeCodeCompleteResults(result);
	}

	dcout << "compared to " << word << endl;

	clang_disposeTranslationUnit(translationUnit);



	return returnList;
}

void MatEdit::ClangIndex::gotoDefinition(class Document *document,
		Gsv::Buffer::iterator location) {
	auto index = _data->index;

	CXUnsavedFile unsavedFile;
	auto buffer = document->buffer();
	ustring fileContent = buffer->get_text(false);
	ustring documentFilename = document->currentFilename();
	unsavedFile.Filename = documentFilename.data();
	unsavedFile.Contents = fileContent.data();
	unsavedFile.Length = fileContent.size();

	auto standardArg = "-std=c++11";
	const char* args[] = {standardArg};
	int argCount = 1;

	CXUnsavedFile unsavedFiles[] = {unsavedFile}; //In plural :)
	int unsavedFilesCount = 1;

	auto translationUnit = clang_parseTranslationUnit(index, document->currentFilename().c_str(), args,1, unsavedFiles, unsavedFilesCount, clang_defaultEditingTranslationUnitOptions());

	auto file = clang_getFile(translationUnit, documentFilename.data());


	auto cxLocation = clang_getLocation(translationUnit, file, location.get_line() + 1, location.get_line_offset() + 1);

	auto cursor = clang_getCursor(translationUnit, cxLocation);

	auto definition = clang_getCursorDefinition(cursor);

	auto definitionLocation = clang_getCursorLocation(definition);

	CXFile definitionFile;
	unsigned definitionLine, definitionColumn, definitionOffset;
	clang_getExpansionLocation(definitionLocation,
	                                               &definitionFile,
	                                               &definitionLine,
	                                               &definitionColumn,
	                                               &definitionOffset);


	auto filename = clang_getFileName(definitionFile);
	cout << "Definition" /*<< clang_getFileName(definitionFile)*/ << ":" << definitionLine << "," << definitionColumn << endl;

	if (filename.data == 0) {
		return;
	}

	document->rootWindow()->openLocation(clang_getCString(filename), definitionLine -1, definitionColumn - 1);
	clang_disposeString(filename);
//	document->gotoLocation(definitionLine - 1, definitionColumn - 1);

	//	clang_getCursor()
	//	clang_getCursorDefinition()


	clang_disposeTranslationUnit(translationUnit);


}
