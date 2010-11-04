#ifndef MY_GRID_H
#define MY_GRID_H

/**
 * \ingroup mesh_grp 
 *
 *  following Setup Parametrs are requied:
 *  - GridParams    
 *  - GeometryParams
 *
 */
class My_Grid
{
public:

  My_Grid();

  double dX()            { return _dX;     };
  int    NumberOfCells() { return _NCells; };

private:

  double _dX;
  int    _NCells;
};

#endif
