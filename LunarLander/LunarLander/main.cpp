/***********************************************************************
 * Source File:
 *    MAIN - LAB SPECIFICATION COMPLIANT
 * Author:
 *    Gary Sibanda
 * Summary:
 *    Apollo 11 Lunar Lander Module Simulator following exact lab specs
 ************************************************************************/

#include "position.h"
#include "uiInteract.h"
#include "uiDraw.h"
#include "ground.h"
#include "lander.h"
#include <cstdlib>
#include <ctime>

// For unit tests
#include "testRunner.h"

/*************************************************************************
 * STAR
 * Represents a twinkling star in space
 ************************************************************************/
struct Star
{
   Position pos;
   unsigned char phase;
   
   Star() : pos(0, 0), phase(0) {}  // Default constructor
   Star(double x, double y) : pos(x, y), phase(rand() % 256) {}
   
   void update() { phase = (phase + 1) % 256; } // Cycle through phases
};

/*************************************************************************
 * SIMULATOR
 * Main simulator class following Lab specifications
 ************************************************************************/
class Simulator
{
public:
   Simulator(const Position& posUpperRight) :
      posUpperRight(posUpperRight),
      ground(posUpperRight),
      lander(posUpperRight),
      gameTime(0.0),
      attempts(0),
      successes(0),
      showInstructions(true)
   {
      generateStars();
   }

   // Main game callback
   void display(const Interface* pUI)
   {
      ogstream gout;

      // Handle input
      handleInput(pUI);

      // Update physics if playing
      if (lander.isFlying())
      {
         updatePhysics(pUI);
         gameTime += 0.1; // Increment game time (each frame = 1/10th second per lab spec)
      }

      // Check for landing/crash
      checkCollisions();

      // Draw everything
      drawGame(gout, pUI);
      
      // Draw UI following lab specifications
      drawInterface(gout, pUI);
   }

private:
   Position posUpperRight;   // Screen dimensions
   Ground ground;           // Lunar surface
   Lander lander;          // The lunar lander
   double gameTime;        // Current game time
   int attempts;           // Number of landing attempts
   int successes;          // Number of successful landings
   bool showInstructions;  // Show control instructions
   
   // Stars for space background (Lab spec: about 50 stars)
   static const int NUM_STARS = 50;
   Star stars[NUM_STARS];

   /*************************************************************************
    * GENERATE STARS
    * Create random stars across the screen
    ************************************************************************/
   void generateStars()
   {
      for (int i = 0; i < NUM_STARS; i++)
      {
         double x = (rand() % static_cast<int>(posUpperRight.getX()));
         double y = (rand() % static_cast<int>(posUpperRight.getY() * 0.7)) +
                   (posUpperRight.getY() * 0.3); // Stars in upper 70% of screen
         stars[i] = Star(x, y);
      }
   }

   /*************************************************************************
    * HANDLE INPUT
    * Lab spec: DOWN = thrust, LEFT = rotate CCW, RIGHT = rotate CW
    ************************************************************************/
   void handleInput(const Interface* pUI)
   {
      if (pUI->isDown() || pUI->isLeft() || pUI->isRight())
         showInstructions = false;

      if (pUI->isSpace() && !lander.isFlying())
      {
         resetGame();
      }
   }

   /*************************************************************************
    * UPDATE PHYSICS - LAB SPECIFICATION COMPLIANT
    * Each frame = 1/10th second
    * Lunar gravity = 1.625 m/s²
    * Thrust = 45,000 N / 15,103 kg = 2.98 m/s²
    * Fuel consumption: 10 lbs/s main, 1 lb/s attitude
    * Rotation: 0.1 radians/frame
    ************************************************************************/
   void updatePhysics(const Interface* pUI)
   {
      Thrust thrust;
      thrust.set(pUI);

      // LAB SPECIFICATION: Each frame accounts for 1/10th of a second
      double timeStep = 0.1;  // Exactly as specified in lab documents
      
      // LAB SPECIFICATION: Lunar gravity = 1.625 m/s²
      Acceleration acceleration = lander.input(thrust, -1.625);

      // Update lander position and velocity
      lander.coast(acceleration, timeStep);
      
      // Update star twinkling
      for (int i = 0; i < NUM_STARS; i++)
      {
         stars[i].update();
      }
   }

   /*************************************************************************
    * CHECK COLLISIONS
    * Lab spec: Crash unless hitting landing pad at < 4.0 m/s AND upright
    * Landing pad: 30 meters across, lander: 20 meters across
    * Must be nearly upright (within ±12 degrees)
    ************************************************************************/
   void checkCollisions()
   {
      if (!lander.isFlying())
         return;

      Position landerPos = lander.getPosition();
      double groundHeight = ground.getElevationMeters(landerPos);

      if (landerPos.getY() <= groundHeight)
      {
         attempts++;
         
         // CORRECTED: Use checkSafetyLanding() which includes ALL requirements:
         // 1. Speed < 4.0 m/s
         // 2. Nearly upright angle (±12 degrees)
         // 3. Must also be on the landing platform
         if (lander.checkSafetyLanding() && ground.onPlatform(landerPos, lander.getWidth()))
         {
            lander.land();
            successes++;
         }
         else
         {
            lander.crash();
         }
      }
   }

