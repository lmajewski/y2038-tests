#include <sys/time.h>
#include <sys/timex.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "tests.h"

void test_clock_adjtime(void)
{
  struct timespec now, then;
  struct timex delta;
  int64_t diff;
  int result;

  test_begin("Get current real time before clock_adjtime()");
  result = clock_gettime(CLOCK_REALTIME, &then);
  if (result)
	  test_failure(1, "clock_gettime returned %d", result);
  else
	  test_success();

  test_begin("Adjusting time forward by 1 ms");
  delta.time.tv_sec = 0;
  delta.time.tv_usec = 1000;
  delta.modes = ADJ_SETOFFSET;

  result = clock_adjtime(CLOCK_REALTIME, &delta);
  if (result == -1)
    test_failure(1, "clock_adjtime returned %d", result);
  else
    test_success();

  test_begin("Get current real time after clock_adjtime()");
  result = clock_gettime(CLOCK_REALTIME, &now);
  if (result)
	  test_failure(1, "clock_gettime returned %d", result);
  else
	  test_success();

  test_begin("Check that clock_adjtime() did not cause more than a few ms adjustment");
  diff = now.tv_sec-then.tv_sec;
  diff *= 1000000000;
  diff += now.tv_nsec-then.tv_nsec;
  if (diff > 10000000) // 10 ms
    test_failure(0, "clock_adjtime drifted by %lld, more than 10000000", diff);
  else
    test_success();
}
