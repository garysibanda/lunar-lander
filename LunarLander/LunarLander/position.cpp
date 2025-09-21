/***********************************************************************
 * Source File:
 *    POSITION
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    Everything we need to know about a position on the screen
 ************************************************************************/

#include "position.h"
#include "velocity.h"
#include "acceleration.h"
#include <cmath>
#include <algorithm>

/****************************************
 * POSITION : ASSIGNMENT OPERATOR
 ****************************************/
Position& Position::operator=(const Position& rhs)
{
   x = rhs.x;
   y = rhs.y;
   return *this;
}

/****************************************
 * POSITION : ADD
 * Update position using physics equation:
 * s = s_0 + vt + 1/2 at^2
 ****************************************/
void Position::add(const Acceleration& acceleration, const Velocity& velocity, double time)
{
   // s = s_0 + vt + 1/2 at^2
   x = x + (velocity.getDX() * time) + (0.5 * acceleration.getDDX() * time * time);
   y = y + (velocity.getDY() * time) + (0.5 * acceleration.getDDY() * time * time);
}

/****************************************
 * POSITION : EQUALITY OPERATOR
 ****************************************/
bool Position::operator==(const Position& rhs) const
{
   const double TOLERANCE = 0.001;
   return (std::abs(x - rhs.x) < TOLERANCE) && (std::abs(y - rhs.y) < TOLERANCE);
}
