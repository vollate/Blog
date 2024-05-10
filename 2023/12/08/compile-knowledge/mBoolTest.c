#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
bool mTrue();
bool mFalse();

int main() {
  if (mTrue()) {
    printf("this is mTrue\n");
  }
  if (!mFalse()) {
    printf("this is mFalse\n");
  }
  return 0;
}
