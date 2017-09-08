#ifndef _MY_CMN_LOG_H_
#define _MY_CMN_LOG_H_

#include <unistd.h>
#include <pthread.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <strings.h>

using namespace std;

namespace my_cmn
{
enum {
	LOG_MOD_EXCHANGE = 0,
	LOG_MOD_TUNNEL,
	LOG_MOD_QUOTE,
	LOG_MOD_SIGNAL,
	LOG_MOD_OSS,
	LOG_MOD_MAX
};

enum {
	LOG_PRI_CRIT = 0,
	LOG_PRI_ERROR,
	LOG_PRI_WARN,
	LOG_PRI_INFO,
	LOG_PRI_DEBUG,
	LOG_PRI_MAX
};

enum {
	LOG_SUCCESS = 0,
	LOG_ERR_CACHEFULL,
	LOG_ERR_HAS_STOPPED,
	LOG_ERR_FILE_OP,
	LOG_ERR_UNKNOWN,
	LOG_ERR_MAX
};

enum {
	LOG_STATE_INIT = 0,
	LOG_STATE_RUN,
	LOG_STATE_LASTCHANCE,
	LOG_STATE_STOPPED,
	LOG_STATE_MAX
};

#define LOG_ENTRY_MAX	10000
#define LOG_ENTRY_SIZE  2048
#define LOG_ENTRY_NUM   LOG_ENTRY_MAX
#define MAX_PATH	256

typedef struct _log_entry {
	unsigned long seq_no;
	struct timespec ts;
	unsigned char mod_id;
	unsigned char priority;
	char *buff;
} log_entry_t;


/*
 * The log will write the log into specified file name in the working path.
 * And the log file will be named as <program_name>.log, for example,
 * exchange.log.
 *
 * Every log will be as:
 *
 *     [log_seq_no] [Date & Time] [prog_name]/(proc_id) [module_name] [level] [msg]
 *
 * For example:
 *
 *     01322 2014-01-03 22:41:32:374832 exchange/23456 signal INFO hi83_IF sig:c|||Y|s|N;s|||3|d|G|IF1312|09:22:47|BPK|2427.2|0.0|0|0
 *     01323 2014-01-03 22:41:32:375317 exchange/23456 exchange INFO feed pos to hi83_IF£ºIF1312|5|0|0|0|5|-12134.00|
 */

class my_log
{
public:
	static my_log* instance(const char *file, int size = 0, int num = 0);

	/*
	 * 0: indicate the log successful.
	 * 1: indicate the log cache is full
	 * 2: indicate the log thread is stopped.
	 * 3: indicate some errors happened in file op.
	 * 4: unknown errors
	 */
	int log(int mod_id, int priority, const char *format, ...);

	int shutdown(void);

	int set_log_entry_size(int size) { return 0; }
	int set_log_entries_num(int num) { return 0; }
	int set_log_file(const char *file, const char *path = NULL);
	int set_log_level(int level) {
		return _level = level;
	}

	int get_log_level(void) const {
		return _level;
	}

	FILE *get_log_file(void);

protected:
	int is_empty(void) const {
		if (_next == _written) {
			return 1;
		}

		return 0;
	}

	int is_full(void) const {
		if ((((_next + 1) - _written) % _entries_num) == 0) {
			return 1;
		}

		return 0;
	}

	unsigned int log_count(void) const{ return 0; }

	// int init(void);

	int get_proc(void);

	void split_log(void);

private:
	my_log(void):_log_file(NULL), _state(0), _level(LOG_PRI_DEBUG) {}

	~my_log(void){}

	void destroy(void);

	static void* _log_worker(void *args);

private:
	static my_log *_log_inst;

	FILE *_log_file;
	char _log_path[MAX_PATH];

	pthread_t _log_thd_id;
	pthread_mutex_t _log_mutex;

	log_entry_t *_log_entries;
	log_entry_t *_written;
	log_entry_t *_next;

	int _entries_num;
	int _entry_size;

	/* process information */
	pid_t _log_pid;
	char _log_proc[32];

	int _state;

	int _level;
};
}

// quote log macros
#define MY_LOG_DEBUG(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_QUOTE, my_cmn::LOG_PRI_DEBUG, format, ##args)
#define MY_LOG_INFO(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_QUOTE, my_cmn::LOG_PRI_INFO, format, ##args)
#define MY_LOG_WARN(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_QUOTE, my_cmn::LOG_PRI_WARN, format, ##args)
#define MY_LOG_ERROR(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_QUOTE, my_cmn::LOG_PRI_ERROR, format, ##args)
#define MY_LOG_FATAL(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_QUOTE, my_cmn::LOG_PRI_CRIT, format, ##args)

//#define MY_LOG_DEBUG(format, args...)
//#define MY_LOG_INFO(format, args...)
//#define MY_LOG_WARN(format, args...)
//#define MY_LOG_ERROR(format, args...)
//#define MY_LOG_FATAL(format, args...)

// tunnel log macros
#define TNL_LOG_DEBUG(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_TUNNEL, my_cmn::LOG_PRI_DEBUG, format, ##args)
#define TNL_LOG_INFO(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_TUNNEL, my_cmn::LOG_PRI_INFO, format, ##args)
#define TNL_LOG_WARN(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_TUNNEL, my_cmn::LOG_PRI_WARN, format, ##args)
#define TNL_LOG_ERROR(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_TUNNEL, my_cmn::LOG_PRI_ERROR, format, ##args)
#define TNL_LOG_FATAL(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_TUNNEL, my_cmn::LOG_PRI_CRIT, format, ##args)

// exchange log macros
//#define EX_LOG_DEBUG(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_EXCHANGE, my_cmn::LOG_PRI_DEBUG, format, ##args)
#define EX_LOG_DEBUG(format, args...)
#define EX_LOG_INFO(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_EXCHANGE, my_cmn::LOG_PRI_INFO, format, ##args)
#define EX_LOG_WARN(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_EXCHANGE, my_cmn::LOG_PRI_WARN, format, ##args)
#define EX_LOG_ERROR(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_EXCHANGE, my_cmn::LOG_PRI_ERROR, format, ##args)
#define EX_LOG_FATAL(format, args...) my_cmn::my_log::instance(NULL)->log(my_cmn::LOG_MOD_EXCHANGE, my_cmn::LOG_PRI_CRIT, format, ##args)

#endif

