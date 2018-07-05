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
#include "quote_cmn_save.h"
#include "quote_datatype_shfe_my.h"

void ConvertLine(char *src_line, QuoteDataSave<MYShfeMarketData> *save_obj)
{
	// TODO: here
	MYShfeMarketData target_data;
	
    timeval t;
    gettimeofday(&t, NULL);
    save_obj->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &target_data);
}

void Convert(string &src, QuoteDataSave<MYShfeMarketData> *save_obj)
{
	char line[4096];
	std::ifstream ifs(src, std::ifstream::in);
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



