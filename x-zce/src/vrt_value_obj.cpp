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

/*
 */
bool IsEqualContract(char *contract1, char* contract2)
{
	if ( contract1[0]==contract2[0] &&
		 contract1[1]==contract2[1] &&
		 contract1[2]==contract2[2] &&
		 contract1[3]==contract2[3] &&
		 contract1[4]==contract2[4]){
		return true;
	}
	else{
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
