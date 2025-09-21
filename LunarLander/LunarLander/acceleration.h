/***********************************************************************
 * Header File:
 *    ACCELERATION
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    Everything we need to know about acceleration
 ************************************************************************/

#pragma once

// Forward declarations
class TestAcceleration;
class Angle;

/*********************************************
 * ACCELERATION
 * Represents acceleration in 2D space
 *********************************************/
class Acceleration
{
   friend TestAcceleration;

public:
   double ddx; // horizontal acceleration
   double ddy; // vertical acceleration

   // Constructors
   Acceleration() : ddx(0.0), ddy(0.0) {}
   Acceleration(double ddx, double ddy) : ddx(ddx), ddy(ddy) {}

   // Getters
   double getDDX() const { return ddx; }
   double getDDY() const { return ddy; }

   // Setters
   void setDDX(double ddx) { this->ddx = ddx; }
   void setDDY(double ddy) { this->ddy = ddy; }
   
   // Set from angle and magnitude (required by tests)
   void set(const Angle& angle, double magnitude);

   // Add components
   void addDDX(double ddx) { this->ddx += ddx; }
   void addDDY(double ddy) { this->ddy += ddy; }
   void add(const Acceleration& acceleration);
};
