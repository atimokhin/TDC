#include "do_print_usage_info.h"
#include "argtable2.h"


bool do_print_usage_info(int argc, char *argv[])
{
  // help **********
  struct arg_lit *help_arg = arg_lit0("h", "help", "print help");
  // construct argtable
  struct arg_end *end_arg = arg_end(20);
  void *argtable[] = {help_arg,end_arg};
  // parse command line
  int nerrors = arg_parse(argc,argv,argtable);

  if ( help_arg->count == 0 ) return false;

  // clear memory
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

  return true;
}
