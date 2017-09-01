#include <thread>
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>

// TODO:
#include "x1_data_formater.h"

using namespace std;
using namespace std::placeholders;

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
{
	ended_ = false;
	this->ParseConfig();

	clog_info("[%s] RPT_BUFFER_SIZE: %d;", module_name_, RPT_BUFFER_SIZE);

	struct vrt_producer  *producer = vrt_producer_new("tunnrpt_producer", 1, queue);
	this->producer_ = producer;
	this->producer_ ->yield = vrt_yield_strategy_threaded();

	api_ = CreateExchange(config_);
	auto fn_ord_resf = bind(&TunnRptProducer::OnRspInsertOrder, this, _1,_2);
	api_->SetCallbackHandler(fn_ord_resf);
	auto fn_cancel_resf = bind(&TunnRptProducer::OnRspCancelOrder, this, _1);
	api_->SetCallbackHandler(fn_cancel_resf);
	auto fn_ord_rtnf = bind(&TunnRptProducer::OnRtnOrder, this, _1,_2);
	api_->SetCallbackHandler(fn_ord_rtnf);
	auto fn_trad_rtnf = bind(&TunnRptProducer::OnRtnMatchedInfo, this, _1,_2);
	api_->SetCallbackHandler(fn_trad_rtnf);
}

TunnRptProducer::~TunnRptProducer()
{
//	if (this->producer_ != NULL){
//		vrt_producer_free(this->producer_);
//		this->producer_ = NULL;
//		clog_info("[%s] release tunnrpt_producer.", module_name_);
//	}

    if (api_) {
        DestroyExchange(api_);
        api_ = NULL;
    }
}

void TunnRptProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
    TiXmlElement *tunn_node = RootElement->FirstChildElement("Tunnel");
	if (tunn_node != NULL){
		string tunn_conf = tunn_node->Attribute("tunnelConfig");
		strcpy(config_.tunnel_cfg_path, tunn_conf.c_str());
		string tunnel_so = tunn_node->Attribute("tunnelSo");
		strcpy(config_.tunnel_so_path, tunnel_so.tunnel_so_file.c_str());
		tunn_node->QueryIntAttribute("initPosAtStart", &config_.init_pos_at_start);
		tunn_node->QueryIntAttribute("modelCtrlOc", &config_.st_ctrl_oc);
		tunn_node->QueryIntAttribute("changeOcFlag", &config_.change_oc_flag);
		tunn_node->QueryIntAttribute("initPosFromEv", &config_.init_pos_from_ev);	

		clog_info("[%s] tunn config:tunnelConfig:%s; tunnelSo:%s; "
					"initPosAtStart:%d; modelCtrlOc:%d"
					"changeOcFlag:%d; initPosFromEv:%d",
					module_name_, 
					this->config_.tunnel_cfg_path,
					this->config_.tunnel_so_path,
					this->config_.init_pos_at_start,
					this->config_.st_ctrl_oc,
					this->config_.change_oc_flag,
					this->config_.init_pos_from_ev);
	} else { 
		clog_error("[%s] x-trader.config error: Tunnel node missing.", module_name_); 
	}
}

int TunnRptProducer::ReqOrderInsert(const T_PlaceOrder *p)
{
	int ret = 0;
	api_->PlaceOrder(p);
	return ret;
}

// 撤单操作请求
int TunnRptProducer::ReqOrderAction(const T_CancelOrder *p)
{
	int ret = 0;
	api_->CancelOrder(p);
	return ret;
}

void TunnRptProducer::End()
{
	ended_ = true;
	(vrt_producer_eof(producer_));
}

void TunnRptProducer::OnRspInsertOrder(const T_OrderRespond *pfield,
			const T_PositionData *pos)
{
    clog_debug("[%s] OnRspInsertOrder ended_:%d", ended_);
	if (ended_) return;

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = pfield->serial_no;
	rpt.OrderStatus = pfield->entrust_status;
	rpt.ErrorID = perror->error_no;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRspInsertOrder: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}

int32_t TunnRptProducer::Push(const TunnRpt& rpt)
{
	static int32_t cursor = RPT_BUFFER_SIZE - 1;
	cursor++;
	if (cursor%RPT_BUFFER_SIZE == 0){
		cursor = 0;
	}
	rpt_buffer_[cursor] = rpt;

	clog_debug("[%s] push TunnRpt: cursor,%d; LocalOrderID:%ld;",
				module_name_, cursor, rpt.LocalOrderID);

	return cursor;
}

void TunnRptProducer::OnRspCancelOrder(const T_CancelRespond *pfield)
{
    clog_debug("[%s] OnRspCancelOrder ended_:%d", ended_);
	if (ended_) return;

	struct TunnRpt rpt;
	memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = pfield->serial_no;
	rpt.OrderStatus = pfield->entrust_status;
	rpt.ErrorID = pfield->error_no;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnMatchedInfo(const T_TradeReturn *pfield, const T_PositionData *pos)
{
    clog_debug("[%s] OnRtnMatchedInfo ended_:%d", ended_);
	if (ended_) return;

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = pfield->serial_no;
	// 因MyExchange成交回报没有状态字段，故在接收是杜撰一个“部分成交状态”
	rpt.OrderStatus = MY_TNL_OS_PARTIALCOM;	
	rpt.MatchedAmount = pfield->business_volume;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnOrder(const T_OrderReturn *ord_rtn, const T_PositionData * pos)
{
	// TODO:
	
	
    clog_debug("[%s] OnRtnOrderended_:%d", ended_);

	if (ended_) return;

    clog_debug("[%s] OnRtnOrder:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	if (pfield->OrderStatus == X1_FTDC_SPD_ERROR){
		clog_warning("[%s] OnRtnOrder:%s",
			module_name_,
			X1DatatypeFormater::ToString(pfield).c_str());
	}

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = pfield->LocalOrderID;
	rpt.OrderStatus = pfield->OrderStatus;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRtnOrder: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField* pfield)
{
    clog_debug("[%s] OnRtnCancelOrder:%d", ended_);

	if (ended_) return;

    clog_debug("[%s] OnRtnCancelOrder:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	if (pfield->OrderStatus == X1_FTDC_SPD_ERROR){
		clog_warning("[%s] OnRtnCancelOrder:%s",
			module_name_,
			X1DatatypeFormater::ToString(pfield).c_str());
	}

	if (pfield->OrderStatus == X1_FTDC_SPD_CANCELED ||
		pfield->OrderStatus == X1_FTDC_SPD_PARTIAL_CANCELED ||
		pfield->OrderStatus == X1_FTDC_SPD_IN_CANCELED){
		struct TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = pfield->LocalOrderID;
		rpt.OrderStatus = pfield->OrderStatus;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = Push(rpt);
		ivalue->data = TUNN_RPT;

		clog_debug("[%s] OnRtnCancelOrder: index,%d; data,%d; LocalOrderID:%ld",
					module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

		(vrt_producer_publish(producer_));
	}
}

long TunnRptProducer::NewLocalOrderID(int32_t strategyid)
{
	static int32_t counter = 1;
    long localorderid = strategyid+ counter * 1000;		
	counter++;

	return localorderid;
}

const char* TunnRptProducer::GetAccount()
{
	return config_.userid.c_str();
}

TunnRpt* TunnRptProducer::GetRpt(int32_t index)
{
	return &(rpt_buffer_[index]);
}

int32_t TunnRptProducer::GetStrategyID(TunnRpt& rpt)
{
	return rpt.LocalOrderID % 1000;
}
