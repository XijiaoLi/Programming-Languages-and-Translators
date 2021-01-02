// C implementation of Bubble sort
#include <stdio.h>

void swap()
{
  int i;
  int j = 0;
  for (i=0; i < 6; i++){
    if (j%5==0) {
      return;
    }
    if (i%3==0) {
      j+=5;
    }
  }
}

// the main function
int main()
{
  swap();
  return 0;
}
