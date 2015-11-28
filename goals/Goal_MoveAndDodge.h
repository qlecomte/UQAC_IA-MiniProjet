#pragma once
#include "Goal_FollowPath.h"
#include "Goal_DodgeSideToSide.h"
#include "Goal_SeekZigzag.h"
#include "../Raven_Bot.h"


class Goal_MoveAndDodge : public Goal_Composite<Raven_Bot>
{
private:
	std::list<PathEdge>  m_Path;

public:
	Goal_MoveAndDodge(Raven_Bot* pBot, std::list<PathEdge> path);

	//the usual suspects
	void Activate();
	int Process();
	void Render();
	void Terminate(){}
};

