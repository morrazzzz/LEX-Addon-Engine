///////////////////////////////////////////////////////////////
// ElectricBall.h
// ElectricBall - �������� ������������� ���
///////////////////////////////////////////////////////////////

#pragma once
#include "artefact.h"

class CElectricBall : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CElectricBall(void);
	virtual ~CElectricBall(void);

	virtual void Load				(LPCSTR section);

protected:
	virtual void	UpdateCLChild	();
	// Lex Addon (correct by Suhar_) 17.05.2019		(begin)
	// ��������� ������ ��������� jump_height ��� ����� ������ ����������
	float m_fJumpHeight;
	// Lex Addon (correct by Suhar_) 17.05.2019		(end)
};