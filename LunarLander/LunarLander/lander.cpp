/***********************************************************************
 * Source File:
 *    LANDER - LAB SPECIFICATION COMPLIANT
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    All the information about the lunar lander following exact lab specs
 ************************************************************************/

#include "lander.h"
#include "uiDraw.h"
#include <cstdlib>  // for rand()
#include <cmath>    // for sin, cos
#include <algorithm> // for std::max, std::min

// Initialize physics constants - EXACT LAB SPECIFICATIONS
const double Lander::FUEL_CONSUMPTION_MAIN = 22.046;        // 10 lbs/s converted to kg/s
const double Lander::FUEL_CONSUMPTION_ATTITUDE = 2.2046;    // 1 lb/s converted to kg/s
const double Lander::THRUST_MAIN = 45000.0;                 // Newtons (exact lab spec)
const double Lander::THRUST_ATTITUDE = 0.1;                 // radians/frame (exact lab spec)

/***********************************************************
 * LANDER : RESET
 * Reset the lander to starting position and state
 ***********************************************************/
void Lander::reset(const Position& posUpperRight)
{
   // Reset angle to pointing up
   angle.setUp();
   
   // Position at top right with some randomness
   pos.setX(posUpperRight.getX() - 1.0);
   pos.setY(posUpperRight.getY() * 0.75 +
           (rand() % 20 - 10)); // Random Y between 75-95% of screen height
   
   // Random initial velocity (slight leftward drift)
   velocity.setDX(-4.0 - (rand() % 7)); // -4 to -10
   velocity.setDY(-2.0 + (rand() % 5));  // -2 to +2
   
   // Reset game state
   status = PLAYING;
   fuel = 2268.0; // 5000 lbs converted to kg (5000 / 2.20462)
}

/***********************************************************
 * LANDER : LAND
 * Successfully land the lander
 ***********************************************************/
void Lander::land()
{
   angle.setUp();
   status = SAFE;
}

/***********************************************************
 * LANDER : CRASH
 * Crash the lander
 ***********************************************************/
void Lander::crash()
{
   angle.setDown();
   status = DEAD;
}

/***********************************************************
 * LANDER : COAST
 * Coast for a given amount of time
 ***********************************************************/
void Lander::coast(const Acceleration& acceleration, double time)
{
   // Apply physics: update position and velocity
   pos.add(acceleration, velocity, time);
   velocity.add(acceleration, time);
}

/***********************************************************
 * LANDER : INPUT
 * Process input and return resulting acceleration
 * EXACT LAB SPECIFICATION IMPLEMENTATION
 ***********************************************************/
Acceleration Lander::input(const Thrust& thrust, double gravity)
{
   Acceleration acceleration;
   
   // Always apply gravity (lab spec: 1.625 m/s²)
   acceleration.setDDY(gravity);
   
   // Only process thrust if we have fuel and are flying
   if (status == PLAYING && fuel > 0.0)
   {
      // Main engine thrust - LAB SPECIFICATION
      if (thrust.isMain())
      {
         // Lab spec: 45,000 N / 15,103 kg = 2.98 m/s²
         double thrustAcceleration = thrust.mainEngineThrust();
         
         // FIXED THRUST PHYSICS: Correct vertical, fix horizontal direction
         // Vertical (Y) thrust works correctly: up when pointing up
         // Horizontal (X) thrust was reversed: need to negate X component
         double thrustX = -sin(angle.getRadians()) * thrustAcceleration;  // Negated for correct horizontal
         double thrustY = cos(angle.getRadians()) * thrustAcceleration;   // Correct for vertical
         
         acceleration.addDDX(thrustX);
         acceleration.addDDY(thrustY);
         
         // Lab spec: 10 lbs/frame fuel consumption
         consumeFuel(FUEL_CONSUMPTION_MAIN * 0.1); // 0.1 second per frame
      }
      
      // Attitude control - CORRECTED ROTATION DIRECTIONS
      if (thrust.isClock())
      {
         // RIGHT arrow = clockwise rotation (when viewed from above)
         // In screen coordinates, this should be NEGATIVE rotation
         angle.add(-THRUST_ATTITUDE);
         consumeFuel(FUEL_CONSUMPTION_ATTITUDE * 0.1);
      }
      
      if (thrust.isCounter())
      {
         // LEFT arrow = counter-clockwise rotation (when viewed from above)
         // In screen coordinates, this should be POSITIVE rotation
         angle.add(THRUST_ATTITUDE);
         consumeFuel(FUEL_CONSUMPTION_ATTITUDE * 0.1);
      }
   }
   
   return acceleration;
}

/***********************************************************
 * LANDER : APPLY GRAVITY
 * Apply gravitational acceleration over time
 ***********************************************************/
void Lander::applyGravity(double gravity, double time)
{
   velocity.addDY(gravity * time);
}

/***********************************************************
 * LANDER : APPLY THRUST - DEPRECATED
 * This method is not used in the current physics system
 ***********************************************************/
void Lander::applyThrust(const Thrust& thrust, double time)
{
   // This method is deprecated - thrust is handled in input() method
   // Keeping for compatibility but not using
}

/***********************************************************
 * LANDER : UPDATE FUEL
 * Reduce fuel by specified amount
 ***********************************************************/
void Lander::updateFuel(double fuelConsumption)
{
   fuel = std::max(0.0, fuel - fuelConsumption);
}

/***********************************************************
 * LANDER : CHECK GROUND COLLISION
 * Check if lander has hit the ground
 ***********************************************************/
bool Lander::checkGroundCollision(double groundY) const
{
   return pos.getY() <= groundY;
}

/***********************************************************
 * LANDER : CHECK SAFETY LANDING
 * Check if this is a safe landing
 * LAB SPECIFICATION: Must land at < 4.0 m/s
 ***********************************************************/
bool Lander::checkSafetyLanding() const
{
   // Lab specification: Must land at less than 4.0 m/s
   bool slowSpeed = (velocity.getSpeed() < 4.0);
   bool uprightAngle = (angle.getRadians() < 0.2 || angle.getRadians() > 6.08); // ~±12 degrees
   
   return slowSpeed && uprightAngle;
}

/***********************************************************
 * LANDER : CONSUME FUEL
 * Private helper to consume fuel
 ***********************************************************/
void Lander::consumeFuel(double amount)
{
   fuel = std::max(0.0, fuel - amount);
}

/***********************************************************
 * LANDER : HAS FUEL FOR
 * Check if enough fuel for operation
 ***********************************************************/
bool Lander::hasFuelFor(double amount) const
{
   return fuel >= amount;
}

/***********************************************************
 * LANDER : GET FUEL PERCENTAGE
 * Return fuel as percentage of starting amount (5000 lbs = 2268 kg)
 ***********************************************************/
//double Lander::getFuelPercentage() const
//{
//   return (fuel / 2268.0) * 100.0; // 2268 kg = 5000 lbs starting fuel
//}

/***********************************************************
 * LANDER : NORMALIZE ANGLE
 * Keep angle in proper range
 ***********************************************************/
void Lander::normalizeAngle()
{
   // Angle class handles normalization automatically
}
