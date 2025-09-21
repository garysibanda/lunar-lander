/***********************************************************************
 * Source File:
 *    ACCELERATION
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    Everything we need to know about acceleration
 ************************************************************************/

#include "acceleration.h"
#include "angle.h"
#include <math.h>

/****************************************
 * ACCELERATION : SET FROM ANGLE
 * Set acceleration from angle and magnitude
 ****************************************/
void Acceleration::set(const Angle& angle, double magnitude)
{
   ddx = magnitude * sin(angle.radians);
   ddy = magnitude * cos(angle.radians);
}

/****************************************
 * ACCELERATION : ADD
 * Add another acceleration to this one
 ****************************************/
void Acceleration::add(const Acceleration& acceleration)
{
   ddx += acceleration.ddx;
   ddy += acceleration.ddy;
}
