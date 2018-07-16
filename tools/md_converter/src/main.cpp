#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>
#include<stdlib.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <cstddef>        // std::size_t
#include "quote_cmn_save.h"
#include "quote_datatype_shfe_my.h"

using namespace std;

void ConvertLine(char *src_line, QuoteDataSave<MYShfeMarketData> *save_obj)
{
	MYShfeMarketData target_data;
	string line = src_line;
	int found = 0;
	int cur_pos = 0;
	// receivetime	
	found = line.find_first_of("|", cur_pos);
	cur_pos = found+1;
	// tradingdate
	found = line.find_first_of("|", cur_pos);
	string tmp = line.substr(cur_pos, found-cur_pos);
	strcpy(target_data.TradingDay, line.substr(cur_pos, found-cur_pos).c_str());
	cur_pos = found+1;
	// settleid
	found = line.find_first_of("|", cur_pos);
	target_data.SettlementID = 0;
	cur_pos = found+1;
	// lastprice
	found = line.find_first_of("|", cur_pos);
	target_data.LastPrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// presettleprice
	found = line.find_first_of("|", cur_pos);
	target_data.PreSettlementPrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// precloseprice
	found = line.find_first_of("|", cur_pos);
	target_data.PreClosePrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// preopeninterest
	found = line.find_first_of("|", cur_pos);
	target_data.PreOpenInterest = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// openprice
	found = line.find_first_of("|", cur_pos);
	target_data.OpenPrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// highestprice
	found = line.find_first_of("|", cur_pos);
	target_data.HighestPrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// lowestprice
	found = line.find_first_of("|", cur_pos);
	target_data.LowestPrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// volume
	found = line.find_first_of("|", cur_pos);
	target_data.Volume = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// turnover
	found = line.find_first_of("|", cur_pos);
	target_data.Turnover = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// openinterest
	found = line.find_first_of("|", cur_pos);
	target_data.OpenInterest = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// closeprice
	found = line.find_first_of("|", cur_pos);
	target_data.ClosePrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// settleprice
	found = line.find_first_of("|", cur_pos);
	target_data.SettlementPrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// upperlimitprice
	found = line.find_first_of("|", cur_pos);
	target_data.UpperLimitPrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// lowerlimitprice
	found = line.find_first_of("|", cur_pos);
	target_data.LowerLimitPrice = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// predelta
	found = line.find_first_of("|", cur_pos);
	target_data.PreDelta = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// currdelta
	found = line.find_first_of("|", cur_pos);
	target_data.CurrDelta = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// time
	found = line.find_first_of("|", cur_pos);
	string time_str = line.substr(cur_pos, found-cur_pos);
	long time = stol(time_str);
	sprintf(target_data.UpdateTime, "%02d:%02d:%02d", time/10000000, time%10000000/100000, time%100000/1000);
	cur_pos = found+1;
	// millisec
	found = line.find_first_of("|", cur_pos);
	target_data.UpdateMillisec = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// symbol
	found = line.find_first_of("|", cur_pos);
	strcpy(target_data.InstrumentID, line.substr(cur_pos, found-cur_pos).c_str());
	cur_pos = found+1;
	// bidprice1
	found = line.find_first_of("|", cur_pos);
	target_data.BidPrice1 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidvol1
	found = line.find_first_of("|", cur_pos);
	target_data.BidVolume1 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askprice1
	found = line.find_first_of("|", cur_pos);
	target_data.AskPrice1 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askvol1
	found = line.find_first_of("|", cur_pos);
	target_data.AskVolume1 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidprice2
	found = line.find_first_of("|", cur_pos);
	target_data.BidPrice2 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidvol2
	found = line.find_first_of("|", cur_pos);
	target_data.BidVolume2 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askprice2
	found = line.find_first_of("|", cur_pos);
	target_data.AskPrice2 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askvol2
	found = line.find_first_of("|", cur_pos);
	target_data.AskVolume2 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidprice3
	found = line.find_first_of("|", cur_pos);
	target_data.BidPrice3 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidvol3
	found = line.find_first_of("|", cur_pos);
	target_data.BidVolume3 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askprice3
	found = line.find_first_of("|", cur_pos);
	target_data.AskPrice3 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askvol3
	found = line.find_first_of("|", cur_pos);
	target_data.AskVolume3 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidprice4
	found = line.find_first_of("|", cur_pos);
	target_data.BidPrice4 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidvol4
	found = line.find_first_of("|", cur_pos);
	target_data.BidVolume4 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askprice4
	found = line.find_first_of("|", cur_pos);
	target_data.AskPrice4 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askvol4
	found = line.find_first_of("|", cur_pos);
	target_data.AskVolume4 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidprice5
	found = line.find_first_of("|", cur_pos);
	target_data.BidPrice5 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidvol5
	found = line.find_first_of("|", cur_pos);
	target_data.BidVolume5 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askprice5
	found = line.find_first_of("|", cur_pos);
	target_data.AskPrice5 = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askvol5
	found = line.find_first_of("|", cur_pos);
	target_data.AskVolume5 = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// calendardate
	found = line.find_first_of("|", cur_pos);
	cur_pos = found+1;
	// dataflag
	found = line.find_first_of("|", cur_pos);
	target_data.data_flag = 6;
	cur_pos = found+1;
	// startindex
	found = line.find_first_of("|", cur_pos);
	target_data.start_index = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// datacount
	found = line.find_first_of("|", cur_pos);
	target_data.data_count = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bprice1-bprice30
	for(int i=0; i<30; i++){
		found = line.find_first_of("|", cur_pos);
		target_data.buy_price[29-i] = stod(line.substr(cur_pos, found-cur_pos));
		cur_pos = found+1;
	}
	// bvol1-bvol30
	for(int i=0; i<30; i++){
		found = line.find_first_of("|", cur_pos);
		target_data.buy_volume[29-i] = stoi(line.substr(cur_pos, found-cur_pos));
		cur_pos = found+1;
	}
	// sprice1-sprice30
	for(int i=0; i<30; i++){
		found = line.find_first_of("|", cur_pos);
		target_data.sell_price[29-i] = stod(line.substr(cur_pos, found-cur_pos));
		cur_pos = found+1;
	}
	// svol1-svol30
	for(int i=0; i<30; i++){
		found = line.find_first_of("|", cur_pos);
		target_data.sell_volume[29-i] = stoi(line.substr(cur_pos, found-cur_pos));
		cur_pos = found+1;
	}
	// totalbidvol
	found = line.find_first_of("|", cur_pos);
	target_data.buy_total_volume = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// totalaskvol
	found = line.find_first_of("|", cur_pos);
	target_data.sell_total_volume = stoi(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// bidwavgprice
	found = line.find_first_of("|", cur_pos);
	target_data.buy_weighted_avg_price = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;
	// askwavgprice
	found = line.find_first_of("|", cur_pos);
	target_data.sell_weighted_avg_price = stod(line.substr(cur_pos, found-cur_pos));
	cur_pos = found+1;

    timeval t;
    gettimeofday(&t, NULL);
    save_obj->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &target_data);
}

void Convert(string &src, QuoteDataSave<MYShfeMarketData> *save_obj)
{
	char line[4096];
	std::ifstream ifs(src, std::ifstream::in);
	// ignore title line
	ifs.getline(line, 4096);
	while (ifs.getline(line, 4096)){
		ConvertLine(line, save_obj);
	}

	ifs.close();
}

int main(int argc, const char **argv)
{
	string base = argv[1];
	string src = base + ".txt";
	string dest = base + ".dat";

    QuoteDataSave<MYShfeMarketData> *save_obj = 
		new QuoteDataSave<MYShfeMarketData>(dest, MY_SHFE_MD_QUOTE_TYPE);

	Convert(src, save_obj);

	delete save_obj;

	return 0;
}



