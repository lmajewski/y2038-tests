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
	"Test pthread_rwlock_"#OP"{rd|rw}lock timeout "#TIMEOUT" s"
#define TEST_DESCR(TIMEOUT, OP) TEST_DESCR_(TIMEOUT, OP)

enum tst_type {
  TST_COND_TIMED = 1,
  TST_COND_CLOCK,
};

struct tst_struct {
  struct timespec ts;
  enum tst_type ttype;
  int ret;
};

pthread_rwlock_t rwlock;

static void *pt_fn(void *param)
{
  struct tst_struct *tstr = param;

  if (tstr->ttype == TST_COND_TIMED)
    tstr->ret = pthread_rwlock_timedwrlock(&rwlock, &tstr->ts);
  else
    tstr->ret = pthread_rwlock_clockwrlock(&rwlock, CLOCK_REALTIME,
					   &tstr->ts);

  pthread_rwlock_destroy(&rwlock);
  pthread_exit(NULL);
}

static void __test_cond(int tsec_inc, enum tst_type ttype)
{
  struct tst_struct tstr;
  pthread_t t0;
  int ret;

  ret = pthread_rwlock_init(&rwlock, NULL);
  if (ret)
    test_failure(1, "failed to initialize rwlock %d!", ret);

  ret = pthread_rwlock_wrlock(&rwlock);
  if (ret)
    test_failure(1, "failed to get wrlock %d!", ret);

  ret = clock_gettime(CLOCK_REALTIME, &tstr.ts);
  if (ret)
    test_failure(1, "failed to create thread %d!", ret);

  tstr.ts.tv_sec += tsec_inc;
  tstr.ttype = ttype;
  tstr.ret = 0;

  ret = pthread_create(&t0, NULL, pt_fn, &tstr);
  if (ret)
    test_failure(1, "failed to create thread %d!", ret);

  sleep(3);
  pthread_join(t0, NULL);
  if (tstr.ret != ETIMEDOUT)
    test_failure(1, "failed to join thread %d!", ret);

  test_success();
}

void test_pthread_rwlock_timed(void)
{
  struct timespec ts;
  pthread_t t0;
  int ret;

  test_begin(TEST_DESCR(TIMEOUT_INC, timed));
  __test_cond(TIMEOUT_INC, TST_COND_TIMED);

  test_begin(TEST_DESCR(TIMEOUT_INC, clock));
  __test_cond(TIMEOUT_INC, TST_COND_CLOCK);
}
