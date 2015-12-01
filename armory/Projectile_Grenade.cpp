#include "Projectile_Grenade.h"
#include "../lua/Raven_Scriptor.h"
#include "misc/cgdi.h"
#include "../Raven_Bot.h"
#include "../Raven_Game.h"
#include "../constants.h"
#include "2d/WallIntersectionTests.h"
#include "../Raven_Map.h"

#include "../Raven_Messages.h"
#include "Messaging/MessageDispatcher.h"

#define  LOG_CREATIONAL_STUFF
#include "debug/DebugConsole.h"


//-------------------------- ctor ---------------------------------------------
//-----------------------------------------------------------------------------
Grenade::Grenade(Raven_Bot* shooter, Vector2D target) :

Raven_Projectile(target,
shooter->GetWorld(),
shooter->ID(),
shooter->Pos(),
shooter->Facing(),
script->GetInt("Grenade_Damage"),
script->GetDouble("Grenade_Scale"),
script->GetDouble("Grenade_MaxSpeed"),
script->GetDouble("Grenade_Mass"),
script->GetDouble("Grenade_MaxForce")),

m_dCurrentBlastRadius(0.0),
m_dBlastRadius(script->GetDouble("Grenade_BlastRadius")),
m_duration(script->GetDouble("Grenade_Duration")),
m_time(0),
m_explosionTime(script->GetInt("Grenade_ExplosionTime"))
{
   assert (target != Vector2D());
}


//------------------------------ Update ---------------------------------------
//-----------------------------------------------------------------------------
void Grenade::Update()
{
  if (!m_bImpacted)
  {
	  if (m_time < m_duration)
		m_vVelocity = MaxSpeed() * Heading()*(1 - (float)m_time / m_duration);

    //make sure vehicle does not exceed maximum velocity
    m_vVelocity.Truncate(m_dMaxSpeed);

	m_time++;
#ifdef LOG_CREATIONAL_STUFF
	debug_con << m_time << "/" << m_duration << "=>" << m_vVelocity.Length();
#endif

    //update the position
    m_vPosition += m_vVelocity;

	TestForImpact();

	if (m_time > m_explosionTime)
	{
		InflictDamageOnBotsWithinBlastRadius();
		m_bImpacted = true;
	}
  }
  else
  {
    m_dCurrentBlastRadius += script->GetDouble("Rocket_ExplosionDecayRate");

    //when the rendered blast circle becomes equal in size to the blast radius
    //the rocket can be removed from the game
    if (m_dCurrentBlastRadius > m_dBlastRadius)
    {
      m_bDead = true;
    }
  }

  
  
}

bool Grenade::FindClosestPointOfIntersectionWithWalls(Vector2D A,
	Vector2D        B,
	double&          distance,
	Vector2D&		normal,
	Vector2D&       ip,
	const std::vector<Wall2D*>& walls)
{
	distance = MaxDouble;

	std::vector<Wall2D*>::const_iterator curWall = walls.begin();
	for (curWall; curWall != walls.end(); ++curWall)
	{
		double dist = 0.0;
		Vector2D point;

		if (LineIntersection2D(A, B, (*curWall)->From(), (*curWall)->To(), dist, point))
		{
			if (dist < distance)
			{
				distance = dist;
				ip = point;
				normal = (*curWall)->Normal();
			}
		}
	}

	if (distance < MaxDouble) return true;

	return false;
}

void Grenade::TestForImpact()
{
   
    //if the projectile has reached the target position or it hits an entity
    //or wall it should explode/inflict damage/whatever and then mark itself
    //as dead


    //test to see if the line segment connecting the rocket's current position
    //and previous position intersects with any bots.
    /*Raven_Bot* hit = GetClosestIntersectingBot(m_vPosition - m_vVelocity, m_vPosition);
    
    //if hit
    if (hit)
    {
      m_bImpacted = true;

      //send a message to the bot to let it know it's been hit, and who the
      //shot came from
      Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                              m_iShooterID,
                              hit->ID(),
                              Msg_TakeThatMF,
                              (void*)&m_iDamageInflicted);

      //test for bots within the blast radius and inflict damage
      InflictDamageOnBotsWithinBlastRadius();
    }*/

    //test for impact with a wall
    double dist;
	Vector2D normal;
     if( FindClosestPointOfIntersectionWithWalls(m_vPosition - m_vVelocity,
                                                 m_vPosition,
                                                 dist,
												 normal,
                                                 m_vImpactPoint,
                                                 m_pWorld->GetMap()->GetWalls()))
     {
        //m_bImpacted = true;
      
        //test for bots within the blast radius and inflict damage
        //InflictDamageOnBotsWithinBlastRadius();
		 m_vPosition -= m_vVelocity;
		 double normalSpeedLength = normal.x*m_vHeading.x + normal.y*m_vHeading.y;
		 Vector2D normalSpeed = normal * normalSpeedLength;
		Vector2D tangentSpeed = m_vHeading - normalSpeed;
		
		m_vHeading = tangentSpeed - normalSpeed;

        return;
    }
    
    /*//test to see if rocket has reached target position. If so, test for
     //all bots in vicinity
    const double tolerance = 5.0;   
    if (Vec2DDistanceSq(Pos(), m_vTarget) < tolerance*tolerance)
    {
      m_bImpacted = true;

      InflictDamageOnBotsWithinBlastRadius();
    }*/
}

//--------------- InflictDamageOnBotsWithinBlastRadius ------------------------
//
//  If the rocket has impacted we test all bots to see if they are within the 
//  blast radius and reduce their health accordingly
//-----------------------------------------------------------------------------
void Grenade::InflictDamageOnBotsWithinBlastRadius()
{
  std::list<Raven_Bot*>::const_iterator curBot = m_pWorld->GetAllBots().begin();

  for (curBot; curBot != m_pWorld->GetAllBots().end(); ++curBot)
  {
    if (Vec2DDistance(Pos(), (*curBot)->Pos()) < m_dBlastRadius + (*curBot)->BRadius())
    {
      //send a message to the bot to let it know it's been hit, and who the
      //shot came from
      Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                              m_iShooterID,
                              (*curBot)->ID(),
                              Msg_TakeThatMF,
                              (void*)&m_iDamageInflicted);
      
    }
  }  
}


//-------------------------- Render -------------------------------------------
//-----------------------------------------------------------------------------
void Grenade::Render()
{
  if (m_bImpacted)
  {
    gdi->HollowBrush();
    gdi->Circle(Pos(), m_dCurrentBlastRadius);
  }
  else
  {
	  gdi->RedPen();
	  gdi->OrangeBrush();
	  gdi->Circle(Pos(), 3);
  }
}