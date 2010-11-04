#ifndef DIRECTION_H
#define DIRECTION_H

#include <iostream>

enum Direction {UP, DOWN};


inline std::ostream& operator<< (std::ostream& o, const Direction& d)
{
  switch (d)
    {
    case UP:
      o<<"\"UP\"";
      break;
    case DOWN:
      o<<"\"DOWN\"";
      break;
    }
  return o;
}

#endif
