#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "tests.h"

static void test_futimes_call(const char *name, const struct timeval t[2])
{
  int result = 0, fd = open(name, O_CREAT);
  if (fd == -1)
  {
    test_failure(1, "open('%s', O_CREAT) returned %d", name, fd);
    return;
  }
  int c = close(fd);
  if (c)
  {
    test_failure(1, "close returned %d", c);
    return;
  }
  fd = open(name, O_RDWR);
  if (fd == -1)
  {
    test_failure(1, "open('%s', O_RDWR) returned %d", name, fd);
    return;
  }
  if (t==NULL)
    test_success();
  else
  {
    result = futimes(fd, t);
    if (result)
      test_failure(1, "futimes('%s'...) returned %d", name, result);
    else
      test_success();
  }
  close(fd);
}

void test_futimes(void)
{
  test_begin("Call futimes(NULL) on /futimes.null");
  test_futimes_call("/futimes.null", NULL);

  test_begin("Call futimes(Y2038-2, Y2038-1) on /futimes.n2038");
  const struct timeval t1[2] = { { 0x7FFFFFFE, 0 },  { 0x7FFFFFFF, 0 } };
  test_futimes_call("/futimes.n2038", t1);

  test_begin("Check stat of /futimes.n2038");
  struct stat st1;
  int result = stat("/futimes.n2038", &st1);
  if (result) test_failure(1, "stat('/futimes.n2038') returned %d", result); else test_success();

  test_begin("Call futimes(Y2038+1, Y2038+2) on /futimes.y2038");
  const struct timeval t2[2] = { { 0x80000001ull, 0 },  { 0x80000002ull, 0 } };
  test_futimes_call("/futimes.y2038", t2);

  test_begin("Check stat of /futimes.y2038");
  struct stat st2;
  result = stat("/futimes.y2038", &st2);
  if (result) test_failure(1, "stat('/futimes.y2038') returned %d", result); else test_success();
}
