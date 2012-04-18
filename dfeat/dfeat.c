/* Feature parser for skim.me
 * By Xiang Zhang @ New York University
 * Version 0.1, 03/28/2012
 *
 * Usage: ./dtime [method] [hindex] [scale]
 *       [method]: method of handling the time
 *                 0: GMT
 *                 1: Local
 *       [hindex]: Index of the wanted hostname
 *       [scale]:  Scale of seconds to count for (must be day-divisible)
 *
 *       The program accepts input from stdin, and write to stdout
 *       each row output a day of time divided into [scale] seconds, with 1 indicates presence and 0 indicates none.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mba/csv.h>

#define SC_SUCCESS 0
#define SC_FAILURE 1

#define BUF_SIZE 10240 //Buffer 10K

unsigned char buf[BUF_SIZE];
unsigned char *row[2];

int hindex;
int scale;
int method;
size_t nfeat; //Number of features for each day
int *x; //Features

//Time function
struct tm* (*tfunc)(const time_t* seconds);

int init();
int pFeat();
int clean();

int main(int argc, char *argv[]){
  int ret;

  if(argc < 4){
    printf("No enough arguments.\n");
    return SC_FAILURE;
  }

  if(((method = atoi(argv[1])) != 0 && method != 1) ||
     (hindex = atoi(argv[2])) <= 0 || (scale = atoi(argv[3])) <= 0 || 86400%scale != 0){
    printf("Invalid arguments.\n");
    return SC_FAILURE;
  }

  switch(method){
  case 0:
    tfunc = &gmtime;
    break;
  case 1:
    tfunc = &localtime;
    break;
  default:
    printf("Invalid method.\n");
    return SC_FAILURE;
    break;
  }

  if((ret = init()) != SC_SUCCESS){
    printf("Initialization failure.\n");
    return ret;
  }

  if((ret = pFeat()) != SC_SUCCESS){
    printf("Feature parsing failure.\n");
    return ret;
  }

  if((ret = clean()) != SC_SUCCESS){
    printf("Cleaning up failure.\n");
    return ret;
  }

  return 0;
}

int init(){
  nfeat = 86400/scale;
  x = (int*) malloc(sizeof(int)*nfeat);
  return SC_SUCCESS;
}

int pFeat(){
  struct tm *pt;
  time_t start;
  time_t stamp;
  time_t seconds;
  time_t diff;
  size_t day;
  size_t days;
  int hi, i, k, n;

  //First read
  if((n = csv_row_fread(stdin, buf, BUF_SIZE, row, 2, ',', CSV_TRIM | CSV_QUOTES)) > 0){
    hi = atoi(row[0]);
    stamp = atol(row[1]);
    seconds = stamp/1000L;
    start = seconds - seconds%86400L;
    day = 0;
    
    //Store in the corresponding value
    if(hi == hindex){
      diff = (seconds%86400L);
      x[diff/(long)scale] = 1;
    }
  } else {
    return SC_SUCCESS;
  }

  while((n = csv_row_fread(stdin, buf, BUF_SIZE, row, 2, ',', CSV_TRIM | CSV_QUOTES)) > 0){
    hi = atoi(row[0]);
    stamp = atol(row[1]);
    seconds = stamp/1000L;

    //Check whether it is a new day
    if((seconds - start)/86400L >= 1){
      days = (seconds - start)/86400L;
      start = seconds - seconds%86400L;
      day = day + days;

      //Repeat days of printing...
      for(k = 0; k < days; k = k + 1){
	printf("%d", x[0]);
	x[0] = 0;
	for(i = 1; i < nfeat; i = i + 1){
	  printf(",%d", x[i]);
	  x[i] = 0;
	}
	printf("\n");
      }
    }

    //Store in the corresponding value
    if(hi == hindex){
      diff = (seconds%86400L);
      x[diff/(long)scale] = 1;
    }
  }
  return SC_SUCCESS;
}

int clean(){
  free(x);
  return SC_SUCCESS;
}
