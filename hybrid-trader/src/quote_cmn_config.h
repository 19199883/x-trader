#ifndef MY_QUOTE_CONFIGDATA_H_
#define MY_QUOTE_CONFIGDATA_H_

#include <list>
#include <string>
#include <vector>
#include <set>

#include <boost/property_tree/ptree.hpp>
#include <thread>

#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))

enum DLL_LOCAL QuoteProviderType
{
    PROVIDERTYPE_GTAEX = 1,
    PROVIDERTYPE_DCE = 2,
    PROVIDERTYPE_CTP = 3,
    PROVIDERTYPE_SHFE_DEEP = 4,
    PROVIDERTYPE_SHFE_EX = 5,
    PROVIDERTYPE_GTA_UDP = 6,
    PROVIDERTYPE_MY_SHFE_MD = 7,
    PROVIDERTYPE_GTAEX_V2 = 8,
    PROVIDERTYPE_TDF = 9,
    PROVIDERTYPE_MY_STOCK = 10,
    PROVIDERTYPE_GTA_MULTI = 11,
    PROVIDERTYPE_FEMAS = 12,
    PROVIDERTYPE_ZEUSING_UDP = 13,
    PROVIDERTYPE_SUNGARD_GOLD = 14,
    PROVIDERTYPE_CZCE = 15,
    PROVIDERTYPE_KSG = 16,
    PROVIDERTYPE_XSPEED = 17,
    PROVIDERTYPE_AC_XELE = 18,

    PROVIDERTYPE_SEC_KMDS_CITICS = 19,
    PROVIDERTYPE_CZCE_MULTI = 20,

    PROVIDERTYPE_FEMAS_MULTICAST = 21,

    PROVIDERTYPE_OPTION_DF = 22,

    PROVIDERTYPE_TAP = 23,
    
    PROVIDERTYPE_NET_XELE = 24,
    PROVIDERTYPE_LOC_XELE = 25,

    PROVIDERTYPE_IB = 26,

    // added on 20151221
    PROVIDERTYPE_LTS = 27,
    PROVIDERTYPE_MYFPGA = 28,

    // added on 20160104
    PROVIDERTYPE_LTS_L2 = 29,

	//added 20160229
	PROVIDERTYPE_SGIT = 30,

	// added for czce multi cast quote source
	PROVIDERTYPE_CZCE_MULTICAST = 31,

	PROVIDERTYPE_REM_VDE = 32,
};

struct DLL_LOCAL AppConfig
{
    int provider_type;
};

struct DLL_LOCAL TOEConfig
{
    std::string connect_cpu_id;
    std::string data_recv_cpu_id;
    std::string toe_id;
    std::string pro_type;
    std::string local_ip;
    std::string local_port;
    std::string remote_ip;
    std::string remote_port;
    std::string multicast_ip;
    //TODO add by hwg
    std::string mac;
};

//add by hwg 20160324
struct DLL_LOCAL zce_multi_config
{
	std::string realtime_ip;
	std::string realtime_port;
	std::string recovery_ip;
	std::string recovery_port;
	std::string gourp_ip;
};

// 登录信息
struct DLL_LOCAL LogonConfig
{
    std::list<std::string> quote_provider_addrs; // 行情提供商服务地址列表
    std::string trade_server_addr;	// 交易地址
    std::string broker_id;			// 经纪商代码
    std::string account;			// 账号
    std::string password;			// 密码
    std::string topic;              // topic of quote

    std::list<std::string> status_data_addrs;        // provider status publish address
    std::list<std::string> mbl_data_addrs;           // 分价位数据pub服务地址
    std::list<std::string> depth_market_data_addrs;  // 上期深度行情pub服务地址

    // TOE configure items
    std::list<TOEConfig> toe_cfg;

    // ZCE Multi configure items  , add  by hwg 20160324
    std::list<zce_multi_config>zce_multi_cfg;

    // added for LTS option quote/ LTS level2 market data (query parameters)
    std::string query_server_addr;
    std::string query_brokerid;
    std::string query_account;
    std::string query_password;
    std::string user_productinfo;
    std::string auth_code;
};

// 订阅列表
typedef std::set<std::string> SubsribeDatas;

struct DLL_LOCAL QuoteDataSaveParam
{
    bool save_to_txt_file;
    std::string file_name_id;
};

// transfer policy
struct DLL_LOCAL TransferPolicy
{
    bool filter_duplicate_flag;
};

// configure files for multi quote source
typedef std::set<std::string> QuoteCfgFiles;

// 配置数据管理
class DLL_LOCAL ConfigData
{
public:
    void Load(const std::string &cfg_file);

    // 对外提供的数据访问接口
    const AppConfig & App_cfg() const
    {
        return app_cfg_;
    }
    const SubsribeDatas & Subscribe_datas() const
    {
        return subscribe_datas_;
    }
    const LogonConfig & Logon_config() const
    {
        return logon_config_;
    }
    const QuoteDataSaveParam & Quote_save_param() const
    {
        return quote_save_param_;
    }
    const QuoteCfgFiles & Quote_cfg_files() const
    {
        return quote_cfg_files_;
    }
    const TransferPolicy &Transfer_policy() const
    {
        return transfer_policy_;
    }

private:
    void ParseSubscribeConfig(boost::property_tree::ptree &pt);

    void ParseLogonConfig(boost::property_tree::ptree &pt);

    void ParseAppConfig(boost::property_tree::ptree &pt);

    void ParseQuoteDataSaveParamConfig(boost::property_tree::ptree &pt);

    void ParseQuoteCfgFiles(boost::property_tree::ptree &pt);

    void ParseTransferPolicy(boost::property_tree::ptree &pt);

private:
    AppConfig app_cfg_;
    SubsribeDatas subscribe_datas_;
    LogonConfig logon_config_;
    QuoteDataSaveParam quote_save_param_;
    QuoteCfgFiles quote_cfg_files_;
    TransferPolicy transfer_policy_;
};

#endif // MY_QUOTE_CONFIGDATA_H_
