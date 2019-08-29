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
	if (strcmp(contract1,contract2)==0){
		return true;
	}
	else{
		return false;
	}
}

/*
 * 大商所品种部分有的是1位，如j2001,；有的是2位，如jm2001.
 * 如果交易程序中主力合约一个品种只有一个合约，那么只需判断前2位，
 * 就可以判断某策略是否订阅了合约
 */
bool IsEqualProduct(char *contract1, char* contract2)
{
	if ((contract1[0]== contract2[0] &&
		 contract1[1]== contract2[1])){
		return true;
	}
	else{
		return false;
	}
}

