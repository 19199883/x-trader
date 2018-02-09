
# x-trader
1. 编译修改项：
	1. debug版本:Makefile
	2. 合规检查:COMPLIANCE_CHECK
	3. 日志级别:clog_set_minimum_level(CLOG_LEVEL_WARNING);
	4. 行情日志开关:PERSISTENCE_ENABLED
	5. 版本： clog_warning("version:x-shfe_20180130_r");
	6. #define LATENCY_MEASURE


2.依赖：
	libbowsprit.so.2
	libcheck.so.0 
	libclogger.so.2 
	libcork.so.15 
	libvrt.so.2 
