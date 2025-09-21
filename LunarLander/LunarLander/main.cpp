/***********************************************************************
 * Source File:
 *    MAIN
 * Author:
 *    Gary Sibanda
 * Summary:
 *    Apollo 11 Lunar Lander Module Simulator
 *    A realistic physics simulation of the Apollo lunar lander
 ************************************************************************/

#include "position.h"
#include "uiInteract.h"
#include "uiDraw.h"
#include "ground.h"
#include "lander.h"

// For unit tests
#include "testRunner.h"

/*************************************************************************
 * SIMULATOR
 * Main simulator class containing all game logic and state
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
         gameTime += 0.1; // Increment game time
      }

      // Check for landing/crash
      checkCollisions();

      // Draw everything
      drawGame(gout);
      
      // Draw UI
      drawInterface(gout);
   }

private:
   Position posUpperRight;   // Screen dimensions
   Ground ground;           // Lunar surface
   Lander lander;          // The lunar lander
   double gameTime;        // Current game time
   int attempts;           // Number of landing attempts
   int successes;          // Number of successful landings
   bool showInstructions;  // Show control instructions

   /*************************************************************************
    * HANDLE INPUT
    * Process user input for lander control
    ************************************************************************/
   void handleInput(const Interface* pUI)
   {
      // Hide instructions after first input
      if (pUI->isDown() || pUI->isLeft() || pUI->isRight())
         showInstructions = false;

      // Reset game on spacebar when dead or landed
      if (pUI->isSpace() && !lander.isFlying())
      {
         resetGame();
      }
   }

   /*************************************************************************
    * UPDATE PHYSICS
    * Update the physics simulation
    ************************************************************************/
   void updatePhysics(const Interface* pUI)
   {
      // Create thrust object based on input
      Thrust thrust;
      thrust.set(pUI);

      // Apply physics for this frame (assuming 60 FPS = 1/60 second)
      double timeStep = 1.0 / 60.0;
      
      // Get acceleration from input and gravity
      Acceleration acceleration = lander.input(thrust, -1.625); // Lunar gravity

      // Update lander position and velocity
      lander.coast(acceleration, timeStep);
   }

   /*************************************************************************
    * CHECK COLLISIONS
    * Check for ground collision and determine outcome
    ************************************************************************/
   void checkCollisions()
   {
      if (!lander.isFlying())
         return;

      Position landerPos = lander.getPosition();
      double groundHeight = ground.getElevationMeters(landerPos);

      // Check if lander has hit the ground
      if (landerPos.getY() <= groundHeight)
      {
         attempts++;
         
         // Check if it's a safe landing
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
    * Reset for a new landing attempt
    ************************************************************************/
   void resetGame()
   {
      lander.reset(posUpperRight);
      ground.reset(posUpperRight);
      gameTime = 0.0;
      showInstructions = true;
   }

   /*************************************************************************
    * DRAW GAME
    * Draw all game objects
    ************************************************************************/
   void drawGame(ogstream& gout)
   {
      // Draw lunar surface
      ground.draw(gout);

      // Draw lander
      gout.drawLander(lander.getPosition(), lander.getAngle().getRadians());

      // Draw thrust flames if engines are firing
      Thrust currentThrust;
      // Note: We'd need to store thrust state to show flames accurately
      gout.drawLanderFlames(lander.getPosition(), lander.getAngle().getRadians(),
                           false, false, false); // Would use actual thrust state
   }

   /*************************************************************************
    * DRAW INTERFACE
    * Draw UI elements and game information
    ************************************************************************/
   void drawInterface(ogstream& gout)
   {
      // Game statistics in top-left
      Position statsPos(10, posUpperRight.getY() - 30);
      gout.setPosition(statsPos);
      gout << "Apollo 11 Lunar Lander Simulator\n";
      gout << "Time: " << static_cast<int>(gameTime) << "s\n";
      gout << "Fuel: " << lander.getFuel() << " kg ("
           << static_cast<int>(lander.getFuelPercentage()) << "%)\n";
      gout << "Speed: " << static_cast<int>(lander.getSpeed() * 10) / 10.0 << " m/s\n";
      gout << "Altitude: " << static_cast<int>(lander.getPosition().getY() -
                                              ground.getElevationMeters(lander.getPosition())) << " m\n";

      // Landing statistics
      gout << "\nMission Statistics:\n";
      gout << "Attempts: " << attempts << "\n";
      gout << "Successes: " << successes << "\n";
      if (attempts > 0)
         gout << "Success Rate: " << (successes * 100 / attempts) << "%\n";

      // Game status
      Position statusPos(10, 100);
      gout.setPosition(statusPos);
      
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
         gout << "LUNAR LANDING INSTRUCTIONS:\n";
         gout << "DOWN ARROW  - Main engine thrust\n";
         gout << "LEFT ARROW  - Rotate counter-clockwise\n";
         gout << "RIGHT ARROW - Rotate clockwise\n";
         gout << "\nLand safely on the platform!\n";
         gout << "Max safe speed: 4 m/s vertical, 2 m/s horizontal\n";
      }

      // Fuel warning
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
 * Main callback function for OpenGL
 ************************************************************************/
void callBack(const Interface* pUI, void* p)
{
   static_cast<Simulator*>(p)->display(pUI);
}

/*************************************************************************
 * MAIN
 * Main entry point
 ************************************************************************/
int main(int argc, char** argv)
{
   // Run unit tests in debug mode
   #ifdef DEBUG
   testRunner();
   #endif

   // Set up the screen dimensions
   Position posUpperRight(800.0, 600.0);

   // Create the simulator
   Simulator simulator(posUpperRight);

   // Initialize the graphics system
   Interface ui("Apollo 11 Lunar Lander Module Simulator", posUpperRight);
   ui.run(callBack, &simulator);

   return 0;
}
