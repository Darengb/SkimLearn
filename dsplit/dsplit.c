/* Data splitter for skim.me project
 * By Xiang Zhang @ New York University
 * Version 0.1, 03/28/2012
 *
 * Usage: ./dsplit [section]
 *       [section] is the index of section you want. Starting from 0.
 *       Read from stdin, write to stdout.
 */

#include <stdio.h>

#define SC_SUCCESS 0
#define SC_FAILURE 1

#define BUF_SIZE 10240 //Buffer size 10K

int section; //The section

char buf[BUF_SIZE];

int skip();
int print();

int main(int argc, char *argv[]){
  int ret;

  //Check the arguments
  if(argc < 2){
    printf("No enough arguments\n");
    return SC_FAILURE;
  }
  if((section = atoi(argv[1])) < 0){
    printf("Argument error.\n");
    return SC_FAILURE;
  }

  if((ret = skip()) != SC_SUCCESS){
    printf("Skipping error.\n");
    return ret;
  }

  if((ret = print()) != SC_SUCCESS){
    printf("Printing error.\n");
    return ret;
  }

  return 0;
}

int skip(){
  int nskip;
  nskip = 0;
  while(nskip < section && gets(buf) != NULL){
    if(buf[0] == '\0'){
      nskip = nskip + 1;
    }
  }
  return SC_SUCCESS;
}


int print(){
  while(gets(buf) != NULL && buf[0] != '\0'){
    printf("%s\n", buf);
  }
  return SC_SUCCESS;
}
