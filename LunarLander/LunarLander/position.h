/***********************************************************************
 * Header File:
 *    POSITION
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    Everything we need to know about a position on the screen
 ************************************************************************/

#pragma once

// Forward declarations
class TestPosition;
class Acceleration;
class Velocity;

/*********************************************
 * POSITION
 * A single position on the screen
 *********************************************/
class Position
{
   friend TestPosition;

public:
   double x; // horizontal position
   double y; // vertical position

   // Constructors
   Position() : x(0.0), y(0.0) {}
   Position(double x, double y) : x(x), y(y) {}
   Position(const Position& rhs) : x(rhs.x), y(rhs.y) {}

   // Assignment operator
   Position& operator=(const Position& rhs);

   // Getters
   double getX() const { return x; }
   double getY() const { return y; }

   // Setters
   void setX(double x) { this->x = x; }
   void setY(double y) { this->y = y; }
   void addX(double dx) { this->x += dx; }
   void addY(double dy) { this->y += dy; }

   // Physics calculation: s = s_0 + vt + 1/2 at^2
   void add(const Acceleration& acceleration, const Velocity& velocity, double time);

   // Comparison operators
   bool operator==(const Position& rhs) const;
   bool operator!=(const Position& rhs) const { return !(*this == rhs); }
};
