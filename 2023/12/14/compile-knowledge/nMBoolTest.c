#include "head.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int main() {
  if (mTrue()) {
    printf("this is mTrue\n");
  }
  if (!mFalse()) {
    printf("this is mFalse\n");
  }
  return 0;
}
