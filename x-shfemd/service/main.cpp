#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>
#include <iostream>     // std::cout, std::end

#include <signal.h>     /* signal */
#include "quote_interface_shfe_my.h"

using namespace std;

static void
SIG_handler(int s)
{
	//exit(0);		/* call exit for the signal */
}

int main(/*int argc, const char **argv*/)
{
	SubscribeContracts subs;
	MYQuoteData data_svc(&subs, "");

	string input = "";
	while(input != "Q"){
		cout << "type Q to quit:" << endl;
		cin >> input;
	}
	return 0;
}



