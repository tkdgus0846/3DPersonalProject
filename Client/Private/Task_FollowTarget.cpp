#include "stdafx.h"
#include "..\Public\Task_FollowTarget.h"
#include "BlackBoard.h"
#include "GameObject.h"
#include "Paladin.h"
#include "Transform.h"
#include "Navigation.h"

CTask_FollowTarget::RESULT CTask_FollowTarget::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	m_pBlackBoard->ChangeData("IsRun", false);

	if (Pass_Decorator() == false)
	{
		//cout << "Follow_Task Failed" << endl;
		return RESULT(FAIL);
	}
		
	//m_pBlackBoard->ClearData();
	//cout << "Follow_Task Running" << endl;

	CGameObject* target = any_cast<CGameObject*>(m_pBlackBoard->FindData("Target"));

	/*if (target == nullptr)
		return RESULT(FAIL);

	_bool bInAttackRange = any_cast<_bool>(m_pBlackBoard->FindData("InAttackRange"));
	if (bInAttackRange == true)
		return RESULT(FAIL);*/

	CGameObject* owner = any_cast<CGameObject*>(m_pBlackBoard->FindData("Owner"));

	CTransform* targetTransform = (CTransform*)target->Get_Component(TRANSFORM_W);
	CTransform* ownerTransform = (CTransform*)owner->Get_Component(TRANSFORM_W);

	_vector targetPosition = targetTransform->Get_State(CTransform::STATE_POSITION);
	_vector targetDir = targetPosition - ownerTransform->Get_State(CTransform::STATE_POSITION);

	ownerTransform->LookAt(targetPosition, CTransform::LANDOBJECT);
	ownerTransform->Go_Dir(targetDir, TimeDelta, (CNavigation*)owner->Get_Component(NAVIGATION_W));
	
	m_pBlackBoard->ChangeData("IsRun", true);
	
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
	__super::Free();
}
