#ifndef GRENADE_H
#define GRENADE_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Rocket.h
//
//  Author: Mat Buckland (ai-junkie.com)
//
//  Desc:   class to implement a rocket
//
//-----------------------------------------------------------------------------

#include "Raven_Projectile.h"

class Raven_Bot;
class Wall2D;

class Grenade : public Raven_Projectile
{
private:

  //the radius of damage, once the rocket has impacted
  double    m_dBlastRadius;

  //this is used to render the splash when the rocket impacts
  double    m_dCurrentBlastRadius;

  int m_time, m_duration, m_explosionTime;



  //If the rocket has impacted we test all bots to see if they are within the 
  //blast radius and reduce their health accordingly
  void InflictDamageOnBotsWithinBlastRadius();

    //tests the trajectory of the shell for an impact
  void TestForImpact();

  bool FindClosestPointOfIntersectionWithWalls(Vector2D A,
	  Vector2D        B,
	  double&          distance,
	  Vector2D&		normal,
	  Vector2D&       ip,
	  const std::vector<Wall2D*>& walls);

public:

  Grenade(Raven_Bot* shooter, Vector2D target);
  
  void Render();

  void Update();
  
};


#endif