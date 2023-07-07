#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite, public ISerializable
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

	

	virtual CGameObject* Clone(void* pArg) override PURE;

	virtual void OnCollisionEnter(const Collision * collision) {}
	virtual void OnCollisionStay(const Collision * collision) {}
	virtual void OnCollisionExit(const Collision * collision) {}

	virtual void SetDead(_bool bDead = true) { m_bDead = bDead; }
	virtual _bool GetDead() { return m_bDead; }

	void	Change_RenderGroup(_uint eRenderGroup) { m_eRenderGroup = eRenderGroup; }
	void	Change_PassNum(_uint iPassNum) { m_iPassNum = iPassNum; }

protected:
	virtual HRESULT Add_Components() { return S_OK; }
	virtual HRESULT SetUp_ShaderResources() { return S_OK; }

protected:
	_uint		m_iPassNum = { 0 };
	_uint		m_eRenderGroup;
	_bool		m_bDead = { false };
};

END