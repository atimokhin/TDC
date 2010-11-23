#ifndef LPT_DATA
#define LPT_DATA

#include "ATbase.h"

/**
 * 
 * \class LPT_Data
 * Class which stores parameters of LPT manipulations
 * in each individual Particles class
 *
 * config file example
 *
 \verbatim

      Group "LPT" {
         ! Required parameters
         DoSplitting ? Yes;
         DoMerging   ? Yes;
         ! Optional parameters - required only if splitting/merging is requested
         NumberOfParticles_Max = 350000;
         NumberOfParticles_Min = 10000;
         merge_f_reduce       = 0.5;   ! probability of particle merge
         split_min_weight     = 1;     ! min weight  of splitted particle
         split_f_dx           = 0.1;   ! fraction of cellsize 
         split_f_splitted_min = 0.5;   ! minimum fraction of splitted particles
         split_n_steps_min    = 100;   ! minimum # of time steps between splitting
      }

 \endverbatim
 * 
 */

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
