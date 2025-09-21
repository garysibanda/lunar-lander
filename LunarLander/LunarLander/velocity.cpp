#include "velocity.h"
#include "acceleration.h"
#include "angle.h"
#include <math.h>

void Velocity::add(const Acceleration& acceleration, double time)
{
   dx = dx + acceleration.getDDX() * time;
   dy = dy + acceleration.getDDY() * time;
}

double Velocity::getSpeed() const
{
   return sqrt((dx * dx) + (dy * dy));
}

void Velocity::set(const Angle& angle, double magnitude)
{
   dx = magnitude * sin(angle.radians);
   dy = magnitude * cos(angle.radians);
}

bool Velocity::isSafeLandingSpeedTest() const
{
   return (fabs(dx) <= 2.0 && fabs(dy) <= 2.0);
}
