/***********************************************************************
 * Source File:
 *    GROUND
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    The lunar surface with realistic terrain generation
 ************************************************************************/

#include "ground.h"
#include "uiDraw.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

// Initialize constants
const double Ground::TERRAIN_ROUGHNESS = 0.6;
const double Ground::PLATFORM_MIN_WIDTH = 50.0;
const double Ground::PLATFORM_MAX_WIDTH = 100.0;
const int Ground::MIN_PLATFORM_DISTANCE = 50;

/*************************************************************************
 * GROUND : CONSTRUCTOR
 * Initialize the lunar surface
 *************************************************************************/
Ground::Ground(const Position& posUpperRight) :
   posUpperRight(posUpperRight),
   ground(nullptr),
   groundSize(0),
   platformWidth(0.0),
   platformHeight(0.0)
{
   reset(posUpperRight);
}

/*************************************************************************
 * GROUND : DESTRUCTOR - FIXED
 * Properly clean up allocated memory
 *************************************************************************/
Ground::~Ground()
{
   deallocateGround();
}

/*************************************************************************
 * GROUND : COPY CONSTRUCTOR - FIXED
 * Proper deep copy semantics
 *************************************************************************/
Ground::Ground(const Ground& rhs) :
   posUpperRight(rhs.posUpperRight),
   ground(nullptr),
   groundSize(rhs.groundSize),
   platformPosition(rhs.platformPosition),
   platformWidth(rhs.platformWidth),
   platformHeight(rhs.platformHeight)
{
   copyGround(rhs);
}

/*************************************************************************
 * GROUND : ASSIGNMENT OPERATOR - FIXED
 * Proper assignment with self-assignment check
 *************************************************************************/
Ground& Ground::operator=(const Ground& rhs)
{
   if (this != &rhs)  // Self-assignment check
   {
      deallocateGround();
      posUpperRight = rhs.posUpperRight;
      groundSize = rhs.groundSize;
      platformPosition = rhs.platformPosition;
      platformWidth = rhs.platformWidth;
      platformHeight = rhs.platformHeight;
      copyGround(rhs);
   }
   return *this;
}

/*************************************************************************
 * GROUND : RESET
 * Generate new terrain
 *************************************************************************/
void Ground::reset(const Position& posUpperRight)
{
   this->posUpperRight = posUpperRight;
   deallocateGround();
   generateTerrain();
   generatePlatform();
   smoothTerrain();
}

/*************************************************************************
 * GROUND : GET ELEVATION METERS
 * Get ground height at specific position
 *************************************************************************/
double Ground::getElevationMeters(const Position& pos) const
{
   if (!ground || groundSize == 0)
      return 0.0;
      
   // Convert position to array index
   int index = static_cast<int>((pos.getX() / posUpperRight.getX()) * groundSize);
   index = std::max(0, std::min(index, groundSize - 1));
   
   return ground[index];
}

/*************************************************************************
 * GROUND : ON PLATFORM
 * Check if lander is on the landing platform
 *************************************************************************/
bool Ground::onPlatform(const Position& posLander, int landerWidth) const
{
   double landerLeft = posLander.getX() - landerWidth / 2.0;
   double landerRight = posLander.getX() + landerWidth / 2.0;
   double platformLeft = platformPosition.getX() - platformWidth / 2.0;
   double platformRight = platformPosition.getX() + platformWidth / 2.0;
   
   // Check if lander is within platform bounds
   return (landerLeft >= platformLeft && landerRight <= platformRight);
}

/*************************************************************************
 * GROUND : DRAW
 * Draw the lunar surface
 *************************************************************************/
void Ground::draw(ogstream& gout) const
{
   if (!ground || groundSize == 0)
      return;
      
   // Draw terrain
   for (int i = 0; i < groundSize - 1; i++)
   {
      double x1 = (static_cast<double>(i) / groundSize) * posUpperRight.getX();
      double x2 = (static_cast<double>(i + 1) / groundSize) * posUpperRight.getX();
      
      Position start(x1, ground[i]);
      Position end(x2, ground[i + 1]);
      
      gout.drawLine(start, end, 0.7, 0.7, 0.7); // Gray lunar surface
   }
   
   // Draw landing platform in a different color
   double platformLeft = platformPosition.getX() - platformWidth / 2.0;
   double platformRight = platformPosition.getX() + platformWidth / 2.0;
   
   Position platStart(platformLeft, platformHeight);
   Position platEnd(platformRight, platformHeight);
   gout.drawLine(platStart, platEnd, 0.0, 1.0, 0.0); // Green platform
   
   // Draw platform markers
   Position marker1(platformLeft, platformHeight + 5);
   Position marker2(platformLeft, platformHeight + 15);
   gout.drawLine(marker1, marker2, 0.0, 1.0, 0.0);
   
   Position marker3(platformRight, platformHeight + 5);
   Position marker4(platformRight, platformHeight + 15);
   gout.drawLine(marker3, marker4, 0.0, 1.0, 0.0);
}

