#pragma once

#include "Goals/Goal.h"
#include "Raven_Goal_Types.h"
#include "../Raven_Bot.h"


class Goal_SeekZigzag : public Goal<Raven_Bot>
{
private:

	Vector2D    m_vTarget;
	bool        m_bClockwise;
	Vector2D	GetTarget()const{ return m_vTarget; }

public:

	Goal_SeekZigzag(Raven_Bot* pBot) :Goal<Raven_Bot>(pBot, goal_strafe),
		m_bClockwise(RandBool())
	{}


	void Activate();

	int  Process();

	void Render();

	void Terminate();

};
