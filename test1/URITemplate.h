
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <map>

using namespace std;

class URITemplate
{
public:
	URITemplate(std::string);
	virtual ~URITemplate() = 0;
	virtual void Match(std::string, std::string) = 0;
private:
	URITemplate(const URITemplate&);
	URITemplate& operator=(const URITemplate&);

};
