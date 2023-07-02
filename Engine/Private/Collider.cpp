#include "..\Public\Collider.h"
#include "PipeLine.h"
#include "GameObject.h"

#include "CollisionManager.h"
#include "GameInstance.h"

#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)		
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
	, m_eColliderType(rhs.m_eColliderType)
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;

	switch (eColliderType)
	{
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext);
		break;
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext);
		break;	
	}

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderByteCode;
	size_t		iShaderByteCodeLength;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;		
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(CBounding* pBounding, void * pArg)
{
	/* 원형객체를 복사하여 생성한다. */

	m_eColGroup = ((CBounding::BOUNDINGDESC*)pArg)->eColGroup;
	
	m_pBounding = pBounding->Clone(pArg);
	if (nullptr == m_pBounding)
		return E_FAIL;	
	
	return S_OK;
}

void CCollider::OnCollisionEnter(const Collision* collision)
{
	if (m_pOwnerObject != nullptr)
		m_pOwnerObject->OnCollisionEnter(collision);
}

void CCollider::OnCollisionStay(const Collision* collision)
{
	if (m_pOwnerObject != nullptr)
		m_pOwnerObject->OnCollisionStay(collision);
}

void CCollider::OnCollisionExit(const Collision* collision)
{
	if (m_pOwnerObject != nullptr)
		m_pOwnerObject->OnCollisionExit(collision);
}

void CCollider::Tick(_double TimeDelta)
{
	if (m_bEnabled == false)
	{
		m_CollisionList.clear();
		return;
	}
	if (m_pBounding == nullptr) return;

	__super::Tick(TimeDelta);
	CGameInstance::GetInstance()->Add_ColGroup(m_eColGroup, this);

	if (m_pOwnerObject == nullptr)
	{
		m_pOwnerObject = ((CGameObject*)m_pOwner);
		m_pOwnerTransform = (CTransform*)m_pOwner->Get_Component(TRANSFORM_W);
	}

	if (m_pOwnerTransform != nullptr)
		m_pBounding->Tick(m_pOwnerTransform->Get_WorldMatrix());


	/// 지금은 콜라이더가 사용중이 아니면 그냥 빼버리는데 나중에 게임의 성격에 따라 Exit 를 불러줘도 된다.
	for (auto it = m_CollisionList.begin(); it != m_CollisionList.end();)
	{
		if (it->first->Get_Enable() == false)
			it = m_CollisionList.erase(it);
		else
			it++;
	}
}

void CCollider::Late_Tick(_double TimeDelta)
{
	if (m_bEnabled == false) return;

	__super::Late_Tick(TimeDelta);

}

void CCollider::Erase_FromOtherCollisionList()
{
	for (auto& item : m_CollisionList)
	{
		item.first->m_CollisionList.erase(this);
	}
	m_CollisionList.clear();
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	if (m_bEnabled == false) return S_OK;

	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	

	return S_OK;
}
#endif

_bool CCollider::Intersect(const CCollider * pCollider)
{
	return m_pBounding->Intersect(pCollider->m_eColliderType, pCollider->m_pBounding);
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eColliderType)
{

	CCollider*	pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*	pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(m_pBounding, pArg)))
	{
		MSG_BOX("Failed to Cloned CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif

	Safe_Release(m_pBounding);

	
}
