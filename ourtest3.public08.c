#include <stdio.h>
#include <assert.h>
#include "code.student.public08.h"


/* this represents one test of the functions in code.student.public08.c */

/* the function called by this program has a bug that is exposed by this
 * test, so this test will fail!
 */

int main(void) {
  assert(power(6, 0) == 1);

  printf("The student code in code.student.public08.c works on its ");
  printf("third test!\n");

  return 0;
}
