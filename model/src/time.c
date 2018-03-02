#include <math.h>
#include <chrono>
#include <ratio>
#include <ctime>
#include <stdio.h>


using namespace std;
using namespace std::chrono;

void sub_c_(int *,float *,double *);
double func_c_(double *);
double func_d_(double [][2]);


void timestamp()
{
	FILE *fp = NULL;
	fp = fopen("test.txt", "w+");

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	fprintf(fp, "%ld\n", t1.time_since_epoch().count());

	fclose(fp);
}

double func_c_(double *n3)
{
      double n4;
      n4=sqrt(*n3);
      return n4;
}

double func_d_(double var2d[3][2])
{
      double NN;
      NN=77.0;
      printf("%5.2f %5.2f %5.2f \n",var2d[0][0],var2d[1][0],var2d[2][0]);
      printf("%5.2f %5.2f %5.2f \n",var2d[0][1],var2d[1][1],var2d[2][1]);
      var2d[1][0]=NN;
      printf("%5.2f %5.2f %5.2f \n",var2d[0][0],var2d[1][0],var2d[2][0]);
      printf("%5.2f %5.2f %5.2f \n",var2d[0][1],var2d[1][1],var2d[2][1]);
      return NN;
}
