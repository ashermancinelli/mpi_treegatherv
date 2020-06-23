
#include "math.h"
#include "stdio.h"
#include "assert.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#define MS 1
#define SECS MS * 1000
#define MINS SECS * 60
#define HRS MINS * 60

/*
 * Small utility intended to find the difference between two
 * high-precision times.
 * For example, touching a file before and after running a binary
 * leaves me with two files, which I then ls like so:
 *
 * ~~~~~~~~~~{console}
 * $ ls --full-time start end
 * -rw-r--r-- 1 manc568 users 0 2020-06-22 20:34:48.512293728 -0700 start
 * -rw-r--r-- 1 manc568 users 0 2020-06-22 20:34:48.516546118 -0700 end
 * ~~~~~~~~~~
 *
 * I would like to parse the times and find the difference in miliseconds:
 *
 * ~~~~~~~~~~{console}
 * $ ls --full-time start end | cut -f7 -d' ' | bench.bin
 * 19429015
 * ~~~~~~~~~~
 *
 */

typedef union
{
  int vals[4];
  struct
  {
    int h, m, s, ms;
  };
} time_fmt;

int abs_time(time_fmt* t)
{
  return (
      (t->h  * HRS) +
      (t->m  * MINS) +
      (t->s  * SECS) +
      (t->ms * MS));
}

int wall_time(unsigned int n, time_fmt* t)
{
  if(t==NULL) return 1;
  t->h = n / HRS;
  t->m = n / MINS;
  t->s = n / SECS;
  t->ms = n / MS;
  return 0;
}

int main(int argc, char** argv)
{
  (void)argc; (void)argv;
  char buf[BUFSIZ];
  time_fmt t1, t2;
  fgets(buf, sizeof(buf), stdin);
  sscanf(buf, "%d:%d:%d.%d %d:%d:%d.%d",
      &t1.h, &t1.m, &t1.s, &t1.ms,
      &t2.h, &t2.m, &t2.s, &t2.ms);

  long int abs1 = abs_time(&t1);
  long int abs2 = abs_time(&t2);
  unsigned int diff = abs(abs1-abs2);
  assert(wall_time(diff,&t1)==0);
  printf("%d\n",diff);
  // printf("%d:%d:%d.%d\n", t1.h, t1.m, t1.s, t1.ms);
}
