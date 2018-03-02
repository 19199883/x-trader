#include "timestamp.h"

#include <math.h>
#include <chrono>
#include <ratio>
#include <ctime>
#include <stdio.h>


using namespace std::chrono;
using namespace std;
void timestamp()
{
	FILE *fp = NULL;
	fp = fopen("test.txt", "w+");

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	fprintf(fp, "%ld\n", t1.time_since_epoch().count());

	fclose(fp);
}


