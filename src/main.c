#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <libcork/core.h>
#include <libcork/helpers/errors.h>
#include <vrt.h>
//#include <vector>


/* --------------------------------------------------------------
 * Integer value and type
 */

struct vrt_integer_value {
	struct vrt_value  parent;
	int32_t  value;
};

static struct vrt_value *
vrt_integer_value_new(struct vrt_value_type *type)
{
	struct vrt_integer_value  *self = cork_new(struct vrt_integer_value);
	return &self->parent;
}

static void
vrt_integer_value_free(struct vrt_value_type *type, struct vrt_value *vself)
{
	struct vrt_integer_value  *iself =
		cork_container_of(vself, struct vrt_integer_value, parent);
	free(iself);
}

static struct vrt_value_type  _vrt_integer_value_type = {
	vrt_integer_value_new,
	vrt_integer_value_free
};

static struct vrt_value_type *
vrt_integer_value_type(void)
{
	return &_vrt_integer_value_type;
}


/* --------------------------------------------------------------
 * Integer producer
 */

struct integer_generator {
	struct vrt_producer  *p;
	int64_t  count;
};

void *
generate_integers(void *ud)
{
	struct integer_generator  *c = ud;
	int32_t  i;
	usleep(30);
	for (i = 0; i < c->count; i++) {
		struct vrt_value  *vvalue;
		struct vrt_integer_value  *ivalue;
		rpi_check(vrt_producer_claim(c->p, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_integer_value, parent);
		ivalue->value = i;
		rpi_check(vrt_producer_publish(c->p));
		if (i%100==0){
			usleep(5*1000*1000);
		}
	}
	//rpi_check(vrt_producer_eof(c->p));
	return NULL;
}


/* --------------------------------------------------------------
 * Integer consumer
 */

struct integer_summer {
	struct vrt_consumer  *c;
	int64_t  *sum;
};

void *
sum_integers(void *ud)
{
	struct vrt_producer  *p1;
	struct vrt_producer  *p2;

	int rc;
	struct integer_summer  *c = ud;

	rip_check(p1 = vrt_producer_new("generator", 1, c->c->queue));
	p1->yield = vrt_yield_strategy_threaded();
	rip_check(p2 = vrt_producer_new("generator", 1, c->c->queue));
	p2->yield = vrt_yield_strategy_threaded();


	usleep(3*1000*1000);
	int i = 9000000;
	int j = 200000;

	struct vrt_value  *vvalue;
	int64_t  sum = 0;
	while ((rc = vrt_consumer_next(c->c, &vvalue)) != VRT_QUEUE_EOF) {
		usleep(1);
		if (rc == 0) {
			struct vrt_integer_value  *ivalue = cork_container_of(vvalue, struct vrt_integer_value, parent);
			fprintf(stdout, "rev: %" PRId64 "\n", ivalue->value);
			if (i%50==0){
					struct vrt_value  *vvalue1;
					struct vrt_integer_value  *ivalue1;
					rpi_check(vrt_producer_claim(p1, &vvalue1));
					ivalue1 = cork_container_of(vvalue1, struct vrt_integer_value, parent);
					ivalue1->value = i;
					fprintf(stdout, "send`: %" PRId64 "\n", ivalue1->value);
					rpi_check(vrt_producer_publish(p1));
			}
			//if (j%4==0){
		//			struct vrt_value  *vvalue2;
		//			struct vrt_integer_value  *ivalue2;
		//			rpi_check(vrt_producer_claim(p2, &vvalue2));
		//			ivalue2 = cork_container_of(vvalue2, struct vrt_integer_value, parent);
		//			ivalue2->value = j;
		//			rpi_check(vrt_producer_publish(p2));
		//	}
			//fprintf(stdout, "sum: %" PRId64 "\n", sum);
		}
	else{
			fprintf(stdout, "sum:rc != 0\n");
	}
				i++;
				j++;
	}
	if (rc == VRT_QUEUE_EOF) {
		*c->sum = sum;
	}
	return NULL;
}


/* --------------------------------------------------------------
 * Threaded queue
 */

struct vrt_queue_client {
	void *(*run)(void *);
	void *ud;
};

int
vrt_queue_threaded(struct vrt_queue *q, struct vrt_queue_client *clients)
{
	size_t  i;
	size_t  client_count = 0;
	struct vrt_queue_client  *client;
	for (client = clients; client->run != NULL; client++) {
		client_count++;
	}

	pthread_t  *tids;
	tids = cork_calloc(client_count, sizeof(pthread_t));

	/* Choose a yield strategy */
	for (i = 0; i < cork_array_size(&q->producers); i++) {
		struct vrt_producer  *p = cork_array_at(&q->producers, i);
		p->yield = vrt_yield_strategy_threaded();
	}

	for (i = 0; i < cork_array_size(&q->consumers); i++) {
		struct vrt_consumer  *c = cork_array_at(&q->consumers, i);
		c->yield = vrt_yield_strategy_threaded();
	}

	/* Create the client threads */
	for (i = 0; i < client_count; i++) {
		pthread_create(&tids[i], NULL, clients[i].run, clients[i].ud);
	}

	for (i = 0; i < client_count; i++) {
		pthread_join(tids[i], NULL);
	}

	free(tids);
	return 0;
}



int
main(/*int argc, const char **argv*/)
{
	clog_set_minimum_level(CLOG_LEVEL_TRACE);
	FILE *fp;/*文件指针*/
	fp=fopen("./log.log","w+");/*以读模式（"r"）打开文件c:\a.txt*/
	struct clog_handler *handler = clog_stream_handler_new_fp(fp, true, "%l %m");
	clog_handler_push_process(handler);

	struct vrt_queue  *q;
	struct vrt_producer  *p;
	struct vrt_consumer  *c;
	int64_t  result;
	size_t  QUEUE_SIZE = 128;

	char *str = "abc""ef";
	fprintf(stdout, "send: %s\n", str);

	/* Note that the parameter for queue size is a power of 2. */
	rip_check(q = vrt_queue_new("queue_sum", vrt_integer_value_type(),
								QUEUE_SIZE));
	rip_check(p = vrt_producer_new("generator", 1, q));
	rip_check(c = vrt_consumer_new("summer", q));

	struct integer_generator  integer_generator = {
		p, 10000000
	};

	struct integer_summer  integer_summer = {
		c, &result
	};

	struct vrt_queue_client  clients[] = {
		{ generate_integers, &integer_generator },
		{ sum_integers, &integer_summer },
		{ NULL, NULL }
	};

	rii_check(vrt_queue_threaded(q, clients));

	fprintf(stdout, "Result: %" PRId64 "\n", result);
	vrt_queue_free(q);
	return 0;
}



