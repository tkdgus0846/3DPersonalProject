#include "stdafx.h"
#include "..\Public\Task_Attack1.h"
#include "BlackBoard.h"
#include "GameObject.h"
#include "Paladin.h"
#include "Transform.h"
#include "Navigation.h"

CTask_Attack1::RESULT CTask_Attack1::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	m_pBlackBoard->ChangeData("IsAttack1", false);

	if (Pass_Decorator() == false)
		return RESULT(FAIL);

	
	m_pBlackBoard->ChangeData("IsAttack1", true);
	
	RESULT result = RESULT(RUNNING);
	result.LastRunningList.push_back(this);
	return result;
}

CTask_Attack1* CTask_Attack1::Create()
{
	return new CTask_Attack1();
}

void CTask_Attack1::Free()
{
	__super::Free();
}
