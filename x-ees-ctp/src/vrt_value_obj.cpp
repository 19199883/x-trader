#include "vrt_value_obj.h"
FILE *Log::fp = NULL;/*文件指针*/


/*
 * 合约只需要比较品种部分和日期部分的后3位，如：rb1910,只需要比较rb和910
 */
bool IsEqualContract(char *contract1, char* contract2)
{
	if ( contract1[0]==contract2[0] &&
		 contract1[1]==contract2[1] &&
		 contract1[3]==contract2[3] &&
		 contract1[4]==contract2[4] &&
		 contract1[5]==contract2[5]){
		return true;
	}else{
		return false;
	}
}

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
