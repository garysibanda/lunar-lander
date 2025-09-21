/***********************************************************************
 * Header File:
 *    Thrust : Represents activation of thrusters
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    down, clockwise, and counterclockwise - LAB SPECIFICATION COMPLIANT
 ************************************************************************/

#pragma once

#include "uiInteract.h"  // for Interface

class TestLander;
class TestThrust;

/*****************************************************
 * THRUST
 * Represents activation of thrusters
 * EXACT LAB SPECIFICATION IMPLEMENTATION
 *****************************************************/
class Thrust
{
   friend TestLander;
   friend TestThrust;
   
   public:
   // Thrust is initially turned off
   Thrust() : mainEngine(false), clockwise(false), counterClockwise(false) {}
   
   // Get rotation in radians per frame (LAB SPEC: 0.1 radians/frame)
   double rotation() const
   {
      return (clockwise ? 0.1 : 0.0) +
      (counterClockwise ? -0.1 : 0.0);
   }
   
   // get main engine thrust in m/s^2 (EXACT LAB SPECIFICATION)
   double mainEngineThrust() const
   {
      if (mainEngine)
      {
         const double thrust = 45000.00; // Newtons (lab specification)
         const double mass = 15103.00;   // Kilograms (lab specification)
         return thrust / mass;        // F = ma -> a = F / m = 2.98 m/s²
      }
      else
         return 0.0; // No thrust
   }
   
   // reflect what is firing
   bool isMain()    const { return mainEngine; }
   bool isClock()   const { return clockwise; }
   bool isCounter() const { return counterClockwise; }
   
   // set the thrusters - LAB SPECIFICATION CONTROLS
   void set(const Interface * pUI)
   {
      mainEngine        = pUI->isDown();        // Down arrow = main engine (lab spec)
      counterClockwise  = pUI->isLeft();        // Left arrow = rotate left (lab spec)
      clockwise         = pUI->isRight();       // Right arrow = rotate right (lab spec)
   }
   
   private:
   bool mainEngine;
   bool clockwise;
   bool counterClockwise;
};
