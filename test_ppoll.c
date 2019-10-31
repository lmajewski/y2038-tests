/* ppoll syscall handling test - it only tests the functionality related
   to struct timespec timeout_ts handling. */
/* The ppoll() is only provided when _GNU_SOURCE is defined */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <time.h>
#include <errno.h>
#include <poll.h>

#include "tests.h"
#define TIMEOUT 3
#define TEST_DESCR_(TIMEOUT) "Checking ppoll exit after "#TIMEOUT" s"
#define TEST_DESCR(TIMEOUT) TEST_DESCR_(TIMEOUT)

static void test_ppoll_timeout(struct timespec *tv, int timeout)
{
  struct timespec tv0 = { 0, 0 }, tv1 = { 0, 0 };
  struct pollfd fds = { -1, 0, 0 };   /* Ignore fds - just wait for timeout */
  int ret;

  /*
   * NOTE:
   * The masks for tv_sec ((1ULL << 32) - 1) checking are necessary as
   * clock_gettime is not yet converted to support 64 bit time ABI.
   *
   */

  ret = clock_gettime(CLOCK_REALTIME, &tv0);
  if (ret)
	  test_failure(1, "failed to read current time %d!", ret);

  ret = ppoll (&fds, 1, tv, 0);
  if (ret == 0) {
	  ret = clock_gettime(CLOCK_REALTIME, &tv1);
	  if (ret == 0) {
		  if ((((1ULL << 32) - 1) & tv0.tv_sec) + timeout ==
		      (((1ULL << 32) - 1) & tv1.tv_sec))
			  test_success();
		  else
			  test_failure(1, "failed to timeout immediately");
	  } else {
		  test_failure(1, "failed to read current time %d!", ret);
	  }
  } else {
	  test_failure(1, "failed to timeout immediately %d!", ret);
  }
}

void test_ppoll(void)
{
  struct timespec tv = { 0, 0 };

  /* Check if ppoll exits immediately */
  test_begin(TEST_DESCR(0)" (immediate exit)");
  errno = 0;

  tv.tv_sec = 0;
  tv.tv_nsec = 0;

  test_ppoll_timeout(&tv, 0);

  /* Check if ppoll exits immediately */
  test_begin(TEST_DESCR(TIMEOUT));
  errno = 0;

  tv.tv_sec = TIMEOUT;
  tv.tv_nsec = 0;

  test_ppoll_timeout(&tv, TIMEOUT);
}
