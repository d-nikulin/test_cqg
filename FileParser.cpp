//(C) 2020 dan-gubkin@mail.ru

#include "Constants.hpp"
#include "FileParser.hpp"
#include "Dictionary.hpp"
#include "SpellChecker.hpp"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


static const char* TEXT_DELIM = "===";	// text delimiter for file


static inline bool isWordSymbol(const char& ch){
	return isalpha( (int)ch )!=0 || '-'==ch || '='==ch;
}


off_t FileParser::parseDictBuf(char* buf, size_t len, Dictionary& dict)const{
	char* word = buf;
	const size_t delim_len = strlen(TEXT_DELIM);
	for (size_t i=0, num=0; i<len; i++){
		if ( ! isWordSymbol( *(buf+i) ) ){
			*(buf+i) = '\0';
			if ( strncmp(word, TEXT_DELIM, delim_len)==0 ){
				std::cout << "loaded " << num << " words into dictionary" << std::endl;
				return (off_t)(i+1);
			}
			if (strlen(word)>0 && strlen(word)<=MAX_WORD_LEN){
				const auto res = dict.insert( {word, num} );
				if (res.second){
					++num;
				}
			}
			word = buf + i + 1;
		}
	}
	return -1;
}


off_t FileParser::parseDict(const char* filename, Dictionary& dict)const{
	dict.clear();
	int fd = open(filename, O_RDONLY);
	if ( fd < 0 ){
		throw std::runtime_error( strerror(errno) );
	}
	off_t flen = lseek(fd, 0, SEEK_END);
	if (flen<0){
		close(fd);
		throw std::runtime_error( strerror(errno) );
	}
	lseek(fd, 0, SEEK_SET);

	const size_t buf_len = std::min(MAX_DICT_LEN, (size_t)flen+1);
	char* buf = NULL;
	try{
		buf = new char[buf_len];
	}catch(std::bad_alloc& e){
		close(fd);
		throw std::runtime_error( e.what() );
	}
	std::cout << "allocated " << buf_len << " bytes for dictionary" << std::endl;
	char* pos = buf;
	size_t len = buf_len;
	ssize_t res = 0;
	do{
		res = read(fd, pos, len);
		if (res<0){
			close(fd);
			throw std::runtime_error( strerror(errno) );
		}
		pos += (size_t) res;
		len -= (size_t) res;
		len = len == 0 ? buf_len : len;
	}while( res>0 );

	close( fd );
	off_t text_pos = parseDictBuf( buf, buf_len, dict );
	if (text_pos<0){
		delete [] buf;
		dict.clear();
		throw std::runtime_error("Dictionary is too big, text delimiter not found");
	}
	dict.setBuffer( buf );
	return text_pos;

}


static inline void write_wrapper(const char* data, size_t len, FILE*out){
	size_t io_res = fwrite( data, len, 1, out);
	if (io_res!=1){
		throw std::runtime_error( strerror(errno) );
	}

}


void FileParser::outResult(const char* word, bool correct, const Result& res, FILE* out)const{
	if ( correct || res.size()==1 ){
		const char* aword = correct ? word :res.begin()->second;
		write_wrapper( aword, strlen( aword), out);
		return;
	}

	if ( res.size()==0 ){
		const char* not_found_pattern = "{%s?}";
		int n = fprintf(out, not_found_pattern, word);
		if (n<0){
			throw std::runtime_error( strerror(errno) );
		}
		return;
	}

	const char* beg = "{";
	const char* end = "}";
	const char* sep = " ";
	write_wrapper(beg, strlen(beg), out);
	bool first = true;
	for (auto k = res.begin(); k!=res.end(); k++){
		if (first){
			first = false;
		}else{
			write_wrapper(sep, strlen(sep),out);
		}
		write_wrapper(k->second, strlen(k->second), out);
	}
	write_wrapper(end, strlen(end), out);
}


