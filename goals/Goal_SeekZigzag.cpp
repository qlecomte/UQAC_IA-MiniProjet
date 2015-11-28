#include "Goal_SeekZigzag.h"

#include "../Raven_SteeringBehaviors.h"


//------------------------------- Activate ------------------------------------
//-----------------------------------------------------------------------------
void Goal_SeekZigzag::Activate()
{
	m_iStatus = active;

	m_pOwner->GetSteering()->SeekOn();

	if (m_bClockwise)
		m_vTarget = m_pOwner->Pos() + (m_pOwner->Facing() + m_pOwner->Facing().Perp()) * m_pOwner->BRadius() * 3;

	else
		m_vTarget = m_pOwner->Pos() + (m_pOwner->Facing() - m_pOwner->Facing().Perp()) * m_pOwner->BRadius() * 3;
	

	if (m_pOwner->canWalkTo(m_vTarget))
		m_pOwner->GetSteering()->SetTarget(m_vTarget);

	else
	{
		//debug_con << "changing" << "";
		m_bClockwise = !m_bClockwise;
		m_iStatus = inactive;
	}


}



//-------------------------- Process ------------------------------------------
//-----------------------------------------------------------------------------
int Goal_SeekZigzag::Process()
{
	//if status is inactive, call Activate()
	ActivateIfInactive();

	//if target goes out of view terminate
	if (!m_pOwner->GetTargetSys()->isTargetWithinFOV())
	{
		m_iStatus = completed;
	}

	//else if bot reaches the target position set status to inactive so the goal 
	//is reactivated on the next update-step
	else if (m_pOwner->isAtPosition(m_vTarget))
	{
		m_iStatus = inactive;
	}

	return m_iStatus;
}

//---------------------------- Terminate --------------------------------------
//-----------------------------------------------------------------------------
void Goal_SeekZigzag::Terminate()
{
	m_pOwner->GetSteering()->SeekOff();
}

//---------------------------- Render -----------------------------------------

void Goal_SeekZigzag::Render()
{
	//#define SHOW_TARGET
#ifdef SHOW_TARGET
	gdi->OrangePen();
	gdi->HollowBrush();

	gdi->Line(m_pOwner->Pos(), m_vStrafeTarget);
	gdi->Circle(m_vStrafeTarget, 3);
#endif

}



