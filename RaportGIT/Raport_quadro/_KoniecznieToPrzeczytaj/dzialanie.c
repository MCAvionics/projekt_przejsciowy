#include <stdio.h>

int x=5, y;

int main()
{
  x=((x++) * (x++));
  printf("%d\n",x);
  return(0);
}
