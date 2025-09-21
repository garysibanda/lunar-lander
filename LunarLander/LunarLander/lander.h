/***********************************************************************
 * Header File:
 *    LANDER
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    All the information about the lunar lander
 ************************************************************************/

#pragma once

#include "position.h"
#include "velocity.h"
#include "acceleration.h"
#include "angle.h"
#include "thrust.h"

// Enhanced status enumeration
enum Status { PLAYING, SAFE, DEAD };

// Forward declaration for unit tests
class TestLander;

/****************************************************************
 * LANDER
 * The position and status of the lunar lander with
 * enhanced physics and fuel management
 ***************************************************************/
class Lander
{
   friend TestLander;
   
public:
   // Constructor - need to know something about the board
   Lander(const Position& posUpperRight) :
      status(DEAD),
      fuel(5000.0),
      totalMass(15103.0),
      dryMass(10183.0)
   {
      reset(posUpperRight);
   }
   
   // Reset the lander and its position
   void reset(const Position& posUpperRight);
   
   // Status queries
   bool isDead() const { return (status == DEAD); }
   bool isLanded() const { return (status == SAFE); }
   bool isFlying() const { return (status == PLAYING); }
   
   // Getters
   Position getPosition() const { return pos; }
   Velocity getVelocity() const { return velocity; }
   Angle getAngle() const { return angle; }
   double getSpeed() const { return velocity.getSpeed(); }
   int getFuel() const { return static_cast<int>(fuel); }
   int getWidth() const { return 20; }
   double getMaxSpeed() const { return 4.0; }
   
   // Enhanced getters for realistic lunar lander
   double getTotalMass() const { return dryMass + fuel; }
   double getFuelPercentage() const { return (fuel / 5000.0) * 100.0; }
   bool isOutOfFuel() const { return fuel <= 0.0; }
   
   // Setters for game state
   void land();    // Successful landing
   void crash();   // Crashed landing
   
   // Physics simulation
   void coast(const Acceleration& acceleration, double time);
   Acceleration input(const Thrust& thrust, double gravity);
   
   // Enhanced physics functions
   void applyGravity(double gravity, double time);
   void applyThrust(const Thrust& thrust, double time);
   void updateFuel(double fuelConsumption);
   
   // Collision detection
   bool checkGroundCollision(double groundY) const;
   bool checkSafetyLanding() const;
   
   // Display information
   void draw() const;
   void drawFlames(const Thrust& thrust) const;

private:
   Position pos;        // position of the lander
   Velocity velocity;   // velocity of the lander
   Angle angle;         // orientation of the lander
   Status status;       // current game state
   double fuel;         // remaining fuel in kg
   double totalMass;    // total mass including fuel
   double dryMass;      // mass without fuel
   
   // Physics constants
   static const double FUEL_CONSUMPTION_MAIN;      // kg/s for main engine
   static const double FUEL_CONSUMPTION_ATTITUDE;  // kg/s for attitude thrusters
   static const double THRUST_MAIN;                // Newtons for main engine
   static const double THRUST_ATTITUDE;            // radians/s for rotation
   
   // Helper functions
   void consumeFuel(double amount);
   bool hasFuelFor(double amount) const;
   void normalizeAngle();
};
