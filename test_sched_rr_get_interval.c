#include <sched.h>
#include <errno.h>
#include <string.h>

#include "tests.h"

void test_sched_rr_get_interval(void)
{
  test_begin("Call sched_rr_get_interval");
  struct timespec ts[2] = { { -1, -1 }, { -1, -1 } };
  int result = sched_rr_get_interval(0, ts);
  if (result)
    test_failure(1, "sched_rr_get_interval returned %d", result);
  else if (ts[1].tv_sec != -1 || ts[1].tv_nsec != -1)
    test_failure(1, "sched_rr_get_interval ts[1] = (%lld,%lld)", (long long) (ts[1].tv_sec), (long long) (ts[1].tv_nsec));
  else
    test_success();
}
