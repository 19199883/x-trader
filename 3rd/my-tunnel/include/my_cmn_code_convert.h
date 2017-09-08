/*
 * codeconvert.h
 *
 *  Created on: 2012-7-4
 *      Author: oliver
 */

#ifndef _CODECONVERT_H_
#define _CODECONVERT_H_

#include <string.h>
#include "iconv.h"

namespace my_cmn 
{
// 字符编码转换类（使用iconv库）
// 运用RAII原则，管理对象的构造函数中分配资源，由其析构函数负责释放资源
class CodeConvert {
public:
	CodeConvert(const char *input, const char * fromCodeset,
			const char * toCodeset);
	~CodeConvert(void);

	const char * DestString();

private:
	iconv_t iconvPtr;
	char * outputBuf;
};
}
#endif /* _CODECONVERT_H_ */
