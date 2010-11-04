#include "my_grid.h"

#include "../SetupParameters/grid_params.h"
#include "../SetupParameters/geometry_params.h"

My_Grid::My_Grid()
{
  GridParams     grid;
  GeometryParams geom;

  // number of cells
  _NCells = grid.NumberOfCells();

  // grid spacing
  _dX = geom.L()/_NCells;
}
