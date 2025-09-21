/***********************************************************************
 * Header File:
 *    ANGLE
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    Everything we need to know about angles for the lunar lander
 ************************************************************************/

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

// Forward declarations
class TestAngle;
class TestAcceleration;
class TestVelocity;

/*********************************************
 * ANGLE
 * Represents an angle
 *********************************************/
class Angle
{
   friend TestAngle;
   friend class TestVelocity;
   friend class TestAcceleration;
   
public:
   double radians;  // PUBLIC member for direct access

   // Default constructor
   Angle() : radians(0.0) {}
   
   // Copy constructor
   Angle(const Angle& rhs) : radians(rhs.radians) {}
   
   // Assignment operator
   Angle& operator=(const Angle& rhs) { radians = rhs.radians; return *this; }

   // Getters
   double getRadians() const { return radians; }
   double getDegrees() const { return radians * 180.0 / M_PI; }

   // Setters
   void setRadians(double radians) { this->radians = radians; }
   void setDegrees(double degrees) { this->radians = degrees * M_PI / 180.0; }

   // Direction setters
   void setUp()    { radians = 0.0; }
   void setDown()  { radians = M_PI; }
   void setLeft()  { radians = M_PI + M_PI_2; }
   void setRight() { radians = M_PI_2; }
   
   // Reverse direction
   void reverse() { radians += M_PI; }

   // Add delta
   void add(double delta) { radians += delta; }
};
