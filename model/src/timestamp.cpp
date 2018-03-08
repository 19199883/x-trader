#include "timestamp.h"

#include <math.h>
#include <chrono>
#include <ratio>
#include <ctime>
#include <stdio.h>


using namespace std::chrono;
using namespace std;
void timestamp(int n)
{
	FILE *fp = NULL;
	fp = fopen("test.txt", "a+");

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	fprintf(fp, "%d: %ld\n", n, duration_cast<std::chrono::nanoseconds>(t1.time_since_epoch()).count());

	fclose(fp);
}