void FileParser::outDelim(char& delim, FILE* out)const{
	write_wrapper(&delim, sizeof(delim), out);
}


size_t FileParser::parseTextBuf(char* buf, size_t len, Dictionary& dict, SpellChecker& checker, FILE* out)const{
	std::cout << "parsing text buf ("<< len << ") bytes" << std::endl;
	char* word = buf;
	Result res;
	bool end_data = false;

	for (size_t i=0; i<len; i++){
		if ( isWordSymbol( buf[i] ) ){
			continue;
		}
		char delim = buf[i];
		buf[i] = '\0';
		size_t w_len = strlen( word );

		if (w_len==0 || w_len>MAX_WORD_LEN){
			if ((w_len)==0){
				outDelim(delim, out);
			}else{ //todo: fixme уточнить логику обработки слов с превышением размера слова (нужны ли они в выводе)
				std::cout << "oversized word " << word << " skipped" << std::endl;
			}
			word = &buf[i+1];
			continue;
		}

		if ( strcmp(TEXT_DELIM, word)==0 ){
			end_data = true;
			break;
		}

		bool correct = checker.correct(word, dict, res);
		outResult(word, correct, res, out);
		outDelim(delim, out);
		res.clear();
		word = &buf[i+1];
	}
	int n = fflush(out);
	if (n==-1){
		throw std::runtime_error( strerror(errno) );
	}
	return end_data ? 0 : (size_t)(word - buf);
}


void FileParser::parseText(const char* filename, off_t fpos, Dictionary& dict, SpellChecker& checker)const{
	int fd = open(filename, O_RDONLY);
	if ( fd < 0 ){
		throw std::runtime_error( strerror(errno) );
	}
	off_t flen = lseek(fd, 0, SEEK_END);
	if (flen==-1){
		close(fd);
		throw std::runtime_error( strerror(errno) );
	}
	if ( lseek(fd, fpos, SEEK_SET) == -1 ){
		close(fd);
		throw std::runtime_error( strerror(errno) );
	}
	FILE* out = NULL;
	std::string filename_out = std::string(filename) + ".out";
	out = fopen(filename_out.c_str(), "w");
	if (out==NULL){
		close(fd);
		throw std::runtime_error( strerror(errno) );
	}
	const size_t buf_len = std::min( (size_t)(flen - fpos ), BUF_LEN);
	char* buf = NULL;
	try{
		buf = new char[ buf_len + 1 ];
		std::cout << "allocated " << buf_len << " bytes for text buffer" << std::endl;
	}catch(std::bad_alloc& e){
		close( fd );
		fclose( out );
		throw std::runtime_error( e.what() );
	}
	char* pos = buf;
	size_t len = buf_len;
	ssize_t io_res = 0;

	std::cout << "parsing: "<< filename <<  std::endl;
	do{
		io_res = read(fd, pos, len);
		if (io_res<0){
			close( fd );
			fclose( out );
			delete [] buf;
			throw std::runtime_error( strerror(errno) );
		}

		pos += (size_t) io_res;
		len -= (size_t) io_res;
		if (len==0 || io_res==0){
			try{
				size_t parse_res = parseTextBuf(buf, (size_t)(pos-buf), dict, checker, out);
				if (parse_res==0){
					break;
				}
				size_t delta = size_t((size_t)(pos-buf) - parse_res);
				if ( delta > 0  ){
					for (size_t i=parse_res, j=0; i<parse_res+delta; i++,j++){
						buf[j] = buf[i];
					}
					pos = buf + delta;
					len = buf_len - delta;
				}else{
					pos = buf;
					len = buf_len;
				}
			}catch(std::exception& e){
				close(fd);
				fclose(out);
				delete [] buf;
				throw e;
			}
		}

	}while( true );

	close( fd );
	fclose( out );
	delete [] buf;

	std::cout << filename << " parsed successfully result placed into "<< filename_out <<  std::endl;
	return ;
}
