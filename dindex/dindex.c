/* Hostname index generator for skim.me
 * By Xiang Zhang @ New York University
 * Version 0.1, 03/28/2012
 *
 * Usage: dindex
 *        the program reads in stdin and write to stdout
 *        the output are two lists, one is hid,time_stamp (in seconds) pair
 *        another is hid, host pair
 *        the two lists are separated by an empty line
 *        the first list is sorted w.r.t. time_stamp
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mba/csv.h>
#include "utlist.h"
#include "uthash.h"

#define SC_SUCCESS 0
#define SC_FAILURE 1

#define BUF_SIZE 10240 //Buffer size: 10KB
#define HOST_SIZE 1024 //Host size: 1KB

typedef struct __STRUCT_record{
  int index;            //Host index
  time_t stamp;         //click time stamp

  struct __STRUCT_record *prev;
  struct __STRUCT_record *next;
} record;

typedef struct __STRUCT_host{
  int index;
  char hostname[HOST_SIZE];
  int visits;

  UT_hash_handle hh;
} host;

int ind; //current index
unsigned char buf[BUF_SIZE];
unsigned char *row[2];
record *rec;
host *hos;

int init(); //Do initialization
int parse(); //Do parsing
int print(); //Print the output
int cleanup(); //Do cleaning up

//Comparators
long rcmp(record *r1, record *r2);
int hcmp(host *h1, host *h2);

int main(int argc, char *argv[]){
  int ret;

  //Initialize
  if((ret = init()) != SC_SUCCESS){
    printf("Initialization error.\n");
    return ret;
  }

  //Parse the input
  if((ret = parse()) != SC_SUCCESS){
    printf("Parsing error.\n");
    return ret;
  }

  //Print the output
  if((ret = print()) != SC_SUCCESS){
    printf("Print error.\n");
    return ret;
  }

  //Clean up
  if((ret = cleanup()) != SC_SUCCESS){
    printf("Cleaning up error.\n");
    return ret;
  }

  return 0;
}

int init(){
  rec = NULL;
  hos = NULL;
  ind = 0;
  return SC_SUCCESS;
}

int parse(){
  record *r;
  host *h;
  int n;
  while((n = csv_row_fread(stdin, buf, BUF_SIZE, row, 2, ',', CSV_TRIM | CSV_QUOTES)) > 0){
    //Find the host in the hash table
    HASH_FIND_STR(hos, row[0], h);
    if(h == NULL){
      h = (host*) malloc(sizeof(host));
      strcpy(h->hostname, row[0]);
      h->index = ind;
      ind = ind + 1;
      h->visits = 0;
      HASH_ADD_STR(hos, hostname, h);
    }

    //New record
    h->visits = h->visits + 1;
    r = (record*) malloc(sizeof(record));
    r->stamp = atol(row[1]);
    r->index = h->index;
    DL_APPEND(rec, r);
  }

  //Sort the lists and hash tables
  DL_SORT(rec, rcmp);
  HASH_SORT(hos, hcmp);
  return SC_SUCCESS;
}

int print(){
  record *rel;
  host *hel;
  host *htmp;

  //Print each record
  DL_FOREACH(rec, rel){
    printf("%d,%ld\n", rel->index, rel->stamp);
  }
  //Print an empty line
  printf("\n");
  //Print each host
  HASH_ITER(hh, hos, hel, htmp){
    printf("%d,%s,%d\n", hel->index, hel->hostname, hel->visits);
  }

  return SC_SUCCESS;
}

int cleanup(){
  record *rel, *rtmp;
  host *hel, *htmp;

  //Delete each record
  DL_FOREACH_SAFE(rec, rel, rtmp){
    free(rel);
  }
  //Delete each host
  HASH_ITER(hh, hos, hel, htmp){
    free(hel);
  }
  rec = NULL;
  hos = NULL;
  return SC_SUCCESS;
}

long rcmp(record *r1, record *r2){
  return (long)(r1->stamp - r2->stamp);
}
int hcmp(host *h1, host *h2){
  return -(h1->visits - h2->visits);
}