/*************************************************************************
 * GROUND : GENERATE TERRAIN - PRIVATE
 * Create realistic lunar terrain
 *************************************************************************/
void Ground::generateTerrain()
{
   groundSize = static_cast<int>(posUpperRight.getX());
   allocateGround(groundSize);
   
   double baseHeight = posUpperRight.getY() * 0.1; // Base terrain at 10% screen height
   
   // Generate terrain using simplified fractal algorithm
   for (int i = 0; i < groundSize; i++)
   {
      double variation = (rand() % 40 - 20) * TERRAIN_ROUGHNESS;
      ground[i] = baseHeight + variation;
      
      // Ensure terrain doesn't go below zero or too high
      ground[i] = std::max(5.0, std::min(ground[i], posUpperRight.getY() * 0.3));
   }
}

/*************************************************************************
 * GROUND : GENERATE PLATFORM - PRIVATE
 * Create landing platform
 *************************************************************************/
void Ground::generatePlatform()
{
   if (!ground || groundSize == 0)
      return;
      
   // Platform width between min and max
   platformWidth = PLATFORM_MIN_WIDTH +
                  (rand() % static_cast<int>(PLATFORM_MAX_WIDTH - PLATFORM_MIN_WIDTH));
   
   // Platform position - not too close to edges
   int minX = MIN_PLATFORM_DISTANCE;
   int maxX = groundSize - MIN_PLATFORM_DISTANCE - static_cast<int>(platformWidth);
   int platformX = minX + (rand() % (maxX - minX));
   
   // Set platform height to average of surrounding terrain
   double avgHeight = 0.0;
   int count = 0;
   for (int i = platformX - 10; i <= platformX + static_cast<int>(platformWidth) + 10; i++)
   {
      if (i >= 0 && i < groundSize)
      {
         avgHeight += ground[i];
         count++;
      }
   }
   platformHeight = (count > 0) ? avgHeight / count : ground[platformX];
   
   // Flatten platform area
   int platformStart = platformX - static_cast<int>(platformWidth) / 2;
   int platformEnd = platformX + static_cast<int>(platformWidth) / 2;
   
   for (int i = platformStart; i <= platformEnd && i < groundSize; i++)
   {
      if (i >= 0)
         ground[i] = platformHeight;
   }
   
   // Set platform position for collision detection
   platformPosition.setX((static_cast<double>(platformX) / groundSize) * posUpperRight.getX());
   platformPosition.setY(platformHeight);
}

/*************************************************************************
 * GROUND : SMOOTH TERRAIN - PRIVATE
 * Apply smoothing to make terrain more realistic
 *************************************************************************/
void Ground::smoothTerrain()
{
   if (!ground || groundSize < 3)
      return;
      
   // Simple smoothing algorithm - average with neighbors
   double* smoothed = new double[groundSize];
   
   // Copy original values
   for (int i = 0; i < groundSize; i++)
      smoothed[i] = ground[i];
      
   // Apply smoothing (except platform area)
   double platformLeft = platformPosition.getX() - platformWidth / 2.0;
   double platformRight = platformPosition.getX() + platformWidth / 2.0;
   
   for (int i = 1; i < groundSize - 1; i++)
   {
      double x = (static_cast<double>(i) / groundSize) * posUpperRight.getX();
      
      // Skip platform area
      if (x >= platformLeft && x <= platformRight)
         continue;
         
      smoothed[i] = (ground[i-1] + ground[i] + ground[i+1]) / 3.0;
   }
   
   // Copy smoothed values back
   for (int i = 0; i < groundSize; i++)
      ground[i] = smoothed[i];
      
   delete[] smoothed;
}

/*************************************************************************
 * GROUND : ALLOCATE GROUND - PRIVATE
 * Allocate memory for ground array
 *************************************************************************/
void Ground::allocateGround(int size)
{
   deallocateGround(); // Clean up any existing allocation
   if (size > 0)
   {
      ground = new double[size];
      groundSize = size;
   }
}

/*************************************************************************
 * GROUND : DEALLOCATE GROUND - PRIVATE
 * Clean up ground array memory
 *************************************************************************/
void Ground::deallocateGround()
{
   delete[] ground;
   ground = nullptr;
   groundSize = 0;
}

/*************************************************************************
 * GROUND : COPY GROUND - PRIVATE
 * Deep copy ground array from another object
 *************************************************************************/
void Ground::copyGround(const Ground& rhs)
{
   if (rhs.ground && rhs.groundSize > 0)
   {
      allocateGround(rhs.groundSize);
      for (int i = 0; i < groundSize; i++)
         ground[i] = rhs.ground[i];
   }
}
