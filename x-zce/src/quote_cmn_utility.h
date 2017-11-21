#if !defined(QUOTE_CMN_UTILITY_H_)
#define QUOTE_CMN_UTILITY_H_

#include <pthread.h>
#include <string>
#include <vector>
#include <set>
#include <string>
#include <float.h>

using namespace std;

#ifndef DLL_PUBLIC
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#endif

#define MAX_PURE_DBL          (double)9007199254740991.0
#define MIN_PURE_DBL          (double)-9007199254740991.0
#define MAX_PURE_FLT          (double)16777215
#define MIN_PURE_FLT          (double)-16777215
inline bool IsValidDouble(double lfValue)
{
    return ((lfValue > MIN_PURE_DBL) && (lfValue < MAX_PURE_DBL));
}

inline bool IsValidFloat(float lfValue)
{
    return ((lfValue > MIN_PURE_FLT) && (lfValue < MAX_PURE_FLT));
}

inline double InvalidToZeroD(double dVal)
{
    return IsValidDouble(dVal) ? dVal : 0.0;
}

inline float InvalidToZeroF(float fVal)
{
    return IsValidFloat(fVal) ? fVal : 0.0;
}

typedef std::pair<std::string, unsigned short> IPAndPortNum;
IPAndPortNum ParseIPAndPortNum(const std::string &addr_cfg);

typedef std::pair<std::string, std::string> IPAndPortStr;
IPAndPortStr ParseIPAndPortStr(const std::string &addr_cfg);

/*
 * 从文件file中读取主力合约，并存储到buffer中。
 * 假设主力合约最多20个。
 * 查找主力合约时，从位置0开始查找，遇到第一个空字符串止
 * 调用者需要对buffer清零
 * contract: e.g. SR801
 * @return:返回主力合约个数
 */
int32_t LoadDominantContracts(string file, char buffer[20][10]);

/*
* check whether the given contract is dominant.
* commodity_no: 要判断是否是主力合约的合约的品种部分,e.g. SR
* contract_no:判断是否是主力合约的合约的月份部分,e.g 801
* buffer: dominant contract list
*/
bool IsDominantImp(const char*commodity_no, const char* contract_no, char buffer[20][10], 
	int32_t buffer_size);

/*
* check whether the given contract is dominant.
* contract:SR1802, 要判断是否是主力合约的合约
* buffer: dominant contract list
*/
bool IsDominantImp(const char* contract, char buffer[20][10], 
	int32_t buffer_size);

/*
 * 判断commidity_no和contract_no代表的合约是否与contract指定的完整合约相等，如：
 * contract:SR801
 * commidity_no:SR
 * contract_no:801
 */
bool IsEqualSize3(const char *contract, const char*commidity_no, const char* contract_no);

/* 判断commidity_no和contract_no代表的合约是否与contract指定的完整合约相等，如：
* contract:SR1801
* commidity_no:SR
* contract_no:801
*/
bool IsEqualSize4(const char *contract, const char*commciodity_no, const char* contract_no);

/*
 * 判断contract_size3是否与contract_size4相等，如：
 * contract_size3:SR801
 * contract_size4:SR1801
 */
bool IsEqual(const char *contract_size3, const char* contract_size4)

#endif  //
