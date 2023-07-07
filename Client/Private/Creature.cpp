#include "stdafx.h"
#include "..\Public\Creature.h"
#include "GameInstance.h"

CCreature::CCreature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCreature::CCreature(const CCreature& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCreature::Initialize(void* pArg)
{
	HRESULT hr = __super::Initialize(pArg);

	if (FAILED(Add_Animations()))
		return E_FAIL;

	return hr;
}

_float CCreature::Compute_NavMesh_Height()
{
	_vector myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vector<Triangle>* vec = m_pNavigationCom->Get_TriangleList();

	// 평면의 방정식으로 점과 노말벡터를 넣으면 A B C D 를 뽑아줌
	_vector equationVec = XMPlaneFromPoints(XMLoadFloat3(&(*vec)[m_CurIndex].vVertex1), XMLoadFloat3(&(*vec)[m_CurIndex].vVertex2), XMLoadFloat3(&(*vec)[m_CurIndex].vVertex3));

	_float x = myPos.m128_f32[0];
	_float y = myPos.m128_f32[1];
	_float z = myPos.m128_f32[2];

	// 평면 방정식의 계수
	_float A = equationVec.m128_f32[0];
	_float B = equationVec.m128_f32[1];
	_float C = equationVec.m128_f32[2];
	_float D = equationVec.m128_f32[3];

	// 평면 위로의 이동량 계산
	_float offsetY = (-A * x - C * z - D) / B;

	return offsetY;
}

void CCreature::ClimbNavMesh()
{
	if (m_CurIndex == -1 || m_bClimbNavMesh == false) return;

	_vector myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	myPos.m128_f32[1] = Compute_NavMesh_Height();

	m_pTransformCom->Set_Position(myPos);
}

void CCreature::Navigation_CurIndex()
{
	m_CurIndex = m_pNavigationCom->Get_CurrentIndex();
}
