#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

#include "tests.h"

#define TIMEOUT_INC 2
#define TEST_DESCR_(TIMEOUT, OP)	  \
	"Test sem_"#OP"wait timeout "#TIMEOUT" s"
#define TEST_DESCR(TIMEOUT, OP) TEST_DESCR_(TIMEOUT, OP)

enum tst_type {
  TST_COND_TIMED = 1,
  TST_COND_CLOCK,
};

static void __test_cond(int tsec_inc, enum tst_type ttype)
{
	struct timespec ts;
  sem_t sem;
  int ret;

  ret = clock_gettime(CLOCK_REALTIME, &ts);
  if (ret)
    test_failure(1, "failed to get current time %d!", ret);

  ts.tv_sec += tsec_inc;

  ret = sem_init(&sem, 0, 0);
  if (ret == -1)
    test_failure(1, "failed to init semaphore %d!", ret);

	if (ttype == TST_COND_TIMED)
		while ((ret = sem_timedwait(&sem, &ts)) == -1 && errno == EINTR)
			continue;
	else
		while ((ret = sem_clockwait(&sem, CLOCK_REALTIME, &ts)) == -1 &&
		       errno == EINTR)
			continue;

  if (ret == -1 && errno != ETIMEDOUT)
    test_failure(1, "failed to join thread %d!", ret);

  test_success();
}

void test_sem_timedwait(void)
{
  test_begin(TEST_DESCR(TIMEOUT_INC, timed));
  __test_cond(TIMEOUT_INC, TST_COND_TIMED);

  test_begin(TEST_DESCR(TIMEOUT_INC, clock));
  __test_cond(TIMEOUT_INC, TST_COND_CLOCK);
}
