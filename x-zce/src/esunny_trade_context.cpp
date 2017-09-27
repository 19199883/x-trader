#include "esunny_trade_context.h"

#include "config_data.h"
#include "my_cmn_log.h"

ESUNNYTradeContext::ESUNNYTradeContext()
{
    po_tmp_ = NULL;
}

void ESUNNYTradeContext::SaveOrderInfo(TAPIUINT32 session_id, const EsunnyOrderInfo * order_info)
{
    std::lock_guard<std::mutex> lock(order_mutex_);
    sn_to_order_.insert(std::make_pair(order_info->po.serial_no, order_info));
    bool insert_result = sessionid_to_order_.insert(std::make_pair(session_id, order_info)).second;
    if (!insert_result)
    {
        TNL_LOG_WARN("duplicate SessionID when InsertOrder, SessionID:%d.", session_id);

        // 回报早于报单请求返回，sessionid和报单关系已经存储，此次new的对象可以删除
        delete order_info;
    }
    po_tmp_ = NULL;
}

const EsunnyOrderInfo* ESUNNYTradeContext::GetOrderInfoBySN(SerialNoType sn)
{
    std::lock_guard<std::mutex> lock(order_mutex_);

    SNToEsunnyOrderInfoMap::const_iterator cit = sn_to_order_.find(sn);
    if (cit != sn_to_order_.end())
    {
        return cit->second;
    }

    return NULL;
}

const EsunnyOrderInfo* ESUNNYTradeContext::GetOrderInfoByOrderNo(const std::string& order_no)
{
    std::lock_guard<std::mutex> lock(order_mutex_);

    OrderNoToEsunnyOrderInfoMap::const_iterator cit = orderno_to_order_.find(order_no);
    if (cit != orderno_to_order_.end())
    {
        return cit->second;
    }

    return NULL;
}

const EsunnyOrderInfo* ESUNNYTradeContext::UpdateOrderNoAndGetOrderInfo(TAPIUINT32 session_id, const char *order_no,
    char server_flag)
{
    std::lock_guard<std::mutex> lock(order_mutex_);
    SessionIDToEsunnyOrderInfoMap::const_iterator cit = sessionid_to_order_.find(session_id);

    // 回报可能在报单函数返回前回来，因报单是串行处理的，此回报肯定是当前正在报的单产生，直接存储关联关系
    if (cit == sessionid_to_order_.end() && po_tmp_ != NULL)
    {
        EsunnyOrderInfo *p = new EsunnyOrderInfo(*po_tmp_);
        sn_to_order_.insert(std::make_pair(po_tmp_->serial_no, p));
        cit = sessionid_to_order_.insert(std::make_pair(session_id, p)).first;
    }

    if (cit != sessionid_to_order_.end())
    {
        if (order_no != NULL)
        {
            if (cit->second->order_no[0] == 0)
            {
                strncpy(cit->second->order_no, order_no, sizeof(TAPISTR_20));
                orderno_to_order_.insert(std::make_pair(std::string(order_no), cit->second));
            }
            else if (strcmp(order_no, cit->second->order_no) != 0)
            {
                TNL_LOG_ERROR("OrderNo of SessionID(%d) changed, from %s to %s.", session_id, cit->second->order_no, order_no);
            }
        }
        cit->second->server_flag = server_flag;

        return cit->second;
    }

    return NULL;
}

void ESUNNYTradeContext::SaveCancelSnOfSessionID(TAPIUINT32 session_id, SerialNoType c_sn)
{
    std::lock_guard<std::mutex> lock(cancel_mutex_);
    sessionid_to_cancel_sn_.insert(std::make_pair(session_id, c_sn));
}

SerialNoType ESUNNYTradeContext::GetSnBySessionID(TAPIUINT32 session_id)
{
    std::lock_guard<std::mutex> lock(cancel_mutex_);

    SessionIDToSerialNoMap::const_iterator cit = sessionid_to_cancel_sn_.find(session_id);
    if (cit != sessionid_to_cancel_sn_.end())
    {
        return cit->second;
    }

    return 0;
}
