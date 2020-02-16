#include <mqueue.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "tests.h"

void test_mq_timedsend_onqueue(mqd_t q)
{
  char msg[8] = "ABCD1234";

  test_begin("mq_send() to fill the queue up");
  int result = mq_send(q, msg, sizeof(msg), 1);
  if (result) test_failure(1, "mq_send returned %d", q); else test_success();

  test_begin("mq_timedsend() with a time-out in the past before Y2038");
  struct timespec ts;
  result = timespec_get(&ts, TIME_UTC);
  if (result != TIME_UTC)
    test_failure(0, "timespec_get returned %d instead of %d (TIME_UTC)", result, TIME_UTC);
  else
  {
    result = mq_timedsend(q, msg, sizeof(msg), 1, &ts);
    if (result != -1)
      test_failure(1, "mq_timedsend returned %d", result);
    else if (errno != ETIMEDOUT)
      test_failure(1, "mq_timedsend did not time out");
    else
      test_success();
  }

  test_begin("mq_timedreceive() with a time-out in the future before Y2038");
  result = timespec_get(&ts, TIME_UTC);
  if (result != TIME_UTC)
    test_failure(1, "timespec_get returned %d instead of %d (TIME_UTC)", result, TIME_UTC);
  else
  {
    ts.tv_sec++;
    result = mq_timedsend(q, msg, sizeof(msg), 1, &ts);
    if (result != -1)
      test_failure(1, "mq_timedsend returned %d", result);
    else if (errno != ETIMEDOUT)
      test_failure(1, "mq_timedsend did not time out");
    else
      test_success();
  }

// Only test this part if we can set the time beyond Y2038
// i.e. if our time_t is 64-bit

#if defined(__USE_TIME_BITS64) && __USE_TIME_BITS64 == 1

  test_begin("Get current time");
  time_t t0 = time(NULL);
  if (t0==(time_t)-1)
  {
    test_failure(1, "time returned -1");
    return;
  }
  test_success();

  test_begin("Set current time at Y2038+1s");
  struct timespec t = { 0x80000001, 0};
  result = clock_settime(CLOCK_REALTIME, &t);
  if (result)
  {
    test_failure(1, "clock_settime returned %d", result);
    return;
  }
  test_success();

  test_begin("mq_timedsend() with a time-out in the past after Y2038");
  result = timespec_get(&ts, TIME_UTC);
  if (result != TIME_UTC)
    test_failure(1, "timespec_get returned %d instead of %d (TIME_UTC)", result, TIME_UTC);
  else
  {
    result = mq_timedsend(q, msg, sizeof(msg), 1, &ts);
    if (result != -1)
      test_failure(1, "mq_timedsend returned %d", result);
    else if (errno != ETIMEDOUT)
      test_failure(1, "mq_timedsend did not time out");
    else
      test_success();
  }

  test_begin("mq_timedreceive() with a time-out in the future after Y2038");
  result = timespec_get(&ts, TIME_UTC);
  if (result != TIME_UTC)
    test_failure(1, "timespec_get returned %d instead of %d (TIME_UTC)", result, TIME_UTC);
  else
  {
    ts.tv_sec++;
    result = mq_timedsend(q, msg, sizeof(msg), 1, &ts);
    if (result != -1)
      test_failure(1, "mq_timedsend returned %d", result);
    else if (errno != ETIMEDOUT)
      test_failure(1, "mq_timedsend did not time out");
    else
      test_success();
  }

  test_begin("Restore current time");
  struct timespec ts0;
  ts0.tv_sec = t0;
  ts0.tv_nsec = 0;

  result = clock_settime(CLOCK_REALTIME, &ts0);
  if (result)
    test_failure(1, "clock_settime returned %d", result);
  else 
    test_success();

#endif
}

void test_mq_timedsend(void)
{
  test_begin("Create the message queue");
  struct mq_attr mq_attr = { .mq_maxmsg = 1, .mq_msgsize = 8 };
  mqd_t q = mq_open("/y2038s", O_RDWR | O_CREAT, 0x777, &mq_attr);
  if (q == (mqd_t) -1)
  {
    test_failure(1, "mq_open returned -1");
    return;
  }
  test_success();
  test_mq_timedsend_onqueue(q);
  test_begin("Close the message queue");
  int cq = mq_close(q);
  if (cq)
    test_failure(1, "mq_close returned %d", cq);
  else
    test_success();

  test_begin("Remove the message queue");
  int uq = mq_unlink("/y2038s");
  if (uq)
    test_failure(1, "mq_unlink returned %d", uq);
  else
    test_success();
}
