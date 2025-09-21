/***********************************************************************
 * Header File:
 *    VELOCITY
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    Everything we need to know about speed and velocity
 ************************************************************************/

#pragma once

// Forward declarations
class TestVelocity;
class TestLander;
class TestPosition;
class Acceleration;
class Angle;

/*********************************************
 * Velocity
 * Represents velocity in 2D space
 *********************************************/
class Velocity
{
   friend TestVelocity;
   friend TestLander;
   friend TestPosition;
   
public:
   double dx; // horizontal velocity
   double dy; // vertical velocity

   // Constructors
   Velocity() : dx(0.0), dy(0.0) {}
   Velocity(double dx, double dy) : dx(dx), dy(dy) {}

   // Getters
   double getDX() const { return dx; }
   double getDY() const { return dy; }
   double getSpeed() const;

   // Setters
   void setDX(double dx) { this->dx = dx; }
   void setDY(double dy) { this->dy = dy; }
   void set(const Angle& angle, double magnitude);
   void addDX(double dx) { this->dx += dx; }
   void addDY(double dy) { this->dy += dy; }
   void add(const Acceleration& acceleration, double time);
   
   // Safety check for landing
   bool isSafeLandingSpeedTest() const;
};
