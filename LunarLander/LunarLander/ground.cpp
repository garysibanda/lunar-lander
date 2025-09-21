/***********************************************************************
 * Source File:
 *    GROUND
 * Author:
 *    Br. Helfrich and Gary Sibanda
 * Summary:
 *    The lunar surface with realistic terrain generation and filled rendering
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
 * GROUND : DESTRUCTOR
 *************************************************************************/
Ground::~Ground()
{
   deallocateGround();
}

/*************************************************************************
 * GROUND : COPY CONSTRUCTOR
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
 * GROUND : ASSIGNMENT OPERATOR
 *************************************************************************/
Ground& Ground::operator=(const Ground& rhs)
{
   if (this != &rhs)
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
   // REMOVED: smoothTerrain() - to keep jagged edges
}

/*************************************************************************
 * GROUND : GET ELEVATION METERS
 *************************************************************************/
double Ground::getElevationMeters(const Position& pos) const
{
   if (!ground || groundSize == 0)
      return 0.0;
      
   int index = static_cast<int>((pos.getX() / posUpperRight.getX()) * groundSize);
   index = std::max(0, std::min(index, groundSize - 1));
   
   return ground[index];
}

/*************************************************************************
 * GROUND : ON PLATFORM
 *************************************************************************/
bool Ground::onPlatform(const Position& posLander, int landerWidth) const
{
   double landerLeft = posLander.getX() - landerWidth / 2.0;
   double landerRight = posLander.getX() + landerWidth / 2.0;
   double platformLeft = platformPosition.getX() - platformWidth / 2.0;
   double platformRight = platformPosition.getX() + platformWidth / 2.0;
   
   return (landerLeft >= platformLeft && landerRight <= platformRight);
}

/*************************************************************************
 * GROUND : DRAW
 * Draw the lunar surface with FILLED TERRAIN and jagged edges
 *************************************************************************/
void Ground::draw(ogstream& gout) const
{
   if (!ground || groundSize == 0)
      return;
      
   // Draw filled terrain using triangles/quads
   for (int i = 0; i < groundSize - 1; i++)
   {
      double x1 = (static_cast<double>(i) / groundSize) * posUpperRight.getX();
      double x2 = (static_cast<double>(i + 1) / groundSize) * posUpperRight.getX();
      
      // Create filled rectangles from ground to bottom of screen
      Position bottomLeft(x1, 0);
      Position topLeft(x1, ground[i]);
      Position topRight(x2, ground[i + 1]);
      Position bottomRight(x2, 0);
      
      // Draw filled brown terrain
      gout.drawRectangle(bottomLeft, topRight, 0.54, 0.27, 0.07); // Brown color
   }
   
   // REMOVED: No more smooth white surface line for jagged look
   
   // Draw landing platform - BLUE STRIP ONLY (not extending down)
   double platformLeft = platformPosition.getX() - platformWidth / 2.0;
   double platformRight = platformPosition.getX() + platformWidth / 2.0;
   
   // Only draw the surface line of the platform (not a full rectangle down)
   Position platStart(platformLeft, platformHeight);
   Position platEnd(platformRight, platformHeight);
   gout.drawLine(platStart, platEnd, 0.0, 0.0, 1.0); // Blue landing strip line only
   
   // Optional: Small platform markers (just at the ends)
   Position marker1(platformLeft, platformHeight);
   Position marker2(platformLeft, platformHeight + 3);
   gout.drawLine(marker1, marker2, 0.0, 0.8, 1.0);
   
   Position marker3(platformRight, platformHeight);
   Position marker4(platformRight, platformHeight + 3);
   gout.drawLine(marker3, marker4, 0.0, 0.8, 1.0);
}

/*************************************************************************
 * GROUND : GENERATE TERRAIN
 * Generate mountainous terrain with moderate, natural jaggedness
 *************************************************************************/
void Ground::generateTerrain()
{
   groundSize = static_cast<int>(posUpperRight.getX() / 2); // Better balance of detail vs performance
   allocateGround(groundSize);
   
   double screenHeight = posUpperRight.getY();
   double baseHeight = screenHeight * 0.25; // Base at 25% screen height
   double maxHeight = screenHeight * 0.6;   // Mountains up to 60% screen height
   
   // Generate multiple mountain peaks and valleys
   for (int i = 0; i < groundSize; i++)
   {
      double x = static_cast<double>(i) / groundSize; // Normalize to 0-1
      
      // Create multiple sine waves for varied terrain
      double terrain = baseHeight;
      
      // Large mountains (primary features)
      terrain += sin(x * M_PI * 3.0) * (maxHeight - baseHeight) * 0.4;
      
      // Medium hills (secondary features)
      terrain += sin(x * M_PI * 7.0) * (maxHeight - baseHeight) * 0.2;
      
      // Small variations (detail)
      terrain += sin(x * M_PI * 15.0) * (maxHeight - baseHeight) * 0.1;
      
      // Moderate random noise for natural roughness (reduced from previous)
      double noise = (rand() % 30 - 15) * TERRAIN_ROUGHNESS; // Moderate level
      terrain += noise;
      
      // Ensure terrain stays within reasonable bounds
      ground[i] = std::max(screenHeight * 0.05, std::min(terrain, maxHeight));
   }
   
   // Add some dramatic peaks and valleys
   addTerrainFeatures();
}

