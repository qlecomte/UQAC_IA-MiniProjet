#include "Goal_MoveAndDodge.h"

#include "Debug/DebugConsole.h"


Goal_MoveAndDodge::Goal_MoveAndDodge(Raven_Bot* pBot, std::list<PathEdge> path) : 
	Goal_Composite<Raven_Bot>(pBot, goal_move_and_dodge),
	m_Path(path)
{

}

//------------------------------ Activate -------------------------------------
//-----------------------------------------------------------------------------
void Goal_MoveAndDodge::Activate()
{
	m_iStatus = active;

	//if this goal is reactivated then there may be some existing subgoals that
	//must be removed
	RemoveAllSubgoals();

	if (m_pOwner->GetTargetSys()->isTargetWithinFOV()){
		// Faire des zig-zag
		debug_con << "Faire des Zig-zag" << "";

		AddSubgoal(new Goal_SeekZigzag(m_pOwner));

	}
	else{
		AddSubgoal(new Goal_FollowPath(m_pOwner, m_Path));
	}

}


//-------------------------- Process ------------------------------------------
//-----------------------------------------------------------------------------
int Goal_MoveAndDodge::Process()
{
	//if status is inactive, call Activate()
	ActivateIfInactive();

	m_iStatus = ProcessSubgoals();

	return m_iStatus;
}

//---------------------------- Render -----------------------------------------
//-----------------------------------------------------------------------------
void Goal_MoveAndDodge::Render()
{
	//render all the path waypoints remaining on the path list
	std::list<PathEdge>::iterator it;
	for (it = m_Path.begin(); it != m_Path.end(); ++it)
	{
		gdi->BlackPen();
		gdi->LineWithArrow(it->Source(), it->Destination(), 5);

		gdi->RedBrush();
		gdi->BlackPen();
		gdi->Circle(it->Destination(), 3);
	}

	//forward the request to the subgoals
	Goal_Composite<Raven_Bot>::Render();
}

