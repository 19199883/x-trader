#include "vrt_value_obj.h"
FILE *Log::fp = NULL;/*文件指针*/

bool IsEmptyString(char *str)
{
	if(0 == str[0]){
		return true;
	}else{
		return false;
	}
}


void get_curtime(char buffer[],int size)
{
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer,size,"%H:%M:%S",timeinfo);
}
