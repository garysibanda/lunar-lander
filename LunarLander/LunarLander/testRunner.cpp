/***********************************************************************
 * Source File:
 *    TEST RUNNER
 * Author:
 *    Gary Sibanda
 * Summary:
 *    Run all unit tests for the Apollo 11 Lunar Lander simulation
 ************************************************************************/

#include "testRunner.h"

// Include all test classes
#include "testAngle.h"
#include "testPosition.h"
#include "testVelocity.h"
#include "testThrust.h"
#include "testLander.h"

#include <iostream>

/*************************************************************************
 * TEST RUNNER
 * Execute all unit tests and display results
 ************************************************************************/
void testRunner()
{
   std::cout << "===================================\n";
   std::cout << " Apollo 11 Lunar Lander Unit Tests\n";
   std::cout << "===================================\n\n";

   // Run tests for each component
   TestAngle().run();
   TestPosition().run();
   TestVelocity().run();
   TestThrust().run();
   TestLander().run();

   std::cout << "\n===================================\n";
   std::cout << " All Unit Tests Complete\n";
   std::cout << "===================================\n\n";
}