   /*************************************************************************
    * RESET GAME
    ************************************************************************/
   void resetGame()
   {
      lander.reset(posUpperRight);
      ground.reset(posUpperRight);
      generateStars(); // New stars for each mission
      gameTime = 0.0;
      showInstructions = true;
   }

   /*************************************************************************
    * DRAW GAME
    * Draw all game objects in proper order
    ************************************************************************/
   void drawGame(ogstream& gout, const Interface* pUI)
   {
      // 1. Draw stars first (background) - Lab spec: about 50 stars
      for (int i = 0; i < NUM_STARS; i++)
      {
         gout.drawStar(stars[i].pos, stars[i].phase);
      }
      
      // 2. Draw lunar surface (filled terrain)
      ground.draw(gout);

      // 3. Draw lander
      gout.drawLander(lander.getPosition(), lander.getAngle().getRadians());

      // 4. Draw thrust flames based on current input
      Thrust currentThrust;
      currentThrust.set(pUI);
      
      gout.drawLanderFlames(lander.getPosition(),
                           lander.getAngle().getRadians(),
                           currentThrust.isMain(),      // Main engine flame
                           currentThrust.isClock(),     // Clockwise thruster
                           currentThrust.isCounter());  // Counter-clockwise thruster
   }

   /*************************************************************************
    * DRAW INTERFACE - LAB SPECIFICATION FORMAT
    * Lab spec shows: Fuel: 2272 lbs, Altitude: 35 meters, Speed: 12.91 m/s
    ************************************************************************/
   void drawInterface(ogstream& gout, const Interface* pUI)
   {
      // Lab specification format for status display
      Position statusPos(10, posUpperRight.getY() - 30);
      gout.setPosition(statusPos);
      
      // Convert kg to lbs for fuel display (lab spec shows lbs)
      int fuelLbs = static_cast<int>(lander.getFuel() * 2.20462); // kg to lbs conversion
      int altitude = static_cast<int>(lander.getPosition().getY() -
                                     ground.getElevationMeters(lander.getPosition()));
      double speed = lander.getSpeed();
      
      gout << "Fuel: " << fuelLbs << " lbs\n";
      gout << "Altitude: " << altitude << " meters\n";
      gout << "Speed: " << static_cast<int>(speed * 100) / 100.0 << " m/s\n";

      // Lab specification physics info
      Thrust debugThrust;
      debugThrust.set(pUI);
      gout << "\nLAB SPECIFICATION PHYSICS:\n";
      gout << "Frame time: 1/10th second | Lunar gravity: 1.625 m/s²\n";
      gout << "Thrust: 45,000 N | Mass: 15,103 kg | Accel: 2.98 m/s²\n";
      gout << "Fuel consumption: 10 lbs/s main, 1 lb/s attitude\n";
      gout << "Rotation: 0.1 radians/frame\n";
      
      gout << "\nCONTROLS (Lab Specification):\n";
      gout << "DOWN ARROW  - Main engine thrust (10 lbs fuel/frame)\n";
      gout << "LEFT ARROW  - Rotate CCW (1 lb fuel/frame)\n";
      gout << "RIGHT ARROW - Rotate CW (1 lb fuel/frame)\n";

      Position statusPos2(10, 100);
      gout.setPosition(statusPos2);
      
      if (lander.isDead())
      {
         gout << "MISSION FAILED!\n";
         gout << "The Eagle has crashed.\n";
         gout << "Press SPACE to try again.\n";
      }
      else if (lander.isLanded())
      {
         gout << "THE EAGLE HAS LANDED!\n";
         gout << "Successful lunar touchdown!\n";
         gout << "Press SPACE for next mission.\n";
      }
      else if (showInstructions)
      {
         gout << "APOLLO 11 LUNAR LANDER (Lab Specification)\n";
         gout << "\nLand safely on the BLUE platform!\n";
         gout << "Must land at less than 4.0 m/s to avoid crash\n";
         gout << "Landing pad: 30m wide, Lander: 20m wide\n";
         gout << "Starting fuel: 5,000 lbs\n";
      }

      // Lab spec warning at low fuel
      if (lander.getFuelPercentage() < 20.0 && lander.isFlying())
      {
         Position warnPos(posUpperRight.getX() / 2 - 100, posUpperRight.getY() / 2);
         gout.setPosition(warnPos);
         gout << "!!! LOW FUEL WARNING !!!\n";
      }
   }
};

/*************************************************************************
 * CALLBACK
 ************************************************************************/
void callBack(const Interface* pUI, void* p)
{
   static_cast<Simulator*>(p)->display(pUI);
}

/*************************************************************************
 * MAIN
 ************************************************************************/
int main(int argc, char** argv)
{
   // Initialize random seed
   srand(static_cast<unsigned>(time(nullptr)));
   
   #ifdef DEBUG
   testRunner();
   #endif

   Position posUpperRight(800.0, 600.0);
   Simulator simulator(posUpperRight);
   Interface ui("Apollo 11 Lunar Lander Module Simulator", posUpperRight);
   ui.run(callBack, &simulator);

   return 0;
}
