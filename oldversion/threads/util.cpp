#include "util.h"

void string_replace(string&s1,const string&s2,const string&s3)
{
	string::size_type pos=0;
	string::size_type a=s2.size();
	string::size_type b=s3.size();
	
	if((pos=s1.find(s2,pos))!=string::npos)
		s1.replace(pos,a,s3);
	return;
}