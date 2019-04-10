#ifndef __TAP_MD_PRODUCER_H__
#define __TAP_MD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "TapQuoteAPI.h"
#include "quote_cmn_utility.h"
#include "quote_datatype_tap.h"
#include "quote_datatype_czce_level2.h"
#include "ZceLevel2ApiStruct.h"

/*
 * 10 power of 2
 */
#define L1MD_BUFFER_SIZE 1000 

/*
 * 识Level行情处于未接收数居前的未知位置
 */
#define L1MD_NPOS -1 

using namespace std;

struct L1Mdconfig
{
	char user[50];
	char password[250];
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20];
};

class L1MDProducerHelper
{
	public:
};

class TapMDProducer : public ITapQuoteAPINotify
{
	public:
		TapMDProducer(struct vrt_queue  *queue);
		~TapMDProducer();

		/*
		 * 与逻辑处理相关
		 */
		TapAPIQuoteWhole_MY* GetData(int32_t index);

		void End();

		// lic
		TapAPIQuoteWhole_MY *GetLastDataForIllegaluser(const char *CommodityNo, const char*ContractNo1);

	private:
		/*
		 * 与API相关
		 */
		ITapQuoteAPI *api_;
		unsigned int *sID;
		void InitApi();
		void Login();

		/*
		 * 与逻辑处理相关
		 */
		int32_t Push(const TapAPIQuoteWhole_MY& md);
		struct vrt_producer  *producer_;
		TapAPIQuoteWhole_MY md_buffer_[L1MD_BUFFER_SIZE] ;
		int32_t l1data_cursor_;
		bool ended_;

		void SubscribeDominantContracts();
#ifdef PERSISTENCE_ENABLED 
		void SubscribeAllContracts();
		void qry_contract(TapAPICommodity &qryReq);
		void subscribe_quote(TapAPIContract contract);
#endif

		void Convert(const TapAPIQuoteWhole &other, TapAPIQuoteWhole_MY &data);

		/*
		 * check whether the given contract is dominant.
		 * contract: e.g. SR801
		 */
		bool IsDominant(const char*commciodity_no, const char* contract_no);
		int32_t dominant_contract_count_;
		char dominant_contracts_[20][10];

		TapAPIQuoteWhole_MY target_data_;

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		L1Mdconfig config_;
		void ParseConfig();

		/*
		 * API
		 */
		/**
		 * @brief	系统登录过程回调。
		 * @details	此函数为Login()登录函数的回调，调用Login()成功后建立了链路连接，然后API将向服务器发送登录认证信息，
		 *			登录期间的数据发送情况和登录的回馈信息传递到此回调函数中。
		 * @param[in] errorCode 返回错误码,0表示成功。
		 * @param[in] info 登陆应答信息，如果errorCode!=0，则info=NULL。
		 * @attention	该回调返回成功，说明用户登录成功。但是不代表API准备完毕。
		 * @ingroup G_Q_Login
		 */
		virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info);

		/**
		 * @brief	通知用户API准备就绪。
		 * @details	只有用户回调收到此就绪通知时才能进行后续的各种行情数据查询操作。\n
		 *			此回调函数是API能否正常工作的标志。
		 * @attention  就绪后才可以进行后续正常操作
		 * @ingroup G_Q_Login
		 */
		virtual void TAP_CDECL OnAPIReady();

		/**
		 * @brief	API和服务失去连接的回调
		 * @details	在API使用过程中主动或者被动与服务器服务失去连接后都会触发此回调通知用户与服务器的连接已经断开。
		 * @param[in] reasonCode 断开原因代码。 \n
		 * @ingroup G_Q_Disconnect
		 */
		virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);

		/**
		 * @brief 返回交易所信息
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
		 * @param[in] isLast 标示是否是最后一批数据；
		 * @param[in] info 返回的交易所信息。
		 * @attention  不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_Q_Exchange
		 */
		virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info);

		/**
		 * @brief	返回所有品种信息。
		 * @details	此回调接口用于向用户返回得到的所有品种信息。
		 * @param[in] sessionID 请求的会话ID，和GetAllCommodities()函数返回对应；
		 * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
		 * @param[in] isLast 标示是否是最后一批数据；
		 * @param[in] info 返回的信息数组的起始指针。
		 * @attention  不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_Q_Commodity
		 */
		virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info);

		/**
		 * @brief 返回系统中合约信息
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
		 * @param[in] isLast 标示是否是最后一批数据；
		 * @param[in] info		指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_Q_Contract
		 */
		virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info);

		/**
		 * @brief	返回订阅行情的全文。
		 * @details	此回调接口用来返回订阅行情的全文。全文为当前时间的行情信息。
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] isLast 标示是否是最后一批数据；
		 * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
		 * @param[in] info		指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @ingroup G_Q_Quote
		 */
		virtual void TAP_CDECL OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole *info);

		/**
		 * @brief 退订指定合约的行情的结果回调
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
		 * @param[in] isLast 标示是否是最后一批数据；
		 * @param[in] info		指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @ingroup G_Q_Quote
		 */
		virtual void TAP_CDECL OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract *info);

		/**
		 * @brief	返回订阅行情的变化内容。
		 * @details	此回调接口用来通知用户行情信息产生了变化，并向用户提交新的行情全文。
		 * @param[in] info 最新的行情全文内容
		 * @attention 不要修改和删除Quote指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_Q_Quote
		 */
		virtual void TAP_CDECL OnRtnQuote(const TapAPIQuoteWhole *info);

		/**
		 * @brief 通知用户密码修改结果
		 * @param[in] sessionID 修改密码的会话ID,和ChangePassword返回的会话ID对应。
		 * @param[in] errorCode 返回错误码，0表示成功。
		 * @note 暂未实现
		 * @ingroup G_Q_Password
		 */
		virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode);

		/**
		 * @brief 通知合约添加
		 * @param[in] info		指向返回的新增合约
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_Q_Contract
		 */
		virtual void TAP_CDECL OnRtnContract(const TapAPIQuoteContractInfo *info);
};

#endif
