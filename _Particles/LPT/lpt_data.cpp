#include "lpt_data.h"

#include "../../SetupParameters/geometry_params.h"
#include "../../SetupParameters/grid_params.h"

LPT_Data::LPT_Data()
{
  n_steps_since_last_splitting=0;
  f_splitted_last_time=1;
};

void LPT_Data::SetupFromConfigGroup(FileInput& in)
{
  // merging params
  if ( do_merging_flag = in.get_answer("DoMerging") )
  {
    n_max = static_cast<int>( in.get_param("NumberOfParticles_Max") );
    merge_f_reduce = in.get_param("merge_f_reduce");
  }
  // splitting params
  if ( do_splitting_flag = in.get_answer("DoSplitting") )
  {
    GeometryParams geom;
    GridParams     grid;
    double dx = geom.L()/grid.NumberOfCells();

    n_min = static_cast<int>( in.get_param("NumberOfParticles_Min") );
    
    split_min_weight     = in.get_param("split_min_weight");
    split_dx             = dx*in.get_param("split_f_dx");
    split_f_splitted_min = in.get_param("split_f_splitted_min");
    split_n_steps_min    = static_cast<int>( in.get_param("split_n_steps_min") );
  }
}
