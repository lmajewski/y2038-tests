#define _GNU_SOURCE
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "tests.h"

#define TIMEOUT_INC 2
#define TEST_DESCR_(TIMEOUT, OP)	  \
	"Test pthread_"#OP"join_np timeout "#TIMEOUT" s"
#define TEST_DESCR(TIMEOUT, OP) TEST_DESCR_(TIMEOUT, OP)

static void *pt_fn(void *param)
{
  sleep(3);

  return NULL;
}

enum tst_type {
  TST_TIMED_JOIN = 1,
  TST_CLOCK_JOIN,
};

void __test_join_np(int tsec_inc, enum tst_type ttype)
{
  struct timespec ts;
  pthread_t t0;
  int ret;

  ret = pthread_create(&t0, NULL, pt_fn, NULL);
  if (ret)
    test_failure(1, "failed to create thread %d!", ret);

  ret = clock_gettime(CLOCK_REALTIME, &ts);
  if (ret)
    test_failure(1, "failed to create thread %d!", ret);

  ts.tv_sec += tsec_inc;

  if (ttype == TST_TIMED_JOIN)
    ret = pthread_timedjoin_np(t0, NULL, &ts);
  else if (ttype == TST_CLOCK_JOIN)
    ret = pthread_clockjoin_np(t0, NULL, CLOCK_REALTIME, &ts);
  else
    test_failure(0, "Wrong test!");

  if (ret != ETIMEDOUT)
    test_failure(1, "failed to join thread %d!", ret);

  test_success();
}

void test_pthread_timedjoin_np(void)
{
  struct timespec ts;
  pthread_t t0;
  int ret;

  test_begin(TEST_DESCR(TIMEOUT_INC, timed));
  __test_join_np(TIMEOUT_INC, TST_TIMED_JOIN);

  test_begin(TEST_DESCR(TIMEOUT_INC, clock));
  __test_join_np(TIMEOUT_INC, TST_CLOCK_JOIN);
}
