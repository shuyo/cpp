#pragma once
//#include "StdAfx.h"
#include "xbyak.h"
#include <boost/shared_ptr.hpp>

//#include "URITemplate.h"
#include "URITemplateParser.h"

URITemplateMatcher::URITemplateMatcher(std::string st) {
	/*
	mov(eax, ptr[esp+4]);
	push(eax);
	for (int i = 0; i < n; i++) {
		//push((int)"Hello Xbyak!");
		call(puts);
	}
    add(esp, 4);
*/
	ret();
}


/*
class URITemplateParser {
private:
	std::auto_ptr<URITemplateMatcher> matcher = NULL;
}
*/

void URITemplateParser::Match(std::string baseAddress, std::string candidate) {
    int (*add3)(int) = (int (*)(int))matcher->getCode();
    add3(3);
}

URITemplateParser::URITemplateParser(std::string st) : URITemplate(st) {
	matcher = new URITemplateMatcher(st);
};

URITemplateParser::~URITemplateParser()
{
	if (matcher) delete matcher;
};
