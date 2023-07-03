#include "stdafx.h"
#include "..\Public\Task_FollowTarget.h"
#include "BlackBoard.h"
#include "GameObject.h"
#include "Paladin.h"
#include "Transform.h"
#include "Navigation.h"

CTask_FollowTarget::RESULT CTask_FollowTarget::Run(const _double& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	if (Pass_Decorator() == false)
		return RESULT(FAIL);

	CGameObject* target = any_cast<CGameObject*>(m_pBlackBoard->FindData("Target"));

	if (target == nullptr)
		return RESULT(FAIL);

	_bool bInAttackRange = any_cast<_bool>(m_pBlackBoard->FindData("InAttackRange"));
	if (bInAttackRange == true)
		return RESULT(FAIL);

	CGameObject* owner = any_cast<CGameObject*>(m_pBlackBoard->FindData("Owner"));

	CTransform* targetTransform = (CTransform*)target->Get_Component(TRANSFORM_W);
	CTransform* ownerTransform = (CTransform*)owner->Get_Component(TRANSFORM_W);

	_vector targetPosition = targetTransform->Get_State(CTransform::STATE_POSITION);
	_vector targetDir = targetPosition - ownerTransform->Get_State(CTransform::STATE_POSITION);

	ownerTransform->LookAt(targetPosition, CTransform::LANDOBJECT);
	ownerTransform->Go_Dir(targetDir, TimeDelta, (CNavigation*)owner->Get_Component(NAVIGATION_W));
	
	RESULT result = RESULT(RUNNING);
	result.LastRunningList.push_back(this);
	return result;
}

CTask_FollowTarget* CTask_FollowTarget::Create()
{
	return new CTask_FollowTarget();
}

void CTask_FollowTarget::Free()
{
}
