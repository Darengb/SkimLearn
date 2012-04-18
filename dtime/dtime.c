/* Time parser for skim.me
 * By Xiang Zhang @ New York University
 * Version 0.1, 03/28/2012
 *
 * Usage: ./dtime [method]
 *       [method]: 0: GMT
 *                 1: Local time
 *       The program accepts input from stdin, and write to stdout
 *       each row output is: hindex,stamp,sec,min,hour,mday,mon,year,wday,yday,dst
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

int method;

int parseGMT();
int parseLT();

int main(int argc, char *argv[]){
  int ret;

  if(argc < 2){
    printf("No enough arguments.\n");
    return SC_FAILURE;
  }

  if((method = atoi(argv[1])) != 0 && method != 1){
    printf("Invalid arguments.\n");
    return SC_FAILURE;
  }

  switch(method){
  case 0:
    ret = parseGMT();
    break;
  case 1:
    ret = parseLT();
    break;
  default:
    printf("Invalid method.\n");
    return SC_FAILURE;
    break;
  }

  if(ret != SC_SUCCESS){
    printf("Parsing failure.\n");
    return ret;
  }
  return 0;
}

int parseGMT(){
  struct tm *pt;
  time_t stamp;
  time_t seconds;
  int hindex;
  int n;
  while((n = csv_row_fread(stdin, buf, BUF_SIZE, row, 2, ',', CSV_TRIM | CSV_QUOTES)) > 0){
    hindex = atoi(row[0]);
    stamp = atol(row[1]);
    seconds = stamp/1000L;
    pt = gmtime(&seconds);
    printf("%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",hindex,stamp,
	   pt->tm_sec,pt->tm_min,pt->tm_hour,pt->tm_mday,pt->tm_mon,1900 + pt->tm_year,pt->tm_wday,pt->tm_yday,pt->tm_isdst);
  }
  return SC_SUCCESS;
}

int parseLT(){
  struct tm *pt;
  time_t stamp;
  time_t seconds;
  int hindex;
  int n;
  while((n = csv_row_fread(stdin, buf, BUF_SIZE, row, 2, ',', CSV_TRIM | CSV_QUOTES)) > 0){
    hindex = atoi(row[0]);
    stamp = atol(row[1]);
    seconds = stamp/1000L;
    pt = localtime(&seconds);
    printf("%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",hindex,stamp,
	   pt->tm_sec,pt->tm_min,pt->tm_hour,pt->tm_mday,pt->tm_mon,1900 + pt->tm_year,pt->tm_wday,pt->tm_yday,pt->tm_isdst);
  }
  return SC_SUCCESS;
}
