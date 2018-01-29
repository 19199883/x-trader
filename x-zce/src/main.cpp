#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>

#include <signal.h>     /* signal */
#include "vrt_value_obj.h"
#include "l2md_producer.h"
#include "tap_md_producer.h"
#include "tunn_rpt_producer.h"
#include "uni_consumer.h"
#include "pos_calcu.h"
#include "esunny_data_formater.h"

/* Note that the parameter for queue size is a power of 2. */
#define  QUEUE_SIZE  4096
UniConsumer *uniConsumer = NULL;
L2MDProducer *l2_md_producer = NULL;
TapMDProducer *l1_md_producer = NULL; 
TunnRptProducer *tunnRptProducer = NULL;

static void
SIG_handler(int s)
{
	uniConsumer->Stop();
}

int main(/*int argc, const char **argv*/)
{
#ifdef LATENCY_MEASURE
	clog_warning("latency measure on"); 
#endif

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
	clog_warning("version:x-zce_20180126_r"); 
	
	struct vrt_queue  *queue;
	int64_t  result;

	rip_check(queue = vrt_queue_new("x-trader queue", vrt_hybrid_value_type(), QUEUE_SIZE));
	l2_md_producer = new L2MDProducer(queue);
	l1_md_producer = new TapMDProducer(queue);
	tunnRptProducer = new TunnRptProducer(queue);
	uniConsumer = new UniConsumer (queue, l1_md_producer, l2_md_producer, tunnRptProducer);
	uniConsumer->Start();

	clog_warning("main exit."); 
	
	fflush (fp);

  // free vrt_queue
	vrt_queue_free(queue);

  delete uniConsumer;
  delete tunnRptProducer; 
	delete l2_md_producer; 
	delete l1_md_producer;

// clog: free resources
	pos_calc::destroy_instance();
	clog_handler_free(clog_handler);

	return 0;
}



