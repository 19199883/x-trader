#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <signal.h>     /* signal */
#include "vrt_value_obj.h"
#include "l2md_producer.h"
#include "tap_md_producer.h"
#include "tunn_rpt_producer.h"
#include "uni_consumer.h"
#include "pos_calcu.h"
#include "ctp_data_formater.h"

/* Note that the parameter for queue size is a power of 2. */
#define  QUEUE_SIZE  4096
UniConsumer *uniConsumer = NULL;
DceMDProducer *dcemd_producer = NULL;
TapMDProducer *l1_md_producer = NULL; 
TunnRptProducer *tunnRptProducer = NULL;

static void
SIG_handler(int s)
{
	uniConsumer->Stop();
}

int main(/*int argc, const char **argv*/)
{

	// TODO: debug
	int a = sizeof(int);
	int test = 200000000;
	struct sigaction SIGINT_act;
	SIGINT_act.sa_handler = SIG_handler;
	sigemptyset(&SIGINT_act.sa_mask);
	SIGINT_act.sa_flags = 0;
	sigaction(SIGUSR2, &SIGINT_act, NULL);

	// clog setting		   CLOG_LEVEL_WARNING
	clog_set_minimum_level(CLOG_LEVEL_INFO);
	FILE *fp;/*文件指针*/
	fp=fopen("./x-trader.log","w+");

	Log::fp = fp;

	struct clog_handler *clog_handler = clog_stream_handler_new_fp(fp, true, "%l %m");
	clog_handler_push_process(clog_handler);

	clog_warning("test..."); 
#ifdef LATENCY_MEASURE
	clog_warning("latency measure on"); 
#else
	clog_warning("latency measure off"); 
#endif

#ifdef COMPLIANCE_CHECK
	clog_warning("COMPLIANCE_CHECK on"); 
#else
	clog_warning("COMPLIANCE_CHECK off"); 
#endif

#ifdef PERSISTENCE_ENABLED
	clog_warning("PERSISTENCE_ENABLED on"); 
#else
	clog_warning("PERSISTENCE_ENABLEDon off"); 
#endif
	// version
	clog_warning("version:x-zce_2019-03-26_d"); 
	
	struct vrt_queue  *queue;
	int64_t  result;

	rip_check(queue = vrt_queue_new("x-trader queue", vrt_hybrid_value_type(), QUEUE_SIZE));
	dcemd_producer = new DceMDProducer(queue);
	//l1_md_producer = new TapMDProducer(queue);
	tunnRptProducer = new TunnRptProducer(queue);
	while(!tunnRptProducer->IsReady()){
		std::this_thread::sleep_for (std::chrono::seconds(1));
	}
	uniConsumer = new UniConsumer (queue, dcemd_producer, tunnRptProducer);
	uniConsumer->Start();

	clog_warning("main exit."); 
	
	fflush (fp);

  // free vrt_queue
	vrt_queue_free(queue);

  delete tunnRptProducer; 
	delete dcemd_producer; 
	delete l1_md_producer;
  delete uniConsumer;

// clog: free resources
	pos_calc::destroy_instance();
	clog_handler_free(clog_handler);

	return 0;
}
