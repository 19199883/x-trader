#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>     /* signal */
#include "vrt_value_obj.h"
#include "dmd_producer.h"
#include "tunn_rpt_producer.h"
#include "uni_consumer.h"
#include "pos_calcu.h"


/* Note that the parameter for queue size is a power of 2. */
#define  QUEUE_SIZE  32768
UniConsumer *uniConsumer = NULL;
DMDProducer *depth_md_producer = NULL;
RTDProducer *realtime_md_producer = NULL;
OBDProducer *orderbook_md_producer = NULL;
TVDProducer *tradevolume_md_producer = NULL;
TunnRptProducer *tunnRptProducer = NULL;

static void
SIG_handler(int s)
{
	uniConsumer->Stop();
	//exit(0);		/* call exit for the signal */
}

bool udp_test()
{
	setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 
 
	// 绑定地址
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){   
		cout<<"socket error"<<endl;	
		return false;
	}else{
		cout<<"socket right"<<endl;	
	}
 
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_ANY);
	addrto.sin_port = htons(27888);
	if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) < 0){   
		cout<<"bind error..."<<endl;
		return false;
	}else{
		cout<<"bind right..."<<endl;
	}
	
	const int opt = 1;
	int nb = 0;
	nb = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&opt, sizeof(opt));
	if(nb < 0){
		cout<<"set socket error..."<<endl;
		return false;
	}else{
		cout<<"set socket right..."<<endl;
	}
	     
	/*加入多播组*/
	struct ip_mreq mreq;   
	mreq.imr_multiaddr.s_addr = inet_addr("238.0.0.1"); /*多播地址*/
	mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*网络接口为默认*/

	/*将本机加入多播组*/
	int err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));//ASM模型
	if (err < 0){
		perror("setsockopt():IP_ADD_MEMBERSHIP");
		return -4;
	}else{
		cout<<"setsockopt():IP_ADD_MEMBERSHIP right..."<<endl;
		perror("");
	}
 
 
	int len = sizeof(sockaddr_in);
	char smsg[1024] = {0};
 
	while(1)
	{
		//从广播地址接受消息
		int ret=recvfrom(sock, smsg, 1024, 0, (struct sockaddr*)&addrto,(socklen_t*)&len);
		if(ret<=0)
		{
			cout<<"read error...."<<sock<<endl;
		}
		else
		{		
			printf("%s\t", "rev");	
			printf("%s\t", smsg);	
		}
 
		sleep(1);
	}
 
	return 0;
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
	struct sigaction SIGINT_act;
	SIGINT_act.sa_handler = SIG_handler;
	sigemptyset(&SIGINT_act.sa_mask);
	SIGINT_act.sa_flags = 0;
	sigaction(SIGUSR1, &SIGINT_act, NULL);

	// version
	clog_warning("version:x-ees_2018-10-22_r"); 
	clog_warning("max contract count:%d",MAX_CONTRACT_COUNT ); 

	struct vrt_queue  *queue;
	int64_t  result;

	rip_check(queue = vrt_queue_new("x-trader queue", vrt_hybrid_value_type(), QUEUE_SIZE));
	tunnRptProducer = new TunnRptProducer(queue);
	depth_md_producer = new DMDProducer(queue);
	realtime_md_producer = new RTDProducer(queue);
	orderbook_md_producer = new OBDProducer(queue);
	tradevolume_md_producer = new TVDProducer(queue);
	uniConsumer = new UniConsumer (queue, depth_md_producer, realtime_md_producer,
				orderbook_md_producer, tradevolume_md_producer, tunnRptProducer);
	uniConsumer->Start();
	fflush (fp);

  // free vrt_queue
	vrt_queue_free(queue);

	delete uniConsumer;
	delete tunnRptProducer; 
	 delete depth_md_producer; 

// clog: free resources
	pos_calc::destroy_instance();

	clog_handler_free(clog_handler);

	return 0;
}



