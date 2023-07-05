#include "stdafx.h"
#include "..\Public\Task_Wait.h"
#include "BlackBoard.h"
#include "GameObject.h"
#include "Paladin.h"
#include "Transform.h"

CTask_Wait::RESULT CTask_Wait::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	if (Pass_Decorator() == false)
		return RESULT(FAIL);
	
	m_TimeAcc += TimeDelta;

	if (m_WaitTime <= m_TimeAcc)
	{
		m_TimeAcc = 0.0f;
		return RESULT(SUCCESS);
	}
	
	RESULT result = RESULT(RUNNING);
	result.LastRunningList.push_back(this);
	return result;
}

CTask_Wait* CTask_Wait::Create()
{
	return new CTask_Wait();
}

void CTask_Wait::Free()
{
	__super::Free();
}