/*************************************************************************
 * GROUND : GENERATE PLATFORM
 * Create a flat landing area in the varied terrain
 *************************************************************************/
void Ground::generatePlatform()
{
   if (!ground || groundSize == 0)
      return;
      
   platformWidth = PLATFORM_MIN_WIDTH +
                  (rand() % static_cast<int>(PLATFORM_MAX_WIDTH - PLATFORM_MIN_WIDTH));
   
   // Find a good location for the platform (not too high, not too low)
   int bestLocation = groundSize / 2; // Default to middle
   double bestHeight = ground[bestLocation];
   
   // Look for a location at reasonable height
   for (int i = MIN_PLATFORM_DISTANCE; i < groundSize - MIN_PLATFORM_DISTANCE; i++)
   {
      if (ground[i] > posUpperRight.getY() * 0.1 && ground[i] < posUpperRight.getY() * 0.4)
      {
         bestLocation = i;
         bestHeight = ground[i];
         break;
      }
   }
   
   platformHeight = bestHeight;
   
   // Flatten platform area to create landing strip
   int platformPixelWidth = static_cast<int>(platformWidth / 3); // Convert to array indices
   int platformStart = std::max(0, bestLocation - platformPixelWidth / 2);
   int platformEnd = std::min(groundSize - 1, bestLocation + platformPixelWidth / 2);
   
   for (int i = platformStart; i <= platformEnd; i++)
   {
      ground[i] = platformHeight;
   }
   
   // Set platform position for collision detection
   platformPosition.setX((static_cast<double>(bestLocation) / groundSize) * posUpperRight.getX());
   platformPosition.setY(platformHeight);
}

/*************************************************************************
 * GROUND : ADD TERRAIN FEATURES - PRIVATE
 * Add dramatic peaks and valleys to make terrain more interesting
 *************************************************************************/
void Ground::addTerrainFeatures()
{
   if (!ground || groundSize == 0)
      return;
      
   int numFeatures = 2 + (rand() % 3); // 2-4 dramatic features
   
   for (int f = 0; f < numFeatures; f++)
   {
      int center = MIN_PLATFORM_DISTANCE + (rand() % (groundSize - 2 * MIN_PLATFORM_DISTANCE));
      int width = 20 + (rand() % 40); // Feature width
      bool isPeak = (rand() % 2 == 0); // Randomly choose peak or valley
      
      double maxHeight = posUpperRight.getY() * 0.6;
      double minHeight = posUpperRight.getY() * 0.05;
      
      for (int i = std::max(0, center - width); i <= std::min(groundSize - 1, center + width); i++)
      {
         double distance = abs(i - center);
         double factor = 1.0 - (distance / width); // Smooth falloff
         
         if (factor > 0)
         {
            if (isPeak)
            {
               // Create peak
               ground[i] += factor * (maxHeight - ground[i]) * 0.5;
            }
            else
            {
               // Create valley
               ground[i] -= factor * (ground[i] - minHeight) * 0.5;
            }
            
            // Keep within bounds
            ground[i] = std::max(minHeight, std::min(ground[i], maxHeight));
         }
      }
   }
}

/*************************************************************************
 * GROUND : SMOOTH TERRAIN
 *************************************************************************/
void Ground::smoothTerrain()
{
   if (!ground || groundSize < 3)
      return;
      
   double* smoothed = new double[groundSize];
   
   for (int i = 0; i < groundSize; i++)
      smoothed[i] = ground[i];
      
   for (int i = 1; i < groundSize - 1; i++)
   {
      smoothed[i] = (ground[i-1] + ground[i] + ground[i+1]) / 3.0;
   }
   
   for (int i = 0; i < groundSize; i++)
      ground[i] = smoothed[i];
      
   delete[] smoothed;
}

/*************************************************************************
 * GROUND : ALLOCATE GROUND
 *************************************************************************/
void Ground::allocateGround(int size)
{
   deallocateGround();
   if (size > 0)
   {
      ground = new double[size];
      groundSize = size;
   }
}

/*************************************************************************
 * GROUND : DEALLOCATE GROUND
 *************************************************************************/
void Ground::deallocateGround()
{
   delete[] ground;
   ground = nullptr;
   groundSize = 0;
}

/*************************************************************************
 * GROUND : COPY GROUND
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
