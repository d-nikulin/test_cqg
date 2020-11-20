//(C) 2020 dan-gubkin@mail.ru

#include <Dictionary.hpp>
#include <FileParser.hpp>
#include <SpellChecker.hpp>

#include <iostream>


int main(int argc, char** argv){
	if (argc<2){
		std::cout << "Usage:" << std::endl << "\t./spellchecker <filename>" << std::endl;
		exit( EXIT_FAILURE );
	}

	try{
		Dictionary dict;
		FileParser parser;
		SpellChecker checker;
		auto pos = parser.parseDict( argv[1], dict );
		parser.parseText(argv[1], pos, dict, checker);
	}catch(std::exception& e){
		std::cout << e.what() << std::endl;
		exit( EXIT_FAILURE );
	}

	return EXIT_SUCCESS;
}
