#include "..\Public\AnimInstance.h"
#include "GameInstance.h"

CAnimInstance::CAnimInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CComponent(pDevice, pContext)

{
}

CAnimInstance::CAnimInstance(const CAnimInstance& rhs)
	: CComponent(rhs)
{
}

HRESULT CAnimInstance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimInstance::Initialize(void* pArg)
{
	m_pModel = (CModel*)pArg;
	Safe_AddRef(m_pModel);
	return S_OK;
}

void CAnimInstance::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	

}

void CAnimInstance::Push_Animation(const string& name, const AnimNode& animNode)
{
	m_AnimationNode[name] = animNode;
}

void CAnimInstance::Proceed_Animation()
{
}

void CAnimInstance::Apply_Animation(const string& name)
{
	m_pCurAnimationName = name;
}

_bool CAnimInstance::Animation_Finished()
{
	return _bool();
}

CAnimInstance* CAnimInstance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimInstance* pInstance = new CAnimInstance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimInstance* CAnimInstance::Clone(void* pArg)
{
	CAnimInstance* pInstance = new CAnimInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimInstance::Free()
{
	Safe_Release(m_pModel);
	__super::Free();
}
