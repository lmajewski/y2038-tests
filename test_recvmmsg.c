#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "tests.h"

void test_recvmmsg(void)
{
  test_begin("Test recvmmsg");
  struct timespec ts = {2, 0};
  int socfd, ret;

  socfd = socket(AF_INET , SOCK_STREAM , 0);
  if (socfd == -1)
    {
      test_failure(1, "Cannot create socket");
    }

  ret = recvmmsg (socfd, NULL, 0, 0, &ts);
  if (ret == -1)
  {
    close (socfd);
    test_failure(1, "recvmmsg with 2s timeout returned -1");
  }

  /* Check passing NULL as struct timespec pointer */
  ret = recvmmsg (socfd, NULL, 0, MSG_DONTWAIT, NULL);
  if (ret == -1)
  {
    close (socfd);
    test_failure(1, "recvmmsg with NULL timeout returned -1");
  }

  close (socfd);	
  test_success();
}
