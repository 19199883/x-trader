#ifndef ESUNNY_TRADE_CONTEXT_H_
#define ESUNNY_TRADE_CONTEXT_H_

#include <pthread.h>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "TapTradeAPI.h"
#include "trade_data_type.h"
#include "my_trade_tunnel_data_type.h"
#include "my_tunnel_lib.h"
#include "my_cmn_util_funcs.h"
#include "tunnel_cmn_utility.h"

struct EsunnyOrderInfo
{
    T_PlaceOrder po;

    mutable EntrustNoType entrust_no;
    mutable char entrust_status;
    mutable VolumeType volume_remain;  // 剩余未成交的手数
    mutable VolumeType volume_total_matched;  // 剩余未成交的手数

    // fields for cancel order
    mutable TAPISTR_20 order_no;
    mutable TAPICHAR server_flag;

    inline bool IsTerminated() const
    {
        return entrust_status == MY_TNL_OS_ERROR
            || entrust_status == MY_TNL_OS_WITHDRAWED
            || (entrust_status == MY_TNL_OS_COMPLETED && volume_total_matched >= po.volume);
    }

    EsunnyOrderInfo(const T_PlaceOrder &po_)
    :po(po_)
    {
        entrust_no = 0;
        entrust_status = MY_TNL_OS_UNREPORT;
        volume_remain = po.volume;
        volume_total_matched = 0;
        memset(order_no, 0, sizeof(order_no));
        server_flag = 0;
    }
};

typedef std::unordered_map<SerialNoType, const EsunnyOrderInfo *> SNToEsunnyOrderInfoMap;
typedef std::unordered_map<std::string, const EsunnyOrderInfo *> OrderNoToEsunnyOrderInfoMap;
typedef std::unordered_map<TAPIUINT32, const EsunnyOrderInfo *> SessionIDToEsunnyOrderInfoMap;
typedef std::unordered_map<TAPIUINT32, SerialNoType> SessionIDToSerialNoMap;

class ESUNNYTradeContext
{
public:
    ESUNNYTradeContext();

    // order id to order
    void SaveOrderInfo(TAPIUINT32 session_id, const EsunnyOrderInfo *order_info);
    const EsunnyOrderInfo * GetOrderInfoBySN(SerialNoType sn);
    const EsunnyOrderInfo * GetOrderInfoByOrderNo(const std::string &order_no);
    const EsunnyOrderInfo * UpdateOrderNoAndGetOrderInfo(TAPIUINT32 session_id, const char *order_no, char server_flag);

    void SavePendingOrder(const T_PlaceOrder *order_info)
    {
        std::lock_guard<std::mutex> lock(order_mutex_);
        po_tmp_ = order_info;
    }

    // cancel serial no to session id(request id)
    void SaveCancelSnOfSessionID(TAPIUINT32 session_id, SerialNoType c_sn);
    SerialNoType GetSnBySessionID(TAPIUINT32 session_id);

private:
    // object for maintain place order requests
    std::mutex order_mutex_;
    SNToEsunnyOrderInfoMap sn_to_order_;
    OrderNoToEsunnyOrderInfoMap orderno_to_order_;
    SessionIDToEsunnyOrderInfoMap sessionid_to_order_;
    const T_PlaceOrder *po_tmp_;

    // object for maintain cancel requests
    std::mutex cancel_mutex_;
    SessionIDToSerialNoMap sessionid_to_cancel_sn_;
};

#endif
