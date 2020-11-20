//(C) 2020, dan-gubkin@mail.ru

#ifndef SPELL_CHECKER_H_
#define SPELL_CHECKER_H_

#include "Dictionary.hpp"

#include <map>


typedef std::map<size_t, const char*> Result; // value - position into dictionary


class SpellChecker final{

public:

	/*
	 * params:
	 * word - checked word
	 * dict - dictionary
	 * res - result
	 * returns:
	 * true if word is correct, overwise - false
	 * in the case of founded corrections res has len more than 0
	 */
	bool correct(const char* word, Dictionary& dict, Result& res);

private:

	typedef std::map<std::string, size_t> Candidates; // value - modified position

	void edits_add(const std::string& word, Candidates& result, ssize_t except);
	void edits_del(const std::string& word, Candidates& result, ssize_t except);
	void edits_second(const Candidates& add, const Candidates& del, Candidates& result);

};

#endif //SPELL_CHECKER_H_
