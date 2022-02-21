#include <stdio.h>
#include <stdlib.h>

int main()
{
  unsigned char reg1 = 0;
  unsigned char reg2 = 0;
  unsigned char reg3 = 0;

  reg1 = 0b1001;
  reg3 = ~reg1;
  printf("reg1: %d", reg1);
  printf("reg3: %d", reg3);


  return 0;
}
