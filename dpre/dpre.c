/* Database preprocessor for Skim.me project
 * By Xiang Zhang @ New York University
 * Version 0.1, 03/27/2012
 *
 * Usage: ./dproc [nItems] [iUrl] [iTime]
 *        nItems: Number of items per row
 *        iUrl:   Index of url in the items (starting from 0)
 *        iTime:  Index of time in the items (starting from 0)
 * Note: This program read in standard input.
 */

//Silence the libwww bool error
#define BOOL bool

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <w3c-libwww/HTParse.h>
#include <mba/csv.h>

#define SC_SUCCESS 0
#define SC_FAILURE 1

#define BUF_SIZE 10240 //Buffer size: 10KB
#define HOST_SIZE 1024 //Host size: 1KB

int nItems; //Number of items in a row
int iUrl; //Index of the url
int iTime; //Index of the time stamp

unsigned char buf[BUF_SIZE];
unsigned char **row;

int init(); //Do initialization
int parse(); //Do parsing

int main(int argc, char *argv[]){
  int ret;

  //Check arguments
  if (argc < 4){
    printf("No enough arguments.\n");
    return SC_FAILURE;
  }
  nItems = atoi(argv[1]);
  iUrl = atoi(argv[2]);
  iTime = atoi(argv[3]);
  if(nItems <= 0 || iUrl < 0 || iTime < 0 ||
     iUrl >= nItems || iTime >= nItems){
    printf("Arguments error.\n");
  }

  //Initialize
  if((ret = init()) != SC_SUCCESS){
    printf("Initialization error.\n");
    return ret;
  }

  //Parse
  if((ret = parse()) != SC_SUCCESS){
    printf("Parsing error.\n");
    return ret;
  }

  return SC_SUCCESS;
}

int init(){
  row = (unsigned char **) malloc(sizeof(unsigned char*)*nItems);
  if(row == NULL){
    return false;
  }
  return SC_SUCCESS;
}

int parse(){
  char *host;
  char *access;
  int n;
  while((n = csv_row_fread(stdin, buf, BUF_SIZE, row, nItems, ',', CSV_TRIM | CSV_QUOTES)) > 0){
    access = HTParse(row[iUrl], "", PARSE_ACCESS);
    if(strcmp(access, "http") == 0 || strcmp(access, "https") == 0){
      //Parse the url
      host = HTParse(row[iUrl], "", PARSE_HOST);
      //Print the result
      printf("%s,%s\n", host, row[iTime]);
      //Free the hostname
      free(host);
    }
    free(access);
  }
  return SC_SUCCESS;
}
