# the directory where this script file is.
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

function convert(){
	 OLD_IFS=$IFS
	 ln_cnt=0
	 for item in `ls *.txt `
	 do  
	   src=`basename $item .txt`
	   echo $src
	   ./md_converter $src
	 done
}	 

ulimit -c unlimited
export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
enter_cur_dir
convert
