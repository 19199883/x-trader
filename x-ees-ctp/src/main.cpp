#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>

#include <signal.h>     /* signal */
#include "vrt_value_obj.h"
#include "l1md_producer.h"
#include "fulldepthmd_producer.h"
#include "tunn_rpt_producer.h"
#include "uni_consumer.h"
#include "pos_calcu.h"


/* Note that the parameter for queue size is a power of 2. */
#define  QUEUE_SIZE  32768
UniConsumer *uniConsumer = NULL;
FullDepthMDProducer *fulldepth_md_producer = NULL;
L1MDProducer *l1_md_producer = NULL; 
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
	clog_set_minimum_level(CLOG_LEVEL_WARNING);
	FILE *fp;/*文件指针*/
	fp=fopen("./x-trader.log","w+");
	Log::fp = fp;
	struct clog_handler *clog_handler = clog_stream_handler_new_fp(fp, true, "%l %m");
	clog_handler_push_process(clog_handler);

#ifdef TCPDIRECT
	clog_warning("TCPDIRECT on"); 
#else
	clog_warning("TCPDIRECT off"); 
#endif

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
	clog_warning("PERSISTENCE_ENABLED off"); 
#endif


#ifdef INE_ENABLE
	clog_warning("INE_ENABLE"); 
#else
	clog_warning("INE_DISABLE"); 
#endif


	struct sigaction SIGINT_act;
	SIGINT_act.sa_handler = SIG_handler;
	sigemptyset(&SIGINT_act.sa_mask);
	SIGINT_act.sa_flags = 0;
	sigaction(SIGUSR2, &SIGINT_act, NULL);

	// version
	clog_warning("version:x-ees_2019-10-31"); 
	clog_warning("max contract count:%d",MAX_CONTRACT_COUNT ); 

	struct vrt_queue  *queue;
	int64_t  result;

	rip_check(queue = vrt_queue_new("x-trader queue", vrt_hybrid_value_type(), QUEUE_SIZE));
	tunnRptProducer = new TunnRptProducer(queue);
	fulldepth_md_producer = new FullDepthMDProducer(queue);
	l1_md_producer = new L1MDProducer(queue);
	uniConsumer = new UniConsumer (queue, fulldepth_md_producer, l1_md_producer, tunnRptProducer);
	uniConsumer->Start();
	fflush (fp);

  // free vrt_queue
	vrt_queue_free(queue);

	delete uniConsumer;
	delete tunnRptProducer; 
	delete l1_md_producer;
	 delete fulldepth_md_producer; 

// clog: free resources
	pos_calc::destroy_instance();

	clog_handler_free(clog_handler);

	return 0;
}



