#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std;
#include "mdclient.h"

#define MAX_COMMAND_LENGTH 32

class CSimpleHandler : public CMdclientSpi
{
public:
	virtual void OnRtnDepthMarketData(CDepthMarketDataField *pDepthMarketData){
        struct timeval  tv;
        struct timezone tz;
        gettimeofday (&tv , &tz);
        cout<<" get a metket data at time " << tv.tv_sec << ":" << tv.tv_usec <<endl;
        cout<<" quote | contract:"<<pDepthMarketData->InstrumentID
        <<" time:"<<pDepthMarketData->UpdateTime<<":"<<pDepthMarketData->UpdateMillisec
        <<" price:"<<pDepthMarketData->LastPrice
        <<" price:"<<pDepthMarketData->LastPrice
        <<" volume:" << pDepthMarketData->Volume
        <<" low:" << pDepthMarketData->LowestPrice
        <<" askprice1:" << pDepthMarketData->AskPrice1
        <<" AskVolume1:" << pDepthMarketData->AskVolume1
        <<" BidPrice1:" << pDepthMarketData->BidPrice1
        <<" BidVolume1:" << pDepthMarketData->BidVolume1
        <<" OpenInterest:" << pDepthMarketData->OpenInterest
        <<" UpLimit:" << pDepthMarketData->UpperLimitPrice
        <<" LowerLimit:" << pDepthMarketData->LowerLimitPrice
        <<endl;
	}
};

#define UDP_PORT 10074
int main(int argc, const char* argv[]){
    CSimpleHandler* pHand = new CSimpleHandler();
    int listen_port = UDP_PORT; //UDP侦听端口，用缺省值就行了
    CMdclientApi* pApi = CMdclientApi::Create(pHand,listen_port);   //listen_port参数也可以不给，直接用缺省值
	pApi->Subscribe("ag1312");
    pApi->Start();
	char command[MAX_COMMAND_LENGTH];
	while(true){
	    cout << ">";
	    cin.getline(command, MAX_COMMAND_LENGTH);
	    if(strlen(command)>0){
            if(strcmp(command,"exit")==0) break;
	    }
	}
	pApi->Stop();
	return 0;
}

