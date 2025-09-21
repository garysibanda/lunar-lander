/***********************************************************************
 * Source File:
 *    LANDER
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    All the information about the lunar lander
 ************************************************************************/

#include "lander.h"
#include "uiDraw.h"
#include <cstdlib>  // for rand()
#include <cmath>

// Initialize physics constants
const double Lander::FUEL_CONSUMPTION_MAIN = 10.0;      // kg/s
const double Lander::FUEL_CONSUMPTION_ATTITUDE = 1.0;   // kg/s
const double Lander::THRUST_MAIN = 45000.0;             // Newtons
const double Lander::THRUST_ATTITUDE = 0.1;             // radians/s

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
   fuel = 5000.0;
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
 ***********************************************************/
Acceleration Lander::input(const Thrust& thrust, double gravity)
{
   Acceleration acceleration;
   
   // Always apply gravity
   acceleration.setDDY(gravity);
   
   // Only process thrust if we have fuel and are flying
   if (status == PLAYING && fuel > 0.0)
   {
      // Main engine thrust
      if (thrust.isMain())
      {
         double thrustAcceleration = thrust.mainEngineThrust();
         acceleration.addDDX(-sin(angle.getRadians()) * thrustAcceleration);
         acceleration.addDDY(cos(angle.getRadians()) * thrustAcceleration);
         consumeFuel(FUEL_CONSUMPTION_MAIN);
      }
      
      // Attitude control
      if (thrust.isClock())
      {
         angle.add(thrust.rotation());
         consumeFuel(FUEL_CONSUMPTION_ATTITUDE);
      }
      
      if (thrust.isCounter())
      {
         angle.add(thrust.rotation());
         consumeFuel(FUEL_CONSUMPTION_ATTITUDE);
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
 * LANDER : APPLY THRUST
 * Apply thrust forces over time
 ***********************************************************/
void Lander::applyThrust(const Thrust& thrust, double time)
{
   if (status != PLAYING || fuel <= 0.0)
      return;
      
   if (thrust.isMain() && hasFuelFor(FUEL_CONSUMPTION_MAIN * time))
   {
      double thrustForce = THRUST_MAIN / getTotalMass();
      velocity.addDX(-sin(angle.getRadians()) * thrustForce * time);
      velocity.addDY(cos(angle.getRadians()) * thrustForce * time);
      updateFuel(FUEL_CONSUMPTION_MAIN * time);
   }
   
   if ((thrust.isClock() || thrust.isCounter()) &&
       hasFuelFor(FUEL_CONSUMPTION_ATTITUDE * time))
   {
      angle.add(thrust.rotation() * time);
      updateFuel(FUEL_CONSUMPTION_ATTITUDE * time);
   }
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
 ***********************************************************/
bool Lander::checkSafetyLanding() const
{
   // Safe landing criteria:
   // 1. Low horizontal speed (< 2.0 m/s)
   // 2. Low vertical speed (< 4.0 m/s)
   // 3. Nearly upright (angle close to 0)
   
   bool slowSpeed = velocity.isSafeLandingSpeedTest();
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
 * LANDER : NORMALIZE ANGLE
 * Keep angle in proper range
 ***********************************************************/
void Lander::normalizeAngle()
{
   // Angle class handles normalization automatically
}
