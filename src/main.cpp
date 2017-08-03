#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <signal.h>     /* signal */
#include "vrt_value_obj.h"
#include "md_producer.h"
#include "pending_sig_producer.h"
#include "tunn_rpt_producer.h"
#include "uni_consumer.h"

/* Note that the parameter for queue size is a power of 2. */
#define  QUEUE_SIZE  128
UniConsumer *uniConsumer = NULL;

static void
SIGINT_handler(int s)
{
	uniConsumer->Stop();
	exit(0);		/* call exit for the signal */
}

extern "C" {
	int main(/*int argc, const char **argv*/)
	{
		struct sigaction SIGINT_act;
		SIGINT_act.sa_handler = SIGINT_handler;
		sigemptyset(&SIGINT_act.sa_mask);
		SIGINT_act.sa_flags = 0;
		sigaction(SIGINT, &SIGINT_act, NULL);

		// clog setting
		clog_set_minimum_level(CLOG_LEVEL_TRACE);
		FILE *fp;/*文件指针*/
		fp=fopen("./x-trader.log","w+");
		struct clog_handler *clog_handler = clog_stream_handler_new_fp(fp, true, "%l %m");
		clog_handler_push_process(clog_handler);

		struct vrt_queue  *queue;
		int64_t  result;

		rip_check(queue = vrt_queue_new("x-trader queue", vrt_hybrid_value_type(), QUEUE_SIZE));
		MDProducer mdproducer(queue);
		PendingSigProducer pendingSigProducer(queue);
		TunnRptProducer tunnRptProducer(queue);
		uniConsumer = new UniConsumer (queue, &mdproducer, &tunnRptProducer, &pendingSigProducer);
		uniConsumer->Start();

		// free vrt_queue
		vrt_queue_free(queue);

		// clog: free resources
		clog_handler_free(clog_handler);
		delete uniConsumer;

		return 0;
	}



};
