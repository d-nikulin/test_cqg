//(C) 2020, dan-gubkin@mail.ru

#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_

#include "Dictionary.hpp"
#include "SpellChecker.hpp"


class FileParser final{

public:

	/*
	 * params:
	 * filename - full path to filename containing dictionary
	 * dict - dictionary for emplacing
	 * returns:
	 * beging text position position into file
	 * throws:
	 * std::exception in the case i/o error
	 */
	off_t parseDict(const char* filename, Dictionary& dict)const;

	/*
	 * params:
	 * filename - full path to filename containing dictionary
	 * pos - beging text position position into file
	 * dict - dictionary for checking
	 * checker - checker instance
	 * no returns:
	 * throws:
	 * std::exception in the case i/o error
	 *
	 */
	void parseText(const char* filename, off_t pos, Dictionary& dict, SpellChecker& checker)const;

private:
	off_t parseDictBuf(char* buf, size_t len, Dictionary& dict)const;
	size_t parseTextBuf(char* buf, size_t len, Dictionary& dict, SpellChecker& checker, FILE* out)const;
	void outResult(const char* word, bool correct, const Result&, FILE* out)const;
	void outDelim(char& delim, FILE* out)const;

};

#endif //FILE_PARSER_H_
