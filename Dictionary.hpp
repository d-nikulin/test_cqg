//(C) 2020, dan-gubkin@mail.ru

#ifndef DICTIONARY_H_
#define DICTIONARY_H_


#include <cstring>
#include <map>
#include <vector>


class StringComparator{
	public:
		bool operator()(const char* v1, const char* v2)const{
			return strcasecmp(v1, v2)<0;
		}
};


typedef std::map<const char*, size_t, StringComparator> InnerDictionary; // value - position into dictionary


class Dictionary:public InnerDictionary{
public:
	Dictionary():InnerDictionary(){
		buf = NULL;
	}
	virtual ~Dictionary(){
		 delete [] buf;
	}
	void setBuffer(char* buf){
		if (this->buf){
			throw std::runtime_error("Dictionary is not empty");
		}
		this->buf = buf;
	}
	void clear(){
		delete [] buf ;
		InnerDictionary::clear();
	}

private:
	char* buf;
};

#endif //DICTIONARY_H_
