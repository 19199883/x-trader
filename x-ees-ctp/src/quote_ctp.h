#ifndef CTP_QUOTE_INTERFACE_H_
#define CTP_QUOTE_INTERFACE_H_

#include <string>
#include <sstream>
#include <list>
#include <boost/function.hpp>

class MYCTPDataHandler: public 
{
public:
    // 构造函数
    MYCTPDataHandler(const SubscribeContracts *subscribe_contracts, const ConfigData &cfg);

    // 数据处理回调函数设置
    void SetQuoteDataHandler(boost::function<void(const CDepthMarketDataField *)> quote_data_handler);

    virtual ~MYCTPDataHandler(void);

    bool Logoned() const
    {
        return logoned_;
    }

private:

    bool logoned_;

    // 数据处理函数对象
    boost::function<void(const CDepthMarketDataField *)> quote_data_handler_;

    // 订阅合约集合
    SubscribeContracts subscribe_contracts_;

    // 配置数据对象
    ConfigData cfg_;

    // save assistant object
    QuoteDataSave<CDepthMarketDataField> *p_save_;

    void RalaceInvalidValue_CTP(CThostFtdcDepthMarketDataField &d);
    CDepthMarketDataField Convert(const CThostFtdcDepthMarketDataField &ctp_data);

};

#endif // GTA_QUOTE_INTERFACE_H_
