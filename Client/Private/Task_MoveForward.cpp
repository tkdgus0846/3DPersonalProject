#include "stdafx.h"
#include "..\Public\Task_MoveForward.h"
#include "BlackBoard.h"
#include "GameObject.h"
#include "Paladin.h"
#include "Transform.h"

CTask_MoveForward::RESULT CTask_MoveForward::Run(const _double& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	if (Pass_Decorator() == false)
		return RESULT(FAIL);

	CComponent* pComp = any_cast<CGameObject*>(m_pBlackBoard->FindData("Owner"))->Get_Component(TRANSFORM_W);
	CTransform* pTransform = (CTransform*)pComp;

	pTransform->Turn({ 0.f,1.f,0.f }, TimeDelta);
	
	return RESULT(SUCCESS);
}

CTask_MoveForward* CTask_MoveForward::Create()
{
	return new CTask_MoveForward;
}

void CTask_MoveForward::Free()
{
}
