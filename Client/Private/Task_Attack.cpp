#include "stdafx.h"
#include "..\Public\Task_Attack.h"
#include "BlackBoard.h"
#include "GameObject.h"
#include "Paladin.h"
#include "Transform.h"
#include "Navigation.h"

CTask_Attack::CTask_Attack(const string& varName) :
	m_VarName(varName)
{
}

CTask_Attack::RESULT CTask_Attack::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	
	m_pBlackBoard->ChangeData(m_VarName, false);

	if (Pass_Decorator() == false)
	{
		//cout << "Attack_Task" << " " << m_VarName << " Failed" << endl;
		return RESULT(FAIL);
	}

	//cout << "Attack_Task" << " " << m_VarName <<" Running" << endl;

	/*if (m_VarName.compare("IsAttack4") == 0)
		cout << "Attack_Task" << " " << m_VarName << " Running" << endl;*/
	
	m_pBlackBoard->ChangeData(m_VarName, true);
	
	RESULT result = RESULT(RUNNING);
	result.LastRunningList.push_back(this);
	return result;
}

CTask_Attack* CTask_Attack::Create(const string& varName)
{
	return new CTask_Attack(varName);
}

void CTask_Attack::Free()
{
	__super::Free();
}
