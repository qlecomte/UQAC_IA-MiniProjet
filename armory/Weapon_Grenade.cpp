#include "Weapon_Grenade.h"
#include "../Raven_Bot.h"
#include "misc/Cgdi.h"
#include "../Raven_Game.h"
#include "../Raven_Map.h"
#include "../lua/Raven_Scriptor.h"
#include "fuzzy/FuzzyOperators.h"


//--------------------------- ctor --------------------------------------------
//-----------------------------------------------------------------------------
GrenadeLauncher::GrenadeLauncher(Raven_Bot*   owner) :

Raven_Weapon(type_rocket_launcher,
script->GetInt("GrenadeLauncher_DefaultRounds"),
script->GetInt("GrenadeLauncher_MaxRoundsCarried"),
script->GetDouble("GrenadeLauncher_FiringFreq"),
script->GetDouble("GrenadeLauncher_IdealRange"),
script->GetDouble("Grenade_MaxSpeed"),
owner)
{
	//setup the vertex buffer
	const int NumWeaponVerts = 8;
	for (int vtx = 0; vtx<NumWeaponVerts; ++vtx)
	{
		m_vecWeaponVB.push_back(Vector2D((1 + cos(vtx*pi * 2 / NumWeaponVerts)) * 5+3, sin(vtx*pi * 2 / NumWeaponVerts) * 5 + 5));
	}

	//setup the fuzzy module
	InitializeFuzzyModule();

}


//------------------------------ ShootAt --------------------------------------
//-----------------------------------------------------------------------------
inline void GrenadeLauncher::ShootAt(Vector2D pos)
{
	if (NumRoundsRemaining() > 0 && isReadyForNextShot())
	{
		//fire off a grenade!
		m_pOwner->GetWorld()->AddGrenade(m_pOwner, pos);

		m_iNumRoundsLeft--;

		UpdateTimeWeaponIsNextAvailable();

		//add a trigger to the game so that the other bots can hear this shot
		//(provided they are within range)
		m_pOwner->GetWorld()->GetMap()->AddSoundTrigger(m_pOwner, script->GetDouble("RocketLauncher_SoundRange"));
	}
}

//---------------------------- Desirability -----------------------------------
//
//-----------------------------------------------------------------------------
double GrenadeLauncher::GetDesirability(double DistToTarget)
{
	if (m_iNumRoundsLeft == 0)
	{
		m_dLastDesirabilityScore = 0;
	}
	else
	{
		//fuzzify distance and amount of ammo
		m_FuzzyModule.Fuzzify("DistToTarget", DistToTarget);
		m_FuzzyModule.Fuzzify("AmmoStatus", (double)m_iNumRoundsLeft);

		m_dLastDesirabilityScore = m_FuzzyModule.DeFuzzify("Desirability", FuzzyModule::max_av);
	}

	return m_dLastDesirabilityScore;
}

//-------------------------  InitializeFuzzyModule ----------------------------
//
//  set up some fuzzy variables and rules
//-----------------------------------------------------------------------------
void GrenadeLauncher::InitializeFuzzyModule()
{
	FuzzyVariable& DistToTarget = m_FuzzyModule.CreateFLV("DistToTarget");
	FzSet& Target_VeryClose = DistToTarget.AddLeftShoulderSet("Target_VeryClose", 0, 15, 50);
	FzSet& Target_Close = DistToTarget.AddTriangularSet("Target_Close", 15, 50, 150);
	FzSet& Target_Medium = DistToTarget.AddTriangularSet("Target_Medium", 25, 150, 300);
	FzSet& Target_Far = DistToTarget.AddTriangularSet("Target_Far", 150, 300, 440);
	FzSet& Target_VeryFar = DistToTarget.AddRightShoulderSet("Target_VeryFar", 300, 440, 1000);

	FuzzyVariable& Desirability = m_FuzzyModule.CreateFLV("Desirability");
	FzSet& VeryVeryDesirable = Desirability.AddRightShoulderSet("VeryVeryDesirable", 60, 75, 100);
	FzSet& VeryDesirable = Desirability.AddTriangularSet("VeryDesirable", 40, 60, 75);
	FzSet& Desirable = Desirability.AddTriangularSet("Desirable", 25, 40, 60);
	FzSet& Undesirable = Desirability.AddTriangularSet("Undesirable", 10, 25, 40);
	FzSet& UnInteresting = Desirability.AddLeftShoulderSet("UnInteresting", 0, 10, 25);

	FuzzyVariable& AmmoStatus = m_FuzzyModule.CreateFLV("AmmoStatus");
	FzSet& Ammo_Lots = AmmoStatus.AddRightShoulderSet("Ammo_Lots", 30, 50, 100);
	FzSet& Ammo_Loads = AmmoStatus.AddTriangularSet("Ammo_Loads", 10, 30, 50);
	FzSet& Ammo_Okay = AmmoStatus.AddTriangularSet("Ammo_Okay", 0, 10, 30);
	FzSet& Ammo_Low = AmmoStatus.AddLeftShoulderSet("Ammo_Low", 1, 1, 10);
	FzSet& Ammo_No = AmmoStatus.AddSingletonSet("Ammo_No", 0, 0, 0);

	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_Lots), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_Loads), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_Okay), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_Low), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryClose, Ammo_No), UnInteresting);

	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Lots), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Loads), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Okay), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_Low), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Close, Ammo_No), UnInteresting);

	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Lots), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Loads), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Okay), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_Low), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Medium, Ammo_No), UnInteresting);

	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Lots), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Loads), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Okay), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_Low), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_Far, Ammo_No), UnInteresting);

	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_Lots), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_Loads), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_Okay), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_Low), VeryVeryDesirable);
	m_FuzzyModule.AddRule(FzAND(Target_VeryFar, Ammo_No), UnInteresting);
}


//-------------------------------- Render -------------------------------------
//-----------------------------------------------------------------------------
void GrenadeLauncher::Render()
{
	m_vecWeaponVBTrans = WorldTransform(m_vecWeaponVB,
		m_pOwner->Pos(),
		m_pOwner->Facing(),
		m_pOwner->Facing().Perp(),
		m_pOwner->Scale());

	gdi->RedPen();
	gdi->OrangeBrush();

	gdi->ClosedShape(m_vecWeaponVBTrans);
}