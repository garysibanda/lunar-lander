/***********************************************************************
 * Header File:
 *    GROUND
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    The lunar surface with realistic terrain generation
 ************************************************************************/

#pragma once

#include "position.h"

// Forward declarations
class ogstream;

/*****************************************************
 * GROUND
 * Represents the lunar surface with landing platforms
 *****************************************************/
class Ground
{
public:
   // Constructor - creates lunar terrain
   Ground(const Position& posUpperRight);
   
   // Destructor - FIXED: Added proper cleanup
   ~Ground();
   
   // Copy constructor - FIXED: Added proper copy semantics
   Ground(const Ground& rhs);
   
   // Assignment operator - FIXED: Added proper assignment
   Ground& operator=(const Ground& rhs);

   // Reset the ground to a new configuration
   void reset(const Position& posUpperRight);

   // Get the elevation at a specific position
   double getElevationMeters(const Position& pos) const;
   
   // Check if position is on a landing platform
   bool onPlatform(const Position& posLander, int landerWidth) const;
   
   // Get platform information
   Position getPlatformPosition() const { return platformPosition; }
   double getPlatformWidth() const { return platformWidth; }

   // Draw the lunar surface
   void draw(ogstream& gout) const;

private:
   Position posUpperRight;    // Screen dimensions
   double* ground;           // Array of ground elevations - FIXED: Will be properly managed
   int groundSize;           // Size of the ground array
   Position platformPosition; // Landing platform location
   double platformWidth;     // Width of landing platform
   double platformHeight;    // Height of landing platform
   
   // Enhanced terrain generation
   void generateTerrain();
   void generatePlatform();
   void smoothTerrain();
   
   // Helper functions for memory management - FIXED: Added proper helpers
   void allocateGround(int size);
   void deallocateGround();
   void copyGround(const Ground& rhs);
   
   // Terrain generation parameters
   static const double TERRAIN_ROUGHNESS;
   static const double PLATFORM_MIN_WIDTH;
   static const double PLATFORM_MAX_WIDTH;
   static const int MIN_PLATFORM_DISTANCE;
};
