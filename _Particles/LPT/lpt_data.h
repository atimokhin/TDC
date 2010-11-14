#ifndef LPT_DATA
#define LPT_DATA

#include "ATbase.h"

class LPT_Data
{
public:
  LPT_Data();
  void SetupFromConfigGroup(FileInput& in);
  
public:
  bool do_splitting_flag;
  bool do_merging_flag;

  int n_max;
  int n_min;

  double merge_f_reduce;    

  double split_min_weight;  
  double split_dx;
  double split_f_splitted_min;
  double split_n_steps_min;

  int    n_steps_since_last_splitting;
  double f_splitted_last_time;
};

#endif
