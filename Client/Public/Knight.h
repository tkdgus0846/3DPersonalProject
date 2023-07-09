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

class CKnight final : public CMonster
{
protected:
	CKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKnight(const CKnight& rhs);
	virtual ~CKnight() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_BehaviorTree() override;


	virtual void Tick(_float TimeDelta) override;
	virtual _int Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	//void Ride() override;

	virtual void OnCollisionEnter(const Collision* collision);
	virtual void OnCollisionStay(const Collision* collision); 
	virtual void OnCollisionExit(const Collision* collision); 

	

private:
	virtual void	Attack(const _float& TimeDelta) override;
	virtual void	Damaged(const _float& TimeDelta) override;
	virtual void	SuperArmor(const _float& TimeDelta) override;
	virtual void	Walk(const _float& TimeDelta) override;
	virtual void	Run(const _float& TimeDelta) override; 
	virtual void	KnockBack(const _float& TimeDelta) override; 
	virtual void	Stun(const _float& TimeDelta) override; 
	virtual void	Idle(const _float& TimeDelta) override; 
	virtual void	Death(const _float& TimeDelta) override;


	// ������ �������� ��ü�� �Ű��ִ� �۾��� �Ѵ�.
	virtual void State() override;

	void			ComboAttack(const _float& TimeDelta);
	void			DashAttack(const _float& TimeDelta);

	

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CModel*					m_pModelCom = { nullptr };

	CCollider*				m_pBodyColliderCom = { nullptr };
	CCollider*				m_pDetectionColliderCom = { nullptr };
	CCollider*				m_pAttackRangeColliderCom = { nullptr };

	CAnimInstance*			m_pAnimInstance = { nullptr };

	// �ִϸ��̼��� NextNext �����ؾ��ϴµ� ��� Apply �ʱ�ȭ �Ǿ �������
	

	// �뽬 ���� ������
	_float						m_MaceDashTimeAcc = { 0.0f };
	const _float				m_MaceDashAccel = { 1.11f };
	const _float				m_MaceDashInitSpeed = { -0.1f };
	_bool						m_bMaceDashFinished = { false };

	_float3						m_MaceDashTargetPos = { -1.f, -1.f, -1.f };
	_float3						m_MaceDashDir = { -1.f, -1.f, -1.f };

private:
	virtual HRESULT Add_Animations() override;
	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	/* ������ �����Ѵ�. */
	static CKnight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END