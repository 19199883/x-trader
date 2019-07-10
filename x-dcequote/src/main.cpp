#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <chrono>    
#include <signal.h>     /* signal */
#include "vrt_value_obj.h"
#include "md_producer.h"

MDProducer *mdProducer = NULL;
bool stopped = true;

static void
SIG_handler(int s)
{
	stopped = true;
}

int main(/*int argc, const char **argv*/)
{
	stopped = false;
	
	struct sigaction SIGINT_act;
	SIGINT_act.sa_handler = SIG_handler;
	sigemptyset(&SIGINT_act.sa_mask);
	SIGINT_act.sa_flags = 0;
	sigaction(SIGUSR1, &SIGINT_act, NULL);

	// clog setting		   CLOG_LEVEL_WARNING
	clog_set_minimum_level(CLOG_LEVEL_INFO);
	FILE *fp;/*文件指针*/
	fp=fopen("./x-trader.log","w+");
	Log::fp = fp;

	struct clog_handler *clog_handler = clog_stream_handler_new_fp(fp, true, "%l %m");
	clog_handler_push_process(clog_handler);

	// version
	clog_warning("version:x-dcequote_2019-07-10_d"); 
		
	mdProducer = new MDProducer();
	mdProducer->InitMDApi();
	
	while(!stopped){
		std::this_thread::sleep_for (std::chrono::seconds(10));
	}
	mdProducer->End();
	delete mdProducer;
	clog_warning("main exit."); 
	
	fflush (fp);
	clog_handler_free(clog_handler);

	return 0;
}



