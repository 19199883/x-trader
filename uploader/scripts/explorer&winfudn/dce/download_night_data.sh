#!/bin/bash
SUFFIX="night"

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

# download straties's log files from JR server for account night28
cd $this_dir
STRA_LOG="/home/u910019/explorer/night28/backup/dce_trad_${SUFFIX}_`date +%y%m%d`.tar.gz"
scp -P 44153 -v "u910019@101.231.3.117:${STRA_LOG}" ${STRATEGY_DIR}
cd ${STRATEGY_DIR}
stra_src="dce_trad_${SUFFIX}_`date +%y%m%d`.tar.gz"
tar -v --remove-files -xzf $stra_src
rm $stra_src

# download straties's log files from JR server for account night100
cd $this_dir
STRA_LOG="/home/u910019/explorer/night100/backup/dce_trad_${SUFFIX}_`date +%y%m%d`.tar.gz"
scp -P 44153 -v "u910019@101.231.3.117:${STRA_LOG}" ${STRATEGY_DIR}
cd ${STRATEGY_DIR}
stra_src="dce_trad_${SUFFIX}_`date +%y%m%d`.tar.gz"
tar -v --remove-files -xzf $stra_src
rm $stra_src

#---combine both night28 and night100
cd $this_dir
cd ${STRATEGY_DIR}
stra_dest="dce_trad_${SUFFIX}_`date +%y%m%d`.tar.bz2"
tar -v  --remove-files -cvjf $stra_dest ./log *.log

# download market data from JR server
cd $this_dir
TICK_DATA="/home/u910019/md/download/night/backup/dce_md_${SUFFIX}_`date +%y%m%d`.tar.gz"
scp -P 44153 -v  "u910019@101.231.3.117:${TICK_DATA}" ${TICKDATA_DIR}
cd ${TICKDATA_DIR}
data_src="dce_md_${SUFFIX}_`date +%y%m%d`.tar.gz"
data_dest="dce_md_${SUFFIX}_`date +%y%m%d`.tar.bz2"
tar -v --remove-files -xzf $data_src
tar -v  --remove-files -cvjf $data_dest ./backup
rm -v  $data_src

