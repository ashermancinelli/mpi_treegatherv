
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
  if (buf[strlen(buf)-1]=='\n')
  {
    sscanf(buf, "%d:%d:%d.%d %d:%d:%d.%d",
        &t1.h, &t1.m, &t1.s, &t1.ms,
        &t2.h, &t2.m, &t2.s, &t2.ms);
  }
  else
  {
    assert(false && "Buffer was truncated");
  }

  // printf("T1:\n\thrs:%d\n\tmin:%d\n\tsec:%d\n\tms:%d\n", t1.vals[0], t1.vals[1], t1.vals[2], t1.vals[3]);
  // printf("T2:\n\thrs:%d\n\tmin:%d\n\tsec:%d\n\tms:%d\n", t2.vals[0], t2.vals[2], t2.vals[2], t2.vals[3]);

  long int abs1 = abs_time(&t1);
  long int abs2 = abs_time(&t2);
  unsigned int diff = abs(abs1-abs2);
  assert(wall_time(diff,&t1)==0);
  printf("%d\n",diff);
  // printf("%d:%d:%d.%d\n", t1.h, t1.m, t1.s, t1.ms);
}
