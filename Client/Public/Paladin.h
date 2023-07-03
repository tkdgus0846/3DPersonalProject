#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CAnimInstance;
class CNavigation;
END

BEGIN(Client)

class CPaladin final : public CMonster
{
protected:
	CPaladin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPaladin(const CPaladin& rhs);
	virtual ~CPaladin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	//void Ride() override;

	virtual void OnCollisionEnter(const Collision* collision);
	virtual void OnCollisionStay(const Collision* collision); 
	virtual void OnCollisionExit(const Collision* collision); 

private:
	void Add_Animations();

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };

	CCollider*				m_pColliderCom = { nullptr };
	CCollider*				m_pDetectionColliderCom = { nullptr };
	CCollider*				m_pAttackRangeColliderCom = { nullptr };
	CAnimInstance*			m_pAnimInstance = { nullptr };
	CNavigation*			m_pNavigationCom = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	/* 원형을 생성한다. */
	static CPaladin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본(실제 사용할 객체)을 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END