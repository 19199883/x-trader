// done
#ifndef _QUOTE_CMN_SAVE_H_
#define _QUOTE_CMN_SAVE_H_

#include <pthread.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <boost/tuple/tuple.hpp>
#include <thread>
#include <functional>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */

#include "my_cmn_util_funcs.h"
#include "quote_cmn_utility.h"
#include "vrt_value_obj.h"

// 行情类型标识定义
// #define GTAEX_CFFEX_QUOTE_TYPE          1 // obsolete; replace by type 11
#define DCE_MDBESTANDDEEP_QUOTE_TYPE    2
#define DCE_ARBI_QUOTE_TYPE             3
#define DCE_MDTENENTRUST_QUOTE_TYPE     4
#define DCE_MDORDERSTATISTIC_QUOTE_TYPE 5
#define DCE_MDREALTIMEPRICE_QUOTE_TYPE  6
#define DCE_MDMARCHPRICEQTY_QUOTE_TYPE  7
//#define CTP_MARKETDATA_QUOTE_TYPE       8  // obsolete; replace by type 10
#define SHFE_DEEP_QUOTE_TYPE            9
#define SHFE_EX_QUOTE_TYPE              10

#define GTA_UDP_CFFEX_QUOTE_TYPE        11

#define MY_SHFE_MD_QUOTE_TYPE           12
// TDF 股票行情和指数行情类型
#define TDF_STOCK_QUOTE_TYPE            13
#define TDF_INDEX_QUOTE_TYPE            14
// my derivative data of stock market
#define MY_STOCK_QUOTE_TYPE             15

// CZCE Market data id
#define CZCE_LEVEL2_QUOTE_TYPE          16
#define CZCE_CMB_QUOTE_TYPE             17

// ksg gold of sh
#define SH_GOLD_KSG_QUOTE_TYPE          18

// 台湾合并行情类型
#define TAI_FEX_MD_TYPE                 19

// 郑州易盛启明星行情类型
#define TAP_QUOTE_TYPE                20

// A50 depth data (build from history data)
#define SGXA50_MARKETDATA_TYPE       21

// IB Api 行情类型
#define IB_QUOTE_TYPE                22
#define IB_QUOTE_TICK_TYPE           23

//飞鼠
#define SGIT_QUOTE_TYPE             24

// added on 20190521 for Yao
#define YAO_QUOTE_TYPE             125

// data identities of kmds {"stockcode", "stockquote", "indexquote", "optionquote", "ordqueue", "perentrust", "perbargain"};
#define KMDS_CODETABLE_TYPE         0xc0
#define KMDS_STOCK_SNAPSHOT_TYPE    0xc1
#define KMDS_INDEX_TYPE             0xc2
#define KMDS_OPTION_QUOTE_TYPE      0xc3
#define KMDS_ORDER_QUEUE_TYPE       0xc4
#define KMDS_PER_ENTRUST_TYPE       0xc5
#define KMDS_PER_BARGAIN_TYPE       0xc6
#define KMDS_FUTURE_QUOTE_TYPE      0xc7

// 二进制存储的文件头信息
struct SaveFileHeaderStruct
{
    int data_count;     // number of quote data items
    short data_type;    // 行情类型标识
    short data_length;  // length of one item (byte)
};

template<typename DataType>
struct SaveDataStruct
{
    long long t_;                  // 时间戳
    DataType data_;                // 数据对象 （8字节对齐）

    // 缺省构造
    SaveDataStruct()
    {
        t_ = 0;
    }

    // 通过时间戳、和网络数据包构造
    SaveDataStruct(long long t, const DataType &d)
        : t_(t), data_(d)
    {
    }
};

template<typename DataType>
class QuoteDataSave
{
public:
    QuoteDataSave(const std::string &file_name_prefix, int data_type, bool monitor = true)
    {
        pthread_spin_init(&save_sync_, 0);
        std::string quote_name = file_name_prefix;
        datas_.reserve(10000);

        if (!quote_name.empty())
        {
            quote_name.append("_");
        }

        running_flag_ = true;
        save_thread_ = NULL;
        data_count_ = 0;
        quote_file_ = NULL;

        // 文件存储
        //Create Directory
        std::string full_path = boost::filesystem::initial_path<boost::filesystem::path>().string() + "/Data";
        if (!boost::filesystem::exists(full_path)) {
            (void) boost::filesystem::create_directories(full_path);
        }

        //Create output file
        if (boost::filesystem::exists(full_path)) {
			time_t rawtime;
			struct tm * timeinfo;
			char buffer [80];
			time (&rawtime);
			timeinfo = localtime (&rawtime);
			strftime (buffer,sizeof(buffer),"%Y%m%d",timeinfo);
            std::string strTime = buffer;
            std::string str = full_path + "/" + quote_name + strTime + ".dat";
			quote_file_ = fopen(str.c_str(), "wb+");
			if (quote_file_) {
				MYUTIL_SaveFileHeader<SaveDataStruct<DataType>, SaveFileHeaderStruct>(data_type, quote_file_);
				clog_warning("file <%s> open success, write file header.", str.c_str());
			} else {
				clog_error("file <%s> open failed.", str.c_str());
			}
        } else { clog_warning("data file path not exist."); }

        save_thread_ = new std::thread(std::bind(&QuoteDataSave::SaveImp, this));
    }
    ~QuoteDataSave()
    {
        clog_info("prepare to shutdown save thread.");
        if (!save_thread_) { return; }

        if (save_thread_->joinable()) {
            // 把任务做完
            while (!datas_.empty()){
                usleep(30 * 1000);
            }

            running_flag_ = false;
            save_thread_->join();
        }

        if (quote_file_) { fclose(quote_file_); }
    }

    /**
     * QuoteData Handler
     */
    void OnQuoteData(long long t, const DataType * const p)
    {
        if (!p){
            // write log
            clog_error("receive one null data.");
            return;
        }
        _TTSpinLockGuard lock(save_sync_);
        datas_.push_back(SaveDataStruct<DataType>(t, *p));
    }

private:
    void SaveImp()
    {
        clog_info("save thread started.");
        std::vector<SaveDataStruct<DataType> > datas_t;
        datas_t.reserve(10000);

        while (running_flag_){
            // 将数据交换到本地
            {
                _TTSpinLockGuard lock(save_sync_);
                if (!datas_.empty()){
                    datas_.swap(datas_t);
                }
            }

            // 没事做，让出CPU
            if (datas_t.empty()){
                usleep(50 * 1000);
                continue;
            }

            // 自己慢慢存
			MYUTIL_SaveDataToFile(datas_t, data_count_, quote_file_);

            // 存完清空
            datas_t.clear();
        }
    }

    volatile bool running_flag_;
    std::thread *save_thread_;
    pthread_spinlock_t save_sync_;

    std::vector<SaveDataStruct<DataType> > datas_;
    int data_count_;

    // 文件存储
    FILE * quote_file_;
};

#endif // _QUOTE_CMN_SAVE_H_
