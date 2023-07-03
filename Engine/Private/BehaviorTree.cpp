#include "..\Public\BehaviorTree.h"
#include "Behavior.h"
#include "RootNode.h"
#include "BlackBoard.h"

void CBehaviorTree::ChangeData(const string& name, any Data)
{
	if (m_pBlackBoard != nullptr)
		m_pBlackBoard->ChangeData(name, Data);
}

CBehaviorTree::CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& rhs)
	: CComponent(rhs)
{
}

HRESULT CBehaviorTree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBehaviorTree::Initialize(void* pArg)
{
	BTDesc* desc = (BTDesc*)pArg;

	m_pBlackBoard = desc->pBlackBoard;
	m_pRootNode = desc->pRootNode;

	m_pRootNode->Set_BlackBoard(m_pBlackBoard);
	
	return S_OK;
}

void CBehaviorTree::Tick(_double TimeDelta)
{
	m_pRootNode->Run(TimeDelta);
}

CComponent* CBehaviorTree::Clone(void* pArg)
{
	CBehaviorTree* pInstance = new CBehaviorTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBehaviorTree* CBehaviorTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBehaviorTree* pInstance = new CBehaviorTree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBehaviorTree::Free()
{
	Safe_Release(m_pBlackBoard);
	Safe_Release(m_pRootNode);

	__super::Free();
	
}
