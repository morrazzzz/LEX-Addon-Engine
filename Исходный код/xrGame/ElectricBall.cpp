///////////////////////////////////////////////////////////////
// ElectricBall.cpp
// ElectricBall - артефакт электрический шар
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ElectricBall.h"
#include "../xrphysics/PhysicsShell.h"
// Lex Addon (correct by Suhar_) 17.05.2019		(begin)
// Добавлено чтение параметра jump_height для этого класса артефактов
#include "level.h"
// Lex Addon (correct by Suhar_) 17.05.2019		(end)


CElectricBall::CElectricBall(void) 
{
	// Lex Addon (correct by Suhar_) 17.05.2019		(begin)
	// Добавлено чтение параметра jump_height для этого класса артефактов
	m_fJumpHeight = 0;
	// Lex Addon (correct by Suhar_) 17.05.2019		(end)
}

CElectricBall::~CElectricBall(void) 
{
}

void CElectricBall::Load(LPCSTR section) 
{
	inherited::Load(section);
	
	// Lex Addon (correct by Suhar_) 17.05.2019		(begin)
	// Добавлено чтение параметра jump_height для этого класса артефактов
	if(pSettings->line_exist(section, "jump_height"))
		m_fJumpHeight = pSettings->r_float(section,"jump_height");
	// Lex Addon (correct by Suhar_) 17.05.2019		(end)
}

void CElectricBall::UpdateCLChild	()
{
	// Lex Addon (correct by Suhar_) 17.05.2019		(begin)
	// Добавлено чтение параметра jump_height для этого класса артефактов
	VERIFY(!physics_world()->Processing());
	//if (getVisible() && m_pPhysicsShell)
	if (m_pPhysicsShell)
	{
		if (m_fJumpHeight)
		{
			Fvector dir; 
			dir.set(0, -1.f, 0);
			collide::rq_result RQ;
			// Проверить высоту артефакта
			if(Level().ObjectSpace.RayPick(Position(), dir, m_fJumpHeight, collide::rqtBoth, RQ, this)) 
			{
				dir.y = 1.f; 
				m_pPhysicsShell->applyImpulse(dir, 30.f * Device.fTimeDelta *  m_pPhysicsShell->getMass());
			}
		}
	}
	// Lex Addon (correct by Suhar_) 17.05.2019		(end)
	inherited::UpdateCLChild();

	if(H_Parent()) XFORM().set(H_Parent()->XFORM());
};