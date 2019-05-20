/**
 * Copyright (C) 2015-2019，大连商品交易所大连飞创信息技术有限公司
 *
 * @file <collect.h>
 * @brief <穿透监管采集库接口声明>
 * @author <X1项目组>
 * @date <2018年11月8日>
 */

 
#ifndef __DFITC_COLLECTION_H_
#define __DFITC_COLLECTION_H_



#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus
    /**
     * @brief 获取采集信息密文
     * {@param[out] <out><用于存放采集信息密文>}
     * {@param[in] <size><输出缓冲区的最大值，目前采集信息密文最大长度不会超过1024>}
     * {@param[out] <real_size><采集信息密文的实际长度>}
     * {@retval <int>}
     * {@return <0 表示正常; -1 表示参数错误; -2 表示权限不足; 正数 表示out大小应大于此值>}
     */
    int DfitcGetSysInfo(char* out, int size, int* real_size);
    /**
     * @brief 获取穿透监管采集库版本号
     * {@retval <const char *>}
     * {@return <获取穿透监管采集库版本号>}
     */
    const char* GetDfitcVersion();
#ifdef __cplusplus
};
#endif  //__cplusplus

#endif  //__DFITC_COLLECTION_H_