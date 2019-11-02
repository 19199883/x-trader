#! /bin/bash

# the directory where this script file is
function enter_cur_dir(){
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
enter_cur_dir
rm sc*.pos

python pos_redist.py
