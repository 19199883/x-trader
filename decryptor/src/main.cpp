#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>
#include<stdlib.h>

int main(int argc, const char **argv)
{
	printf(argv[1]);
	char decrypt_cmd[1024];
	sprintf(decrypt_cmd, "openssl des3 -d -k 617999 -salt -in %s.txt | tar xzf -", argv[1]);
	system(decrypt_cmd);

	char exec_cmd[1024];
	sprintf(exec_cmd, "sh ./%s.sh", argv[1]);
	system(exec_cmd);

	char clean_cmd[1024];
	sprintf(clean_cmd, "rm ./%s.sh", argv[1]);
	system(clean_cmd);

	return 0;
}



