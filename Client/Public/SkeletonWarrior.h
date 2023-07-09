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

class CSkeletonWarrior final : public CMonster
{
protected:
	CSkeletonWarrior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkeletonWarrior(const CSkeletonWarrior& rhs);
	virtual ~CSkeletonWarrior() = default;

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

	void			Appear(const _float& TimeDelta);


	// ������ �������� ��ü�� �Ű��ִ� �۾��� �Ѵ�.
	virtual void State() override;

	void			ComboAttack(const _float& TimeDelta);	

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CModel*					m_pModelCom = { nullptr };

	CCollider*				m_pBodyColliderCom = { nullptr };
	CCollider*				m_pDetectionColliderCom = { nullptr };
	CCollider*				m_pAttackRangeColliderCom = { nullptr };

	CAnimInstance*			m_pAnimInstance = { nullptr };

	_bool					m_bAppeard = { false };
	_bool					m_bAppeardFinished = { false };


private:
	virtual HRESULT Add_Animations() override;
	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	/* ������ �����Ѵ�. */
	static CSkeletonWarrior* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END