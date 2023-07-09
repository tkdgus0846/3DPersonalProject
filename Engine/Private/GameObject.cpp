#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Renderer.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComposite(pDevice, pContext)
{

}

CGameObject::CGameObject(const CGameObject & rhs)
	: CComposite(rhs)
	, m_iPassNum(rhs.m_iPassNum)
	, m_eRenderGroup(rhs.m_eRenderGroup)
{
	
}

HRESULT CGameObject::Initialize_Prototype()
{
	HRESULT result = __super::Initialize_Prototype();

	return result;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	HRESULT result = __super::Initialize(pArg);

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	if (pArg != nullptr)
	{
		ObjectParsingData* objectParsingData = (ObjectParsingData*)pArg;

		TransformParsingData* transformParsingData = (TransformParsingData*)&objectParsingData->TransformData;

		CTransform* pTransform = (CTransform*)Get_Component(TRANSFORM_W);

		if (pTransform != nullptr)
		{
			_float4x4 WorldMatrix = transformParsingData->WorldMatrix;
			//XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			/*memcpy(&WorldMatrix.m[0], &transformParsingData->WorldMatrix.m[0], sizeof _float4);
			memcpy(&WorldMatrix.m[1], &transformParsingData->WorldMatrix.m[1], sizeof _float4);
			memcpy(&WorldMatrix.m[2], &transformParsingData->WorldMatrix.m[2], sizeof _float4);
			memcpy(&WorldMatrix.m[3], &transformParsingData->WorldMatrix.m[3], sizeof _float4);*/
			//WorldMatrix.m[0] = transformParsingData->WorldMatrix.m[0];
			pTransform->Set_WorldFloat4x4(WorldMatrix);
		}
	}
	

	m_iPassNum = 0;
	m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	return result;
}

HRESULT CGameObject::Render()
{
	HRESULT hr = __super::Render();

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return hr;
}
