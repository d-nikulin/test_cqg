//(C) 2020, dan-gubkin@mail.ru

#include "Constants.hpp"
#include "SpellChecker.hpp"

#include <fstream>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstring>


bool SpellChecker::correct(const char* word, Dictionary& dict, Result& res){

	const auto j = dict.find( word );
	if ( j!=dict.end() ){
		return true;
	}

	Candidates cnd[2]; // 0 - add; 1- del
	edits_add(std::string(word), cnd[0], -1);
	edits_del(std::string(word), cnd[1], -1);

	bool found = false;
	for (size_t k=0; k<2; k++){
		for(auto i=cnd[k].begin(); i!=cnd[k].end(); i++){
			const auto j = dict.find( i->first.c_str() );
			if (j!=dict.end()){
				found = true;
				res.insert({j->second, j->first});
			}
		}
	}
	if (found){
		return false;
	}

	Candidates second;
	edits_second(cnd[0], cnd[1], second);
	found = false;
	for(auto i=second.begin(); i!=second.end(); i++){
		const auto j = dict.find( i->first.c_str() );
		if (j!=dict.end()){
			found = true;
			res.insert({j->second, j->first});
		}
	}

	return false;
}


static inline bool check_except_del(ssize_t except, size_t i){
	if (except == -1){
		return false;
	}
	if ( except == (ssize_t)i || except+1 == (ssize_t)i || except-1 == (ssize_t)i){
		return true;
	}
	return false;
}


static inline bool check_except_add(ssize_t except, size_t i){
	if (except == -1){
		return false;
	}
	if ( except == (ssize_t)i || except+1 == (ssize_t)i){
		return true;
	}
	return false;
}


void SpellChecker::edits_del(const std::string& word, Candidates& cnd, ssize_t except){
	std::string::size_type w_len = word.size();
	if (w_len<2 || w_len>MAX_WORD_LEN){
		return;
	}
	for (std::string::size_type i = 0; i < word.size(); i++){
		if (check_except_del(except, i)){
			continue;
		}
		std::string copy = word.substr(0, i) + word.substr(i+1);
		cnd.insert( {copy, i} ); //deletions
	}
}


void SpellChecker::edits_add(const std::string& word, Candidates& cnd, ssize_t except){
	std::string::size_type w_len = word.size();
	if (w_len>=MAX_WORD_LEN){
		return;
	}
	for (size_t i = 0; i <= w_len; i++){
		if (check_except_add(except, i)){
			continue;
		}
		for (char ch = 'a'; ch <= 'z'; ch++){
			std::string copy  = word.substr(0,i) + ch + word.substr(i);
			cnd.insert( {copy, i} ); //insertion
		}
	}
}


void SpellChecker::edits_second(const Candidates& add, const Candidates& del, Candidates& res){
	for (auto j = add.begin(); j!=add.end(); j++){
		std::string word = j->first;
		edits_del(word, res, -1);
		edits_add(word, res, (ssize_t)j->second);
	}

	for (auto j = del.begin(); j!=del.end(); j++){
		std::string word = j->first;
		edits_del(word, res, (ssize_t)j->second);
	}
}
