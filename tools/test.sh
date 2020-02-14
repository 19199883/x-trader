#!/bin/bash

##################
# !!!!!!!!!!!!!!注意事项！！！！！
# 1. 该脚本必须在交易程序和行情程序启动后启动，
#    否则会产生错误报警信息，如"交易程序异常退出"
# 2. 该计划的时间依赖实盘的交易程序，行情程序的启停时间，
#     该脚本必须在交易程序和行情程序启动后启动；关闭前关闭
#
#
#######################################

# 如下定义该脚本要退出的时间范围
t_str1="02:25:00"
t_str2="08:30:00"
t_str3="14:59:00"
t_str4="20:30:00"
t1=`date -d "$t_str1" +%s`
t2=`date -d "$t_str2" +%s`
t3=`date -d "$t_str3" +%s`
t4=`date -d "$t_str4" +%s`

###########################
# 休市时间
#
##############################
rest_str1="09:00:00"
rest_str2="10:15:00"
rest_str3="10:30:00"
rest_str4="11:30:00"
rest_str5="13:30:00"
rest_str6="15:00:00"
rest_str7="21:00:00"
rest_str8="23:00:00"
rest1=`date -d "$rest_str1" +%s`
rest2=`date -d "$rest_str2" +%s`
rest3=`date -d "$rest_str3" +%s`
rest4=`date -d "$rest_str4" +%s`
rest5=`date -d "$rest_str5" +%s`
rest6=`date -d "$rest_str6" +%s`
rest7=`date -d "$rest_str7" +%s`
rest8=`date -d "$rest_str8" +%s`

# the directory where this script file is.
function enter_cur_dir()
{
	 this_dir=`pwd`
	 dirname $0|grep "^/" >/dev/null
	 if [ $? -eq 0 ];then
		 this_dir=`dirname $0`
	 else
		 dirname $0|grep "^\." >/dev/null
		 retval=$?
		 if [ $retval -eq 0 ];then
			 this_dir=`dirname $0|sed "s#^.#$this_dir#"`
		 else
			 this_dir=`dirname $0|sed "s#^#$this_dir/#"`
		 fi
	 fi

	cd $this_dir
}

function exit_script()
{
	t_str=`date +"%H:%M:%S"`
	t=`date +"%H:%M:%S"`

	echo "now: $t"
}


enter_cur_dir

#######################
# TODO: 需要相应修改
####################
interval=1 # minute

exit_script
echo "it is $t"
sleep 3

exit_script
echo "it is $t"
sleep 3

exit_script
echo "it is $t"
