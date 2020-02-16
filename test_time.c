#include <time.h>
#include <errno.h>
#include <string.h>

#include "tests.h"

static void test_time(const time_t sec)
{
  time_t t0 = time(NULL);
  if (t0 == (time_t)-1)
  {
    test_failure(1, "time returned -1");
    return;
  }
  double d = difftime(t0, sec);
  if (d < -1.0 || d > 1.0)
    test_failure(0, "time error is %g second, more than one second", d);
  else
    test_success();
}

static int test_clock_settime(const time_t sec)
{
  struct timespec ts;
  int result;

  ts.tv_sec = sec;
  ts.tv_nsec = 0;

  result = clock_settime(CLOCK_REALTIME, &ts);
  if (result)
    test_failure(1, "clock_settime returned %d", result);
  else
    test_success();
  return result;
}

void test_time_(void)
{
  time_t t0, t;

  test_begin("Save current time");
  t0 = time(NULL);
  if (t0==(time_t)-1) test_failure(1, "time returned -1"); else test_success();

  test_begin("Set time to Y2038 minus 60 seconds");
  t = 0x7FFFFFFF;
  t -= 59;
  int result = test_clock_settime(t);
  if (result == 0) test_time(t);

  test_begin("Set time to Y2038 plus 60 seconds");
  t = 0x7FFFFFFF;
  t += 61;
  result = test_clock_settime(t);
  if (result == 0) test_time(t);

  test_begin("Restore time");
  result = test_clock_settime(t0);
  if (result)
    test_failure(1, "clock_settime returned %d", result);
  else
    test_success();
}
