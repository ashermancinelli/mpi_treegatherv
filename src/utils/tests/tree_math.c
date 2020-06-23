#include "utils.h"

int main(void)
{
  struct options opts;
  set_default_opts(&opts);
  assert(opts.persistent      == DEFAULT_PERSISTENT && "Default option not correctly set.");
  assert(opts.display_buf     == DEFAULT_DISPLAY_BUF && "Default option not correctly set.");
  assert(opts.num_loops       == DEFAULT_NUM_LOOPS && "Default option not correctly set.");
  assert(opts.data_per_node   == DEFAULT_DATA_PER_NODE && "Default option not correctly set.");
  assert(opts.data_per_proc   == DEFAULT_DATA_PER_PROC && "Default option not correctly set.");
  assert(opts.method          == DEFAULT_METHOD && "Default option not correctly set.");
  assert(opts.outfile         == DEFAULT_OUTFILE && "Default option not correctly set.");
  return 0;
}
