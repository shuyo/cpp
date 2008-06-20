// test1.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

//#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <map>
//#include <boost/shared_ptr.hpp>
//#include "xbyak.h"

#include "URITemplateParser.h"

using namespace std;


string getSomeString() {
	return "hoge";
}

extern "C" char* getCString() {
	char ret[128];
	strcpy_s(ret, "hoge");
	strcat_s(ret, _strdup("hage"));
	return _strdup(ret);
}

int _tmain(int argc, _TCHAR* argv[]) {

	map<string, char*> param;
	param.insert(pair<string, char*>("book_id", "456"));
	param["user_id"] = "123";
	map<string, char*>::iterator it = param.begin();
	while ( it != param.end() ) {
		cout << it->first << ", " << it->second << "\n";
		it++;
	}
	cout << getSomeString() << getCString() << param.size() << "\n";

    URITemplateParser hello("weather/{state}/{city}?forecast={day}");
	hello.Match("","");

	return 0;
}

