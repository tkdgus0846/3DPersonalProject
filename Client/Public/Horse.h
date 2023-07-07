#pragma once

#include "Client_Defines.h"
#include "Creature.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CAnimInstance;
END

BEGIN(Client)

class CHorse final : public CCreature
{
	enum HORSETYPE
	{
		HORSE_MOUNT, HORSE_NATURAL, HORSE_END 
	};
protected:
	CHorse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHorse(const CHorse& rhs);
	virtual ~CHorse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual _int Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	//void Ride() override;

	virtual void OnCollisionEnter(const Collision* collision) { cout << "Horse Enter!" << endl; }
	virtual void OnCollisionStay(const Collision* collision) { cout << "Horse Stay!" << endl; }
	virtual void OnCollisionExit(const Collision* collision) { cout << "Horse Exit!" << endl; }

private:
	virtual HRESULT Add_Animations() override;
	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;
	virtual void	Select_AnimationKey() override {}

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };
	CModel*					m_pModelCom[HORSE_END] = { nullptr, nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CAnimInstance*			m_pAnimInstance[HORSE_END] = { nullptr, nullptr };

	HORSETYPE				m_eCurHorseType = { HORSE_NATURAL };



public:
	/* 원형을 생성한다. */
	static CHorse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본(실제 사용할 객체)을 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END