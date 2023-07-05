#include "stdafx.h"
#include "..\Public\Task_LookAt.h"
#include "BlackBoard.h"
#include "GameObject.h"
#include "Paladin.h"
#include "Transform.h"

CTask_LookAt::RESULT CTask_LookAt::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	if (Pass_Decorator() == false)
		return RESULT(FAIL);

	//cout << "Turn ÁøÀÔ!!" << endl;
	
	m_TimeAcc += TimeDelta;

	if (m_TurnTime <= m_TimeAcc)
	{
		m_TimeAcc = 0.0f;
		return RESULT(SUCCESS);
	}
	else
	{
		CComponent* pComp = any_cast<CGameObject*>(m_pBlackBoard->FindData("Owner"))->Get_Component(TRANSFORM_W);
		CTransform* pTransform = (CTransform*)pComp;

		pTransform->Turn({ 0.f,1.f,0.f }, TimeDelta);

		RESULT result = RESULT(RUNNING);
		result.LastRunningList.push_back(this);
		return result;
	}
}

CTask_LookAt* CTask_LookAt::Create()
{
	return new CTask_LookAt;
}

void CTask_LookAt::Free()
{
	__super::Free();
}
