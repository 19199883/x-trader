#!/bin/bash
SUFFIX="day"
program_name="sh_tra_${SUFFIX}"

STRATEGY_DIR="./stra_log"
TICKDATA_DIR="./tickdata"

# obtain the directory where this script file locates.
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
STRA_LOG="/home/u910019/trade/day056/backup/sh_stra_${SUFFIX}_`date +%y%m%d`.tar.gz"
STRA_LOG_GZ="sh_stra_${SUFFIX}_`date +%y%m%d`.tar.gz"
STRA_LOG_BJ2="sh_stra_${SUFFIX}_`date +%y%m%d`.tar.bz2"
scp  -P 44163  "u910019@101.231.3.117:${STRA_LOG}" ${STRATEGY_DIR}
cd ./stra_log
tar -xvzf $STRA_LOG_GZ
rm $STRA_LOG_GZ
tar -cvjf $STRA_LOG_BJ2 ./backup
rm -r ./backup
cd ..



