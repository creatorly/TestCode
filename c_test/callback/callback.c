#include <string.h>
#include <stdio.h>

typedef int (*CALLBACK) (unsigned char *inputdata, unsigned char *rspdata);
int callback_init(CALLBACK callback);
int data_callback(unsigned char *inputdata, unsigned char *rspdata);
static CALLBACK pTest;

void main(void)
{
  unsigned char inputdata[32];

  printf("begin\n");

  callback_init(data_callback);
  
  while(1)
  {
    printf("please enter data...\n");
	fgets(inputdata, 31, stdin);
	printf("enter:%s\n", inputdata);
	read_input_data(inputdata);
  }
}


int read_input_data(unsigned char *inputdata)
{
  unsigned char rspdata[32];

  pTest(inputdata, &rspdata);

  printf("read_input_data:%s\n", rspdata);
}

int callback_init(CALLBACK callback)
{
  pTest = callback;
  printf("callback_init\n");
}

int data_callback(unsigned char *inputdata, unsigned char *rspdata)
{
  printf("data_callback:%s\n", inputdata);

  strcpy(rspdata,"test ok");

  return 0;
}
