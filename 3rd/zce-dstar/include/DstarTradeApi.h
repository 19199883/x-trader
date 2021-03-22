///@system  Dstar V10
///@file    DstarTradeApi.h
///@author  Hao Lin 2020-08-20


#ifndef DSTARTRADEAPI_H
#define DSTARTRADEAPI_H

#include "DstarTradeApiError.h"
#include "DstarTradeApiDataType.h"
#include "DstarTradeApiStruct.h"


///@brief DstarTradeApi的回调基类
class IDstarTradeSpi
{
public:
    ///客户端与接口通信连接断开
    virtual void OnFrontDisconnected() = 0;

    ///错误应答
    virtual void OnRspError(DstarApiErrorCodeType nErrorCode) = 0;

    ///登录请求响应,错误码为0说明用户登录成功。
    virtual void OnRspUserLogin(const DstarApiRspLoginField *pRspUserLogin) = 0;
    
    ///提交信息响应
    virtual void OnRspSubmitInfo(const DstarApiRspSubmitInfoField *pRspSubmitInfo) = 0;
    
    ///合约信息响应
    virtual void OnRspContract(const DstarApiContractField *pContract) = 0;

    ///席位信息响应
    virtual void OnRspSeat(const DstarApiSeatField* pSeat) = 0;
    
    ///资金快照响应
    virtual void OnRspFund(const DstarApiFundField *pFund) = 0;

    ///昨持仓快照响应
    virtual void OnRspPrePosition(const DstarApiPrePositionField *pPrePosition) = 0;
    
    ///实时持仓快照响应
    virtual void OnRspPosition(const DstarApiPositionField *pPosition) = 0;

    ///委托响应
    virtual void OnRspOrder(const DstarApiOrderField *pOrder) = 0;
    
    ///报价响应
    virtual void OnRspOffer(const DstarApiOfferField *pOffer) = 0;
    
    ///成交响应
    virtual void OnRspMatch(const DstarApiMatchField *pMatch) = 0;

    ///出入金响应
    virtual void OnRspCashInOut(const DstarApiCashInOutField *pCashInOut) = 0;
    
    ///API准备就绪,用户只有收到此就绪通知时才能进行后续的操作
    ///@param nSerialId 数据快照对应的流号,如果依据快照数据计算,使用该流号之后的数据
    virtual void OnApiReady(const DstarApiSerialIdType nSerialId) = 0;
    
    ///UDP认证请求响应,错误码为0说明认证成功。
    virtual void OnRspUdpAuth(const DstarApiRspUdpAuthField *pRspUdpAuth) = 0;
    
    ///报单应答
    virtual void OnRspOrderInsert(const DstarApiRspOrderInsertField *pOrderInsert) = 0;
    
    ///报价应答
    virtual void OnRspOfferInsert(const DstarApiRspOfferInsertField *pOfferInsert) = 0;
    
    ///撤单应答
    virtual void OnRspOrderDelete(const DstarApiRspOrderDeleteField *pOrderDelete) = 0;
    
    ///最新请求号应答
    virtual void OnRspLastReqId(const DstaApiRspLastReqIdField *pLastReqId) = 0;
    
    ///委托通知 (撤单失败时返回委托通知,委托状态不变,包含撤单失败的错误码)
    virtual void OnRtnOrder(const DstarApiOrderField *pOrder) = 0;

    ///成交通知
    virtual void OnRtnMatch(const DstarApiMatchField *pMatch) = 0;
    
    ///出入金通知
    virtual void OnRtnCashInOut(const DstarApiCashInOutField *pCashInOut) = 0;
    
    ///报价通知 (撤单失败时返回报价通知,报价状态不变,包含撤单失败的错误码)
    virtual void OnRtnOffer(const DstarApiOfferField *pOffer) = 0;
    
    ///询价通知
    virtual void OnRtnEnquiry(const DstarApiEnquiryField *pEnquiry) = 0;
    
};

class IDstarTradeApi
{
public:
    ///注册回调接口
    ///@param pSpi 派生自回调接口类的实例
    virtual void RegisterSpi(IDstarTradeSpi *pSpi) = 0;

    ///注册接口地址
    virtual void RegisterFrontAddress(DstarApiIpType pIp, DstarApiPortType nPort) = 0;

    ///设置API保存交易数据目录和工作日志目录
    ///@param  pPath 目录。目录必须可用
    virtual int SetApiLogPath(DstarApiPathType pPath) = 0;

    ///设置用户信息
    virtual int SetLoginInfo(DstarApiReqLoginField *pLoginInfo) = 0;
    
    ///绑定CPU信息
    ///@param nRecvCpuId 绑定接收通知数据线程Cpu Id,可选。-1:不绑定
    ///@param nLogCpuId  绑定日志线程Cpu Id,可选。-1:不绑定,不记录交易数据日志,也不会开启日志线程
    virtual int SetCpuId(DstarApiCpuIdType nRecvNoticeDataCpuId, DstarApiCpuIdType nLogCpuId) = 0;
    
    ///设置通知流订阅位置
    ///@param nStartId 通知流订阅位置 -1:从最新开始订阅 0:从头订阅 >0:从指定位置订阅
    virtual int SetSubscribeStartId(DstarApiNoticeSubIdType nStartId) = 0;

    ///采集系统信息
    ///@remark 该函数需要系统权限(root)。如果不使用root运行,需要单独设置权限(dmidecode/lshw)
    ///@param pSystemInfo 接收系统信息,内存由客户申请
    ///@param nLen pSystemInfo的长度
    ///@param nAuthKeyVersion 返回密钥版本
    ///@return 错误码 0:成功,-1:获取Ip失败,-2:获取Mac失败,-3:获取设备名称失败,-4:获取操作系统版本失败,-5:获取硬盘序列号失败,
    ///                    -6:获取Cpu序列号失败,-7:获取Bios序列号失败,-8:获取系统分盘信息失败,-9:获取MacOS设备序列号失败
    virtual int GetSystemInfo(char* pSystemInfo, int *nLen, unsigned int *nAuthKeyVersion) = 0;
    
    ///设置上报信息
    ///@param pSubmitInfo 上报信息,包含系统信息和用户登录信息
    virtual int SetSubmitInfo(DstarApiSubmitInfoField *pSubmitInfo) = 0;
    
    ///初始化
    ///@remark 初始化运行环境,只有调用后,API才开始工作
    ///@param nInitFlag 是否查询基础数据(合约、席位等),如果已有数据,可不再查询提高初始化速度
    ///@return 错误码 0:成功,-1:api日志文件创建失败,-2:数据日志文件创建失败,-3:连接已创建,-4:socket创建失败,-5:连接失败
    virtual int Init(DstarApiInitType nInitFlag) = 0;
    
    ///查询最新客户请求号
    ///@remark 可以用来检测报撤单丢包情况,查询间隔不小于5s
    ///@return 错误码 0:成功,-1:Api未就绪,-2:频率超限
    virtual int ReqLastClientReqId() = 0;

    ///获取API的版本信息
    ///@retrun 获取到的版本号
    virtual const char *GetApiVersion() = 0;

};


extern "C" {

IDstarTradeApi *CreateDstarTradeApi();

void FreeDstarTradeApi(IDstarTradeApi *pApiObj);

}


#endif // DSTARTRADEAPI_H
