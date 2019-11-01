#include "vrt_value_obj.h"
FILE *Log::fp = NULL;/*文件指针*/


/*
 * 合约只需要比较品种部分和日期部分的后3位，如：rb1910,只需要比较rb和910
 */
bool IsEqualContract(char *contract1, char* contract2)
{
	if (strcmp(contract1, contract2) == 0)
	{
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
