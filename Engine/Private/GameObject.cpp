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

	m_iPassNum = 0;
	m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	return result;
}
