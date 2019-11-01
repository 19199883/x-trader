#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>

#include <signal.h>     /* signal */
#include "vrt_value_obj.h"
#include "md_producer.h"
#include "tunn_rpt_producer.h"
#include "uni_consumer.h"
#include "pos_calcu.h"


/* Note that the parameter for queue size is a power of 2. */
#define  QUEUE_SIZE  4096
UniConsumer *uniConsumer = NULL;
MDProducer *mdproducer = NULL;
TunnRptProducer *tunnRptProducer = NULL;

static void
SIG_handler(int s)
{
	uniConsumer->Stop();
	//exit(0);		/* call exit for the signal */
}

int main(/*int argc, const char **argv*/)
{
	// clog setting		   CLOG_LEVEL_WARNING
	clog_set_minimum_level(CLOG_LEVEL_INFO);
	FILE *fp;/*文件指针*/
	fp=fopen("./x-trader.log","w+");
	Log::fp = fp;
	struct clog_handler *clog_handler = clog_stream_handler_new_fp(fp, true, "%l %m");
	clog_handler_push_process(clog_handler);

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

#ifdef LOCK_POSITION_DISNABLE
	clog_warning("LOCK_POSITION_DISNABLE"); 
#endif
#ifdef  LOCK_POSITION_ENABLE	
	clog_warning("LOCK_POSITION_ENABLE"); 
#endif
  
#ifdef ONE_PRODUCT_ONE_CONTRACT
	clog_warning("ONE_PRODUCT_ONE_CONTRACT"); 
#else
	clog_warning("ONE_PRODUCT_MULTI_CONTRACT"); 
#endif

	clog_warning("server name:%s", SERVER_NAME); 

	struct sigaction SIGINT_act;
	SIGINT_act.sa_handler = SIG_handler;
	sigemptyset(&SIGINT_act.sa_mask);
	SIGINT_act.sa_flags = 0;
	sigaction(SIGUSR1, &SIGINT_act, NULL);


	// version
	clog_warning("version:dce_2019-08-28"); 

	struct vrt_queue  *queue;
	int64_t  result;

	rip_check(queue = vrt_queue_new("x-trader queue", vrt_hybrid_value_type(), QUEUE_SIZE));
	mdproducer = new MDProducer(queue);
	tunnRptProducer = new TunnRptProducer(queue);
	uniConsumer = new UniConsumer (queue, mdproducer, tunnRptProducer);
	uniConsumer->Start();

	fflush (fp);

  // free vrt_queue
	vrt_queue_free(queue);

  delete uniConsumer;
  delete tunnRptProducer; 
  delete mdproducer; 

// clog: free resources
	pos_calc::destroy_instance();
	clog_handler_free(clog_handler);

	return 0;
}



